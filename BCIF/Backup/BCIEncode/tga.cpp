// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// class header.
#include "tga.h"

// std c includes.
#include <assert.h>

static void ReadRLE( unsigned char* dst, unsigned int width, unsigned int height, const unsigned char* src );
static void ReadRaw( unsigned char* dst, unsigned int width, unsigned int height, const unsigned char* src );

TGAImage::TGAImage( const void* data )
{
	const unsigned char* src = ( unsigned char* )data;

	// get the header.
	STGAHeaderInfo header = *( ( STGAHeaderInfo* )src );
	_width = header.imwidth;
	_height = header.imheight;
	_bytesPerPel = header.imdepth / 8;

	bool compressed = ( header.imtype & 0x8 ) != 0;

	// this cannot be a palettized image!
	assert( ( header.imtype & 1 ) == 0 );
	assert( _bytesPerPel != 2 );
	unsigned int bytesPerPelLocal = _bytesPerPel == 3 ? 4 : _bytesPerPel;

	// allocate space for the pixels.
	_bits = new unsigned char[ _width * _height * bytesPerPelLocal ];

	// read in the data.
	if ( compressed )
		ReadRLE( src );
	else
		ReadRaw( src );
}

//----------------------------------------------------------
TGAImage::~TGAImage()
{
	delete[] _bits;
}

//----------------------------------------------------------
void TGAImage::ReadRLE( const unsigned char* src )
{
	// read in the data.
	unsigned char* dst = _bits;
	unsigned char colorBuf[ 4 ];
	unsigned int end = _width * _height;
	unsigned int i = 0;
	while ( i < end )
	{
		// get the current packet header and get the length of the packet.
		unsigned char header = *src++;
		unsigned int count = ( header & 0x7F ) + 1;

		// is the current packet compressed or raw?
		if ( header & 0x80 )
		{
			// scan in the color and replicate 'count' times.
			for ( unsigned int c = 0; c < _bytesPerPel; ++c, ++src )
				colorBuf[ c ] = *src;

			// now replicate the color 'count' times.
			while ( count-- > 0 )
			{
				for ( unsigned int c = 0; c < _bytesPerPel; ++dst )
					*dst = colorBuf[ c ];

				if ( _bytesPerPel == 3 )
					*dst++ = 0;
			}
		}
		else
		{
			// raw.
			while ( count-- > 0 )
			{
				for ( unsigned int c = 0; c < _bytesPerPel; ++dst, ++src )
					*dst = *src;

				if ( _bytesPerPel == 3 )
					*dst++ = 0;
			}
		}

		// next pixels.
		i += count;
	}

	if ( _bytesPerPel == 3 )
		_bytesPerPel = 4;
}

//----------------------------------------------------------
void TGAImage::ReadRaw( const unsigned char* src )
{
	// read in the data.
	unsigned char* dst = _bits;
	unsigned int end = _width * _height;
	for ( unsigned int i = 0; i < end; ++i )
	{
		for ( unsigned int c = 0; c < _bytesPerPel; ++c, ++src, ++dst )
			*dst = *src;

		if ( _bytesPerPel == 3 )
			*dst++ = 0;
	}

	if ( _bytesPerPel == 3 )
		_bytesPerPel = 4;
}

//----------------------------------------------------------
void TGAImage::Swizzle()
{
	unsigned char* dst = new unsigned char[ _width * _height * _bytesPerPel ];
	unsigned char* src = _bits;
	unsigned int offs = 0;
	for ( unsigned int y = 0; y < _height; ++y )
	{
		for ( unsigned int x = 0; x < _width; ++x )
		{
			offs = 0;
			for ( unsigned int b = 0; b < 16; ++b )
			{
				offs |= ( x & ( 1 << b ) ) << b;
				offs |= ( y & ( 1 << b ) ) << ( b+1 );
			}
			for ( unsigned int c = 0; c < _bytesPerPel; ++c, ++src )
				dst[ offs * _bytesPerPel + c ] = *src;
		}
	}

	delete[] _bits;
	_bits = dst;
}
