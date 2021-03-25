//----------------------------------------------------------
// File:		Color.cpp
// Author:		Shawn Presser
// Created:		09-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Color.h"

// engine headers.
#include "GrColor.h"

//**********************************************************
// value class Color
//**********************************************************

//----------------------------------------------------------
Bootstrap::Color::Color( const GrColor& color )
{
	R = color.GetR();
	G = color.GetG();
	B = color.GetB();
	A = color.GetA();
}

//----------------------------------------------------------
GrColor 
Bootstrap::Color::ToGrColor()
{
	return GrColor( R, G, B, A );
}

//----------------------------------------------------------
Bootstrap::Color::Color( float r, float g, float b, float a )
{
	R = r;
	G = g;
	B = b;
	A = a;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Color::ToString()
{
	return "#( " + R + ", " + G + ", " + B + ", " + A + " )";
}		

//----------------------------------------------------------
Bootstrap::Color::operator Color( System::Drawing::Color col )
{
    return Color( 
        col.R / 255.0f, 
        col.G / 255.0f, 
        col.B / 255.0f, 
        col.A / 255.0f );
}

//----------------------------------------------------------
Bootstrap::Color::operator System::Drawing::Color( Color col )
{
    return System::Drawing::Color::FromArgb( 
        ( int )( Min( col.A, 1.0f ) * 255 ),
        ( int )( Min( col.R, 1.0f ) * 255 ), 
        ( int )( Min( col.G, 1.0f ) * 255 ), 
        ( int )( Min( col.B, 1.0f ) * 255 )
        );
}

//----------------------------------------------------------
Bootstrap::Color
Bootstrap::Color::operator + ( Bootstrap::Color col1, Bootstrap::Color col2 )
{
	return Color(
		col1.R + col2.R,
		col1.G + col2.G,
		col1.B + col2.B,
		col1.A + col2.A );
}

//----------------------------------------------------------
Bootstrap::Color
Bootstrap::Color::operator - ( Bootstrap::Color col1, Bootstrap::Color col2 )
{
	return Color(
		col1.R - col2.R,
		col1.G - col2.G,
		col1.B - col2.B,
		col1.A - col2.A );
}

//----------------------------------------------------------
Bootstrap::Color
Bootstrap::Color::operator * ( float s, Bootstrap::Color col )
{
	return Color(
		s * col.R,
		s * col.G,
		s * col.B,
		s * col.A );
}

//----------------------------------------------------------
Bootstrap::Color::operator GrColor( Color col )
{
    return col.ToGrColor();
}
