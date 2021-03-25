//----------------------------------------------------------
// File:		UReader.cpp
// Author:		Kevin Bray
// Created:		09-19-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"
#include "UReader.h"

//**********************************************************
// class UReader
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UReader::UReader( const UReader& reader )
{
	Assign( reader );
}

//----------------------------------------------------------
UReader::UReader( const void *buffer, unsigned long size, bool takeOwnership )
: _buffer( buffer )
, _size( size )
, _offset( 0 )
, _own( takeOwnership )
{

}

//----------------------------------------------------------
UReader::~UReader()
{
	if ( _own )
		delete[] (char*)_buffer;
}


//==========================================================
// public members
//==========================================================

//----------------------------------------------------------
tstring
UReader::ReadString( bool peek )
{
	const char *str = ( ( const char * )_buffer ) + _offset;

	// make sure we hit a NULL char before the end of the data.
	if ( !peek )
	{
		while ( ( ( const char * )_buffer )[ _offset++ ] != '\0' )
		{
			B_ASSERT( _offset < _size );
		}
	}
	return str;
}

//----------------------------------------------------------
tstring
UReader::ReadLine( bool peek )
{
	tstring newString;

	// make sure we hit a newline before the end of the data.
	unsigned long offset = _offset;
	unsigned long newLen = 0;
	while ( ( ( const char * )_buffer )[ offset++ ] != '\n' )
	{
		++newLen;

		// make sure we didn't hit the end of the file!
		if ( offset == _size )
			break;
	}

	// skip the carriage return if necessary.
	if ( offset != _size && ( ( const char * )_buffer )[ offset ] == '\r' )
		++offset;

	// copy to the new string.
	newString.append( ( ( const char * )_buffer ) + _offset, newLen );

	// advance the offset if necessary.
	if ( !peek )
	{
		_offset = offset;
	}

	// return the newly created string.
	return newString;
}

//----------------------------------------------------------
int
UReader::CurLineLen() const
{
	// make sure we hit a newline before the end of the data.
	unsigned long offset = _offset;
	while ( ( ( const char * )_buffer )[ offset++ ] != '\n' )
	{
		// if we hit the end of the file, then we hit the end of the
		// line.
		if ( offset == _size )
			break;
	}

	// add the carriage return if necessary.
	if ( offset != _size && ( ( const char * )_buffer )[ offset ] == '\r' )
		++offset;

	// return the line length.
	return( offset - _offset );
}

//----------------------------------------------------------
char
UReader::ReadChar( bool peek )
{
	B_ASSERT( _offset < _size );
	char ch = ( ( const char * )_buffer )[ _offset ];
	if ( !peek )
		_offset += sizeof( char );
	return ch;
}

//----------------------------------------------------------
unsigned char
UReader::ReadUChar( bool peek )
{
	B_ASSERT( _offset < _size );
	unsigned char ch = ( ( const unsigned char * )_buffer )[ _offset ];
	if ( !peek )
		_offset += sizeof( unsigned char );
	return ch;
}

//----------------------------------------------------------
short
UReader::ReadShort( bool peek )
{
	B_ASSERT( _offset < _size );
	B_ASSERT( _offset + sizeof( short ) <= _size );
	short val = *( ( short * )( ( const char * )_buffer + _offset ) );
	if ( !peek )
		_offset += sizeof( short );
	return val;
}

//----------------------------------------------------------
unsigned short
UReader::ReadUShort( bool peek )
{
	B_ASSERT( _offset < _size );
	B_ASSERT( _offset + sizeof( unsigned short ) <= _size );
	short val = *( ( unsigned short * )( ( const char * )_buffer + _offset ) );
	if ( !peek )
		_offset += sizeof( unsigned short );
	return val;
}

//----------------------------------------------------------
int
UReader::ReadInt( bool peek )
{
	B_ASSERT( _offset < _size );
	B_ASSERT( _offset + sizeof( int ) <= _size );
	int val = *( ( int * )( ( const char * )_buffer + _offset ) );
	if ( !peek )
		_offset += sizeof( int );
	return val;
}

//----------------------------------------------------------
unsigned int
UReader::ReadUInt( bool peek )
{
	B_ASSERT( _offset < _size );
	B_ASSERT( _offset + sizeof( unsigned int ) <= _size );
	unsigned int val = *( ( unsigned int* )( ( const char* )_buffer + _offset ) );
	if ( !peek )
		_offset += sizeof( unsigned int );
	return val;
}

//----------------------------------------------------------
float
UReader::ReadFloat( bool peek )
{
	B_ASSERT( _offset < _size );
	B_ASSERT( _offset + sizeof( float ) <= _size );
	float val = *( ( float * )( ( const char * )_buffer + _offset ) );
	if ( !peek )
		_offset += sizeof( float );
	return val;
}

//----------------------------------------------------------
void
UReader::ReadBytes( void *buffer, unsigned int count, bool peek )
{
	B_ASSERT( ( _offset + count ) <= _size );
	MemCopy( buffer, ( const char * )_buffer + _offset, count );
	if ( !peek )
		_offset += count;
}

//----------------------------------------------------------
void
UReader::Skip( unsigned long bytes )
{
	B_ASSERT( ( _offset + bytes ) <= _size );
	_offset += bytes;
}

//----------------------------------------------------------
void
UReader::Rewind( unsigned long bytes )
{
	B_ASSERT( bytes <= _offset );
	_offset -= bytes;
}

//----------------------------------------------------------
UReader&
UReader::operator = ( const UReader& reader )
{
	Assign( reader );
	return *this;
}


//==========================================================
// public members
//==========================================================

//----------------------------------------------------------
void
UReader::Assign( const UReader& reader )
{
	if ( _own )
		delete[] (char*)_buffer;

	_offset = reader._offset;
	_size = reader._size;
	_own = reader._own;
	if ( reader._own )
	{
		void* buffer = new char[ reader._size ];
		MemCopy( buffer, reader._buffer, _size );
		_buffer = buffer;
	}
	else
		_buffer = reader._buffer;
}
