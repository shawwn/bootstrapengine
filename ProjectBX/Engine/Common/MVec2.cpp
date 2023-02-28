//----------------------------------------------------------
// File:		MVec2.cpp
// Author:		Kevin Bray
// Created:		10-05-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

#include "common_afx.h"

// class header.
#include "MVec2.h"

//**********************************************************
// class MVec2
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MVec2::MVec2()
{
	_data = Align16( _array );
	_data[ 0 ] = 0.0f;
	_data[ 1 ] = 0.0f;
}

//----------------------------------------------------------
MVec2::MVec2( float x, float y )
{
	_data = Align16( _array );
	_data[ 0 ] = x;
	_data[ 1 ] = y;
}

//----------------------------------------------------------
MVec2::MVec2( const MVec2& v )
{
	_data = Align16( _array );
	_data[ 0 ] = v._data[ 0 ];
	_data[ 1 ] = v._data[ 1 ];
}

//----------------------------------------------------------
MVec2::~MVec2()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MVec2&
MVec2::operator = ( const MVec2& v )
{
	_data[ 0 ] = v._data[ 0 ];
	_data[ 1 ] = v._data[ 1 ];
	return *this;
}

//----------------------------------------------------------
float&
MVec2::operator ()( int i )
{
	B_ASSERT( i < 2 );
	return _data[ i ];
}

//----------------------------------------------------------
float
MVec2::operator ()( int i ) const
{
	B_ASSERT( i < 2 );
	return _data[ i ];
}

//----------------------------------------------------------
MVec2
MVec2::operator * ( const MVec2& v ) const
{
#ifdef _MSC_VER
	MVec2 d;
	float *data1Ptr = _data;
	float *data2Ptr = v._data;
	float *output = d._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		mov edx, output;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		mulps xmm0, xmm1;
		movaps [ edx ], xmm0;
	};
	return d;
#else
	return MVec2( _data[ 0 ] * v._data[ 0 ], _data[ 1 ] * v._data[ 1 ] );
#endif
}

//----------------------------------------------------------
MVec2
MVec2::operator / ( const MVec2& v ) const
{
#ifdef _MSC_VER
	MVec2 d;
	float *data1Ptr = _data;
	float *data2Ptr = v._data;
	float *output = d._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		mov edx, output;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		divps xmm0, xmm1;
		movaps [ edx ], xmm0;
	};
	return d;
#else
	return MVec2( _data[ 0 ] / v._data[ 0 ], _data[ 1 ] / v._data[ 1 ] );
#endif
}

//----------------------------------------------------------
MVec2
MVec2::operator + ( const MVec2& v ) const
{
#ifdef _MSC_VER
	MVec2 d;
	float *data1Ptr = _data;
	float *data2Ptr = v._data;
	float *output = d._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		mov edx, output;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		addps xmm0, xmm1;
		movaps [ edx ], xmm0;
	};
	return d;
#else
	return MVec2( _data[ 0 ] + v._data[ 0 ], _data[ 1 ] + v._data[ 1 ] );
#endif
}

//----------------------------------------------------------
MVec2
MVec2::operator - ( const MVec2& v ) const
{
#ifdef _MSC_VER
	MVec2 d;
	float *data1Ptr = _data;
	float *data2Ptr = v._data;
	float *output = d._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		mov edx, output;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		subps xmm0, xmm1;
		movaps [ edx ], xmm0;
	};
	return d;
#else
	return MVec2( _data[ 0 ] - v._data[ 0 ], _data[ 1 ] - v._data[ 1 ] );
#endif
}

//----------------------------------------------------------
MVec2&
MVec2::operator *= ( const MVec2& v )
{
#ifdef _MSC_VER
	float *data1Ptr = _data;
	float *data2Ptr = v._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		mulps xmm0, xmm1;
		movaps [ eax ], xmm0;
	};
#else
	_data[ 0 ] *= v._data[ 0 ];
	_data[ 1 ] *= v._data[ 1 ];
#endif
	return *this;
}

//----------------------------------------------------------
MVec2&
MVec2::operator *= ( float scale )
{
#ifdef _MSC_VER
	float *data1Ptr = _data;

	// SSE
	__asm
	{
		mov ebx, data1Ptr;
		movd xmm0, scale;
		shufps xmm0, xmm0, 0x00;
		movaps xmm1, [ ebx ];
		mulps xmm0, xmm1;
		movaps [ ebx ], xmm0;
	};
#else
	_data[ 0 ] *= scale;
	_data[ 1 ] *= scale;
#endif
	return *this;
}

//----------------------------------------------------------
MVec2&
MVec2::operator /= ( const MVec2& v )
{
#ifdef _MSC_VER
	float *data1Ptr = _data;
	float *data2Ptr = v._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		divps xmm0, xmm1;
		movaps [ eax ], xmm0;
	};
#else
	_data[ 0 ] /= v._data[ 0 ];
	_data[ 1 ] /= v._data[ 1 ];
#endif
	return *this;
}

//----------------------------------------------------------
MVec2&
MVec2::operator += ( const MVec2& v )
{
#ifdef _MSC_VER
	float *data1Ptr = _data;
	float *data2Ptr = v._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		addps xmm0, xmm1;
		movaps [ eax ], xmm0;
	};
#else
	_data[ 0 ] += v._data[ 0 ];
	_data[ 1 ] += v._data[ 1 ];
#endif
	return *this;
}

//----------------------------------------------------------
MVec2&
MVec2::operator -= ( const MVec2& v )
{
#ifdef _MSC_VER
	float *data1Ptr = _data;
	float *data2Ptr = v._data;

	// SSE
	__asm
	{
		mov eax, data1Ptr;
		mov ebx, data2Ptr;
		movaps xmm0, [ eax ];
		movaps xmm1, [ ebx ];
		subps xmm0, xmm1;
		movaps [ eax ], xmm0;
	};
#else
	_data[ 0 ] -= v._data[ 0 ];
	_data[ 1 ] -= v._data[ 1 ];
#endif
	return *this;
}

//----------------------------------------------------------
MVec2
operator * ( float scale, const MVec2& v )
{
#ifdef _MSC_VER
	MVec2 d;
	float *data2Ptr = v._data;
	float *output = d._data;

	// SSE
	__asm
	{
		mov eax, data2Ptr;
		mov edx, output;
		movd xmm0, scale;
		shufps xmm0, xmm0, 0x00;
		movaps xmm1, [ eax ];
		mulps xmm0, xmm1;
		movaps [ edx ], xmm0;
	}
	return d;
#else
	return MVec2( v._data[ 0 ] * scale, v._data[ 1 ] * scale );
#endif
}
