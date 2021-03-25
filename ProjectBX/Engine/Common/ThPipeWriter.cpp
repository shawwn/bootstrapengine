//----------------------------------------------------------
// File:		ThPipeWriter.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "ThPipeWriter.h"

// project includes.
#include "ThPipe.h"

//**********************************************************
// class ThPipedStream
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThPipeWriter::ThPipeWriter()
: _pipe( 0 )
, _writeRegion( 0 )
, _localOffset( 0 )
, _regionSize( 0 )
{

}

//----------------------------------------------------------
ThPipeWriter::ThPipeWriter( ThPipe* pipe )
: _pipe( pipe )
, _writeRegion( 0 )
, _localOffset( 0 )
, _regionSize( 0 )
{
	// get the region size if necessary.
	if ( pipe )
	{
		_regionSize = pipe->GetRegionSize();
		_writeRegion = ( unsigned char* )pipe->GetWriteRegion();
	}
}

//----------------------------------------------------------
ThPipeWriter::~ThPipeWriter()
{
	// flush data.
	Flush();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
ThPipeWriter::Reset( ThPipe* pipe )
{
	_pipe = pipe;
	_localOffset = 0;
	if ( pipe != 0 )
	{
		_regionSize = pipe->GetRegionSize();
		_writeRegion = ( unsigned char* )pipe->GetWriteRegion();
	}
	else
		_regionSize = 0;
}

//----------------------------------------------------------
void
ThPipeWriter::WriteString( const tstring& str, bool writeNullTerm )
{
	// write the string to the buffer.
	unsigned int lenAdj = writeNullTerm ? 1 : 0;
	WriteBytes( str.c_str(), ( unsigned int )str.length() + lenAdj );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteString( const char* str, bool writeNullTerm )
{
	unsigned int lenAdj = writeNullTerm ? 1 : 0;
	WriteBytes( str, ( unsigned int )strlen( str ) + lenAdj );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteChar( char ch )
{
	// write the character to the buffer.
	*GetWriteLoc< char >() = ch;

	// advance the offset.
	AdvanceOffset( sizeof( char ) );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteByte( unsigned char ch )
{
	// write the character to the buffer.
	*GetWriteLoc< unsigned char >() = ch;

	// advance the offset.
	AdvanceOffset( sizeof( unsigned char ) );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteShort( short v )
{
	WriteValue( v );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteInt( int i )
{
	WriteValue( i );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteUInt( unsigned int i )
{
	WriteValue( i );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteFloat( float f )
{
	WriteValue( f );
}

//----------------------------------------------------------
void
ThPipeWriter::WriteBytes( const void *buffer, unsigned int count )
{
	// make sure a pipe has been set!
	B_ASSERT( _pipe );

	// get the location we're reading from.
	const unsigned char* src = ( const unsigned char* )buffer;

	// copy data up to the first boundary.
	unsigned int bytesRemaining = count;
	unsigned int copySize = Min( bytesRemaining, ( _regionSize - _localOffset ) );
	MemCopy( _writeRegion + _localOffset, src, copySize );
	src += copySize;

	// copy remaining data in chunks.
	while ( bytesRemaining > 0 )
	{
		// calculate the size of the current copy.
		copySize = Min( bytesRemaining, _regionSize );

		// now copy the data.
		_writeRegion = ( unsigned char* )_pipe->NextWriteRegion();
		MemCopy( _writeRegion, src, copySize );

		// advance the source pointer and adjust the byteRemaining
		// counter.
		bytesRemaining -= copySize;
		src += copySize;
	}

	// advance the offset.
	AdvanceOffset( count );
}

//----------------------------------------------------------
void
ThPipeWriter::Flush()
{
	// flush the pipe if there is data waiting in the current region.
	if ( _localOffset > 0 )
		_writeRegion = ( unsigned char* )_pipe->NextWriteRegion();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
ThPipeWriter::AdvanceOffset( unsigned int count )
{
	// make sure a valid value was passed in!
	B_ASSERT( count > 0 );

	// adjust the local offset and flush if necessary.
	_localOffset = ( _localOffset + count ) % _regionSize;
	if ( _localOffset == 0 )
		_writeRegion = ( unsigned char* )_pipe->NextWriteRegion();
}

//----------------------------------------------------------
bool
ThPipeWriter::CrossesRegion( unsigned int count )
{
	unsigned int nextOffset = _localOffset + count;
	return ( nextOffset > _regionSize );
}
