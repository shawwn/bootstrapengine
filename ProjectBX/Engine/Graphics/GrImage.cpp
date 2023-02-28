//----------------------------------------------------------
// File:		GrImage.cpp
// Author:		Kevin Bray
// Created:		09-20-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrImage.h"

// project includes.
#include "GrDXT.h"
#include "UReader.h"
#include "MVec3.h"
#include "RFileMgr.h"
//#include "GrDebug.h"

// OpenGL includes.
#include "GLSubsys.h"

// Debug includes.
#ifdef WINDOWS
#include <crtdbg.h>
#endif

//#define _NO_TEXTURE_TEST

// macros.
#define	MAKE_FOUR_CC( c1, c2, c3, c4 )		( ( c1 ) | ( ( c2 )<<8 ) | ( ( c3 )<<16 ) | ( ( c4 )<<24 ) )

// pack/unpack a quantized float.
#define PACK_QFLOAT( v )					( ( unsigned char )( Clamp( ( float )( v ) * 0.5f + 0.5f, -1.0f, 1.0f ) * 254.0f ) )
#define UNPACK_QFLOAT( v )					( float( ( Min( ( float )( v ), 254.0f ) / 254.0f * 2.0f ) - 1.0f ) )

unsigned int _elemsPerPixel[] =
{
	3,		// EF_BGR24
	4,		// EF_BGRA32
	3,		// EF_RGB96F
	4,		// EF_RGBA128F
	1,		// EF_I8
	1,		// EF_I16F
	2,		// EF_LA16
	2,		// EF_LA64F
	1,		// EF_DEPTH
};

const GLenum _imageFormatMap[] =
{
	GL_BGR,				// EF_BGR24
	GL_BGRA,			// EF_BGRA32
	GL_RGB,				// EF_RGB96F
	GL_RGBA,			// EF_RGBA128F
	GL_RED,				// EF_I8
	GL_RED,				// EF_I16F
	GL_LUMINANCE_ALPHA,	// EF_LA16
	GL_LUMINANCE_ALPHA,	// EF_LA64F
	GL_DEPTH_COMPONENT,	// EF_DEPTH
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
};

const GLenum _imageTypeMap[] =
{
	GL_UNSIGNED_BYTE,			// EF_BGR24
	GL_UNSIGNED_BYTE,			// EF_BGRA32
	GL_FLOAT,					// EF_RGB96F
	GL_FLOAT,					// EF_RGBA128F
	GL_UNSIGNED_BYTE,			// EF_I8
	GL_FLOAT,					// EF_I16F
	GL_UNSIGNED_BYTE,			// EF_LA16
	GL_FLOAT,					// EF_LA64F
	GL_FLOAT,					// EF_DEPTH
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
};

unsigned int _elemSize[] =
{
	1,			// EF_BGR24
	1,			// EF_BGRA32
	4,			// EF_RGB96F
	4,			// EF_RGBA128F
	1,			// EF_I8
	4,			// EF_I16F
	1,			// EF_LA16
	4,			// EF_LA64F
	4,			// EF_DEPTH
	1,			// EF_RGB_DXT1
	1,			// EF_RGBA_DXT1
	1,			// EF_RGBA_DXT5
};


//**********************************************************
// file local functions.
//**********************************************************
typedef void* ( *GenMipLevelFunc )( void* srcIn, unsigned int srcWidth, unsigned int srcHeight, unsigned int elemsPerPixel );

static void* GenMipLevelBytes( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );
static void* GenMipLevelBytesBorder( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );

static void* GenMipLevelBytesBGR15_A1( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );
static void* GenMipLevelBytesBorderBGR15_A1( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );

static void* GenMipLevelBytesNM( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );
static void* GenMipLevelBytesNMBorder( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );

static void* GenMipLevelFloats( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );
static void* GenMipLevelFloatsBorder( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel );

static void ReplaceMipBorder( void* data, unsigned int mipWidth, unsigned int mipHeight, unsigned int elemsPerPixel, unsigned char* borderColor );

//**********************************************************
// class GrImage
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrImage::GrImage( const UPath& name, EFORMAT format, unsigned int width, unsigned int height, void* data )
: _name( name )
, _bits( new void*[ 1 ] )
, _width( width )
, _height( height )
, _bytesPerPel( _elemsPerPixel[ format ] * _elemSize[ format ] )
, _mipCount( 1 )
, _maxMipCount( 1 )
, _fmt( format )
{
	unsigned int imageSize = _width * _height * _bytesPerPel;
	_bits[ 0 ] = new unsigned char [ imageSize ];

	// if data was passed in, clone it.
	if ( data )
		MemCopy( _bits[ 0 ], data, imageSize );
}

//----------------------------------------------------------
GrImage::GrImage( const UPath& name, unsigned long bgraColor )
: _name( name )
, _bits( new void *[ 1 ] )
, _width( 1 )
, _height( 1 )
, _bytesPerPel( 4 )
, _mipCount( 1 )
, _maxMipCount( 1 )
, _fmt( EF_BGRA32 )
{
	_bits[ 0 ] = new unsigned char[ 4 ];
	( ( unsigned long * )_bits[ 0 ] )[ 0 ] = bgraColor;
}

//----------------------------------------------------------
GrImage::GrImage( const UPath& name, UReader& reader )
: _name( name )
, _bits( 0 )
, _width( 0 )
, _height( 0 )
, _bytesPerPel( 0 )
, _mipCount( 1 )
, _maxMipCount( 1 )
{
	Load( name, reader );
}

//----------------------------------------------------------
GrImage::GrImage( const GrImage& image )
: _name( image._name )
, _bits( new void*[ image._maxMipCount ] )
, _width( image._width )
, _height( image._height )
, _bytesPerPel( image._bytesPerPel )
, _mipCount( 1 )
, _maxMipCount( image._maxMipCount )
, _fmt( image._fmt )
{
	// allocate the image data and copy it.
	unsigned int imageSize = _width * _height * _bytesPerPel;
	_bits[ 0 ] = new unsigned char [ imageSize ];
	MemCopy( _bits[ 0 ], image._bits[ 0 ], imageSize );
}

//----------------------------------------------------------
GrImage::~GrImage()
{
	// cleanup all data.
	for ( unsigned int i = 0; i < _mipCount; ++i )
	{
		// FIXME: this pointer possibly came from outside the class,  so might not be safe to delete with this cast
		delete[] ( unsigned char* )_bits[ i ];
	}
	delete[] _bits;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrImage::EFORMAT
GrImage::GetFormat()
{
	return _fmt;
}

//----------------------------------------------------------
bool
GrImage::IsCompressed() const
{
	return ( _fmt == EF_BGR_DXT1 ) || ( _fmt == EF_BGRA_DXT5 );
}

//----------------------------------------------------------
void
GrImage::ConvertToI8( bool keepMipMaps )
{
	// check for early outs.
	if ( _fmt == EF_I8 )
		return;

	// store whether or not we had mipmaps to begin with and then clear mipmaps.
	bool rebuildMipMaps = _mipCount != 1 && keepMipMaps;
	ClearMipMaps();

	// create the new pointer for the data bits.
	void* finalImage = new unsigned char[ _width * _height ];
	if ( _fmt == EF_BGRA32 )
	{
		unsigned char* src = ( unsigned char* )_bits[ 0 ];
		unsigned char* dst = ( unsigned char* )finalImage;
		unsigned int elemCount = _elemsPerPixel[ _fmt ];
		for ( unsigned int y = 0; y < _height; ++y )
			for ( unsigned int x = 0; x < _width; ++x, src += elemCount, ++dst )
				*dst = src[ 3 ];	// store alpha.
	}
	else if ( _fmt == EF_BGR24 )
	{
		unsigned char* src = ( unsigned char* )_bits[ 0 ];
		unsigned char* dst = ( unsigned char* )finalImage;
		unsigned int elemCount = _elemsPerPixel[ _fmt ];
		for ( unsigned int y = 0; y < _height; ++y )
			for ( unsigned int x = 0; x < _width; ++x, src += elemCount, ++dst )
				*dst = *src;		// use blue component.
	}
	else
	{
		// invalid source format.  Must be 8-bit.
		B_ASSERT( false );
	}

	// store the format change.
	_fmt = EF_I8;

	// free up the old memory and store the new memory.
	delete[] ( unsigned char* )_bits[ 0 ];
	_bits[ 0 ] = finalImage;

	// rebuild mipmaps if necessary.
	if ( rebuildMipMaps )
		GenMipMaps( false );
}

//----------------------------------------------------------
void
GrImage::ConvertToLA16( bool keepMipMaps )
{
	// no need?
	if ( _fmt == EF_LA16 )
		return;

	// ensure that we're 32-bit RGBA.
	B_ASSERT( _fmt == EF_BGRA32 );

	// store whether or not we had mipmaps to begin with and then clear mipmaps.
	bool rebuildMipMaps = _mipCount != 1 && keepMipMaps;
	ClearMipMaps();

	// create the new pointer for the data bits.
	void* finalImage = new unsigned char[ _width * _height * 2 ];
	unsigned char* src = ( unsigned char* )_bits[ 0 ];
	unsigned char* dst = ( unsigned char* )finalImage;
	unsigned int elemCount = _elemsPerPixel[ _fmt ];
	if ( _fmt == EF_BGRA32 )
	{
		for ( unsigned int y = 0; y < _height; ++y )
		{
			for ( unsigned int x = 0; x < _width; ++x, src += elemCount, dst += 2 )
			{
				dst[ 0 ] = src[ 0 ];		// use blue component.
				dst[ 1 ] = src[ 3 ];		// use alpha component.
			}
		}
	}
	else if ( _fmt == EF_BGR24 )
	{
		for ( unsigned int y = 0; y < _height; ++y )
		{
			for ( unsigned int x = 0; x < _width; ++x, src += elemCount, dst += 2 )
			{
				dst[ 0 ] = src[ 0 ];		// use blue component.
				dst[ 1 ] = 255;				// fully white alpha component.
			}
		}
	}
	else if ( _fmt == EF_I8 )
	{
		for ( unsigned int y = 0; y < _height; ++y )
		{
			for ( unsigned int x = 0; x < _width; ++x, src += elemCount, dst += 2 )
			{
				dst[ 0 ] = src[ 0 ];
				dst[ 1 ] = src[ 0 ];
			}
		}
	}
	else
	{
		// invalid source format.  Must be 8-bit.
		B_ASSERT( false );
	}

	// store the format change.
	_fmt = EF_LA16;

	// free up the old memory and store the new memory.
	delete[] ( unsigned char* )_bits[ 0 ];
	_bits[ 0 ] = finalImage;

	// rebuild mipmaps if necessary.
	if ( rebuildMipMaps )
		GenMipMaps( false );
}

//----------------------------------------------------------
void
GrImage::ConvertBGR24ToBGRA32()
{
	// ensure that the image is BGR24.
	B_ASSERT( _fmt == EF_BGR24 );

	// expand the image from RGB to RGBA.
	unsigned int mipWidth = _width;
	unsigned int mipHeight = _height;
	unsigned char* src = 0;
	unsigned char* dst = 0;
	unsigned char* newMip = 0;
	for ( unsigned int mip = 0; mip < _mipCount; ++mip )
	{
		// get the source image.
		src = ( unsigned char* )_bits[ mip ];

		// determine what to do with the source image.
		// alloc new memory and copy.
		newMip = dst = new unsigned char[ mipWidth * mipHeight * 4 ];
		for ( unsigned int y = 0; y < mipHeight; ++y )
		{
			for ( unsigned int x = 0; x < mipWidth; ++x, dst += 4 )
			{
				for ( unsigned int e = 0; e < 3; ++e, ++src )
					dst[ e ] = *src;
				dst[ 3 ] = 255;
			}
		}

		// delete the old mip level and replace with the current one.
		delete[] ( unsigned char* )_bits[ mip ];
		_bits[ mip ] = newMip;

		// next mip-level.
		mipWidth = Max( mipWidth >> 1, 1U );
		mipHeight = Max( mipHeight >> 1, 1U );
	}

	// adjust our format to reflect our changes.
	_fmt = EF_BGRA32;
	_bytesPerPel = 4;
}

//----------------------------------------------------------
void
GrImage::SwizzleToBXRG()
{
	// This function replicates R to channels RGB, and move G to the A channel.
	// If the image is RGB, it will be converted to RGBA first.  If the image is
	// L8 or LA16/64F, then this function will simply return.
	if ( _fmt == EF_I8 || _fmt == EF_LA16 || _fmt == EF_LA64F )
		return;

	// floating point images are not supported at this time.
	B_ASSERT( !IsFloatImage() );

	unsigned int mipWidth = 0;
	unsigned int mipHeight = 0;
	if ( !IsFloatImage() )
	{
		unsigned char* src = 0;
		unsigned char* dst = 0;

		// do we need to expand the image from RGB to RGBA?
		if ( _fmt == EF_BGR24 )
			ConvertBGR24ToBGRA32();

		// only format that should be possible right here is RGBA32.
		B_ASSERT( _fmt == EF_BGRA32 );

		// go through each level and swizzle.
		mipWidth = _width;
		mipHeight = _height;
		for ( unsigned int mip = 0; mip < _mipCount; ++mip )
		{
			src = ( unsigned char* )_bits[ mip ];
			for ( unsigned int y = 0; y < mipHeight; ++y )
			{
				for ( unsigned int x = 0; x < mipWidth; ++x, src += 4 )
				{
					// zyxa -> zaxy
					// alpha = green.

					// unpack X, Y, and Z.
					float nx = 2.0f * ( src[ 2 ] / 255.0f ) - 1.0f;
					float ny = 2.0f * ( src[ 1 ] / 255.0f ) - 1.0f;
					float nz = 2.0f * ( src[ 0 ] / 255.0f ) - 1.0f;

					// divide X and Y by Z.
					float invZ = 1.0f / nz;
					nx *= invZ;
					ny *= invZ;

					// repack the values.
					src[ 0 ] = 255;
					src[ 1 ] = 255;
					src[ 2 ] = ( unsigned char )Clamp( ( 0.5f * nx + 0.5f ) * 255.0f, 0.0f, 255.0f );
					src[ 3 ] = ( unsigned char )Clamp( ( 0.5f * ny + 0.5f ) * 255.0f, 0.0f, 255.0f );
				}
			}
			mipWidth = Max( mipWidth >> 1, 1U );
			mipHeight = Max( mipHeight >> 1, 1U );
		}
	}
	else
	{
		B_ASSERT( false );
	}
}

//----------------------------------------------------------
void
GrImage::CompressDXT1( bool normalMap )
{
	// expand the image to BGRA if necessary.
	if ( _fmt == EF_BGR24 )
		ConvertBGR24ToBGRA32();

	// make sure the image is in the appropriate format.
	B_ASSERT( _fmt == EF_BGRA32 );

	// Make sure mipmaps are available since we're compressing
	// the existing data.
	GenMipMaps( normalMap );

	// compress to DXT1.
	for ( unsigned int i = 0; i < _mipCount; ++i )
	{
		// calculate the width and height of the current mipmap.
		unsigned int mipWidth = Max( _width >> i, 1U );
		unsigned int mipHeight = Max( _height >> i, 1U );

		// allocate space for the current mip-level.
		unsigned int dataSize = CalcImageSizeDXT1( mipWidth, mipHeight );
		unsigned char* data = new unsigned char[ dataSize ];

		// get the current mip level and calculate it's stride.
		unsigned char* src = ( unsigned char* )_bits[ i ];

		// compress the miplevel.
		::CompressDXT1( ( unsigned int* )data, mipWidth, mipHeight, ( unsigned int* )src );

		// free the previous mip-level and replace it with the
		// compressed data.
		delete[] src;
		_bits[ i ] = data;
	}

	// change info about the image.
	_bytesPerPel = 0;
	_fmt = EF_BGR_DXT1;
}

//----------------------------------------------------------
void
GrImage::CompressDXT5( bool normalMap )
{
	// make sure the image is in the appropriate format.  Note
	// that it would make no sense to compress any other format.
	B_ASSERT( _fmt == EF_BGRA32 );

	// Make sure mipmaps are available since we're compressing
	// the existing data.
	GenMipMaps( normalMap );

	// compress to DXT5.
	for ( unsigned int i = 0; i < _mipCount; ++i )
	{
		// calculate the width and height of the current mipmap.
		unsigned int mipWidth = Max( _width >> i, 1U );
		unsigned int mipHeight = Max( _height >> i, 1U );

		// allocate space for the current mip-level.
		unsigned int dataSize = CalcImageSizeDXT5( mipWidth, mipHeight );;
		unsigned char* data = new unsigned char[ dataSize ];

		// get the current mip level and calculate it's stride.
		unsigned char* src = ( unsigned char* )_bits[ i ];

		// compress the miplevel.
		::CompressDXT5( ( unsigned int* )data, mipWidth, mipHeight, ( unsigned int* )src );

		// free the previous mip-level and replace it with the
		// compressed data.
		delete[] src;
		_bits[ i ] = data;
	}

	// change info about the image.
	_bytesPerPel = 0;
	_fmt = EF_BGRA_DXT5;
}

//----------------------------------------------------------
void
GrImage::GenMipMaps( bool normalMap )
{
	EnsureMipLevel( _maxMipCount-1, normalMap );
}

//----------------------------------------------------------
void
GrImage::ClearMipMaps()
{
	if ( !HasMipMaps() )
		return;

	// free up mipmaps.
	for ( unsigned int i = 1; i < _mipCount; ++i )
	{
		delete[] ( unsigned char* )_bits[ i ];
		_bits[ i ] = 0;
	}
	_mipCount = 1;
}

//----------------------------------------------------------
void
GrImage::TexImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
					 unsigned int mipCount, bool normalMap )
{
	// submit each mipmap to the currently bound texture.
	B_ASSERT( ( startMip + mipCount ) <= _maxMipCount );

	// submit each mipmap to the currently bound texture.
#if !defined _NO_TEXTURE_TEST
	if ( mipCount == 0 )
		mipCount = _maxMipCount;
	else
		mipCount += startMip;
	mipCount = Min( mipCount, _maxMipCount );

	// submit the mip levels.
	for ( unsigned int i = startMip; i < mipCount; ++i )
	{
		unsigned int mipLevel = ( i - startMip );
		unsigned int mipWidth = Max( _width >> i, 1U );
		unsigned int mipHeight = Max( _height >> i, 1U );
		unsigned int format = _imageFormatMap[ _fmt ];
		if ( _fmt == EF_I8 )
		{
			// if we're an alpha format, we need to use alpha, not luminance.
			if ( glTexFormat == GL_ALPHA8 || glTexFormat == GL_ALPHA )
				format = GL_ALPHA;
		}

		EnsureMipLevel( i, normalMap );
		bglTexImage2D( bindTarget, mipLevel, glTexFormat, mipWidth, mipHeight, 0,
			format, _imageTypeMap[ _fmt ], _bits[ i ] );
	}
#else
	bglTexImage2D( bindTarget, 0, glTexFormat, 1, 1, 0, _imageFormatMap[ _fmt ],
		_imageTypeMap[ _fmt ], _bits[ _mipCount - 1 ] );
#endif

	CHECK_GL();
}

//----------------------------------------------------------
void
GrImage::TexSubImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
					    unsigned int mipCount, bool normalMap )
{
	// submit each mipmap to the currently bound texture.
	B_ASSERT( ( startMip + mipCount ) <= _maxMipCount );

#if !defined _NO_TEXTURE_TEST
	// submit each mipmap to the currently bound texture.
	if ( mipCount == 0 )
		mipCount = _maxMipCount;
	else
		mipCount += startMip;
	mipCount = Min( mipCount, _maxMipCount );

	// submit the mip levels.
	for ( unsigned int i = startMip; i < mipCount; ++i )
	{
		// get info about the current mip level.
		unsigned int mipLevel = ( i - startMip );
		unsigned int mipWidth = Max( _width >> i, 1U );
		unsigned int mipHeight = Max( _height >> i, 1U );
		unsigned int format = _imageFormatMap[ _fmt ];
		if ( _fmt == EF_I8 )
		{
			// if we're an alpha format, we need to use alpha, not luminance.
			if ( glTexFormat == GL_ALPHA8 || glTexFormat == GL_ALPHA )
				format = GL_ALPHA;
		}

		// update our texture.
		EnsureMipLevel( i, normalMap );
		bglTexSubImage2D( bindTarget, mipLevel, 0, 0, mipWidth, mipHeight, format,
			_imageTypeMap[ _fmt ], _bits[ i ] );
	}
#else
	bglTexImage2D( bindTarget, 0, glTexFormat, 1, 1, 0, _imageFormatMap[ _fmt ],
		_imageTypeMap[ _fmt ], _bits[ _mipCount - 1 ] );
#endif

	CHECK_GL();
}

//----------------------------------------------------------
void
GrImage::CompressedTexImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
							   unsigned int mipCount, bool normalMap )
{
	// make sure glTexFormat is a supported compression format and that the format
	// of the image is EF_BGRA32.
#if _DEBUG
	B_ASSERT( _fmt == EF_BGR_DXT1 || _fmt == EF_BGRA_DXT5 );
	B_ASSERT( _mipCount == _maxMipCount );
	if ( _fmt == EF_BGR_DXT1 )
	{
		B_ASSERT( glTexFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
	}
	else
	{
		B_ASSERT( glTexFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
	}
#endif

#if !defined _NO_TEXTURE_TEST
	// submit each mipmap to the currently bound texture.
	if ( mipCount == 0 )
		mipCount = _maxMipCount;
	else
		mipCount += startMip;
	mipCount = Min( mipCount, _maxMipCount );

	// submit the mip levels.
	for ( unsigned int i = startMip; i < mipCount; ++i )
	{
		// get info about the current mip level.
		unsigned int mipLevel = ( i - startMip );
		unsigned int mipWidth = Max( _width >> i, 1U );
		unsigned int mipHeight = Max( _height >> i, 1U );
		unsigned int mipSize = 0;
		if ( _fmt == EF_BGR_DXT1 )
			mipSize = CalcImageSizeDXT1( mipWidth, mipHeight );
		else
			mipSize = CalcImageSizeDXT5( mipWidth, mipHeight );

		// update our texture.
		EnsureMipLevel( i, normalMap );
		bglCompressedTexImage2D( bindTarget, mipLevel, glTexFormat, mipWidth, mipHeight, 0,
			mipSize, _bits[ i ] );
	}
#else
	bglTexImage2D( bindTarget, 0, glTexFormat, 1, 1, 0, _imageFormatMap[ _fmt ],
		_imageTypeMap[ _fmt ], _bits[ _mipCount - 1 ] );
#endif

	CHECK_GL();
}

//----------------------------------------------------------
void*
GrImage::GetImageData( unsigned int mipLevel )
{
	B_ASSERT( mipLevel < _maxMipCount );
	EnsureMipLevel( mipLevel, false );
	return _bits[ mipLevel ];
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrImage::Load( const UPath& name, UReader& reader )
{
	// determine what kind of image we're loading.
	size_t extDot = name.GetPathString().find_last_of( '.' );
	tstring ext;
	if ( extDot != tstring::npos )
	{
		ext = name.GetPathString().substr( extDot + 1 );
	}

	// use the extension to determine how to load the file.
	if ( _stricmp( ext.c_str(), "dds" ) == 0 )
	{
		ReadDDS( name, reader );
	}
	else if ( _stricmp( ext.c_str(), "tga" ) == 0 )
	{
		ReadTGA( name, reader );
	}
	else
	{
		// unsupported image format!
		B_ASSERT( false );
	}
}

//----------------------------------------------------------
void
GrImage::ReadDDS( const UPath& name, UReader& reader )
{
	enum EIMAGEFLAGS
	{
		kPitchFlag		= 0x00000008,	// pitchOrLinSize specifies pitch.
		kLinearSizeFlag	= 0x00080000,	// pitchOrLinSize specifies linear size.
		kDepthFlag		= 0x00800000,	// depth of volume textures.
		kMipMapCountFlag= 0x00020000,	// mipmaps are present.
	};

	enum EFORMATFLAGS
	{
		kAlphaFlag		= 0x00000001,
		kFourCCFlag		= 0x00000004,
		kRGBFlag		= 0x00000040,
	};

	enum EIMAGECAPS1
	{
		kComplex		= 0x00000008,	// more than one surface specified in the file.
		kMipMapFlag		= 0x00400000,	// contains mipmap surfaces.
	};

	enum EIMAGECAPS2
	{
		kCubeMapFlag	= 0x00000200,	// a cubemap is specified.
		kCubePositiveXFlag	= 0x00000400,	// positive X face is specified.
		kCubeNegativeXFlag	= 0x00000800,	// negative X face is specified.
		kCubePositiveYFlag	= 0x00001000,	// positive Y face is specified.
		kCubeNegativeYFlag	= 0x00002000,	// negative Y face is specified.
		kCubePositiveZFlag	= 0x00004000,	// positive Z face is specified.
		kCubeNegativeZFlag	= 0x00008000,	// negative Z face is specified.
		kVolumeFlag		= 0x00200000,	// a volume is specified.
	};

	struct Header
	{
		unsigned long	reserved;		// must be 124
		unsigned long	imageFlags;		// must always have 0x1007 set.
		unsigned long	height;
		unsigned long	width;
		unsigned long	pitchOrLinSize; // specifies pitch for uncompressed images and linear size otherwise.
		unsigned long	depth;
		unsigned long	mipMapCount;
		unsigned long	reserved1[ 11 ];
		unsigned long	reserved2;		// must be 32
		unsigned long	formatFlags;	// kRGBFlag for uncompressed formats, kFourCCFlag for compressed formats.
		unsigned long	fourCC;			// four CC code.
		unsigned long	bitCount;		// used only for uncompressed formats.
		unsigned long	redMask;		// red bits
		unsigned long	greenMask;		// green bits
		unsigned long	blueMask;		// blue bits
		unsigned long	alphaMask;		// alpha bits... this is only used when kAlphaFlag is set.
		unsigned long	imageCaps1;		// bit 0x00001000 must always be set.  Refer to EIMAGECAPS1 for other flags.
		unsigned long	imageCaps2;		// describes what is contained in the image.
	};

	// read the magic ( DDS ).
	unsigned int magic = reader.ReadInt();
	B_ASSERT( magic == MAKE_FOUR_CC( 'D','D','S',' ' ) );

	// read the header.
	Header header;
	reader.ReadBytes( &header, sizeof( Header ) );

	// we don't support compressed textures or 16-bit formats!
	B_ASSERT( ( header.formatFlags & kFourCCFlag ) == 0 );

	// determine whether or not we need to flip the data for the image.
	if ( header.bitCount == 8 )
	{
		// we only support alpha textures for 8-bit.
		if ( header.alphaMask != 0xFF )
		{
			B_ERROR( "Unsupported format for image " + name.GetPathString() + ": Only alpha textures are supported for 8-bit greyscale." );
		}
		_fmt = EF_I8;
	}
	else if ( header.bitCount == 24 )
	{
		// 24-bit RGB
		if ( header.redMask != 0xFF0000 || header.greenMask != 0xFF00 || header.blueMask != 0xFF )
		{
			B_ERROR( "Unsupported format for 24-bit image " + name.GetPathString() + ": Must be BGR format!" );
		}
		_fmt = EF_BGR24;
	}
	else if ( header.bitCount == 32 )
	{
		// 32-bit RGBA
		if ( header.alphaMask != 0xFF000000 || header.redMask != 0xFF0000 || header.greenMask != 0xFF00 || header.blueMask != 0xFF )
		{
			B_ERROR( "Unsupported format for 32-bit image " + name.GetPathString() + ": Must be BGRA format!" );
		}
		_fmt = EF_BGRA32;
	}
	else
	{
		// unsupported format.
		B_ERROR( "Unsupported format for image " + name.GetPathString() + "!" );
	}

	// calculate the image size.  Note that I was checking kPitchFlag, but the ATI
	// compressonator seems to have a bug regarding this.  Weird, but not surprising.
	// Since we don't support loading compressed textures anyways, this is a safe
	// assumption.
	unsigned long imageSize = header.pitchOrLinSize * header.height;

	_width = header.width;
	_height = header.height;

	// determine how many mipmap levels need to be present.
	_maxMipCount = CalcMaxMipCount();

	// load the image data.
	unsigned long ddsBytesPerPel = header.bitCount / 8;
	_bytesPerPel = ddsBytesPerPel;
	unsigned long mipMapCount = header.mipMapCount > 0 ? header.mipMapCount : 1;

	// make sure there is only one mip-level or all mip-levels... nothing in between.
	if ( mipMapCount != 1 && mipMapCount != _maxMipCount )
	{
		B_ERROR( "Error loading image " + name.GetPathString() + ": When using custom mipmaps, all must be specified ( down to 1x1 )!" );
	}

	_bits = new void *[ _maxMipCount ];
	MemSet( _bits, 0, _maxMipCount * sizeof( void * ) );

	unsigned char* pixelRow = ( unsigned char* )alloca( _width * ddsBytesPerPel );
	for ( unsigned int mipLevel = 0; mipLevel < mipMapCount; ++mipLevel )
	{
		// read in the current mip level.
		unsigned int width = Max( header.width >> mipLevel, 1UL );
		unsigned int ddsPitch = width * ddsBytesPerPel;
		unsigned int imgPitch = width * _bytesPerPel;
		unsigned int height = Max( header.height >> mipLevel, 1UL );
		_bits[ mipLevel ] = new unsigned char[ _bytesPerPel * height ];

		// read in the top down image and convert to bottom up ( GL goes bottom up, DX top down ).
		unsigned int row = height;
		while ( row-- > 0 )
		{
			// make sure pitch is aligned.
			reader.ReadBytes( pixelRow, ddsPitch );
			for ( unsigned int pixel = 0; pixel < width; ++pixel )
			{
				MemCopy( ( unsigned char* )_bits[ mipLevel ] + row * imgPitch + pixel * _bytesPerPel,
						pixelRow + pixel * ddsBytesPerPel,
						ddsBytesPerPel );
			}
		}
	}
	_mipCount = mipMapCount;
}

//----------------------------------------------------------
void
GrImage::ReadTGA( const UPath& name, UReader& reader )
{
#ifdef __GNUC__
 #pragma pack ( push, 1 )
#else
 #pragma pack ( push )
 #pragma pack ( 1 )
#endif
struct STGAHeaderInfo
{
	unsigned char	idlen;    // length of optional identification sequence
	unsigned char	cmtype;   // indicates whether a palette is present
	unsigned char	imtype;   // image data type ( e.g., uncompressed RGB )
	unsigned short	cmorg;    // first palette index, if present
	unsigned short	cmcnt;    // number of palette entries, if present
	unsigned char	cmsize;   // number of bits per palette entry
	unsigned short	imxorg;   // horiz pixel coordinate of lower left of image
	unsigned short	imyorg;   // vert pixel coordinate of lower left of image
	unsigned short	imwidth;  // image width in pixels
	unsigned short	imheight; // image height in pixels
	unsigned char	imdepth;  // image color depth ( bits per pixel )
	unsigned char	imdesc;   // image attribute flags
};
#pragma pack ( pop )

	STGAHeaderInfo header;
	reader.ReadBytes( &header, sizeof( STGAHeaderInfo ) );

	// make sure the image is unmapped BGRA.
	if ( ( header.imtype & 2 ) == 0 )
	{
		B_ERROR( "Unable to load image " + name.GetPathString() + ": Invalid format (Non-palettized images only)!" );
	}
	B_ASSERT( IsPow2( header.imwidth ) );
	B_ASSERT( IsPow2( header.imheight ) );

	// store info regarding the image.
	_width = header.imwidth;
	_height = header.imheight;

	// determine the maximum mip count.
	_maxMipCount = CalcMaxMipCount();

	// make sure this is a valid image!
	if ( !IsPow2( _width ) || !IsPow2( _height ) )
	{
		B_ERROR( "Unable to load image " + name.GetPathString() + ": Dimensions must be a power of 2!" );
	}

	// determine the format.
	if ( header.imdepth == 32 )
	{
		_fmt = EF_BGRA32;
	}
	else if ( header.imdepth == 24 )
	{
		_fmt = EF_BGR24;
	}
	else if ( header.imdepth == 8 )
	{
		_fmt = EF_I8;
	}
	else
	{
		// we do not support 16-bit TGAs.  I can't find any program that
		// actually still creates them.
		B_ERROR( "Unable to load image " + name.GetPathString() + ": It is not a supported color format!" );
	}

	unsigned int bpp = header.imdepth;

	// allocate storage for data.
//	unsigned int tgaBytesPerPel = bpp / 8;
	_bytesPerPel = bpp / 8; // == 3 ? 4 : tgaBytesPerPel;	// DWORD align if 24-bit.
	_bits = new void* [ _maxMipCount ];
	MemSet( _bits, 0, _maxMipCount * sizeof( void* ) );

	unsigned int imgStride = _bytesPerPel * _width;
	unsigned int imageSize = _height * imgStride;

	// skip any extra characters in the identification field.
	reader.Skip( header.idlen );

	// allocate the top level.
	unsigned char* dst = 0;
	_bits[ 0 ] = new unsigned char [ imageSize ];

	// is the image compressed?
	if ( ( header.imtype & 8 ) == 0 )
	{
		// load an uncompressed image.
		reader.ReadBytes( _bits[ 0 ], imageSize );
/*
		unsigned char* src = 0;
		unsigned char* pixelRow = ( unsigned char* )alloca( tgaStride );
		for ( unsigned int y = 0; y < _height; ++y )
		{
			dst = ( unsigned char* )_bits[ 0 ] + y * imgStride;
			src = pixelRow;
			reader.ReadBytes( pixelRow, tgaStride );
			for ( unsigned int x = 0; x < _width; ++x )
			{
				for ( unsigned int e = 0; e < _bytesPerPel; ++e, ++src, ++dst )
				{
					*dst = *src;
				}
			}
		}
*/
	}
	else
	{
		// load a compressed image.
		dst = ( unsigned char* )_bits[ 0 ];
		unsigned char* end = ( unsigned char* )_bits[ 0 ] + imageSize;
		unsigned int totalBytes = _height * imgStride;
		while ( dst != end )
		{
			// get the current command byte.
			unsigned int cmd = reader.ReadUChar();
			unsigned int len = cmd & 0x7F;

			// is the command an RLE block?
			if ( cmd & 0x80 )
			{
				// read in the pixel value and duplicate it 'len' types.
				unsigned char pel[ 4 ];
				reader.ReadBytes( pel, _bytesPerPel );
				for ( unsigned int i = 0; i <= len; ++i )
					for ( unsigned int e = 0; e < _bytesPerPel; ++e, ++dst )
						*dst = pel[ e ];
			}
			else
			{
				// read in a raw block.
				unsigned int size = len * _bytesPerPel + _bytesPerPel;
				reader.ReadBytes( dst, size );
				dst += size;
			}
		}
	}

	// does the image need to be expanded to 32-bits?


	// do we need to flip the image?
}

//----------------------------------------------------------
unsigned int
GrImage::CalcMaxMipCount() const
{
	unsigned int maxMipCount = 1;
	unsigned int mipWidth = _width;
	unsigned int mipHeight = _height;
	while ( mipWidth != 1 || mipHeight != 1 )
	{
		mipWidth = Max( mipWidth >> 1, 1U );
		mipHeight = Max( mipHeight >> 1, 1U );
		++maxMipCount;
	}

	// return the maximum mip count.
	return maxMipCount;
}

//----------------------------------------------------------
bool
GrImage::IsFloatImage() const
{
//	( _fmt == EF_RGB24 || _fmt == EF_RGBA32 || _fmt == EF_L8 || _fmt == EF_LA16 );
	return ( _fmt == EF_BGR96F || _fmt == EF_BGRA128F || _fmt == EF_LA64F || _fmt == EF_I16F );
}

//----------------------------------------------------------
void
GrImage::EnsureMipLevel( unsigned int level, bool normalMap )
{
	B_ASSERT( level < _maxMipCount );
	if ( _bits[ level ] == 0 )
	{
		// level 0 cannot be null!  If it is, then we don't even have a starting image!
		B_ASSERT( level != 0 );

		// make sure we have the previous mip level available.
		unsigned int prevLevel = level - 1;
		EnsureMipLevel( prevLevel, normalMap );

		// using our current format, we need to determine how to generate the mip level.
		bool bytes = !IsFloatImage();

		// determine which function we should use to build our mipmaps.
		GenMipLevelFunc genMipLevelFunc = GenMipLevelBytes;
		if ( bytes )
		{
			if ( normalMap )
				genMipLevelFunc = GenMipLevelBytesNM;
			else
				genMipLevelFunc = GenMipLevelBytes;
		}
		else
		{
			// not yet implemented.
			B_ASSERT( false );
			B_ERROR( "Support for floating point textures is not yet implemented." );
		}

		// generate the mip level.
		unsigned int prevWidth = Max( _width >> prevLevel, 1U );
		unsigned int prevHeight = Max( _height >> prevLevel, 1U );
		_bits[ level ] = genMipLevelFunc( _bits[ prevLevel ], prevWidth, prevHeight, _elemsPerPixel[ _fmt ] );
		++_mipCount;
	}
}


//**********************************************************
// file local functions.
//**********************************************************

//----------------------------------------------------------
void*
GenMipLevelBytes( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	// make sure incoming dimensions are valid.
	B_ASSERT( inWidth > 0 && inHeight > 0 );
	B_ASSERT( elemsPerPixel <= 4 && elemsPerPixel > 0 );

	// figure out some stuff ahead of time.
	unsigned int inStride = inWidth * elemsPerPixel;
	unsigned int mipWidth = inWidth >> 1;
	unsigned int mipHeight = inHeight >> 1;

	// source read location.
	unsigned char* out = 0;
	unsigned char* dst = 0;
	unsigned char* src = ( unsigned char* )srcIn;

	// check for a 1D image.
	if ( mipWidth == 0 || mipHeight == 0 )
	{
		// one of them can't be 0.
		B_ASSERT( mipWidth != 0 || mipHeight != 0 );

		unsigned int len = ( mipWidth + mipHeight );
		out = dst = new unsigned char[ len * elemsPerPixel ];
		for ( unsigned int i = 0; i < len; ++i, src += elemsPerPixel*2 )
			for ( unsigned int e = 0; e < elemsPerPixel; ++e, ++dst )
				*dst = ( src[ e ] + src[ elemsPerPixel + e ] ) >> 1;
	}
	else
	{
		// allocate memory copy over each line.
		out = dst = new unsigned char[ mipWidth * mipHeight * elemsPerPixel ];
		for ( unsigned int y = 0; y < mipHeight; ++y, src += inStride )
			for ( unsigned int x = 0; x < mipWidth; ++x, src += elemsPerPixel*2 )
				for ( unsigned int e = 0; e < elemsPerPixel; ++e, ++dst )
					*dst = ( src[ e ] + src[ elemsPerPixel + e ] + src[ inStride + e ] + src[ inStride + elemsPerPixel + e ] ) >> 2;
	}

	// return the output image.
	return out;
}

//----------------------------------------------------------
void*
GenMipLevelBytesBorder( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	B_ASSERT( inWidth > 0 && inHeight > 0 );
	B_ASSERT( elemsPerPixel > 0 && elemsPerPixel <= 4 );

	// generate the mip level as usual.
	void* out = GenMipLevelBytes( srcIn, inWidth, inHeight, elemsPerPixel );

	// get the border color by sampling the upper left corner's color.
	unsigned char* src = ( unsigned char* )srcIn;
	unsigned char borderColor[ 4 ];
	for ( unsigned int e = 0; e < elemsPerPixel; ++e )
		borderColor[ e ] = src[ e ];

	// replace the border.
	ReplaceMipBorder( out, inWidth >> 1, inHeight >> 1, elemsPerPixel, borderColor );

	// return the output image.
	return out;
}

//----------------------------------------------------------
void* GenMipLevelBytesBGR15_A1( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	B_ASSERT( false );
	return 0;
}

//----------------------------------------------------------
void* GenMipLevelBytesBorderBGR15_A1( void* src, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	B_ASSERT( false );
	return 0;
}

//----------------------------------------------------------
void*
GenMipLevelBytesNM( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	// make sure incoming dimensions are valid.
	B_ASSERT( inWidth > 0 && inHeight > 0 );
	B_ASSERT( elemsPerPixel == 3 || elemsPerPixel == 4 );

	// figure out some stuff ahead of time.
//	unsigned int elemCount = _elemsPerPixel[ _fmt ];
	unsigned int inStride = inWidth * elemsPerPixel;
	unsigned int mipWidth = inWidth >> 1;
	unsigned int mipHeight = inHeight >> 1;

	// source read location.
	unsigned char* out = 0;
	unsigned char* dst = 0;
	unsigned char* src = ( unsigned char* )srcIn;

	// z-up normal maps.
	const MVec3 up( 0.0f, 0.0f, 1.0f );
	const float kWeightFudge = 20.0f;

	// check for a 1D image.
	if ( mipWidth == 0 || mipHeight == 0 )
	{
		// one of them can't be 0.
		B_ASSERT( mipWidth != 0 || mipHeight != 0 );

		unsigned int len = ( mipWidth + mipHeight );	// largest dimension.
		out = dst = new unsigned char[ len * elemsPerPixel ];
		for ( unsigned int i = 0; i < len; ++i, src += elemsPerPixel*2, dst += elemsPerPixel )
		{
			MVec3 normal0( UNPACK_QFLOAT( src[ 0 ] ), UNPACK_QFLOAT( src[ 1 ] ), UNPACK_QFLOAT( src[ 2 ] ) );
			MVec3 normal1( UNPACK_QFLOAT( src[ elemsPerPixel ] ), UNPACK_QFLOAT( src[ elemsPerPixel+1 ] ), UNPACK_QFLOAT( src[ elemsPerPixel+2 ] ) );
			float weight0 = 1.0f; //1.0f + kWeightFudge * ( 1.0f - up.Dot( normal0 ) );
			float weight1 = 1.0f; //1.0f + kWeightFudge * ( 1.0f - up.Dot( normal1 ) );
			MVec3 avg = ( weight0*normal0 + weight1*normal1 ).Normalized();
			dst[ 0 ] = PACK_QFLOAT( avg.GetX() );
			dst[ 1 ] = PACK_QFLOAT( avg.GetY() );
			dst[ 2 ] = PACK_QFLOAT( avg.GetZ() );

			// average the fourth element if there is one.
			if ( elemsPerPixel == 4 )
				dst[ 3 ] = ( src[ 3 ] + src[ elemsPerPixel + 3 ] ) >> 1;
		}
	}
	else
	{
		// allocate memory.
		out = dst = new unsigned char[ mipWidth * mipHeight * elemsPerPixel ];

		// for each line in the dest.
		for ( unsigned int y = 0; y < mipHeight; ++y, src += inStride )
			for ( unsigned int x = 0; x < mipWidth; ++x, src += elemsPerPixel*2, dst += elemsPerPixel )
			{
				// compute the average normal and store it.
				MVec3 normal0( UNPACK_QFLOAT( src[ 0 ] ), UNPACK_QFLOAT( src[ 1 ] ), UNPACK_QFLOAT( src[ 2 ] ) );
				MVec3 normal1( UNPACK_QFLOAT( src[ elemsPerPixel ] ), UNPACK_QFLOAT( src[ elemsPerPixel+1 ] ), UNPACK_QFLOAT( src[ elemsPerPixel+2 ] ) );
				MVec3 normal2( UNPACK_QFLOAT( src[ inStride ] ), UNPACK_QFLOAT( src[ inStride+1 ] ), UNPACK_QFLOAT( src[ inStride+2 ] ) );
				MVec3 normal3( UNPACK_QFLOAT( src[ inStride+elemsPerPixel ] ), UNPACK_QFLOAT( src[ inStride+elemsPerPixel+1 ] ), UNPACK_QFLOAT( src[ inStride+elemsPerPixel+2 ] ) );
				float weight0 = 1.0f; //+ kWeightFudge * ( 1.0f - up.Dot( normal0 ) );
				float weight1 = 1.0f; //+ kWeightFudge * ( 1.0f - up.Dot( normal1 ) );
				float weight2 = 1.0f; //+ kWeightFudge * ( 1.0f - up.Dot( normal2 ) );
				float weight3 = 1.0f; //+ kWeightFudge * ( 1.0f - up.Dot( normal3 ) );
				MVec3 avg = ( weight0*normal0 + weight1*normal1 + normal2 + normal3 ).Normalized();
				dst[ 0 ] = PACK_QFLOAT( avg.GetX() );
				dst[ 1 ] = PACK_QFLOAT( avg.GetY() );
				dst[ 2 ] = PACK_QFLOAT( avg.GetZ() );

				// average the fourth element if there is one.
				if ( elemsPerPixel == 4 )
					dst[ 3 ] = ( src[ 3 ] + src[ elemsPerPixel + 3 ] + src[ inStride + 3 ] + src[ inStride + elemsPerPixel + 3 ] ) >> 2;
			}
	}

	// return the output image.
	return out;
}

//----------------------------------------------------------
void*
GenMipLevelBytesNMBorder( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	B_ASSERT( inWidth > 0 && inHeight > 0 );
	B_ASSERT( elemsPerPixel == 3 || elemsPerPixel == 4 );

	// generate the mip level as usual.
	void* out = GenMipLevelBytesNM( srcIn, inWidth, inHeight, elemsPerPixel );

	// get the border color by sampling the upper left corner's color.
	unsigned char* src = ( unsigned char* )srcIn;
	unsigned char borderNormal[ 4 ];
	for ( unsigned int e = 0; e < elemsPerPixel; ++e )
		borderNormal[ e ] = src[ e ];
	
	// replace the border.
	ReplaceMipBorder( out, inWidth >> 1, inHeight >> 1, elemsPerPixel, borderNormal );

	// return the output image.
	return out;
}

//----------------------------------------------------------
void*
GenMipLevelFloats( void* srcIn, unsigned int inWidth, unsigned int inHeight, unsigned int elemsPerPixel )
{
	B_ASSERT( false );
	return 0;
/*
	// make sure incoming dimensions are valid.
	B_ASSERT( inWidth > 0 && inHeight > 0 );

	// figure out some stuff ahead of time.
	unsigned int elemCount = _elemsPerPixel[ _fmt ];
	unsigned int elemCount2x = elemCount << 1;
	unsigned int mipWidth = Max( inWidth >> 1, 1U );
	unsigned int mipHeight = Max( inHeight >> 1, 1U );
	unsigned int elemWidth = mipWidth * elemCount;
	unsigned int inElemWidth = inWidth * elemCount;
	unsigned int size = mipHeight * elemWidth;
	float* src = ( float* )srcIn;
	float* dest = new float[ size ];

	// for each line in the dest.
	for ( unsigned int lineStart = 0, y0 = 0, y1 = inElemWidth; lineStart < size; lineStart += elemWidth, y0 += inElemWidth, y1 += inElemWidth )
	{
		// for each pixel in the dest.
		for ( unsigned int x = 0, x0 = 0, x1 = elemCount; x < elemWidth; x += elemCount, x0 += elemCount2x, x1 += elemCount2x )
		{
			// for each pixel component.
			for ( unsigned int e = 0; e < elemCount; ++e )
				dest[ lineStart + x + e ] += ( src[ y0 + x0 + e ] + src[ y0 + x1 + e ] + src[ y1 + x0 + e ] + src[ y1 + x1 + e ] ) * 0.25f;
		}
	}
	return dest;
*/
}

static void ReplaceMipBorder( void* data, unsigned int mipWidth, unsigned int mipHeight, unsigned int elemsPerPixel, unsigned char* borderColor )
{
	// get pointers to work with.
	unsigned char* dst = ( unsigned char* )data;
	if ( mipWidth == 0 || mipHeight == 0 )
	{
		// take non-zero dimension.
		unsigned int len = mipWidth + mipHeight;

		// replace all texels with the border color.
		for ( unsigned int i = 0; i < len; ++i, dst += elemsPerPixel )
			for ( unsigned int e = 0; e < elemsPerPixel; ++e )
				dst[ e ] = borderColor[ e ];
	}
	else
	{
		// calculate the stride.
		unsigned int mipStride = mipWidth * elemsPerPixel;

		// replace the first row with the border color.
		for ( unsigned int x = 0; x < mipWidth; ++x, dst += elemsPerPixel )
			for ( unsigned int e = 0; e < elemsPerPixel; ++e )
				dst[ e ] = borderColor[ e ];

		// replace the last row with the border color.
		dst = ( ( unsigned char* )data ) + ( mipHeight - 1 ) * mipStride;
		for ( unsigned int x = 0; x < mipWidth; ++x, dst += elemsPerPixel )
			for ( unsigned int e = 0; e < elemsPerPixel; ++e )
				dst[ e ] = borderColor[ e ];

		// replace the sides with the border color.
		unsigned char* dstLeft = ( ( unsigned char* )data ) + mipStride;
		unsigned char* dstRight = ( ( unsigned char* )data ) + ( mipStride << 1 ) - elemsPerPixel;
		unsigned int lastRow = ( mipHeight - 1 );
		for ( unsigned int y = 1; y < lastRow; ++y, dstLeft += mipStride, dstRight += mipStride )
		{
			for ( unsigned int e = 0; e < elemsPerPixel; ++e )
			{
				dstLeft[ e ] = borderColor[ e ];
				dstRight[ e ] = borderColor[ e ];
			}
		}
	}
}
