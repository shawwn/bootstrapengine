//----------------------------------------------------------
// File:        MVec4.cpp
// Author:      Kevin Bray
// Created:     11-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MVec4.h"

// project includes.
#include "MVec3.h"
#include "UReader.h"
#include "UWriter.h"


//**********************************************************
// class MVec4
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MVec4::MVec4()
{
    _data = Align16( _array );
    MemSet( _array, 0, sizeof( _array ) );
}

//----------------------------------------------------------
MVec4::MVec4( float x, float y, float z, float w )
{
    _data = Align16( _array );
    _data[ 0 ] = x;
    _data[ 1 ] = y;
    _data[ 2 ] = z;
    _data[ 3 ] = w;
}

//----------------------------------------------------------
MVec4::MVec4( const MVec3& vec, float w )
{
    _data = Align16( _array );
    _data[ 0 ] = vec.GetX();
    _data[ 1 ] = vec.GetY();
    _data[ 2 ] = vec.GetZ();
    _data[ 3 ] = w;
}

//----------------------------------------------------------
MVec4::MVec4( const MVec4& v )
{
    _data = Align16( _array );
    _data[ 0 ] = v._data[ 0 ];
    _data[ 1 ] = v._data[ 1 ];
    _data[ 2 ] = v._data[ 2 ];
    _data[ 3 ] = v._data[ 3 ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MVec4&
MVec4::operator = ( const MVec4& v )
{
    _data[ 0 ] = v._data[ 0 ];
    _data[ 1 ] = v._data[ 1 ];
    _data[ 2 ] = v._data[ 2 ];
    _data[ 3 ] = v._data[ 3 ];
    return *this;
}

//----------------------------------------------------------
MVec4&
MVec4::operator *= ( float s )
{
    _data[ 0 ] *= s;
    _data[ 1 ] *= s;
    _data[ 2 ] *= s;
    _data[ 3 ] *= s;
    return *this;
}

//----------------------------------------------------------
void
MVec4::MakeVec3( MVec3& vec ) const
{
    vec.SetX( _data[ 0 ] );
    vec.SetY( _data[ 1 ] );
    vec.SetZ( _data[ 2 ] );
}

//----------------------------------------------------------
void
MVec4::Save( UWriter& writer )
{
    writer.WriteFloat( _data[ 0 ] );
    writer.WriteFloat( _data[ 1 ] );
    writer.WriteFloat( _data[ 2 ] );
    writer.WriteFloat( _data[ 3 ] );
}

//----------------------------------------------------------
void
MVec4::Load( UReader& reader )
{
    _data[ 0 ] = reader.ReadFloat();
    _data[ 1 ] = reader.ReadFloat();
    _data[ 2 ] = reader.ReadFloat();
    _data[ 3 ] = reader.ReadFloat();
}
