//----------------------------------------------------------
// File:        Plane.cpp
// Author:      Shawn Presser
// Created:     09-22-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Plane.h"

// engine headers.
#include "MPlane.h"

// project headers.
#include "Matrix.h"
#include "Line.h"

//**********************************************************
// value class Plane
//**********************************************************

//----------------------------------------------------------
Bootstrap::Plane::Plane( const MPlane& plane )
{
    _normal = plane.GetNormal();
    _d = plane.GetD();
}

//----------------------------------------------------------
MPlane 
Bootstrap::Plane::ToMPlane()
{
    return MPlane( _normal.ToMVec3(), _d );
}

//----------------------------------------------------------
Bootstrap::Plane::Plane( Vector3 p0, Vector3 p1, Vector3 p2 )
{
    _normal = (p1-p0).Cross(p2-p0).Normalized;
    _d = -_normal.Dot( p0 );
}

//----------------------------------------------------------
Bootstrap::Plane::Plane( Vector3 normal, Vector3 point )
    : _normal( normal )
    , _d( 0.0f )
{
    _normal = _normal.Normalized;
    _d = -_normal.Dot( point );
}

//----------------------------------------------------------
Bootstrap::Plane::Plane( Vector3 normal, float d )
    : _normal( normal )
    , _d( d )
{
    _normal = _normal.Normalized;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Plane::Normal::get() 
{ 
    return _normal; 
}

//----------------------------------------------------------
void
Bootstrap::Plane::Normal::set( Vector3 v )
{
    _normal = v.Normalized;
}

//----------------------------------------------------------
float 
Bootstrap::Plane::D::get() 
{ 
    return _d; 
}

//----------------------------------------------------------
void
Bootstrap::Plane::D::set( float d )
{
    _d = d;
}

//----------------------------------------------------------
void 
Bootstrap::Plane::ReverseFacing()
{
    _normal = -_normal; 
    _d = -_d;
}

//----------------------------------------------------------
void 
Bootstrap::Plane::Bias( float amount )
{
    _d -= ( System::Math::Sign( _d ) * amount );
}

//----------------------------------------------------------
void 
Bootstrap::Plane::Transform( Matrix transform, Matrix3x3 normalRot )
{
    // transform the plane into world space.
    MVec3 point = -_d * _normal;
    _normal = normalRot.RotatePoint( _normal );
    _d = -_normal.Dot( transform.TransformCoord( point ) );
}

//----------------------------------------------------------
void 
Bootstrap::Plane::Translate( Vector3 translate )
{
    Vector3 point = -_d * _normal;
    _d = -_normal.Dot( point + translate );
}

//----------------------------------------------------------
void 
Bootstrap::Plane::Scale( float scale )
{
    _d *= scale;
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Plane::BuildReflectMatrix()
{
    float negD = -_d;
    MVec3 pos = negD * _normal;
    float nX = _normal.X;
    float nY = _normal.Y;
    float nZ = _normal.Z;

    return Matrix( 
        1.0f - ( 2.0f*nX*nX ),      -2.0f*nX*nY,            -2.0f*nX*nZ,            2.0f*negD*nX,
        -2.0f*nX*nY,                 1.0f - ( 2.0f*nY*nY ), -2.0f*nY*nZ,            2.0f*negD*nY,
        -2.0f*nX*nZ,                -2.0f*nY*nZ,             1.0f - ( 2.0f*nZ*nZ ), 2.0f*negD*nZ,
        0.0f,                        0.0f,                   0.0f,                  1.0f );
}

//----------------------------------------------------------
bool 
Bootstrap::Plane::IsCoplanar( Plane plane, float angleEpsilon, float dEpsilon )
{
return ( ( 1.0 - Abs( _normal.Dot( plane._normal ) ) ) < angleEpsilon && Abs( _d - plane._d ) < dEpsilon );
}

//----------------------------------------------------------
int 
Bootstrap::Plane::GetSide( Vector3 pt, float epsilon )
{
    float dist = Dist( pt );
    if ( Abs( dist ) < epsilon )
        return 0;
    return dist >= 0 ? 1 : -1;
}

//----------------------------------------------------------
bool
Bootstrap::Plane::GetIntersect( [Out] Line% line, Plane plane )
{
    float n00 = _normal.MagSqr;
    float n01 = _normal.Dot( plane.Normal );
    float n11 = plane.Normal.MagSqr;
    float det = n00 * n11 - n01 * n01;
    const float kError = 1e-06f;
    if ( Abs( det ) < kError )
        return false;

    float invDet = 1.0f/det;
    float c0 = ( n11 * _d - n01 * plane._d ) * invDet;
    float c1 = ( n00 * plane._d - n01 * _d ) * invDet;

    Vector3 m = _normal.Cross( plane._normal ).Normalized;
    Vector3 p = c0 * _normal + c1 * plane._normal;
    line = Line( m, p );
    return true;
}

//----------------------------------------------------------
bool 
Bootstrap::Plane::GetIntersect( [Out] Vector3% point, Plane plane1, Plane plane2 )
{
    // determine the denominator.
    float denom = Normal.Dot( plane1.Normal.Cross( plane2.Normal ) );
    if ( Abs( denom ) < 1e-06f )
        return false;

    // invert.
    denom = 1.0f / denom;

    // calculate the top.
    MVec3 t = -D * plane1.Normal.Cross( plane2.Normal );
    t += -plane1.D * plane2.Normal.Cross( Normal );
    t += -plane2.D * Normal.Cross( plane1.Normal );

    // calculate the final point.
    point = denom * t;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
float 
Bootstrap::Plane::Dist( Vector3 pt )
{
    return( _normal.Dot( pt ) + _d );
}

//----------------------------------------------------------
bool 
Bootstrap::Plane::GetSegmentIntersect( [Out] Vector3% pt, Vector3 pt0, Vector3 pt1 )
{
    // essentially, we're finding how much of the line resides on one half
    // of the plane, and how much of the line resides on the other half.  I
    // then use that to find a ratio that I then use to interpolate between
    // the two points passed in.
    float d0 = Dist( pt0 );
    float d1 = Dist( pt1 );
    if ( ( d0 < 0.0f ) == ( d1 < 0.0f ) )
    {
        // both points are on the same side of the plane; does not intersect.
        return false;
    }

    // find the ratio of how much of the line resides on opposite sides of
    // the plane.
    d0 = Abs( d0 );
    d1 = Abs( d1 );
    float t = d0 / ( d0 + d1 );
    pt = pt0.Lerp( pt1, t );

    // indicate an intersection point was found.
    return true;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Plane::ToString() 
{
    return _normal + "," + _d;
}