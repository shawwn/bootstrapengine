//----------------------------------------------------------
// File:        AABox.cpp
// Author:      Shawn Presser
// Created:     09-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "AABox.h"

// project headers.
#include "OBox.h"
#include "Plane.h"
#include "Sphere.h"

// engine headers.
#include "MAABox.h"

//**********************************************************
// value class AABox
//**********************************************************

//----------------------------------------------------------
Bootstrap::AABox::AABox( const MAABox& aaBox )
{
    Center      = aaBox.GetCenter();
    HalfExtents = aaBox.GetHalfExts();
}

//----------------------------------------------------------
MAABox
Bootstrap::AABox::ToMAABox() 
{
    return MAABox( Center, HalfExtents );
}

//----------------------------------------------------------
Bootstrap::AABox::AABox( Vector3 center, Vector3 halfExtents )
{
    Center      = center;
    HalfExtents = halfExtents;
}

//----------------------------------------------------------
Bootstrap::AABox::AABox( OBox box )
{
    // iterate through each corner of the oriented box 
    // and keep the min/max.
    Vector3 minExts(  FLT_MAX,  FLT_MAX,  FLT_MAX );
    Vector3 maxExts( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    for each ( Vector3 corner in box.Corners )
    {
        minExts = minExts.Min( corner );
        maxExts = maxExts.Max( corner );
    }

    // calculate the center and the extents.
    Center      = 0.5f * ( maxExts + minExts );
    HalfExtents = 0.5f * ( maxExts - minExts );
}

//-----------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::AABox::Corners::get()
{
    array< Vector3 >^ corners = gcnew array< Vector3 >( 8 );

    // bottom, back-left.
    corners[ 0 ].X = Center.X - HalfExtents.X;
    corners[ 0 ].Y = Center.Y - HalfExtents.Y;
    corners[ 0 ].Z = Center.Z - HalfExtents.Z;

    // bottom, forward-left.
    corners[ 1 ].X = Center.X - HalfExtents.X;
    corners[ 1 ].Y = Center.Y - HalfExtents.Y;
    corners[ 1 ].Z = Center.Z + HalfExtents.Z;

    // bottom, forward-right.
    corners[ 2 ].X = Center.X + HalfExtents.X;
    corners[ 2 ].Y = Center.Y - HalfExtents.Y;
    corners[ 2 ].Z = Center.Z + HalfExtents.Z;

    // bottom, back-right.
    corners[ 3 ].X = Center.X + HalfExtents.X;
    corners[ 3 ].Y = Center.Y - HalfExtents.Y;
    corners[ 3 ].Z = Center.Z - HalfExtents.Z;

    // top, back-left.
    corners[ 4 ].X = Center.X - HalfExtents.X;
    corners[ 4 ].Y = Center.Y + HalfExtents.Y;
    corners[ 4 ].Z = Center.Z - HalfExtents.Z;

    // top, forward-left.
    corners[ 5 ].X = Center.X - HalfExtents.X;
    corners[ 5 ].Y = Center.Y + HalfExtents.Y;
    corners[ 5 ].Z = Center.Z + HalfExtents.Z;

    // top, forward-right.
    corners[ 6 ].X = Center.X + HalfExtents.X;
    corners[ 6 ].Y = Center.Y + HalfExtents.Y;
    corners[ 6 ].Z = Center.Z + HalfExtents.Z;

    // top, back-right.
    corners[ 7 ].X = Center.X + HalfExtents.X;
    corners[ 7 ].Y = Center.Y + HalfExtents.Y;
    corners[ 7 ].Z = Center.Z - HalfExtents.Z;

    return corners;
}

//----------------------------------------------------------
Bootstrap::Sphere
Bootstrap::AABox::BoundingSphere::get()
{
    return Sphere( Center, HalfExtents.Mag );
}

//----------------------------------------------------------
bool 
Bootstrap::AABox::Intersect( AABox box )
{
    // get the distance from centers along each axis.
    Vector3 dist( ( Center - box.Center ).Abs );

    // check to see if we're too far to intersect on any axes.
    if ( dist.X > ( HalfExtents.X + box.HalfExtents.X ) )
        return false;
    if ( dist.Y > ( HalfExtents.Y + box.HalfExtents.Y ) )
        return false;
    if ( dist.Z > ( HalfExtents.Z + box.HalfExtents.Z ) )
        return false;

    // we're too close on all axes, and therefore, we intersect!
    return true;
}

//----------------------------------------------------------
bool 
Bootstrap::AABox::AddBoxToVolume( AABox box )
{
    // calculate min/max extents.
    Vector3 boxMin = box.Center - box.HalfExtents;
    Vector3 boxMax = box.Center + box.HalfExtents;
    Vector3 thisMin = Center - HalfExtents;
    Vector3 thisMax = Center + HalfExtents;

    // do we need to do anything?
    if ( boxMin.X >= thisMin.X &&
         boxMin.Y >= thisMin.Y &&
         boxMin.Z >= thisMin.Z &&
         boxMax.X <= thisMin.X &&
         boxMax.Y <= thisMin.Y &&
         boxMax.Z <= thisMin.Z )
        return false;

    // calculate the new min/max.
    Vector3 newMin( ::Min( boxMin.X, thisMin.X ),
                    ::Min( boxMin.Y, thisMin.Y ),
                    ::Min( boxMin.Z, thisMin.Z ) );
    Vector3 newMax( ::Max( boxMax.X, thisMax.X ),
                    ::Max( boxMax.Y, thisMax.Y ),
                    ::Max( boxMax.Z, thisMax.Z ) );

    // calculate the new center/extents.
    Center      = 0.5f * ( newMax + newMin );
    HalfExtents = 0.5f * ( newMax - newMin );

    // we adjusted the box.
    return true;
}

//----------------------------------------------------------
bool 
Bootstrap::AABox::AddBoxToVolume( OBox box )
{
    Vector3 thisMin = Center - HalfExtents;
    Vector3 thisMax = Center + HalfExtents;

    // calculate the min/max points of the box passed in.
    Vector3 boxMin = box.Transform.Translate;
    Vector3 boxMax = boxMin;

    for each ( Vector3 corner in box.Corners )
    {
        boxMin = boxMin.Min( corner );
        boxMax = boxMax.Max( corner );
    }

    // do we need to do anything?
    if ( boxMin.X >= thisMin.X &&
         boxMin.Y >= thisMin.Y &&
         boxMin.Z >= thisMin.Z &&
         boxMax.X <= thisMin.X &&
         boxMax.Y <= thisMin.Y &&
         boxMax.Z <= thisMin.Z )
        return false;

    // calculate the new min/max.
    Vector3 newMin( ::Min( boxMin.X, thisMin.X ),
                    ::Min( boxMin.Y, thisMin.Y ),
                    ::Min( boxMin.Z, thisMin.Z ) );
    Vector3 newMax( ::Max( boxMax.X, thisMax.X ),
                    ::Max( boxMax.Y, thisMax.Y ),
                    ::Max( boxMax.Z, thisMax.Z ) );

    // calculate the new center/extents.
    Center      = 0.5f * ( newMax + newMin );
    HalfExtents = 0.5f * ( newMax - newMin );

    // we adjusted the box.
    return true;
}

//----------------------------------------------------------
bool
Bootstrap::AABox::IsInside( Vector3 point )
{
    Vector3 relativePos = ( point - Center );
    return (
        ::Abs( relativePos.X ) < HalfExtents.X &&
        ::Abs( relativePos.Y ) < HalfExtents.Y &&
        ::Abs( relativePos.Z ) < HalfExtents.Z
        );
}

//----------------------------------------------------------
int
Bootstrap::AABox::GetSide( Plane plane )
{
    float hX = HalfExtents.X;
    float hY = HalfExtents.Y;
    float hZ = HalfExtents.Z;

    // determine if all points are on one side.  If not, then we intersect the plane.
    int side = plane.GetSide( Center + HalfExtents, 0.0f );
    if ( side != plane.GetSide( Center + Vector3( -hX, hY, hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( hX, -hY, hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( -hX, -hY, hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( hX, hY, -hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( -hX, hY, -hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( hX, -hY, -hZ ), 0.0f ) )
        return 0;
    if ( side != plane.GetSide( Center + Vector3( -hX, -hY, -hZ ), 0.0f ) )
        return 0;

    // return the side we lie on.
    return side;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::AABox::ToString()
{
    return "Center " + Center + " HalfExtents " + HalfExtents;
}