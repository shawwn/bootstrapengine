//----------------------------------------------------------
// File:        ThPipeWriter.h
// Author:      Kevin Bray
// Created:     09-01-08
//
// Purpose:     To provide a stream like interface over pipes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ThSection.h"
#include "ThPipe.h"

//**********************************************************
// class ThPipeWriter
//**********************************************************
class ThPipeWriter
{
public:
    // initialization.
    ThPipeWriter();
    ThPipeWriter( ThPipe* pipe );
    ~ThPipeWriter();

    // resets the stream interface.  Note that this does not
    // do a flush.
    void            Reset( ThPipe* pipe );

    // writes data to the pipe.
    void            WriteString( const tstring& str, bool writeNullTerm = true );
    void            WriteString( const char* str, bool writeNullTerm = true );
    void            WriteChar( char ch );
    void            WriteByte( unsigned char ch );
    void            WriteShort( short v );
    void            WriteInt( int i );
    void            WriteUInt( unsigned int i );
    void            WriteFloat( float f );
    void            WriteBytes( const void *source, unsigned int size );

    template < typename _T >
    void            WriteValue( const _T& value );

    // flushes any waiting data in the pipe.  This should be
    // called whenever data is to be sent off.
    void            Flush();

private:
    void            AdvanceOffset( unsigned int count );
    bool            CrossesRegion( unsigned int count );

    template< typename _T >
    _T*             GetWriteLoc() const;

    ThPipe*         _pipe;
    unsigned char*  _writeRegion;
    unsigned int    _localOffset;
    unsigned int    _regionSize;
};


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
void
ThPipeWriter::WriteValue( const _T& value )
{
    // write the value directly or choose the slow path for
    // boundary conditions.
    if ( !CrossesRegion( sizeof( _T ) ) )
    {
        // advance to the current offset.
        *GetWriteLoc< _T >() = value;
        AdvanceOffset( sizeof( _T ) );
    }
    else
        WriteBytes( &value, sizeof( _T ) );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
_T*
ThPipeWriter::GetWriteLoc() const
{
    // return the write location casted to the correct type.
    return ( _T* )( _writeRegion + _localOffset );
}
