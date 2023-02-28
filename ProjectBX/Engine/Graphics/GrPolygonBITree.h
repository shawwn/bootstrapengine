//----------------------------------------------------------
// File:        GrPolygonBITree.h
// Author:      Kevin Bray
// Created:     09-15-08
//
// Purpose:     To partition a group of polygons for fast spacial
//              indexing.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrPolygonBITree.h"

// engine includes.
#include "GrPolygonGroup.h"
#include "GrPolygon.h"

// forward declarations.
class GrPolygonBINode;
class GrPolygonGroup;
class GrPolygon;

//**********************************************************
// class GrPolygonBITree
//**********************************************************
class GrPolygonBITree
{
public:
    GrPolygonBITree( const GrPolygonGroup& polygons );
    GrPolygonBITree( const GrPolygonGroup* polygons, bool takeOwnership );
    ~GrPolygonBITree();

    // polygon group access.
    const GrPolygon&            GetPolygon( unsigned int idx );

    // returns all polygons that touch the bounding volume specified.
    void                        GetPolygons( UFastArray< unsigned int >& polygonIndices,
                                             const SVec3& boxMin, const SVec3& boxMax );

    // gets the polygon group.
    const GrPolygonGroup&       GetPolygonGroup() const     {   return *_polygons;      }

private:
    void                        Init();

    const GrPolygonGroup*       _polygons;
    GrPolygonBINode*            _root;
    bool                        _ownPolygons;
};
