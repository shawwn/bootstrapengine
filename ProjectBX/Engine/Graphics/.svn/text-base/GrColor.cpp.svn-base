//----------------------------------------------------------
// File:		GrColor.cpp
// Author:		Kevin Bray
// Created:		10-28-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrColor.h"

#include "UReader.h"
#include "UWriter.h"

// unpacks a floating point value.
static inline unsigned char PackFloatColor( float v )
{
	return ( unsigned char )( 255.0f * Clamp( v, 0.0f, 1.0f ) + 0.5f );
}

//**********************************************************
// class GrColor
//**********************************************************

//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrColor::IsGray( float epsilon ) const
{
	return ( Abs( _r - _g ) < epsilon && 
			 Abs( _r - _b ) < epsilon && 
			 Abs( _r - _g ) < epsilon && 
			 Abs( _r - _a ) < epsilon );
}

//----------------------------------------------------------
bool
GrColor::ApproxEqual( const GrColor& color, float epsilon ) const
{
	return ( Abs( _r - color.GetR() ) <= epsilon &&
			 Abs( _g - color.GetG() ) <= epsilon &&
			 Abs( _b - color.GetB() ) <= epsilon &&
			 Abs( _a - color.GetA() ) <= epsilon );	 
}

//----------------------------------------------------------
GrColor
GrColor::Min( const GrColor& color ) const
{
	return GrColor( ::Min( _r, color.GetR() ), ::Min( _g, color.GetG() ), ::Min( _b, color.GetB() ),
		::Min( _a, color.GetA() ) );
}

//----------------------------------------------------------
GrColor
GrColor::Max( const GrColor& color ) const
{
	return GrColor( ::Max( _r, color.GetR() ), ::Max( _g, color.GetG() ), ::Max( _b, color.GetB() ),
		::Max( _a, color.GetA() ) );
}

//----------------------------------------------------------
bool
GrColor::IsBetween( const GrColor& color1, const GrColor& color2 ) const
{
	GrColor min = color1.Min( color2 );
	GrColor max = color1.Max( color2 );
	return ( ( _r >= min.GetR() && _g >= min.GetG() && _b >= min.GetB() ) &&
			 ( _r <= max.GetR() && _g <= max.GetG() && _b <= max.GetB() ) );
}

//----------------------------------------------------------
void
GrColor::PackToBGR( unsigned char* bgra ) const
{
	bgra[ 0 ] = PackFloatColor( _b );
	bgra[ 1 ] = PackFloatColor( _g );
	bgra[ 2 ] = PackFloatColor( _r );
}

//----------------------------------------------------------
void
GrColor::PackToBGRA( unsigned char* bgra ) const
{
	bgra[ 0 ] = PackFloatColor( _b );
	bgra[ 1 ] = PackFloatColor( _g );
	bgra[ 2 ] = PackFloatColor( _r );
	bgra[ 3 ] = PackFloatColor( _a );
}

//----------------------------------------------------------
void
GrColor::PackToRGBA( unsigned char* rgba ) const
{
	rgba[ 0 ] = PackFloatColor( _r );
	rgba[ 1 ] = PackFloatColor( _g );
	rgba[ 2 ] = PackFloatColor( _b );
	rgba[ 3 ] = PackFloatColor( _a );
}

//----------------------------------------------------------
void
GrColor::Save( UWriter& writer ) const
{
	writer.WriteFloat( _r );
	writer.WriteFloat( _g );
	writer.WriteFloat( _b );
	writer.WriteFloat( _a );
	
}

//----------------------------------------------------------
void
GrColor::Load( UReader& reader )
{
	_r = reader.ReadFloat();
	_g = reader.ReadFloat();
	_b = reader.ReadFloat();
	_a = reader.ReadFloat();
}
