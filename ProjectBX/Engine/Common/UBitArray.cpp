//----------------------------------------------------------
// File:		UBitArray.cpp
// Author:		Kevin Bray
// Created:		09-22-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UBitArray.h"

// bit array expand size in DWORDs.
static const unsigned int kExpandSize = 32;

//**********************************************************
// class UBitArray
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UBitArray::UBitArray()
: _bitCount( 0 )
{
	// always start with room for 32 bits.
	_bitArray.push_back( 0 );
}

//----------------------------------------------------------
UBitArray::UBitArray( unsigned char* data, unsigned int bitCount )
{
	_bitCount = bitCount;
	_bitArray.resize( ( bitCount + 31 ) / 32, 0 );
	memcpy( &_bitArray.front(), data, bitCount >> 3 );
}

//----------------------------------------------------------
UBitArray::~UBitArray()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UBitArray::Resize( unsigned int newSize )
{
	// make sure we have enough DWORDS to fit the number of bits specified.
	_bitArray.resize( ( newSize + 31 ) >> 5, 0 );
}

//----------------------------------------------------------
void
UBitArray::PushBit( unsigned int value )
{
	// check to see if we need to add another dword.
	if ( ( _bitCount & 0x1F ) == 0 )
		_bitArray.push_back( 0 );

	// now set the bit.
	++_bitCount;
	SetBitValue( _bitCount-1, value );
}

//----------------------------------------------------------
void
UBitArray::PushBits( unsigned int value, unsigned int count )
{
	for ( unsigned int i = 0; i < count; ++i )
		PushBit( ( value >> i ) & 1 );
}

//----------------------------------------------------------
void
UBitArray::PushBits( int value, unsigned int count )
{
	PushBits( ( unsigned int )value, count );
}

//----------------------------------------------------------
unsigned int
UBitArray::PopBit()
{
	// get the top value and decrement the bit count.
	int value = PeekTopBit();
#ifdef _DEBUG
	// clear removed bits in debug.
	SetBitValue( _bitCount-1, 0 );
#endif
	--_bitCount;

	// return the value.
	return value;
}

//----------------------------------------------------------
unsigned int
UBitArray::PopBits( unsigned int count )
{
	unsigned int retVal = 0;
	while ( count-- > 0 )
		retVal |= ( PopBit() << count );
	return retVal;
}

//----------------------------------------------------------
int
UBitArray::PopBitsSignExtend( unsigned int count )
{
	// get the return value.
	unsigned int signBit = count - 1;
	unsigned int retVal = 0;
	while ( count-- > 0 )
		retVal |= ( PopBit() << count );

	// extend the sign bit.
	int set = ( retVal >> signBit ) & 1;
	retVal |= ( ( -set ) << signBit );

	// return the value.
	return ( int )retVal;
}

//----------------------------------------------------------
unsigned int
UBitArray::PeekTopBit()
{
	// return the last bit.
	return GetBitValue( _bitCount-1 );
}

//----------------------------------------------------------
void
UBitArray::SetBitValue( unsigned int idx, unsigned int value )
{
	// make sure the incoming parameters are valid.
	B_ASSERT( idx < _bitCount );
	B_ASSERT( value == 0 || value == 1 );

	// get the index into the array of DWORDs.
	unsigned int arrayIdx = idx >> 5;

	// set the bit's value.
	if ( value )
		_bitArray[ arrayIdx ] |= ( 1 << ( idx & 0x1F ) );
	else
		_bitArray[ arrayIdx ] &= ~( 1 << ( idx & 0x1F ) );
}

//----------------------------------------------------------
unsigned int
UBitArray::GetBitValue( unsigned int idx ) const
{
	B_ASSERT( idx < _bitCount );

	// return whether or not the bit is set.
	unsigned int arrayIdx = idx >> 5;
	unsigned int shift = idx & 0x1F;
	return ( _bitArray[ arrayIdx ] >> shift ) & 1;
}
