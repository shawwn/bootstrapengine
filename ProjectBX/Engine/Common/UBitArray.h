//----------------------------------------------------------
// File:		UBitArray.h
// Author:		Kevin Bray
// Created:		09-22-06
//
// Purpose:		To manage an array of bits.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>

//**********************************************************
// class UBitArray
//**********************************************************
class UBitArray
{
public:
	UBitArray();
	UBitArray( unsigned char* data, unsigned int bitCount );
	~UBitArray();

	// resizes the bit array to the size specified.
	void					Resize( unsigned int newSize );

	// push/pop/peek bit functionality.
	void					PushBit( unsigned int value );
	void					PushBits( unsigned int value, unsigned int count );
	void					PushBits( int value, unsigned int count );
	unsigned int			PopBit();
	unsigned int			PopBits( unsigned int count );
	int						PopBitsSignExtend( unsigned int count );
	unsigned int			PeekTopBit();

	// get/set bit functionality.
	void					SetBitValue( unsigned int idx, unsigned int value );
	unsigned int			GetBitValue( unsigned int idx ) const;

	// returns a pointer to bit data.
	const unsigned char*	GetBits() const			{	return ( const unsigned char* )&_bitArray.front();		}
	unsigned int			GetBitCount() const		{	return _bitCount;										}

private:
	typedef std::vector< unsigned int > UIntVec;

	// array to store bit data.
	UIntVec					_bitArray;
	unsigned int			_bitCount;
};
