//----------------------------------------------------------
// File:		Line.h
// Author:		Shawn Presser
// Created:		09-20-08
//
// Purpose:		To represent a line in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Vector3.h"

// forward declarations.
class MLine;

namespace Bootstrap 
{
    //**********************************************************
    // value class Line
    //**********************************************************
	public value class Line
	{
	internal:
        // constructs a line from an MLine.
		Line( const MLine& line );

        // converts the line to an MLine.
		MLine               ToMLine();

	public:
        // constructs a line with the specified direction 
        // that passes through the specified point.
		Line( Vector3 point, Vector3 dir );

        // gets / sets a point on the line.
		property Vector3    Point
		{
			Vector3             get()                   {   return _point;              }
			void                set( Vector3 point )    {   _point = point;             }
		}

        // gets / sets the direction of the line.
		property Vector3    Dir
		{
			Vector3             get()                   {   return _dir;                }
			void                set( Vector3 dir )      {   _dir = dir.Normalized;    }			
		}

        // pretty-prints the line.
		virtual System::String^     ToString() override;

	internal:
        // auto-converts a Line to an MLine.
		static              operator MLine( Line v );

        // auto-converts an MLine to a Line.
		static              operator Line( const MLine& v );

	private:
		Vector3			    _point;
		Vector3			    _dir;
	};
}
