//----------------------------------------------------------
// File:		GrUTCompress.cpp
// Author:		Kevin Bray
// Created:		11-17-08
//
// Purpose:		To provide compression services for the ubertexture
//				worker thread.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUTCompress.h"

// project includes.
#include "UHuffman.h"

#ifdef _DEBUG
bool gLibInit = false;
#define UT_CMP_INIT()			gLibInit = true;
#define UT_CMP_CHECK_INIT()		B_ASSERT( gLibInit );
#else
#define UT_CMP_INIT()
#define UT_CMP_CHECK_INIT()
#endif

// forward declarations.
void ExtractBlockBGRA( unsigned char* bgra, const unsigned char* bgraSource, unsigned int srcWidth,
					   unsigned int srcX, unsigned int srcY );
void ExtractBlockA( short* v, const unsigned char* vSource, unsigned int bytesPerPel,
				    unsigned int srcWidth, unsigned int srcX, unsigned int srcY );
void InsertBlockBGRA( unsigned char* bgraDest, unsigned int dstWidth, unsigned int dstX,
					  unsigned int dstY, unsigned char* bgra );
void InsertBlockA( unsigned char* vDest, unsigned int bytesPerPel, unsigned int dstWidth,
				   unsigned int dstX, unsigned int dstY, short* v );
void EncodeBlock( UHuffman* encoder, const short* src, const unsigned short* quant,
				  unsigned char chop );
void DecodeBlock( short* dest, UHuffman* decoder, const unsigned short* quant );
void UnpackYCoCg( unsigned char* bgra, const short* Y, const short* Co, const short* Cg );
void PackYCoCg( short* Y, short* Co, short* Cg, const unsigned char* bgra );
void ForwardDCT( short* dest, const short* coeff, const unsigned short* quant );
void InverseDCT( short* dest, const short* coeff, const unsigned short* quant );
void BuildDXTTables();
void CompressImageDXT1( const unsigned char* argb, unsigned char* dxt1, int width, int height );
void CompressImageDXT5(const unsigned char* argb, BYTE* dxt5, int width, int height);


//==========================================================
// DCT compression tables.
//==========================================================

//----------------------------------------------------------
// quantization table for Y.
static DECLARE_ALIGNED_16( unsigned short, kQuantY[ 64 ] ) =
{
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	  5,  11,  10,  16,  28,  68,  80,  80,
	 12,  12,  14,  18,  32,  73,  90, 110,
	 14,  13,  16,  30,  51,  87, 138, 112,
	 14,  17,  28,  36,  80, 174, 160, 124,
	 18,  28,  47,  88, 136, 255, 206, 154,
	 30,  42,  86, 128, 162, 208, 226, 184,
	 60, 128, 156, 174, 206, 241, 240, 202,
	144, 184, 190, 196, 224, 200, 206, 198,
};

//----------------------------------------------------------
// quantization table for Co.
static DECLARE_ALIGNED_16( unsigned short, kQuantCo[ 64 ] ) =
{
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	 18,  20,  24,  32,  34,  38,  41,  50,
	 20,  30,  32,  40,  42,  50,  52,  58,
	 24,  32,  40,  45,  48,  52,  56,  60,
	 32,  40,  45,  56,  64,  70,  88, 102,
	 34,  42,  48,  64,  78,  92, 110, 120,
	 38,  50,  52,  70,  92, 120, 128, 160,
	 41,  52,  56,  88, 110, 128, 220, 190,
	 50,  58,  60, 102, 120, 160, 190, 190,
};

//----------------------------------------------------------
// quantization table for Cg.
static DECLARE_ALIGNED_16( unsigned short, kQuantCg[ 64 ] ) =
{
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
//	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	 18,  20,  24,  32,  34,  38,  41,  50,
	 20,  30,  32,  40,  42,  50,  52,  58,
	 24,  32,  40,  45,  48,  52,  56,  60,
	 32,  40,  45,  56,  64,  70,  88, 102,
	 34,  42,  48,  64,  78,  92, 110, 120,
	 38,  50,  52,  70,  92, 120, 128, 160,
	 41,  52,  56,  88, 110, 128, 220, 190,
	 50,  58,  60, 102, 120, 160, 190, 190,
};

//----------------------------------------------------------
// quantization table for XY tangent space normal maps.
static DECLARE_ALIGNED_16( unsigned short, kQuantXY[ 64 ] ) =
{
	  8,   9,  11,  13,  16,  21,  24,  27,
	  9,   9,  12,  15,  18,  22,  24,  29,
	 11,  12,  15,  19,  22,  25,  28,  32,
	 13,  15,  19,  24,  27,  30,  34,  37,
	 16,  18,  22,  27,  34,  39,  41,  46,
	 21,  22,  25,  30,  39,  48,  59,  74,
	 24,  24,  28,  34,  41,  59,  81, 109,
	 27,  29,  32,  37,  46,  74, 109, 160,
};

//----------------------------------------------------------
// matrix block swizzle pattern 0.
static DECLARE_ALIGNED_16( const unsigned int, kDCTBlockSwizzle0[ 64 ] ) =
{
	0,
	1,	8,
	16,	9,	2,
	3,	10,	17,	24,
	32,	25,	18,	11,	4,
	5,	12,	19,	26,	33,	40,
	48,	41,	34,	27,	20,	13,	6,
	7,	14,	21,	28,	35,	42,	49,	56,
	57,	50,	43,	36,	29,	22,	15,
	23,	30,	37,	44,	51,	58,
	59,	52,	45,	38,	31,
	39,	46,	53,	60,
	61,	54,	47,
	55,	62,
	63,
};

#define ONE_OVER_SQRT2		( 0.5f / Sqrt( 2.0f ) )
#define DCT_COS( i )		( 0.5f * Cos( i * PI / 16.0f ) )

//----------------------------------------------------------
// the DCT matrix.
static DECLARE_ALIGNED_16( const float, kDCTMatrix[ 64 ] ) =
{
	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,	ONE_OVER_SQRT2,
	DCT_COS( 1 ),	DCT_COS( 3 ),	DCT_COS( 5 ),	DCT_COS( 7 ),	DCT_COS( 9 ),	DCT_COS( 11 ),	DCT_COS( 13 ),	DCT_COS( 15 ),
	DCT_COS( 2 ),	DCT_COS( 6 ),	DCT_COS( 10 ),	DCT_COS( 14 ),	DCT_COS( 18 ),	DCT_COS( 22 ),	DCT_COS( 26 ),	DCT_COS( 30 ),
	DCT_COS( 3 ),	DCT_COS( 9 ),	DCT_COS( 15 ),	DCT_COS( 21 ),	DCT_COS( 27 ),	DCT_COS( 33 ),	DCT_COS( 39 ),	DCT_COS( 45 ),
	DCT_COS( 4 ),	DCT_COS( 12 ),	DCT_COS( 20 ),	DCT_COS( 28 ),	DCT_COS( 36 ),	DCT_COS( 44 ),	DCT_COS( 52 ),	DCT_COS( 60 ),
	DCT_COS( 5 ),	DCT_COS( 15 ),	DCT_COS( 25 ),	DCT_COS( 35 ),	DCT_COS( 45 ),	DCT_COS( 55 ),	DCT_COS( 65 ),	DCT_COS( 75 ),
	DCT_COS( 6 ),	DCT_COS( 18 ),	DCT_COS( 30 ),	DCT_COS( 42 ),	DCT_COS( 54 ),	DCT_COS( 66 ),	DCT_COS( 78 ),	DCT_COS( 90 ),
	DCT_COS( 7 ),	DCT_COS( 21 ),	DCT_COS( 35 ),	DCT_COS( 49 ),	DCT_COS( 63 ),	DCT_COS( 77 ),	DCT_COS( 91 ),	DCT_COS( 105 ),
};

//----------------------------------------------------------
// the transposed DCT matrix.
static DECLARE_ALIGNED_16( const float, kDCTMatrixT[ 64 ] ) =
{
	ONE_OVER_SQRT2,	DCT_COS( 1 ),	DCT_COS( 2 ),	DCT_COS( 3 ),	DCT_COS( 4 ),	DCT_COS( 5 ),	DCT_COS( 6 ),	DCT_COS( 7 ),	
	ONE_OVER_SQRT2,	DCT_COS( 3 ),	DCT_COS( 6 ),	DCT_COS( 9 ),	DCT_COS( 12 ),	DCT_COS( 15 ),	DCT_COS( 18 ),	DCT_COS( 21 ),
	ONE_OVER_SQRT2,	DCT_COS( 5 ),	DCT_COS( 10 ),	DCT_COS( 15 ),	DCT_COS( 20 ),	DCT_COS( 25 ),	DCT_COS( 30 ),	DCT_COS( 35 ),
	ONE_OVER_SQRT2,	DCT_COS( 7 ),	DCT_COS( 14 ),	DCT_COS( 21 ),	DCT_COS( 28 ),	DCT_COS( 35 ),	DCT_COS( 42 ),	DCT_COS( 49 ),
	ONE_OVER_SQRT2,	DCT_COS( 9 ),	DCT_COS( 18 ),	DCT_COS( 27 ),	DCT_COS( 36 ),	DCT_COS( 45 ),	DCT_COS( 54 ),	DCT_COS( 63 ),
	ONE_OVER_SQRT2,	DCT_COS( 11 ),	DCT_COS( 22 ),	DCT_COS( 33 ),	DCT_COS( 44 ),	DCT_COS( 55 ),	DCT_COS( 66 ),	DCT_COS( 77 ),
	ONE_OVER_SQRT2,	DCT_COS( 13 ),	DCT_COS( 26 ),	DCT_COS( 39 ),	DCT_COS( 52 ),	DCT_COS( 65 ),	DCT_COS( 78 ),	DCT_COS( 91 ),
	ONE_OVER_SQRT2,	DCT_COS( 15 ),	DCT_COS( 30 ),	DCT_COS( 45 ),	DCT_COS( 60 ),	DCT_COS( 75 ),	DCT_COS( 90 ),	DCT_COS( 105 ),
};

// entropy encoding score macro.
#define SV( x )			( ( ( 256 - ( ( x ) & 0x7F ) ) * ( 256 - ( ( x ) & 0x7F ) ) ) >> 6 )

//----------------------------------------------------------
// global tables.
static DECLARE_ALIGNED_16( short, gDCTCoeff[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeCo[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeCg[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeY0[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeY1[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeY2[ 64 ] );
static DECLARE_ALIGNED_16( short, gEncodeY3[ 64 ] );
static DECLARE_ALIGNED_16( unsigned char, gEncodeBGRA[ 256 ] );
static DECLARE_ALIGNED_16( short, gDecodeCo[ 64 ] );
static DECLARE_ALIGNED_16( short, gDecodeCg[ 64 ] );
static DECLARE_ALIGNED_16( short, gDecodeY0[ 64 ] );
static DECLARE_ALIGNED_16( short, gDecodeY1[ 64 ] );
static DECLARE_ALIGNED_16( short, gDecodeY2[ 64 ] );
static DECLARE_ALIGNED_16( short, gDecodeY3[ 64 ] );
static DECLARE_ALIGNED_16( unsigned char, gDecodeBGRA[ 256 ] );


//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
void GrUTInitCompressor()
{
	BuildDXTTables();
	UT_CMP_INIT();
}

//----------------------------------------------------------
void GrUTCompressBGRX( UHuffman* encoder, const unsigned char* bgrx, unsigned char chop )
{
	UT_CMP_CHECK_INIT();

	// iterate over each 8x8 block of pixels.
	for ( unsigned int y = 0; y < 8; y += 2 )
	{
		for ( unsigned int x = 0; x < 8; x += 2 )
		{
			// extract 4 8x8 blocks.
			ExtractBlockBGRA( gEncodeBGRA, bgrx, 64, x + 0, y + 0 );
			PackYCoCg( gEncodeY0, gEncodeCo +  0, gEncodeCg +  0, gEncodeBGRA );
			ExtractBlockBGRA( gEncodeBGRA, bgrx, 64, x + 1, y + 0 );
			PackYCoCg( gEncodeY1, gEncodeCo +  4, gEncodeCg +  4, gEncodeBGRA );
			ExtractBlockBGRA( gEncodeBGRA, bgrx, 64, x + 0, y + 1 );
			PackYCoCg( gEncodeY2, gEncodeCo + 32, gEncodeCg + 32, gEncodeBGRA );
			ExtractBlockBGRA( gEncodeBGRA, bgrx, 64, x + 1, y + 1 );
			PackYCoCg( gEncodeY3, gEncodeCo + 36, gEncodeCg + 36, gEncodeBGRA );

			// store the subsampled Co and Cg values.
			EncodeBlock( encoder, gEncodeCo, kQuantCo, chop );
			EncodeBlock( encoder, gEncodeCg, kQuantCg, chop );

			// store Y values.
			EncodeBlock( encoder, gEncodeY0, kQuantY, chop );
			EncodeBlock( encoder, gEncodeY1, kQuantY, chop );
			EncodeBlock( encoder, gEncodeY2, kQuantY, chop );
			EncodeBlock( encoder, gEncodeY3, kQuantY, chop );
		}
	}
}

//----------------------------------------------------------
void GrUTCompressA( UHuffman* encoder, const unsigned char* a, unsigned int bytesPerPel, unsigned char chop )
{
	UT_CMP_CHECK_INIT();

	// iterate over each 8x8 block of pixels.
	for ( unsigned int y = 0; y < 8; ++y )
	{
		for ( unsigned int x = 0; x < 8; ++x )
		{
			// extract the block and adjust so that we have -128..127 values.
			ExtractBlockA( gEncodeY0, a, bytesPerPel, 64, x, y );

			// encode the X and Y values.
			EncodeBlock( encoder, gEncodeY0, kQuantY, chop );
		}
	}
}

//----------------------------------------------------------
void GrUTDecompressBGRX( unsigned char* bgrx, UHuffman* decoder )
{
	UT_CMP_CHECK_INIT();

	// iterate over 8x8 blocks of pixels.
	for ( unsigned int y = 0; y < 8; y += 2 )
	{
		for ( unsigned int x = 0; x < 8; x += 2 )
		{
			// decode subsampled Co and Cg values.
			DecodeBlock( gDecodeCo, decoder, kQuantCo );
			DecodeBlock( gDecodeCg, decoder, kQuantCg );

			// decode Y values.
			DecodeBlock( gDecodeY0, decoder, kQuantY );
			DecodeBlock( gDecodeY1, decoder, kQuantY );
			DecodeBlock( gDecodeY2, decoder, kQuantY );
			DecodeBlock( gDecodeY3, decoder, kQuantY );

			// unpack colors and insert into the destination block.
			UnpackYCoCg( gDecodeBGRA, gDecodeY0, gDecodeCo +  0, gDecodeCg +  0 );
			InsertBlockBGRA( bgrx, 64, x + 0, y + 0, gDecodeBGRA );
			UnpackYCoCg( gDecodeBGRA, gDecodeY1, gDecodeCo +  4, gDecodeCg +  4 );
			InsertBlockBGRA( bgrx, 64, x + 1, y + 0, gDecodeBGRA );
			UnpackYCoCg( gDecodeBGRA, gDecodeY2, gDecodeCo + 32, gDecodeCg + 32 );
			InsertBlockBGRA( bgrx, 64, x + 0, y + 1, gDecodeBGRA );
			UnpackYCoCg( gDecodeBGRA, gDecodeY3, gDecodeCo + 36, gDecodeCg + 36 );
			InsertBlockBGRA( bgrx, 64, x + 1, y + 1, gDecodeBGRA );
		}
	}
}

//----------------------------------------------------------
void GrUTDecompressA( unsigned char* a, unsigned int bytesPerPel, UHuffman* decoder )
{
	UT_CMP_CHECK_INIT();

	// iterate over 8x8 blocks of pixels.
	for ( unsigned int y = 0; y < 8; ++y )
	{
		for ( unsigned int x = 0; x < 8; ++x )
		{
			// decode X and Y values.
			DecodeBlock( gDecodeY0, decoder, kQuantY );

			// insert the block into the outgoing stream.
			InsertBlockA( a, bytesPerPel, 64, x, y, gDecodeY0 );
		}
	}
}

//----------------------------------------------------------
void GrUTCompressDXT5( unsigned char* destSurface, unsigned int destStride, unsigned char* bgra )
{
	UT_CMP_CHECK_INIT();

	B_ASSERT( false );
}

//----------------------------------------------------------
void GrUTCompressDXT5XY( unsigned char* destSurface, unsigned int destStride, unsigned char* xy )
{
	UT_CMP_CHECK_INIT();

	B_ASSERT( false );
}


//**********************************************************
// File local functions
//**********************************************************

//----------------------------------------------------------
void ExtractBlockBGRA( unsigned char* bgra, const unsigned char* bgraSource, unsigned int srcWidth,
					   unsigned int srcX, unsigned int srcY )
{
	unsigned int stride = 4 * srcWidth;
	const unsigned char* scan = bgraSource + srcY * stride + srcX * 4;
	for ( unsigned int y = 0; y < 8; ++y, scan += stride )
	{
		const unsigned char* pel = scan;
		for ( unsigned int x = 0; x < 8; ++x, pel += 4, bgra += 4 )
		{
			bgra[ 0 ] = pel[ 0 ];
			bgra[ 1 ] = pel[ 1 ];
			bgra[ 2 ] = pel[ 2 ];
			bgra[ 3 ] = pel[ 3 ];
		}
	}
}

//----------------------------------------------------------
void ExtractBlockA( short* dstA, const unsigned char* aSource, unsigned int bytesPerPel,
				    unsigned int srcWidth, unsigned int srcX, unsigned int srcY )
{
	unsigned int stride = srcWidth * bytesPerPel;
	const unsigned char* scan = aSource + srcY * stride + srcX * bytesPerPel;
	for ( unsigned int y = 0; y < 8; ++y, scan += stride )
	{
		const unsigned char* pel = scan;
		for ( unsigned int x = 0; x < 8; ++x, pel += bytesPerPel, ++dstA )
			*dstA = *pel - 128;
	}
}

//----------------------------------------------------------
void InsertBlockBGRA( unsigned char* bgraDest, unsigned int dstWidth, unsigned int dstX,
					  unsigned int dstY, unsigned char* bgra )
{
	unsigned int stride = dstWidth * 4;
	unsigned char* scan = bgraDest + dstY * stride + dstX * 4;
	for ( unsigned int y = 0; y < 8; ++y, scan += stride )
	{
		unsigned char* pel = scan;
		for ( unsigned int x = 0; x < 8; ++x, pel += 4, bgra += 4 )
		{
			pel[ 0 ] = bgra[ 0 ];
			pel[ 1 ] = bgra[ 1 ];
			pel[ 2 ] = bgra[ 2 ];
			pel[ 3 ] = bgra[ 3 ];
		}
	}
}

//----------------------------------------------------------
void InsertBlockA( unsigned char* aDest, unsigned int bytesPerPel, unsigned int dstWidth,
				   unsigned int dstX, unsigned int dstY, short* srcA )
{
	// note:  The YCoCg decoding stage writes values as XMMWORDs.
	// This means that the right way to do this is to read the values
	// in as XMMWORDs, replace the alpha components, and then write
	// them back out.  If we don't do this, then we hit a store
	// forwarding problem.
	unsigned int stride = dstWidth * 2;
	unsigned char* scan = aDest + dstY * stride + dstX * 2;
	for ( unsigned int y = 0; y < 8; ++y, scan += stride )
	{
		unsigned char* pel = scan;
		for ( unsigned int x = 0; x < 8; ++x, pel += bytesPerPel, ++srcA )
			*pel = Clamp( *srcA + 128, 0, 255 );
	}
}

//----------------------------------------------------------
void EncodeBlock( UHuffman* encoder, const short* src, const unsigned short* quant, unsigned char chop )
{
	// perform the DCT and quantize.
	ForwardDCT( gDCTCoeff, src, quant );

	// store the DC term.
	encoder->WriteValue( ( unsigned char )( gDCTCoeff[ 0 ] & 0xFF ) );
	encoder->WriteValue( ( unsigned char )( gDCTCoeff[ 0 ] >> 8 ) );

	// now encode the data as a run of bytes.
	for ( unsigned int i = 1; i < 64; ++i )
	{
		// simply write out non-zero coefficients.
		unsigned char curCoeff = ( unsigned char )( gDCTCoeff[ kDCTBlockSwizzle0[ i ] ] );
		if ( abs( curCoeff ) > chop )
		{
			// note that we reserve these two values for the encoding
			// process.  Therefor, if any values match it, we must
			// clamp it to 0x82 (-126).
			if ( curCoeff == 0x80 || curCoeff == 0x81 )
				curCoeff = 0x82;
			encoder->WriteValue( curCoeff );
		}
		else
		{
			// write out a run of zeros.
			unsigned int runLength = 0;
			for ( unsigned int j = i + 1; j < 64; ++j )
			{
				if ( abs( gDCTCoeff[ kDCTBlockSwizzle0[ j ] ] ) <= chop )
					++runLength;
				else
					break;
			}

			// if we have a run length greater than one, then we
			// should encode it.  Otherwise, simply write the 0.
			if ( runLength > 0 )
			{
				// make sure we advance i.
				i += runLength;

				// write out the zero-length run if necessary.
				if ( i < 63 )
				{
					encoder->WriteValue( 0x80 );
					encoder->WriteValue( runLength );
				}
				else
				{
					// end the block.
					encoder->WriteValue( 0x81 );
				}
			}
			else
			{
				encoder->WriteValue( 0 );
			}
		}
	}
}

//----------------------------------------------------------
void DecodeBlock( short* dest, UHuffman* decoder, const unsigned short* quant )
{
	// buffer to hold decoded (but still quantized) coefficients.
	short cur = 0;

	// clear the coefficient matrix to zero.
	MemSet( gDCTCoeff, 0, sizeof( gDCTCoeff ) );

	// read the AC term.
	gDCTCoeff[ 0 ] =  decoder->ReadValue();
	gDCTCoeff[ 0 ] |= decoder->ReadValue() << 8;

	// huffman and RLE decode the data into the buffer.
	for ( unsigned int i = 1; i < 64; ++i )
	{
		// determine if we have a run of deltas or a run of zeros.
		unsigned char value = decoder->ReadValue();
		if ( value == 0x80 )
		{
			// we have a run of zeros.
			i += decoder->ReadValue();
		}
		else if ( value == 0x81 )
		{
			break;
		}
		else
		{
			// write out the new value.
			gDCTCoeff[ kDCTBlockSwizzle0[ i ] ] = ( short )( char )value;
		}
	}

	// perform the iDCT.
	InverseDCT( dest, gDCTCoeff, quant );
}

//----------------------------------------------------------
void UnpackYCoCg( unsigned char* bgra, const short* Y, const short* Co, const short* Cg )
{
	// This function takes in one block of Co Cg values that each
	// have a stride of 8 values.  However, only 4 values from each
	// row are actually used because they are subsampled.
#if 1
	// convert two rows at a time to RGB.
	for ( unsigned int i = 0; i < 4; ++i, bgra += 64 )
	{
		__asm
		{
			mov			eax, Co
			mov			ebx, Cg
			mov			esi, Y
			mov			edi, bgra

#if 0
			short curY	= Y[ x ];
			short curCo = Co[ ( y >> 1 ) * 8 + ( x >> 1 ) ];
			short curCg = Cg[ ( y >> 1 ) * 8 + ( x >> 1 ) ];

			short t = curY - ( curCg >> 1 );
			unsigned char g = Max( curCg + t, 0 );
			unsigned char b = Max( t - ( curCo >> 1 ), 0 );
			unsigned char r = Max( b + curCo, 0 );
#endif

			; load the first line of Co and Cg.
			movq		mm0, [ eax ]
			movq		mm1, [ ebx ]

			; promote Co and Cg to 8 words and replicate them across
			; adjacent channels.
			movq2dq		xmm0, mm0
			pshufd		xmm0, xmm0, 01000100b		; x32103210
			pshuflw		xmm0, xmm0, 01010000b		; x32101100
			pshufhw		xmm0, xmm0, 11111010b		; x33221100
			movq2dq		xmm2, mm1
			pshufd		xmm2, xmm2, 01000100b		; x32103210
			pshuflw		xmm2, xmm2, 01010000b		; x32101100
			pshufhw		xmm2, xmm2, 11111010b		; x33221100

			; calculate Co >> 1 and Cg >> 1
			movdqa		xmm1, xmm0
			movdqa		xmm3, xmm2
			psraw		xmm1, 1
			psraw		xmm3, 1

			; load in two rows of Y and calculate G.
			movdqa		xmm4, [ esi +  0 ]
			movdqa		xmm5, [ esi + 16 ]
			psubsw		xmm4, xmm3			; xmm4 = t = curY - ( curCg >> 1 )
			psubsw		xmm5, xmm3			; xmm5 = t = curY - ( curCg >> 1 )

			; xmm2, xmm3	// green.
			; xmm4, xmm5	// blue.
			; xmm6, xmm7	// red.

			; calculate two rows of green.
			movdqa		xmm7, xmm5
			paddsw		xmm7, xmm2
			paddsw		xmm2, xmm4
			movdqa		xmm3, xmm7

			; calculate blue as t - ( curCo >> 1 )
			psubsw		xmm4, xmm1
			psubsw		xmm5, xmm1

			; calculate red as b + curCo
			movdqa		xmm6, xmm4
			movdqa		xmm7, xmm5
			paddsw		xmm6, xmm0
			paddsw		xmm7, xmm0

			; pack the first set of blue and green into xmm0
			packuswb	xmm1, xmm2
			packuswb	xmm0, xmm4
			punpckhbw	xmm0, xmm1		; xmm0 = bgbgbgbgbgbgbgbg

			; pack the first set of red and 0 into xmm1
			packuswb	xmm1, xmm6
			pxor		xmm4, xmm4
			punpckhbw	xmm1, xmm4		; xmm1 = r0r0r0r0r0r0r0r0

			; pack the second set of blue and green into xmm2
			packuswb	xmm3, xmm3
			packuswb	xmm2, xmm5
			punpckhbw	xmm2, xmm3		; xmm2 = bgbgbgbgbgbgbgbg

			; pack the second set of red and 0 into xmm3
			packuswb	xmm3, xmm7
			punpckhbw	xmm3, xmm4		; xmm3 = r0r0r0r0r0r0r0r0

			; split the values across four registers.
			movdqa		xmm4, xmm0
			punpcklwd	xmm4, xmm1		; xmm4 = bgr0bgr0bgr0bgr0
			movdqa		xmm5, xmm0
			punpckhwd	xmm5, xmm1		; xmm5 = bgr0bgr0bgr0bgr0
			movdqa		xmm6, xmm2
			punpcklwd	xmm6, xmm3		; xmm6 = bgr0bgr0bgr0bgr0
			movdqa		xmm7, xmm2
			punpckhwd	xmm7, xmm3		; xmm7 = bgr0bgr0bgr0bgr0

			; store the output.
			movdqa		[edi+ 0], xmm4
			movdqa		[edi+16], xmm5
			movdqa		[edi+32], xmm6
			movdqa		[edi+48], xmm7
		};

		Y += 16;
		Co += 8;
		Cg += 8;
	}

	// clear MMX state.
	__asm emms;
#else
	for ( unsigned int y = 0; y < 8; ++y, Y += 8 )
	{
		for ( unsigned int x = 0; x < 8; ++x, ++red, ++green, ++blue )
		{
			short curY	= Y[ x ];
			short curCo = Co[ ( y >> 1 ) * 8 + ( x >> 1 ) ];
			short curCg = Cg[ ( y >> 1 ) * 8 + ( x >> 1 ) ];

			short t = curY - ( curCg >> 1 );
			unsigned char g = Max( curCg + t, 0 );
			unsigned char b = Max( t - ( curCo >> 1 ), 0 );
			unsigned char r = Max( b + curCo, 0 );

//			unsigned char r = curY + curCo - curCg;
//			unsigned char g = curY + curCg;
//			unsigned char b = curY - curCo - curCg;

			*red = r;
			*green = g;
			*blue = b;
		}
	}
#endif
}

//----------------------------------------------------------
void PackYCoCg( short* Y, short* Co, short* Cg, const unsigned char* bgra )
{
	// This function takes in one block of Co Cg values that each
	// have a stride of 8 values.  However, only 4 values from each
	// row are actually written to because they are subsampled.

	// convert two rows at a time to RGB.
	short CoFull[ 64 ];
	short CgFull[ 64 ];
	for ( unsigned int i = 0; i < 64; ++i, bgra += 4 )
	{
		// get RGB components and expand them to 16-bits.
		short r = bgra[ 2 ];
		short g = bgra[ 1 ];
		short b = bgra[ 0 ];

		CoFull[ i ]	= ( r - b );
		short t = b + ( CoFull[ i ] >> 1 );
		CgFull[ i ]	= ( g - t );
		Y[ i ]		= ( t + ( CgFull[ i ] >> 1 ) );
	}

	// subsample CoCg.
	short* CoSrc = CoFull;
	short* CgSrc = CgFull;
	for ( unsigned int y = 0; y < 4; ++y, Co += 8, Cg += 8, CoSrc += 16, CgSrc += 16 )
	{
		for ( unsigned int x = 0; x < 4; ++x )
		{
#if 1
			short CoSum = CoSrc[ x + x + 0 ] + CoSrc[ x + x + 1 ] +
						  CoSrc[ x + x + 8 ] + CoSrc[ x + x + 9 ];
			short CgSum = CgSrc[ x + x + 0 ] + CgSrc[ x + x + 1 ] +
						  CgSrc[ x + x + 8 ] + CgSrc[ x + x + 9 ];
			Co[ x ] = ( CoSum + 2 ) >> 2;
			Cg[ x ] = ( CgSum + 2 ) >> 2;
#else
			Co[ x ] = CoSrc[ x + x ];
			Cg[ x ] = CgSrc[ x + x ];
#endif
		}
	}
}

//----------------------------------------------------------
void ForwardDCT( short* dest, const short* coeff, const unsigned short* quant )
{
	short temp1[ 64 ];
	float temp2[ 64 ];
	for ( unsigned int i = 0; i < 64; ++i )
		temp1[ i ] = coeff[ i ];

	// perform an 8x8 matrix multiply.
	for ( unsigned int i = 0; i < 8; ++i )
	{
		for ( unsigned int j = 0; j < 8; ++j )
		{
			// convert the current coefficient to floating point.
			float curCoeff = 0;
			for ( unsigned int k = 0; k < 8; ++k )
				curCoeff += kDCTMatrix[ 8 * i + k ] * temp1[ 8 * k + j ];

			// store the current coefficient.
			temp2[ 8 * i + j ] = ( curCoeff );
		}
	}

	// perform an 8x8 matrix multiply.
	for ( unsigned int i = 0; i < 8; ++i )
	{
		for ( unsigned int j = 0; j < 8; ++j )
		{
			// convert the current coefficient to floating point.
			float curCoeff = 0;
			for ( unsigned int k = 0; k < 8; ++k )
				curCoeff += temp2[ 8 * i + k ] * kDCTMatrixT[ 8 * k + j ];

			// quantize and store the current coefficient.
			dest[ 8 * i + j ] = ( short )( ( curCoeff / quant[ 8 * i + j ] ) + 0.5f );
		}
	}
}

//----------------------------------------------------------
#define BITS_INV_ACC						5							// 4 or 5 for IEEE
#define SHIFT_INV_ROW						16 - BITS_INV_ACC
#define SHIFT_INV_COL						1 + BITS_INV_ACC
#define RND_INV_ROW							1024 * (6 - BITS_INV_ACC)	// 1 << (SHIFT_INV_ROW-1)
#define RND_INV_COL							16 * (BITS_INV_ACC - 3)		// 1 << (SHIFT_INV_COL-1)
#define DUP4( X ) (X),(X),(X),(X)
#define DUP8( X ) (X),(X),(X),(X),(X),(X),(X),(X)
#define BIAS_SCALE( X ) ( X / ( BITS_INV_ACC - 3 ) )

static DECLARE_ALIGNED_16( short, M128_tg_1_16[8] ) = { DUP8( 13036 ) };		// tg * (1<<16) + 0.5
static DECLARE_ALIGNED_16( short, M128_tg_2_16[8] ) = { DUP8( 27146 ) };		// tg * (1<<16) + 0.5
static DECLARE_ALIGNED_16( short, M128_tg_3_16[8] ) = { DUP8( -21746 ) };	// tg * (1<<16) + 0.5
static DECLARE_ALIGNED_16( short, M128_cos_4_16[8] ) = { DUP8( -19195 ) };	// cos * (1<<16) + 0.5

// Table for rows 0,4 - constants are multiplied by cos_4_16
static DECLARE_ALIGNED_16( short, M128_tab_i_04[] ) =
{
	16384, 21407, 16384, 8867, // w05 w04 w01 w00
	16384, -8867, 16384, -21407, // w13 w12 w09 w08
	16384, 8867, -16384, -21407, // w07 w06 w03 w02
	-16384, 21407, 16384, -8867, // w15 w14 w11 w10
	22725, 19266, 19266, -4520, // w21 w20 w17 w16
	12873, -22725, 4520, -12873, // w29 w28 w25 w24
	12873, 4520, -22725, -12873, // w23 w22 w19 w18
	4520, 19266, 19266, -22725 // w31 w30 w27 w26
};

// Table for rows 1,7 - constants are multiplied by cos_1_16
static DECLARE_ALIGNED_16( short, M128_tab_i_17[] ) =
{
	22725, 29692, 22725, 12299, // w05 w04 w01 w00
	22725, -12299, 22725, -29692, // w13 w12 w09 w08
	22725, 12299, -22725, -29692, // w07 w06 w03 w02
	-22725, 29692, 22725, -12299, // w15 w14 w11 w10
	31521, 26722, 26722, -6270, // w21 w20 w17 w16
	17855, -31521, 6270, -17855, // w29 w28 w25 w24
	17855, 6270, -31521, -17855, // w23 w22 w19 w18
	6270, 26722, 26722, -31521 // w31 w30 w27 w26
};

// Table for rows 2,6 - constants are multiplied by cos_2_16
static DECLARE_ALIGNED_16( short, M128_tab_i_26[] ) =
{
	21407, 27969, 21407, 11585, // w05 w04 w01 w00
	21407, -11585, 21407, -27969, // w13 w12 w09 w08
	21407, 11585, -21407, -27969, // w07 w06 w03 w02
	-21407, 27969, 21407, -11585, // w15 w14 w11 w10
	29692, 25172, 25172, -5906, // w21 w20 w17 w16
	16819, -29692, 5906, -16819, // w29 w28 w25 w24
	16819, 5906, -29692, -16819, // w23 w22 w19 w18
	5906, 25172, 25172, -29692 // w31 w30 w27 w26
};

// Table for rows 3,5 - constants are multiplied by cos_3_16
static DECLARE_ALIGNED_16( short, M128_tab_i_35[] ) = {
	19266, 25172, 19266, 10426, // w05 w04 w01 w00
	19266, -10426, 19266, -25172, // w13 w12 w09 w08
	19266, 10426, -19266, -25172, // w07 w06 w03 w02
	-19266, 25172, 19266, -10426, // w15 w14 w11 w10
	26722, 22654, 22654, -5315, // w21 w20 w17 w16
	15137, -26722, 5315, -15137, // w29 w28 w25 w24
	15137, 5315, -26722, -15137, // w23 w22 w19 w18
	5315, 22654, 22654, -26722 // w31 w30 w27 w26
};

static DECLARE_ALIGNED_16( const unsigned int, rounder_0[4] ) = { DUP4( RND_INV_ROW - BIAS_SCALE( 2048 ) + 65536 ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_1[4] ) = { DUP4( RND_INV_ROW + BIAS_SCALE( 3755 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_2[4] ) = { DUP4( RND_INV_ROW + BIAS_SCALE( 2472 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_3[4] ) = { DUP4( RND_INV_ROW + BIAS_SCALE( 1361 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_4[4] ) = { DUP4( RND_INV_ROW + BIAS_SCALE( 0 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_5[4] ) = { DUP4( RND_INV_ROW - BIAS_SCALE( 1139 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_6[4] ) = { DUP4( RND_INV_ROW - BIAS_SCALE( 1024 ) ) };
static DECLARE_ALIGNED_16( const unsigned int, rounder_7[4] ) = { DUP4( RND_INV_ROW - BIAS_SCALE( 1301 ) ) };

#define DCT_8_INV_ROW( table1, table2, rounder1, rounder2 ) \
		__asm pshuflw xmm0, xmm0, 0xD8 \
		__asm pshufhw xmm0, xmm0, 0xD8 \
		__asm pshufd xmm3, xmm0, 0x55 \
		__asm pshufd xmm1, xmm0, 0 \
		__asm pshufd xmm2, xmm0, 0xAA \
		__asm pshufd xmm0, xmm0, 0xFF \
		__asm pmaddwd xmm1, [table1+ 0] \
		__asm pmaddwd xmm2, [table1+16] \
		__asm pmaddwd xmm3, [table1+32] \
		__asm pmaddwd xmm0, [table1+48] \
		__asm paddd xmm0, xmm3 \
		__asm pshuflw xmm4, xmm4, 0xD8 \
		__asm pshufhw xmm4, xmm4, 0xD8 \
		__asm paddd xmm1, rounder1 \
		__asm pshufd xmm6, xmm4, 0xAA \
		__asm pshufd xmm5, xmm4, 0 \
		__asm pmaddwd xmm5, [table2+ 0] \
		__asm paddd xmm5, rounder2 \
		__asm pmaddwd xmm6, [table2+16] \
		__asm pshufd xmm7, xmm4, 0x55 \
		__asm pmaddwd xmm7, [table2+32] \
		__asm pshufd xmm4, xmm4, 0xFF \
		__asm pmaddwd xmm4, [table2+48] \
		__asm paddd xmm1, xmm2 \
		__asm movdqa xmm2, xmm1 \
		__asm psubd xmm2, xmm0 \
		__asm psrad xmm2, SHIFT_INV_ROW \
		__asm pshufd xmm2, xmm2, 0x1B \
		__asm paddd xmm0, xmm1 \
		__asm psrad xmm0, SHIFT_INV_ROW \
		__asm paddd xmm5, xmm6 \
		__asm packssdw xmm0, xmm2 \
		__asm paddd xmm4, xmm7 \
		__asm movdqa xmm6, xmm5 \
		__asm psubd xmm6, xmm4 \
		__asm psrad xmm6, SHIFT_INV_ROW \
		__asm paddd xmm4, xmm5 \
		__asm psrad xmm4, SHIFT_INV_ROW \
		__asm pshufd xmm6, xmm6, 0x1B \
		__asm packssdw xmm4, xmm6

#define DCT_8_INV_COL_8 \
		__asm movdqa xmm6, xmm4 \
		__asm movdqa xmm2, xmm0 \
		__asm movdqa xmm3, XMMWORD PTR [edx+3*16] \
		__asm movdqa xmm1, XMMWORD PTR M128_tg_3_16 \
		__asm pmulhw xmm0, xmm1 \
		__asm movdqa xmm5, XMMWORD PTR M128_tg_1_16 \
		__asm pmulhw xmm1, xmm3 \
		__asm paddsw xmm1, xmm3 \
		__asm pmulhw xmm4, xmm5 \
		__asm movdqa xmm7, XMMWORD PTR [edx+6*16] \
		__asm pmulhw xmm5, [edx+1*16] \
		__asm psubsw xmm5, xmm6 \
		__asm movdqa xmm6, xmm5 \
		__asm paddsw xmm4, [edx+1*16] \
		__asm paddsw xmm0, xmm2 \
		__asm paddsw xmm0, xmm3 \
		__asm psubsw xmm2, xmm1 \
		__asm movdqa xmm1, xmm0 \
		__asm movdqa xmm3, XMMWORD PTR M128_tg_2_16 \
		__asm pmulhw xmm7, xmm3 \
		__asm pmulhw xmm3, [edx+2*16] \
		__asm paddsw xmm0, xmm4 \
		__asm psubsw xmm4, xmm1 \
		__asm movdqa [edx+7*16], xmm0 \
		__asm psubsw xmm5, xmm2 \
		__asm paddsw xmm6, xmm2 \
		__asm movdqa [edx+3*16], xmm6 \
		__asm movdqa xmm1, xmm4 \
		__asm movdqa xmm0, XMMWORD PTR M128_cos_4_16 \
		__asm movdqa xmm2, xmm0 \
		__asm paddsw xmm4, xmm5 \
		__asm psubsw xmm1, xmm5 \
		__asm paddsw xmm7, [edx+2*16] \
		__asm psubsw xmm3, [edx+6*16] \
		__asm movdqa xmm6, [edx] \
		__asm pmulhw xmm0, xmm1 \
		__asm movdqa xmm5, [edx+4*16] \
		__asm paddsw xmm5, xmm6 \
		__asm psubsw xmm6, [edx+4*16] \
		__asm pmulhw xmm2, xmm4 \
		__asm paddsw xmm4, xmm2 \
		__asm movdqa xmm2, xmm5 \
		__asm psubsw xmm2, xmm7 \
		__asm paddsw xmm0, xmm1 \
		__asm paddsw xmm5, xmm7 \
		__asm movdqa xmm1, xmm6 \
		__asm movdqa xmm7, [edx+7*16] \
		__asm paddsw xmm7, xmm5 \
		__asm psraw xmm7, SHIFT_INV_COL \
		__asm movdqa [edx], xmm7 \
		__asm paddsw xmm6, xmm3 \
		__asm psubsw xmm1, xmm3 \
		__asm movdqa xmm7, xmm1 \
		__asm movdqa xmm3, xmm6 \
		__asm paddsw xmm6, xmm4 \
		__asm psraw xmm6, SHIFT_INV_COL \
		__asm movdqa [edx+1*16], xmm6 \
		__asm paddsw xmm1, xmm0 \
		__asm psraw xmm1, SHIFT_INV_COL \
		__asm movdqa [edx+2*16], xmm1 \
		__asm movdqa xmm1, [edx+3*16] \
		__asm movdqa xmm6, xmm1 \
		__asm psubsw xmm7, xmm0 \
		__asm psraw xmm7, SHIFT_INV_COL \
		__asm movdqa [edx+5*16], xmm7 \
		__asm psubsw xmm5, [edx+7*16] \
		__asm psraw xmm5, SHIFT_INV_COL \
		__asm movdqa [edx+7*16], xmm5 \
		__asm psubsw xmm3, xmm4 \
		__asm paddsw xmm6, xmm2 \
		__asm psubsw xmm2, xmm1 \
		__asm psraw xmm6, SHIFT_INV_COL \
		__asm movdqa [edx+3*16], xmm6 \
		__asm psraw xmm2, SHIFT_INV_COL \
		__asm movdqa [edx+4*16], xmm2 \
		__asm psraw xmm3, SHIFT_INV_COL \
		__asm movdqa [edx+6*16], xmm3

#define DEQUANTIZE( reg, mem )			__asm pmullw reg, mem

void InverseDCT( short *dest, const short *coeff, const unsigned short *quant )
{
	__asm mov eax, coeff
	__asm mov edx, dest
	__asm mov esi, quant
	__asm movdqa xmm0, XMMWORD PTR[eax+16*0] // row 0
	__asm movdqa xmm4, XMMWORD PTR[eax+16*2] // row 2
	DEQUANTIZE( xmm0, XMMWORD PTR[esi+16*0] )
	DEQUANTIZE( xmm4, XMMWORD PTR[esi+16*2] )
	DCT_8_INV_ROW( M128_tab_i_04, M128_tab_i_26, rounder_0, rounder_2 );

	__asm movdqa XMMWORD PTR[edx+16*0], xmm0
	__asm movdqa XMMWORD PTR[edx+16*2], xmm4
	__asm movdqa xmm0, XMMWORD PTR[eax+16*4] // row 4
	__asm movdqa xmm4, XMMWORD PTR[eax+16*6] // row 6
	DEQUANTIZE( xmm0, XMMWORD PTR[esi+16*4] )
	DEQUANTIZE( xmm4, XMMWORD PTR[esi+16*6] )
	DCT_8_INV_ROW( M128_tab_i_04, M128_tab_i_26, rounder_4, rounder_6 );

	__asm movdqa XMMWORD PTR[edx+16*4], xmm0
	__asm movdqa XMMWORD PTR[edx+16*6], xmm4
	__asm movdqa xmm0, XMMWORD PTR[eax+16*3] // row 3
	__asm movdqa xmm4, XMMWORD PTR[eax+16*1] // row 1
	DEQUANTIZE( xmm0, XMMWORD PTR[esi+16*3] )
	DEQUANTIZE( xmm4, XMMWORD PTR[esi+16*1] )
	DCT_8_INV_ROW( M128_tab_i_35, M128_tab_i_17, rounder_3, rounder_1 );

	__asm movdqa XMMWORD PTR[edx+16*3], xmm0
	__asm movdqa XMMWORD PTR[edx+16*1], xmm4
	__asm movdqa xmm0, XMMWORD PTR[eax+16*5] // row 5
	__asm movdqa xmm4, XMMWORD PTR[eax+16*7] // row 7
	DEQUANTIZE( xmm0, XMMWORD PTR[esi+16*5] )
	DEQUANTIZE( xmm4, XMMWORD PTR[esi+16*7] )
	DCT_8_INV_ROW( M128_tab_i_35, M128_tab_i_17, rounder_5, rounder_7 );

	DCT_8_INV_COL_8
}


//**********************************************************
// Implementation of the extreme DXT algorithm in SSE2.
//**********************************************************

unsigned int kColorDividerTable[768];
unsigned int kAlphaDividerTable[256];
unsigned int kColorIndicesTable[256];
unsigned int kAlphaIndicesTable[640];

DECLARE_ALIGNED_16( const unsigned char, kSSEByte0[16] ) = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEWord1[16] ) = { 0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEWord8[16] ) = { 0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEBoundsMask[16] ) = { 0x00,0x1F,0x00,0x1F,0xE0,0x07,0xE0,0x07,0x00,0xF8,0x00,0xF8,0x00,0xFF,0xFF,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEBoundScale[16] ) = { 0x20,0x00,0x20,0x00,0x08,0x00,0x08,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x01,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesMask0[16] ) = { 0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesMask1[16] ) = { 0x00,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesMask2[16] ) = { 0x08,0x08,0x08,0x00,0x08,0x08,0x08,0x00,0x08,0x08,0x08,0x00,0x08,0x08,0x08,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale0[16] ) = { 0x01,0x00,0x04,0x00,0x10,0x00,0x40,0x00,0x01,0x00,0x04,0x00,0x10,0x00,0x40,0x00 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale1[16] ) = { 0x01,0x00,0x04,0x00,0x01,0x00,0x08,0x00,0x10,0x00,0x40,0x00,0x00,0x01,0x00,0x08 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale2[16] ) = { 0x01,0x04,0x10,0x40,0x01,0x04,0x10,0x40,0x01,0x04,0x10,0x40,0x01,0x04,0x10,0x40 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale3[16] ) = { 0x01,0x04,0x01,0x04,0x01,0x08,0x01,0x08,0x01,0x04,0x01,0x04,0x01,0x08,0x01,0x08 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale4[16] ) = { 0x01,0x00,0x10,0x00,0x01,0x00,0x00,0x01,0x01,0x00,0x10,0x00,0x01,0x00,0x00,0x01 };
DECLARE_ALIGNED_16( const unsigned char, kSSEIndicesScale[16] ) = { 0x00,0x02,0x04,0x06,0x01,0x03,0x05,0x07,0x08,0x0A,0x0C,0x0E,0x09,0x0B,0x0D,0x0F };

// temporary stores.  This avoids stack frame generation for aligned
// local variables (ebx).
DECLARE_ALIGNED_16( unsigned char, gDXTMin[32] );
DECLARE_ALIGNED_16( unsigned char, gDXTRange[32] );
DECLARE_ALIGNED_16( unsigned char, gDXTBounds[32] );
DECLARE_ALIGNED_16( unsigned char, gDXTIndices[64] );

//----------------------------------------------------------
void BuildDXTTables()
{
	// build the color divider table.
	for ( int i = 0; i < 768; ++i )
		kColorDividerTable[i] = ( ( ( 1 << 15 ) / ( i + 1 ) ) << 16 ) | ( ( 1 << 15 ) / ( i + 1 ) );

	// build the alpha divider table.
	for ( int i = 0; i < 256; ++i )
		kAlphaDividerTable[i] = ( ( ( 1 << 16 ) / ( i + 1 ) ) << 16 );

	// build the color indices table.
	const unsigned char kColorIndexLUT[] =
	{
		1, 3, 2, 0,
	};
	for ( int i = 0; i < 256; ++i )
	{
		unsigned char ci3 = kColorIndexLUT[ ( i & 0xC0 ) >> 6 ] << 6;
		unsigned char ci2 = kColorIndexLUT[ ( i & 0x30 ) >> 4 ] << 4;
		unsigned char ci1 = kColorIndexLUT[ ( i & 0x0C ) >> 2 ] << 2;
		unsigned char ci0 = kColorIndexLUT[ ( i & 0x03 ) >> 0 ] << 0;
		kColorIndicesTable[ i ] = ci3 | ci2 | ci1 | ci0;
	}

	// build the alpha indices table.
	const int kShiftLeft[] = { 0, 1, 2, 0, 1, 2, 3, 0, 1, 2 };
	const int kShiftRight[] = { 0, 0, 0, 2, 2, 2, 2, 1, 1, 1 };
	const unsigned short kAlphaIndex[] = { 1, 7, 6, 5, 4, 3, 2, 0 };
	for ( int j = 0; j < 10; ++j )
	{
		int sl = kShiftLeft[ j ] * 6;
		int sr = kShiftRight[ j ] * 2;
		for ( int i = 0; i < 64; ++i )
		{
			unsigned short ai1 = kAlphaIndex[ ( i & 0x38 ) >> 3 ] << 3;
			unsigned short ai0 = kAlphaIndex[ ( i & 0x07 ) >> 0 ] << 0;
			kAlphaIndicesTable[ ( j * 64 ) + i ] = ( ( ai1 | ai0 ) << sl ) >> sr;
		}
	}
}

//----------------------------------------------------------
void CompressImageDXT1( const unsigned char* argb, unsigned char* dxt1, int width, int height )
{
	int x_count;
	int y_count;
	__asm
	{
		mov			esi, DWORD PTR argb // src
		mov			edi, DWORD PTR dxt1 // dst
		mov			eax, DWORD PTR height
		mov			DWORD PTR y_count, eax
	y_loop:
		mov			eax, DWORD PTR width
		mov			DWORD PTR x_count, eax
	x_loop:
		mov			eax, DWORD PTR width // width * 1
		lea			ebx, DWORD PTR [eax + eax*2] // width * 3
		movdqa		xmm0, XMMWORD PTR [esi + 0] // src + width * 0 + 0
		movdqa		xmm3, XMMWORD PTR [esi + eax*4 + 0] // src + width * 4 + 0
		movdqa		xmm1, xmm0
		pmaxub		xmm0, xmm3
		pmaxub		xmm0, XMMWORD PTR [esi + eax*8 + 0] // src + width * 8 + 0
		pmaxub		xmm0, XMMWORD PTR [esi + ebx*4 + 0] // src + width * 12 + 0
		pminub		xmm1, xmm3
		pminub		xmm1, XMMWORD PTR [esi + eax*8 + 0] // src + width * 8 + 0
		pminub		xmm1, XMMWORD PTR [esi + ebx*4 + 0] // src + width * 12 + 0
		pshufd		xmm2, xmm0, 0x4E
		pshufd		xmm3, xmm1, 0x4E
		pmaxub		xmm0, xmm2
		pminub		xmm1, xmm3
		pshufd		xmm2, xmm0, 0xB1
		pshufd		xmm3, xmm1, 0xB1
		pmaxub		xmm0, xmm2
		pminub		xmm1, xmm3
		movdqa		xmm4, XMMWORD PTR [esi + 16] // src + width * 0 + 16
		movdqa		xmm7, XMMWORD PTR [esi + eax*4 + 16] // src + width * 4 + 16
		movdqa		xmm5, xmm4
		pmaxub		xmm4, xmm7
		pmaxub		xmm4, XMMWORD PTR [esi + eax*8 + 16] // src + width * 8 + 16
		pmaxub		xmm4, XMMWORD PTR [esi + ebx*4 + 16] // src + width * 12 + 16
		pminub		xmm5, xmm7
		pminub		xmm5, XMMWORD PTR [esi + eax*8 + 16] // src + width * 8 + 16
		pminub		xmm5, XMMWORD PTR [esi + ebx*4 + 16] // src + width * 12 + 16
		pshufd		xmm6, xmm4, 0x4E
		pshufd		xmm7, xmm5, 0x4E
		pmaxub		xmm4, xmm6
		pminub		xmm5, xmm7
		pshufd		xmm6, xmm4, 0xB1
		pshufd		xmm7, xmm5, 0xB1
		pmaxub		xmm4, xmm6
		pminub		xmm5, xmm7
		movdqa		XMMWORD PTR gDXTMin[ 0], xmm1
		movdqa		XMMWORD PTR gDXTMin[16], xmm5
		movdqa		xmm7, XMMWORD PTR kSSEByte0
		punpcklbw	xmm0, xmm7
		punpcklbw	xmm4, xmm7
		punpcklbw	xmm1, xmm7
		punpcklbw	xmm5, xmm7
		movdqa		xmm2, xmm0
		movdqa		xmm6, xmm4
		psubw		xmm2, xmm1
		psubw		xmm6, xmm5
		movq		MMWORD PTR gDXTRange[ 0], xmm2
		movq		MMWORD PTR gDXTRange[16], xmm6
		psrlw		xmm2, 4
		psrlw		xmm6, 4
		psubw		xmm0, xmm2
		psubw		xmm4, xmm6
		paddw		xmm1, xmm2
		paddw		xmm5, xmm6
		punpcklwd	xmm0, xmm1
		pmullw		xmm0, XMMWORD PTR kSSEBoundScale
		pand		xmm0, XMMWORD PTR kSSEBoundsMask
		movdqa		XMMWORD PTR gDXTBounds[ 0], xmm0
		punpcklwd	xmm4, xmm5
		pmullw		xmm4, XMMWORD PTR kSSEBoundScale
		pand		xmm4, XMMWORD PTR kSSEBoundsMask
		movdqa		XMMWORD PTR gDXTBounds[16], xmm4
		movzx		ecx, WORD PTR gDXTRange [ 0]
		movzx		edx, WORD PTR gDXTRange [16]
		mov			eax, DWORD PTR gDXTBounds[ 0]
		mov			ebx, DWORD PTR gDXTBounds[16]
		shr			eax, 8
		shr			ebx, 8
		or			eax, DWORD PTR gDXTBounds[ 4]
		or			ebx, DWORD PTR gDXTBounds[20]
		or			eax, DWORD PTR gDXTBounds[ 8]
		or			ebx, DWORD PTR gDXTBounds[24]
		mov			DWORD PTR [edi + 0], eax
		mov			DWORD PTR [edi + 8], ebx
		add			cx, WORD PTR gDXTRange [ 2]
		add			dx, WORD PTR gDXTRange [18]
		add			cx, WORD PTR gDXTRange [ 4]
		add			dx, WORD PTR gDXTRange [20]
		mov			ecx, DWORD PTR kColorDividerTable[ecx*4]
		mov			edx, DWORD PTR kColorDividerTable[edx*4]
		movzx		eax, WORD PTR [edi + 0]
		xor			ax, WORD PTR [edi + 2]
		cmovz		ecx, eax
		movzx		ebx, WORD PTR [edi + 8]
		xor			bx, WORD PTR [edi + 10]
		cmovz		edx, ebx
		mov			eax, DWORD PTR width // width * 1
		lea			ebx, DWORD PTR [eax + eax*2] // width * 3
		movdqa		xmm0, XMMWORD PTR [esi + 0] // src + width * 0 + 0
		movdqa		xmm1, XMMWORD PTR [esi + eax*4 + 0] // src + width * 4 + 0
		movdqa		xmm7, XMMWORD PTR gDXTMin[ 0]
		psubb		xmm0, xmm7
		psubb		xmm1, xmm7
		movdqa		xmm2, XMMWORD PTR [esi + eax*8 + 0] // src + width * 8 + 0
		movdqa		xmm3, XMMWORD PTR [esi + ebx*4 + 0] // src + width * 12 + 0
		psubb		xmm2, xmm7
		psubb		xmm3, xmm7
		movdqa		xmm4, xmm0
		movdqa		xmm5, xmm1
		movdqa		xmm6, XMMWORD PTR kSSEIndicesMask0
		movdqa		xmm7, XMMWORD PTR kSSEIndicesMask1
		pand		xmm0, xmm6
		pand		xmm1, xmm6
		pmaddwd		xmm0, XMMWORD PTR kSSEWord8
		pmaddwd		xmm1, XMMWORD PTR kSSEWord8
		pand		xmm4, xmm7
		pand		xmm5, xmm7
		psrlw		xmm4, 5
		psrlw		xmm5, 5
		paddw		xmm0, xmm4
		paddw		xmm1, xmm5
		movdqa		xmm4, xmm2
		movdqa		xmm5, xmm3
		pand		xmm2, xmm6
		pand		xmm3, xmm6
		pmaddwd		xmm2, XMMWORD PTR kSSEWord8
		pmaddwd		xmm3, XMMWORD PTR kSSEWord8
		pand		xmm4, xmm7
		pand		xmm5, xmm7
		psrlw		xmm4, 5
		psrlw		xmm5, 5
		paddw		xmm2, xmm4
		paddw		xmm3, xmm5
		movd		xmm7, ecx
		pshufd		xmm7, xmm7, 0x00
		packssdw	xmm0, xmm1
		pmulhw		xmm0, xmm7
		pmaddwd		xmm0, XMMWORD PTR kSSEIndicesScale0
		packssdw	xmm2, xmm3
		pmulhw		xmm2, xmm7
		pmaddwd		xmm2, XMMWORD PTR kSSEIndicesScale0
		packssdw	xmm0, xmm2
		pmaddwd		xmm0, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[ 0], xmm0
		movdqa		xmm0, XMMWORD PTR [esi + 16] // src + width * 0 + 16
		movdqa		xmm1, XMMWORD PTR [esi + eax*4 + 16] // src + width * 4 + 16
		movdqa		xmm7, XMMWORD PTR gDXTMin[16]
		psubb		xmm0, xmm7
		psubb		xmm1, xmm7
		movdqa		xmm2, XMMWORD PTR [esi + eax*8 + 16] // src + width * 8 + 16
		movdqa		xmm3, XMMWORD PTR [esi + ebx*4 + 16] // src + width * 12 + 16
		psubb		xmm2, xmm7
		psubb		xmm3, xmm7
		movdqa		xmm4, xmm0
		movdqa		xmm5, xmm1
		movdqa		xmm6, XMMWORD PTR kSSEIndicesMask0
		movdqa		xmm7, XMMWORD PTR kSSEIndicesMask1
		pand		xmm4, xmm7
		pand		xmm5, xmm7
		psrlw		xmm4, 5
		psrlw		xmm5, 5
		pand		xmm0, xmm6
		pand		xmm1, xmm6
		pmaddwd		xmm0, XMMWORD PTR kSSEWord8
		pmaddwd		xmm1, XMMWORD PTR kSSEWord8
		paddw		xmm0, xmm4
		paddw		xmm1, xmm5
		movdqa		xmm4, xmm2
		movdqa		xmm5, xmm3
		pand		xmm4, xmm7
		pand		xmm5, xmm7
		psrlw		xmm4, 5
		psrlw		xmm5, 5
		pand		xmm2, xmm6
		pand		xmm3, xmm6
		pmaddwd		xmm2, XMMWORD PTR kSSEWord8
		pmaddwd		xmm3, XMMWORD PTR kSSEWord8
		paddw		xmm2, xmm4
		paddw		xmm3, xmm5
		movd		xmm7, edx
		pshufd		xmm7, xmm7, 0x00
		packssdw	xmm0, xmm1
		pmulhw		xmm0, xmm7
		pmaddwd		xmm0, XMMWORD PTR kSSEIndicesScale0
		packssdw	xmm2, xmm3
		pmulhw		xmm2, xmm7
		pmaddwd		xmm2, XMMWORD PTR kSSEIndicesScale0
		packssdw	xmm0, xmm2
		pmaddwd		xmm0, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[32], xmm0
		movzx		eax, BYTE PTR gDXTIndices[ 0]
		movzx		ebx, BYTE PTR gDXTIndices[ 4]
		mov			cl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			ch, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 4], cl
		mov			BYTE PTR [edi + 5], ch
		movzx		eax, BYTE PTR gDXTIndices[ 8]
		movzx		ebx, BYTE PTR gDXTIndices[12]
		mov			dl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			dh, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 6], dl
		mov			BYTE PTR [edi + 7], dh
		movzx		eax, BYTE PTR gDXTIndices[32]
		movzx		ebx, BYTE PTR gDXTIndices[36]
		mov			cl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			ch, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 12], cl
		mov			BYTE PTR [edi + 13], ch
		movzx		eax, BYTE PTR gDXTIndices[40]
		movzx		ebx, BYTE PTR gDXTIndices[44]
		mov			dl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			dh, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 14], dl
		mov			BYTE PTR [edi + 15], dh
		add			esi, 32 // src += 32
		add			edi, 16 // dst += 16
		sub			DWORD PTR x_count, 8
		jnz	x_loop
		mov			eax, DWORD PTR width // width * 1
		lea			ebx, DWORD PTR [eax + eax*2] // width * 3
		lea			esi, DWORD PTR [esi + ebx*4] // src += width * 12
		sub			DWORD PTR y_count, 4
		jnz y_loop
	}
}

//----------------------------------------------------------
void CompressImageDXT5( const unsigned char* argb, BYTE* dxt5, int width, int height )
{
	int x_count;
	int y_count;
	__asm
	{
		mov			esi, DWORD PTR argb			// src
		mov			edi, DWORD PTR dxt5			// dst
		mov			eax, DWORD PTR height
		mov			DWORD PTR y_count, eax
		y_loop:
		mov			eax, DWORD PTR width
		mov			DWORD PTR x_count, eax
		x_loop:
		mov			eax, DWORD PTR width // width * 1
		lea			ebx, DWORD PTR [eax + eax*2] // width * 3
		movdqa		xmm0, XMMWORD PTR [esi + 0] // src + width * 0 + 0
		movdqa		xmm3, XMMWORD PTR [esi + eax*4 + 0] // src + width * 4 + 0
		movdqa		xmm1, xmm0
		pmaxub		xmm0, xmm3
		pminub		xmm1, xmm3
		pmaxub		xmm0, XMMWORD PTR [esi + eax*8 + 0] // src + width * 8 + 0
		pminub		xmm1, XMMWORD PTR [esi + eax*8 + 0] // src + width * 8 + 0
		pmaxub		xmm0, XMMWORD PTR [esi + ebx*4 + 0] // src + width * 12 + 0
		pminub		xmm1, XMMWORD PTR [esi + ebx*4 + 0] // src + width * 12 + 0
		pshufd		xmm2, xmm0, 0x4E
		pmaxub		xmm0, xmm2
		pshufd		xmm3, xmm1, 0x4E
		pminub		xmm1, xmm3
		pshufd		xmm2, xmm0, 0xB1
		pmaxub		xmm0, xmm2
		pshufd		xmm3, xmm1, 0xB1
		pminub		xmm1, xmm3
		movdqa		xmm4, XMMWORD PTR [esi + 16] // src + width * 0 + 16
		movdqa		xmm7, XMMWORD PTR [esi + eax*4 + 16] // src + width * 4 + 16
		movdqa		xmm5, xmm4
		pmaxub		xmm4, xmm7
		pminub		xmm5, xmm7
		pmaxub		xmm4, XMMWORD PTR [esi + eax*8 + 16] // src + width * 8 + 16
		pminub		xmm5, XMMWORD PTR [esi + eax*8 + 16] // src + width * 8 + 16
		pmaxub		xmm4, XMMWORD PTR [esi + ebx*4 + 16] // src + width * 12 + 16
		pminub		xmm5, XMMWORD PTR [esi + ebx*4 + 16] // src + width * 12 + 16
		pshufd		xmm6, xmm4, 0x4E
		pmaxub		xmm4, xmm6
		pshufd		xmm7, xmm5, 0x4E
		pminub		xmm5, xmm7
		pshufd		xmm6, xmm4, 0xB1
		pmaxub		xmm4, xmm6
		pshufd		xmm7, xmm5, 0xB1
		pminub		xmm5, xmm7
		movdqa		XMMWORD PTR gDXTMin[ 0], xmm1
		movdqa		XMMWORD PTR gDXTMin[16], xmm5
		movdqa		xmm7, XMMWORD PTR kSSEByte0
		punpcklbw	xmm0, xmm7
		punpcklbw	xmm4, xmm7
		punpcklbw	xmm1, xmm7
		punpcklbw	xmm5, xmm7
		movdqa		xmm2, xmm0
		movdqa		xmm6, xmm4
		psubw		xmm2, xmm1
		psubw		xmm6, xmm5
		movq		MMWORD PTR gDXTRange[ 0], xmm2
		movq		MMWORD PTR gDXTRange[16], xmm6
		psrlw		xmm2, 4
		psrlw		xmm6, 4
		psubw		xmm0, xmm2
		psubw		xmm4, xmm6
		paddw		xmm1, xmm2
		paddw		xmm5, xmm6
		punpcklwd	xmm0, xmm1
		pmullw		xmm0, XMMWORD PTR kSSEBoundScale
		pand		xmm0, XMMWORD PTR kSSEBoundsMask
		movdqa		XMMWORD PTR gDXTBounds[ 0], xmm0
		punpcklwd	xmm4, xmm5
		pmullw		xmm4, XMMWORD PTR kSSEBoundScale
		pand		xmm4, XMMWORD PTR kSSEBoundsMask
		movdqa		XMMWORD PTR gDXTBounds[16], xmm4
		mov			eax, DWORD PTR gDXTBounds[ 0]
		mov			ebx, DWORD PTR gDXTBounds[16]
		shr			eax, 8
		shr			ebx, 8
		movzx		ecx, WORD PTR gDXTBounds[13]
		movzx		edx, WORD PTR gDXTBounds[29]
		mov			DWORD PTR [edi + 0], ecx
		mov			DWORD PTR [edi + 16], edx
		or			eax, DWORD PTR gDXTBounds[ 4]
		or			ebx, DWORD PTR gDXTBounds[20]
		or			eax, DWORD PTR gDXTBounds[ 8]
		or			ebx, DWORD PTR gDXTBounds[24]
		mov			DWORD PTR [edi + 8], eax
		mov			DWORD PTR [edi + 24], ebx
		movzx		eax, WORD PTR gDXTRange [ 6]
		movzx		ebx, WORD PTR gDXTRange [22]
		mov			eax, DWORD PTR kAlphaDividerTable[eax*4]
		mov			ebx, DWORD PTR kAlphaDividerTable[ebx*4]
		movzx		ecx, WORD PTR gDXTRange [ 0]
		movzx		edx, WORD PTR gDXTRange [16]
		add			cx, WORD PTR gDXTRange [ 2]
		add			dx, WORD PTR gDXTRange [18]
		add			cx, WORD PTR gDXTRange [ 4]
		add			dx, WORD PTR gDXTRange [20]
		movzx		ecx, WORD PTR kColorDividerTable[ecx*4]
		movzx		edx, WORD PTR kColorDividerTable[edx*4]
		or			ecx, eax
		or			edx, ebx
		mov			eax, DWORD PTR width				; width * 1
		lea			ebx, DWORD PTR [eax + eax*2]		; width * 3
		movdqa		xmm0, XMMWORD PTR [esi + 0]			; src + width * 0 + 0
		movdqa		xmm1, XMMWORD PTR [esi + eax*4 + 0] ; src + width * 4 + 0
		movdqa		xmm7, XMMWORD PTR gDXTMin[ 0]
		psubb		xmm0, xmm7
		psubb		xmm1, xmm7
		movdqa		xmm2, XMMWORD PTR [esi + eax*8 + 0] ; src + 8 * width + 0
		psubb		xmm2, xmm7
		movdqa		xmm3, XMMWORD PTR [esi + ebx*4 + 0] ; src + 12 * width + 0
		psubb		xmm3, xmm7
		movdqa		xmm6, XMMWORD PTR kSSEIndicesMask0
		movdqa		xmm7, XMMWORD PTR kSSEWord8
		movdqa		xmm4, xmm0
		movdqa		xmm5, xmm1
		pand		xmm0, xmm6
		pand		xmm1, xmm6
		psrlw		xmm4, 8
		psrlw		xmm5, 8
		pmaddwd		xmm0, xmm7
		pmaddwd		xmm1, xmm7
		psllw		xmm4, 3
		psllw		xmm5, 3
		paddw		xmm0, xmm4
		paddw		xmm1, xmm5
		movdqa		xmm4, xmm2
		movdqa		xmm5, xmm3
		pand		xmm2, xmm6
		pand		xmm3, xmm6
		psrlw		xmm4, 8
		psrlw		xmm5, 8
		pmaddwd		xmm2, xmm7
		pmaddwd		xmm3, xmm7
		psllw		xmm4, 3
		psllw		xmm5, 3
		paddw		xmm2, xmm4
		paddw		xmm3, xmm5
		movd		xmm7, ecx
		pshufd		xmm7, xmm7, 0x00
		pmulhw		xmm0, xmm7
		pmulhw		xmm1, xmm7
		pshuflw		xmm0, xmm0, 0xD8
		pshufhw		xmm0, xmm0, 0xD8
		pshuflw		xmm1, xmm1, 0xD8
		pshufhw		xmm1, xmm1, 0xD8
		movdqa		xmm6, XMMWORD PTR kSSEIndicesScale1
		pmaddwd		xmm0, xmm6
		pmaddwd		xmm1, xmm6
		packssdw	xmm0, xmm1
		pshuflw		xmm0, xmm0, 0xD8
		pshufhw		xmm0, xmm0, 0xD8
		pmaddwd		xmm0, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[ 0 ], xmm0
		pmulhw		xmm2, xmm7
		pmulhw		xmm3, xmm7
		pshuflw		xmm2, xmm2, 0xD8
		pshufhw		xmm2, xmm2, 0xD8
		pshuflw		xmm3, xmm3, 0xD8
		pshufhw		xmm3, xmm3, 0xD8
		pmaddwd		xmm2, xmm6
		pmaddwd		xmm3, xmm6
		packssdw	xmm2, xmm3
		pshuflw		xmm2, xmm2, 0xD8
		pshufhw		xmm2, xmm2, 0xD8
		pmaddwd		xmm2, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[16], xmm2
		movdqa		xmm0, XMMWORD PTR [esi + 16]			// src + width * 0 + 16
		movdqa		xmm1, XMMWORD PTR [esi + eax*4 + 16]	// src + width * 4 + 16
		movdqa		xmm7, XMMWORD PTR gDXTMin[16]
		psubb		xmm0, xmm7
		psubb		xmm1, xmm7
		movdqa		xmm2, XMMWORD PTR [esi + eax*8 + 16]	// src + width * 8 + 16
		psubb		xmm2, xmm7
		movdqa		xmm3, XMMWORD PTR [esi + ebx*4 + 16]	// src + width * 12 + 16
		psubb		xmm3, xmm7
		movdqa		xmm6, XMMWORD PTR kSSEIndicesMask0
		movdqa		xmm7, XMMWORD PTR kSSEWord8
		movdqa		xmm4, xmm0
		movdqa		xmm5, xmm1
		pand		xmm0, xmm6
		pand		xmm1, xmm6
		pmaddwd		xmm0, xmm7
		pmaddwd		xmm1, xmm7
		psrlw		xmm4, 8
		psrlw		xmm5, 8
		psllw		xmm4, 3
		psllw		xmm5, 3
		paddw		xmm0, xmm4
		paddw		xmm1, xmm5
		movdqa		xmm4, xmm2
		movdqa		xmm5, xmm3
		pand		xmm2, xmm6
		pand		xmm3, xmm6
		pmaddwd		xmm2, xmm7
		pmaddwd		xmm3, xmm7
		psrlw		xmm4, 8
		psrlw		xmm5, 8
		psllw		xmm4, 3
		psllw		xmm5, 3
		paddw		xmm2, xmm4
		paddw		xmm3, xmm5
		movd		xmm7, edx
		pshufd		xmm7, xmm7, 0x00
		pmulhw		xmm0, xmm7
		pmulhw		xmm1, xmm7
		pshuflw		xmm0, xmm0, 0xD8
		pshufhw		xmm0, xmm0, 0xD8
		pshuflw		xmm1, xmm1, 0xD8
		pshufhw		xmm1, xmm1, 0xD8
		movdqa		xmm6, XMMWORD PTR kSSEIndicesScale1
		pmaddwd		xmm0, xmm6
		pmaddwd		xmm1, xmm6
		packssdw	xmm0, xmm1
		pshuflw		xmm0, xmm0, 0xD8
		pshufhw		xmm0, xmm0, 0xD8
		pmaddwd		xmm0, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[32], xmm0
		pmulhw		xmm2, xmm7
		pmulhw		xmm3, xmm7
		pshuflw		xmm2, xmm2, 0xD8
		pshufhw		xmm2, xmm2, 0xD8
		pshuflw		xmm3, xmm3, 0xD8
		pshufhw		xmm3, xmm3, 0xD8
		pmaddwd		xmm2, xmm6
		pmaddwd		xmm3, xmm6
		packssdw	xmm2, xmm3
		pshuflw		xmm2, xmm2, 0xD8
		pshufhw		xmm2, xmm2, 0xD8
		pmaddwd		xmm2, XMMWORD PTR kSSEWord1
		movdqa		XMMWORD PTR gDXTIndices[48], xmm2
		movzx		eax, BYTE PTR gDXTIndices[ 0]
		movzx		ebx, BYTE PTR gDXTIndices[ 8]
		mov			cl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			ch, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 12], cl
		mov			BYTE PTR [edi + 13], ch
		movzx		eax, BYTE PTR gDXTIndices[16]
		movzx		ebx, BYTE PTR gDXTIndices[24]
		mov			dl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			dh, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 14], dl
		mov			BYTE PTR [edi + 15], dh
		movzx		eax, BYTE PTR gDXTIndices[32]
		movzx		ebx, BYTE PTR gDXTIndices[40]
		mov			cl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			ch, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 28], cl
		mov			BYTE PTR [edi + 29], ch
		movzx		eax, BYTE PTR gDXTIndices[48]
		movzx		ebx, BYTE PTR gDXTIndices[56]
		mov			dl, BYTE PTR kColorIndicesTable[eax*1 + 0]
		mov			dh, BYTE PTR kColorIndicesTable[ebx*1 + 0]
		mov			BYTE PTR [edi + 30], dl
		mov			BYTE PTR [edi + 31], dh
		movzx		eax, BYTE PTR gDXTIndices[ 4]
		movzx		ebx, BYTE PTR gDXTIndices[36]
		mov			cx, WORD PTR kAlphaIndicesTable[eax*2 + 0]
		mov			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 0]
		movzx		eax, BYTE PTR gDXTIndices[ 5]
		movzx		ebx, BYTE PTR gDXTIndices[37]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 128]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 128]
		movzx		eax, BYTE PTR gDXTIndices[12]
		movzx		ebx, BYTE PTR gDXTIndices[44]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 256]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 256]
		mov			WORD PTR [edi + 2], cx
		mov			WORD PTR [edi + 18], dx
		mov			cx, WORD PTR kAlphaIndicesTable[eax*2 + 384]
		mov			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 384]
		movzx		eax, BYTE PTR gDXTIndices[13]
		movzx		ebx, BYTE PTR gDXTIndices[45]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 512]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 512]
		movzx		eax, BYTE PTR gDXTIndices[20]
		movzx		ebx, BYTE PTR gDXTIndices[52]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 640]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 640]
		movzx		eax, BYTE PTR gDXTIndices[21]
		movzx		ebx, BYTE PTR gDXTIndices[53]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 768]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 768]
		mov			WORD PTR [edi + 4], cx
		mov			WORD PTR [edi + 20], dx
		mov			cx, WORD PTR kAlphaIndicesTable[eax*2 + 896]
		mov			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 896]
		movzx		eax, BYTE PTR gDXTIndices[28]
		movzx		ebx, BYTE PTR gDXTIndices[60]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 1024]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 1024]
		movzx		eax, BYTE PTR gDXTIndices[29]
		movzx		ebx, BYTE PTR gDXTIndices[61]
		or			cx, WORD PTR kAlphaIndicesTable[eax*2 + 1152]
		or			dx, WORD PTR kAlphaIndicesTable[ebx*2 + 1152]
		mov			WORD PTR [edi + 6], cx
		mov			WORD PTR [edi + 22], dx
		add			esi, 32 // src += 32
		add			edi, 32 // dst += 32
		sub			DWORD PTR x_count, 8
		jnz x_loop
		mov			eax, DWORD PTR width // width * 1
		lea			ebx, DWORD PTR [eax + eax*2] // width * 3
		lea			esi, DWORD PTR [esi + ebx*4] // src += width * 12
		sub			DWORD PTR y_count, 4
		jnz y_loop
	}
}
