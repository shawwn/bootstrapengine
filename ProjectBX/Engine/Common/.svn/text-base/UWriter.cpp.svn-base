//----------------------------------------------------------
// File:		UWriter.cpp
// Author:		Kevin Bray
// Created:		09-19-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"
#include "UWriter.h"

//**********************************************************
// class UWriter
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UWriter::UWriter( void *buffer, unsigned long size, unsigned long growSize, bool own )
: _size( size )
, _buffer( buffer )
, _growSize( growSize )
, _offset( 0 )
, _ownPtr( own )
{
	B_ASSERT( growSize != 0 );
}

//----------------------------------------------------------
UWriter::UWriter( unsigned long initSize, unsigned long growSize )
: _size( initSize )
, _buffer( new char[ initSize ] )
, _growSize( growSize )
, _offset( 0 )
, _ownPtr( true )
{
	B_ASSERT( growSize != 0 );	
}

//----------------------------------------------------------
UWriter::~UWriter()
{
	if ( _ownPtr )
		delete[] ( char* )_buffer;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UWriter::WriteString( const tstring& str, bool writeNullTerm )
{
	// write string...
	unsigned int len = ( unsigned int )str.length() + ( writeNullTerm ? 1 : 0 );
	EnsureMemSize( len );
	MemCopy( ( ( char * )_buffer ) + _offset, str.c_str(), len );
	_offset += len;
}

//----------------------------------------------------------
void
UWriter::WriteString( const char* str, bool writeNullTerm )
{
	// write string...
	unsigned int len = ( unsigned int )strlen( str ) + ( writeNullTerm ? 1 : 0 );
	EnsureMemSize( len );
	MemCopy( ( ( char * )_buffer ) + _offset, str, len );
	_offset += len;
}

//----------------------------------------------------------
void
UWriter::WriteChar( char ch )
{
	EnsureMemSize( sizeof( char ) );
	*( ( ( char * )_buffer ) + _offset ) = ch;
	++_offset;
}

//----------------------------------------------------------
void
UWriter::WriteShort( short v )
{
	EnsureMemSize( sizeof( unsigned short ) );
	*( ( unsigned short* )( ( ( char* )_buffer ) + _offset ) ) = v;
	_offset += sizeof( unsigned short );	
}

//----------------------------------------------------------
void
UWriter::WriteInt( int i )
{
	EnsureMemSize( sizeof( int ) );
	*( ( int* )( ( ( char* )_buffer ) + _offset ) ) = i;
	_offset += sizeof( int );
}

//----------------------------------------------------------
void
UWriter::WriteUInt( unsigned int i )
{
	EnsureMemSize( sizeof( unsigned int ) );
	*( ( unsigned int * )( ( ( char * )_buffer ) + _offset ) ) = i;
	_offset += sizeof( unsigned int );
}

//----------------------------------------------------------
void
UWriter::WriteFloat( float f )
{
	EnsureMemSize( sizeof( float ) );
	*( ( float * )( ( ( char * )_buffer ) + _offset ) ) = f;
	_offset += sizeof( float );
}

//----------------------------------------------------------
void
UWriter::WriteBytes( const void *buffer, unsigned int count )
{
	EnsureMemSize( count );
	MemCopy( ( ( char * )_buffer ) + _offset, buffer, count );
	_offset += count;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
UWriter::EnsureMemSize( unsigned long bytesToWrite )
{
	unsigned long neededSize = _offset + bytesToWrite;
	if ( neededSize > _size )
	{
		// Someone is trying to write to more memory than is available, and we
		// cannot grow the memory because we do not own it!
		B_ASSERT( _ownPtr );

		// allocate new memory.
		unsigned long newSize = neededSize + ( _growSize - ( neededSize % _growSize ) );
		void *newMem = new char[ newSize ];

		// copy the old data to the new block and free the old memory.
		MemCopy( newMem, _buffer, GetBytesWritten() );
		delete[] ( char* )_buffer;

		// set the new buffer and size.
		_buffer = newMem;
		_size = newSize;
	}
}
