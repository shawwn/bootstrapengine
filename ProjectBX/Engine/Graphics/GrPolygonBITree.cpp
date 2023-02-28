//----------------------------------------------------------
// File:        GrPolygonBITree.cpp
// Author:      Kevin Bray
// Created:     09-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBITree.h"

// project includes.
#include "GrPolygonBINode.h"
#include "GrPolygonGroup.h"
#include "GrPolygon.h"


//**********************************************************
// class GrPolygonBITree
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonBITree::GrPolygonBITree( const GrPolygonGroup& polygons )
: _polygons( new GrPolygonGroup( polygons ) )
, _root( 0 )
, _ownPolygons( true )
{
    Init();
}

//----------------------------------------------------------
GrPolygonBITree::GrPolygonBITree( const GrPolygonGroup* polygons, bool takeOwnership )
: _polygons( polygons )
, _root( 0 )
, _ownPolygons( takeOwnership )
{
    // make sure that a valid polygon group was passed in.
    B_ASSERT( polygons != 0 );
    Init();
}

//----------------------------------------------------------
GrPolygonBITree::~GrPolygonBITree()
{
    if ( _ownPolygons )
    {
        delete _polygons;
        _polygons = 0;
    }
    delete _root;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
const GrPolygon&
GrPolygonBITree::GetPolygon( unsigned int idx )
{
    return _polygons->GetPolygon( idx );
}

//----------------------------------------------------------
void
GrPolygonBITree::GetPolygons( UFastArray< unsigned int >& polygonIndices,
                              const SVec3& boxMin, const SVec3& boxMax )
{
    _root->GetPolygons( polygonIndices, boxMin, boxMax );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBITree::Init()
{
    // build the tree.
    unsigned int treeDepth = 0;
    _root = new GrPolygonBINode( treeDepth, this, *_polygons );
}
