//----------------------------------------------------------
// File:        MVec3.cpp
// Author:      Kevin Bray
// Created:     10-05-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------    
#include "common_afx.h"

// class header
#include "MVec3.h"

// project includes.
#include "UReader.h"
#include "UWriter.h"

const float MVec3::kNormalizeEpsilon = 0.00000095367431640625f;
//const float MVec3::kNormalizeEpsilon = 0.000000059604644775390625f;
const float MVec3::kNormalizeFastEpsilon = 0.0009765625f;


//**********************************************************
// class MVec3
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MVec3::MVec3()
{
    _data = Align16( _array );
    _data[ 0 ] = 0.0f;
    _data[ 1 ] = 0.0f;
    _data[ 2 ] = 0.0f;
    _data[ 3 ] = 0.0f;
}

//----------------------------------------------------------
MVec3::MVec3( float x, float y, float z )
{
    _data = Align16( _array );
    _data[ 0 ] = x;
    _data[ 1 ] = y;
    _data[ 2 ] = z;
    _data[ 3 ] = 0.0f;
}

//----------------------------------------------------------
MVec3::MVec3( const float* vec )
{
    _data = Align16( _array );
    _data[ 0 ] = vec[ 0 ];
    _data[ 1 ] = vec[ 1 ];
    _data[ 2 ] = vec[ 2 ];
    _data[ 3 ] = 0.0f;
}

//----------------------------------------------------------
MVec3::MVec3( const MVec3& v )
{
    _data = Align16( _array );
    _data[ 0 ] = v._data[ 0 ];
    _data[ 1 ] = v._data[ 1 ];
    _data[ 2 ] = v._data[ 2 ];
    _data[ 3 ] = 0.0f;
}

//----------------------------------------------------------
MVec3::~MVec3()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MVec3&
MVec3::operator = ( const MVec3& v )
{
    _data[ 0 ] = v._data[ 0 ];
    _data[ 1 ] = v._data[ 1 ];
    _data[ 2 ] = v._data[ 2 ];
    return *this;
}

//----------------------------------------------------------
void
MVec3::AssignMin( const MVec3& v )
{
    _data[ 0 ] = ::Min( _data[ 0 ], v._data[ 0 ] );
    _data[ 1 ] = ::Min( _data[ 1 ], v._data[ 1 ] );
    _data[ 2 ] = ::Min( _data[ 2 ], v._data[ 2 ] );
}

//----------------------------------------------------------
void
MVec3::AssignMax( const MVec3& v )
{
    _data[ 0 ] = ::Max( _data[ 0 ], v._data[ 0 ] );
    _data[ 1 ] = ::Max( _data[ 1 ], v._data[ 1 ] );
    _data[ 2 ] = ::Max( _data[ 2 ], v._data[ 2 ] ); 
}

//----------------------------------------------------------
void
MVec3::Orthogonalize( const MVec3& base )
{
    MVec3 right = Cross( base );
    *this = base.Cross( right );
}

//----------------------------------------------------------
MVec3
MVec3::Orthogonalized( const MVec3& base )
{
    MVec3 right = Cross( base );
    return base.Cross( right );
}

//----------------------------------------------------------
void
MVec3::Orthonormalize( const MVec3& base )
{
    Orthogonalize( base );
    Normalize();
}

//----------------------------------------------------------
MVec3
MVec3::Orthonormalized( const MVec3& base )
{
    MVec3 temp( *this );
    temp.Orthonormalize( base );
    return temp;
}

//----------------------------------------------------------
void
MVec3::Normalize()
{
    // get the current magnitude.
    float curMag = Mag();
    B_ASSERT( Mag() >= kNormalizeEpsilon );

    // calculate the inverse magnitude and normalize.
    float invMag = 1.0f / curMag;
    _data[ 0 ] *= invMag;
    _data[ 1 ] *= invMag;
    _data[ 2 ] *= invMag;
}

//----------------------------------------------------------
void
MVec3::NormalizeFast()
{
    // performs a fast in-place approximate normalize.
    B_ASSERT( MagSqr() >= kNormalizeFastEpsilon );
    float invMag = InvSqrtFast( _data[0]*_data[0] + _data[1]*_data[1] + _data[2]*_data[2] );
    _data[ 0 ] *= invMag;
    _data[ 1 ] *= invMag;
    _data[ 2 ] *= invMag;
}

//----------------------------------------------------------
MVec3
MVec3::Normalized() const
{
    B_ASSERT( MagSqr() >= kNormalizeEpsilon );
    float invMag = 1.0f / Mag();
    return MVec3( invMag * _data[ 0 ], invMag * _data[ 1 ], invMag * _data[ 2 ] );
}

//----------------------------------------------------------
MVec3
MVec3::NormalizedFast() const
{
    B_ASSERT( MagSqr() >= kNormalizeFastEpsilon );
    float invMag = InvSqrtFast( _data[0]*_data[0] + _data[1]*_data[1] + _data[2]*_data[2] );
    return MVec3( invMag * _data[ 0 ], invMag * _data[ 1 ], invMag * _data[ 2 ] );
}

//----------------------------------------------------------
float
MVec3::Dist( const MVec3& pt ) const
{
    return Sqrt( DistSqr( pt ) );
}

//----------------------------------------------------------
float
MVec3::DistSqr( const MVec3& pt ) const
{
    float x = _data[ 0 ] - pt.GetX();
    float y = _data[ 1 ] - pt.GetY();
    float z = _data[ 2 ] - pt.GetZ();
    return( x*x+y*y+z*z );
}

//----------------------------------------------------------
float
MVec3::MagSqr() const
{
    return Dot( *this );
}

//----------------------------------------------------------
float
MVec3::Mag() const
{
    return Sqrt( MagSqr() );
}

//----------------------------------------------------------
MVec3
MVec3::Cross( const MVec3& v ) const
{
    return MVec3( _data[ 1 ]*v._data[ 2 ] - _data[ 2 ]*v._data[ 1 ],
                  _data[ 2 ]*v._data[ 0 ] - _data[ 0 ]*v._data[ 2 ],
                  _data[ 0 ]*v._data[ 1 ] - _data[ 1 ]*v._data[ 0 ] );
}

//----------------------------------------------------------
float&
MVec3::operator ()( int i )
{
    B_ASSERT( i < 3 );
    return _data[ i ];
}

//----------------------------------------------------------
float
MVec3::operator ()( int i ) const
{
    B_ASSERT( i < 3 );
    return _data[ i ];
}

//----------------------------------------------------------
MVec3
MVec3::operator * ( float v ) const
{
    return MVec3( _data[ 0 ] * v, _data[ 1 ] * v, _data[ 2 ] * v );
}

//----------------------------------------------------------
MVec3
MVec3::operator * ( const MVec3& v ) const
{
//  MVec3 d;
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      mov edx, output;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      mulps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  };
//  return d;
    return MVec3( _data[ 0 ]*v._data[ 0 ], _data[ 1 ]*v._data[ 1 ], _data[ 2 ]*v._data[ 2 ] );
}

//----------------------------------------------------------
MVec3
MVec3::operator / ( const MVec3& v ) const
{
//  MVec3 d;
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      mov edx, output;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      divps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  };
//  return d;
    return MVec3( _data[ 0 ] / v._data[ 0 ], _data[ 1 ] / v._data[ 1 ], _data[ 2 ] / v._data[ 2 ] );
}

//----------------------------------------------------------
MVec3
MVec3::operator + ( const MVec3& v ) const
{
//  MVec3 d;
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      mov edx, output;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      addps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  };
//  return d;
    return MVec3( _data[ 0 ] + v._data[ 0 ], _data[ 1 ] + v._data[ 1 ], _data[ 2 ] + v._data[ 2 ] );
}

//----------------------------------------------------------
MVec3
MVec3::operator - () const
{
    return MVec3( -_data[ 0 ], -_data[ 1 ], -_data[ 2 ] );
}

//----------------------------------------------------------
MVec3
MVec3::operator - ( const MVec3& v ) const
{
//  MVec3 d;
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      mov edx, output;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      subps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  };
//  return d;
    return MVec3( _data[ 0 ] - v._data[ 0 ], _data[ 1 ] - v._data[ 1 ], _data[ 2 ] - v._data[ 2 ] );
}

//----------------------------------------------------------
MVec3&
MVec3::operator *= ( const MVec3& v )
{
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      mulps xmm0, xmm1;
//      movaps [ eax ], xmm0;
//  };
    _data[ 0 ] *= v._data[ 0 ];
    _data[ 1 ] *= v._data[ 1 ];
    _data[ 2 ] *= v._data[ 2 ];

    return *this;
}

//----------------------------------------------------------
MVec3&
MVec3::operator *= ( float scale )
{
//  float *data1Ptr = _data;

    // SSE
//  __asm
//  {
//      mov ebx, data1Ptr;
//      movd xmm0, scale;
//      shufps xmm0, xmm0, 0x00;
//      movaps xmm1, [ ebx ];
//      mulps xmm0, xmm1;
//      movaps [ ebx ], xmm0;
//  };
    _data[ 0 ] *= scale;
    _data[ 1 ] *= scale;
    _data[ 2 ] *= scale;

    return *this;
}

//----------------------------------------------------------
MVec3&
MVec3::operator /= ( const MVec3& v )
{
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      divps xmm0, xmm1;
//      movaps [ eax ], xmm0;
//  };
    _data[ 0 ] /= v._data[ 0 ];
    _data[ 1 ] /= v._data[ 1 ];
    _data[ 2 ] /= v._data[ 2 ];

    return *this;
}

//----------------------------------------------------------
MVec3&
MVec3::operator += ( const MVec3& v )
{
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      addps xmm0, xmm1;
//      movaps [ eax ], xmm0;
//  };
    _data[ 0 ] += v._data[ 0 ];
    _data[ 1 ] += v._data[ 1 ];
    _data[ 2 ] += v._data[ 2 ];

    return *this;
}

//----------------------------------------------------------
MVec3&
MVec3::operator -= ( const MVec3& v )
{
//  float *data1Ptr = _data;
//  float *data2Ptr = v._data;

    // SSE
//  __asm
//  {
//      mov eax, data1Ptr;
//      mov ebx, data2Ptr;
//      movaps xmm0, [ eax ];
//      movaps xmm1, [ ebx ];
//      subps xmm0, xmm1;
//      movaps [ eax ], xmm0;
//  };
    _data[ 0 ] -= v._data[ 0 ];
    _data[ 1 ] -= v._data[ 1 ];
    _data[ 2 ] -= v._data[ 2 ];

    return *this;
}

//----------------------------------------------------------
MVec3
operator * ( float scale, MVec3& v )
{
//  MVec3 d;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data2Ptr;
//      mov edx, output;
//      movd xmm0, scale;
//      shufps xmm0, xmm0, 0x00;
//      movaps xmm1, [ eax ];
//      mulps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  }
//  return d;
    return MVec3( scale * v.GetX(), scale * v.GetY(), scale * v.GetZ() );
}

//----------------------------------------------------------
MVec3
operator * ( float scale, const MVec3& v )
{
//  MVec3 d;
//  float *data2Ptr = v._data;
//  float *output = d._data;

    // SSE
//  __asm
//  {
//      mov eax, data2Ptr;
//      mov edx, output;
//      movd xmm0, scale;
//      shufps xmm0, xmm0, 0x00;
//      movaps xmm1, [ eax ];
//      mulps xmm0, xmm1;
//      movaps [ edx ], xmm0;
//  }
//  return d;
    return MVec3( scale * v.GetX(), scale * v.GetY(), scale * v.GetZ() );
}

//----------------------------------------------------------
void
MVec3::Save( UWriter& writer ) const
{
    writer.WriteFloat( _data[ 0 ] );
    writer.WriteFloat( _data[ 1 ] );
    writer.WriteFloat( _data[ 2 ] );
}

//----------------------------------------------------------
void
MVec3::Load( UReader& reader )
{
    _data[ 0 ] = reader.ReadFloat();
    _data[ 1 ] = reader.ReadFloat();
    _data[ 2 ] = reader.ReadFloat();
}
