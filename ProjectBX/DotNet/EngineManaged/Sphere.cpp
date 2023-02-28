//----------------------------------------------------------
// File:		Sphere.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Sphere.h"

// engine headers.
#include "MSphere.h"
#include "Plane.h"
#include "Matrix.h"

//**********************************************************
// value class Sphere
//**********************************************************

//----------------------------------------------------------
Bootstrap::Sphere::Sphere( const MSphere& sphere )
{
    Center = sphere.GetCenter();
    Radius = sphere.GetRadius();
}

//----------------------------------------------------------
MSphere
Bootstrap::Sphere::ToMSphere()
{
    return MSphere( Center, Radius );
}

//----------------------------------------------------------
Bootstrap::Sphere::Sphere( Vector3 center, float radius )
{
    Center = center;
    Radius = radius;
}

//----------------------------------------------------------
bool
Bootstrap::Sphere::Intersect( Sphere sphere )
{
	float maxDistSqr = Radius + sphere.Radius;
	maxDistSqr *= maxDistSqr;
	return ( Center.DistSqr( sphere.Center ) < maxDistSqr );
}

//----------------------------------------------------------
bool
Bootstrap::Sphere::IsInside( Sphere sphere )
{
	return ( ( Center.Dist( sphere.Center ) + sphere.Radius ) < Radius );
}

//----------------------------------------------------------
int
Bootstrap::Sphere::GetSide( Plane plane )
{
	float dist = plane.Dist( Center );
    if ( ::Abs( dist ) > Radius )
		return dist >= 0.0f ? 1 : -1;
	return 0;
}

//----------------------------------------------------------
int 
Bootstrap::Sphere::GetSection( Plane plane, [Out] Vector3% center, [Out] float% radius )
{
	// get the distance to the plane.  If we're entirely on one side, return
	// the indication as such.
	float dist = plane.Dist( Center );
    if ( ::Abs( dist ) > Radius )
		return dist > 0.0f ? 1 : -1;

	// determine the radius of our cross section.
	radius = Sqrt( Radius*Radius - dist*dist );

	// determine the point of intersection along the plane normal.
	center = Center - dist * plane.Normal;

	// we intersect and are returning a valid section.
	return 0;
}

//----------------------------------------------------------
Bootstrap::Sphere 
Bootstrap::Sphere::Transform( Matrix transform )
{
    Sphere sphere;

	Vector3 scales = transform.ScaleSqr;

	// determine the maximum scale.
    sphere.Radius = ::Sqrt( scales.Max() ) * Radius;

	// transform the center.
	sphere.Center = transform.TransformCoord( Center );

    return sphere;
}

//----------------------------------------------------------
void 
Bootstrap::Sphere::TransformFast( Matrix transform )
{
	Vector3 scales = transform.ScaleSqr;

	// determine the maximum scale.
    Radius = ::Sqrt( scales.Max() ) * Radius;

	// transform the center.
	Center = transform.TransformCoord( Center );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Sphere::ToString()
{
	return "Center " + Center + " Radius " + Radius;
}