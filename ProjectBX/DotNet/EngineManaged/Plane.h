//----------------------------------------------------------
// File:        Plane.h
// Author:      Shawn Presser
// Created:     09-22-08
//
// Purpose:     To represent a plane in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Vector3.h"

// .NET namespaces.
using System::Runtime::InteropServices::OutAttribute;

// engine forward declarations.
class MPlane;

namespace Bootstrap 
{
    // .NET forward declarations.
    value class Matrix;
    value class Matrix3x3;
    value class Line;

    //**********************************************************
    // value class Plane
    //**********************************************************
    public value class Plane
    {
    internal:
        // constructs a plane from an MPlane.
        Plane( const MPlane& plane );

        // converts the plane to an MPlane.
        MPlane                      ToMPlane();

    public:
        // constructs a plane that passes through three points.
        Plane( Vector3 p0, Vector3 p1, Vector3 p2 );

        // constructs a plane with the specified normal and that 
        // passes through teh specified point.
        Plane( Vector3 normal, Vector3 point );

        // constructs a plane with the specified normal and the 
        // specified distance from origin.
        Plane( Vector3 normal, float d );

        // gets or sets the plane's normal.
        property Vector3            Normal  
        { 
            Vector3                     get(); 
            void                        set( Vector3 v );
        }

        // gets or sets the plane's distance from the origin.
        property float              D       
        { 
            float                       get(); 
            void                        set( float d );
        }

        // reverses the plane.
        void                        ReverseFacing();

        // adds a bias to the plane ( moves it along the normal by the amount specified ).
        void                        Bias( float amount );

        // transforms the plane by a 4x4 matrix.
        void                        Transform( Matrix transform, Matrix3x3 normalRot );

        // translates the plane by a given amount.
        void                        Translate( Vector3 translate );

        // scales the plane's distance to the origin by the given amount.
        void                        Scale( float scale );

        // builds a reflection matrix.
        Matrix                      BuildReflectMatrix();

        // returns true if two planes are coplanar.
        bool                        IsCoplanar( Plane plane, float angleEpsilon, float dEpsilon );

        // returns the side the point passed in is on.
        int                         GetSide( Vector3 pt, float epsilon );

        // returns the line that is formed where two planes intersect.  If the line cannot
        // be determined, false is returned.  Otherwise, true is returned.
        bool                        GetIntersect( [Out] Line% line, Plane plane );

        // returns the point that is formed where three planes intersect.  If a point cannot
        // be determined, false is returned.  Otherwise, true is returned.
        bool                        GetIntersect( [Out] Vector3% point, Plane plane1, Plane plane2 );

        // returns the signed distance to a point.
        float                       Dist( Vector3 pt );

        // computes the intersection point of a line segment and the plane.  
        // Returns false if the line segment does not intersect.
        bool                        GetSegmentIntersect( [Out] Vector3% pt, Vector3 pt0, Vector3 pt1 );

        // pretty-prints the plane.
        virtual System::String^     ToString() override;

    private:
        Vector3                     _normal;
        float                       _d;
    };
}