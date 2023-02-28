//----------------------------------------------------------
// File:        MAABox.h
// Author:      Kevin Bray
// Created:     09-06-05
//
// Purpose:     To represent an axis aligned box in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

// forward declarations
class MPlane;
class MSphere;
class UWriter;
class UReader;
class MOBox;

//**********************************************************
// class MAABox
//**********************************************************
class MAABox
{
public:
    MAABox() { }
    MAABox( const MVec3& center, const MVec3& halfExts );
    MAABox( const MOBox& box );
    ~MAABox();

    MSphere         GetBoundingSphere();

    // returns the distance from a point to the AABB.
    float           Dist( const MVec3& pos );

    // returns whether the axis-aligned box intersects various shapes.
    bool            Intersect( const MAABox& box ) const;
    bool            Intersect( const MSphere& sphere ) const;

    // expands the axis-aligned box by the box passed in.
    bool            AddBoxToVolume( const MAABox& box );
    bool            AddBoxToVolume( const MOBox& box );

    const MVec3&    GetCenter() const                       {   return _center;             }
    const MVec3&    GetHalfExts() const                     {   return _halfExts;           }

    void            SetCenter( const MVec3& center )        {   _center = center;           }
    void            SetHalfExts( const MVec3& halfExts )    {   _halfExts = halfExts.Abs(); }

    // pass in an array of 8 MVec3's.
    void            GetCorners( MVec3 *pts ) const;

    bool            IsInside( const MVec3& point ) const;

    int             GetSide( const MPlane& plane ) const;

    void            Save( UWriter& writer ) const;
    void            Load( UReader& reader );

private:
    MVec3           _center;
    MVec3           _halfExts;
};
