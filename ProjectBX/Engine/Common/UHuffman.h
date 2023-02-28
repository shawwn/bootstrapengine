//----------------------------------------------------------
// File:		UHuffman.h
// Author:		Kevin Bray
// Created:		10-03-08
//
// Purpose:		To perform huffman encoding and fast decoding.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "UBitWriter.h"
#include "UBitReader.h"

//**********************************************************
// class UHuffman
//**********************************************************
class UHuffman
{
public:
	UHuffman();
	~UHuffman();

	// build bit codes and compresses the given sequence of bytes.
	// Result should be the size of the data passed in.  The total
	// number of bits in the sequence is returned.
	unsigned int	Compress( unsigned char* result, unsigned char* codeLengths, const void* srcData,
							  unsigned int srcSize );

	// uses the given code lengths to build a dictionary and
	// decompress the data passed in.  The return value is the total
	// number of bytes in the result.  Note that you have to know the
	// size of the result buffer ahead of time.  Note that this
	// function trashes the current stream state.
	unsigned int	Decompress( unsigned char* result, const unsigned char* codeLengths,
								const void* compressedData, unsigned int srcBits );

	//----------------------------------------------------------
	// compression methods

	// This method must be called before calling SetCompStream.  This
	// same data must be passed to the decompression methods before
	// decompressing an encoded bit stream.
	void			CalcCodeLengths( unsigned char* codeLengths, const unsigned char* srcData,
									 unsigned int srcSize );

	// This function takes a set of 256 scores (one for each of the
	// 256 characters) and generates code lengths for them.  Note
	// that any entries that have a score of 0 must not be passed to
	// any of the compression functions.  This same set of
	// codeLengths must be passed to the decompression routines.
	void			CalcCodeLengths( unsigned char* codeLengths, const unsigned int* scores );

	// sets the destination buffer for bit stream compression.
	void			SetCompStream( unsigned char* compressedData, const unsigned char* codeLengths );

	// sets the destination buffer for bit stream compression.  Note
	// that the code lengths must have been set in a previous call to
	// SetCompStream or SetCodeLengths.
	void			SetCompCodeLengths( const unsigned char* codeLengths );
	void			SetCompStreamFast( unsigned char* compressedData );

	// returns the length of compressed bitcode (in bits) for the
	// value specified.  This function is useful for determining the
	// total size of compressed data before actually compressing the
	// data.
	unsigned int	GetValueLen( unsigned char value ) const;

	// writes a value to the stream.
	void			WriteValue( unsigned char value );

	// terminates the stream and returns the total number of bits
	// written.
	unsigned int	FinishWriting();

	//----------------------------------------------------------
	// decompression methods
	// sets the current source data for bit stream decompression
	// routines.
	void			SetDecompStream( const unsigned char* codeLengths, const unsigned char* compressedData,
									 unsigned int srcBitCount );

	// builds internal tables using the code lengths supplied.  This
	// allows for calls to SetDecompStreamFast.
	void			SetDecompCodeLengths( const unsigned char* codeLengths );

	// builds the tables for use with SetDecompFastTables.  If you
	// have multiple streams that use the same decompression tree,
	// then it's advantageous to call this function once, and then
	// use the tables with the SetDecompStreamFast() function over
	// several function calls.  This can save a lot of redundant
	// calculations.  Note that SetDecompFastTables() does not have
	// to be called after calling this function unless
	// SetDecompStream() is called, or this function is called with
	// a different set of codeLengths.
	void			BuildDecompFastTables( unsigned char* symTable, unsigned char* lenTable, unsigned char* rareSymTable,
										   unsigned short* maxCodeTable, unsigned char* rareCodeOffsetTable,
										   const unsigned char* codeLengths );

	// sets the tables used for decompression.
	void			SetDecompFastTables( const unsigned char* symTable, const unsigned char* lenTable,
										 const unsigned char* rareSymTable, const unsigned short* maxCodeTable,
										 const unsigned char* rareCodeOffsetTable );

	// sets the stream for decompression and uses the supplied tables
	// to decompress the data.  You must call BuildDecompTables,
	// SetDecompFastTables, or SetDecompStream before calling this
	// function.
	void			SetDecompStreamFast( const unsigned char* compressedData, unsigned int srcBitCount );

	// decompresses a byte off of the current set bit stream.
	unsigned char	ReadValue();

	// returns true while there is more data to be read.
	bool			IsReadFinished();

private:
	struct SDicEntry
	{
		unsigned char value;
		unsigned char codeLen;
		unsigned short code;
	};

	// internal functions.
	static bool		DicSort( const SDicEntry& lhs, const SDicEntry& rhs );
	void			BuildDic( SDicEntry* dic, const unsigned char* codeLengths );
	void			BuildCompTables( const unsigned char* codeLengths );
	void			BuildDecompTables( const unsigned char* codeLengths );
	unsigned short	ReverseBits( unsigned short src );

	// stream data.
	UBitWriter		_compDst;
	UBitReader		_decompSrc;

	// compression tables.
	unsigned short	_cmpCodes[ 256 ];
	unsigned char	_cmpLenTable[ 256 ];

	// decompression tables.
	unsigned char	_symTable[ 256 ];
	unsigned char	_lenTable[ 256 ];
	unsigned char	_rareSymTable[ 512 ];
	unsigned short	_maxCodeTable[ 17 ];
	unsigned short	_rareCodeOffsetTable[ 17 ];
};

// ye olde code.
#if 0
	// sets the compressed data.
	void			Encode( const void* srcData, unsigned int srcSize );
	void			SetEncodedBits( const void* src, unsigned int bitCount );
	const void*		GetEncodedBits() const					{	return _bitStream;			}
	unsigned int	GetEncodedBitCount() const				{	return _bitCount;			}

	// decodes the bit stream contained in this object.
	unsigned int	Decode( void* dst ) const;

	// faster encoding/decoding algorithm.
	void			EncodeFast( const void* srcData, unsigned int srcSize );
	unsigned int	DecodeFast( void* dst );

private:
	// compressed data.
	void*			_bitStream;
	unsigned int	_bitCount;
	bool			_ownBits;
};
#endif