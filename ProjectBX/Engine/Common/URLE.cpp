//----------------------------------------------------------
// File:		URLE.cpp
// Author:		Kevin Bray
// Created:		11-17-2008
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "URLE.h"

//**********************************************************
// class URLE
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
URLE::URLE()
{

}

//----------------------------------------------------------
URLE::~URLE()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
URLE::CalcMaxCompSize( unsigned int srcSize )
{
	// return the source size + 33% since that's the maximum
	// amount it can expand.
	return srcSize + ( srcSize + 2 ) / 3;
}

//----------------------------------------------------------
unsigned int
URLE::Compress( unsigned char* dest, const unsigned char* source, unsigned int srcSize )
{
	// useful constants.
	const unsigned char* srcEnd = source + srcSize;
	const unsigned char* src = source;
	unsigned char* dst = dest;

	// RLE compress the data passed in.
	unsigned int end = 0;
	unsigned int runLen = 0;
	while ( src != srcEnd )
	{
		// determine if we have a run.
		end = Min( 128U, ( unsigned int )( srcEnd - src ) );
		runLen = 1;
		while ( runLen < end && src[ 0 ] == src[ runLen ] )
			++runLen;

		// if we have a run, store it.
		if ( runLen > 1 || ( src + 1 ) == srcEnd )
		{
			// store the run.
			*dst++ = 0x80 | ( runLen - 1 );
			*dst++ = src[ 0 ];
			src += runLen;
		}

		// determine if we have raw bytes.
		end = Min( 128U, ( unsigned int )( srcEnd - src ) );
		runLen = 1;
		while ( runLen < end && src[ runLen - 1 ] != src[ runLen ] )
			++runLen;

		// if we have a sequence of raw bytes, store it.
		if ( runLen > 1 )
		{
			// store the raw sequence.
			*dst++ = ( ( --runLen ) - 1 );
			for ( unsigned int i = 0; i < runLen; ++i )
				*dst++ = src[ i ];
			src += runLen;
		}
	}

	// return the number of bytes written.
	return ( unsigned int )( dst - dest );
}

//----------------------------------------------------------
void
URLE::Decompress( unsigned char* dst, unsigned int dstSize, const unsigned char* src )
{
	// iterate through all data and decompress.
	while ( ( int )dstSize > 0 )
	{
		// determine if we need to write a run of repeat values or
		// write a run of unique values.
		unsigned char cmd = *src++;
		unsigned char runLen = ( cmd & 0x7F ) + 1;
		dstSize -= runLen;
		if ( cmd & 0x80 )
		{
			while ( runLen-- > 0 )
				*dst++ = *src;
			++src;
		}
		else
		{
			while ( runLen-- > 0 )
				*dst++ = *src++;
		}
	}
}
