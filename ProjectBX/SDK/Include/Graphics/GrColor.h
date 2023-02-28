//----------------------------------------------------------
// File:		GrColor.h
// Author:		Kevin Bray
// Created:		10-29-04
//
// Purpose:		To contain a color value.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

class UReader;
class UWriter;

//**********************************************************
// class GrColor
//**********************************************************
class GrColor
{
public:
	GrColor() : _r( 1.0f ), _g( 1.0f ), _b( 1.0f ), _a( 1.0f ) { }
	GrColor( float intensity ) : _r( intensity ), _g( intensity ), _b( intensity ), _a( intensity ) { }
	GrColor( float luminance, float a ) : _r( luminance ), _g( luminance ), _b( luminance ), _a( a ) { }
	GrColor( float r, float g, float b, float a = 1.0f ) : _r( r ), _g( g ), _b( b ), _a( a ) { }

	void					SetR( float r )	{	_r = r;		}
	void					SetG( float g )	{	_g = g;		}
	void					SetB( float b )	{	_b = b;		}
	void					SetA( float a )	{	_a = a;		}

	float					GetR() const	{	return _r;	}
	float					GetG() const	{	return _g;	}
	float					GetB() const	{	return _b;	}
	float					GetA() const	{	return _a;	}

	float					MaxRGB() const		{	return ::Max( _r, ::Max( _g, _b ) );				}
	float					MaxRGBA() const		{	return ::Max( _r, ::Max( _g, ::Max( _b, _a ) ) );		}

	operator float *		()				{	return _rgba;		}
	operator const float *	() const		{	return _rgba;		}

	bool					IsGray( float epsilon = 0.001 ) const;
	bool					ApproxEqual( const GrColor& color, float epsilon = 0.001 ) const;

	GrColor					Min( const GrColor& color ) const;
	GrColor					Max( const GrColor& color ) const;

	bool					IsBetween( const GrColor& color1, const GrColor& color2 ) const;

	// packs to a BGRA color.
	void					PackToBGR( unsigned char* bgra ) const;
	void					PackToBGRA( unsigned char* bgra ) const;
	void					PackToRGBA( unsigned char* rgba ) const;

	// saving and loading.
	void					Save( UWriter& writer ) const;
	void					Load( UReader& reader );

private:
	union
	{
		struct
		{
			float	_r;
			float	_g;
			float	_b;
			float	_a;
		};
		float	_rgba[ 4 ];
	};
};

//----------------------------------------------------------
// scales the color.
inline
GrColor
operator * ( float s, const GrColor& color )
{
	return GrColor( s*color.GetR(), s*color.GetG(), s*color.GetB(), s*color.GetA() );
}
