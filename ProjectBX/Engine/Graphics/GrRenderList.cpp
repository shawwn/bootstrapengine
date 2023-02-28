//----------------------------------------------------------
// File:        GrRenderList.cpp
// Author:      Kevin Bray
// Created:     03-15-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderList.h"

// project includes.
#include "GrMeshInst.h"
#include "GrMaterial.h"
#include "GrCamera.h"

static const GrCamera* _depthSortCamera = 0;
static int StateSortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right );
static int StateDepthSortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right );

//**********************************************************
// class GrRenderList
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderList::GrRenderList()
{

}

//----------------------------------------------------------
GrRenderList::~GrRenderList()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderList::AddMeshInstRange( GrMeshInst* meshInst, unsigned int range, bool preTransformed )
{
    // skip if no material is assigned.
    const GrMaterial* material = meshInst->GetMaterial( range );
    if ( material == 0 )
        return;
    
    _items.Push( SItem( meshInst, range, preTransformed ) );
}

//----------------------------------------------------------
void
GrRenderList::AddMeshInstRanges( GrMeshInst* meshInst, unsigned int startRange, unsigned int rangeCount, bool preTransformed )
{
    // skip if no material is assigned.
    unsigned int rangeEnd = startRange + rangeCount;
    for ( unsigned int i = startRange; i < rangeEnd; ++i )
        AddMeshInstRange( meshInst, i, preTransformed );
}

//----------------------------------------------------------
void
GrRenderList::AddMeshInst( GrMeshInst* meshInst, bool preTransformed )
{
    // skip if no material is assigned.
    unsigned int rangeCount = meshInst->GetRangeCount();
    for ( unsigned int i = 0; i < rangeCount; ++i )
        AddMeshInstRange( meshInst, i, preTransformed );
}

//----------------------------------------------------------
void
GrRenderList::Add( const SItem& item )
{
    _items.Push( item );
}

//----------------------------------------------------------
void
GrRenderList::AddUberPatchSet( GrUberPatchSet* patchSet )
{
    _patches.Push( patchSet );
}

//----------------------------------------------------------
void
GrRenderList::Sort( ItemCompare compare ) const
{
    _items.Sort( compare );
}

//----------------------------------------------------------
void
GrRenderList::SortByState() const
{
    _items.Sort( StateSortCmp );
}

//----------------------------------------------------------
void
GrRenderList::SortByStateAndFrontToBack( const GrCamera& camera ) const
{
    _depthSortCamera = &camera;
    _items.Sort( StateDepthSortCmp );
    _depthSortCamera = 0;
}

//----------------------------------------------------------
void
GrRenderList::GetVisibleItems( GrRenderList& visible, const GrFrustum& frustum ) const
{
    const unsigned int objCount = _items.GetElemCount();
    for ( unsigned int j = 0; j < objCount; ++j )
    {
        // check the current receiver for visibility.
        const SItem& curItem = _items[ j ];
        if ( curItem.preTransformed )
        {
            if ( frustum.IsAABoxInside( curItem.meshInst->GetMesh()->GetRangeAABox( curItem.range ) ) )
                visible.Add( curItem );
        }
        else
        {
            if ( frustum.IsOBoxInside( curItem.meshInst->GetRangeOBox( curItem.range ) ) )
                visible.Add( curItem );
        }
    }
}

//----------------------------------------------------------
void
GrRenderList::Clear()
{
    _items.Clear();
    _patches.Clear();
}


//==========================================================
// global functions
//==========================================================

//----------------------------------------------------------
int
StateSortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right )
{
    // get the materials for object.
    const GrMaterial* leftMat = left.meshInst->GetMaterial( left.range );
    const GrMaterial* rightMat = right.meshInst->GetMaterial( right.range );
    B_ASSERT( leftMat != 0 && rightMat != 0 );

    // smaller sort type comes first.
    if ( leftMat->GetSortType() != rightMat->GetSortType() )
        return leftMat->GetSortType() < rightMat->GetSortType();

    // if materials are equal, sort by whether or not they're
    // pretransformed.
    if ( leftMat == rightMat )
        return left.preTransformed > left.preTransformed;

    // don't care, so sort by material address and dynamic vs. static.
    return ( size_t )leftMat < ( size_t )rightMat;
}

//----------------------------------------------------------
int
StateDepthSortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right )
{
    // make sure a camera has been set.
    B_ASSERT( _depthSortCamera != 0 );

    // get the materials for object.
    const GrMaterial* leftMat = left.meshInst->GetMaterial( left.range );
    const GrMaterial* rightMat = right.meshInst->GetMaterial( right.range );
    B_ASSERT( leftMat != 0 && rightMat != 0 );

    // smaller sort type comes first.
    if ( leftMat->GetSortType() != rightMat->GetSortType() )
        return leftMat->GetSortType() < rightMat->GetSortType();

    // if materials are equal, sort by AO map.
    if ( leftMat == rightMat )
    {
        // if both items are pretransformed, sort by distance to the
        // camera.
        if ( left.preTransformed == left.preTransformed )
        {
            float distSqr1 = _depthSortCamera->GetPos().DistSqr( left.meshInst->GetTransform().GetTranslate() );
            float distSqr2 = _depthSortCamera->GetPos().DistSqr( right.meshInst->GetTransform().GetTranslate() );
            return ( distSqr1 < distSqr2 );
        }
        else
            return left.preTransformed > left.preTransformed;
    }

    // don't care, so sort by material address and dynamic vs. static.
    return ( size_t )leftMat < ( size_t )rightMat;
}
