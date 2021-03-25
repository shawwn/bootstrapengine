//----------------------------------------------------------
// File:		MLineSegment.cpp
// Author:		Kevin Bray
// Created:		06-28-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MLineSegment.h"

// project includes.


//**********************************************************
// class MLineSegment
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MLineSegment::MLineSegment()
: _pt0( 0.0f, 0.0f, 0.0f )
, _pt1( 0.0f, 0.0f, 1.0f )
{

}

//----------------------------------------------------------
MLineSegment::MLineSegment( const MVec3& pt0, const MVec3& pt1 )
: _pt0( pt0 )
, _pt1( pt1 )
{

}

//----------------------------------------------------------
MLineSegment::~MLineSegment()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MLineSegment::Reset( const MVec3& pt0, const MVec3& pt1 )
{
	_pt0 = pt0;
	_pt1 = pt1;
}

//----------------------------------------------------------
float
MLineSegment::Dist( const MVec3& pt )
{
	// return the shortest distance to the line from the point.
	MVec3 hyp = ( pt - _pt0 );
	MVec3 vec = _pt1 - _pt0;
	float vecMag = vec.Mag();
	if ( vecMag < 0.00001f )
		return _pt1.Dist( pt );

	// calculate the direction vector's length and normalize it.
	float invLen = 1.0f / vecMag;
	MVec3 dir = invLen * vec;

	// calculate where the line comes closest to point 'pt'.
	float t = invLen * dir.Dot( hyp );

	// if we don't come closest to the point 'pt' somewhere on
	// the segment, take the distance to the respective end
	// points.
	if ( t <= 0.0f )
		return _pt0.Dist( pt );
	else if ( t >= 1.0f )
		return _pt1.Dist( pt );

	// return the distance to the line.
	return hyp.Dist( t * vec );
}
