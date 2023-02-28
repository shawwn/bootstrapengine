//----------------------------------------------------------
// File:		Vector2.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To perform vector operations in 2D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec2;
struct SVec2;

namespace Bootstrap 
{
    //**********************************************************
    // value class Vector2
    //**********************************************************
	public value class Vector2
	{
	internal:
		Vector2( const MVec2& vec );
		Vector2( const SVec2& vec );

		MVec2                       ToMVec2();
		SVec2                       ToSVec2();

	public:
		Vector2( float x, float y );

		property float              X;
		property float              Y;

		property Vector2            Normalized
        {
            Vector2                     get();
        }

        // math operators.
		static Vector2              operator + ( Vector2 left, Vector2 right );
		static Vector2              operator - ( Vector2 left, Vector2 right );
		static Vector2              operator - ( Vector2 value );
		static Vector2              operator * ( Vector2 vector, float scale );
		static Vector2              operator * ( float scale, Vector2 vector );
		static Vector2              operator / ( Vector2 vector, float scale );

        // pretty-prints the vector.
		virtual System::String^     ToString() override;

		// equality.
		virtual bool				Equals( System::Object^ obj ) override;
		virtual int					GetHashCode() override;

	internal:
		static                      operator MVec2( Vector2 v );
		static                      operator Vector2( const MVec2& v );
	};
}
