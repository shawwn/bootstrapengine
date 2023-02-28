//----------------------------------------------------------
// File:        GrPolygonEdgeList.h
// Author:      Kevin Bray
// Created:     12-05-08
//
// Purpose:     To represent a list of polygon edges.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrPolygonGroup;
struct SPolygonVertex;

//**********************************************************
// class GrPolygonEdgeList
//**********************************************************
class GrPolygonEdgeList
{
public:
    struct SEdge
    {
        unsigned int poly0;
        unsigned int poly1;
        unsigned int poly0Verts[ 2 ];
        unsigned int poly1Verts[ 2 ];
    };

    GrPolygonEdgeList( GrPolygonGroup* group );
    ~GrPolygonEdgeList();

    // edge iteration.
    unsigned int        GetEdgeCount() const                    {   return _edgeCount;          }
    const SEdge&        GetEdge( unsigned int edgeIdx ) const   {   return _edges[ edgeIdx ];   }

    // returns the neighboring polygon on the edge of the polygon
    // specified.
    unsigned int        GetNeighbor( unsigned int polygonIdx, unsigned int polygonEdge ) const;

private:
    void                BuildEdgeList();
    unsigned int        HashEdge( const SPolygonVertex& v0, const SPolygonVertex& v1 );
    bool                IsEdgeShared( const SPolygonVertex& e0v0, const SPolygonVertex& e0v1,
                                      const SPolygonVertex& e1v0, const SPolygonVertex& e1v1 );

    GrPolygonGroup*     _group;
    SEdge*              _edges;
    unsigned int*       _vertexEdges;
    unsigned int*       _polygonOffsets;
    unsigned int        _edgeCount;
};