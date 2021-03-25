// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.

// file header.
#include "bitarray.h"

// project includes.
#include "stream.h"

// std c includes.
#include <assert.h>


void BitArray::PushBit( int i )
{
	// calculate the word and the bit we need.
	unsigned int wordIdx = _bitCount / 32;
	unsigned int bitIdx = _bitCount % 32;

	// expand the bit array if needed.
	if ( wordIdx >= _bits.size() )
		_bits.push_back( 0 );

	// set the bit.
	_bits[ wordIdx ] |= ( i & 1 ) << bitIdx;

	// increment the bit count.
	++_bitCount;
}

int	BitArray::GetBit( unsigned int idx )
{
	// make sure a valid index was passed in!
	assert( idx < _bitCount );

	// calculate the word and the bit we need.
	unsigned int word = idx / 32;
	unsigned int bit = idx % 32;

	// return the bit we're interested in.
	return ( _bits[ word ] >> bit ) & 1;
}

void BitArray::Save( Stream& output )
{
	// write the word count.
	unsigned int count = ( unsigned int )_bits.size();
	output.WriteValue( count );

	// write out the bit count.
	output.WriteValue( _bitCount );

	// write the words.
	for ( unsigned int i = 0; i < count; ++i )
		output.WriteValue( _bits[ i ] );
}

void BitArray::Load( Stream& input )
{
	// load the word count.
	unsigned int count = input.ReadValue< unsigned int >();

	// load the bit count.
	_bitCount = input.ReadValue< unsigned int >();

	// read the words.
	_bits.reserve( count );
	for ( unsigned int i = 0; i < count; ++i )
		_bits.push_back( input.ReadValue< unsigned int >() );
}
