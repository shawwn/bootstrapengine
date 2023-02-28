//----------------------------------------------------------
// File:        GrPolygonBSPArea.h
// Author:      Kevin Bray
// Created:     09-12-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBSPArea.h"

// project includes.
#include "GrPolygonBSPPortalSet.h"
#include "GrPolygonBSPPortal.h"
#include "GrPolygonBSPSplit.h"
#include "GrPolygonUberPatchGroup.h"
#include "GrMeshInst.h"
#include "GrFrustum.h"
#include "UReader.h"
#include "UWriter.h"
#include "MRay.h"

//#define _NO_PVS

//**********************************************************
// class GrPolygonBSPArea
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonBSPArea::GrPolygonBSPArea( const GrPolygonGroup& hullPolygons )
: _polygons( hullPolygons )
, _uberData( 0 )
, _inFlood( false )
{

}

//----------------------------------------------------------
GrPolygonBSPArea::GrPolygonBSPArea( UReader& reader )
: _inFlood( false )
{
    Load( reader );
}

//----------------------------------------------------------
GrPolygonBSPArea::~GrPolygonBSPArea()
{
    Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBSPArea::GetVisibilityData( const std::vector< unsigned int >** pvsIndices ) const
{
    B_ASSERT( pvsIndices != 0 );
    *pvsIndices = &_pvsIndices;
}

//----------------------------------------------------------
const MAABox&
GrPolygonBSPArea::GetBoundingAABox() const
{
    return _polygons.GetAABox();
}

//----------------------------------------------------------
MSphere
GrPolygonBSPArea::GetBoundingSphere() const
{
    return _polygons.GetBoundingSphere();
}

//----------------------------------------------------------
void
GrPolygonBSPArea::GenUberTexData( float texelsPerMeter )
{
    // in the future, we need to have a separate geometry group that
    // contains only ubertextured polygons.
    _uberData = new GrPolygonUberPatchGroup( &_polygons, texelsPerMeter );
}

//----------------------------------------------------------
void
GrPolygonBSPArea::AddPortal( GrPolygonBSPPortalSet* portal )
{
    // set ourselves as the owner of the portal passed in.
    portal->SetOwner( this );

    // ensure that the portal being added is helping to form a convex
    // hull.
#if 0
    const MPlane& portalPlane = portal->GetPlane();
    const unsigned int portalCount = _portals.GetElemCount();
    for ( int i = 0; i < portalCount; ++i )
    {
        GrPolygonBSPPortalSet frontTemp;
        GrPolygonBSPPortalSet backTemp;
        int ret = _portals[ i ]->Split( frontTemp, backTemp, portalPlane, 5.0f );
        if ( ret >= 0 && frontTemp.GetPortalCount() > 0 )
        {
            frontTemp.SetOwner( this );
            *_portals[ i ] = frontTemp;
        }
        else
            _portals.Erase( i-- );
//      B_ASSERT( ret >= 0 );/* ||
//          ( 
//              Abs( portalPlane.GetNormal().Dot( _portals[ i ]->GetPlane().GetNormal() ) ) > 0.95f &&
//              ( Abs( portalPlane.GetD() ) - Abs( _portals[ i ]->GetPlane().GetD() ) ) < 0.1f
//          )
//      );*/
    }
#endif

    // add the new portal only if necessary.
    if ( SubtractAreaFromPortal( portal, this ) )
        _portals.Push( portal );
    else
        delete portal;
}

//----------------------------------------------------------
void
GrPolygonBSPArea::FindNeighbors( GrPolygonBSPSplit* root )
{
    // take all of our portal sets and re-insert them into the BSP tree.
    // for every leaf we hit where geometry is still left, then we have
    // found a neighbor.
    std::vector< GrPolygonBSPPortalSet* > neighborLinks;
    const unsigned int portalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalSetCount; ++i )
    {
        // note that FindPortalSetNeighbors will delete the portal passed
        // in.
        GrPolygonBSPPortalSet* reversed = new GrPolygonBSPPortalSet( *_portals[ i ] );
        reversed->ReverseWinding();
        FindPortalSetNeighbors( neighborLinks, root, reversed );
        delete _portals[ i ];
    }

    // free all current portals and use the neighbor links instead.
    _portals.Clear();
    const unsigned int neighborCount = ( unsigned int )neighborLinks.size();
    for( unsigned int i = 0; i < neighborCount; ++i )
        _portals.Push( neighborLinks[ i ] );

    // calculate the bounding sphere for all portals
//  CalcPortalBounds();
}

//----------------------------------------------------------
void
GrPolygonBSPArea::FixTJuncs()
{
    // portal sets are the links to neighbors.
    unsigned int portalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalSetCount; ++i )
    {
        // get the current neighbor and make sure it's valid.
        GrPolygonBSPArea* curNeighbor = _portals[ i ]->GetNeighbor();
        B_ASSERT( curNeighbor != 0 );

        // fix t-junctions with the current neighbor.
        _polygons.FixTJuncs( curNeighbor->_polygons, GR_BSP_TJUNC_EPSILON );
    }
}

//----------------------------------------------------------
void
GrPolygonBSPArea::CalcPVS( const std::vector< GrPolygonBSPArea* >& leaves )
{
#if !defined _NO_PVS
 #if 0
    // insert ourselves into the visible set.
    std::set< GrPolygonBSPArea* > visibleSet;
    visibleSet.insert( this );

    // check all other leaves for visibility.
    std::vector< GrPolygonBSPArea* >::const_iterator iter = leaves.begin();
    const std::vector< GrPolygonBSPArea* >::const_iterator end = leaves.end();
    for ( ; iter != end; ++iter )
    {
        // no need to check visibility on ourselves.
        if ( *iter == this )
            continue;

        // check visibility to the current leaf.
        if ( CheckVisibility( *iter ) )
            visibleSet.insert( *iter );
    }
 #else
    // a vector of all potentially visible leaves and add ourselves.
    std::set< GrPolygonBSPArea* > visibleSet;
    visibleSet.insert( this );

    // set our flood ID.
    _inFlood = true;

    // now flood all neighbors.
    const unsigned int portalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalSetCount; ++i )
    {
        // get the neighbor.
        GrPolygonBSPArea* neighbor = _portals[ i ]->GetNeighbor();
        B_ASSERT( neighbor != 0 && neighbor != this );

        // we only want to flood into neighbors that have polygons!
        if ( neighbor->GetPolygons().GetPolygonCount() == 0 )
            continue;

        // get the current portal set.
        GrPolygonBSPPortalSet* curPortalSet = _portals[ i ];

        // calculate visibility for each portal in the current set.
        const unsigned int portalCount = curPortalSet->GetPortalCount();
        for ( unsigned int j = 0; j < portalCount; ++j )
        {
            // flood the neighbor from the current portal.
            const GrPortal& curPortal = curPortalSet->GetPortal( j );
            std::vector< MPlane > temp;
            neighbor->FloodPVS( visibleSet, temp, curPortal, curPortalSet->GetCreator() );
        }
    }

    // copy data in the visible set into the visibility array.
    std::set< GrPolygonBSPArea* >::iterator iter = visibleSet.begin();
    const std::set< GrPolygonBSPArea* >::iterator end = visibleSet.end();
    for ( ; iter != end; ++iter )
    {
        _pvsVector.push_back( *iter );
    }

    // increment the flood ID so the next flood can take place.
    _inFlood = false;
 #endif
#else

#endif
}

//----------------------------------------------------------
void
GrPolygonBSPArea::CalcPVSAllVisibleIndices( unsigned int leafCount )
{
    for ( unsigned int i = 0; i < leafCount; ++i )
        _pvsIndices.push_back( i );
}

//----------------------------------------------------------
void
GrPolygonBSPArea::CompressPVSData( const std::vector< GrPolygonBSPArea* >& leaves )
{
#if !defined _NO_PVS
    const unsigned int pvsItemCount = ( unsigned int )_pvsVector.size();
    for ( unsigned int i = 0; i < pvsItemCount; ++i )
    {
        GrPolygonBSPArea* curArea = _pvsVector[ i ];
        const unsigned int leafCount = ( unsigned int )leaves.size();
        for ( unsigned int j = 0; j < leafCount; ++j )
        {
            // once we've found the leaf, record it's index.
            if ( curArea == leaves[ j ] )
            {
                _pvsIndices.push_back( j );
                break;
            }
        }
    }
#else
    const unsigned int leafCount = ( unsigned int )leaves.size();
    for ( unsigned int j = 0; j < leafCount; ++j )
        _pvsIndices.push_back( j ); 
#endif
}

//----------------------------------------------------------
bool
GrPolygonBSPArea::GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist )
{
    // ignore if we don't hit the AABB.
    if ( !ray.Intersect( GetBoundingAABox(), maxDist ) )
        return false;

    // check all polygons for a collision.
    SVec3 rayStart = ray.GetStart();
    SVec3 rayDir = ray.GetDir();
    float bestDist = FLT_MAX;
    GrPolygon::SRayIntersectInfo rayIntersectInfo;

    // now check all polygons for ray intersection.
    unsigned int i = _polygons.GetPolygonCount();
    while ( i-- > 0 )
    {
        // perform the intersection test.
        const GrPolygon& curPolygon = _polygons.GetPolygon( i );
        if ( curPolygon.Intersect( rayIntersectInfo, rayStart, rayDir, maxDist ) )
        {
            if ( rayIntersectInfo.hitDist < bestDist )
            {
                bestDist = rayIntersectInfo.hitDist;
                hitPos = rayIntersectInfo.hitLoc;
            }
        }
    }

    return bestDist < FLT_MAX;
}

//----------------------------------------------------------
void
GrPolygonBSPArea::GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const
{
    if ( _uberData )
        dataArray.push_back( _uberData );
}

//----------------------------------------------------------
void
GrPolygonBSPArea::Load( UReader& reader )
{
    Clean();

    // read in polygons.
    unsigned int polyCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < polyCount; ++i )
        _polygons.AddPolygon( GrPolygon( reader ) );

    // read in PVS data.
    const unsigned int pvsCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < pvsCount; ++i )
        _pvsIndices.push_back( reader.ReadUInt() );
}

//----------------------------------------------------------
void
GrPolygonBSPArea::Save( UWriter& writer )
{
    // write out polygons.
    unsigned int polyCount = _polygons.GetPolygonCount();
    writer.WriteUInt( polyCount );
    for ( unsigned int i = 0; i < polyCount; ++i )
        _polygons.GetPolygon( i ).Save( writer );

    // write out PVS data.
    const unsigned int pvsCount = ( unsigned int )_pvsIndices.size();
    writer.WriteUInt( pvsCount );
    for ( unsigned int i = 0; i < pvsCount; ++i )
        writer.WriteUInt( _pvsIndices[ i ] );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
GrPolygonBSPArea::SubtractAreaFromPortal( GrPolygonBSPPortalSet* portals, GrPolygonBSPArea* area )
{
    // subtract all coplanar polygons.
    unsigned int polygonCount = area->_polygons.GetPolygonCount();
    for ( unsigned int i = 0; i < polygonCount; ++i )
    {
        const GrPolygon& curPolygon = area->_polygons.GetPolygon( i );
        if ( curPolygon.IsCoplanar( portals->GetPlane(), GR_BSP_PORTAL_EPSILON ) )
        {
            // return false if we have no more polygons.
            if ( !portals->Subtract( area->_polygons.GetPolygon( i ), GR_BSP_PORTAL_EPSILON ) )
                return false;
        }
    }

    // still have polygons.
    return true;
}

//----------------------------------------------------------
void
GrPolygonBSPArea::FindPortalSetNeighbors( std::vector< GrPolygonBSPPortalSet* >& neighborLinks, GrPolygonBSPSplit* root, GrPolygonBSPPortalSet* curPortalSet )
{
    GrPolygonBSPPortalSet* frontPortals = new GrPolygonBSPPortalSet;
    GrPolygonBSPPortalSet* backPortals = new GrPolygonBSPPortalSet;

    // get the original owner from the incoming portal set.
    GrPolygonBSPArea* originalOwner = curPortalSet->GetOwner();

    // initialize the new portals to the correct owner.
    frontPortals->SetOwner( originalOwner );
    backPortals->SetOwner( originalOwner );

    // split the current portal set.
    curPortalSet->Split( *frontPortals, *backPortals, root->GetPlane(), GR_BSP_PORTAL_EPSILON );
    delete curPortalSet;
    curPortalSet = 0;

    if ( frontPortals->GetPortalCount() > 0 )
    {
        if ( root->GetFrontChild() )
        {
            FindPortalSetNeighbors( neighborLinks, root->GetFrontChild(), frontPortals );
        }
        else if ( root->GetFrontLeaf() && originalOwner != root->GetFrontLeaf() )
        {
            // subtract all polygons from the neighboring leaf.
            GrPolygonBSPArea* frontLeaf = root->GetFrontLeaf();
            if ( !SubtractAreaFromPortal( frontPortals, frontLeaf ) )
            {
                delete frontPortals;
                return;
            }
            frontPortals->SetNeighbor( frontLeaf );

            // reverse the portals before adding.  This is because the portals were
            // reversed to aid in finding coplanar polygons in neighboring regions.
            frontPortals->ReverseWinding();

            // add the portals.
            neighborLinks.push_back( frontPortals );
        }
    }
    if ( backPortals->GetPortalCount() > 0 )
    {
        if ( root->GetBackChild() )
        {
            FindPortalSetNeighbors( neighborLinks, root->GetBackChild(), backPortals );
        }
        else if ( root->GetBackLeaf() && originalOwner != root->GetBackLeaf() )
        {
            // subtract all polygons from the neighboring leaf.
            GrPolygonBSPArea* backLeaf = root->GetBackLeaf();
            if ( !SubtractAreaFromPortal( backPortals, backLeaf ) )
            {
                delete backPortals;
                return;
            }
            backPortals->SetNeighbor( backLeaf );

            // reverse the portals before adding.  This is because the portals were
            // reversed to aid in finding coplanar polygons in neighboring regions.
            backPortals->ReverseWinding();

            // add the portals.
            neighborLinks.push_back( backPortals );
        }
    }
}

//----------------------------------------------------------
void
GrPolygonBSPArea::FloodPVS( std::set< GrPolygonBSPArea* >& visibleLeaves, std::vector< MPlane >& cullPlanes, const GrPortal& fromPortal,
                     const GrPolygonBSPSplit* portalCreator )
{
    // we shouldn't ever be visited twice!
    B_ASSERT( !_inFlood );

    // no visit yet, so record our visit!
    _inFlood = true;

    // if this function was called for us, then we're visible.
    visibleLeaves.insert( this );

    // iterate through all of our portal sets.
    const unsigned int toPortalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < toPortalSetCount; ++i )
    {
        // we don't want to visit where we came from.
        GrPolygonBSPPortalSet* toPortals = _portals[ i ];
        if ( toPortals->GetNeighbor()->_inFlood )
            continue;

        // if the portals we're looking to are coplanar to the portal we're
        // looking from, skip.
        if ( toPortals->GetCreator() == portalCreator /*|| fromPortal.IsCoplanar( toPortals->GetPlane(), 0.1f * GR_PVS_VISIBILITY_EPSILON )*/ )
            continue;

        // don't need to flood if there are no polygons in the neighbor.
        if ( toPortals->GetNeighbor()->GetPolygons().GetPolygonCount() == 0 )
            continue;

        // go through all portals in the current portal set.
        unsigned int portalCount = toPortals->GetPortalCount();
        for ( unsigned int j = 0; j < portalCount; ++j )
        {
            // get the current portal we're looking at.
            const GrPortal& toPortal = toPortals->GetPortal( j );

            // if the current portal isn't visible, then we don't need to flood it.
            if ( IsPortalVisible( toPortal, cullPlanes ) )
            {
                // since the current portal is visible, we need to generate cull planes
                size_t prevSize = cullPlanes.size();
                toPortals->GenCullPlaneSet( cullPlanes, fromPortal, j, false );

                // now flood the next area using the new cull planes.
                toPortals->GetNeighbor()->FloodPVS( visibleLeaves, cullPlanes, fromPortal, portalCreator );

                // remove added cull planes.
                cullPlanes.erase( cullPlanes.begin() + prevSize, cullPlanes.end() );
            }
        }
    }

    // reset the flood ID.
    _inFlood = false;
}
#if 0
//----------------------------------------------------------
bool
GrPolygonBSPArea::CheckVisibility( GrPolygonBSPArea* area )
{
    // go through all of our portals.
    std::vector< MPlane > cullPlanes;
    unsigned int fromPortalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < fromPortalSetCount; ++i )
    {
        // get the current portal set.
        GrPolygonBSPPortalSet* fromPortalSet = _portals[ i ];
        GrPolygonBSPArea* neighbor = fromPortalSet->GetNeighbor();
        if ( neighbor == area )
            return true;

        // add the current portal set's plane facing in the reverse direction.
        MPlane plane = fromPortalSet->GetPlane();
        plane.ReverseFacing();
        cullPlanes.push_back( plane );

        // iterate through all of the portals in the current portal set.
        unsigned int fromPortalCount = fromPortalSet->GetPortalCount();
        for ( unsigned int j = 0; j < fromPortalCount; ++j )
        {
            // get the current portal.
            const GrPortal& fromPortal = fromPortalSet->GetPortal( j );

            // go through all portal sets in the current area.
            unsigned int toPortalSetCount = area->_portals.GetElemCount();
            for ( unsigned int k = 0; k < toPortalSetCount; ++k )
            {
                // get the current portal set.
                GrPolygonBSPPortalSet* toPortalSet = _portals[ i ];

                // iterate through all of the portals in the current portal set.
                unsigned int toPortalCount = toPortalSet->GetPortalCount();
                for ( unsigned int l = 0; l < toPortalCount; ++l )
                {
                    // skip the current portal if it's no longer visible.
                    if ( !IsPortalVisible( toPortalSet->GetPortal( j ), cullPlanes ) )
                        continue;

                    // generate cull planes to the current portal.
                    size_t planeSize = cullPlanes.size();
                    toPortalSet->GenCullPlaneSet( cullPlanes, fromPortal, l, true );

                    // no flood to the desired portal.
                    if ( neighbor->FloodToArea( cullPlanes, fromPortal, area ) )
                        return true;

                    // clear the cull planes and repeat.
                    cullPlanes.erase( cullPlanes.begin() + planeSize, cullPlanes.end() );
                }
            }
        }

        // remove the current from portal's plane.
        cullPlanes.pop_back();
    }

    // not visible!
    return false;
}

//----------------------------------------------------------
bool
GrPolygonBSPArea::FloodToArea( std::vector< MPlane >& cullPlanes, const GrPortal& fromPortal, GrPolygonBSPArea* toArea )
{
    _lastFloodID = 1;

    // check visibility of our portals.
    unsigned int portalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalSetCount; ++i )
    {
        // get the current portal set.
        GrPolygonBSPPortalSet* curPortalSet = _portals[ i ];
        GrPolygonBSPArea* neighbor = curPortalSet->GetNeighbor();

        // make sure we aren't already visiting this neighbor.
        if ( neighbor->_lastFloodID == 1 )
            continue;

        // get the plane of the current portal set.
        cullPlanes.push_back( curPortalSet->GetPlane() );

        // go through all of the portals in the current portal set.
        unsigned int portalCount = curPortalSet->GetPortalCount();
        for ( unsigned int j = 0; j < portalCount; ++j )
        {
            // get the current portal.
            const GrPortal& curPortal = curPortalSet->GetPortal( j );

            // check visibility.
            if ( IsPortalVisible( curPortal, cullPlanes ) )
            {
                // did we hit our goal portal?
                if ( neighbor == toArea )
                {
                    _lastFloodID = 0;
                    return true;
                }

                // generate new cull planes and add them to our list.
                size_t curCullPlaneCount = cullPlanes.size();
                curPortalSet->GenCullPlaneSet( cullPlanes, fromPortal, j, false );

                // flood towards the portal.
                if ( neighbor->FloodToArea( cullPlanes, fromPortal, toArea ) )
                {
                    _lastFloodID = 0;
                    return true;
                }

                // remove added cull planes.
                cullPlanes.erase( cullPlanes.begin() + curCullPlaneCount, cullPlanes.end() );
            }
        }

        // remove the plane of the current portal set.
        cullPlanes.pop_back();
    }

    // if we get here, then we didn't find what we were looking for.
    _lastFloodID = 0;
    return false;
}
#endif
//----------------------------------------------------------
bool
GrPolygonBSPArea::IsPortalVisible( const GrPortal& curPortal, const std::vector< MPlane >& cullPlanes )
{
    // if there are no cull planes, assume we're visible.
    if ( cullPlanes.size() == 0 )
        return true;

    // get information about the polygon.
//  const GrPolygon& portalPoly = curPortal.GetPolygon();
    const unsigned int portalVertexCount = curPortal.GetVertexCount();

    // go through each plane...
    const unsigned int planeCount = ( unsigned int )cullPlanes.size();
    unsigned int i = planeCount;
//  for ( unsigned int i = 0; i < planeCount; ++i )
    while ( i-- > 0 )
    {
        // get the current plane.
        const MPlane& curPlane = cullPlanes[ i ];

        // check to see if all of the polygon's vertices are entirely on the negative
        // side of the current plane.
        unsigned int j = 0;
        for ( ; j < portalVertexCount; ++j )
        {
            if ( curPlane.GetSide( curPortal.GetVertex( j ), 0.01f * GR_PVS_VISIBILITY_EPSILON ) > 0 )
                break;
        }

        // the portal is not visible if all points of the current polygon are outside
        // of the hull.
        if ( j == portalVertexCount )
            return false;
    }

    // the polygon is either on the positive side of or intersects with all planes.  It's
    // therefor considered visible.
    return true;
}
/*
//----------------------------------------------------------
void
GrPolygonBSPArea::CalcPortalBounds()
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float minZ = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;
    float maxZ = -FLT_MAX;
    const unsigned int portalSetCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalSetCount; ++i )
    {
        const unsigned int portalCount = _portals[ i ]->GetPortalCount();
        for ( unsigned int j = 0; j < portalCount; ++j )
        {
            // get the current portal.
            const GrPortal& curPortal = _portals[ i ]->GetPortal( j );

            // iterate all vertices taking min and max.
            const unsigned int portalVertexCount = curPortal.GetVertexCount();
            for ( unsigned int k = 0; k < portalVertexCount; ++k )
            {
                const MVec3& curVertex = curPortal.GetVertex( k );
                minX = ( curVertex.GetX() < minX ) ? curVertex.GetX() : minX;
                minY = ( curVertex.GetY() < minY ) ? curVertex.GetY() : minY;
                minZ = ( curVertex.GetZ() < minZ ) ? curVertex.GetZ() : minZ;
                maxX = ( curVertex.GetX() > maxX ) ? curVertex.GetX() : maxX;
                maxY = ( curVertex.GetY() > maxY ) ? curVertex.GetY() : maxY;
                maxZ = ( curVertex.GetZ() > maxZ ) ? curVertex.GetZ() : maxZ;
            }
        }
    }

    // build the AA box.
    _portalBounds = MAABox(
        MVec3(
            0.5f * ( maxX + minX ),
            0.5f * ( maxY + minY ), 
            0.5f * ( maxZ + minZ )
            ),
        MVec3(
            0.5f * ( maxX - minX ),
            0.5f * ( maxY - minY ), 
            0.5f * ( maxZ - minZ )
            )
        );
}
*/
//----------------------------------------------------------
void
GrPolygonBSPArea::Clean()
{
    // free all of the portals.
    const unsigned int portalCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalCount; ++i )
        delete _portals[ i ];
    _portals.Clear();

    // free the ubertexture data.
    delete _uberData;
    _uberData = 0;
}
