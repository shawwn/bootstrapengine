//----------------------------------------------------------
// File:        GrPolygonBSPSplit.cpp
// Author:      Kevin Bray
// Created:     09-12-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBSPSplit.h"

// project includes.
#include "GrPolygonBSPArea.h"
#include "GrPolygonGroup.h"
#include "GrPolygonBSPPortalSet.h"
#include "GrPolygonUberUVMapper.h"
#include "GrMaterialMgr.h"
#include "GrMaterial.h"
#include "GrMeshInst.h"
#include "GrCamera.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrFrustumSet.h"
#include "MRay.h"

// windows includes (for PVS calculation timing)
#include <mmsystem.h>

// std c++ includes.
#include <malloc.h>
#include <float.h>

//**********************************************************
// class GrPolygonBSPSplit
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonBSPSplit::GrPolygonBSPSplit()
: _front( 0 )
, _back( 0 )
, _frontLeaf( new GrPolygonBSPArea( GrPolygonGroup() ) )
, _backLeaf( new GrPolygonBSPArea( GrPolygonGroup() ) )
, _parent( 0 )
{

}

//----------------------------------------------------------
GrPolygonBSPSplit::GrPolygonBSPSplit( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
                                      GrPolygonUberUVMapper& mapper, bool calcPVS, bool uberTexture,
                                      float uberTexelsPerMeter )
: _front( 0 )
, _back( 0 )
, _frontLeaf( 0 )
, _backLeaf( 0 )
, _parent( 0 )
{
    B_ASSERT( hullPolygons.GetPolygonCount() > 0 );

    // set the floating point control word for better precision.
    unsigned int prevFPCtrl = 0;

#ifdef _MSC_VER
    errno_t error = __INTERNAL_MATH_IMPL::_controlfp_s( &prevFPCtrl, _PC_64, _MCW_PC );
    B_ASSERT( error == 0 );
#elif defined( __MINGW32__ )
    prevFPCtrl =  __INTERNAL_MATH_IMPL::_controlfp( 0, 0 );
    __INTERNAL_MATH_IMPL::_controlfp( _PC_53, _MCW_PC );    
#else
#error need to implement FPU settings for this platform
#endif
    // create a modifyable copy of the hull and prop polygons.
    GrPolygonGroup tempHullPolygons( hullPolygons );

    // generate a list of planes for the current split.
    std::vector< MPlane > planes;
    tempHullPolygons.GenPlaneList( planes, GR_BSP_PLANE_EPSILON );

    // choose the first split.
    std::vector< MPlane >::iterator firstSplitIter = ChooseSplit( planes, tempHullPolygons );
    MPlane firstSplitPlane = *firstSplitIter;

    // print out some useful info.
    PrintF( "Building BSP tree...\n" );
    PrintF(
        " Number of primary BSP splits: %i\n"
        " Initial split chosen: %i\n",
        ( unsigned int )planes.size(), ( unsigned int )( firstSplitIter - planes.begin() ) );

    // build the BSP tree.
    planes.erase( firstSplitIter );
    Build( tempHullPolygons, planes, firstSplitPlane );

    // build all portals.
    if ( calcPVS )
    {
        PrintF( "Generating portals...\n" );
        BuildPortals();
    }

    // get all of our leaves.
    std::vector< GrPolygonBSPArea* > leaves;
    GetLeaves( leaves );

    // connect neighboring leaves with eachother.
    if ( calcPVS )
    {
        PrintF( "Connecting neighbors...\n" );
        ConnectNeighborLeaves( leaves );
    }

    // calculate bounding volumes.
    CalcBounds();

    // calculate PVS data.
    if ( calcPVS )
    {
        PrintF( "Calculating PVS data...\n" );
        CalcPVS( leaves );
    }

    // push prop geometry to leaves.
    PushPropsToLeaves( propPolygons );

    // generate ubertexture data.
    if ( uberTexture )
    {
        PrintF( "Generating BSP ubertexture UVs...\n" );
        GenUberTexData( uberTexelsPerMeter );

        PrintF( "Placing BSP ubertexture groups...\n" );
        std::vector< GrPolygonUberPatchGroup* > uberPatches;
        GetUberTexData( uberPatches );
        mapper.PlaceGroupsSlow( uberPatches, GrPolygonUberUVMapper::EP_XY, 16.0f );
    }

    // fix T junctions.
//  PrintF( "Fixing t-junctions...\n" );
//  FixTJuncs( leaves );

    // restore the previous floating point control word.
#ifdef _MSC_VER
    error = __INTERNAL_MATH_IMPL::_controlfp_s( &prevFPCtrl, prevFPCtrl, _MCW_PC );
    B_ASSERT( error == 0 );
#elif defined( __MINGW32__ )
    __INTERNAL_MATH_IMPL::_controlfp( prevFPCtrl, _MCW_PC );    
#else
#error need to implement FPU settings for this platform
#endif


    PrintF( "Building of BSP tree completed.\n" );
}

//----------------------------------------------------------
GrPolygonBSPSplit::GrPolygonBSPSplit( const GrPolygonGroup& polygons, const std::vector< MPlane >& planes, const MPlane& split,
                                      GrPolygonBSPSplit* parent )
: _front( 0 )
, _back( 0 )
, _frontLeaf( 0 )
, _backLeaf( 0 )
, _parent( parent )
{
    GrPolygonGroup tempCopy( polygons );
    Build( tempCopy, planes, split );
}

//----------------------------------------------------------
GrPolygonBSPSplit::GrPolygonBSPSplit( UReader& reader, GrPolygonBSPSplit* parent )
: _front( 0 )
, _back( 0 )
, _frontLeaf( 0 )
, _backLeaf( 0 )
, _parent( parent )
{
    Load( reader );
}

//----------------------------------------------------------
GrPolygonBSPSplit::~GrPolygonBSPSplit()
{
    Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBSPSplit::GetVisibilityData( const MVec3& viewPos, const std::vector< unsigned int >** pvsIndices )
{
    int side = _split.GetSide( viewPos, 0.0f );
    if ( side >= 0 )
    {
        if ( _front )
            _front->GetVisibilityData( viewPos, pvsIndices );
        else if ( _frontLeaf )
            _frontLeaf->GetVisibilityData( pvsIndices );
    }
    else
    {
        if ( _back )
            _back->GetVisibilityData( viewPos, pvsIndices );
        else if ( _backLeaf )
            _backLeaf->GetVisibilityData( pvsIndices );
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::GetLeaves( std::vector< GrPolygonBSPArea* >& leaves )
{
    if ( _front )
        _front->GetLeaves( leaves );
    else if ( _frontLeaf )
        leaves.push_back( _frontLeaf );

    if ( _back )
        _back->GetLeaves( leaves );
    else if ( _backLeaf )
        leaves.push_back( _backLeaf );
}

//----------------------------------------------------------
bool
GrPolygonBSPSplit::GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist )
{
    // if we don't intersect with the bounding box, simply return.
//  if ( !ray.Intersect( _aaBox, maxDist ) )
//      return false;

    // determine if we even hit the plane.
    float dist = _split.Dist( ray.GetStart() );

    // recurse towards the starting location first.
    if ( dist >= 0 )
    {
        // recurse the front.
        if ( _front )
        {
            if ( _front->GetIntersect( hitPos, ray, maxDist ) )
                return true;
        }
        else if ( _frontLeaf )
        {
            if ( _frontLeaf->GetIntersect( hitPos, ray, maxDist ) )
                return true;
        }

        // determine if we can recurse the back.
        float distAlongRay = dist / -ray.GetDir().Dot( _split.GetNormal() );

        // if no intersection was found, recurse the back.
        if ( distAlongRay > 0.0f && distAlongRay < maxDist )
        {
            if ( _back )
            {
                if ( _back->GetIntersect( hitPos, ray, maxDist ) )
                    return true;
            }
            else if ( _backLeaf )
            {
                if ( _backLeaf->GetIntersect( hitPos, ray, maxDist ) )
                    return true;
            }
        }
    }
    else
    {
        // recurse the back.
        if ( _back )
        {
            if ( _back->GetIntersect( hitPos, ray, maxDist ) )
                return true;
        }
        else if ( _backLeaf )
        {
            if ( _backLeaf->GetIntersect( hitPos, ray, maxDist ) )
                return true;
        }

        // determine if we can recurse the front.
        float distAlongRay = dist / -ray.GetDir().Dot( _split.GetNormal() );

        // if no intersection was found, recurse the front.
        if ( distAlongRay > 0.0f && distAlongRay < maxDist )
        {
            if ( _front )
            {
                if ( _front->GetIntersect( hitPos, ray, maxDist ) )
                    return true;
            }
            else if ( _frontLeaf )
            {
                if ( _frontLeaf->GetIntersect( hitPos, ray, maxDist ) )
                    return true;
            }
        }
    }

    // no hit found.
    return false;
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const
{
    // traverse the front.
    if ( _front )
        _front->GetUberTexData( dataArray );
    else if ( _frontLeaf )
        _frontLeaf->GetUberTexData( dataArray );

    // traverse the back.
    if ( _back )
        _back->GetUberTexData( dataArray );
    else if ( _backLeaf )
        _backLeaf->GetUberTexData( dataArray );
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::Save( UWriter& writer )
{
    writer.WriteUInt( 'NPSB' );

    _split.Save( writer );
    _aaBox.Save( writer );

    unsigned int flags = 0;
    if ( _front )
        flags |= 1;
    if ( _back )
        flags |= 2;
    if ( _frontLeaf )
        flags |= 4;
    if ( _backLeaf )
        flags |= 8;

    writer.WriteUInt( flags );
    if ( _front )
        _front->Save( writer );
    if ( _back )
        _back->Save( writer );
    if ( _frontLeaf )
        _frontLeaf->Save( writer );
    if ( _backLeaf )
        _backLeaf->Save( writer );
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::Load( UReader& reader )
{
    Clean();

    unsigned int tag = reader.ReadUInt();
    B_ASSERT( tag == 'NPSB' );

    _split.Load( reader );
    _aaBox.Load( reader );

    unsigned int flags = reader.ReadUInt();
    if ( flags & 1 )
        _front = new GrPolygonBSPSplit( reader, this );
    if ( flags & 2 )
        _back = new GrPolygonBSPSplit( reader, this );
    if ( flags & 4 )
        _frontLeaf = new GrPolygonBSPArea( reader );
    if ( flags & 8 )
        _backLeaf = new GrPolygonBSPArea( reader );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBSPSplit::Build( GrPolygonGroup& polygons, const std::vector< MPlane >& planes, const MPlane& split )
{
    // store the split plane.
    _split = split;

    // split the polygons.
    GrPolygonGroup* front = new GrPolygonGroup();
    GrPolygonGroup* back = new GrPolygonGroup();
    int ret = polygons.Split( *front, *back, _split, GR_BSP_PLANE_EPSILON );
    polygons.Clear();

    // either recurse or create leaves if the returned groups are convex.
    if ( ret >= 0 )
    {
        // choose the best split plane.
        std::vector< MPlane > localPlanes = planes;
        std::vector< MPlane >::iterator splitIter = ChooseSplit( localPlanes, *front );

        // we're a leaf if there are no more valid split planes left.
        if ( splitIter == localPlanes.end() )
        {
            _frontLeaf = new GrPolygonBSPArea( *front );
        }
        else
        {
            MPlane nextSplit = *splitIter;
            localPlanes.erase( splitIter );
            _front = new GrPolygonBSPSplit( *front, localPlanes, nextSplit, this );
        }
    }
    else
    {
        // empty front leaf.
        _frontLeaf = new GrPolygonBSPArea( GrPolygonGroup() );
    }
    delete front;

    if ( ret <= 0 )
    {
        // choose the best split plane and remove it from the list.
        std::vector< MPlane > localPlanes = planes;
        std::vector< MPlane >::iterator splitIter = ChooseSplit( localPlanes, *back );

        // we're a leaf if there are no more valid split planes left.
        if ( splitIter == localPlanes.end() )
        {
            _backLeaf = new GrPolygonBSPArea( *back );
        }
        else
        {
            MPlane nextSplit = *splitIter;
            localPlanes.erase( splitIter );
            _back = new GrPolygonBSPSplit( *back, localPlanes, nextSplit, this );
        }
    }
    else
    {
        // empty back leaf.
        _backLeaf = new GrPolygonBSPArea( GrPolygonGroup() );
    }
    delete back;
}

//----------------------------------------------------------
std::vector< MPlane >::iterator
GrPolygonBSPSplit::ChooseSplit( std::vector< MPlane >& planes, const GrPolygonGroup& polygons ) const
{
    std::vector< MPlane >::iterator iter = planes.begin();
    const std::vector< MPlane >::iterator end = planes.end();
    for ( ; iter != end; ++iter )
    {
        // find a polygon that is coplanar to this plane.
        const unsigned int polyCount = polygons.GetPolygonCount();
        for ( unsigned int i = 0; i < polyCount; ++i )
        {
            // get the current polygon.
            const GrPolygon& curPolygon = polygons.GetPolygon( i );
            if ( curPolygon.IsCoplanar( *iter, GR_BSP_PLANE_EPSILON ) )
                return iter;
        }
    }

    // no valid planes were found.  This means that we have a convex
    // hull that can be used as geometry.
    return planes.end();
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::CalcBounds()
{
    bool boundsSet = false;
    if ( _front )
    {
        _front->CalcBounds();
        _aaBox = _front->_aaBox;
        boundsSet = true;
    }
    else if ( _frontLeaf && _frontLeaf->GetPolygons().GetPolygonCount() > 0 )
    {
        _aaBox = _frontLeaf->GetPolygons().GetAABox();
        boundsSet = true;
    }

    if ( _back )
    {
        _back->CalcBounds();
        if ( boundsSet )
            _aaBox.AddBoxToVolume( _back->_aaBox );
        else
            _aaBox = _back->_aaBox;
    }
    else if ( _backLeaf && _backLeaf->GetPolygons().GetPolygonCount() > 0 )
    {
        if ( boundsSet )
            _aaBox.AddBoxToVolume( _backLeaf->GetPolygons().GetAABox() );
        else
            _aaBox = _backLeaf->GetPolygons().GetAABox();
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::BuildPortals()
{
    // build the portal for the current BSP node.
    GrPolygonBSPPortalSet* newPortalSet = new GrPolygonBSPPortalSet( this, GR_BSP_GEOMETRY_SCALE );

    // reduce the current portal if needed.
    if ( ReducePortal( newPortalSet ) )
    {
        // push the portal to the front children.
        if ( _front )
        {
            _front->PushPortalToLeaves( newPortalSet );
        }
        else if ( _frontLeaf )
            _frontLeaf->AddPortal( new GrPolygonBSPPortalSet( *newPortalSet ) );

        // reverse the winding and push down to the back children.
        newPortalSet->ReverseWinding();
        if ( _back )
        {
            _back->PushPortalToLeaves( newPortalSet );
        }
        else if ( _backLeaf )
            _backLeaf->AddPortal( new GrPolygonBSPPortalSet( *newPortalSet ) );
    }

    delete newPortalSet;

    // recurse
    if ( _front )
        _front->BuildPortals();
    if ( _back )
        _back->BuildPortals();
}

//----------------------------------------------------------
bool
GrPolygonBSPSplit::ReducePortal( GrPolygonBSPPortalSet* portal )
{
    // go through parents, splitting the portal.
    GrPolygonBSPSplit* parent = _parent;
    GrPolygonBSPSplit* child = this;
    while ( parent != 0 )
    {
        // split by the current parent plane.
        GrPolygonBSPPortalSet front;
        GrPolygonBSPPortalSet back;
        int ret = portal->Split( front, back, parent->_split, GR_BSP_PLANE_EPSILON );

        // get the previous.
        if ( child == parent->_front )
        {
            // it's possible to have a degenerate region.  In which case, we're screwed.  Therefor,
            // we should just ignore this portal.
            if ( ret < 0 || front.GetPortalCount() == 0 )
                return false;
            *portal = front;
        }
        else if ( child == parent->_back )
        {
            // it's possible to have a degenerate region.  In which case, we're screwed.  Therefor,
            // we should just ignore this portal.
            if ( ret > 0 || back.GetPortalCount() == 0 )
                return false;
            *portal = back;
        }
        else
        {
            B_ASSERT( false );
        }

        // next.
        child = parent;
        parent = parent->_parent;
    }
    return true;
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::PushPortalToLeaves( GrPolygonBSPPortalSet* newPortal )
{
    // split the portal by the current BSP plane.
    GrPolygonBSPPortalSet* front = new GrPolygonBSPPortalSet();
    GrPolygonBSPPortalSet* back = new GrPolygonBSPPortalSet();
    int ret = newPortal->Split( *front, *back, _split, GR_BSP_PLANE_EPSILON );
    if ( ret >= 0 && front->GetPortalCount() > 0 )
    {
        if ( _front )
        {
            _front->PushPortalToLeaves( front );
            delete front;
        }
        else if ( _frontLeaf )
            _frontLeaf->AddPortal( front );
    }
    if ( ret <= 0 && back->GetPortalCount() > 0 )
    {
        if ( _back )
        {
            _back->PushPortalToLeaves( back );
            delete back;
        }
        else if ( _backLeaf )
            _backLeaf->AddPortal( back );
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::PushPropsToLeaves( const GrPolygonGroup& propPolygons )
{
    // split the current group by the split plane.
    GrPolygonGroup front;
    GrPolygonGroup back;
    int ret = propPolygons.Split( front, back, _split, GR_BSP_PLANE_EPSILON );

    // push to front.
    if ( ret >= 0 )
    {
        if ( _front )
            _front->PushPropsToLeaves( front );
        else if ( _frontLeaf )
            _frontLeaf->GetPolygons().AddGroup( front );
    }

    // push to back.
    if ( ret <= 0 )
    {
        if ( _back )
            _back->PushPropsToLeaves( back );
        else if ( _backLeaf )
            _backLeaf->GetPolygons().AddGroup( back );
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::GenUberTexData( float texelsPerMeter )
{
    if ( _front )
        _front->GenUberTexData( texelsPerMeter );
    else if ( _frontLeaf )
        _frontLeaf->GenUberTexData( texelsPerMeter );

    if ( _back )
        _back->GenUberTexData( texelsPerMeter );
    else if ( _backLeaf )
        _backLeaf->GenUberTexData( texelsPerMeter );
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::GenUniquePlanes( std::vector< MPlane >& planes, const GrPolygonGroup& polygons )
{
    planes.clear();
    const unsigned int polyCount = polygons.GetPolygonCount();
    for ( unsigned int i = 0; i < polyCount; ++i )
    {
        unsigned int j = 0;
        for ( ; j < planes.size(); ++j )
        {
            MPlane& plane = planes[ j ];
            if ( polygons.GetPolygon( i ).IsCoplanar( plane, GR_BSP_PLANE_EPSILON ) )
                break;
        }
        if ( j == planes.size() )
            planes.push_back( polygons.GetPolygon( i ).GetPlane() );
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::FixTJuncs( const std::vector< GrPolygonBSPArea* >& leaves )
{
    // tell the leaves to fix t-junctions with their neighbors.
    unsigned int leafCount = ( unsigned int )leaves.size();
    for ( unsigned int i = 0; i < leafCount; ++i )
    {
        leaves[ i ]->FixTJuncs();
//      for ( unsigned int j = 0; j < leafCount; ++j )
//      {
//          if ( i == j )
//              continue;
//
//          leaves[ i ]->GetPolygons().FixTJuncs( leaves[ j ]->GetPolygons() );
//      }
    }
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::ConnectNeighborLeaves( const std::vector< GrPolygonBSPArea* >& leaves )
{
    // tell the leaves to connect with their neighbors.
    unsigned int leafCount = ( unsigned int )leaves.size();
    for ( unsigned int i = 0; i < leafCount; ++i )
        leaves[ i ]->FindNeighbors( this );
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::CalcPVS( const std::vector< GrPolygonBSPArea* >& leaves )
{
    // generate PVS data.
    unsigned int time = 0;
    unsigned int totalTime = 0;
    unsigned int leafCount = ( unsigned int )leaves.size();
    for ( unsigned int i = 0; i < leafCount; ++i )
    {
        time = timeGetTime();
        PrintF( "- Calculating PVS data for leaf %d out of %d... ", i+1, leafCount );
        leaves[ i ]->CalcPVS( leaves );
        time = timeGetTime() - time;
        PrintF( "[done]  Total time: %d\n", time );
        totalTime += time;
    }
    PrintF( "PVS generation completed.  Total time: %d\n", totalTime );
}

//----------------------------------------------------------
void
GrPolygonBSPSplit::Clean()
{
    delete _front;
    delete _back;

    _front = 0;
    _back = 0;
}
