//----------------------------------------------------------
// File:        GrPolygonUberPatchGroup.cpp
// Author:      Kevin Bray
// Created:     12-05-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonUberPatchGroup.h"

// project includes.
#include "GrPolygonUberPatch.h"
#include "GrPolygonGroup.h"
#include "GrPolygon.h"
#include "GrPolygonEdgeList.h"
#include "GrConfig.h"
#include "ULRUHash.h"
#include "MSVD.h"

// states for polygons when generating patches.
#define POLY_STATE_VISITED          255

// Maximum size of each patch in UV space.  Note that we subtract 64
// from the size to allow for border space.
#define MAX_PATCH_SIZE              256

// calculate UVs.
const float kMatTable[ 6 ][ 16 ] =
{
    {
         0.0f, 0.0f,-1.0f, 0.0f,        // -X
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    },
    {
         0.0f, 0.0f, 1.0f, 0.0f,        // +X
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f,
    },
    {
         1.0f, 0.0f, 0.0f, 0.0f,        // -Y
         0.0f, 0.0f,-1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    },
    {
         1.0f, 0.0f, 0.0f, 0.0f,        // +Y
         0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f,
    },
    {
         1.0f, 0.0f, 0.0f, 0.0f,        // -Z
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f,
    },
    {
        -1.0f, 0.0f, 0.0f, 0.0f,        // +Z
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f,
    },
};

const float kPlaneNormalTable[ 6 ][ 3 ] =
{
    { -1.0f,  0.0f,  0.0f },            // -X
    {  1.0f,  0.0f,  0.0f },            // +X
    {  0.0f, -1.0f,  0.0f },            // -Y
    {  0.0f,  1.0f,  0.0f },            // +Y
    {  0.0f,  0.0f, -1.0f },            // -Z
    {  0.0f,  0.0f,  1.0f },            // +Z
};

//**********************************************************
// class GrPolygonUberPatchGroup
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonUberPatchGroup::GrPolygonUberPatchGroup( GrPolygonGroup* group, float texelsPerMeter )
: _texelsPerMeter( texelsPerMeter )
{
    // iterate through all polygons and classify them based on their
    // face normal.
    BuildPatches( group, texelsPerMeter );
}

//----------------------------------------------------------
GrPolygonUberPatchGroup::~GrPolygonUberPatchGroup()
{
    // free the patches.
    unsigned int patchCount = _patches.GetElemCount();
    for ( unsigned int i = 0; i < patchCount; ++i )
        delete _patches[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrPolygonUberPatch*
GrPolygonUberPatchGroup::GetPatch( unsigned int idx ) const
{
    B_ASSERT( idx < _patches.GetElemCount() );
    return _patches[ idx ];
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonUberPatchGroup::BuildPatches( GrPolygonGroup* group, float texelsPerMeter )
{
    // build a polygon edge list.
    GrPolygonEdgeList edgeList( group );

    // get the total polygon count.
    unsigned int polyCount = group->GetPolygonCount();

    // allocate memory to track what polygons we've already added to
    // patches.
    unsigned int* patchMem = new unsigned int[ polyCount ];
    unsigned char* polyState = new unsigned char[ polyCount ];
    MemSet( polyState, 0, polyCount * sizeof( unsigned char ) );

    // calculate the uv scale.
    float uvScale = texelsPerMeter / GR_UBERTEX_SIZE_IN_TEXELS;

    // classify polygons and generate UVs.
    for ( unsigned int i = 0; i < polyCount; ++i )
    {
        // classify the current polygon and generate UVs for it.
        EAXIS curAxis = CalcDomAxis( group->GetPolygon( i ) );
        polyState[ i ] = curAxis;

        // generate UVs for the current polygon.
        CalcUVs( group, i, curAxis, uvScale );
    }

    // now iterate through contiguous groups of polygons and add them
    // to patches based on their face normal.
    const float kUVToTileScale = GR_UBERTEX_SIZE_IN_TILES;
    for ( unsigned int i = 0; i < polyCount; ++i )
    {
        // skip the current polygon if it's already been visited.
        if ( polyState[ i ] == POLY_STATE_VISITED )
            continue;

        // get the starting polygon.
        const GrPolygon& startPoly = group->GetPolygon( i );

        // calculate the UV bounds for the current polygon.
        SVec2 patchMinUV(  FLT_MAX,  FLT_MAX );
        SVec2 patchMaxUV( -FLT_MAX, -FLT_MAX );
        unsigned int startPolyVertCount = startPoly.GetVertexCount();
        for ( unsigned int j = 0; j < startPolyVertCount; ++j )
        {
            patchMinUV.Min( startPoly.GetVertex( j ).uv );
            patchMaxUV.Max( startPoly.GetVertex( j ).uv );
        }

        // start a new patch with the current polygon.
        EAXIS curPatchAxis = ( EAXIS )polyState[ i ];
        patchMem[ 0 ] = i;
        polyState[ i ] = POLY_STATE_VISITED;
        unsigned int patchPolyCount = 1;

        // add neighboring polygons to the current patch.
        unsigned int patchPolyIdx = 0;
        do
        {
            // get the current polygon's neighbors and add them.
            unsigned int curPolyGroupIdx = patchMem[ patchPolyIdx ];
            const GrPolygon& patchPoly = group->GetPolygon( curPolyGroupIdx );
            unsigned int edgeCount = patchPoly.GetVertexCount();
            for ( unsigned int edge = 0; edge < edgeCount; ++edge )
            {
                // get the current neighbor.
                unsigned int curNeighborIdx = edgeList.GetNeighbor( curPolyGroupIdx, edge );

                // if there is no neighbor, simply skip the current
                // edge.
                if ( curNeighborIdx == ~0 )
                    continue;

                // if we got here, then we need to check to see if
                // the neighboring polygon should be added to the
                // current patch or not.
                if ( polyState[ curNeighborIdx ] != curPatchAxis )
                    continue;

                // calculate a new AABB in texture space for the
                // current polygon to ensure that it won't make the
                // current group too big in texture space.
                const GrPolygon& curNeighbor = group->GetPolygon( curNeighborIdx );
                SVec2 neighborMinUV = curNeighbor.GetVertex( 0 ).uv;
                SVec2 neighborMaxUV = neighborMinUV;
                unsigned int neighborVertCount = curNeighbor.GetVertexCount();
                for ( unsigned int v = 1; v < neighborVertCount; ++v )
                {
                    const SVec2& curUV = curNeighbor.GetVertex( v ).uv;
                    neighborMinUV.Min( curUV );
                    neighborMaxUV.Max( curUV );
                }

                // calculate the new UV.
                SVec2 newMinUV = patchMinUV;
                SVec2 newMaxUV = patchMaxUV;
                newMinUV.Min( neighborMinUV );
                newMaxUV.Max( neighborMaxUV );

                // determine whether or not the new bounding box is
                // an acceptable size.
                float sizeInTilesU = Ceil( kUVToTileScale * newMaxUV.x ) - Floor( kUVToTileScale * newMinUV.x );
                float sizeInTilesV = Ceil( kUVToTileScale * newMaxUV.y ) - Floor( kUVToTileScale * newMinUV.y );
                if ( sizeInTilesU > MAX_PATCH_SIZE || sizeInTilesV > MAX_PATCH_SIZE )
                    continue;

                // update the patch's texture space bounding volume.
                patchMinUV = newMinUV;
                patchMaxUV = newMaxUV;

                // if we got here, then we need to add the current
                // polygon to our patch.
                polyState[ curNeighborIdx ] = POLY_STATE_VISITED;
                patchMem[ patchPolyCount++ ] = curNeighborIdx;
            }

            // get the next polygon in the patch.
            ++patchPolyIdx;
        }
        while( patchPolyIdx < patchPolyCount );

        // make sure the patch's poly-count is greater than zero.
        // There should always be at least one polygon per patch.
        B_ASSERT( patchPolyCount > 0 );

        SVec2 patchSizeUV;
        patchSizeUV.x = Ceil( kUVToTileScale * patchMaxUV.x ) - Floor( kUVToTileScale * patchMinUV.x );
        patchSizeUV.y = Ceil( kUVToTileScale * patchMaxUV.y ) - Floor( kUVToTileScale * patchMinUV.y );

        // now create a patch out of the current polygon patch that
        // was just built.
        BuildUberPatch( group, patchMem, patchPolyCount, patchMinUV, patchSizeUV,
            curPatchAxis, uvScale );
    }

    delete[] polyState;
    delete[] patchMem;
}

//----------------------------------------------------------
void
GrPolygonUberPatchGroup::BuildUberPatch( GrPolygonGroup* group, unsigned int* patchPolygons,
                                         unsigned int patchPolyCount, const SVec2& uvPos,
                                         const SVec2& uvSize, EAXIS axis, float texelsPerMeter )
{
    // make sure there is at least 1 polygon in the patch!
    B_ASSERT( patchPolyCount > 0 );

    // build the uv-matrix.
    float uvTileScale = texelsPerMeter / GR_UBERTEX_TILE_SIZE;
    MMat4x4 uvTileMat( kMatTable[ axis ] );
    for ( unsigned int i = 0; i < 4; ++i )
        for ( unsigned int j = 0; j < 4; ++j )
            uvTileMat( i, j ) *= uvTileScale;

    // build the plane.
    MVec3 normal( kPlaneNormalTable[ axis ] );

    // vector component we need for building the plane.
    unsigned int vecComp = ( unsigned int )axis >> 1;
    unsigned int patchVertCount = 0;
    float dMax = -FLT_MAX;
    float dMin =  FLT_MAX;
    for ( unsigned int i = 0; i < patchPolyCount; ++i )
    {
        // get the current polygon.
        const GrPolygon& curPoly = group->GetPolygon( patchPolygons[ i ] );

        // find the bounds along the axis required.
        unsigned int polyVertCount = curPoly.GetVertexCount();
        for ( unsigned int j = 0; j < polyVertCount; ++j, ++patchVertCount )
        {
            dMax = Max( curPoly.GetVertex( j ).position[ vecComp ], dMax );
            dMin = Min( curPoly.GetVertex( j ).position[ vecComp ], dMin );
        }
    }

    // calculate the center point for the polygons.
    MVec3 center( 0.0f, 0.0f, 0.0f );
    center[ vecComp ] = 0.5f * ( dMax + dMin );

    // build the plane for the patch.
    MPlane worldPlane( normal, center );

    // create a new patch and add it to our list.
    GrPolygonUberPatch* newPatch = new GrPolygonUberPatch( group, patchPolygons, patchPolyCount, worldPlane,
        uvTileMat, uvPos, uvSize );

    // add the patch to our array.
    _patches.Push( newPatch );
}

//----------------------------------------------------------
GrPolygonUberPatchGroup::EAXIS
GrPolygonUberPatchGroup::CalcDomAxis( const GrPolygon& polygon )
{
    // get the polygon's face normal.
    const MVec3& faceNormal = polygon.GetNormal();

    // classify based on the face normal.
    EAXIS domAxis = EA_NZ;
    MVec3 absNormal = faceNormal.Abs();
    if ( absNormal.GetX() > faceNormal.GetY() )
    {
        if ( absNormal.GetX() > faceNormal.GetZ() )
            domAxis = ( Sign( faceNormal.GetX() ) < 0 ) ? EA_NX : EA_PX;
        else
            domAxis = ( Sign( faceNormal.GetZ() ) < 0 ) ? EA_NZ : EA_PZ;
    }
    else
    {
        if ( absNormal.GetY() > faceNormal.GetZ() )
            domAxis = ( Sign( faceNormal.GetY() ) < 0 ) ? EA_NY : EA_PY;
        else
            domAxis = ( Sign( faceNormal.GetZ() ) < 0 ) ? EA_NZ : EA_PZ;
    }

    // return the dominant axis.
    return domAxis;
}

//----------------------------------------------------------
void
GrPolygonUberPatchGroup::CalcUVs( GrPolygonGroup* group, unsigned int polyIdx, EAXIS mapAxis, float uvScale ) const
{
    // uv generation matrix.
    MMat4x4 mapMatrix( kMatTable[ mapAxis ] );

    // calculate the polygon's uvs.
    const GrPolygon& polygon = group->GetPolygon( polyIdx );
    unsigned int vertexCount = polygon.GetVertexCount();
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        MVec3 tempUV = mapMatrix.TransformCoordNoPersp( polygon.GetVertex( i ).position );
        group->SetPolygonUV( polyIdx, i, SVec2( uvScale * tempUV.GetX(), uvScale * tempUV.GetY() ) );
    }
}
