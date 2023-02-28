//----------------------------------------------------------
// File:		MMat4x4.cpp
// Author:		Kevin Bray
// Created:		09-25-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MMat4x4.h"

// project includes.
#include "MMat3x3.h"
#include "MVec3.h"
#include "MVec4.h"
#include "UReader.h"
#include "UWriter.h"

// std c++ includes.
#include <math.h>

const float
MMat4x4::kInverseDetEpsilon = 0.00000095367431640625f;

MMat4x4
MMat4x4::_identity;

const MMat4x4&
MMat4x4::Identity()
{
	return _identity;
}

//**********************************************************
// class Mat4x4
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MMat4x4::MMat4x4()
{
	// initialize the array to 0.
	MemSet( _array, 0, sizeof( _array ) );

	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// set to identity.
	_data[ 0 ] = 1.0f;
	_data[ 5 ] = 1.0f;
	_data[ 10 ] = 1.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( float scale )
{
	// initialize the array to 0.
	MemSet( _array, 0, sizeof( _array ) );

	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// initialize the data.
	_data[ 0 ] = scale;
	_data[ 1 ] = 0.0f;
	_data[ 2 ] = 0.0f;
	_data[ 3 ] = 0.0f;

	_data[ 4 ] = 0.0f;
	_data[ 5 ] = scale;
	_data[ 6 ] = 0.0f;
	_data[ 7 ] = 0.0f;

	_data[ 8 ] = 0.0f;
	_data[ 9 ] = 0.0f;
	_data[ 10 ] = scale;
	_data[ 11 ] = 0.0f;

	_data[ 12 ] = 0.0f;
	_data[ 13 ] = 0.0f;
	_data[ 14 ] = 0.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const MVec3& translation )
{
	// initialize the array to 0.
	MemSet( _array, 0, sizeof( _array ) );

	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// initialize the data.
	_data[ 0 ] = 1.0f;
	_data[ 1 ] = 0.0f;
	_data[ 2 ] = 0.0f;
	_data[ 3 ] = translation.GetX();

	_data[ 4 ] = 0.0f;
	_data[ 5 ] = 1.0f;
	_data[ 6 ] = 0.0f;
	_data[ 7 ] = translation.GetY();

	_data[ 8 ] = 0.0f;
	_data[ 9 ] = 0.0f;
	_data[ 10 ] = 1.0f;
	_data[ 11 ] = translation.GetZ();

	_data[ 12 ] = 0.0f;
	_data[ 13 ] = 0.0f;
	_data[ 14 ] = 0.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const MMat3x3& rot )
{
	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// set the matrix data.
	_data[ 0 ] = rot( 0, 0 );
	_data[ 1 ] = rot( 0, 1 );
	_data[ 2 ] = rot( 0, 2 );
	_data[ 3 ] = 0.0f;

	_data[ 4 ] = rot( 1, 0 );
	_data[ 5 ] = rot( 1, 1 );
	_data[ 6 ] = rot( 1, 2 );
	_data[ 7 ] = 0.0f;

	_data[ 8 ] = rot( 2, 0 );
	_data[ 9 ] = rot( 2, 1 );
	_data[ 10 ] = rot( 2, 2 );
	_data[ 11 ] = 0.0f;

	_data[ 12 ] = 0.0f;
	_data[ 13 ] = 0.0f;
	_data[ 14 ] = 0.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const MMat3x3& rot, const MVec3& translation )
{
	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// set the matrix data.
	_data[ 0 ] = rot( 0, 0 );
	_data[ 1 ] = rot( 0, 1 );
	_data[ 2 ] = rot( 0, 2 );
	_data[ 3 ] = translation.GetX();

	_data[ 4 ] = rot( 1, 0 );
	_data[ 5 ] = rot( 1, 1 );
	_data[ 6 ] = rot( 1, 2 );
	_data[ 7 ] = translation.GetY();

	_data[ 8 ] = rot( 2, 0 );
	_data[ 9 ] = rot( 2, 1 );
	_data[ 10 ] = rot( 2, 2 );
	_data[ 11 ] = translation.GetZ();

	_data[ 12 ] = 0.0f;
	_data[ 13 ] = 0.0f;
	_data[ 14 ] = 0.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const MMat3x3& rot, const MVec3& translation, const MVec3& scale )
{
	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// set the matrix data.
	_data[ 0 ] = scale.GetX() * rot( 0, 0 );
	_data[ 1 ] = scale.GetX() * rot( 0, 1 );
	_data[ 2 ] = scale.GetX() * rot( 0, 2 );
	_data[ 3 ] = translation.GetX();

	_data[ 4 ] = scale.GetY() * rot( 1, 0 );
	_data[ 5 ] = scale.GetY() * rot( 1, 1 );
	_data[ 6 ] = scale.GetY() * rot( 1, 2 );
	_data[ 7 ] = translation.GetY();

	_data[ 8 ] = scale.GetZ() * rot( 2, 0 );
	_data[ 9 ] = scale.GetZ() * rot( 2, 1 );
	_data[ 10 ] = scale.GetZ() * rot( 2, 2 );
	_data[ 11 ] = translation.GetZ();

	_data[ 12 ] = 0.0f;
	_data[ 13 ] = 0.0f;
	_data[ 14 ] = 0.0f;
	_data[ 15 ] = 1.0f;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( float _11, float _12, float _13, float _14,
				  float _21, float _22, float _23, float _24,
				  float _31, float _32, float _33, float _34,
				  float _41, float _42, float _43, float _44 )
{
	// initialize the array to 0.
	MemSet( _array, 0, sizeof( _array ) );

	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// initialize the data.
	_data[ 0 ] = _11;
	_data[ 1 ] = _12;
	_data[ 2 ] = _13;
	_data[ 3 ] = _14;

	_data[ 4 ] = _21;
	_data[ 5 ] = _22;
	_data[ 6 ] = _23;
	_data[ 7 ] = _24;

	_data[ 8 ] = _31;
	_data[ 9 ] = _32;
	_data[ 10 ] = _33;
	_data[ 11 ] = _34;

	_data[ 12 ] = _41;
	_data[ 13 ] = _42;
	_data[ 14 ] = _43;
	_data[ 15 ] = _44;
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const float* mat )
{
	// initialize the array to 0.
	MemSet( _array, 0, sizeof( _array ) );

	// make sure _data is set to an aligned array.
	GetAlignedPtr();

	// initialize the data.
	for ( unsigned int i = 0; i < 16; ++i )
		_data[ i ] = mat[ i ];
}

//----------------------------------------------------------
MMat4x4::MMat4x4( const MMat4x4& mat )
{
	// get the aligned pointer and copy the data.
	GetAlignedPtr();
	Copy( mat );
}

//----------------------------------------------------------
MMat4x4::~MMat4x4()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MMat4x4&
MMat4x4::operator = ( const MMat4x4& mat )
{
	Copy( mat );
	return *this;
}

//----------------------------------------------------------
bool
MMat4x4::Inverse( MMat4x4& output ) const
{
	// matrix inversion using Cramer's rule.
	float tmp[ 12 ];
	float src[ 16 ];
	float det;

	// transpose the matrix.
	for ( int i = 0; i < 4; ++i ) {
		src[ i ] = _data[ 4 * i ];
		src[ i + 4 ] = _data[ 4 * i + 1 ];
		src[ i + 8 ] = _data[ 4 * i + 2 ];
		src[ i + 12 ] = _data[ 4 * i + 3 ];
	}

	// calculate pairs for the first 8 elements ( cofactors ).
	tmp[ 0 ] = src[ 10 ] * src[ 15 ];
	tmp[ 1 ] = src[ 11 ] * src[ 14 ];
	tmp[ 2 ] = src[ 9 ] * src[ 15 ];
	tmp[ 3 ] = src[ 11 ] * src[ 13 ];
	tmp[ 4 ] = src[ 9 ] * src[ 14 ];
	tmp[ 5 ] = src[ 10 ] * src[ 13 ];
	tmp[ 6 ] = src[ 8 ] * src[ 15 ];
	tmp[ 7 ] = src[ 11 ] * src[ 12 ];
	tmp[ 8 ] = src[ 8 ] * src[ 14 ];
	tmp[ 9 ] = src[ 10 ] * src[ 12 ];
	tmp[ 10 ] = src[ 8 ] * src[ 13 ];
	tmp[ 11 ] = src[ 9 ] * src[ 12 ];

	// calculate first 8 elements ( cofactors ).
	float dst[ 16 ];

	dst[ 0 ] =  tmp[ 0 ] * src[ 5 ] + tmp[ 3 ] * src[ 6 ] + tmp[ 4 ] * src[ 7 ];
	dst[ 0 ] -= tmp[ 1 ] * src[ 5 ] + tmp[ 2 ] * src[ 6 ] + tmp[ 5 ] * src[ 7 ];
	dst[ 1 ] =  tmp[ 1 ] * src[ 4 ] + tmp[ 6 ] * src[ 6 ] + tmp[ 9 ] * src[ 7 ];
	dst[ 1 ] -= tmp[ 0 ] * src[ 4 ] + tmp[ 7 ] * src[ 6 ] + tmp[ 8 ] * src[ 7 ];
	dst[ 2 ] =  tmp[ 2 ] * src[ 4 ] + tmp[ 7 ] * src[ 5 ] + tmp[ 10 ] * src[ 7 ];
	dst[ 2 ] -= tmp[ 3 ] * src[ 4 ] + tmp[ 6 ] * src[ 5 ] + tmp[ 11 ] * src[ 7 ];
	dst[ 3 ] =  tmp[ 5 ] * src[ 4 ] + tmp[ 8 ] * src[ 5 ] + tmp[ 11 ] * src[ 6 ];
	dst[ 3 ] -= tmp[ 4 ] * src[ 4 ] + tmp[ 9 ] * src[ 5 ] + tmp[ 10 ] * src[ 6 ];
	dst[ 4 ] =  tmp[ 1 ] * src[ 1 ] + tmp[ 2 ] * src[ 2 ] + tmp[ 5 ] * src[ 3 ];
	dst[ 4 ] -= tmp[ 0 ] * src[ 1 ] + tmp[ 3 ] * src[ 2 ] + tmp[ 4 ] * src[ 3 ];
	dst[ 5 ] =  tmp[ 0 ] * src[ 0 ] + tmp[ 7 ] * src[ 2 ] + tmp[ 8 ] * src[ 3 ];
	dst[ 5 ] -= tmp[ 1 ] * src[ 0 ] + tmp[ 6 ] * src[ 2 ] + tmp[ 9 ] * src[ 3 ];
	dst[ 6 ] =  tmp[ 3 ] * src[ 0 ] + tmp[ 6 ] * src[ 1 ] + tmp[ 11 ] * src[ 3 ];
	dst[ 6 ] -= tmp[ 2 ] * src[ 0 ] + tmp[ 7 ] * src[ 1 ] + tmp[ 10 ] * src[ 3 ];
	dst[ 7 ] =  tmp[ 4 ] * src[ 0 ] + tmp[ 9 ] * src[ 1 ] + tmp[ 10 ] * src[ 2 ];
	dst[ 7 ] -= tmp[ 5 ] * src[ 0 ] + tmp[ 8 ] * src[ 1 ] + tmp[ 11 ] * src[ 2 ];

	// calculate pairs for the second 8 elements ( cofactors ).
	tmp[ 0 ] = src[ 2 ] * src[ 7 ];
	tmp[ 1 ] = src[ 3 ] * src[ 6 ];
	tmp[ 2 ] = src[ 1 ] * src[ 7 ];
	tmp[ 3 ] = src[ 3 ] * src[ 5 ];
	tmp[ 4 ] = src[ 1 ] * src[ 6 ];
	tmp[ 5 ] = src[ 2 ] * src[ 5 ];
	tmp[ 6 ] = src[ 0 ] * src[ 7 ];
	tmp[ 7 ] = src[ 3 ] * src[ 4 ];
	tmp[ 8 ] = src[ 0 ] * src[ 6 ];
	tmp[ 9 ] = src[ 2 ] * src[ 4 ];
	tmp[ 10 ] = src[ 0 ] * src[ 5 ];
	tmp[ 11 ] = src[ 1 ] * src[ 4 ];

	// calculate second 8 elements ( cofactors ).
	dst[ 8 ] =  tmp[ 0 ] * src[ 13 ] + tmp[ 3 ] * src[ 14 ] + tmp[ 4 ] * src[ 15 ];
	dst[ 8 ] -= tmp[ 1 ] * src[ 13 ] + tmp[ 2 ] * src[ 14 ] + tmp[ 5 ] * src[ 15 ];
	dst[ 9 ] =  tmp[ 1 ] * src[ 12 ] + tmp[ 6 ] * src[ 14 ] + tmp[ 9 ] * src[ 15 ];
	dst[ 9 ] -= tmp[ 0 ] * src[ 12 ] + tmp[ 7 ] * src[ 14 ] + tmp[ 8 ] * src[ 15 ];
	dst[ 10 ] =  tmp[ 2 ] * src[ 12 ] + tmp[ 7 ] * src[ 13 ] + tmp[ 10 ] * src[ 15 ];
	dst[ 10 ] -= tmp[ 3 ] * src[ 12 ] + tmp[ 6 ] * src[ 13 ] + tmp[ 11 ] * src[ 15 ];
	dst[ 11 ] =  tmp[ 5 ] * src[ 12 ] + tmp[ 8 ] * src[ 13 ] + tmp[ 11 ] * src[ 14 ];
	dst[ 11 ] -= tmp[ 4 ] * src[ 12 ] + tmp[ 9 ] * src[ 13 ] + tmp[ 10 ] * src[ 14 ];
	dst[ 12 ] =  tmp[ 2 ] * src[ 10 ] + tmp[ 5 ] * src[ 11 ] + tmp[ 1 ] * src[ 9 ];
	dst[ 12 ] -= tmp[ 4 ] * src[ 11 ] + tmp[ 0 ] * src[ 9 ] + tmp[ 3 ] * src[ 10 ];
	dst[ 13 ] =  tmp[ 8 ] * src[ 11 ] + tmp[ 0 ] * src[ 8 ] + tmp[ 7 ] * src[ 10 ];
	dst[ 13 ] -= tmp[ 6 ] * src[ 10 ] + tmp[ 9 ] * src[ 11 ] + tmp[ 1 ] * src[ 8 ];
	dst[ 14 ] =  tmp[ 6 ] * src[ 9 ] + tmp[ 11 ] * src[ 11 ] + tmp[ 3 ] * src[ 8 ];
	dst[ 14 ] -= tmp[ 10 ] * src[ 11 ] + tmp[ 2 ] * src[ 8 ] + tmp[ 7 ] * src[ 9 ];
	dst[ 15 ] =  tmp[ 10 ] * src[ 10 ] + tmp[ 4 ] * src[ 8 ] + tmp[ 9 ] * src[ 9 ];
	dst[ 15 ] -= tmp[ 8 ] * src[ 9 ] + tmp[ 11 ] * src[ 10 ] + tmp[ 5 ] * src[ 8 ];

	// calculate the determinant.
	det = src[ 0 ] * dst[ 0 ] + src[ 1 ] * dst[ 1 ] + src[ 2 ] * dst[ 2 ] + src[ 3 ] * dst[ 3 ];
	if ( Abs( det ) < kInverseDetEpsilon )
		return false;

	// calculate the matrix inverse.
	det = 1 / det;

	// store the new matrix.
	output = MMat4x4( dst[ 0 ] * det, dst[ 1 ] * det, dst[ 2 ] * det, dst[ 3 ] * det,
					dst[ 4 ] * det, dst[ 5 ] * det, dst[ 6 ] * det, dst[ 7 ] * det,
					dst[ 8 ] * det, dst[ 9 ] * det, dst[ 10 ] * det, dst[ 11 ] * det,
					dst[ 12 ] * det, dst[ 13 ] * det, dst[ 14 ] * det, dst[ 15 ] * det );

	return true;
}

//----------------------------------------------------------
void
MMat4x4::Transpose() const
{
	Swap( _data[ 1 ], _data[ 4 ] );
	Swap( _data[ 2 ], _data[ 8 ] );
	Swap( _data[ 3 ], _data[ 12 ] );
	Swap( _data[ 6 ], _data[ 9 ] );
	Swap( _data[ 7 ], _data[ 13 ] );
	Swap( _data[ 11 ], _data[ 14 ] );
}

//----------------------------------------------------------
MMat4x4
MMat4x4::Transposed() const
{
	// This sucks.  Fix this.
	MMat4x4 newMat( *this );
	newMat.Transpose();
	return newMat;
}

//----------------------------------------------------------
void
MMat4x4::TransformCoordFast( MVec3& coord ) const
{
	float x = _data[ 0 ] * coord.GetX() + _data[ 1 ] * coord.GetY() + _data[ 2 ] * coord.GetZ() + _data[ 3 ];
	float y = _data[ 4 ] * coord.GetX() + _data[ 5 ] * coord.GetY() + _data[ 6 ] * coord.GetZ() + _data[ 7 ];
	float z = _data[ 8 ] * coord.GetX() + _data[ 9 ] * coord.GetY() + _data[ 10 ] * coord.GetZ() + _data[ 11 ];
	float invW = 1.0f / ( _data[ 12 ] * coord.GetX() + _data[ 13 ] * coord.GetY() + _data[ 14 ] * coord.GetZ() + _data[ 15 ] );
	coord.SetX( x*invW );
	coord.SetY( y*invW );
	coord.SetZ( z*invW );
}

//----------------------------------------------------------
MVec3
MMat4x4::TransformCoord( const MVec3& coord ) const
{
	float x = _data[ 0 ] * coord.GetX() + _data[ 1 ] * coord.GetY() + _data[ 2 ] * coord.GetZ() + _data[ 3 ];
	float y = _data[ 4 ] * coord.GetX() + _data[ 5 ] * coord.GetY() + _data[ 6 ] * coord.GetZ() + _data[ 7 ];
	float z = _data[ 8 ] * coord.GetX() + _data[ 9 ] * coord.GetY() + _data[ 10 ] * coord.GetZ() + _data[ 11 ];
	float invW = 1.0f / ( _data[ 12 ]*coord.GetX() + _data[ 13 ]*coord.GetY() + _data[ 14 ]*coord.GetZ() + _data[ 15 ] );
	return MVec3( x * invW, y * invW, z * invW );
}

//----------------------------------------------------------
void
MMat4x4::TransformCoordPrePersp( MVec4& result, const MVec3& coord ) const
{
	result.SetX( _data[ 0 ] * coord.GetX() + _data[ 1 ] * coord.GetY() + _data[ 2 ] * coord.GetZ() + _data[ 3 ] );
	result.SetY( _data[ 4 ] * coord.GetX() + _data[ 5 ] * coord.GetY() + _data[ 6 ] * coord.GetZ() + _data[ 7 ] );
	result.SetZ( _data[ 8 ] * coord.GetX() + _data[ 9 ] * coord.GetY() + _data[ 10 ] * coord.GetZ() + _data[ 11 ] );
	result.SetW( _data[ 12 ] * coord.GetX() + _data[ 13 ] * coord.GetY() + _data[ 14 ] * coord.GetZ() + _data[ 15 ] );
}

//----------------------------------------------------------
MVec3
MMat4x4::TransformCoordNoPersp( const MVec3& coord ) const
{
	float x = _data[ 0 ] * coord.GetX() + _data[ 1 ] * coord.GetY() + _data[ 2 ] * coord.GetZ() + _data[ 3 ];
	float y = _data[ 4 ] * coord.GetX() + _data[ 5 ] * coord.GetY() + _data[ 6 ] * coord.GetZ() + _data[ 7 ];
	float z = _data[ 8 ] * coord.GetX() + _data[ 9 ] * coord.GetY() + _data[ 10 ] * coord.GetZ() + _data[ 11 ];
	return MVec3( x, y, z );
}

//----------------------------------------------------------
void
MMat4x4::TransformCoordNoPerspFast( MVec3& coord ) const
{
	float x = _data[ 0 ] * coord.GetX() + _data[ 1 ] * coord.GetY() + _data[ 2 ] * coord.GetZ() + _data[ 3 ];
	float y = _data[ 4 ] * coord.GetX() + _data[ 5 ] * coord.GetY() + _data[ 6 ] * coord.GetZ() + _data[ 7 ];
	float z = _data[ 8 ] * coord.GetX() + _data[ 9 ] * coord.GetY() + _data[ 10 ] * coord.GetZ() + _data[ 11 ];
	coord.SetX( x );
	coord.SetY( y );
	coord.SetZ( z );
}

//----------------------------------------------------------
void
MMat4x4::SetRotate( const MMat3x3& rot )
{
	MVec3 scale = GetScale();
	_data[ 0 ]  = scale.GetX() * rot( 0, 0 );
	_data[ 1 ]  = scale.GetX() * rot( 0, 1 );
	_data[ 2 ]  = scale.GetX() * rot( 0, 2 );
	_data[ 4 ]  = scale.GetY() * rot( 1, 0 );
	_data[ 5 ]  = scale.GetY() * rot( 1, 1 );
	_data[ 6 ]  = scale.GetY() * rot( 1, 2 );
	_data[ 8 ]  = scale.GetZ() * rot( 2, 0 );
	_data[ 9 ]  = scale.GetZ() * rot( 2, 1 );
	_data[ 10 ] = scale.GetZ() * rot( 2, 2 );
}

//----------------------------------------------------------
void
MMat4x4::SetTranslate( const MVec3& pos )
{
	_data[ 3 ] = pos.GetX();
	_data[ 7 ] = pos.GetY();
	_data[ 11 ] = pos.GetZ();
}

//----------------------------------------------------------
void
MMat4x4::GetRotate( MMat3x3& mat ) const
{
	float xInvScale = InvSqrtFast( _data[ 0 ]*_data[ 0 ] + _data[ 1 ]*_data[ 1 ] + _data[ 2 ]*_data[ 2 ] );
	float yInvScale = InvSqrtFast( _data[ 4 ]*_data[ 4 ] + _data[ 5 ]*_data[ 5 ] + _data[ 6 ]*_data[ 6 ] );
	float zInvScale = InvSqrtFast( _data[ 8 ]*_data[ 8 ] + _data[ 9 ]*_data[ 9 ] + _data[ 10 ]*_data[ 10 ] );

	mat( 0,0 ) = xInvScale*_data[ 0 ];
	mat( 0,1 ) = xInvScale*_data[ 1 ];
	mat( 0,2 ) = xInvScale*_data[ 2 ];

	mat( 1,0 ) = yInvScale*_data[ 4 ];
	mat( 1,1 ) = yInvScale*_data[ 5 ];
	mat( 1,2 ) = yInvScale*_data[ 6 ];

	mat( 2,0 ) = zInvScale*_data[ 8 ];
	mat( 2,1 ) = zInvScale*_data[ 9 ];
	mat( 2,2 ) = zInvScale*_data[ 10 ];
}

//----------------------------------------------------------
MMat3x3
MMat4x4::GetRotate() const
{
	float xInvScale = InvSqrtFast( _data[ 0 ]*_data[ 0 ] + _data[ 1 ]*_data[ 1 ] + _data[ 2 ]*_data[ 2 ] );
	float yInvScale = InvSqrtFast( _data[ 4 ]*_data[ 4 ] + _data[ 5 ]*_data[ 5 ] + _data[ 6 ]*_data[ 6 ] );
	float zInvScale = InvSqrtFast( _data[ 8 ]*_data[ 8 ] + _data[ 9 ]*_data[ 9 ] + _data[ 10 ]*_data[ 10 ] );
	return MMat3x3( xInvScale*_data[ 0 ], xInvScale*_data[ 1 ], xInvScale*_data[ 2 ],
				    yInvScale*_data[ 4 ], yInvScale*_data[ 5 ], yInvScale*_data[ 6 ],
				    zInvScale*_data[ 8 ], zInvScale*_data[ 9 ], zInvScale*_data[ 10 ] );
}

//----------------------------------------------------------
void
MMat4x4::GetTranslate( MVec3& pos ) const
{
	pos.SetX( _data[ 3 ] );
	pos.SetY( _data[ 7 ] );
	pos.SetZ( _data[ 11 ] );
}

//----------------------------------------------------------
MVec3
MMat4x4::GetTranslate() const
{
	return MVec3( _data[ 3 ], _data[ 7 ], _data[ 11 ] );
}

//----------------------------------------------------------
void
MMat4x4::GetScaleSqr( MVec3& scale ) const
{
	// extract the scale of the matrix.  Ensure that the rotational component of the matrix is of uniform scaling.
	MVec3 x( _data[ 0 ], _data[ 1 ], _data[ 2 ] );
	MVec3 y( _data[ 4 ], _data[ 5 ], _data[ 6 ] );
	MVec3 z( _data[ 8 ], _data[ 9 ], _data[ 10 ] );

	// return the scale^2 of the axes.
	scale.SetX( x.MagSqr() );
	scale.SetY( y.MagSqr() );
	scale.SetZ( z.MagSqr() );
}

//----------------------------------------------------------
MVec3
MMat4x4::GetScaleSqr() const
{
	// extract the scale of the matrix.  Ensure that the rotational component of the matrix is of uniform scaling.
	MVec3 x( _data[ 0 ], _data[ 1 ], _data[ 2 ] );
	MVec3 y( _data[ 4 ], _data[ 5 ], _data[ 6 ] );
	MVec3 z( _data[ 8 ], _data[ 9 ], _data[ 10 ] );

	// return the scale^2 of the axes.
	return MVec3( x.MagSqr(), y.MagSqr(), z.MagSqr() );
}

//----------------------------------------------------------
void
MMat4x4::GetScale( MVec3& scale ) const
{
	// extract the scale of the matrix.  Ensure that the rotational component of the matrix is of uniform scaling.
	MVec3 x( _data[ 0 ], _data[ 1 ], _data[ 2 ] );
	MVec3 y( _data[ 4 ], _data[ 5 ], _data[ 6 ] );
	MVec3 z( _data[ 8 ], _data[ 9 ], _data[ 10 ] );

	// return the scales of the axes.
	scale.SetX( x.Mag() );
	scale.SetY( y.Mag() );
	scale.SetZ( z.Mag() );
}

//----------------------------------------------------------
MVec3
MMat4x4::GetScale() const
{
	// extract the scale of the matrix.  Ensure that the rotational component of the matrix is of uniform scaling.
	MVec3 x( _data[ 0 ], _data[ 1 ], _data[ 2 ] );
	MVec3 y( _data[ 4 ], _data[ 5 ], _data[ 6 ] );
	MVec3 z( _data[ 8 ], _data[ 9 ], _data[ 10 ] );

	// return the scales of the axes.
	return MVec3( x.Mag(), y.Mag(), z.Mag() );
}

//----------------------------------------------------------
void 
MMat4x4::SetAxes( const MVec3& x, const MVec3& y, const MVec3& z )
{
	_data[ 0 ] = x.GetX();
	_data[ 1 ] = x.GetY();
	_data[ 2 ] = x.GetZ();
	_data[ 4 ] = y.GetX();
	_data[ 5 ] = y.GetY();
	_data[ 6 ] = y.GetZ();
	_data[ 8 ] = z.GetX();
	_data[ 9 ] = z.GetY();
	_data[ 10 ] = z.GetZ();
}

//----------------------------------------------------------
void
MMat4x4::SetScale( const MVec3& scale )
{
	// orthonormalize the matrix axes.
	MVec3 x( _data[ 0 ], _data[ 1 ], _data[ 2 ] );
	MVec3 y( _data[ 4 ], _data[ 5 ], _data[ 6 ] );
	MVec3 z( _data[ 8 ], _data[ 9 ], _data[ 10 ] );
	x.Normalize();
	y.Normalize();
	z.Normalize();
	x *= scale.GetX();
	y *= scale.GetY();
	z *= scale.GetZ();
	SetAxes( x, y, z );
}

//----------------------------------------------------------
void
MMat4x4::SetOrientation( const MVec3& side, const MVec3& forward )
{
	// compute the new matrix axes.
    MVec3 x( side.Normalized() );
    MVec3 z( forward.Normalized() );
    MVec3 y( z.Cross( x ).Normalized() );
    x = y.Cross( z ).Normalized();

	// scale them.
	MVec3 scale = GetScale();
	x *= scale.GetX();
	y *= scale.GetY();
	z *= scale.GetZ();

	// set them.
	SetAxes( x, y, z );
}

//----------------------------------------------------------
void
MMat4x4::SetRow( unsigned int row, const MVec4& rowVec )
{
	B_ASSERT( row < 4 );
	float* dst = _data + row * 4;
	dst[ 0 ] = rowVec.GetX();
	dst[ 1 ] = rowVec.GetY();
	dst[ 2 ] = rowVec.GetZ();
	dst[ 3 ] = rowVec.GetW();
}

//----------------------------------------------------------
void
MMat4x4::SetCol( unsigned int col, const MVec4& rowVec )
{
	B_ASSERT( col < 4 );
	float* dst = _data + col;
	dst[ 0 ] = rowVec.GetX();
	dst[ 4 ] = rowVec.GetY();
	dst[ 8 ] = rowVec.GetZ();
	dst[ 12 ] = rowVec.GetW();
}

//----------------------------------------------------------
void
MMat4x4::GetRow( MVec4& rowVec, unsigned int row ) const
{
	B_ASSERT( row < 4 );
	float* src = _data + row * 4;
	rowVec.SetX( src[ 0 ] );
	rowVec.SetY( src[ 1 ] );
	rowVec.SetZ( src[ 2 ] );
	rowVec.SetW( src[ 3 ] );
}

//----------------------------------------------------------
MVec4
MMat4x4::GetRow( unsigned int row ) const
{
	B_ASSERT( row < 4 );
	float* src = _data + row * 4;
	return MVec4( src[ 0 ], src[ 1 ], src[ 2 ], src[ 3 ] );
}

//----------------------------------------------------------
void
MMat4x4::GetCol( MVec4& rowVec, unsigned int col ) const
{
	B_ASSERT( col < 4 );
	float* src = _data + col;
	rowVec.SetX( src[ 0 ] );
	rowVec.SetY( src[ 4 ] );
	rowVec.SetZ( src[ 8 ] );
	rowVec.SetW( src[ 12 ] );
}

//----------------------------------------------------------
MVec4
MMat4x4::GetCol( unsigned int col ) const
{
	B_ASSERT( col < 4 );
	float* src = _data + col;
	return MVec4( src[ 0 ], src[ 4 ], src[ 8 ], src[ 12 ] );
}

//----------------------------------------------------------
void
MMat4x4::GetAxis( MVec3& axis, unsigned int col ) const
{
	B_ASSERT( col < 3 );
	const float* src = _data + col;
	axis.SetX( src[ 0 ] );
	axis.SetY( src[ 4 ] );
	axis.SetZ( src[ 8 ] );
}

//----------------------------------------------------------
float&
MMat4x4::operator ()( unsigned int i, unsigned int j )
{
	B_ASSERT( i < 4 );
	B_ASSERT( j < 4 );
	return _data[ i*4+j ];
}

//----------------------------------------------------------
float
MMat4x4::operator ()( unsigned int i, unsigned int j ) const
{
	B_ASSERT( i < 4 );
	B_ASSERT( j < 4 );
	return _data[ i*4+j ];
}

//----------------------------------------------------------
// performs a matrix multiply.
MMat4x4
MMat4x4::operator * ( const MMat4x4& mat ) const
{
#ifdef _MSC_VER
	MMat4x4 d;

	// SSE, Pentium III and up.
	float *data1Ptr = _data;
	float *data2Ptr = mat._data;
	float *output = d._data;
	__asm
	{
		// load data.
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		mov edx, output;

		// load second matrix rows.
		movaps xmm1, [ ebx ];
		movaps xmm3, [ ebx+16 ];
		movaps xmm5, [ ebx+32 ];
		movaps xmm7, [ ebx+48 ];

		// first row...
		// load first row and replicate components to each register.
		movaps xmm0, [ eax ];			// load the first src row into xmm0.
		movaps xmm6, xmm0;
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm6, xmm6, 0xFF;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		mulps xmm6, xmm7;			// this( 0,3 ) * mat.row3
		addps xmm0, xmm2;
		addps xmm0, xmm4;
		addps xmm0, xmm6;	// xmm4 now holds the result of the first 4 components.

		// write the result to the output.
		movaps [ edx ], xmm0;

		// second row...
		// load second row and replicate components to each register.
		movaps xmm0, [ eax+16 ];			// load the first src row into xmm0.
		movaps xmm6, xmm0;
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm6, xmm6, 0xFF;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		mulps xmm6, xmm7;			// this( 0,3 ) * mat.row3
		addps xmm0, xmm2;
		addps xmm0, xmm4;
		addps xmm0, xmm6;	// xmm4 now holds the result of the first 4 components.

		// write the result to the output.
		movaps [ edx+16 ], xmm0;

		// third row...
		// load third row and replicate components to each register.
		movaps xmm0, [ eax+32 ];			// load the first src row into xmm0.
		movaps xmm6, xmm0;
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm6, xmm6, 0xFF;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		mulps xmm6, xmm7;			// this( 0,3 ) * mat.row3
		addps xmm0, xmm2;
		addps xmm0, xmm4;
		addps xmm0, xmm6;	// xmm4 now holds the result of the first 4 components.

		// write the result to the output.
		movaps [ edx+32 ], xmm0;

		// fourth row...
		// load fourth row and replicate components to each register.
		movaps xmm0, [ eax+48 ];			// load the first src row into xmm0.
		movaps xmm6, xmm0;
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm6, xmm6, 0xFF;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		mulps xmm6, xmm7;			// this( 0,3 ) * mat.row3
		addps xmm0, xmm2;
		addps xmm0, xmm4;
		addps xmm0, xmm6;	// xmm4 now holds the result of the first 4 components.

		// write the result to the output.
		movaps [ edx+48 ], xmm0;
	};
	return d;
#else
	MMat4x4 d;
	
	float *a = _data;
	float *b = mat._data;
	float *dst = d._data;

    dst[ 0] = a[ 0]*b[ 0] + a[ 1]*b[ 4] + a[ 2]*b[ 8] + a[ 3]*b[12];
    dst[ 1] = a[ 0]*b[ 1] + a[ 1]*b[ 5] + a[ 2]*b[ 9] + a[ 3]*b[13];
    dst[ 2] = a[ 0]*b[ 2] + a[ 1]*b[ 6] + a[ 2]*b[10] + a[ 3]*b[14];
    dst[ 3] = a[ 0]*b[ 3] + a[ 1]*b[ 7] + a[ 2]*b[11] + a[ 3]*b[15];

    dst[ 4] = a[ 4]*b[ 0] + a[ 5]*b[ 4] + a[ 6]*b[ 8] + a[ 7]*b[12];
    dst[ 5] = a[ 4]*b[ 1] + a[ 5]*b[ 5] + a[ 6]*b[ 9] + a[ 7]*b[13];
    dst[ 6] = a[ 4]*b[ 2] + a[ 5]*b[ 6] + a[ 6]*b[10] + a[ 7]*b[14];
    dst[ 7] = a[ 4]*b[ 3] + a[ 5]*b[ 7] + a[ 6]*b[11] + a[ 7]*b[15];

    dst[ 8] = a[ 8]*b[ 0] + a[ 9]*b[ 4] + a[10]*b[ 8] + a[11]*b[12];
    dst[ 9] = a[ 8]*b[ 1] + a[ 9]*b[ 5] + a[10]*b[ 9] + a[11]*b[13];
    dst[10] = a[ 8]*b[ 2] + a[ 9]*b[ 6] + a[10]*b[10] + a[11]*b[14];
    dst[11] = a[ 8]*b[ 3] + a[ 9]*b[ 7] + a[10]*b[11] + a[11]*b[15];

    dst[12] = a[12]*b[ 0] + a[13]*b[ 4] + a[14]*b[ 8] + a[15]*b[12];
    dst[13] = a[12]*b[ 1] + a[13]*b[ 5] + a[14]*b[ 9] + a[15]*b[13];
    dst[14] = a[12]*b[ 2] + a[13]*b[ 6] + a[14]*b[10] + a[15]*b[14];
    dst[15] = a[12]*b[ 3] + a[13]*b[ 7] + a[14]*b[11] + a[15]*b[15];

	return d;
#endif
}

//----------------------------------------------------------
bool
MMat4x4::IsIdentity() const
{
	bool ident =	 ApproxEqual( _data[  0 ], 1.0f );
	ident = ident && ApproxEqual( _data[  1 ], 0.0f );
	ident = ident && ApproxEqual( _data[  2 ], 0.0f );
	ident = ident && ApproxEqual( _data[  3 ], 0.0f );

	ident = ident && ApproxEqual( _data[  4 ], 0.0f );
	ident = ident && ApproxEqual( _data[  5 ], 1.0f );
	ident = ident && ApproxEqual( _data[  6 ], 0.0f );
	ident = ident && ApproxEqual( _data[  7 ], 0.0f );

	ident = ident && ApproxEqual( _data[  8 ], 0.0f );
	ident = ident && ApproxEqual( _data[  9 ], 0.0f );
	ident = ident && ApproxEqual( _data[ 10 ], 1.0f );
	ident = ident && ApproxEqual( _data[ 11 ], 0.0f );

	ident = ident && ApproxEqual( _data[ 12 ], 0.0f );
	ident = ident && ApproxEqual( _data[ 13 ], 0.0f );
	ident = ident && ApproxEqual( _data[ 14 ], 0.0f );
	return  ident && ApproxEqual( _data[ 15 ], 1.0f );
}

//----------------------------------------------------------
void
MMat4x4::Save( UWriter& writer ) const
{
	writer.WriteFloat( _data[ 0 ] );
	writer.WriteFloat( _data[ 1 ] );
	writer.WriteFloat( _data[ 2 ] );
	writer.WriteFloat( _data[ 3 ] );
	writer.WriteFloat( _data[ 4 ] );
	writer.WriteFloat( _data[ 5 ] );
	writer.WriteFloat( _data[ 6 ] );
	writer.WriteFloat( _data[ 7 ] );
	writer.WriteFloat( _data[ 8 ] );
	writer.WriteFloat( _data[ 9 ] );
	writer.WriteFloat( _data[ 10 ] );
	writer.WriteFloat( _data[ 11 ] );
	writer.WriteFloat( _data[ 12 ] );
	writer.WriteFloat( _data[ 13 ] );
	writer.WriteFloat( _data[ 14 ] );
	writer.WriteFloat( _data[ 15 ] );
}

//----------------------------------------------------------
void
MMat4x4::Load( UReader& reader )
{
	_data[ 0 ] = reader.ReadFloat();
	_data[ 1 ] = reader.ReadFloat();
	_data[ 2 ] = reader.ReadFloat();
	_data[ 3 ] = reader.ReadFloat();
	_data[ 4 ] = reader.ReadFloat();
	_data[ 5 ] = reader.ReadFloat();
	_data[ 6 ] = reader.ReadFloat();
	_data[ 7 ] = reader.ReadFloat();
	_data[ 8 ] = reader.ReadFloat();
	_data[ 9 ] = reader.ReadFloat();
	_data[ 10 ] = reader.ReadFloat();
	_data[ 11 ] = reader.ReadFloat();
	_data[ 12 ] = reader.ReadFloat();
	_data[ 13 ] = reader.ReadFloat();
	_data[ 14 ] = reader.ReadFloat();
	_data[ 15 ] = reader.ReadFloat();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
MMat4x4::GetAlignedPtr()
{
	// determine the 16-byte aligned pointer.
//	unsigned long offset = ( unsigned long )_array;
//	offset = ( offset + 0xF ) & 0xFFFFFFF0;
//	_data = ( float * )offset;
	_data = Align16( _array );
}

//----------------------------------------------------------
void
MMat4x4::Copy( const MMat4x4& mat )
{
	for ( int i = 0; i < 16; ++i )
	{
		_data[ i ] = mat._data[ i ];
	}
}
