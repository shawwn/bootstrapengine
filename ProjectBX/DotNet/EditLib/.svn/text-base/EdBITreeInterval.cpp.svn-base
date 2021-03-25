//----------------------------------------------------------
// File:		EdBITreeInterval.cpp
// Author:		Kevin Bray
// Created:		08-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBITreeInterval.h"

// project includes.
#include "EdBITreeStack.h"

// engine includes.
#include "EdSimplePolygonGroup.h"
#include "EdSimplePolygon.h"
#include "MAABox.h"
#include "MVec3.h"

UBlockAllocator
EdBITreeInterval::_allocator( sizeof( EdBITreeInterval ), 4096 );

unsigned int
EdBITreeInterval::_instanceCount = 0;


//**********************************************************
// class EdOctreeNode
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBITreeInterval::EdBITreeInterval( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group )
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
EdBITreeInterval::EdBITreeInterval( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group,
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
EdBITreeInterval::~EdBITreeInterval()
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
bool
EdBITreeInterval::CastRay( SRayIntersectInfo& hitInfo, const EdSimplePolygonGroup& group,
						   const SVec3& start, const SVec3& dir, EdBITreeStack& stack, float rayLength )
{
// OPTIMIZATION NOTES: The microcode instruction cache is approximately 16K.
// We need to make sure that all of this code can fit into that limitation.
	// current polygon hits.
	unsigned int polyHit = ~0;
	float polyDist = FLT_MAX;

	// calculate the initial start and end of the.  We use these as scratch
	// for ray-bounds intersection.
	SVec3 curStart = start;
	SVec3 curEnd = curStart + rayLength * dir;

	// fill the stack.
	bool hit = WalkBack( hitInfo, stack, this, group, curStart, curEnd, start, dir );

// OPTIMIZATION NOTES: This loop can be unrolled easily using a switch statement.
// The main benefit of this will be to free up a slot or two in the BTB.  Also
// a potentially larger gain would be to vectorize this using SSE.  That could
// lead to some very large gains for larger tree structures.
	// now unwind the stack.
	while ( stack.GetItemCount() > 0 )
	{
		// now pop the top item off of the stack.
		const EdBITreeStack::SEntry& top = stack.Pop();

// OPTIMIZATION NOTES: Are leaves ever pushed onto the stack?  Doesn't WalkBack()
// deal with leaves explicitly?
		// check for a front fracing node and handle it.
		EdBITreeInterval* curNode = top.interval;
		if ( curNode->_children[ ES_FRONT ] )
		{
			if ( WalkBack( hitInfo, stack, curNode->_children[ ES_FRONT ], group,
				top.start, top.end, start, dir ) )
			{
				hit = true;
			}
		}
	}

	// return the polygon that was hit.
	return hit;
}

//----------------------------------------------------------
EdBITreeInterval*
EdBITreeInterval::FindBestCastRoot( const MAABox& box )
{
	// calculate the box's extents.
	const MVec3& center = box.GetCenter();
	SVec3 minCorner = center - box.GetHalfExts();
	SVec3 maxCorner = center + box.GetHalfExts();

	// find the best root for ray casting.
	return FindBestCastRootFast( minCorner, maxCorner );
}

//----------------------------------------------------------
EdBITreeInterval*
EdBITreeInterval::FindBestCastRootFast( const SVec3& boxMin, const SVec3& boxMax )
{
	// Algorithm notes:
	//	This function finds the first node whose children both
	//	intersect the bounds.

	// check to see if the box passed in fits inside of the
	// bounds.
	float p0 = boxMin[ _boundingAxis ];
	float p1 = boxMax[ _boundingAxis ];
	if ( p1 < _min )
		return 0;
	if ( p0 > _max )
		return 0;

	// if we're not a leaf, then we need to check children.
	if ( !_isLeaf )
	{
		// check for intersection with the children.
		EdBITreeInterval* first = _children[ 0 ]->FindBestCastRootFast( boxMin, boxMax );
		EdBITreeInterval* second = _children[ 1 ]->FindBestCastRootFast( boxMin, boxMax );

		// if both first and second are non-null, then we need
		// to return our this pointer.  Note that I do the
		// multiplication to avoid the && operator which can
		// generate branching internally due to short-circuit
		// evaluation.
		if ( ( ( size_t )first * ( size_t )second ) != 0 )
			return this;

		// return either null or the node that was non-null.
		// Note that I do this addition to avoid branching.
		// cases:
		//  0 + second	= second
		//  first + 0	= first
		//  0 + 0		= 0
		return ( EdBITreeInterval* )( ( size_t )first + ( size_t )second );
	}

	// if we're a leaf, then simply return our 'this' pointer.
	return this;
}

//----------------------------------------------------------
bool
EdBITreeInterval::Intersects( const MAABox& box )
{
	// calculate the box's extents.
	const MVec3& center = box.GetCenter();
	MVec3 minCorner = center - box.GetHalfExts();
	MVec3 maxCorner = center + box.GetHalfExts();

	// determine if we intersect.
	return IntersectsAABB( center, minCorner, maxCorner );
}

//----------------------------------------------------------
void*
EdBITreeInterval::operator new( size_t size )
{
	B_ASSERT( size == sizeof( EdBITreeInterval ) );
	return _allocator.AllocMem();
}

//----------------------------------------------------------
void
EdBITreeInterval::operator delete( void* mem )
{
	// no need to do anything since the memory is owned by the
	// block allocator.
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdBITreeInterval::BuildTree( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group,
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
		const EdSimplePolygon& curPolygon = group.GetPolygon( curIdx );
		unsigned int vertCount = curPolygon.GetVertexCount();
		for ( unsigned int v = 0; v < vertCount; ++v )
		{
			// get the current vertex and check it's dimensions.
			const SVec3& curVertex = curPolygon.GetVertex( v );

			// calculate the min/max for the current group of polygons.
			minAxis[ EA_X ] = Min( minAxis[ EA_X ], curVertex.x );
			minAxis[ EA_Y ] = Min( minAxis[ EA_Y ], curVertex.y );
			minAxis[ EA_Z ] = Min( minAxis[ EA_Z ], curVertex.z );
			maxAxis[ EA_X ] = Max( maxAxis[ EA_X ], curVertex.x );
			maxAxis[ EA_Y ] = Max( maxAxis[ EA_Y ], curVertex.y );
			maxAxis[ EA_Z ] = Max( maxAxis[ EA_Z ], curVertex.z );
		}

		// calculate the center point.
		center[ 0 ] += invPolyCount * curPolygon.GetCenter().x;
		center[ 1 ] += invPolyCount * curPolygon.GetCenter().y;
		center[ 2 ] += invPolyCount * curPolygon.GetCenter().z;
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
			const EdSimplePolygon& curPolygon = group.GetPolygon( curIdx );
			const SVec3& curPos = curPolygon.GetCenter();

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
			_children[ ES_BACK	] = new EdBITreeInterval( depth, parent, group, indices + front, backCount );
		if ( front > 0 )
			_children[ ES_FRONT ] = new EdBITreeInterval( depth, parent, group, indices, front );
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

//----------------------------------------------------------
bool
EdBITreeInterval::WalkBack( SRayIntersectInfo& hitInfo, EdBITreeStack& stack, EdBITreeInterval* root,
							const EdSimplePolygonGroup& polygons, SVec3 curStart, SVec3 curEnd, const SVec3& start,
							const SVec3& dir )
{
	// push nodes on to the stack.
	EdBITreeInterval* curNode = root;
	while ( curNode )
	{
		// verify that the ray intersects with the region in the current
		// node.  If it doesn't, then we can stop traversal.
		if ( curNode->IntersectRay( &curStart, &curEnd, dir ) )
		{
			// check to see if we have a leaf.
			if ( !curNode->_isLeaf )
			{
				// push the current node on to the stack.
				EdBITreeStack::SEntry& curEntry = stack.Push();
				curEntry.start = curStart;
				curEntry.end = curEnd;
				curEntry.interval = curNode;
			}
			else
			{
				// check the current polygon for an intersection.
				EdSimplePolygon::SRayIntersectInfo curHitInfo;
				const EdSimplePolygon& polygon = polygons.GetPolygon( curNode->_polygon );
				if ( polygon.Intersect( curHitInfo, start, dir, 1024.0f ) )
				{
					// if the intersection is closer than the previous intersection,
					// overwrite it.
					if ( curHitInfo.hitDist <= hitInfo.polyHitInfo.hitDist )
					{
						hitInfo.polygonIdx = curNode->_polygon;
						hitInfo.polyHitInfo = curHitInfo;
						curEnd = curHitInfo.hitLoc;
						return true;
					}
				}
				return false;
			}
		}
		else
		{
			break;
		}

		// get the next node.
		curNode = curNode->_children[ ES_BACK ];
	}

	// return false since we didn't hit anything.
	return false;
}

#if defined( _MSC_VER ) && 0
//----------------------------------------------------------
bool
EdBITreeInterval::IntersectRay( SVec3* start, SVec3* end, const SVec3& dir )
{
// OPTIMIZATION NOTES: This function will most likely benefit heavily from
// being rewritten in assembly.  Also, this function will benefit heavily
// by being inlined due to store forwarding stalls on parameter passing.
// I've observed that the compiler tends to do this inlining for us.

// OPTIMIZATION NOTES:  can this branch be eliminated?
	int retVal = 1;
	int boundingAxis = _boundingAxis;
	float* tmin = &_min;
	float* tmax = &_max;
	float* vstart = start->v;
	float* vend = end->v;
	const float* vdir = dir.v;
	__asm
	{
		; ecx = _boundingAxis
		; eax = start
		; ebx = end

		mov		ecx, boundingAxis
		mov		eax, start
		mov		ebx, end
		add		ecx, ecx
		add		ecx, ecx
		movss	xmm0, [eax+ecx]		; load the starting position.
		movss	xmm1, [ebx+ecx]		; load the ending position.

		comiss	xmm0, xmm1			; if ( start[ _boundingAxis ] > end[ _boundingAxis ] )
		cmovg	eax, end				; swap eax and ebx
		cmovg	ebx, start
		movss	xmm0, [eax+ecx]		; reload the starting position.
		movss	xmm1, [ebx+ecx]		; reload the ending position.

		mov		ecx, tmax
		movss	xmm2, [ecx]
		comiss	xmm0, xmm2			; if ( start[ _boundingAxis ] > _max )
		ja		$end					; return

		mov		ecx, tmin
		movss	xmm3, [ecx]
		comiss	xmm1, xmm3			; if ( end[ _boundingAxis ] < _min )
		jb		$end					; return

		pxor	xmm4, xmm4
		subss	xmm3, xmm0			; _min - start->v[ _boundingAxis ];
		subss	xmm2, xmm1			; _max - end->v[ _boundingAxis ];
		maxss	xmm3, xmm4 			; xmm3 = Max( xmm3, 0 )
		maxss	xmm4, xmm2 			; xmm4 = Max( xmm2, 0 )

		; clamp the end point
		mov		eax, vdir;
		mov		ebx, vend;
		mov		ecx, vstart;

		; adjust the end point.
		movss	xmm0, [eax+0]
		movss	xmm1, [eax+4]
		movss	xmm2, [eax+8]
		mulss	xmm0, xmm4
		mulss	xmm1, xmm4
		mulss	xmm2, xmm4
		addss	xmm0, [ebx+0]
		addss	xmm1, [ebx+4]
		addss	xmm2, [ebx+8]
		movd	[ebx+0], xmm0
		movd	[ebx+4], xmm1
		movd	[ebx+8], xmm2

		; clamp the start point
		movss	xmm0, [eax+0]
		movss	xmm1, [eax+4]
		movss	xmm2, [eax+8]
		mulss	xmm0, xmm3
		mulss	xmm1, xmm3
		mulss	xmm2, xmm3
		addss	xmm0, [ecx+0]
		addss	xmm1, [ecx+4]
		addss	xmm2, [ecx+8]
		movd	[ecx+0], xmm0
		movd	[ecx+4], xmm1
		movd	[ecx+8], xmm2

		xor		eax, eax
		mov		retVal, eax;

$end:
	}

	return !retVal;
/*
	returnValue = true;

// OPTIMIZATION NOTES:  This is most likely unoptimizable.  However, most
// rays might not be clipped... I'm really not sure which is a better guess
// here.
	// if we got here, then the segment is inside of our bounds. Therefor,
	// we need to clip the line.
	if ( end->v[ _boundingAxis ] > _max )
	{
		float adj = _max - end->v[ _boundingAxis ];
		*end += adj * dir;
	}
	if ( start->v[ _boundingAxis ] < _min )
	{
		float adj = _min - start->v[ _boundingAxis ];
		*start += adj * dir;
	}

$end:
	// return true to indicate that the segment intersects with this
	// region.
	return returnValue;
*/
}
#else
//----------------------------------------------------------
bool
EdBITreeInterval::IntersectRay( SVec3* start, SVec3* end, const SVec3& dir )
{
// OPTIMIZATION NOTES: This function will most likely benefit heavily from
// being rewritten in assembly.  Also, this function will benefit heavily
// by being inlined due to store forwarding stalls on parameter passing.
// I've observed that the compiler tends to do this inlining for us.

// OPTIMIZATION NOTES:  can this branch be eliminated?
	// order the end points from min to max.
	if ( start->v[ _boundingAxis ] > end->v[ _boundingAxis ] )
		Swap( start, end );

// OPTIMIZATION NOTES:  These are uncommon cases.  This is likely going
// to be a source of branch mispredictions.
	// determine if the segment is entirely outside of our bounds.
	if ( end->v[ _boundingAxis ] < _min )
		return false;
	if ( start->v[ _boundingAxis ] > _max )
		return false;

// OPTIMIZATION NOTES:  This is most likely unoptimizable.  However, most
// rays might not be clipped... I'm really not sure which is a better guess
// here.
	// if we got here, then the segment is inside of our bounds. Therefor,
	// we need to clip the line.
	if ( end->v[ _boundingAxis ] > _max )
	{
		float adj = _max - end->v[ _boundingAxis ];
		*end += adj * dir;
	}
	if ( start->v[ _boundingAxis ] < _min )
	{
		float adj = _min - start->v[ _boundingAxis ];
		*start += adj * dir;
	}

	// return true to indicate that the segment intersects with this
	// region.
	return true;
}
#endif

//----------------------------------------------------------
bool
EdBITreeInterval::IntersectsAABB( const MVec3& boxCenter, const MVec3& boxMin, const MVec3& boxMax )
{
	// determine which side(s) the box resides on.
	float center = boxCenter[ _boundingAxis ];
	float p0 = boxMin[ _boundingAxis ];
	float p1 = boxMax[ _boundingAxis ];

	// are we outside of the bounds?
	if ( p1 < _min )
		return false;
	if ( p0 > _max )
		return false;

	// recurse.
	if ( !_isLeaf )
	{
		// choose which side to recurse first.  If we intersect
		// with one side, then we don't need to check the other
		// side.
		float biCenter2x = _min + _max;
		float boxCenter2x = center + center;

		// if the AABB's center is less than the interval's
		// center then check the min side, followed by the max
		// side.  Otherwise, we check the max side followed by
		// the min side.
		int recurse0 = ( boxCenter2x > biCenter2x );
		int recurse1 = recurse0 ^ 1;

		// check for intersection with the children.
		if ( _children[ recurse0 ]->IntersectsAABB( boxCenter, boxMin, boxMax ) )
			return true;
		if ( _children[ recurse1 ]->IntersectsAABB( boxCenter, boxMin, boxMax ) )
			return true;

		// return false to indicate that the AABB does not
		// intersect with anything in the tree.
		return false;
	}

	// if we're a leaf, then simply return true.
	return true;
}
