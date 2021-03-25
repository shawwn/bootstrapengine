//----------------------------------------------------------
// File:		EdBITreeInterval.h
// Author:		Kevin Bray
// Created:		08-15-08
//
// Purpose:		To define a node in a bounding interval.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrTypes.h"
#include "UBlockAllocator.h"

// project includes.
#include "EdSimplePolygon.h"

// forward declarations.
class EdBITreeStack;
class EdBITree;
class EdSimplePolygonGroup;
class MAABox;
class MVec3;

//**********************************************************
// class EdBITreeInterval
//**********************************************************
class EdBITreeInterval
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

	struct SRayIntersectInfo
	{
		unsigned int polygonIdx;
		EdSimplePolygon::SRayIntersectInfo polyHitInfo;
	};

	EdBITreeInterval( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group );
	EdBITreeInterval( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group,
					  unsigned int* indices, unsigned int polyCount );
	~EdBITreeInterval();

	// ray casting.  This function simply returns the index of the
	// first polygon hit.
	bool				CastRay( SRayIntersectInfo& hitInfo, const EdSimplePolygonGroup& group,
								 const SVec3& start, const SVec3& dir, EdBITreeStack& stack, float rayLength );

	// finds the best node to use as the root node for multiple
	// casting operations.
	EdBITreeInterval*	FindBestCastRoot( const MAABox& box );
	EdBITreeInterval*	FindBestCastRootFast( const SVec3& boxMin, const SVec3& boxMax );

	// returns true if the box passed in intersects with any geometry
	// in the bi-tree.
	bool				Intersects( const MAABox& box );

	void*				operator new( size_t );
	void				operator delete( void* mem );

private:
	static UBlockAllocator	_allocator;
	static unsigned int		_instanceCount;

	void				BuildTree( unsigned int& depth, EdBITree* parent, const EdSimplePolygonGroup& group,
								   unsigned int* indices, unsigned int polyCount );
	bool				WalkBack( SRayIntersectInfo& hitInfo, EdBITreeStack& stack, EdBITreeInterval* root,
								  const EdSimplePolygonGroup& polygons, SVec3 curStart, SVec3 curEnd, const SVec3& start,
								  const SVec3& dir );
	bool				IntersectRay( SVec3* start, SVec3* end, const SVec3& dir );
	bool				IntersectsAABB( const MVec3& boxCenter, const MVec3& boxMin, const MVec3& boxMax );

	// child and polygon info.
	union
	{
		EdBITreeInterval*	_children[ 2 ];
		unsigned int		_polygon;
		EdBITreeInterval*	_nextFree;
	};

	// split plane info.
	EAXIS				_boundingAxis;
	unsigned short		_isLeaf;
	float				_min;
	float				_max;
};
