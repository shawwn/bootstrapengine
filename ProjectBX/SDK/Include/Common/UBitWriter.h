//----------------------------------------------------------
// File:		UBitWriter.h
// Author:		Kevin Bray
// Created:		08-19-07
//
// Purpose:		To efficiently write a stream of bits.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UBitWriter
//**********************************************************
class UBitWriter
{
public:
	// note that you must ensure that the buffer is large enough to
	// contain the number of bits you need to write.
	UBitWriter( unsigned char* bitArray );
	~UBitWriter();

	// resets the writer and assigns a new output stream.
	void			Reset( unsigned char* bitArray );

	// stream read/write functionality.  Note that count must be less
	// than 16.
	void			WriteBits( unsigned int value, unsigned int count );

	// flushes any buffered data.  This must be called before using
	// the data written.
	void			Finish();

	// returns the total number of bits written.
	unsigned int	GetBitCount() const		{	return _bitCount;		}

private:
	void			FlushBitBuffer();

	unsigned char*	_bits;
	unsigned int	_bitBuffer;
	unsigned int	_bitBufCount;
	unsigned int	_bitCount;
};
