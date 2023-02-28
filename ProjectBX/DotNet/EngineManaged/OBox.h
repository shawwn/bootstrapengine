//----------------------------------------------------------
// File:        OBox.h
// Author:      Shawn Presser
// Created:     09-22-08
//
// Purpose:     To manage an oriented bounding box.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "MOBox.h"

// project headers.
#include "Vector3.h"
#include "Matrix.h"

// using namespaces.
using System::Runtime::InteropServices::OutAttribute;

// forward declarations.
class MOBox;

namespace Bootstrap 
{
    // .NET forward declarations.
    value class AABox;
    value class Ray;

    //**********************************************************
    // value class OBox
    //**********************************************************
    public value class OBox
    {
    internal:
        // constructs an oriented box from an MOBox.
        OBox( const MOBox& box );

        // converts the oriented box to an MOBox.
        MOBox                       ToMOBox();

    public:
        // constructs an oriented box with no rotation and 
        // with dimensions defined by an axis-aligned box.
        OBox( AABox aaBox );

        // constructs an oriented box with a specific 
        // transform and with dimensions defined by an 
        // axis-aligned box.
        OBox( Matrix transform, AABox aaBox );

        // constructs an oriented box with a specific 
        // transform and with dimensions defined by the
        // 'halfExtents' vector.
        OBox( Matrix transform, Vector3 halfExtents );

        // the transform of the oriented box.
        property Matrix             Transform;

        // the dimensions of the oriented box.
        property Vector3            HalfExtents;

        // computes the 8 world space corners of the oriented box.
        property array< Vector3 >^  Corners
        {
            array< Vector3 >^           get();
        }

        // pretty-prints the oriented box.
        virtual System::String^     ToString() override;
    };
}

