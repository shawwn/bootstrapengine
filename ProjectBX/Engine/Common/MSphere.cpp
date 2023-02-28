//----------------------------------------------------------
// File:        MSphere.cpp
// Author:      Kevin Bray
// Created:     09-05-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header
#include "MSphere.h"

// project includes.
#include "MPlane.h"
#include "UReader.h"
#include "UWriter.h"
#include "MMat4x4.h"

//**********************************************************
// class MSphere
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MSphere::MSphere()
: _center()
, _radius( 1.0f )
{

}

//----------------------------------------------------------
MSphere::MSphere( MVec3 center, float radius )
: _center( center )
, _radius( radius )
{
    B_ASSERT( radius >= 0.0f );
}

//----------------------------------------------------------
MSphere::~MSphere()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
MSphere::Intersect( const MSphere& sphere ) const
{
    float maxDistSqr = _radius + sphere.GetRadius();
    maxDistSqr *= maxDistSqr;
    return ( _center.DistSqr( sphere.GetCenter() ) < maxDistSqr );
}

//----------------------------------------------------------
bool
MSphere::IsInside( const MSphere& sphere ) const
{
    return ( ( _center.Dist( sphere.GetCenter() ) + sphere.GetRadius() ) < _radius );
}

//----------------------------------------------------------
void
MSphere::SetRadius( float radius )
{
//  B_ASSERT( radius > 0.0f );
    _radius = radius;
}

//----------------------------------------------------------
int
MSphere::GetSide( const MPlane& plane ) const
{
    float dist = plane.Dist( _center );
    if ( Abs( dist ) > _radius )
        return dist >= 0.0f ? 1 : -1;
    return 0;
}

//----------------------------------------------------------
int
MSphere::GetSection( const MPlane& plane, MVec3& center, float& radius ) const
{
    // get the distance to the plane.  If we're entirely on one side, return
    // the indication as such.
    float dist = plane.Dist( _center );
    if ( Abs( dist ) > _radius )
        return dist > 0.0f ? 1 : -1;

    // determine the radius of our cross section.
    radius = Sqrt( _radius*_radius - dist*dist );

    // determine the point of intersection along the plane normal.
    center = _center - dist * plane.GetNormal();

    // we intersect and are returning a valid section.
    return 0;
}

//----------------------------------------------------------
void
MSphere::Transform( MSphere& sphere, const MMat4x4& transform ) const
{
    MVec3 scales;
    transform.GetScaleSqr( scales );

    // determine the maximum scale.
    sphere.SetRadius( Sqrt( scales.Max() ) * _radius );

    // transform the center.
    sphere.SetCenter( transform.TransformCoord( _center ) );
}

//----------------------------------------------------------
void
MSphere::TransformFast( const MMat4x4& transform )
{
    MVec3 scales;
    transform.GetScaleSqr( scales );

    // determine the maximum scale.
    _radius = Sqrt( scales.Max() ) * _radius;

    // transform the center.
    transform.TransformCoordFast( _center );
}

//----------------------------------------------------------
void
MSphere::Save( UWriter& writer ) const
{
    _center.Save( writer );
    writer.WriteFloat( _radius );
}

//----------------------------------------------------------
void
MSphere::Load( UReader& reader )
{
    _center.Load( reader );
    _radius = reader.ReadFloat();
}
