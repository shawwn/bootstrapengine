//----------------------------------------------------------
// File:		Line.cpp
// Author:		Shawn Presser
// Created:		09-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Line.h"

// engine headers.
#include "MLine.h"

//**********************************************************
// value class Line
//**********************************************************

//----------------------------------------------------------
Bootstrap::Line::Line( const MLine& line )
{
	_point = line.GetPt();
	_dir = line.GetDir();
}

//----------------------------------------------------------
MLine 
Bootstrap::Line::ToMLine()
{
	return MLine( _point.ToMVec3(), _dir.ToMVec3() );
}

//----------------------------------------------------------
Bootstrap::Line::Line( Vector3 point, Vector3 dir )
{
	_point = point;
	_dir = dir.Normalized;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Line::ToString()
{
	return "Pt " + _point.ToString() + " Dir " + _dir.ToString();
}		

//----------------------------------------------------------
Bootstrap::Line::operator MLine( Line v )
{
    return v.ToMLine();
}

//----------------------------------------------------------
Bootstrap::Line::operator Line( const MLine& v )
{
    return Line( v );
}
