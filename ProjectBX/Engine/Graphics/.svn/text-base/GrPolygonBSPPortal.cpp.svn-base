//----------------------------------------------------------
// File:		GrPortal.cpp
// Author:		Kevin Bray
// Created:		09-12-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBSPPortal.h"

// project includes.
#include "GrPolygonGroup.h"
#include "UReader.h"
#include "UWriter.h"


//**********************************************************
// class GrPortal
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPortal::GrPortal( UReader& reader )
{
	Load( reader );
}

//----------------------------------------------------------
GrPortal::GrPortal( const MPlane& plane, const VertexArray& polygon )
: _polygon( polygon )
{
	// ensure that we have a valid polygon!
	B_ASSERT( polygon.GetElemCount() >= 3 );

	// reproject the polygon to the plane passed in to reduce error.
	unsigned int vertexCount = polygon.GetElemCount();
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		float dist = plane.Dist( _polygon[ i ] );
		_polygon[ i ] -= dist * plane.GetNormal();
		B_ASSERT( plane.Dist( _polygon[ i ] ) < 0.1f );
	}
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
bool
GrPortal::IsCoplanar( const MPlane& plane, float epsilon ) const
{
	// if all points lie within an acceptible distance of the plane,
	// then the portal is coplanar.
	unsigned int vertexCount = GetVertexCount();
	B_ASSERT( vertexCount >= 3 );
	for ( unsigned int i = 2; i < vertexCount; ++i )
	{
		// too far from the plane?  if so, return false.
		if ( Abs( plane.Dist( _polygon[ i ] ) ) > epsilon )
			return false;
	}

	// success!
	return true;
}

//----------------------------------------------------------
bool
GrPortal::IsDegenerate() const
{
	// fan the polygon and calculate area for each triangle.
	float area = 0.0f;
	unsigned int vertexCount = GetVertexCount();
	B_ASSERT( vertexCount >= 3 );
	for ( unsigned int i = 2; i < vertexCount; ++i )
	{
		// get edge vectors of the current triangle.
		MVec3 edge0 = _polygon[ i - 1 ] - _polygon[ i - 2 ];
		MVec3 edge1 = _polygon[ i - 0 ] - _polygon[ i - 2 ];

		// skip this triangle if one of the edges is 0 length as it obviously has
		// no area.
		if ( edge0.MagSqr() <= 0.0001f || edge1.MagSqr() <= 0.0001f )
			continue;

		// calculate the area of the current triangle.
		float sinTheta = edge0.Normalized().Cross( edge1.Normalized() ).Mag();
		area += 0.5f * Abs( sinTheta ) * edge0.Mag() * edge1.Mag();
	}

	return area < 0.1f;
}

//----------------------------------------------------------
int
GrPortal::Split( GrPortal& front, GrPortal& back, const MPlane& splitPlane, const MPlane& orgPlane, float epsilon ) const
{
	// make sure the output portals are clear.
	front._polygon.Clear();
	back._polygon.Clear();

	// don't use an epsilon on the first one.  We need an absolute side here.
	unsigned int vertexCount = GetVertexCount();
	B_ASSERT( vertexCount >= 3 );

	int lastSide = 0;
	unsigned int vert = 0;
	while ( lastSide == 0 && vert < vertexCount )
		lastSide = splitPlane.GetSide( _polygon[ vert++ ], epsilon );

	// if we're still on the plane, then ALL points are on the plane!  Therefor,
	// just consider it as on the front side and be done with it.
	if ( lastSide == 0 )
	{
		front = *this;
		back = *this;
		return 0;
	}

	// not all on the plane, therefor, we should check for any vertices on
	// opposing sides of the plane.
//	unsigned int frontVertCount = 0;
//	unsigned int backVertCount = 0;
//	unsigned int maxArraySize = vertexCount + 10;
	for ( unsigned int i = 1; i <= vertexCount; ++i )
	{
		unsigned int curVert = i % vertexCount;

		// determine the side of the current point.
		int side = splitPlane.GetSide( _polygon[ curVert ], epsilon );
		vert = 0;
		while ( side == 0 && vert < vertexCount )
		{
			side = splitPlane.GetSide( _polygon[ ( curVert + vert ) % vertexCount ], epsilon );
			++vert;
		}

		// should be impossible.
		B_ASSERT( side != 0 );

		// if the side of the plane has changed, and side is not 0, then -we
		// need to generate a new vertex at the point of intersection with the
		// plane.
		if ( side != lastSide )
		{
			unsigned int prevVert = i - 1;
			float d0 = splitPlane.Dist( _polygon[ prevVert ] );
			float d1 = splitPlane.Dist( _polygon[ curVert ] );

			// if the current vertex's side was within an acceptible tolerance
			// level, simply use it as the split.  Since it will already be on
			// the opposite side, we only need to add it to the new side.
			if ( Abs( d1 ) < epsilon )
			{
				// add the new vertex to both front and back lists.
				front._polygon.Push( _polygon[ curVert ] );
				back._polygon.Push( _polygon[ curVert ] );
				lastSide = side;

				// continue forward.  Since we're using the current vertex as a split,
				// we have no reason to add the vertex to it's current side.
				continue;
			}
			else
			{
				// we found a side change.  We need to generate a new vertex and
				// add it to both lists.
				float t = Abs( d0 ) / ( Abs( d0 ) + Abs( d1 ) );
				MVec3 newVert = Lerp( _polygon[ prevVert ], _polygon[ curVert ], t );

				// add the new vertex to both front and back lists.
				front._polygon.Push( newVert );
				back._polygon.Push( newVert );
			}

			// set the side.
			lastSide = side;
		}

		// add the current vertex to the correct side.
		if ( side == 1 )
		{
			front._polygon.Push( _polygon[ curVert ] );
		}
		else
		{
			B_ASSERT( side == -1 );
			back._polygon.Push( _polygon[ curVert ] );
		}
	}

	// this shouldn't be possible: we either have a full polygon or we don't.
	// there should be no grey area here.
	B_ASSERT( front._polygon.GetElemCount() >= 3 || front._polygon.GetElemCount() == 0 );
	B_ASSERT( back._polygon.GetElemCount() >= 3 || back._polygon.GetElemCount() == 0 );

	// if we have 3 or more vertices on a side of the plane, then we need
	// to create a new polygon.
	int ret = 0;
	if ( front._polygon.GetElemCount() )
		ret += 1;
	if ( back._polygon.GetElemCount() )
		ret -= 1;

	// return 1 for front, -1 for back, 0 for both.
	return ret;
/*
	VertexArray frontArray;
	VertexArray backArray;

	// split the polygon.
	int curSide = 0;
	const unsigned int vertexCount = _polygon.GetElemCount();
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		MVec3 v0 = _polygon[ i ];
		MVec3 v1 = _polygon[ ( i + 1 ) % vertexCount ];
		float d0 = splitPlane.Dist( v0 );
		float d1 = splitPlane.Dist( v1 );

		// add to both if necessary.
		if ( Abs( d0 ) < epsilon )
		{
			frontArray.Push( v0 );
			backArray.Push( v0 );
			continue;
		}

		if ( d0 > 0.0f )
			frontArray.Push( v0 );
		else
			backArray.Push( v0 );

		// are we intersecting?
		if ( ( d0*d1 ) < 0.0f )
		{
			d0 = Abs( d0 );
			d1 = Abs( d1 );
			float t = d0 / ( d0 + d1 );
			MVec3 pt = Lerp( v0, v1, t );
			frontArray.Push( pt );
			backArray.Push( pt );
		}
	}

	int ret = 0;
	if ( frontArray.GetElemCount() >= 3 )
		ret += 1;
	if ( backArray.GetElemCount() >= 3 )
		ret -= 1;

	// return the
	return ret;
*/
}

//----------------------------------------------------------
void
GrPortal::ReverseWinding()
{
	B_ASSERT( _polygon.GetElemCount() >= 3 );
	unsigned int vertexCount = _polygon.GetElemCount();
	unsigned int loopCount = vertexCount >> 1;
	for ( unsigned int i = 0, j = vertexCount-1; i < loopCount; ++i, --j )
		Swap( _polygon[ i ], _polygon[ j ] );
}

//----------------------------------------------------------
unsigned int
GrPortal::GetVertexCount() const
{
	B_ASSERT( _polygon.GetElemCount() >= 3 );
	return _polygon.GetElemCount();
}

//----------------------------------------------------------
const MVec3&
GrPortal::GetVertex( unsigned int idx ) const
{
	B_ASSERT( _polygon.GetElemCount() >= 3 );
	B_ASSERT( idx < _polygon.GetElemCount() );
	return _polygon[ idx ];
}
/*
//----------------------------------------------------------
void
GrPortal::CheckWinding( const GrPolygonGroup& group, float epsilon )
{
	unsigned int polygonCount = group.GetPolygonCount();
	for ( unsigned int i = 0; i < polygonCount; ++i )
	{
		// no triangles should be behind the portal!  Otherwise, the portal is broken.
		GrPolygon::EPLANERELATION planeRel = group.GetPolygon( i ).CalcPlaneRelation( _plane, epsilon );
		if ( planeRel != GrPolygon::EPR_FRONT && planeRel != GrPolygon::EPR_COPLANAR )
		{
			B_ASSERT( planeRel != GrPolygon::EPR_SPLIT );
			_polygon.ReverseWinding();
			break;
		}
	}
}
*/
//----------------------------------------------------------
void
GrPortal::Save( UWriter& writer )
{
	// write out our vertices.
	const unsigned int vertexCount = _polygon.GetElemCount();
	writer.WriteUInt( vertexCount );
	for ( unsigned int i = 0; i < vertexCount; ++i )
		_polygon[ i ].Save( writer );
}

//----------------------------------------------------------
void
GrPortal::Load( UReader& reader )
{
	// read in our vertices.
	const unsigned int vertexCount = reader.ReadUInt();
	for ( unsigned int i = 0; i < vertexCount; ++i )
		_polygon.Push().Load( reader );
}


//==========================================================
// private methods.
//==========================================================

