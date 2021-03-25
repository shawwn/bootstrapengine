//----------------------------------------------------------
// File:		UBitReader.h
// Author:		Kevin Bray
// Created:		08-19-07
//
// Purpose:		To efficiently read a stream of bits.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UBitReader
//**********************************************************
class UBitReader
{
public:
	UBitReader( const unsigned char* bitArray, unsigned int bitCount );
	~UBitReader();

	// resets the reader.
	void					Reset( const unsigned char* bitArray, unsigned int bitCount );

	// stream read/write functionality.  Note that count must be less
	// than 16.
	unsigned int			ReadBits( unsigned int count );
	unsigned int			PeekBits( unsigned int count );
	void					SkipBits( unsigned int count );

	// returns true if there are no more bits remaining in the bit
	// stream.
	bool					IsFinished() const;

private:
	void					FillBitBuffer();

	const unsigned char*	_bits;
	unsigned int			_bitBuffer;
	unsigned int			_bitBufCount;
	unsigned int			_bytesRemaining;
};
