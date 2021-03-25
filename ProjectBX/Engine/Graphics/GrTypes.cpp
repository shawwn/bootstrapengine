//----------------------------------------------------------
// File:		GrTypes.cpp
// Author:		Kevin Bray
// Created:		10-04-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTypes.h"

// std c++ includes.
#ifdef _DEBUG
#include <math.h>
#endif
#include <float.h>

//**********************************************************
// struct SVec3
//**********************************************************

//----------------------------------------------------------
SVec2::SVec2( const MVec2& v )
: x( v.GetX() )
, y( v.GetY() )
{

}

//----------------------------------------------------------
float
SVec2::Normalize()
{
	// get the magnitude.
	float mag = Mag();
	B_ASSERT( mag > 0.0001f );

	// divide the components by the magnitude.
	float invM = 1.0f / mag;
	x *= invM;
	y *= invM;

	// return the magnitude prior to normalization.
	return mag;
}

//----------------------------------------------------------
SVec2
SVec2::Normalized() const
{
	// get the magnitude.
	float mag = Mag();
	B_ASSERT( mag > 0.0001f );

	// divide the components by the magnitude.
	float invM = 1.0f / mag;

	// return the normalized vector.
	return SVec2( x * invM, y * invM );
}

//----------------------------------------------------------
float
SVec2::InvMag() const
{
	float m = Mag();
	B_ASSERT( m > 0.0001f );
	return ( 1.0f / m );
}

//----------------------------------------------------------
float
SVec2::Dist( const SVec2& v ) const
{
	return Sqrt( DistSqr( v ) );
}

//----------------------------------------------------------
float
SVec2::DistSqr( const SVec2& v ) const
{
	float tx = x - v.x;
	float ty = y - v.y;
	return tx*tx + ty*ty;
}

//----------------------------------------------------------
SVec2&
SVec2::operator = ( const MVec2& v )
{
	x = v.GetX();
	y = v.GetY();
	return *this;
}

//----------------------------------------------------------
bool
SVec2::operator == ( const SVec2& v ) const
{
	return ( x == v.x && y == v.y );
}

//----------------------------------------------------------
bool
SVec2::operator != ( const SVec2& v ) const
{
	return ( x != v.x || y != v.y );
}

//**********************************************************
// struct SVec3
//**********************************************************

//----------------------------------------------------------
SVec3::SVec3( const MVec3& vec )
: x( vec.GetX() )
, y( vec.GetY() )
, z( vec.GetZ() )
{

}

//----------------------------------------------------------
SVec3&
SVec3::operator = ( const MVec3& vec )
{
	x = vec.GetX();
	y = vec.GetY();
	z = vec.GetZ();
	return *this;
}

//----------------------------------------------------------
bool
SVec3::operator == ( const SVec3& v ) const
{
	return ( x == v.x && y == v.y && z == v.z );
}

//----------------------------------------------------------
bool
SVec3::operator != ( const SVec3& v ) const
{
	return ( x != v.x || y != v.y || z != v.z );
}

//**********************************************************
// struct SByteVec3
//**********************************************************

SByteVec3::SByteVec3( float x, float y, float z )
: x( ClampValue( x ) )
, y( ClampValue( y ) )
, z( ClampValue( z ) )
{
}

//----------------------------------------------------------
SByteVec3::SByteVec3( const MVec3& vec )
: x( ClampValue( vec.GetX() ) )
, y( ClampValue( vec.GetY() ) )
, z( ClampValue( vec.GetZ() ) )
{
}

//----------------------------------------------------------
SByteVec3::SByteVec3( const SVec3& vec )
: x( ClampValue( vec.x ) )
, y( ClampValue( vec.y ) )
, z( ClampValue( vec.z ) )
{
}

//----------------------------------------------------------
SByteVec3&
SByteVec3::operator = ( const MVec3& vec )
{
	x = ClampValue( vec.GetX() );
	y = ClampValue( vec.GetY() );
	z = ClampValue( vec.GetZ() );
	return *this;
}

//----------------------------------------------------------
SByteVec3&
SByteVec3::operator = ( const SVec3& vec )
{
	x = ClampValue( vec.x );
	y = ClampValue( vec.y );
	z = ClampValue( vec.z );
	return *this;
}

//----------------------------------------------------------
bool
SByteVec3::operator == ( const SByteVec3& vec )
{
	return ( x == vec.x && y == vec.y && z == vec.z );
}

//----------------------------------------------------------
bool
SByteVec3::operator != ( const SByteVec3& vec )
{
	return ( x != vec.x || y != vec.y || z != vec.z );
}

//----------------------------------------------------------
unsigned char
SByteVec3::ClampValue( float v )
{
	float temp = Clamp( v * 0.5f + 0.5f, 0.0f, 1.0f );
	return ( unsigned char )( temp * 254.0f );
}

//----------------------------------------------------------
float
SByteVec3::ExpandValue( unsigned char v )
{
	return ( ( float( v ) / 254.0f ) * 2.0f ) - 1.0f;
}


//**********************************************************
// struct SLine
//**********************************************************

//----------------------------------------------------------
SLine::SLine()
{

}

//----------------------------------------------------------
SLine::SLine( const SVec3& pt0, const SVec3& pt1 )
: pt0( pt0 )
, pt1( pt1 )
{

}


//**********************************************************
// struct STriangle
//**********************************************************

//----------------------------------------------------------
STriangle::STriangle()
{

}

//----------------------------------------------------------
STriangle::STriangle( const SVec3& pt0, const SVec3& pt1, const SVec3& pt2 )
: pt0( pt0 )
, pt1( pt1 )
, pt2( pt2 )
{

}

//----------------------------------------------------------
bool
STriangle::IsDegenerate()
{
	return	( pt0 == pt1 ) || ( pt1 == pt2 ) || ( pt2 == pt0 );
}

//----------------------------------------------------------
bool
STriangle::IsDegenerate( float epsilon )
{
	// true if any of the vertices are equal.
	return	( Abs( ( MVec3( pt0 ) - pt1 ).AbsMax() ) < epsilon ) ||
			( Abs( ( MVec3( pt1 ) - pt2 ).AbsMax() ) < epsilon ) ||
			( Abs( ( MVec3( pt2 ) - pt0 ).AbsMax() ) < epsilon );
}

//**********************************************************
// struct SQuad
//**********************************************************

//----------------------------------------------------------
SQuad::SQuad()
{

}

//----------------------------------------------------------
SQuad::SQuad( const SVec3& pos, const SVec3& up, const SVec3& right, const SVec3& normal )
: pos( pos )
, up( up )
, right( right )
, normal( normal )
{

}
