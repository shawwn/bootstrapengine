//----------------------------------------------------------
// File:		MPlane.cpp
// Author:		Kevin Bray
// Created:		11-10-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MPlane.h"

// project includes.
#include "UReader.h"
#include "UWriter.h"
#include "MLine.h"
#include "MMat3x3.h"

// std c++ includes.
#include <math.h>

//**********************************************************
// class MPlane
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MPlane::MPlane()
: _normal( 0.0f, 1.0f, 0.0f )
, _d( 0.0f )
{

}

//----------------------------------------------------------
MPlane::MPlane( UReader& reader )
: _normal( 0.0f, 1.0f, 0.0f )
, _d( 0.0f )
{
	Load( reader );
}

//----------------------------------------------------------
MPlane::MPlane( const MVec3& normal, const MVec3& point )
: _normal( normal )
, _d( 0.0f )
{
	_normal.Normalize();
	_d = -_normal.Dot( point );
}

//----------------------------------------------------------
MPlane::MPlane( const MVec3& normal, float d )
: _normal( normal )
, _d( d )
{

}

//----------------------------------------------------------
MPlane::~MPlane()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MPlane::Bias( float amount )
{
	_d -= ( Sign( _d ) * amount );
}

//----------------------------------------------------------
MPlane
MPlane::Transform( const MMat4x4& transform, const MMat3x3& normalRot )
{
	// transform the plane into world space.
	MVec3 point = -_d * _normal;
	return MPlane( normalRot.RotatePoint( _normal ), point );
}

//----------------------------------------------------------
void
MPlane::TransformFast( const MMat4x4& transform, const MMat3x3& normalRot )
{
	// transform the plane into world space.
	MVec3 point = -_d * _normal;
	_normal = normalRot.RotatePoint( _normal );
	_d = -_normal.Dot( transform.TransformCoord( point ) );
}

//----------------------------------------------------------
void
MPlane::Translate( const MVec3& translate )
{
	MVec3 point = -_d * _normal;
	_d = -_normal.Dot( point + translate );
}

//----------------------------------------------------------
void
MPlane::Scale( float scale )
{
	_d *= scale;
}

//----------------------------------------------------------
MMat4x4
MPlane::BuildReflectMatrix() const
{
	float negD = -_d;
	MVec3 pos = negD * _normal;
	float nX = _normal.GetX();
	float nY = _normal.GetY();
	float nZ = _normal.GetZ();
	return MMat4x4( 1.0f - ( 2.0f*nX*nX ), -2.0f*nX*nY, -2.0f*nX*nZ, 2.0f*negD*nX,
					-2.0f*nX*nY, 1.0f - ( 2.0f*nY*nY ), -2.0f*nY*nZ, 2.0f*negD*nY,
					-2.0f*nX*nZ, -2.0f*nY*nZ, 1.0f - ( 2.0f*nZ*nZ ), 2.0f*negD*nZ,
					0.0f, 0.0f, 0.0f, 1.0f );
}

//----------------------------------------------------------
bool
MPlane::IsCoplanar( const MPlane& plane, float angleEpsilon, float dEpsilon ) const
{
	return ( ( 1.0 - Abs( _normal.Dot( plane._normal ) ) ) < angleEpsilon && Abs( _d - plane._d ) < dEpsilon );
}

//----------------------------------------------------------
int
MPlane::GetSide( const MVec3& pt, float epsilon ) const
{
	float dist = Dist( pt );
	if ( Abs( dist ) < epsilon )
		return 0;
	return dist >= 0 ? 1 : -1;
}

//----------------------------------------------------------
bool
MPlane::GetIntersect( MLine& line, const MPlane& plane ) const
{
	float n00 = _normal.MagSqr();
	float n01 = _normal.Dot( plane.GetNormal() );
	float n11 = plane.GetNormal().MagSqr();
	float det = n00 * n11 - n01 * n01;
	const float kError = 1e-06f;
	if ( Abs( det ) < kError )
	    return false;

	float invDet = 1.0f/det;
    float c0 = ( n11 * _d - n01 * plane._d ) * invDet;
    float c1 = ( n00 * plane._d - n01 * _d ) * invDet;

    MVec3 m = _normal.Cross( plane._normal ).Normalized();
    MVec3 p = c0 * _normal + c1 * plane._normal;
	line = MLine( m, p );
    return true;
}

//----------------------------------------------------------
bool
MPlane::GetIntersect( MVec3& point, const MPlane& plane1, const MPlane& plane2 ) const
{
	// determine the denominator.
	float denom = GetNormal().Dot( plane1.GetNormal().Cross( plane2.GetNormal() ) );
	if ( Abs( denom ) < 1e-06f )
		return false;

	// invert.
	denom = 1.0f / denom;

	// calculate the top.
	MVec3 t = -GetD() * plane1.GetNormal().Cross( plane2.GetNormal() );
	t += -plane1.GetD() * plane2.GetNormal().Cross( GetNormal() );
	t += -plane2.GetD() * GetNormal().Cross( plane1.GetNormal() );

	// calculate the final point.
	point = denom * t;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void
MPlane::Save( UWriter& writer ) const
{
	_normal.Save( writer );
	writer.WriteFloat( _d );
}

//----------------------------------------------------------
void
MPlane::Load( UReader& reader )
{
	_normal.Load( reader );
	_d = reader.ReadFloat();
}
