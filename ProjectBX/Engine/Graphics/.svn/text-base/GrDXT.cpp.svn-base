//----------------------------------------------------------
// File:		GrDXT.cpp
// Author:		Kevin Bray
// Created:		04-03-08
//
// Purpose:		To perform DXT compression in realtime.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// file header.
#include "GrDXT.h"

// this decreases the size of the bounding box by 1/16th of it's size.  This
// makes the min/max through color space closer together, and thus reduces the
// overall error in the image (makes error tend towards the average of a 4x4
// block).
#define COLORBOX_INSET_SHIFT		4

// forward declarations.
void Extract4x4( unsigned int* block, unsigned int* src, unsigned int srcWidth );
void Extract4x4BGR( unsigned char* block, unsigned char* src, unsigned int srcStride );
void Extract4x4XY1( unsigned char* block, unsigned char* src, unsigned int srcStride );

void MinMax( unsigned char* min, unsigned char* max, unsigned char* block );
int ColorDistSqr( unsigned char* color0, unsigned char* color1 );
unsigned short ColorTo565( const unsigned char* color );
void CompressBlockBGR( unsigned int* dest, unsigned char* block, unsigned char* min, unsigned char* max );
void CompressBlockAlpha( unsigned int* dest, unsigned char* block, unsigned char* min, unsigned char* max );

#define ALIGN_16		__declspec( align( 16 ) )

//**********************************************************
// global functions.
//**********************************************************

//----------------------------------------------------------
unsigned int CalcImageSizeDXT1( unsigned int width, unsigned int height )
{
	width += 3;
	height += 3;
	unsigned int blockCount = ( width / 4 ) * ( height / 4 );
	return blockCount * 8;
}

//----------------------------------------------------------
unsigned int CalcImageSizeDXT5( unsigned int width, unsigned int height )
{
	width += 3;
	height += 3;
	unsigned int blockCount = ( width / 4 ) * ( height / 4 );
	return blockCount * 16;
}

//----------------------------------------------------------
void CompressDXT1( unsigned int* dst, unsigned int width, unsigned int height, void* src )
{
	// reserve some space for the 4x4 block of pixels.
	ALIGN_16 unsigned char block[ 64 ];
	ALIGN_16 unsigned char min[4];
	ALIGN_16 unsigned char max[4];

	// iterate through blocks.
	unsigned int* srcScan = ( unsigned int* )src;
	unsigned int width4 = 4 * width;
	for ( unsigned int y = 0; y < height; y += 4, srcScan += width4 )
	{
		unsigned int* srcPel = srcScan;
		for ( unsigned int x = 0; x < width; x += 4, srcPel += 4 )
		{
			// move data into our local cache.
			Extract4x4( ( unsigned int* )block, srcPel, width );

			// get the color space min/max.
			MinMax( min, max, block );

			// write out the color indices.
			CompressBlockBGR( dst, block, min, max );
			dst += 2;
		}
	}
}

//----------------------------------------------------------
void CompressDXT1BGR( unsigned int* dst, unsigned int width, unsigned int height, void* src )
{
	// reserve some space for the 4x4 block of pixels.
	ALIGN_16 unsigned char block[ 64 ];
	ALIGN_16 unsigned char min[4];
	ALIGN_16 unsigned char max[4];

	// iterate through blocks.
	unsigned char* srcScan = ( unsigned char* )src;
	unsigned int pelSize4 = 4 * 3;
	unsigned int stride = 3 * width;
	unsigned int stride4 = 4 * stride;
	for ( unsigned int y = 0; y < height; y += 4, srcScan += stride4 )
	{
		unsigned char* srcPel = srcScan;
		for ( unsigned int x = 0; x < width; x += 4, srcPel += pelSize4 )
		{
			// move data into our local cache.
			Extract4x4BGR( block, srcPel, stride );

			// get the color space min/max.
			MinMax( min, max, block );

			// write out the color indices.
			CompressBlockBGR( dst, block, min, max );
			dst += 2;
		}
	}
}

//----------------------------------------------------------
void CompressDXT5( unsigned int* dst, unsigned int width, unsigned int height, void* src )
{
	// reserve some space for the 4x4 block of pixels.
	ALIGN_16 unsigned char block[ 64 ];
	ALIGN_16 unsigned char min[4];
	ALIGN_16 unsigned char max[4];

	// iterate through blocks.
	for ( unsigned int y = 0; y < height; y += 4 )
	{
		for ( unsigned int x = 0; x < width; x += 4 )
		{
			// note: by treating these as DWORDs, this could cause problems
			// with store forwarding.  Basically, the block is getting written
			// to and read from in bytes by other functions, so the different
			// addresses would simply cause store forwarding not to work for
			// 3 out of 4 read operations!
			Extract4x4( ( unsigned int* )block, ( unsigned int* )src + ( y * width ) + x, width );

			// get the color space min/max.
			MinMax( min, max, block );

			// write out the alpha indices.
			CompressBlockAlpha( dst, block, min, max );
			dst += 2;

			// write out the color indices.
			CompressBlockBGR( dst, block, min, max );
			dst += 2;
		}
	}
}

//----------------------------------------------------------
void CompressDXT5XY1( unsigned int* dst, unsigned int width, unsigned int height, void* src )
{
	// reserve some space for the 4x4 block of pixels.
	ALIGN_16 unsigned char block[ 64 ];
	ALIGN_16 unsigned char min[4];
	ALIGN_16 unsigned char max[4];

	// iterate through blocks.
	unsigned int pelSize4 = 2 * 4;
	unsigned int srcStride = 2 * width;
	unsigned int srcStride4 = 4 * srcStride;
	unsigned char* srcScan = ( unsigned char* )src;
	for ( unsigned int y = 0; y < height; y += 4, srcScan += srcStride4 )
	{
		unsigned char* srcPel = srcScan;
		for ( unsigned int x = 0; x < width; x += 4, srcPel += pelSize4 )
		{
			// note: by treating these as DWORDs, this could cause problems
			// with store forwarding.  Basically, the block is getting written
			// to and read from in bytes by other functions, so the different
			// addresses would simply cause store forwarding not to work for
			// 3 out of 4 read operations!
			Extract4x4XY1( block, srcPel, srcStride );

			// get the color space min/max.
			MinMax( min, max, block );

			// write out the alpha indices.
			CompressBlockAlpha( dst, block, min, max );
			dst += 2;

			// NOTE: we can do a faster technique for this since we know that
			// that two of the three color values are set to 1.0 (255).
			// write out the color indices.
			CompressBlockBGR( dst, block, min, max );
			dst += 2;
		}
	}
}

//----------------------------------------------------------
void FillDXT1( void* dst, unsigned int width, unsigned int height, unsigned char* bgrxColor )
{
	// calculate the color palette for each block.
	unsigned short color0 = ColorTo565( bgrxColor );
	unsigned int colorPalette = ( color0 << 16 ) | color0;

	// fill each DXT block with the palette data specified.
	unsigned int* dstBlock = ( unsigned int* )dst;
	for ( unsigned int y = 0; y < height; y += 4 )
	{
		for ( unsigned int x = 0; x < width; x += 4, dstBlock += 2 )
		{
			dstBlock[ 0 ] = colorPalette;	// color palette.
			dstBlock[ 1 ] = 0;				// color indices.
		}
	}
}

//----------------------------------------------------------
void FillDXT5( void* dst, unsigned int width, unsigned int height, unsigned char* bgraColor )
{
	// calculate the color palette for each block.
	unsigned short color0 = ColorTo565( bgraColor );
	unsigned int colorPalette = ( color0 << 16 ) | color0;

	// calculate the alpha palette for each block.  Technically, only
	// the first 2 bytes of alphaPelette are the palette.  The remaining
	// two bytes are part of the indices.
	unsigned char alpha0 = bgraColor[ 3 ];
	unsigned int alphaPalette = alpha0;
	alphaPalette <<= 8;
	alphaPalette |= alpha0;

	// fill each DXT block with the palette data specified.
	unsigned int* dstBlock = ( unsigned int* )dst;
	for ( unsigned int y = 0; y < height; y += 4 )
	{
		for ( unsigned int x = 0; x < width; x += 4, dstBlock += 4 )
		{
			dstBlock[ 0 ] = colorPalette;	// color palette.
			dstBlock[ 1 ] = 0;				// color indices.
			dstBlock[ 2 ] = alphaPalette;	// alpha palette.
			dstBlock[ 3 ] = 0;				// alpha indices.
		}
	}
}

//**********************************************************
// file local functions.
//**********************************************************

//----------------------------------------------------------
void Extract4x4( unsigned int* block, unsigned int* src, unsigned int srcWidth )
{
	for ( unsigned int y = 0; y < 4; ++y )
	{
		unsigned int* srcPel = src + y * srcWidth;
		for ( unsigned int x = 0; x < 4; ++x, ++block )
			*block = srcPel[ x ];
	}
}

//----------------------------------------------------------
void Extract4x4BGR( unsigned char* block, unsigned char* src, unsigned int srcStride )
{
	unsigned char* srcScan = src;
	for ( unsigned int y = 0; y < 4; ++y, srcScan += srcStride )
	{
		unsigned char* srcPel = srcScan;
		for ( unsigned int x = 0; x < 4; ++x, block += 4, srcPel += 3 )
		{
			block[ 0 ] = srcPel[ 0 ];
			block[ 1 ] = srcPel[ 1 ];
			block[ 2 ] = srcPel[ 2 ];
			block[ 3 ] = 1;
		}
	}
}

//----------------------------------------------------------
void Extract4x4XY1( unsigned char* block, unsigned char* src, unsigned int srcStride )
{
	unsigned char* srcScan = src;
	for ( unsigned int y = 0; y < 4; ++y, srcScan += srcStride )
	{
		unsigned char* srcPel = srcScan;
		for ( unsigned int x = 0; x < 4; ++x, block += 4 )
		{
			block[ 0 ] = srcPel[ 2 * x + 0 ];
			block[ 1 ] = 255;
			block[ 2 ] = 255;
			block[ 3 ] = srcPel[ 2 * x + 1 ];
		}
	}
}

//----------------------------------------------------------
void MinMax( unsigned char* min, unsigned char* max, unsigned char* block )
{
	// clear.
	*( unsigned int* )min = 0xFFFFFFFF;
	*( unsigned int* )max = 0;

	// calculate the min/max of the color cube.
	for ( unsigned int i = 0; i < 16; ++i )
	{
		// calculate the min/max.
		min[0] = Min( block[4*i+0], min[0] );
		max[0] = Max( block[4*i+0], max[0] );
		min[1] = Min( block[4*i+1], min[1] );
		max[1] = Max( block[4*i+1], max[1] );
		min[2] = Min( block[4*i+2], min[2] );
		max[2] = Max( block[4*i+2], max[2] );
		min[3] = Min( block[4*i+3], min[3] );
		max[3] = Max( block[4*i+3], max[3] );
	}

	// is this really even worth it?
#if 1
	// calculate the inset of the box.
	unsigned char inset[4];
	inset[0] = ( max[0] - min[0] ) >> COLORBOX_INSET_SHIFT;
	inset[1] = ( max[1] - min[1] ) >> COLORBOX_INSET_SHIFT;
	inset[2] = ( max[2] - min[2] ) >> COLORBOX_INSET_SHIFT;
	inset[3] = ( max[3] - min[3] ) >> COLORBOX_INSET_SHIFT;

	// CAN dstMin + inset be larger than 255?
	// adjust the minimum.
	min[0] = min[0] + inset[0];
	min[1] = min[1] + inset[1];
	min[2] = min[2] + inset[2];
	min[3] = min[3] + inset[3];

	// CAN dstMax - inset be negative?
	// adjust the maximum.
	max[0] = max[0] - inset[0];
	max[1] = max[1] - inset[1];
	max[2] = max[2] - inset[2];
	max[3] = max[3] - inset[3];
#endif

	// adjust the min to ensure that it isn't equal to max.
	min[0] = ( min[0] > 0 ) ? ( min[0] - 1 ) : min[0];
	min[1] = ( min[1] > 0 ) ? ( min[1] - 1 ) : min[1];
	min[2] = ( min[2] > 0 ) ? ( min[2] - 1 ) : min[2];
	min[3] = ( min[3] > 0 ) ? ( min[3] - 1 ) : min[3];

	// adjust the max to ensure that it isn't equal to min.
	max[0] = ( max[0] < 255 ) ? ( max[0] + 1 ) : max[0];
	max[1] = ( max[1] < 255 ) ? ( max[1] + 1 ) : max[1];
	max[2] = ( max[2] < 255 ) ? ( max[2] + 1 ) : max[2];
	max[3] = ( max[3] < 255 ) ? ( max[3] + 1 ) : max[3];
}

//----------------------------------------------------------
int ColorDistSqr( unsigned char* color0, unsigned char* color1 )
{
	return  ( color0[0] - color1[0] ) * ( color0[0] - color1[0] ) +
			( color0[1] - color1[1] ) * ( color0[1] - color1[1] ) +
			( color0[2] - color1[2] ) * ( color0[2] - color1[2] );
}

//----------------------------------------------------------
unsigned short ColorTo565( const unsigned char* color )
{
	unsigned short color565 = color[0] >> 3;
	color565 |= ( unsigned short )( color[1] >> 2 ) << 5;
	color565 |= ( unsigned short )( color[2] >> 3 ) << 11;
	return color565;
}

//----------------------------------------------------------
void CompressBlockBGR( unsigned int* dest, unsigned char* block, unsigned char* min, unsigned char* max )
{
	// calculate our color table.
	unsigned char colors[4][4];

	// RGB0
	colors[0][0] = max[0];
	colors[0][1] = max[1];
	colors[0][2] = max[2];

	// RGB1
	colors[1][0] = min[0];
	colors[1][1] = min[1];
	colors[1][2] = min[2];

	// (2*RGB0+RGB1)/3
	colors[2][0] = ( 2 * ( int )max[0] + ( int )min[0] ) / 3;
	colors[2][1] = ( 2 * ( int )max[1] + ( int )min[1] ) / 3;
	colors[2][2] = ( 2 * ( int )max[2] + ( int )min[2] ) / 3;

	// (RGB0+2*RGB1)/3
	colors[3][0] = ( ( int )max[0] + 2 * ( int )min[0] ) / 3;
	colors[3][1] = ( ( int )max[1] + 2 * ( int )min[1] ) / 3;
	colors[3][2] = ( ( int )max[2] + 2 * ( int )min[2] ) / 3;

	// now perform our fit.
	unsigned int bits = 0;
	unsigned int shift = 0;
	for ( unsigned int i = 0; i < 16; ++i, shift += 2, block += 4 )
	{
		// calculate the best index.
		int best = 0;
		int dist = 0x7FFFFFFF;
		for ( unsigned int j = 0; j < 4; ++j )
		{
			int cur = ColorDistSqr( colors[j], block );
			if ( cur < dist )
			{
				dist = cur;
				best = j;
			}
		}

		// write out the best index.
		bits |= best << shift;
	}

	// write out our max color/min color.
	dest[0] =  ( unsigned int )ColorTo565( colors[0] );
	dest[0] |= ( unsigned int )ColorTo565( colors[1] ) << 16;

	// write the indices.
	dest[1] = bits;
}

//----------------------------------------------------------
void CompressBlockAlpha( unsigned int* dest, unsigned char* block, unsigned char* min, unsigned char* max )
{
	// calculate our alpha table.
	unsigned char values[8];
	values[0] = max[3];
	values[1] = min[3];
	values[2] = ( 6*values[0] + 1*values[1] ) / 7;
	values[3] = ( 5*values[0] + 2*values[1] ) / 7;
	values[4] = ( 4*values[0] + 3*values[1] ) / 7;
	values[5] = ( 3*values[0] + 4*values[1] ) / 7;
	values[6] = ( 2*values[0] + 5*values[1] ) / 7;
	values[7] = ( 1*values[0] + 6*values[1] ) / 7;

	// calculate the indices.
	unsigned char indices[16];
	for ( unsigned int i = 0; i < 16; ++i )
	{
		int best = 0;
		int dist = 0xFFFF;
		for ( unsigned int j = 0; j < 8; ++j )
		{
			int cur = abs( block[i*4+3] - values[j] );
			if ( cur < dist )
			{
				dist = cur;
				best = j;
			}
		}
		indices[ i ] = best;
	}

	// write the min/max values down before emitting the indices.
	dest[0]  = values[0];
	dest[0] |= values[1] << 8;

	// now store the indices.
	dest[0] |= indices[ 0] << 16;
	dest[0] |= indices[ 1] << 19;
	dest[0] |= indices[ 2] << 22;
	dest[0] |= indices[ 3] << 25;
	dest[0] |= indices[ 4] << 28;
	dest[0] |= indices[ 5] << 31;
	dest[1]  = indices[ 5] >> 1;
	dest[1] |= indices[ 6] << 2;
	dest[1] |= indices[ 7] << 5;
	dest[1] |= indices[ 8] << 8;
	dest[1] |= indices[ 9] << 11;
	dest[1] |= indices[10] << 14;
	dest[1] |= indices[11] << 17;
	dest[1] |= indices[12] << 20;
	dest[1] |= indices[13] << 23;
	dest[1] |= indices[14] << 26;
	dest[1] |= indices[15] << 29;
}
