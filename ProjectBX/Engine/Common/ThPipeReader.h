//----------------------------------------------------------
// File:        ThPipeReader.h
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
// class ThPipeReader
//**********************************************************
class ThPipeReader
{
public:
    // initialization.
    ThPipeReader();
    ThPipeReader( ThPipe* pipe );
    ~ThPipeReader();

    // resets the stream interface.  Note that this does not
    // do a flush.
    void            Reset( ThPipe* pipe );

    tstring         ReadString();
    char            ReadChar();
    unsigned char   ReadUChar();
    short           ReadShort();
    unsigned short  ReadUShort();
    int             ReadInt();
    unsigned int    ReadUInt();
    float           ReadFloat();
    void            ReadBytes( void *buffer, unsigned int count );

    template < typename _T >
    _T              ReadValue();

    void            Skip( unsigned int bytes );

    // flushes the read buffer and moves to the next read region.
    void            Flush();

    // returns true if there is data waiting to be read.
    bool            IsDataWaiting() const;

private:
    void            AdvanceOffset( unsigned int count );
    bool            CrossesRegion( unsigned int count );

    template< typename _T >
    _T*             GetReadLoc() const;

    ThPipe*         _pipe;
    unsigned char*  _readRegion;
    unsigned int    _localOffset;
    unsigned int    _regionSize;
};


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
_T
ThPipeReader::ReadValue()
{
    // create a spot to hold the value we're reading in.
    _T value;

    // read the value directly or choose the slow path for
    // boundary conditions.
    if ( !CrossesRegion( sizeof( _T ) ) )
    {
        // advance to the current offset.
        value = *GetReadLoc< _T >();
        AdvanceOffset( sizeof( _T ) );      
    }
    else
        ReadBytes( &value, sizeof( _T ) );

    // return the value.
    return value;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
_T*
ThPipeReader::GetReadLoc() const
{
    // return the write location in the correct type.
    return ( _T* )( _readRegion + _localOffset );
}
