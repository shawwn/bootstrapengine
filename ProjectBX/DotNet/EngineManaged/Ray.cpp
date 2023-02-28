//----------------------------------------------------------
// File:		Ray.cpp
// Author:		Shawn Presser
// Created:		09-20-08
//
// Purpose:		To represent a ray in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Ray.h"

// project headers.
#include "OBox.h"
#include "AABox.h"
#include "Sphere.h"
#include "Plane.h"

// engine headers.
#include "MLineSegment.h"
#include "MSphere.h"
#include "MPlane.h"

//**********************************************************
// value class MRay
//**********************************************************

//----------------------------------------------------------
Bootstrap::Ray::Ray( const MRay& ray )
{
	_start = ray.GetStart();
	_dir = ray.GetDir();
}

//----------------------------------------------------------
MRay 
Bootstrap::Ray::ToMRay()
{
	return MRay( _start.ToMVec3(), _dir.ToMVec3() );
}

//----------------------------------------------------------
Bootstrap::Ray::Ray( Vector3 start, Vector3 dir )
{
	_start = start;
	_dir = dir;
}

//----------------------------------------------------------
bool 
Bootstrap::Ray::Intersect( AABox box, float maxDist )
{
    // forward the implementation to MRay.
    return ToMRay().Intersect( box.ToMAABox(), maxDist );
}

//----------------------------------------------------------
bool 
Bootstrap::Ray::Intersect( OBox box )
{
    // forward the implementation to MRay.
    return ToMRay().Intersect( box.ToMOBox() );
}

//----------------------------------------------------------
bool 
Bootstrap::Ray::Intersect( Sphere sphere )
{
    // forward the implementation to MRay.
    return ToMRay().Intersect( sphere.ToMSphere() );
}

//----------------------------------------------------------
Bootstrap::Ray 
Bootstrap::Ray::Transform( Matrix transform, Matrix3x3 normalRot )
{
	return Ray( transform.TransformCoord( _start ), normalRot.RotatePoint( _dir ) );
}

//----------------------------------------------------------
bool 
Bootstrap::Ray::Intersect( [Out] Vector3% hitPos, Plane plane, float maxDist )
{
    // forward the implementation to MRay.
    MVec3 nativeHitPos( 0.0f, 0.0f, 0.0f );
    bool result = ToMRay().Intersect( nativeHitPos, plane.ToMPlane(), maxDist );
    hitPos = Vector3( nativeHitPos );
    return result;
}

//----------------------------------------------------------
bool			            
Bootstrap::Ray::Intersect( [Out] Vector3% point, 
                           [Out] float% u, [Out] float% v, 
                           Vector3 pt0, Vector3 pt1, Vector3 pt2,
			               bool twoSided )
{
    // forward the implementation to MRay.
    MVec3 nativePoint( 0.0f, 0.0f, 0.0f );
    float nativeU = 0.0f;
    float nativeV = 0.0f;
    bool result = ToMRay().Intersect( nativePoint, nativeU, nativeV, pt0.ToMVec3(), pt1.ToMVec3(), pt2.ToMVec3(), twoSided );
    point = Vector3( nativePoint );
    u = nativeU;
    v = nativeV;
    return result;
}

//----------------------------------------------------------
bool			            
Bootstrap::Ray::Intersect( [Out] float% dist, [Out] Vector3% point, 
                           [Out] float% u, [Out] float% v, 
                           Vector3 pt0, Vector3 pt1, Vector3 pt2, 
                           bool twoSided, float maxDist )
{
    // forward the implementation to MRay.
    float nativeDist = 0.0f;
    MVec3 nativePoint( 0.0f, 0.0f, 0.0f );
    float nativeU = 0.0f;
    float nativeV = 0.0f;
    bool result = ToMRay().Intersect( nativeDist, nativePoint, nativeU, nativeV, pt0.ToMVec3(), pt1.ToMVec3(), pt2.ToMVec3(), twoSided, maxDist );
    point = Vector3( nativePoint );
    u = nativeU;
    v = nativeV;
    dist = nativeDist;
    return result;
}

//----------------------------------------------------------
bool			            
Bootstrap::Ray::Intersect( [Out] Vector3% point, 
                           [Out] float% u, [Out] float% v, 
                           Vector3 pt0, Vector3 pt1, Vector3 pt2,
					       Vector3 faceNormal, bool twoSided )
{
    // forward the implementation to MRay.
    MVec3 nativePoint( 0.0f, 0.0f, 0.0f );
    float nativeU = 0.0f;
    float nativeV = 0.0f;
    bool result = ToMRay().Intersect( nativePoint, nativeU, nativeV, pt0.ToMVec3(), pt1.ToMVec3(), pt2.ToMVec3(), faceNormal.ToMVec3(), twoSided );
    point = Vector3( nativePoint );
    u = nativeU;
    v = nativeV;
    return result;
}

//----------------------------------------------------------
float 
Bootstrap::Ray::Dist( Vector3 point )
{
    // forward the implementation to MRay.
	return ToMRay().Dist( point.ToMVec3() );
}

//----------------------------------------------------------
float 
Bootstrap::Ray::Dist( LineSegment segment, float epsilon )
{
    // forward the implementation to MRay.
	return ToMRay().Dist( segment.ToMLineSegment(), epsilon );
}

//----------------------------------------------------------
float 
Bootstrap::Ray::Dist( LineSegment segment, 
                      [Out] float% rayMu, [Out] float% segMu )
{
    // provide a reasonable epsilon.
    return Dist( segment, rayMu, segMu, 0.001f );
}

//----------------------------------------------------------
float 
Bootstrap::Ray::Dist( LineSegment segment, 
                      [Out] float% rayMu, [Out] float% segMu, 
                      float epsilon )
{
    // forward the implementation to MRay.
    float nativeRayMu = 0.0f;
    float nativeSegMu = 0.0f;
	float result = ToMRay().Dist( segment.ToMLineSegment(), nativeRayMu, nativeSegMu, epsilon );
    rayMu = nativeRayMu;
    segMu = nativeSegMu;
    return result;
}
