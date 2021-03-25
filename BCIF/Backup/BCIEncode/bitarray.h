// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// std c++ includes.
#include <vector>

// forward declarations.
class Stream;

class BitArray
{
public:
	BitArray() : _bitCount( 0 ) { }
	~BitArray() { }

	// returns the size of the data in bytes.
	unsigned int	GetSizeInBytes()			{	return ( unsigned int )_bits.size();		}

	// returns the total number of bits.
	unsigned int	GetBitCount() const			{	return _bitCount;							}

	// pushes a bit.
	void			PushBit( int i );
	int				GetBit( unsigned int idx );

	// save/load.
	void			Save( Stream& output );
	void			Load( Stream& input );

private:
	typedef std::vector< unsigned int > UIntVec;
	UIntVec			_bits;
	unsigned int	_bitCount;
};
