//----------------------------------------------------------
// File:		MVec2.h
// Author:		Kevin Bray
// Created:		10-05-04
//
// Purpose:		performs vector operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class MVec2
//**********************************************************
class MVec2
{
public:
	MVec2();
	MVec2( float x, float y );
	MVec2( const MVec2& v );
	~MVec2();

	MVec2&			operator = ( const MVec2& v );

	float			GetX() const	{	return _data[ 0 ];	}
	float			GetY() const	{	return _data[ 1 ];	}
	void			SetX( float x ) {	_data[ 0 ] = x;		}
	void			SetY( float y ) {	_data[ 1 ] = y;		}

	float&			operator ()( int i );
	float			operator ()( int i ) const;

	// math operators
	MVec2			operator * ( const MVec2& v ) const;
	MVec2			operator / ( const MVec2& v ) const;
	MVec2			operator + ( const MVec2& v ) const;
	MVec2			operator - ( const MVec2& v ) const;
	MVec2&			operator *= ( const MVec2& v );
	MVec2&			operator *= ( float scale );
	MVec2&			operator /= ( const MVec2& v );
	MVec2&			operator += ( const MVec2& v );
	MVec2&			operator -= ( const MVec2& v );

	friend MVec2	operator * ( float scale, const MVec2& v );

private:
	float			_array[ 7 ];
	float *			_data;
};
