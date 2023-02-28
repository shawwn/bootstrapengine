//----------------------------------------------------------
// File:        AABox.h
// Author:      Shawn Presser
// Created:     09-20-08
//
// Purpose:     To represent an axis aligned box in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "Vector3.h"

// forward declarations.
class MAABox;

namespace Bootstrap
{
    // .NET forward declarations.
    value class OBox;
    value class Plane;
    value class Sphere;

    //**********************************************************
    // value class AABox
    //**********************************************************
    public value class AABox
    {
    internal:
        // constructs an axis-aligned box from an MAABox.
        AABox( const MAABox& aaBox );

        // converts the axis-aligned box to an MAABox.
        MAABox                      ToMAABox();

    public:
        // constructs an axis-aligned box with the specified center 
        // and half-extents.
        AABox( Vector3 center, Vector3 halfExtents );

        // constructs an axis-aligned box that encompasses the
        // specified oriented box.
        AABox( OBox box );

        // gets or sets the center point of the axis-aligned box.
        property Vector3            Center;

        // gets or sets the half-extents of the axis-aligned box.
        property Vector3            HalfExtents;

        // computes the 8 world space corners of the axis-aligned box.
        property array< Vector3 >^  Corners
        {
            array< Vector3 >^           get();
        }

        // computes the box's bounding sphere.
        property Sphere             BoundingSphere
        {
            Sphere                      get();
        }

        // returns whether the axis-aligned box intersects various shapes.
        bool                        Intersect( AABox box );
        //bool                      Intersect( Sphere sphere );

        // expands the axis-aligned box by the box passed in.
        bool                        AddBoxToVolume( AABox box );
        bool                        AddBoxToVolume( OBox box );

        // returns whether the specified point is inside the axis-aligned box.
        bool                        IsInside( Vector3 point );

        // returns whether the axis-aligned box is in front of, intersects with,
        // or is behind the specified plane. 
        int                         GetSide( Plane plane );

        // pretty-prints the axis-aligned box.
        virtual System::String^     ToString() override;
    };
}