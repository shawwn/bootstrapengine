//----------------------------------------------------------
// File:		GrPolygonGroupUberUVPatch.cpp
// Author:		Kevin Bray
// Created:		12-05-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonUberPatch.h"

// project includes.
#include "GrPolygonGroup.h"
#include "GrConfig.h"
#include "MSVD.h"

//**********************************************************
// class GrPolygonUberPatch
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonUberPatch::GrPolygonUberPatch( GrPolygonGroup* polygons, unsigned int* polyIndices, unsigned int polyCount,
										const MPlane& worldPlane, const MMat4x4& uvTileMatrix, const SVec2& uvPos,
										const SVec2& uvSize )
: _polygons( polygons )
, _polyIndices( polyIndices )
, _polyCount( polyCount )
, _surfacePlane( worldPlane )
, _uvTileMatrix( uvTileMatrix )
, _uvSize( uvSize )
, _uberTexIndex( 0 )
, _tileX( 0 )
, _tileY( 0 )
, _tileWidth( ( unsigned int )( int )Ceil( uvSize.x * GR_UBERTEX_SIZE_IN_TILES ) + 1 )
, _tileHeight( ( unsigned int )( int )Ceil( uvSize.y * GR_UBERTEX_SIZE_IN_TILES ) + 1 )
{
	// copy the polygon indices passed in.
	_polyIndices = new unsigned int[ polyCount ];
	MemCopy( _polyIndices, polyIndices, polyCount * sizeof( unsigned int ) );

	// set the translation in the UV matrix.
	_uvTileMatrix( 0, 3 ) = -Floor( uvPos.x * GR_UBERTEX_SIZE_IN_TILES );
	_uvTileMatrix( 1, 3 ) = -Floor( uvPos.y * GR_UBERTEX_SIZE_IN_TILES );

	// adjust the polygon uvs so they're relative to the center of a
	// tile.
	const float kHalfTexel = 0.5f / GR_UBERTEX_SIZE_IN_TEXELS;
	TranslateUVs( -uvPos + SVec2( kHalfTexel, kHalfTexel ) );

	// now calculate the plane data for the patch.
	CalcSurfaceInfo();
}

//----------------------------------------------------------
GrPolygonUberPatch::~GrPolygonUberPatch()
{
	delete[] _polyIndices;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonUberPatch::AssignToUberTexture( unsigned int uberTexIndex, unsigned int tileX, unsigned int tileY )
{
	// calculate the translate to the new location.
	float uvTileDeltaX = ( float )( ( int )tileX - ( int )_tileX );
	float uvTileDeltaY = ( float )( ( int )tileY - ( int )_tileY );

	// store the new ubertexture number.
	_uberTexIndex = uberTexIndex;

	// store the new tile X and Y.
	_tileX = tileX;
	_tileY = tileY;

	// adjust the UV matrix.
	_uvTileMatrix( 0, 3 ) += uvTileDeltaX;
	_uvTileMatrix( 1, 3 ) += uvTileDeltaY;

	// translate the UVs to the new location.
	float invSizeInTiles = 1.0f / GR_UBERTEX_SIZE_IN_TILES;
	float uvDeltaX = invSizeInTiles * uvTileDeltaX;
	float uvDeltaY = invSizeInTiles * uvTileDeltaY;
	TranslateUVs( SVec2( uvDeltaX, uvDeltaY ) );

	// verify the UVs.
#if 1
	for ( unsigned int i = 0; i < _polyCount; ++i )
	{
		// get the current polygon.
		const GrPolygon& curPoly = _polygons->GetPolygon( _polyIndices[ i ] );

		// verify that the polygon's uvs are valid.
		unsigned int vertexCount = curPoly.GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
		{
			B_ASSERT( curPoly.GetVertex( j ).uv.x < 1.0f );
			B_ASSERT( curPoly.GetVertex( j ).uv.y < 1.0f );
			B_ASSERT( curPoly.GetVertex( j ).uv.x >= 0.0f );
			B_ASSERT( curPoly.GetVertex( j ).uv.y >= 0.0f );
		}
	}
#endif
}

//----------------------------------------------------------
GrPolygonGroup*
GrPolygonUberPatch::GetPolygonGroup() const
{
	return _polygons;
}

//----------------------------------------------------------
unsigned int
GrPolygonUberPatch::GetPolygonCount() const
{
	return _polyCount;
}

//----------------------------------------------------------
unsigned int
GrPolygonUberPatch::GetPolygonIndex( unsigned int i ) const
{
	B_ASSERT( i < GetPolygonCount() );
	return _polyIndices[ i ];
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonUberPatch::TranslateUVs( const SVec2& translate )
{
	// iterate through each polygon and adjust the UVs by the
	// position provided.
	for ( unsigned int i = 0; i < _polyCount; ++i )
	{
		// get the current polygon.
		const GrPolygon& curPoly = _polygons->GetPolygon( _polyIndices[ i ] );

		// adjust the polygon's uvs.
		unsigned int vertexCount = curPoly.GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
			_polygons->SetPolygonUV( _polyIndices[ i ], j, curPoly.GetVertex( j ).uv + translate );
	}
}

//----------------------------------------------------------
void
GrPolygonUberPatch::CalcSurfaceInfo()
{
	// calculate the number of vertices in the patch.
	unsigned int patchVertCount = 0;
	for ( unsigned int i = 0; i < _polyCount; ++i )
		patchVertCount += _polygons->GetPolygon( _polyIndices[ i ] ).GetVertexCount();

	// first, calculate the centroid.
	MVec3 centroid;
	MVec3 minPoint(  FLT_MAX,  FLT_MAX,  FLT_MAX );
	MVec3 maxPoint( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	for ( unsigned int i = 0; i < _polyCount; ++i )
	{
		const GrPolygon& curPoly = _polygons->GetPolygon( _polyIndices[ i ] );
		unsigned int vertCount = curPoly.GetVertexCount();
		for ( unsigned int j = 0; j < vertCount; ++j )
		{
			MVec3 position = curPoly.GetVertex( j ).position;
			centroid += position;
			minPoint.AssignMin( position );
			maxPoint.AssignMax( position );
		}
	}

	// set the AA box.
	_aaBox.SetCenter( 0.5f * ( maxPoint + minPoint ) );
	_aaBox.SetHalfExts( 0.5f * ( maxPoint - minPoint ) );
}
