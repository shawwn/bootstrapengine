//----------------------------------------------------------
// File:		Sphere.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To represent a sphere in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Vector3.h"

// .NET namespaces.
using System::Runtime::InteropServices::OutAttribute;

// forward declarations.
class MSphere;

namespace Bootstrap
{
    // .NET forward declarations.
    value class Plane;
    value class Matrix;

    //**********************************************************
    // value class Sphere
    //**********************************************************
	public value class Sphere
	{
	internal:
        // constructs a sphere from an MSphere.
		Sphere( const MSphere& sphere );

        // converts the sphere to an MSphere.
		MSphere                     ToMSphere();

	public:
        // constructs a sphere with the specified center point 
        // and radius.
        Sphere( Vector3 center, float radius );

        // gets or sets the sphere's center.
        property Vector3            Center;

        // gets or sets the sphere's radius.
        property float              Radius;

        // returns true if the sphere passed in is inside this sphere or if it
        // intersects this sphere in anyway.
        bool			            Intersect( Sphere sphere );

        // returns true if the sphere passed in is inside this sphere.
        bool			            IsInside( Sphere sphere );

        // determines which side of the plane this sphere is on.
        int				            GetSide( Plane plane );

        // returns the center and radius of the cross-section formed by 
        // the intersection of the plane and this sphere.  If the plane
        // doesn't intersect this sphere, then the method returns 
        // which side of the plane this sphere is on.
        int				            GetSection( Plane plane, [Out] Vector3% center, [Out] float% radius );

        // transforms the sphere, returning the transformed sphere 
        // without modifying this one.
        Sphere			            Transform( Matrix transform );

        // transforms this sphere.
        void			            TransformFast( Matrix transform );

        // pretty-prints the sphere.
		virtual System::String^     ToString() override;
	};
}