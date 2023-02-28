//----------------------------------------------------------
// File:        UWriter.h
// Author:      Kevin Bray
// Created:     09-19-04
//
// Purpose:     To wrap and simplify pointer write operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UWriter
//**********************************************************
class UWriter
{
public:
    UWriter( void *buffer, unsigned long size, unsigned long growSize, bool own );
    UWriter( unsigned long initSize = 512, unsigned long growSize = 512 );
    ~UWriter();

    void            WriteString( const tstring& str, bool writeNullTerm = true );
    void            WriteString( const char* str, bool writeNullTerm = true );
    void            WriteChar( char ch );
    void            WriteShort( short v );
    void            WriteInt( int i );
    void            WriteUInt( unsigned int i );
    void            WriteFloat( float f );
    void            WriteBytes( const void *buffer, unsigned int count );

    unsigned long   GetBytesWritten() const                 {   return _offset;     }
    const void *    GetBufferPtr() const                    {   return _buffer;     }

private:
    // no copies.
    UWriter( const UWriter& writer ) { }
    UWriter& operator = ( const UWriter& writer ) { return *this; }

    void            EnsureMemSize( unsigned long bytesToWrite );

    unsigned long   _size;
    unsigned long   _growSize;
    unsigned long   _offset;
    void *          _buffer;
    bool            _ownPtr;
};
