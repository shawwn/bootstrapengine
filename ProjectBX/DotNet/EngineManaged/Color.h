//----------------------------------------------------------
// File:		Color.h
// Author:		Shawn Presser
// Created:		09-20-08
//
// Purpose:		To contain a color value.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrColor;

namespace Bootstrap
{
    //**********************************************************
    // value class Color
    //**********************************************************
	public value class Color
	{
	internal:
        // constructs a color from a GrColor.
		Color( const GrColor& color );

        // converts the color to a GrColor.
		GrColor                     ToGrColor();

	public:
        // constructs a color from an RGBA value.  Note that 
        // (1.0f, 1.0f, 1.0f, 1.0f) is "full white".
		Color( float r, float g, float b, float a );

        // the color components.
		property float              B;
		property float              G;
		property float              R;
		property float              A;

        // pretty-prints the color.
		virtual System::String^     ToString() override;

        // auto-converts a System.Drawing.Color to a Bootstrap.Color.
		static                      operator Color( System::Drawing::Color col );
		
        // auto-converts a Bootstrap.Color to a System.Drawing.Color.
		static                      operator System::Drawing::Color( Color col );

		// math operators.
		static Color				operator + ( Color col1, Color col2 );
		static Color				operator - ( Color col1, Color col2 );
		static Color				operator * ( float s, Color col );

	internal:
        // auto-converts a Bootstrap.Color to a GrColor.
		static                      operator GrColor( Color col );
	};
}