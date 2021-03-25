//----------------------------------------------------------
// File:		ThPipeReader.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "ThPipeReader.h"

// project includes.
#include "ThPipe.h"
#include "ThSignal.h"

//**********************************************************
// class ThPipedStream
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThPipeReader::ThPipeReader()
: _pipe( 0 )
, _localOffset( 0 )
, _regionSize( 0 )
{

}

//----------------------------------------------------------
ThPipeReader::ThPipeReader( ThPipe* pipe )
: _pipe( pipe )
, _localOffset( 0 )
, _regionSize( 0 )
{
	// get the region size if necessary.
	if ( pipe )
	{
		_regionSize = pipe->GetRegionSize();
		_readRegion = ( unsigned char* )pipe->GetReadRegion();
	}
}

//----------------------------------------------------------
ThPipeReader::~ThPipeReader()
{
	Flush();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
ThPipeReader::Reset( ThPipe* pipe )
{
	_pipe = pipe;
	_localOffset = 0;
	if ( pipe != 0 )
	{
		_regionSize = pipe->GetRegionSize();
		_readRegion = ( unsigned char* )pipe->GetReadRegion();
	}
	else
		_regionSize = 0;
}

//----------------------------------------------------------
tstring
ThPipeReader::ReadString()
{
	// read until we hit a null terminator.
	tstring str;
	char ch = ReadChar();
	while ( ch != 0 )
	{
		// add the current character to the string and
		// get the next character.
		str += ch;
		ch = ReadChar();
	}

	// return the string.
	return str;
}

//----------------------------------------------------------
char
ThPipeReader::ReadChar()
{
	// read the character and advance the offset.
	char ch = *GetReadLoc< char >();
	AdvanceOffset( sizeof( char ) );

	// return the character.
	return ch;
}

//----------------------------------------------------------
unsigned char
ThPipeReader::ReadUChar()
{
	// read the character and advance the offset.
	unsigned char ch = *GetReadLoc< unsigned char >();
	AdvanceOffset( sizeof( unsigned char ) );

	// return the character.
	return ch;
}

//----------------------------------------------------------
short
ThPipeReader::ReadShort()
{
	return ReadValue< short >();
}

//----------------------------------------------------------
unsigned short
ThPipeReader::ReadUShort()
{
	return ReadValue< unsigned short >();
}

//----------------------------------------------------------
int
ThPipeReader::ReadInt()
{
	return ReadValue< int >();
}

//----------------------------------------------------------
unsigned int
ThPipeReader::ReadUInt()
{
	return ReadValue< unsigned int >();
}

//----------------------------------------------------------
float
ThPipeReader::ReadFloat()
{
	return ReadValue< float >();
}

//----------------------------------------------------------
void
ThPipeReader::ReadBytes( void *buffer, unsigned int count )
{
	// make sure a pipe has been set!
	B_ASSERT( _pipe );

	// get the location we're reading from.
	unsigned char* dst = ( unsigned char* )buffer;

	// copy data up to the first boundary.
	unsigned int bytesRemaining = count;
	unsigned int copySize = Min( bytesRemaining, ( _regionSize - _localOffset ) );
	MemCopy( dst, _readRegion + _localOffset, copySize );
	dst += copySize;

	// copy remaining data in chunks.
	while ( bytesRemaining > 0 )
	{
		// calculate the size of the current copy.
		copySize = Min( bytesRemaining, _regionSize );

		// now copy the data.
		_readRegion = ( unsigned char* )_pipe->NextReadRegion();
		MemCopy( dst, _readRegion, copySize );

		// advance the source pointer and adjust the byteRemaining
		// counter.
		bytesRemaining -= copySize;
		dst += copySize;
	}

	// advance the offset.
	AdvanceOffset( count );
}

//----------------------------------------------------------
void
ThPipeReader::Skip( unsigned int bytes )
{
	// make sure a pipe has been set!
	B_ASSERT( _pipe );

	// advance to the first boundary.
	unsigned int bytesRemaining = bytes;
	bytesRemaining -= Min( bytesRemaining, ( _regionSize - _localOffset ) );

	// skip regions.
	while ( bytesRemaining > 0 )
	{
		// if we're crossing a region, advance to the next region.
		if ( CrossesRegion( bytesRemaining ) )
			_readRegion = ( unsigned char* )_pipe->NextReadRegion();

		// adjust the bytes remaining.
		bytesRemaining -= Min( bytesRemaining, _regionSize );
	}
}

//----------------------------------------------------------
void
ThPipeReader::Flush()
{
	if ( _localOffset == 0 )
		_readRegion = ( unsigned char* )_pipe->NextReadRegion();
}

//----------------------------------------------------------
bool
ThPipeReader::IsDataWaiting() const
{
	return _pipe->GetReadSignal().IsRaised();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
ThPipeReader::AdvanceOffset( unsigned int count )
{
	// make sure a valid value was passed in!
	B_ASSERT( count > 0 );

	// adjust the local offset and flush if necessary.
	_localOffset = ( _localOffset + count ) % _regionSize;
	if ( _localOffset == 0 )
		_readRegion = ( unsigned char* )_pipe->NextReadRegion();
}

//----------------------------------------------------------
bool
ThPipeReader::CrossesRegion( unsigned int count )
{
	unsigned int nextOffset = _localOffset + count;
	return ( nextOffset > _regionSize );
}
