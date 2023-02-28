//----------------------------------------------------------
// File:        GrMesh.h
// Author:      Kevin Bray
// Created:     10-04-04
//
// Purpose:     Contains mesh data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "MVec4.h"
#include "MVec3.h"
#include "MVec2.h"

//**********************************************************
// struct SVec2
//**********************************************************
struct SVec2
{
    SVec2() : x( 0 ), y( 0 ) { }
    SVec2( float x, float y ) : x( x ), y( y ) { }
    SVec2( const MVec2& v );

    void    Min( const SVec2& v )                           {   x = ::Min( x, v.x ); y = ::Min( y, v.y );       }
    void    Max( const SVec2& v )                           {   x = ::Max( x, v.x ); y = ::Max( y, v.y );       }

    void    Min3( const SVec2& v1, const SVec2& v2 )        {   x = ::Min( x, ::Min( v1.x, v2.x ) );    y = ::Min( y, ::Min( v1.y, v2.y ) );    }
    void    Max3( const SVec2& v1, const SVec2& v2 )        {   x = ::Max( x, ::Max( v1.x, v2.x ) );    y = ::Max( y, ::Max( v1.y, v2.y ) );    }

    float   Normalize();
    SVec2   Normalized() const;
    float   InvMag() const;
    float   Mag() const                                     {   return Sqrt( MagSqr() );                        }
    float   MagSqr() const                                  {   return x * x + y * y;                           }

    float   Dist( const SVec2& v ) const;
    float   DistSqr( const SVec2& v ) const;

    SVec2&  operator = ( const MVec2& v );
    bool    operator == ( const SVec2& v ) const;
    bool    operator != ( const SVec2& v ) const;

    SVec2   operator + ( const SVec2& v ) const             {   return SVec2( x+v.x, y+v.y );                   }
    SVec2   operator - ( const SVec2& v ) const             {   return SVec2( x-v.x, y-v.y );                   }
    SVec2   operator * ( const SVec2& v ) const             {   return SVec2( x*v.x, y*v.y );                   }
    SVec2   operator / ( const SVec2& v ) const             {   return SVec2( x/v.x, y/v.y );                   }
    SVec2   operator - () const                             {   return SVec2( -x, -y );                         }

    SVec2&  operator *= ( const SVec2& v )                  {   x*=v.x; y*=v.y; return *this;                   }
    SVec2&  operator *= ( float s )                         {   x*=s; y*=s; return *this;                       }
    SVec2&  operator /= ( const SVec2& v )                  {   x/=v.x; y/=v.y; return *this;                   }
    SVec2&  operator /= ( float v )                 {   x/=v; y/=v; return *this;                   }
    SVec2&  operator += ( const SVec2& v )                  {   x+=v.x; y+=v.y; return *this;                   }
    SVec2&  operator -= ( const SVec2& v )                  {   x-=v.x; y-=v.y; return *this;                   }

    operator MVec2 () const                                 {   return MVec2( x, y );                           }
    float& operator [] ( unsigned int idx )                 {   B_ASSERT( idx < 2 ); return *( &x + idx );      }
    const float& operator [] ( unsigned int idx ) const     {   B_ASSERT( idx < 2 ); return *( &x + idx );      }

    float x;
    float y;
};
inline SVec2 operator * ( float s, const SVec2& v )         {   return SVec2( s*v.x, s*v.y );                   }

//**********************************************************
// struct SVec3
//**********************************************************
struct SVec3
{
    SVec3() : x( 0 ), y( 0 ), z( 0 ) { }
    SVec3( float x, float y, float z ) : x( x ), y( y ), z( z ) { }
    SVec3( const MVec3& vec );

    float           Dist( const SVec3& vec ) const          {   return Sqrt( DistSqr( vec ) );                          }
    inline float    DistSqr( const SVec3& vec ) const;

    float           Mag() const                             {   return Sqrt( MagSqr() );                                }
    float           MagSqr() const                          {   return x*x+y*y+z*z;                                     }

    float           Dot( const SVec3& vec ) const           {   return x*vec.x + y*vec.y + z*vec.z;                     }
    float           Dot( const MVec3& vec ) const           {   return x*vec.GetX() + y*vec.GetY() + z*vec.GetZ();      }

    float           AbsMax() const                          {   return Max( Abs( x ), Max( Abs( y ), Abs( z ) ) );      }

    inline void     Normalize();
    inline SVec3    Normalized() const;

    inline void     NormalizeFast();
    inline SVec3    NormalizedFast() const;

    SVec3&          operator = ( const MVec3& v );
    bool            operator == ( const SVec3& v ) const;
    bool            operator != ( const SVec3& v ) const;

    SVec3           operator + ( const SVec3& v ) const     {   return SVec3( x+v.x, y+v.y, z+v.z );                    }
    SVec3           operator - ( const SVec3& v ) const     {   return SVec3( x-v.x, y-v.y, z-v.z );                    }
    SVec3           operator * ( const SVec3& v ) const     {   return SVec3( x*v.x, y*v.y, z*v.z );                    }
    SVec3           operator * ( float f ) const        {   return SVec3( x*f, y*f, z*f );                  }
    SVec3           operator / ( const SVec3& v ) const     {   return SVec3( x/v.x, y/v.y, z/v.z );                    }
    SVec3           operator / ( const float s ) const      {   return SVec3( x/s, y/s, z/s );                  }
    SVec3           operator - () const                     {   return SVec3( -x, -y, -z );                             }

    SVec3&          operator *= ( const SVec3& v )          {   x*=v.x; y*=v.y; z*=v.z; return *this;                   }
    SVec3&          operator *= ( float s )                 {   x*=s; y*=s; z*=s; return *this;                         }
    SVec3&          operator /= ( const SVec3& v )          {   x/=v.x; y/=v.y; z/=v.z; return *this;                   }
    SVec3&          operator /= ( float s )                 {   x/=s; y/=s; z/=s; return *this;                         }
    SVec3&          operator += ( const SVec3& v )          {   x+=v.x; y+=v.y; z+=v.z; return *this;                   }
    SVec3&          operator += ( float s )         {   x+=s; y+=s; z+=s; return *this;                 }
    SVec3&          operator -= ( const SVec3& v )          {   x-=v.x; y-=v.y; z-=v.z; return *this;                   }
    SVec3&          operator -= ( float s )         {   x-=s; y-=s; z-=s; return *this;                 }

    float&          operator [] ( unsigned int idx )        {   B_ASSERT( idx < 3 ); return *( &x + idx );              }
    const float&    operator [] ( unsigned int idx ) const  {   B_ASSERT( idx < 3 ); return *( &x + idx );              }

    operator MVec3 () const                                 {   return MVec3( x, y, z );                                }

    SVec3           Cross( const SVec3& v ) const           {   return SVec3( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x );  }

    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float v[ 3 ];
    };
};
inline SVec3 operator * ( float s, const SVec3& v ) {   return SVec3( s*v.x, s*v.y, s*v.z );                    }

//----------------------------------------------------------
inline float
SVec3::DistSqr( const SVec3& vec ) const
{
    float dx = x - vec.x;
    float dy = y - vec.y;
    float dz = z - vec.z;
    return dx*dx + dy*dy + dz*dz;
}

//----------------------------------------------------------
inline void
SVec3::Normalize()
{
    // is the vector too small?
    B_ASSERT( 
        !ApproxEqual( x, 0.0f, FLT_EPSILON ) ||
        !ApproxEqual( y, 0.0f, FLT_EPSILON ) ||
        !ApproxEqual( z, 0.0f, FLT_EPSILON ) );

    // normalize.
    float invMag = 1.0f / Mag();
    x *= invMag;
    y *= invMag;
    z *= invMag;
}

//----------------------------------------------------------
inline SVec3
SVec3::Normalized() const
{
    // is the vector too small?
    B_ASSERT( MagSqr() > 0.00001f );

    // normalize.
    float invMag = 1.0f / Mag();
    return SVec3( x*invMag, y*invMag, z*invMag );
}

//----------------------------------------------------------
inline void
SVec3::NormalizeFast()
{
    float invMag = InvSqrtFast( x*x + y*y + z*z );
    x *= invMag;
    y *= invMag;
    z *= invMag;
}

//----------------------------------------------------------
inline SVec3
SVec3::NormalizedFast() const
{
    float invMag = InvSqrtFast( x*x + y*y + z*z );
    return SVec3( x*invMag, y*invMag, z*invMag );
}


//**********************************************************
// struct SVec4
//**********************************************************

//----------------------------------------------------------
struct SVec4
{
    SVec4() : x( 0 ), y( 0 ), z( 0 ), w( 1 ) { }
    SVec4( float x, float y, float z, float w ) : x( x ), y( y ), z( z ), w( w ) { }
    SVec4( const MVec4& v ) : x( v.GetX() ), y( v.GetY() ), z( v.GetZ() ), w( v.GetW() ) { }

    inline SVec4& operator += ( const SVec4& rhs )
    {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
        return *this;
    }

    inline SVec4& operator = ( const SVec3& rhs )
    {
        x = rhs.x; y = rhs.y; z = rhs.z; w = 1.0f;
        return *this;
    }

    inline SVec4& operator = ( const SVec4& rhs )
    {
        x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
        return *this;
    }

    inline SVec4& operator = ( const MVec4& rhs )
    {
        x = rhs.GetX(); y = rhs.GetY(); z = rhs.GetZ(); w = rhs.GetW();
        return *this;
    }

    inline float operator [] ( int idx ) const { return *(float*)this; }
    inline float& operator [] ( int idx ) { return *(float*)this; }

    inline float Dot( const SVec4& v ) const        {   return x*v.x + y*v.y + z*v.z + v.w*w;   }
    inline float Dot( const SVec3& v ) const        {   return x*v.x + y*v.y + z*v.z + w;       }

    inline SVec4 operator - ( const SVec4& rhs )
    {
        return SVec4( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w );
    }

    inline SVec4& operator -= ( const SVec4& rhs )
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
};

inline SVec4 operator * ( float scale, const SVec4& vec )
{
    return SVec4( scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w );
}


//**********************************************************
// struct SByteVec3
//**********************************************************
struct SByteVec3
{
    SByteVec3() : x( 0 ), y( 0 ), z( 0 ) { }
    SByteVec3( float x, float y, float z );
    SByteVec3( const MVec3& vec );
    SByteVec3( const SVec3& vec );

    SByteVec3&  operator = ( const MVec3& vec );
    SByteVec3&  operator = ( const SVec3& vec );

    operator MVec3 () const                                 {   return MVec3( ExpandValue( x ), ExpandValue( y ), ExpandValue( z ) );   }

    bool        operator == ( const SByteVec3& vec );
    bool        operator != ( const SByteVec3& vec );

    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char unused;

    static unsigned char ClampValue( float v );
    static float ExpandValue( unsigned char v );
};

//**********************************************************
// struct SLine
//**********************************************************
struct SLine
{
    SLine();
    SLine( const SVec3& pt0, const SVec3& pt1 );

    SVec3 pt0;
    SVec3 pt1;
};

//**********************************************************
// struct STriangle
//**********************************************************
struct STriangle
{
    STriangle();    // initializes as degenerate ( having no area ).
    STriangle( const SVec3& pt0, const SVec3& pt1, const SVec3& pt2 );

    bool            IsDegenerate();
    bool            IsDegenerate( float epsilon );

    SVec3 pt0;
    SVec3 pt1;
    SVec3 pt2;
};

//**********************************************************
// struct SQuad
//**********************************************************
struct SQuad
{
    SQuad();        // initializes as degenerate ( having no area ).  up and right are in quad space.
    SQuad( const SVec3& pos, const SVec3& up, const SVec3& right, const SVec3& normal );

    SVec3 pos;
    SVec3 up;
    SVec3 right;
    SVec3 normal;
};
