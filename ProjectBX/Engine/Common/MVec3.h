//----------------------------------------------------------
// File:        MVec3.h
// Author:      Kevin Bray
// Created:     10-05-04
//
// Purpose:     performs vector operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

class UReader;
class UWriter;

//#define C_MVEC3_NORMALIZE_EPS         0.00000095367431640625f
//#define C_MVEC3_NORMALIZE_FAST_EPS        0.0009765625f

//**********************************************************
// class MVec3
//**********************************************************
class MVec3
{
public:
    static const float kNormalizeEpsilon;
    static const float kNormalizeFastEpsilon;

    MVec3();
    MVec3( float x, float y, float z );
    MVec3( const float* vec );
    MVec3( const MVec3& v );
    ~MVec3();

    MVec3&          operator = ( const MVec3& v );
    void            AssignMin( const MVec3& v );
    void            AssignMax( const MVec3& v );

    float*          GetDataStart()          {   return _data;       }
    const float*    GetDataStart() const    {   return _data;       }

    float           GetX() const            {   return _data[ 0 ];  }
    float           GetY() const            {   return _data[ 1 ];  }
    float           GetZ() const            {   return _data[ 2 ];  }

    void            SetX( float x )         {   _data[ 0 ] = x;     }
    void            SetY( float y )         {   _data[ 1 ] = y;     }
    void            SetZ( float z )         {   _data[ 2 ] = z;     }

    float           Max() const             {   return ::Max( _data[ 0 ], ::Max( _data[ 1 ], _data[ 2 ] ) );    }
    float           Min() const             {   return ::Min( _data[ 0 ], ::Min( _data[ 1 ], _data[ 2 ] ) );    }
    float           AbsMax() const          {   return ::Max( ::Abs( _data[ 0 ] ), ::Max( ::Abs( _data[ 1 ] ), ::Abs( _data[ 2 ] ) ) ); }
    float           AbsMin() const          {   return ::Min( ::Abs( _data[ 0 ] ), ::Min( ::Abs( _data[ 1 ] ), ::Abs( _data[ 2 ] ) ) ); }

    MVec3           Abs() const             {   return MVec3( ::Abs( _data[ 0 ] ), ::Abs( _data[ 1 ] ), ::Abs( _data[ 2 ] ) );  }

    void            Orthogonalize( const MVec3& base );
    MVec3           Orthogonalized( const MVec3& base );
    void            Orthonormalize( const MVec3& base );
    MVec3           Orthonormalized( const MVec3& base );

    void            Normalize();
    void            NormalizeFast();
    MVec3           Normalized() const;
    MVec3           NormalizedFast() const;

    void            Negate()                {   _data[0] = -_data[0]; _data[1] = -_data[1]; _data[2] = -_data[2];   }

    float           Dist( const MVec3& pt ) const;
    float           DistSqr( const MVec3& pt ) const;
    float           MagSqr() const;
    float           Mag() const;
    float           Dot( const MVec3& v ) const     {   return _data[ 0 ]*v._data[ 0 ] + _data[ 1 ]*v._data[ 1 ] + _data[ 2 ]*v._data[ 2 ]; }
    MVec3           Cross( const MVec3& v ) const;

    float&          operator ()( int i );
    float           operator ()( int i ) const;

    // math operators
    MVec3           operator * ( float v ) const;
    MVec3           operator * ( const MVec3& v ) const;
    MVec3           operator / ( const MVec3& v ) const;
    MVec3           operator + ( const MVec3& v ) const;
    MVec3           operator - ( const MVec3& v ) const;
    MVec3           operator - () const;
    MVec3&          operator *= ( const MVec3& v );
    MVec3&          operator *= ( float scale );
    MVec3&          operator /= ( const MVec3& v );
    MVec3&          operator += ( const MVec3& v );
    MVec3&          operator -= ( const MVec3& v );

    friend MVec3    operator * ( float, MVec3& v );
    friend MVec3    operator * ( float, const MVec3& v );

    operator float* ()          {   return _data;   }
    operator const float* () const  {   return _data;   }

    // save/load.
    void            Save( UWriter& writer ) const;
    void            Load( UReader& reader );

private:
    float           _array[ 7 ];
    float*          _data;
};

MVec3
operator * ( float scale, const MVec3& v );

MVec3
operator * ( float scale, MVec3& v );
