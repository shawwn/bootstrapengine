//----------------------------------------------------------
// File:		GrPolygonEdgeList.h
// Author:		Kevin Bray
// Created:		12-05-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonEdgeList.h"

// project includes.
#include "GrPolygonGroup.h"
#include "GrPolygon.h"
#include "GrConfig.h"
#include "ULRUHash.h"

inline int ToFixed( float x )
{
	return ( int )( x * 1024.0 );
}

inline bool IsPosEqual( const SVec3& p0, const SVec3& p1 )
{
	return ( Abs( p0.x - p1.x ) <= GR_SMALLEST_UNIT &&
			 Abs( p0.y - p1.y ) <= GR_SMALLEST_UNIT &&
			 Abs( p0.z - p1.z ) <= GR_SMALLEST_UNIT );
}

//**********************************************************
// class GrPolygonEdgeList
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonEdgeList::GrPolygonEdgeList( GrPolygonGroup* group )
: _group( group )
, _edges( 0 )
, _vertexEdges( 0 )
, _polygonOffsets( 0 )
, _edgeCount( 0 )
{
	// allocate space for edge data.
	_edges = new SEdge[ group->GetVertexCount() ];
	_vertexEdges = new unsigned int[ group->GetVertexCount() ];
	_polygonOffsets = new unsigned int[ group->GetPolygonCount() ];

	// build the edge list.
	BuildEdgeList();
}

//----------------------------------------------------------
GrPolygonEdgeList::~GrPolygonEdgeList()
{
	delete[] _polygonOffsets;
	delete[] _vertexEdges;
	delete[] _edges;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrPolygonEdgeList::GetNeighbor( unsigned int polygonIdx, unsigned int polygonEdge ) const
{
	const SEdge& edge = _edges[ _vertexEdges[ _polygonOffsets[ polygonIdx ] + polygonEdge ] ];
	return ( edge.poly0 != polygonIdx ) ? edge.poly0 : edge.poly1;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonEdgeList::BuildEdgeList()
{
	// hash table for polygon edges.
	ULRUHash< unsigned int, unsigned int > edgeHash( FindPow2( _group->GetVertexCount() ) );

	// calculate edges.
	unsigned int curVertexEdge = 0;
	unsigned int polyCount = _group->GetPolygonCount();
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		// store the current polygon's vertexIndex array offset.
		_polygonOffsets[ i ] = curVertexEdge;

		// get the current polygon and iterate through it's edges.
		const GrPolygon& curPoly = _group->GetPolygon( i );
		unsigned int edgeCount = curPoly.GetVertexCount();
		for ( unsigned int curVertIdx = 0; curVertIdx < edgeCount; ++curVertIdx, ++curVertexEdge )
		{
			// get the next vertex index.
			unsigned int nextVertIdx = ( curVertIdx + 1 ) % edgeCount;

			// get the current vertices.
			const SPolygonVertex& v0 = curPoly.GetVertex( curVertIdx );
			const SPolygonVertex& v1 = curPoly.GetVertex( nextVertIdx );

			// hash the current vertex.
			unsigned int revKey = HashEdge( v1, v0 );
			void* iter = edgeHash.FindFirst( revKey );
			while ( iter != 0 )
			{
				// get the current edge.
				unsigned int edgeIdx = edgeHash.GetValue( iter );
				SEdge& curEdge = _edges[ edgeIdx ];

				// get the neighboring polygon and it's vertices.
				const GrPolygon& neighbor = _group->GetPolygon( curEdge.poly0 );
				const SPolygonVertex& neighborVert0 = neighbor.GetVertex( curEdge.poly0Verts[ 0 ] );
				const SPolygonVertex& neighborVert1 = neighbor.GetVertex( curEdge.poly0Verts[ 1 ] );

				// determine if the edges are really neighbors.
				if ( IsEdgeShared( neighborVert0, neighborVert1, v0, v1 ) )
				{
					// we have a shared edge, so modify the current
					// edge entry.
					curEdge.poly1 = i;
					curEdge.poly1Verts[ 0 ] = curVertIdx;
					curEdge.poly1Verts[ 1 ] = nextVertIdx;

					// store the vertex's edge index.
					_vertexEdges[ curVertexEdge ] = edgeIdx;

					// break since we found a shared edge.
					break;
				}

				// next.
				iter = edgeHash.FindNext( iter, revKey );
			}

			// check to see if we need to add a new edge entry.
			if ( iter == 0 )
			{
				// allocate a new edge and fill it out.
				SEdge& newEdge = _edges[ _edgeCount ];
				newEdge.poly0 = i;
				newEdge.poly0Verts[ 0 ] = curVertIdx;
				newEdge.poly0Verts[ 1 ] = nextVertIdx;
				newEdge.poly1 = ~0;
				newEdge.poly1Verts[ 0 ] = ~0;
				newEdge.poly1Verts[ 1 ] = ~0;

				// store the vertex's edge index.
				_vertexEdges[ curVertexEdge ] = _edgeCount;

				// hash our newly allocated edge.
				unsigned int edgeKey = HashEdge( v0, v1 );
				edgeHash.Insert( edgeKey, edgeKey, _edgeCount++ );
			}
		}
	}
}

//----------------------------------------------------------
unsigned int
GrPolygonEdgeList::HashEdge( const SPolygonVertex& v0, const SPolygonVertex& v1 )
{
	// position data.
	int posData[ 6 ] = 
	{
		ToFixed( v0.position.x ),
		ToFixed( v0.position.y ),
		ToFixed( v0.position.z ),
		ToFixed( v1.position.x ),
		ToFixed( v1.position.y ),
		ToFixed( v1.position.z ),
	};

	// hash fixed point data.
	return HashMem32( posData, sizeof( posData ) );
}

//----------------------------------------------------------
bool
GrPolygonEdgeList::IsEdgeShared( const SPolygonVertex& e0v0, const SPolygonVertex& e0v1,
								 const SPolygonVertex& e1v0, const SPolygonVertex& e1v1 )
{
	return IsPosEqual( e0v0.position, e1v1.position ) && IsPosEqual( e0v1.position, e1v0.position );
}
