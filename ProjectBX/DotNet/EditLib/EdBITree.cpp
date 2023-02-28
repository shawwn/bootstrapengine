//----------------------------------------------------------
// File:        EdBITree.cpp
// Author:      Kevin Bray
// Created:     08-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBITree.h"

// project includes.
#include "EdBITreeInterval.h"
#include "EdBITreeStack.h"

//**********************************************************
// class EdBITree
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBITree::EdBITree( const EdSimplePolygonGroup& polygons )
: _polygons( new EdSimplePolygonGroup( polygons ) )
, _root( 0 )
, _stack( 0 )
, _ownPolygons( true )
{
    Init();
}

//----------------------------------------------------------
EdBITree::EdBITree( const EdSimplePolygonGroup* polygons, bool takeOwnership )
: _polygons( polygons )
, _root( 0 )
, _stack( 0 )
, _ownPolygons( takeOwnership )
{
    // make sure that a valid polygon group was passed in.
    B_ASSERT( polygons != 0 );
    Init();
}

//----------------------------------------------------------
EdBITree::~EdBITree()
{
    if ( _ownPolygons )
    {
        delete _polygons;
        _polygons = 0;
    }
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
const EdSimplePolygon&
EdBITree::GetPolygon( unsigned int idx )
{
    return _polygons->GetPolygon( idx );
}

//----------------------------------------------------------
bool
EdBITree::CastRay( EdBITreeInterval::SRayIntersectInfo& hitInfo, const SVec3& start,
                   const SVec3& dir, float maxLen )
{
    // cast a ray.
    hitInfo.polygonIdx = ~0;
    hitInfo.polyHitInfo.hitDist = FLT_MAX;
    return _root->CastRay( hitInfo, *_polygons, start, dir, *_stack, maxLen );
}

//----------------------------------------------------------
bool
EdBITree::CastRayFast( EdBITreeInterval::SRayIntersectInfo& hitInfo, const SVec3& start,
                       const SVec3& dir, float maxLen, EdBITreeInterval* root )
{
    // make sure the root is non-zero.
    B_ASSERT( root != 0 );

    // cast a ray.
    hitInfo.polygonIdx = ~0;
    hitInfo.polyHitInfo.hitDist = FLT_MAX;
    return root->CastRay( hitInfo, *_polygons, start, dir, *_stack, maxLen );
}

//----------------------------------------------------------
EdBITreeInterval*
EdBITree::FindBestCastRoot( const MAABox& box )
{
    // finds the best interval to use as the root for multiple
    // ray casting operations.
    return _root->FindBestCastRoot( box );
}

//----------------------------------------------------------
EdBITreeInterval*
EdBITree::FindBestCastRootFast( const SVec3& boxMin, const SVec3& boxMax )
{
    // finds the best interval to use as the root for multiple
    // ray casting operations.
    return _root->FindBestCastRootFast( boxMin, boxMax );
}

//----------------------------------------------------------
bool
EdBITree::Intersects( const MAABox& box )
{
    return _root->Intersects( box );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdBITree::Init()
{
    // build the tree.
    unsigned int treeDepth = 0;
    _root = new EdBITreeInterval( treeDepth, this, *_polygons );

    // now based on the tree's depth, allocate the stack.
    _stack = new EdBITreeStack( treeDepth );
}
