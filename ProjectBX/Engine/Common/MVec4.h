//----------------------------------------------------------
// File:        MVec4.h
// Author:      Kevin Bray
// Created:     11-20-05
//
// Purpose:     To contain a 4 component vector.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec3;
class UReader;
class UWriter;

//**********************************************************
// class MVec4
//**********************************************************
class MVec4
{
public:
    MVec4();
    MVec4( float x, float y, float z, float w );
    MVec4( const MVec3& v, float w );
    MVec4( const MVec4& v );

    // assignment operators.
    MVec4&          operator = ( const MVec4& v );

    // arithmetic operators.
    MVec4&          operator *= ( float s );

    void            SetX( float x )         {   _data[ 0 ] = x;     }
    void            SetY( float y )         {   _data[ 1 ] = y;     }
    void            SetZ( float z )         {   _data[ 2 ] = z;     }
    void            SetW( float w )         {   _data[ 3 ] = w;     }

    float           GetX() const            {   return _data[ 0 ];  }
    float           GetY() const            {   return _data[ 1 ];  }
    float           GetZ() const            {   return _data[ 2 ];  }
    float           GetW() const            {   return _data[ 3 ];  }

    const float*    GetDataStart() const    {   return _data;       }

    void            MakeVec3( MVec3& vec ) const;

    void            Save( UWriter& writer );
    void            Load( UReader& reader );

private:
    float           _array[ 7 ];
    float*          _data;
};
