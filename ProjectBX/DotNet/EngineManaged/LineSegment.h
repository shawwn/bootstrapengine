//----------------------------------------------------------
// File:		LineSegment.h
// Author:		Shawn Presser
// Created:		09-22-08
//
// Purpose:		To represent a line segment in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "Vector3.h"

// forward declarations.
class MLineSegment;

namespace Bootstrap 
{
    //**********************************************************
    // value class LineSegment
    //**********************************************************
	public value class LineSegment
	{
	internal:
        // constructs a line segment from an MLineSegment.
		LineSegment( const MLineSegment& line );

        // converts from a LineSegment to an MLineSegment.
		MLineSegment        ToMLineSegment();

	public:
        // constructs a line segment 
		LineSegment( Vector3 start, Vector3 end );

        // gets / sets the starting point.
		property Vector3    Start
		{
			Vector3             get()                   {   return _start;              }
			void                set( Vector3 start )    {   _start = start;             }
		}

        // gets / sets the ending point.
		property Vector3    End
		{
			Vector3             get()                   {   return _end;                }
			void                set( Vector3 end )      {   _end = end;                 }			
		}

	internal:
        // auto-converts from a LineSegment to an MLineSegment.
		static              operator MLineSegment( LineSegment v );

        // auto-converts from an MLineSegment to a LineSegment.
		static              operator LineSegment( const MLineSegment& v );

	private:
		Vector3			    _start;
		Vector3			    _end;
	};
}
