//----------------------------------------------------------
// File:		EdSimplePolygon.cpp
// Author:		Kevin Bray
// Created:		08-18-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdSimplePolygon.h"

// project includes.
#include "EdSimplePolygonGroup.h"

// engine includes.
#include "MMat3x3.h"

inline float FSign( float v )
{
	if ( v >= 0.0f )
		return 1.0f;
	else 
		return -1.0f;
}

inline SVec2 SignedSqr( const SVec2& v )
{
	return SVec2(
		FSign( v.x ) * v.x * v.x,
		FSign( v.y ) * v.y * v.y );
}

inline SVec2 SignedSqrt( const SVec2& v )
{
	return SVec2(
		FSign( v.x ) * Sqrt( Abs( v.x ) ),
		FSign( v.y ) * Sqrt( Abs( v.y ) ) );
}

inline SVec2 UnitizeZ( const SVec3& vec )
{
	return SVec2(
		vec.x / vec.z,
		vec.y / vec.z );
}

//**********************************************************
// class EdSimplePolygon
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdSimplePolygon::EdSimplePolygon()
: _group( 0 )
, _vertexCount( 0 )
{ 

}

//----------------------------------------------------------
EdSimplePolygon::EdSimplePolygon( EdSimplePolygonGroup* group, const unsigned int* vertexIndices,
								  unsigned int count )
: _group( group )
, _vertexCount( count )
{
	B_ASSERT( group != 0 );
	B_ASSERT( count >= 3 );
	B_ASSERT( count <= E_MAX_VERTEX_COUNT );

	// get the vertices.
	for ( unsigned int i = 0; i < count; ++i )
		_vertexIndices[ i ] = vertexIndices[ i ];

	// calculate the face normal.
	CalcFaceNormal();
	CalcCenter();
}

//----------------------------------------------------------
EdSimplePolygon::~EdSimplePolygon()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdSimplePolygon::SetVertices( EdSimplePolygonGroup* group, const unsigned int* vertexIndices,
							  unsigned int count )
{
	B_ASSERT( group != 0 );

	// store the new vertex count.
	_group = group;

	// copy the vertex indices.
	_vertexCount = count;
	for ( unsigned int i = 0; i < count; ++i )
		_vertexIndices[ i ] = vertexIndices[ i ];

	// calculate the face normal and the center point.
	CalcFaceNormal();
	CalcCenter();
}

//----------------------------------------------------------
void
EdSimplePolygon::GenNormals()
{
	for ( unsigned int i = 0; i < _vertexCount; ++i )
	{
		_normalIndices[ i ] = _vertexIndices[ i ];
		const SVec3& curNormal = _group->GetVertexNormal( _normalIndices[ i ] );
		_group->SetVertexNormal( _normalIndices[ i ], curNormal + _faceNormal );
	}
}

//----------------------------------------------------------
const SVec3&
EdSimplePolygon::GetVertex( unsigned int idx ) const
{
	B_ASSERT( idx < _vertexCount );
	return _group->GetVertex( _vertexIndices[ idx ] );
}

//----------------------------------------------------------
const SVec3&
EdSimplePolygon::GetVertexNormal( unsigned int idx ) const
{
	B_ASSERT( _group->HasVertexNormals() );
	B_ASSERT( idx < _vertexCount );
	return _group->GetVertexNormal( _normalIndices[ idx ] );
}

//----------------------------------------------------------
bool
EdSimplePolygon::Intersect( SRayIntersectInfo& info, const SVec3& rayStart,
							const SVec3& rayDir, float maxDist ) const
{
	// calculate the point on the triangle's plane along the ray.
	float d = rayDir.Dot( _faceNormal );

	// get the first position in the polygon.
	const SVec3& pt0 = GetVertex( 0 );

	// calculate the distance along the ray to the plane.
	SVec3 vec = rayStart - pt0;
	float distance = -vec.Dot( _faceNormal ) / d;
	if ( distance < 0.0f || distance > maxDist )
		return false;

	// using the face normal, find the dominant axis so we can perform
	// our triangle-ray tests in 2D.
	float maxAxis = _faceNormal.AbsMax();
	int domAxis = 0;

	// we only need to go through this loop twice... if we fail the
	// first two times, then we know the third axis is the dominant
	// one.
	for ( ; domAxis < 2; ++domAxis )
	{
		if ( Abs( maxAxis - Abs( _faceNormal[ domAxis ] ) ) < 0.0001f )
			break;
	}

	// choose an axis to throw away and solve in 2D.
	unsigned int a0 = ( domAxis == 0 ) ? 1 : 0;
	unsigned int a1 = ( domAxis <= 1 ) ? 2 : 1;

	// TODO: optimize this by converting it to 2D.
	// calculate where we intersect the plane the triangle lies on.
	SVec3 point = rayStart + distance * rayDir;
	SVec3 p = point - pt0;

	// now fan the polygon and perform ray tests with each triangle.
	float b[ 2 ];
	float c[ 2 ];
	float u = 0.0f;
	float v = 0.0f;
	unsigned int i = GetVertexCount();
	while ( i-- > 2 )
	{
		const SVec3& pt1 = GetVertex( i-1 );
		const SVec3& pt2 = GetVertex( i );

		// get the other points on the current triangle.
		b[ 0 ] = pt1[ a0 ] - pt0[ a0 ];
		b[ 1 ] = pt1[ a1 ] - pt0[ a1 ];
		c[ 0 ] = pt2[ a0 ] - pt0[ a0 ];
		c[ 1 ] = pt2[ a1 ] - pt0[ a1 ];
		
		// calculate the barycentric coordinates.
		u = ( p[a1]*c[0] - p[a0]*c[1] ) / ( b[1]*c[0] - b[0]*c[1] );
		v = ( p[a1]*b[0] - p[a0]*b[1] ) / ( c[1]*b[0] - c[0]*b[1] );

		// return true if we're inside the triangle.
		if ( u >= -0.0f && v >= -0.0f && ( u + v ) <= 1.0f )
		{
			info.hitDist = distance;
			info.hitLoc = point;
			info.triIdx = i - 2;
			info.u = u;
			info.v = v;
			return true;
		}
	}

	// if we got here, then we don't intersect with any triangles in
	// the polygon.
	return false;
}

//----------------------------------------------------------
void
EdSimplePolygon::CalcIntersectNormal( SVec3& normal, const SRayIntersectInfo& info ) const
{
	// if there are no vertex normals, simply return the face normal.
	if ( !_group->HasVertexNormals() )
	{
		normal = _faceNormal;
		return;
	}

	// now calculate the interpolated face normal.
	const SVec3& n0 = GetVertexNormal( 0 );
	const SVec3& n1 = GetVertexNormal( info.triIdx + 1 );
	const SVec3& n2 = GetVertexNormal( info.triIdx + 2 );

	// return the interpolated normal.
	normal = n0 + info.u * ( n1 - n0 ) + info.v * ( n2 - n0 );
	normal.Normalize();
}

//----------------------------------------------------------
void
EdSimplePolygon::CalcIntersectNormalHQ( SVec3& normal, const SRayIntersectInfo& info ) const
{
	CalcIntersectNormal( normal, info );
	return;
#if 0
	// if there are no vertex normals, simply return the face normal.
	if ( !_group->HasVertexNormals() )
	{
		normal = _faceNormal;
		return;
	}

	// now calculate the interpolated face normal.
	const SVec3& n0 = GetVertexNormal( 0 );
	const SVec3& n1 = GetVertexNormal( info.triIdx + 1 );
	const SVec3& n2 = GetVertexNormal( info.triIdx + 2 );

	// calculate the local coordinate frame.
	SVec3 right = GetVertex( 1 ) - GetVertex( 0 );
	MMat3x3 rot( right.Normalized(),
				 right.Cross( _faceNormal ).Normalized(),
				 _faceNormal );

	// now transform the vertex normals into the local coordinate frame.
	SVec2 localN0 = UnitizeZ( rot.RotatePoint( n0 ) );
	SVec2 localN1 = UnitizeZ( rot.RotatePoint( n1 ) );
	SVec2 localN2 = UnitizeZ( rot.RotatePoint( n2 ) );

	// square the local deltas and calculate the new local normal in 2D.
	localN1 = SignedSqr( localN1 );
	localN2 = SignedSqr( localN2 );
	SVec2 localN = localN0 + SignedSqrt( info.u * ( localN1 - localN0 ) ) + SignedSqrt( info.v * ( localN2 - localN0 ) );

	// now construct and return the interpolated normal.
	normal = SVec3( localN.x, localN.y, 1.0f );

	// now calculate the inverse coordinate frame and transform the normal
	rot.Transpose();
	normal = rot.RotatePoint( normal );
	normal.Normalize();
#endif
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdSimplePolygon::CalcFaceNormal()
{
	// calculate the actual plane of the surface.
	for ( unsigned int i = 2; i < _vertexCount; ++i )
	{
		// get the edges.
		SVec3 edge0 = GetVertex( i ) - GetVertex( i-1 );
		SVec3 edge1 = GetVertex( i ) - GetVertex( i-2 );
		if ( edge0.MagSqr() <= 0.0001f || edge1.MagSqr() <= 0.0001f )
			continue;

		// calculate the face normal.
		_faceNormal = edge1.Normalized().Cross( edge0.Normalized() );
		if ( _faceNormal.MagSqr() > 0.0001f )
		{
			_faceNormal.Normalize();
			return;
		}
	}

	// just use a default face normal.
	_faceNormal = SVec3( 0.0f, 1.0f, 0.0f );
}

//----------------------------------------------------------
void
EdSimplePolygon::CalcCenter()
{
	// calculate the actual plane of the surface.
	_center = GetVertex( 0 );
	for ( unsigned int i = 1; i < _vertexCount; ++i )
		_center += GetVertex( i );

	// take the new center position.
	_center *= 1.0f / _vertexCount;
}
