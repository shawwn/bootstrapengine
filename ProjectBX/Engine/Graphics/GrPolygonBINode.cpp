//----------------------------------------------------------
// File:		GrPolygonBINode.h
// Author:		Kevin Bray
// Created:		09-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBINode.h"

// project includes.
#include "GrPolygonGroup.h"
#include "GrPolygon.h"
#include "MAABox.h"
#include "MVec3.h"


UBlockAllocator
GrPolygonBINode::_allocator( sizeof( GrPolygonBINode ), 4096 );

unsigned int
GrPolygonBINode::_instanceCount = 0;

//**********************************************************
// class GrPolygonBINode
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonBINode::GrPolygonBINode( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group )
: _polygon( ~0 )
, _boundingAxis( EA_X )
, _isLeaf( 0 )
, _min( 0.0f )
, _max( 0.0f )
{
	++_instanceCount;

	// get the total number of polygons that need to be partitioned.
	unsigned int polyCount = group.GetPolygonCount();

	// allocate and initialize the indices.
	unsigned int* indices = new unsigned int[ polyCount ];
	for ( unsigned int i = 0; i < polyCount; ++i )
		indices[ i ] = i;

	// now build the tree.
	BuildTree( depth, parent, group, indices, polyCount );

	// free the indices.
	delete[] indices;
}

//----------------------------------------------------------
GrPolygonBINode::GrPolygonBINode( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group,
								  unsigned int* indices, unsigned int polyCount )
: _polygon( ~0 )
, _boundingAxis( EA_X )
, _isLeaf( 0 )
, _min( 0.0f )
, _max( 0.0f )
{
	// build the tree.
	BuildTree( depth, parent, group, indices, polyCount );
	++_instanceCount;
}

//----------------------------------------------------------
GrPolygonBINode::~GrPolygonBINode()
{
	if ( !_isLeaf )
	{
		delete _children[ 0 ];
		delete _children[ 1 ];
	}

	// decrement the instance counter.
	--_instanceCount;

	// if there are no more nodes left, free all memory related
	// to nodes.
	if ( _instanceCount == 0 )
		_allocator.FreeMem();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBINode::GetPolygons( UFastArray< unsigned int >& polygonIndices,
							  const SVec3& boxMin, const SVec3& boxMax )
{
	// check to see if the box passed in fits inside of the
	// bounds.
	float p0 = boxMin[ _boundingAxis ];
	float p1 = boxMax[ _boundingAxis ];
	if ( p1 < _min )
		return;
	if ( p0 > _max )
		return;

	// if we're not a leaf, then we need to check children.
	if ( !_isLeaf )
	{
		// recurse to children.
		_children[ ES_FRONT ]->GetPolygons( polygonIndices, boxMin, boxMax );
		_children[ ES_BACK  ]->GetPolygons( polygonIndices, boxMin, boxMax );
	}
	else
	{
		// if we're a leaf and we intersect with the bounds
		// passed in, then we should add our polygon to the
		// list.
		polygonIndices.Push() = _polygon;
	}
}

//----------------------------------------------------------
void*
GrPolygonBINode::operator new( size_t size )
{
	B_ASSERT( size == sizeof( GrPolygonBINode ) );
	return _allocator.AllocMem();
}

//----------------------------------------------------------
void
GrPolygonBINode::operator delete( void* mem )
{
	// no need to do anything since the memory is owned by the
	// block allocator.
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBINode::BuildTree( unsigned int& depth, GrPolygonBITree* parent, const GrPolygonGroup& group,
							unsigned int* indices, unsigned int polyCount )
{
	// this is a recursion counter for our constructor.
	static unsigned int treeDepth = 0;

	// increase the tree depth counter.
	++treeDepth;
	depth = Max( depth, treeDepth );

	// clear the children.
	_children[ 0 ] = 0;
	_children[ 1 ] = 0;

	// iterate through all polygons to determine how we should split them.
	float maxAxis[ 3 ] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	float minAxis[ 3 ] = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
	double invPolyCount = 1.0 / polyCount;
	double center[ 3 ] = { 0.0, 0.0, 0.0 };
	for ( unsigned int p = 0; p < polyCount; ++p )
	{
		// get the current polygon's index.
		unsigned int curIdx = indices[ p ];

		// iterate through the current polygon's vertex data.
		const GrPolygon& curPolygon = group.GetPolygon( curIdx );
		unsigned int vertCount = curPolygon.GetVertexCount();
		for ( unsigned int v = 0; v < vertCount; ++v )
		{
			// get the current vertex and check it's dimensions.
			const SVec3& curVertex = curPolygon.GetVertex( v ).position;

			// calculate the min/max for the current group of polygons.
			minAxis[ EA_X ] = Min( minAxis[ EA_X ], curVertex.x );
			minAxis[ EA_Y ] = Min( minAxis[ EA_Y ], curVertex.y );
			minAxis[ EA_Z ] = Min( minAxis[ EA_Z ], curVertex.z );
			maxAxis[ EA_X ] = Max( maxAxis[ EA_X ], curVertex.x );
			maxAxis[ EA_Y ] = Max( maxAxis[ EA_Y ], curVertex.y );
			maxAxis[ EA_Z ] = Max( maxAxis[ EA_Z ], curVertex.z );
		}

		// calculate the center point.
		center[ 0 ] += invPolyCount * curPolygon.GetVertex( 0 ).position.x;
		center[ 1 ] += invPolyCount * curPolygon.GetVertex( 0 ).position.y;
		center[ 2 ] += invPolyCount * curPolygon.GetVertex( 0 ).position.z;
	}

	// split along the longest axis.
	float xExt = maxAxis[ EA_X ] - minAxis[ EA_X ];
	float yExt = maxAxis[ EA_Y ] - minAxis[ EA_Y ];
	float zExt = maxAxis[ EA_Z ] - minAxis[ EA_Z ];
	if ( xExt > yExt )
	{
		if ( xExt > zExt )
			_boundingAxis = EA_X;
		else
			_boundingAxis = EA_Z;
	}
	else
	{
		if ( yExt > zExt )
			_boundingAxis = EA_Y;
		else
			_boundingAxis = EA_Z;
	}

	// store the bounding interval.
	_min = minAxis[ _boundingAxis ];
	_max = maxAxis[ _boundingAxis ];
	float axisCenter = ( float )center[ _boundingAxis ];

	// if there is more than one polygon, split the group.
	if ( polyCount > 1 )
	{
		// now split the group.  Polygons are classified by their first
		// vertex.  Using the center would probably give a cleaner split,
		// but this is faster to implement and leads to better build
		// times.
		unsigned int back = polyCount - 1;
		unsigned int front = 0;
		for ( unsigned int classified = 0; classified < polyCount; ++classified )
		{
			// get the current polygon's index.
			unsigned int curIdx = indices[ front ];

			// get the first vertex from the current polygon.
			const GrPolygon& curPolygon = group.GetPolygon( curIdx );
			const SVec3& curPos = curPolygon.GetVertex( 0 ).position;

			// determine what side of the split the current vertex is on.
			// If the polygon is on the front, keep it in place, otherwise
			// swap with the back and advance p.
			if ( curPos[ _boundingAxis ] < axisCenter )
			{
				Swap( indices[ front ], indices[ back ] );
				--back;
			}
			else
			{
				++front;
			}
		}

		// calculate the number of polygons behind the split plane.
		unsigned int backCount = polyCount - front;

		// if either the front or the back is 0, then simply grab one from the
		// other buckets.  This will avoid some bad cases where we would
		// otherwise end up with some infinite recursion.
		if ( backCount == 0 )
		{
			B_ASSERT( front > 1 );
			unsigned int swap = front / 2;
			back -= swap;
			front -= swap;
			backCount += swap;
		}
		if ( front == 0 )
		{
			B_ASSERT( backCount > 1 );
			unsigned int swap = backCount / 2;
			back += swap;
			front += swap;
			backCount -= swap;
		}

		B_ASSERT( ( backCount + front ) == polyCount );

		// recurse for children.
		if ( backCount > 0 )
			_children[ ES_BACK	] = new GrPolygonBINode( depth, parent, group, indices + front, backCount );
		if ( front > 0 )
			_children[ ES_FRONT ] = new GrPolygonBINode( depth, parent, group, indices, front );
	}
	else
	{
		// there should only be one polygon here.  Nodes must always contain
		// either 1 or 2 children, or 1 polygon.
		B_ASSERT( polyCount == 1 );

		// store the polygon's index.
		_polygon = indices[ 0 ];
		_isLeaf = 1;
	}

	// decrease the tree depth counter.
	--treeDepth;
}
