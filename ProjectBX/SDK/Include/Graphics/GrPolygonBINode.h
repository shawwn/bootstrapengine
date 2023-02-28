//----------------------------------------------------------
// File:        GrPolygonBINode.h
// Author:      Kevin Bray
// Created:     09-15-08
//
// Purpose:     To define a node in a bounding interval.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UBlockAllocator.h"
#include "UFastArray.h"
#include "MAABox.h"
#include "MVec3.h"
#include "GrTypes.h"

// forward declarations.
class GrPolygonBITree;
class GrPolygonGroup;
class MAABox;
class MVec3;

//**********************************************************
// class GrPolygonBINode
//**********************************************************
class GrPolygonBINode
{
public:
    enum EAXIS
    {
        EA_X = 0,
        EA_Y = 1,
        EA_Z = 2,
    };

    enum ESIDE
    {
        ES_FRONT,
        ES_BACK,
    };

    GrPolygonBINode( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group );
    GrPolygonBINode( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group,
                     unsigned int* indices, unsigned int polyCount );
    ~GrPolygonBINode();

    // returns all polygons that touch the bounding volume specified.
    void                GetPolygons( UFastArray< unsigned int >& polygonIndices,
                                     const SVec3& boxMin, const SVec3& boxMax );

    void*               operator new( size_t );
    void                operator delete( void* mem );

private:
    static UBlockAllocator  _allocator;
    static unsigned int     _instanceCount;

    void                BuildTree( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group,
                                   unsigned int* indices, unsigned int polyCount );

    // child and polygon info.
    union
    {
        GrPolygonBINode*    _children[ 2 ];
        unsigned int        _polygon;
        GrPolygonBINode*    _nextFree;
    };

    // split plane info.
    EAXIS               _boundingAxis;
    unsigned short      _isLeaf;
    float               _min;
    float               _max;
};
