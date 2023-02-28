//----------------------------------------------------------
// File:		MLine.cpp
// Author:		Kevin Bray
// Created:		09-05-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MLine.h"

// project includes.
#include "UReader.h"
#include "UWriter.h"

//**********************************************************
// class MLine
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MLine::MLine()
: _dir( 0.0f, 1.0f, 0.0f )
, _pt( 0.0f, 0.0f, 0.0f )
{

}

//----------------------------------------------------------
MLine::MLine( const MVec3& dir, const MVec3& pt )
: _dir( dir )
, _pt( pt )
{
	B_ASSERT( Abs( dir.MagSqr() - 1.0f ) < 0.001 );
}

//----------------------------------------------------------
MLine::~MLine()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
MLine::SetDir( const MVec3& dir )
{
	// make sure the line direction vector is approximately normalized.
	B_ASSERT( Abs( dir.MagSqr() - 1.0f ) < 0.001 );
	_dir = dir;
}

//----------------------------------------------------------
float
MLine::Dist( const MVec3& pt )
{
	// return the shortest distance to the line from the point.
	MVec3 hyp = ( pt - _pt );
	float t = _dir.Dot( hyp );
	return pt.Dist( t * _dir );
}

//----------------------------------------------------------
bool
MLine::IsCoLinear( const MLine& line, float tolerance ) const
{
	if ( 1.0f - _dir.Dot( line._dir ) > tolerance )
		return false;

	// we know they're going the same direction, so we should determine
	// if they pass through the same point.
	MVec3 dir = line._pt - _pt;
	dir.Normalize();

	// return true if colinear.
	return ( 1.0f - dir.Dot( _dir ) ) <= tolerance;
}

//----------------------------------------------------------
void
MLine::Load( UReader& reader )
{
	_dir.Load( reader );
	_pt.Load( reader );
}

//----------------------------------------------------------
void
MLine::Save( UWriter& writer ) const
{
	_dir.Save( writer );
	_pt.Save( writer );
}
