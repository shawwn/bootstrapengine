//----------------------------------------------------------
// File:		Ray.h
// Author:		Shawn Presser
// Created:		09-20-08
//
// Purpose:		To represent a ray in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "Vector3.h"
#include "LineSegment.h"

// engine includes.
#include "MRay.h"

// .NET namespaces.
using System::Runtime::InteropServices::OutAttribute;

namespace Bootstrap 
{
    // .NET forward declarations.
    value class OBox;
    value class AABox;
    value class Matrix;
    value class LineSegment;
    value class Sphere;
    value class Matrix3x3;
    value class Plane;

    //**********************************************************
    // value class MRay
    //**********************************************************
	public value class Ray
	{
	internal:
        // constructs a ray from an MRay.
		Ray( const MRay& ray );

        // converts from a ray to an MRay.
		MRay                        ToMRay();

	public:
        // constructs a ray with the specified direction that 
        // originates at the specified point.
		Ray( Vector3 start, Vector3 dir );

		// the ray's origin.
		property Vector3            Start
		{
			Vector3                     get()                   {   return _start;              }
			void                        set( Vector3 value )    {   _start = value;             }
		}

		// the ray's direction.
		property Vector3            Dir
		{
			Vector3                     get()                   {   return _dir;                }
			void                        set( Vector3 value )    {   _dir = value.Normalized;    }
		}

        // returns true if the ray intersects with the shapes provided.
        bool                        Intersect( AABox box, float maxDist );
        bool                        Intersect( OBox box );
        bool                        Intersect( Sphere sphere );

        // transforms the ray and returns the transformed ray.
        Ray                         Transform( Matrix transform, Matrix3x3 normalRot );

    	// returns true if the ray intersects with the plane.  It also returns where
    	// the ray intersects with the plane.
    	bool			            Intersect( [Out] Vector3% hitPos, Plane plane, float maxDist );

    	// returns the distance from the ray to the triangle as well as the barycentric
    	// coordinates of the intersection if a intersection occurs.  If no intersection
    	// occurs, then the function returns -1.0.
    	bool			            Intersect( [Out] Vector3% point, 
                                               [Out] float% u, [Out] float% v, 
                                               Vector3 pt0, Vector3 pt1, Vector3 pt2,
    							               bool twoSided );
    	bool			            Intersect( [Out] float% dist, [Out] Vector3% point, 
                                               [Out] float% u, [Out] float% v, 
                                               Vector3 pt0, Vector3 pt1, Vector3 pt2, 
                                               bool twoSided, float maxDist );
    	bool			            Intersect( [Out] Vector3% point, 
                                               [Out] float% u, [Out] float% v, 
                                               Vector3 pt0, Vector3 pt1, Vector3 pt2,
            							       Vector3 faceNormal, bool twoSided );

    	// distance from the ray to a point.
    	float			            Dist( Vector3 point );

    	// distance between two rays.
    //	float			            Dist( Ray ray, float epsilon );

    	// distance between a ray and a line segment.  This function returns -1.0f if
    	// the line segment is just a point.
    	float			            Dist( LineSegment segment, float epsilon );
    	float			            Dist( LineSegment segment, 
                                          [Out] float% rayMu, [Out] float% segMu );
    	float			            Dist( LineSegment segment, 
                                          [Out] float% rayMu, [Out] float% segMu, 
                                          float epsilon );

	internal:
		static                      operator MRay( Ray v )      {   return v.ToMRay();          }
		static                      operator Ray( MRay v )      {   return Ray( v );            }

    private:
        Vector3                     _start;
        Vector3                     _dir;
	};
}
