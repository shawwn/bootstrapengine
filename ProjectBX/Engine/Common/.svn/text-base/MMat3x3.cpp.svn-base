//----------------------------------------------------------
// File:		MMat3x3.cpp
// Author:		Kevin Bray
// Created:		09-25-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MMat3x3.h"

// project includes.
#include "MVec3.h"
#include "UReader.h"
#include "UWriter.h"

// std c++ includes.
#include <math.h>

//**********************************************************
// class MMat3x3
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MMat3x3::MMat3x3()
{
	// set to identity.
	_data[ 0 ] = 1.0f;
	_data[ 1 ] = 0.0f;
	_data[ 2 ] = 0.0f;
	_data[ 3 ] = 0.0f;
	_data[ 4 ] = 1.0f;
	_data[ 5 ] = 0.0f;
	_data[ 6 ] = 0.0f;
	_data[ 7 ] = 0.0f;
	_data[ 8 ] = 1.0f;
}

//----------------------------------------------------------
MMat3x3::MMat3x3( float y, float x, float z )
{
	FromEulerYXZ( y, x, z );
}

//----------------------------------------------------------
MMat3x3::MMat3x3( float _11, float _12, float _13,
				  float _21, float _22, float _23,
				  float _31, float _32, float _33 )
{
	// initialize the array to 0.
	MemSet( _data, 0, sizeof( _data ) );

	// initialize the data.
	_data[ 0 ] = _11;
	_data[ 1 ] = _12;
	_data[ 2 ] = _13;

	_data[ 3 ] = _21;
	_data[ 4 ] = _22;
	_data[ 5 ] = _23;

	_data[ 6 ] = _31;
	_data[ 7 ] = _32;
	_data[ 8 ] = _33;
}

//----------------------------------------------------------
MMat3x3::MMat3x3( const MVec3& axis, float angle )
{
	MakeRotation( axis, angle );
}

//----------------------------------------------------------
MMat3x3::MMat3x3( const MVec3& row0, const MVec3& row1, const MVec3& row2 )
{
	SetRow( 0, row0 );
	SetRow( 1, row1 );
	SetRow( 2, row2 );
}

//----------------------------------------------------------
MMat3x3::MMat3x3( const MMat3x3& mat )
{
	// get the aligned pointer and copy the data.
	Copy( mat );
}

//----------------------------------------------------------
MMat3x3::~MMat3x3()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MMat3x3&
MMat3x3::operator = ( const MMat3x3& mat )
{
	Copy( mat );
	return *this;
}

//----------------------------------------------------------
void
MMat3x3::ToEulerYXZ( float& y, float& x, float& z )
{
	// singularity at north pole
	if ( _data[ 3 ] > 0.998 ) {
		y = ATan2( _data[ 2 ], _data[ 8 ] );
		z = HALF_PI;
		x = 0.0f;
		return;
	}

	// singularity at south pole
	if ( _data[ 3 ] < -0.998 ) {
		y = ATan2( _data[ 2 ], _data[ 8 ] );
		z = -HALF_PI;
		x = 0.0f;
		return;
	}

	// matrix to euler, normal case.
	y = ATan2( -_data[ 6 ], _data[ 0 ] );
	x = ATan2( -_data[ 5 ], _data[ 4 ] );
	z = ASin( _data[ 3 ] );	
}

//----------------------------------------------------------
void
MMat3x3::FromEulerYXZ( float y, float x, float z )
{
    // Assuming the angles are in radians.
    float ch = Cos( y );
    float sh = Sin( y );
    float cb = Cos( x );
    float sb = Sin( x );
    float ca = Cos( z );
    float sa = Sin( z );

    _data[ 0 ] = ch * ca;
    _data[ 1 ] = sh*sb - ch*sa*cb;
    _data[ 2 ] = ch*sa*sb + sh*cb;
    _data[ 3 ] = sa;
    _data[ 4 ] = ca*cb;
    _data[ 5 ] = -ca*sb;
    _data[ 6 ] = -sh*ca;
    _data[ 7 ] = sh*sa*cb + ch*sb;
    _data[ 8 ] = -sh*sa*sb + ch*cb;
}

//----------------------------------------------------------
void
MMat3x3::MakeXRotation( float angleInRad )
{
	_data[ 0 ] = 1.0f;
	_data[ 1 ] = 0.0f;
	_data[ 2 ] = 0.0f;
	_data[ 3 ] = 0.0f;
	_data[ 4 ] = Cos( angleInRad );
	_data[ 5 ] = -Sin( angleInRad );
	_data[ 6 ] = 0.0f;
	_data[ 7 ] = Sin( angleInRad );
	_data[ 8 ] = Cos( angleInRad );
}

//----------------------------------------------------------
void
MMat3x3::MakeYRotation( float angleInRad )
{
	_data[ 0 ] = Cos( angleInRad );
	_data[ 1 ] = 0.0f;
	_data[ 2 ] = Sin( angleInRad );
	_data[ 3 ] = 0.0f;
	_data[ 4 ] = 1.0f;
	_data[ 5 ] = 0.0f;
	_data[ 6 ] = -Sin( angleInRad );
	_data[ 7 ] = 0.0f;
	_data[ 8 ] = Cos( angleInRad );
}

//----------------------------------------------------------
void
MMat3x3::MakeZRotation( float angleInRad )
{
	_data[ 0 ] = Cos( angleInRad );
	_data[ 1 ] = -Sin( angleInRad );
	_data[ 2 ] = 0.0f;
	_data[ 3 ] = Sin( angleInRad );
	_data[ 4 ] = Cos( angleInRad );
	_data[ 5 ] = 0.0f;
	_data[ 6 ] = 0.0f;
	_data[ 7 ] = 0.0f;
	_data[ 8 ] = 1.0f;
}

//----------------------------------------------------------
void
MMat3x3::MakeRotation( const MVec3& axis, float angle )
{
	// make an axis angle rotation.
#if 1
	_data[ 0 ] = 1 + ( 1 - Cos( angle ) ) * ( axis.GetX() * axis.GetX() - 1.0f );
	_data[ 1 ] = -axis.GetZ() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetX() * axis.GetY();
	_data[ 2 ] = axis.GetY() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetX() * axis.GetZ();

	_data[ 3 ] = axis.GetZ() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetX() * axis.GetY();
	_data[ 4 ] = 1 + ( 1.0f - Cos( angle ) ) * ( axis.GetY() * axis.GetY() - 1.0f );
	_data[ 5 ] = -axis.GetX() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetY() * axis.GetZ();

	_data[ 6 ] = -axis.GetY() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetX() * axis.GetZ();
	_data[ 7 ] = axis.GetX() * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.GetY() * axis.GetZ();
	_data[ 8 ] = 1.0f + ( 1.0f - Cos( angle ) ) * ( axis.GetZ() * axis.GetZ() - 1.0f );
#else
	float c = Cos( angle );
	float s = Sin( angle );
	float t = 1 - c;

	_data[ 0 ] = c + axis.GetX() * axis.GetX() * t;
	_data[ 4 ] = c + axis.GetY() * axis.GetY() * t;
	_data[ 8 ] = c + axis.GetZ() * axis.GetZ() * t;

	float tmp1 = axis.GetX() * axis.GetY() * t;
	float tmp2 = axis.GetZ() * s;
	_data[ 3 ] = tmp1 + tmp2;
	_data[ 1 ] = tmp1 - tmp2;

	tmp1 = axis.GetX() * axis.GetZ() * t;
	tmp2 = axis.GetY() * s;
	_data[ 6 ] = tmp1 - tmp2;
	_data[ 2 ] = tmp1 + tmp2;

	tmp1 = axis.GetY() * axis.GetZ() * t;
	tmp2 = axis.GetX() * s;
	_data[ 7 ] = tmp1 + tmp2;
	_data[ 5 ] = tmp1 - tmp2;

#endif
}

//----------------------------------------------------------
MVec3
MMat3x3::RotatePoint( const MVec3& point ) const
{
	return MVec3( _data[ 0 ] * point.GetX() + _data[ 1 ] * point.GetY() + _data[ 2 ] * point.GetZ(),
				  _data[ 3 ] * point.GetX() + _data[ 4 ] * point.GetY() + _data[ 5 ] * point.GetZ(),
				  _data[ 6 ] * point.GetX() + _data[ 7 ] * point.GetY() + _data[ 8 ] * point.GetZ() );
}

//----------------------------------------------------------
void
MMat3x3::RotatePointFast( MVec3& point ) const
{
	float x = _data[ 0 ] * point.GetX() + _data[ 1 ] * point.GetY() + _data[ 2 ] * point.GetZ();
	float y = _data[ 3 ] * point.GetX() + _data[ 4 ] * point.GetY() + _data[ 5 ] * point.GetZ();
	float z = _data[ 6 ] * point.GetX() + _data[ 7 ] * point.GetY() + _data[ 8 ] * point.GetZ();
	point.SetX( x );
	point.SetY( y );
	point.SetZ( z );
}

//----------------------------------------------------------
void
MMat3x3::SetRow( unsigned int i, const MVec3& row )
{
	int start = i * 3;
	_data[ start ] = row.GetX();
	_data[ start+1 ] = row.GetY();
	_data[ start+2 ] = row.GetZ();
}

//----------------------------------------------------------
void
MMat3x3::GetRow( MVec3& rowOut, unsigned int i ) const
{
	int start = i * 3;
	rowOut.SetX( _data[ start ] );
	rowOut.SetY( _data[ start+1 ] );
	rowOut.SetZ( _data[ start+2 ] );
}

//----------------------------------------------------------
MVec3
MMat3x3::GetRow( unsigned int i ) const
{
	int start = i * 3;
	return MVec3( _data[ start ], _data[ start+1 ], _data[ start+2 ] );
}

//----------------------------------------------------------
void
MMat3x3::SetCol( unsigned int i, const MVec3& col )
{
	_data[ i ]   = col.GetX();
	_data[ i+3 ] = col.GetY();
	_data[ i+6 ] = col.GetZ();
}

//----------------------------------------------------------
void
MMat3x3::GetCol( MVec3& colOut, unsigned int i ) const
{
	colOut.SetX( _data[ i ] );
	colOut.SetY( _data[ i+3 ] );
	colOut.SetZ( _data[ i+6 ] );
}

//----------------------------------------------------------
MVec3
MMat3x3::GetCol( unsigned int i ) const
{
	return MVec3( _data[ i ], _data[ i+3 ], _data[ i+6 ] );
}

//----------------------------------------------------------
void
MMat3x3::Transpose()
{
	Swap( _data[1], _data[3] );
	Swap( _data[2], _data[6] );
	Swap( _data[5], _data[7] );
}

//----------------------------------------------------------
MMat3x3
MMat3x3::Transposed() const
{
	return MMat3x3( _data[ 0 ], _data[ 3 ], _data[ 6 ],
				   _data[ 1 ], _data[ 4 ], _data[ 7 ],
				   _data[ 2 ], _data[ 5 ], _data[ 8 ] );
}

//----------------------------------------------------------
bool
MMat3x3::Inverse()
{
	float det = _data[0]*( _data[8]*_data[4] - _data[7]*_data[5] ) -
				_data[3]*( _data[8]*_data[1] - _data[7]*_data[2] ) +
				_data[6]*( _data[5]*_data[1] - _data[4]*_data[2] );
	if ( Abs( det ) < 0.0001 )
		return false;

	float i11 = _data[8]*_data[4] - _data[7]*_data[5];
	float i12 = _data[8]*_data[1] - _data[7]*_data[2];
	float i13 = _data[5]*_data[1] - _data[4]*_data[2];
	float i21 = _data[8]*_data[3] - _data[6]*_data[5];
	float i22 = _data[8]*_data[0] - _data[6]*_data[2];
	float i23 = _data[5]*_data[0] - _data[3]*_data[2];
	float i31 = _data[7]*_data[3] - _data[6]*_data[4];
	float i32 = _data[7]*_data[0] - _data[6]*_data[1];
	float i33 = _data[4]*_data[0] - _data[3]*_data[1];
	float invDet = 1.0f / det;
	_data[0] = invDet *  i11;
	_data[1] = invDet * -i12;
	_data[2] = invDet *  i13;
	_data[3] = invDet * -i21;
	_data[4] = invDet *  i22;
	_data[5] = invDet * -i23;
	_data[6] = invDet *  i31;
	_data[7] = invDet * -i32;
	_data[8] = invDet *  i33;
	return true;
}

//----------------------------------------------------------
bool
MMat3x3::InverseTransposed( MMat3x3& output ) const
{
	// TODO: fill this out!
	output = *this;
	if ( !output.Inverse() )
		return false;
	output.Transpose();
	return true;
}

//----------------------------------------------------------
float&
MMat3x3::operator ()( unsigned int i, unsigned int j )
{
	B_ASSERT( i < 3 );
	B_ASSERT( j < 3 );
	return _data[ i*3+j ];
}

//----------------------------------------------------------
float
MMat3x3::operator ()( unsigned int i, unsigned int j ) const
{
	B_ASSERT( i < 3 );
	B_ASSERT( j < 3 );
	return _data[ i*3+j ];
}

//----------------------------------------------------------
// performs a matrix multiply.
MMat3x3
MMat3x3::operator * ( const MMat3x3& mat ) const
{
	MMat3x3 d( 0, 0, 0,
			   0, 0, 0,
			   0, 0, 0 );

	for ( int i = 0; i < 3; ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			for ( int k = 0; k < 3; ++k )
			{
				d( i, j ) += _data[ i * 3 + k ] * mat( k, j );
//				d( i, j ) += mat( i, k ) * _data[ k * 3 + j ];
			}
		}
	}
	return d;
//	This data structure *MUST NOT* be 4-byte aligned due to GL calls!  That rules out MMX or SSE =( 
/*	
	// SSE, Pentium III and up.
	float *data1Ptr = _data;
	float *data2Ptr = mat._data;
	float *output = d._data;
	__asm
	{
		// load data.
		mov eax, data2Ptr;
		mov ebx, data1Ptr;
		mov edx, output;

		// load second matrix rows.
		movaps xmm1, [ ebx ];
		movaps xmm3, [ ebx+16 ];
		movaps xmm5, [ ebx+32 ];

		// first row...
		// load first row and replicate components to each register.
		movaps xmm0, [ eax ];			// load the first src row into xmm0.
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		addps xmm0, xmm2;
		addps xmm0, xmm4;

		// write the result to the output.
		movaps [ edx ], xmm0;

		// second row...
		// load second row and replicate components to each register.
		movaps xmm0, [ eax+16 ];			// load the first src row into xmm0.
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		addps xmm0, xmm2;
		addps xmm0, xmm4;

		// write the result to the output.
		movaps [ edx+16 ], xmm0;

		// third row...
		// load third row and replicate components to each register.
		movaps xmm0, [ eax+32 ];			// load the first src row into xmm0.
		movaps xmm4, xmm0;
		movaps xmm2, xmm0;
		shufps xmm4, xmm4, 0xAA;
		shufps xmm2, xmm2, 0x55;
		shufps xmm0, xmm0, 0x00;	// 0<<2 | 0<<0

		// multiply data and add.
		mulps xmm0, xmm1;			// this( 0,0 ) * mat.row0
		mulps xmm2, xmm3;			// this( 0,1 ) * mat.row1
		mulps xmm4, xmm5;			// this( 0,2 ) * mat.row2
		addps xmm0, xmm2;
		addps xmm0, xmm4;

		// write the result to the output.
		movaps [ edx+32 ], xmm0;
	};

	return d;
*/
}

//----------------------------------------------------------
void
MMat3x3::Save( UWriter& writer ) const
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
}

//----------------------------------------------------------
void
MMat3x3::Load( UReader& reader )
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
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
MMat3x3::Copy( const MMat3x3& mat )
{
	for ( int i = 0; i < 9; ++i )
	{
		_data[ i ] = mat._data[ i ];
	}
}
