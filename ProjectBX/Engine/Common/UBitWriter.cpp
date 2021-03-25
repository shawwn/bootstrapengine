//----------------------------------------------------------
// File:		UBitWriter.cpp
// Author:		Kevin Bray
// Created:		08-19-07
//
// Purpose:		To manage a stream of bits.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UBitWriter.h"


//**********************************************************
// class UBitWriter
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UBitWriter::UBitWriter( unsigned char* bitArray )
: _bits( bitArray )
, _bitBuffer( 0 )
, _bitBufCount( 0 )
, _bitCount( 0 )
{

}

//----------------------------------------------------------
UBitWriter::~UBitWriter()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UBitWriter::Reset( unsigned char* bitArray )
{
	_bits = bitArray;
	_bitBuffer = 0;
	_bitBufCount = 0;
	_bitCount = 0;
}

//----------------------------------------------------------
void
UBitWriter::WriteBits( unsigned int value, unsigned int count )
{
	B_ASSERT( count <= 16 );

	// add the bits to the current byte.
	if ( _bitBufCount > 16 )
		FlushBitBuffer();

	// add the bits to the bit buffer.
	_bitBuffer |= value << _bitBufCount;
	_bitBufCount += count;
	_bitCount += count;
}

//----------------------------------------------------------
void
UBitWriter::Finish()
{
	// write the contents of the cache to the bit stream.
	int bitsRemaining = _bitBufCount;
	while ( bitsRemaining > 0 )
	{
		// write the current value to the bit stream.
		*_bits++ = ( unsigned char )_bitBuffer;

		// adjust the bit buffer and subtract from the number of bits
		// that remaining.
		_bitBuffer >>= 8;
		bitsRemaining -= 8;
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
UBitWriter::FlushBitBuffer()
{
	// make sure there are less than 16 bits in the bit buffer,
	// otherwise, we'll just write bogus data.
	B_ASSERT( _bitBufCount >= 16 );

	// write the low 16-bits to the bit stream.
	_bits[ 0 ] = ( unsigned char )_bitBuffer;
	_bits[ 1 ] = ( unsigned char )( _bitBuffer >> 8 );
	_bits += 2;

	// adjust the bit buffer.
	_bitBuffer >>= 16;

	// subtract from our bit buffer counter.
	_bitBufCount -= 16;
}
