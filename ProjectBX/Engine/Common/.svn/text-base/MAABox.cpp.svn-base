//----------------------------------------------------------
// File:		MAABox.cpp
// Author:		Kevin Bray
// Created:		09-06-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MAABox.h"

// project includes.
#include "MPlane.h"
#include "MSphere.h"
#include "MOBox.h"

static inline float Sqr( float x )	{	return x*x;		}

//**********************************************************
// class MAABox
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MAABox::MAABox( const MVec3& center, const MVec3& halfExts )
: _center( center )
, _halfExts( halfExts.Abs() )
{

}

//----------------------------------------------------------
MAABox::MAABox( const MOBox& box )
{
	// build from the corners of the oriented box.
	MVec3 corners[ 8 ];
	box.GetCorners( corners );

	// iterate through each corner and keep the min/max.
	MVec3 minExts( FLT_MAX, FLT_MAX, FLT_MAX );
	MVec3 maxExts( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	for ( unsigned int i = 0; i < 8; ++i )
	{
		// calculate the min.
		for ( unsigned int j = 0; j < 3; ++j )
			minExts[ j ] = Min( minExts[ j ], corners[ i ][ j ] );

		// calculate the max.
		for ( unsigned int j = 0; j < 3; ++j )
			maxExts[ j ] = Max( maxExts[ j ], corners[ i ][ j ] );
	}

	// calculate the center and the extents.
	_center = 0.5f * ( maxExts + minExts );
	_halfExts = 0.5f * ( maxExts - minExts );
}

//----------------------------------------------------------
MAABox::~MAABox()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
float
MAABox::Dist( const MVec3& pos )
{
	float boxMin[ 3 ] =
	{
		_center.GetX() - _halfExts.GetX(),
		_center.GetY() - _halfExts.GetY(),
		_center.GetZ() - _halfExts.GetZ(),
	};
	float boxMax[ 3 ] =
	{
		_center.GetX() + _halfExts.GetX(),
		_center.GetY() + _halfExts.GetY(),
		_center.GetZ() + _halfExts.GetZ(),
	};

	// calculate distance to the box along each axis.
	float axisDist[ 3 ] = { 0.0f, 0.0f, 0.0f };
	for ( unsigned int i = 0; i < 3; ++i )
	{
		float axisMax = _center[ i ] + _halfExts[ i ];
		if ( pos[ i ] > boxMax[ i ] )
		{
			axisDist[ i ] = pos[ i ] - boxMax[ i ];
		}
		else
		{
			float axisMin = _center[ i ] + _halfExts[ i ];
			if ( pos[ i ] < boxMin[ i ] )
				axisDist[ i ] = boxMin[ i ] - pos[ i ];
		}
	}

	// now calculate the length of the resulting vector.
	float magSqr = axisDist[ 0 ] * axisDist[ 0 ] +
				   axisDist[ 1 ] * axisDist[ 1 ] +
				   axisDist[ 2 ] * axisDist[ 2 ];
	return Sqrt( magSqr );
}

//----------------------------------------------------------
bool
MAABox::Intersect( const MAABox& box ) const
{
	// get the distance from centers along each axis.
	MVec3 dist( ( _center - box._center ).Abs() );

	// check to see if we're too far to intersect on any axes.
	if ( dist.GetX() > ( _halfExts.GetX() + box._halfExts.GetX() ) )
		return false;
	if ( dist.GetY() > ( _halfExts.GetY() + box._halfExts.GetY() ) )
		return false;
	if ( dist.GetZ() > ( _halfExts.GetZ() + box._halfExts.GetZ() ) )
		return false;

	// we're too close on all axes, and therefore, we intersect!
	return true;
}

//----------------------------------------------------------
bool
MAABox::Intersect( const MSphere& sphere ) const
{
	float dist = 0.0f;

	MVec3 boxMin = _center - _halfExts;
	MVec3 boxMax = _center + _halfExts;

    // X Axis
    if ( sphere.GetCenter().GetX() < boxMin.GetX() )
		dist += Sqr( sphere.GetCenter().GetX() - boxMin.GetX() );
    else if ( sphere.GetCenter().GetX() > boxMax.GetX() ) 
		dist += Sqr( sphere.GetCenter().GetX() - boxMax.GetX() );

    // Y Axis
    if ( sphere.GetCenter().GetY() < boxMin.GetY() ) 
		dist += Sqr( sphere.GetCenter().GetY() - boxMin.GetY() ); 
    else if ( sphere.GetCenter().GetY() > boxMax.GetY() ) 
		dist += Sqr( sphere.GetCenter().GetY() - boxMax.GetY() );

    // Z Axis
    if ( sphere.GetCenter().GetZ() < boxMin.GetZ() ) 
		dist += Sqr( sphere.GetCenter().GetZ() - boxMin.GetZ() ); 
    else if ( sphere.GetCenter().GetZ() > boxMax.GetZ() ) 
		dist += Sqr( sphere.GetCenter().GetZ() - boxMax.GetZ() );

	// if the distance to the box is less than the radius of the sphere...
    return dist <= Sqr( sphere.GetRadius() );
}

//----------------------------------------------------------
MSphere
MAABox::GetBoundingSphere()
{
	return MSphere( _center, _halfExts.Mag() );
}

//----------------------------------------------------------
bool
MAABox::AddBoxToVolume( const MAABox& box )
{
	// calculate min/max extents.
	MVec3 boxMin = box._center - box._halfExts;
	MVec3 boxMax = box._center + box._halfExts;
	MVec3 thisMin = _center - _halfExts;
	MVec3 thisMax = _center + _halfExts;

	// do we need to do anything?
	if ( boxMin.GetX() >= thisMin.GetX() &&
		 boxMin.GetY() >= thisMin.GetY() &&
		 boxMin.GetZ() >= thisMin.GetZ() &&
		 boxMax.GetX() <= thisMin.GetX() &&
		 boxMax.GetY() <= thisMin.GetY() &&
		 boxMax.GetZ() <= thisMin.GetZ() )
		return false;

	// calculate the new min/max.
	MVec3 newMin( Min( boxMin.GetX(), thisMin.GetX() ),
				  Min( boxMin.GetY(), thisMin.GetY() ),
				  Min( boxMin.GetZ(), thisMin.GetZ() ) );
	MVec3 newMax( Max( boxMax.GetX(), thisMax.GetX() ),
				  Max( boxMax.GetY(), thisMax.GetY() ),
				  Max( boxMax.GetZ(), thisMax.GetZ() ) );

	// calculate the new center/extents.
	_center = 0.5f * ( newMax + newMin );
	_halfExts = 0.5f * ( newMax - newMin );

	// we adjusted the box.
	return true;
}

//----------------------------------------------------------
bool
MAABox::AddBoxToVolume( const MOBox& box )
{
	// get the corners from the OBox.
	MVec3 pts[ 8 ];
	box.GetCorners( pts );

	MVec3 thisMin = _center - _halfExts;
	MVec3 thisMax = _center + _halfExts;

	// calculate the min/max points of the box passed in.
	MVec3 boxMin = box.GetTransform().GetTranslate();
	MVec3 boxMax = boxMin;
	for ( unsigned int i = 0; i < 8; ++i )
	{
		for ( unsigned int j = 0; j < 3; ++j )
		{
			boxMin[ j ] = Min( pts[ i ][ j ], boxMin[ j ] );
			boxMax[ j ] = Max( pts[ i ][ j ], boxMax[ j ] );
		}
	}

	// do we need to do anything?
	if ( boxMin.GetX() >= thisMin.GetX() &&
		 boxMin.GetY() >= thisMin.GetY() &&
		 boxMin.GetZ() >= thisMin.GetZ() &&
		 boxMax.GetX() <= thisMin.GetX() &&
		 boxMax.GetY() <= thisMin.GetY() &&
		 boxMax.GetZ() <= thisMin.GetZ() )
		return false;

	// calculate the new min/max.
	MVec3 newMin( Min( boxMin.GetX(), thisMin.GetX() ),
				  Min( boxMin.GetY(), thisMin.GetY() ),
				  Min( boxMin.GetZ(), thisMin.GetZ() ) );
	MVec3 newMax( Max( boxMax.GetX(), thisMax.GetX() ),
				  Max( boxMax.GetY(), thisMax.GetY() ),
				  Max( boxMax.GetZ(), thisMax.GetZ() ) );

	// calculate the new center/extents.
	_center = 0.5f * ( newMax + newMin );
	_halfExts = 0.5f * ( newMax - newMin );

	// we adjusted the box.
	return true;
}

//----------------------------------------------------------
void
MAABox::GetCorners( MVec3 *pts ) const
{
	// bottom, back-left.
	pts[ 0 ].SetX( _center.GetX() - _halfExts.GetX() );
	pts[ 0 ].SetY( _center.GetY() - _halfExts.GetY() );
	pts[ 0 ].SetZ( _center.GetZ() - _halfExts.GetZ() );

	// bottom, forward-left.
	pts[ 1 ].SetX( _center.GetX() - _halfExts.GetX() );
	pts[ 1 ].SetY( _center.GetY() - _halfExts.GetY() );
	pts[ 1 ].SetZ( _center.GetZ() + _halfExts.GetZ() );

	// bottom, forward-right.
	pts[ 2 ].SetX( _center.GetX() + _halfExts.GetX() );
	pts[ 2 ].SetY( _center.GetY() - _halfExts.GetY() );
	pts[ 2 ].SetZ( _center.GetZ() + _halfExts.GetZ() );

	// bottom, back-right.
	pts[ 3 ].SetX( _center.GetX() + _halfExts.GetX() );
	pts[ 3 ].SetY( _center.GetY() - _halfExts.GetY() );
	pts[ 3 ].SetZ( _center.GetZ() - _halfExts.GetZ() );

	// top, back-left.
	pts[ 4 ].SetX( _center.GetX() - _halfExts.GetX() );
	pts[ 4 ].SetY( _center.GetY() + _halfExts.GetY() );
	pts[ 4 ].SetZ( _center.GetZ() - _halfExts.GetZ() );

	// top, forward-left.
	pts[ 5 ].SetX( _center.GetX() - _halfExts.GetX() );
	pts[ 5 ].SetY( _center.GetY() + _halfExts.GetY() );
	pts[ 5 ].SetZ( _center.GetZ() + _halfExts.GetZ() );

	// top, forward-right.
	pts[ 6 ].SetX( _center.GetX() + _halfExts.GetX() );
	pts[ 6 ].SetY( _center.GetY() + _halfExts.GetY() );
	pts[ 6 ].SetZ( _center.GetZ() + _halfExts.GetZ() );

	// top, back-right.
	pts[ 7 ].SetX( _center.GetX() + _halfExts.GetX() );
	pts[ 7 ].SetY( _center.GetY() + _halfExts.GetY() );
	pts[ 7 ].SetZ( _center.GetZ() - _halfExts.GetZ() );
/*
	// get all of the points in the AA box.  This can be used for conversion to non-aligned as well.
	pts[ 0 ] = SVec3( pos.x - halfWidth, pos.y - halfHeight, pos.z - halfDepth );		// bottom, back-left.
	pts[ 1 ] = SVec3( pos.x - halfWidth, pos.y - halfHeight, pos.z + halfDepth );		// bottom, forward-left.
	pts[ 2 ] = SVec3( pos.x + halfWidth, pos.y - halfHeight, pos.z + halfDepth );		// bottom, forward-right.
	pts[ 3 ] = SVec3( pos.x + halfWidth, pos.y - halfHeight, pos.z - halfDepth );		// bottom, back-right.
	pts[ 4 ] = SVec3( pos.x - halfWidth, pos.y + halfHeight, pos.z - halfDepth );		// top, back-left.
	pts[ 5 ] = SVec3( pos.x - halfWidth, pos.y + halfHeight, pos.z + halfDepth );		// top, forward-left.
	pts[ 6 ] = SVec3( pos.x + halfWidth, pos.y + halfHeight, pos.z + halfDepth );		// top, forward-right.
	pts[ 7 ] = SVec3( pos.x + halfWidth, pos.y + halfHeight, pos.z - halfDepth );		// top, back-right.
*/
}
/*
//----------------------------------------------------------
bool
MAABox::Intersect( const MSphere& sphere ) const
{
	// get the relative location of the sphere, reflected into the positive
	// cartesian octant.
	MVec3 relLoc = ( _center - sphere.GetCenter() ).Abs();

	// determine the maximum distance along each axis for the sphere to be
	// considered as intersecting.
	float radius = sphere.GetRadius();
	MVec3 maxDist = _halfExts + MVec3( radius, radius, radius );

	// check to see if the point within the maximum distance to be considered
	// as intersecting.
	return ( relLoc.GetX() < maxDist.GetX() ) && ( relLoc.GetY() < maxDist.GetY() ) && ( relLoc.GetZ() < maxDist.GetZ() );
}
*/

//----------------------------------------------------------
bool
MAABox::IsInside( const MVec3& pt ) const
{
	MVec3 relativePos = ( pt - _center );
	return (
		Abs( relativePos.GetX() ) < _halfExts.GetX() &&
		Abs( relativePos.GetY() ) < _halfExts.GetY() &&
		Abs( relativePos.GetZ() ) < _halfExts.GetZ()
		);
}

//----------------------------------------------------------
int
MAABox::GetSide( const MPlane& plane ) const
{
	float hX = _halfExts.GetX();
	float hY = _halfExts.GetY();
	float hZ = _halfExts.GetZ();

	// determine if all points are on one side.  If not, then we intersect the plane.
	int side = plane.GetSide( _center + _halfExts, 0.0f );
	if ( side != plane.GetSide( _center + MVec3( -hX, hY, hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( hX, -hY, hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( -hX, -hY, hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( hX, hY, -hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( -hX, hY, -hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( hX, -hY, -hZ ), 0.0f ) )
		return 0;
	if ( side != plane.GetSide( _center + MVec3( -hX, -hY, -hZ ), 0.0f ) )
		return 0;

	// return the side we lie on.
	return side;
}

//----------------------------------------------------------
void
MAABox::Save( UWriter& writer ) const
{
	_center.Save( writer );
	_halfExts.Save( writer );
}

//----------------------------------------------------------
void
MAABox::Load( UReader& reader )
{
	_center.Load( reader );
	_halfExts.Load( reader );
}
