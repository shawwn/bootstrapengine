//----------------------------------------------------------
// File:		GrSoftwareRTBuffer.h
// Author:		Kevin Bray
// Created:		08-14-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSoftwareRTBuffer.h"

#define RENDER_TILE_SIZE		8

//**********************************************************
// class GrSoftwareRTBuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSoftwareRTBuffer::GrSoftwareRTBuffer( unsigned int width, unsigned int height,
										unsigned int bytesPerPel )
: _width( width )
, _height( height )
, _bytesPerPel( bytesPerPel )
, _stride( 0 )
, _blockStride( 0 )
, _buffer( 0 )
, _block( 0 )
{
	// calculate the stride of the target.
	_stride = ( ( width + 7 ) & 0xFFFFFFF8 ) * bytesPerPel;
	_blockStride = 8 * bytesPerPel;

	// allocate needed buffers.
	unsigned int allocHeight = ( ( height + 7 ) & 0xFFFFFFF8 );
    _buffer = ( unsigned char* )AlignedAlloc( 16, allocHeight * _stride );
    _block = ( unsigned char* )AlignedAlloc( 16, RENDER_TILE_SIZE * RENDER_TILE_SIZE * bytesPerPel );
}

//----------------------------------------------------------
GrSoftwareRTBuffer::~GrSoftwareRTBuffer()
{
	// free buffers.
	AlignedFree( _buffer );
	AlignedFree( _block );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned char*
GrSoftwareRTBuffer::BeginTile8x8( unsigned int x, unsigned int y )
{
	// fill the temporary buffer with the contents of the target.
	unsigned char* srcScan = _buffer + y * _stride + x * _bytesPerPel;
	unsigned char* dstScan = _block;

	// copy each line.
	for ( unsigned int y = 0; y < RENDER_TILE_SIZE; ++y )
	{
		MemCopy( dstScan, srcScan, _blockStride );
		dstScan += _blockStride;
		srcScan += _stride;
	}

	// return the temporary buffer.
	return _block;
}

//----------------------------------------------------------
unsigned char*
GrSoftwareRTBuffer::BeginTile8x8WriteOnly( unsigned int x, unsigned int y )
{
	return _block;
}

//----------------------------------------------------------
void
GrSoftwareRTBuffer::EndTile8x8( unsigned int x, unsigned int y )
{
	// flush the temporary buffer's contents to memory.
	unsigned char* srcScan = _block;
	unsigned char* dstScan = _buffer + y * _stride + x * _bytesPerPel;

	// copy each line.
	for ( unsigned int tileY = 0; tileY < RENDER_TILE_SIZE; ++tileY )
	{
		MemCopy( dstScan, srcScan, _blockStride );
		dstScan += _stride;
		srcScan += _blockStride;
	}
}

//----------------------------------------------------------
void
GrSoftwareRTBuffer::Clear( void* pelClearData )
{
	if ( _bytesPerPel == 2 || _bytesPerPel == 4 || _bytesPerPel == 8 || _bytesPerPel == 16 )
	{
		// buffer containing 16 bytes worth of clear data.
		static DECLARE_ALIGNED( unsigned char, clearData[ 16 ], 16 );
	
		// fill the buffer with 16 bytes of clear data.
		unsigned char* src = ( unsigned char* )pelClearData;
		unsigned int srcMask = _bytesPerPel - 1;
		for ( unsigned int c = 0; c < 16; ++c )
			clearData[ c ] = src[ c & srcMask ];

		// given the current pixel size, the _stride should be a multiple
		// of at least 16.
		B_ASSERT( ( _stride & 0xF ) == 0 );

		// now write 16 bytes of clear data to every line.
		__m128 clearValue = _mm_load_ps( ( float* )clearData );
		unsigned char* dst = _buffer;
		unsigned int writeCount = _stride >> 4;
		for ( unsigned int y = 0; y < _height; ++y )
		{
			for ( unsigned int i = 0; i < writeCount; ++i, dst += 16 )
				_mm_store_ps( ( float* )dst, clearValue );
		}
	}
	else
	{
		unsigned char* src = ( unsigned char* )pelClearData;
		unsigned char* dstScan = _buffer;
		for ( unsigned int y = 0; y < _height; ++y, dstScan += _stride )
		{
			unsigned char* dst = dstScan;
			for ( unsigned int x = 0; x < _width; ++x, dst += _bytesPerPel )
			{
				for ( unsigned int c = 0; c < _bytesPerPel; ++c )
					dst[ c ] = src[ c ];
			}
		}
	}
}
