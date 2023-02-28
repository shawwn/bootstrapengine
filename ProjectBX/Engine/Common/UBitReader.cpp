//----------------------------------------------------------
// File:        UBitReader.cpp
// Author:      Kevin Bray
// Created:     08-19-07
//
// Purpose:     To manage a stream of bits.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UBitReader.h"

//**********************************************************
// class UBitReader
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UBitReader::UBitReader( const unsigned char* bitArray, unsigned int bitCount )
: _bits( bitArray )
, _bitBuffer( 0 )
, _bitBufCount( 0 )
, _bytesRemaining( ( bitCount + 7 ) / 8 )
{

}

//----------------------------------------------------------
UBitReader::~UBitReader()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UBitReader::Reset( const unsigned char* bitArray, unsigned int bitCount )
{
    _bits = bitArray;
    _bytesRemaining = ( bitCount + 7 ) / 8;
    _bitBuffer = 0;
    _bitBufCount = 0;
}

//----------------------------------------------------------
unsigned int
UBitReader::ReadBits( unsigned int count )
{
    B_ASSERT( count <= 16 );

    // make sure there are enough bits available to satisfy the
    // request.
    if ( _bitBufCount < count )
        FillBitBuffer();

    // get the bits and remove them from the bit buffer.
    unsigned int bits = _bitBuffer & ( ( 1 << count ) - 1 );
    _bitBuffer >>= count;
    _bitBufCount -= count;

    // return the bits.
    return bits;
}

//----------------------------------------------------------
unsigned int
UBitReader::PeekBits( unsigned int count )
{
    B_ASSERT( count <= 16 );

    // make sure there are enough bits available to satisfy the
    // request.
    if ( _bitBufCount < count )
        FillBitBuffer();

    // return the bits.
    return _bitBuffer & ( ( 1 << count ) - 1 );
}

//----------------------------------------------------------
void
UBitReader::SkipBits( unsigned int count )
{
    B_ASSERT( count <= 16 );

    // make sure there are enough bits available to satisfy the
    // request.
    if ( _bitBufCount < count )
        FillBitBuffer();

    // remove the bits from the bit buffer.
    _bitBuffer >>= count;
    _bitBufCount -= count;
}

//----------------------------------------------------------
bool
UBitReader::IsFinished() const
{
    return ( ( int )_bytesRemaining + ( ( _bitBufCount + 7 ) / 8 ) ) == 0;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
UBitReader::FillBitBuffer()
{
    // make sure there are less than 16 bits remaining in our bit
    // buffer.  Otherwise, we'll lose data.
    B_ASSERT( _bitBufCount < 16 );

    // subtract two from the bytes remaining.
    _bytesRemaining -= 2;

    // if there isn't enough space to satisfy the request, simply
    // repeat the last byte.
    unsigned int s = ( ~_bytesRemaining >> 31 );

    // add the bits from the first dword.
    unsigned int temp = _bits[ 0 ];
    temp |= ( _bits[ s ] << 8 );
    
    // add the bits to the bit buffer.
    _bitBuffer |= ( temp << _bitBufCount );
    _bits += 2 * s;

    // add to our bit buffer counter.
    _bitBufCount += 16;
}
