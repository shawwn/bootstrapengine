//----------------------------------------------------------
// File:		LineSegment.cpp
// Author:		Shawn Presser
// Created:		09-22-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "LineSegment.h"

// engine headers.
#include "MLineSegment.h"

//**********************************************************
// value class LineSegment
//**********************************************************

//----------------------------------------------------------
Bootstrap::LineSegment::LineSegment( const MLineSegment& line )
{
	_start = line.GetPt0();
	_end = line.GetPt1();
}

//----------------------------------------------------------
MLineSegment 
Bootstrap::LineSegment::ToMLineSegment()
{
	return MLineSegment( _start.ToMVec3(), _end.ToMVec3() );
}

//----------------------------------------------------------
Bootstrap::LineSegment::LineSegment( Vector3 start, Vector3 end )
{
	_start = start;
	_end = end;
}

//----------------------------------------------------------
Bootstrap::LineSegment::operator MLineSegment( LineSegment v )
{
	return v.ToMLineSegment();
}

//----------------------------------------------------------
Bootstrap::LineSegment::operator LineSegment( const MLineSegment& v )
{
	return LineSegment( v );
}