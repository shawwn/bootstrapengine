//----------------------------------------------------------
// File:		Frustum.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To link sectors together.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Plane.h"
#include "AABox.h"

// forward declarations.
class GrFrustum;
class MRay;
class MVec3;
class MPlane;

namespace Bootstrap
{
    //**********************************************************
    // enum class CullPlane
    //**********************************************************
	public enum class CullPlane
	{
		Left,
		Right,
		Top,
		Bottom,
		Near,
		Far
	};

    //**********************************************************
    // ref class Frustum
    //**********************************************************
	public ref class Frustum
	{
	internal:
		Frustum( const GrFrustum& frustum );

		GrFrustum*		                    _frustum;

	public:
		// creates a frustum with no area.
		Frustum( Vector3 lookPoint, Vector3 lookDir, Vector3 nearBottomLeft, Vector3 nearTopLeft,
    			 Vector3 nearTopRight, Vector3 nearBottomRight, Vector3 farBottomLeft, Vector3 farTopLeft,
    			 Vector3 farTopRight, Vector3 farBottomRight, float nearDist, float farDist );
		~Frustum();
		!Frustum();

		property array< Plane >^	        CullPlanes	
        { 
            array< Plane >^                     get(); 
        }

		property Vector3			        Position	
        { 
            Vector3                             get(); 
        }

		void						        AddCullPlane( Plane plane );
		Plane						        GetCullPlane( CullPlane plane );

		// returns the 'best' cross-section of the frustum.
		array< Vector3 >^			        GetCrossSection( Plane plane );
		array< Vector3 >^			        GetPerpCrossSection( float dist );

		// builds a hull using the frustum corners.  This does NOT include any additional planes,
		// only the 6 original planes.  This returns 18 vertices in a triangle list format.
		array< Vector3 >^			        BuildHull();

		// the frustum is only a hull if it's position is on or is in the positive half-space of
		// all of the planes.
		property bool				        IsHull 
        { 
            bool                                get(); 
        }

		// These determine how an object is relative to the frustum.  The IsXXXInside functions
		// return true if an object is fully or partially inside.
		bool						        IsSphereInside( Vector3 pt, float radius );
		bool						        IsPointInside( Vector3 pt );
		bool						        IsAABoxInside( AABox box );
		bool								IsOBoxInside( OBox box );
		int							        FindRleativeLoc( Plane plane );					// -1 = outside, 1 = inside, 0 = both.
		int							        FindRelativeLoc( Vector3 pt0, Vector3 pt1 );	// -1 = outside, 1 = inside, 0 = both.
		bool						        IsFrustumInside( Frustum^ frustum );			// true if fully or partially inside.
		bool						        IsTriangleInside( Vector3 p0, Vector3 p1, Vector3 p2 );
        bool				                IsRayInside( const MRay& ray, float epsilon );

		property Vector3			        FarBottomLeft	        
        {   
            Vector3                             get();      
        }

		property Vector3			        FarTopLeft
        {   
            Vector3                             get();      
        }

		property Vector3			        FarTopRight
        {   
            Vector3                             get();      
        }

		property Vector3			        FarBottomRight
        {   
            Vector3                             get();      
        }

		property Vector3			        NearBottomLeft
        {   
            Vector3                             get();      
        }

		property Vector3			        NearTopLeft
        {   
            Vector3                             get();      
        }

		property Vector3			        NearTopRight
        {   
            Vector3                             get();      
        }

		property Vector3			        NearBottomRight
        {   
            Vector3                             get();      
        }
	};
}