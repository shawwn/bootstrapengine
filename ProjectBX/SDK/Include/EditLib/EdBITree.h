//----------------------------------------------------------
// File:        EdBITree.h
// Author:      Kevin Bray
// Created:     08-15-08
//
// Purpose:     Defines a bounding interval tree to allow for fast
//              ray casting.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "EdBITreeInterval.h"

// engine includes.
#include "EdSimplePolygonGroup.h"
#include "EdSimplePolygon.h"

// forward declarations.
class EdBITreeStack;

//**********************************************************
// class EdBITree
//**********************************************************
class EdBITree
{
public:
    EdBITree( const EdSimplePolygonGroup& polygons );
    EdBITree( const EdSimplePolygonGroup* polygons, bool takeOwnership );
    ~EdBITree();

    // polygon group access.
    const EdSimplePolygon&      GetPolygon( unsigned int idx );

    // ray casting functionality.  Returns true if there was an
    // intersection.
    bool                        CastRay( EdBITreeInterval::SRayIntersectInfo& hitInfo, const SVec3& start,
                                         const SVec3& dir, float maxLen );
    bool                        CastRayFast( EdBITreeInterval::SRayIntersectInfo& hitInfo, const SVec3& start,
                                             const SVec3& dir, float maxLen, EdBITreeInterval* root );

    // finds an optimal node to use as the casting root.  This can
    // be used to significantly speed up ray casts from within an
    // area.  The box passed in should encompass the area that can
    // possibly be hit by any ray.  The node returned by this
    // function should be passed in to the CastRay() variant that
    // accepts a root node.
    EdBITreeInterval*           FindBestCastRoot( const MAABox& box );
    EdBITreeInterval*           FindBestCastRootFast( const SVec3& boxMin, const SVec3& boxMax );

    // returns true if the AAB passed in intersects with the tree
    // at all.
    bool                        Intersects( const MAABox& box );

    // gets the polygon group.
    const EdSimplePolygonGroup& GetPolygonGroup() const     {   return *_polygons;      }

private:
    void                        Init();

    const EdSimplePolygonGroup* _polygons;
    EdBITreeInterval*           _root;
    EdBITreeStack*              _stack;
    bool                        _ownPolygons;
};
