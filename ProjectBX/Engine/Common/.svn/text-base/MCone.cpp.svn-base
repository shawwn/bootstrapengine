//----------------------------------------------------------
// File:		MCone.cpp
// Author:		Kevin Bray
// Created:		09-04-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MCone.h"

// project includes.
#include "MPlane.h"

//**********************************************************
// class MCone
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MCone::MCone( MVec3 point, MVec3 base, float baseRadius )
: _point( point )
, _base( base )
, _baseRadius( baseRadius )
{
	B_ASSERT( baseRadius > 0.0f );
}

//----------------------------------------------------------
MCone::~MCone()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MCone::SetBaseRadius( float baseRadius )
{
	B_ASSERT( baseRadius > 0.0f );
	_baseRadius = baseRadius;
}

//----------------------------------------------------------
int
MCone::GetSide( const MPlane& plane ) const
{
	float pointDist = plane.Dist( _point );
	float baseDist = plane.Dist( _base );

	// if the line crosses the plane, we're colliding.
	if ( ( pointDist >= 0.0f ) != ( baseDist >= 0.0f ) )
		return 0;

	// both points are on the same side of the plane, so we need to
	// determine the distance from the plane to the base of the cone.

	// get the shortest perpendicular to the line that intersects with
	// the plane.  To do this, we need to first calculate the perpendicular
	// to the shortest line on the base that intersects the plane.
	MVec3 line = _base - _point;
	MVec3 perp = line.Cross( plane.GetNormal() );

	// if the perpendicular's length is 0, then the base cannot possibly
	// intersect with the plane.  Therefor, we're entirely on one side.
	if ( perp.MagSqr() < 0.0001f )
		return ( pointDist > 0.0f ? 1 : -1 );

	// if a perpendicular exists that intersects with the plane and the line
	// on the base, then we need to determine the shortest line on the base
	// plane that intersects with the plane in question.  first, we should
	// calculate the direction vector that would intersect the plane along
	// the shortest line.
	MVec3 baseLine = perp.Cross( line );

	// now, take the cosine to get the ratio of the distance to the plane
	// from the base point and the distance to the plane along the base line.
	baseLine.Normalize();
	float cosBaseNormal = baseLine.Dot( plane.GetNormal() );

	// multiply the base distance by the inverse of the cosine between the
	// base line vector and the plane's normal.
	// ( simplified to baseDist / cosBaseNormal )
	float coneDist = Abs( baseDist ) / cosBaseNormal;
	if ( coneDist <= _baseRadius )
		return 0;

	// if we get here, we're entirely on one side of the plane.
	return ( baseDist > 0.0f ? 1 : -1 );
}
