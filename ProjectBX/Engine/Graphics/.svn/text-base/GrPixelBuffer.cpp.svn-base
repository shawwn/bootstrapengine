//----------------------------------------------------------
// File:		GrPixelBuffer.cpp
// Author:		Kevin Bray
// Created:		04-11-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPixelBuffer.h"

// project includes.
#include "GrTexture.h"
#include "GrTextureRect.h"
#include "GrUtil.h"
#include "GLSubsys.h"

// format conversion map.
static GrPixelBuffer::EFORMAT _formatMap[ GrTextureBase::EF_COUNT ] = {
	GrPixelBuffer::EF_DXT1,		// GrTextureBase::EF_DXT1
	GrPixelBuffer::EF_DXT5,		// GrTextureBase::EF_DXT5
	GrPixelBuffer::EF_DXT5R,	// GrTextureBase::EF_DXT5R
	GrPixelBuffer::EF_BGR16,	// GrTextureBase::EF_RGB16
	GrPixelBuffer::EF_BGRX32,	// GrTextureBase::EF_RGB24
	GrPixelBuffer::EF_BGR15_A1,	// GrTextureBase::EF_RGB15_A1
	GrPixelBuffer::EF_BGRA32,	// GrTextureBase::EF_RGBA32
	GrPixelBuffer::EF_RGBX64F,	// GrTextureBase::EF_RGB48F
	GrPixelBuffer::EF_RGBA64F,	// GrTextureBase::EF_RGBA64F
	GrPixelBuffer::EF_RGBA128F,	// GrTextureBase::EF_RGBA128F
	GrPixelBuffer::EF_R8,		// GrTextureBase::EF_R8
	GrPixelBuffer::EF_RG16,		// GrTextureBase::EF_RG16
	GrPixelBuffer::EF_L8,		// GrTextureBase::EF_L8
	GrPixelBuffer::EF_I8,		// GrTextureBase::EF_I8
	GrPixelBuffer::EF_INVALID,	// GrTextureBase::EF_I16F
	GrPixelBuffer::EF_A8,		// GrTextureBase::EF_A8
	GrPixelBuffer::EF_LA16,		// GrTextureBase::EF_LA16
	GrPixelBuffer::EF_INVALID,	// GrTextureBase::EF_LA32F
	GrPixelBuffer::EF_BGRA32,	// GrTextureBase::EF_NORMALMAP
	GrPixelBuffer::EF_RGBA64F,	// GrTextureBase::EF_HIGHDEFNORMALMAP
	GrPixelBuffer::EF_INVALID,	// GrTextureBase::EF_DEPTH
	GrPixelBuffer::EF_INVALID,	// GrTextureBase::EF_DEPTHSTENCIL
	GrPixelBuffer::EF_INVALID,	// GrTextureBase::EF_GUESS
};

// format pixel size map.
static unsigned int _pixelSizeMap[ GrPixelBuffer::EF_INVALID ] = {
	8,			// EF_DXT1,
	16,			// EF_DXT5,
	8,			// EF_DXT5R,
	2,			// EF_BGR16,
	2,			// EF_BGR15_A1,
	4,			// EF_BGRX32,
	4,			// EF_BGRA32,
	8,			// EF_RGBX64F,
	8,			// EF_RGBA64F,
	16,			// EF_RGBA128F,
	1,			// EF_R8,
	2,			// EF_RG16
	1,			// EF_L8,
	1,			// EF_I8,
	1,			// EF_A8,
	2,			// EF_LA16,f
};

// format type map.
static unsigned int _rawPixelFormatMap[ GrPixelBuffer::EF_INVALID ] = {
	0,						// EF_DXT1,
	0,						// EF_DXT5,
	0,						// EF_DXT5R,
	0,						// EF_BGR16,
	0,						// EF_BGR15_A1,
	0,						// EF_BGRX32,
	GL_BGRA,				// EF_BGRA32,
	0,						// EF_RGBX64F,
	GL_RGBA,				// EF_RGBA64F,
	GL_RGBA,				// EF_RGBA128F,
	GL_RED,					// EF_R8,
	GL_RG,					// EF_RG16,
	GL_LUMINANCE,			// EF_L8,
	GL_RED,					// EF_I8,
	GL_ALPHA,				// EF_A8,
	GL_LUMINANCE_ALPHA,		// EF_LA16,
};

static unsigned int _rawPixelFormatTypeMap[ GrPixelBuffer::EF_INVALID ] = {
	0,						// EF_DXT1,
	0,						// EF_DXT5,
	0,						// EF_DXT5R
	0,						// EF_BGR16,
	0,						// EF_BGR15_A1,
	0,						// EF_BGRX32,
	GL_UNSIGNED_BYTE,		// EF_BGRA32,
	0,						// EF_RGBX64F,
	GL_HALF_FLOAT_ARB,		// EF_RGBA64F,
	GL_FLOAT,				// EF_RGBA128F,
	GL_UNSIGNED_BYTE,		// EF_R8,
	GL_UNSIGNED_BYTE,		// EF_RG16,
	GL_UNSIGNED_BYTE,		// EF_L8,
	GL_UNSIGNED_BYTE,		// EF_I8,
	GL_UNSIGNED_BYTE,		// EF_A8,
	GL_UNSIGNED_BYTE,		// EF_LA16,
};

static unsigned int _usageMap[] = {
	GL_STATIC_DRAW,
	GL_STREAM_DRAW,
	GL_DYNAMIC_DRAW,
	GL_STATIC_READ,
	GL_STREAM_READ,
	GL_DYNAMIC_READ,
};

//**********************************************************
// class GrPixelBuffer
//**********************************************************

//==========================================================
// static methods
//==========================================================

//----------------------------------------------------------
GrPixelBuffer::EFORMAT
GrPixelBuffer::GetMatchingFormat( GrTextureBase::EFORMAT textureFormat )
{
	return _formatMap[ textureFormat ];
}


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPixelBuffer::GrPixelBuffer( unsigned int width, unsigned int height, EFORMAT format, EUSAGE usage )
: _glHandle( 0 )
, _width( width )
, _height( height )
, _size( 0 )
, _resetStorage( 0 )
, _format( format )
, _usage( usage )
, _mapped( false )
{
	// make sure the width and height are valid.
	B_ASSERT( width > 0 && height > 0 );

	// ensure that the usage is valid for the given format.
	if ( usage == EU_READBACK || usage == EU_READBACK_DISCARD || usage == EU_READBACK_STREAM )
	{
		// the format requested is not valid for readback!
		B_ASSERT( _rawPixelFormatMap[ format ] != 0 );
	}

	// calculate the buffer size.
	if ( _format == EF_DXT1 || _format == EF_DXT5 )
	{
		// make sure the width and height are both multiples of 4.
		B_ASSERT( ( width & 3 ) == 0 && ( height & 3 ) == 0 );

		// calculate the total size for compressed formats.
		unsigned int blockSize = _pixelSizeMap[ format ];
		_size = ( width / 4 ) * ( height / 4 );
		_size *= blockSize;
	}
	else
	{
		// calculate the size for uncompressed data.
		_size = width * height * _pixelSizeMap[ format ];
	}

	// generate a new buffer handle.
	AllocBuffer( NULL );
}

//----------------------------------------------------------
GrPixelBuffer::~GrPixelBuffer()
{
	B_ASSERT( !_mapped );
	GrDeletePB( _glHandle );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPixelBuffer::ReadPixels( unsigned int srcX, unsigned int srcY ) const
{
	// make sure the buffer isn't mapped!
	B_ASSERT( !_mapped );
	
	// verify that OpenGL is in a valid state going in and coming out
	// of the readpixels call.
	CHECK_GL();
	GrBindPackPB( _glHandle );
	unsigned int glFormat = _rawPixelFormatMap[ _format ];
	unsigned int glType = _rawPixelFormatTypeMap[ _format ];
	bglReadPixels( srcX, srcY, _width, _height, glFormat, glType, ( ( unsigned char* )0 ) );
	GrBindPackPB( 0 );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrPixelBuffer::UploadTexData( GrTexture* dst, unsigned int dstLevel, unsigned int dstX, unsigned int dstY ) const
{
	CHECK_GL();
	GrBindUnpackPB( _glHandle );
	unsigned int glFormat = _rawPixelFormatMap[ _format ];
	unsigned int glType = _rawPixelFormatTypeMap[ _format ];
	dst->UploadPixelData( dstLevel, dstX, dstY, _width, _height, ( ( unsigned char* )0 ), glFormat, glType );
	GrBindUnpackPB( 0 );
}

//----------------------------------------------------------
void
GrPixelBuffer::UploadTexData( GrTextureRect* dst, unsigned int dstLevel, unsigned int dstX, unsigned int dstY ) const
{
	CHECK_GL();
	GrBindUnpackPB( _glHandle );
	unsigned int glFormat = _rawPixelFormatMap[ _format ];
	unsigned int glType = _rawPixelFormatTypeMap[ _format ];
	dst->UploadPixelData( dstX, dstY, _width, _height, ( ( unsigned char* )0 ), glFormat, glType );
	GrBindUnpackPB( 0 );
}

//----------------------------------------------------------
void*
GrPixelBuffer::Map()
{
	if ( _usage == EU_READBACK || _usage == EU_READBACK_DISCARD )
		return Map( true, false );
	return Map( false, true );
}

//----------------------------------------------------------
void*
GrPixelBuffer::Map( bool read, bool write )
{
	// ensure that either read or write is mapped.
	B_ASSERT( read || write );

	// ensure that the buffer isn't already mapped!
	B_ASSERT( !_mapped );
	_mapped = true;

	// make sure everything is well in OpenGL land.
	CHECK_GL();

	// calculate read/write flags.
	GLenum flags = 0;
	if ( read && write )
		flags = GL_READ_WRITE;
	else if ( read )
		flags = GL_READ_ONLY;
	else
		flags = GL_WRITE_ONLY;

	// bind the buffer and map it.
	void* mapPtr = 0;
	if ( _usage == EU_READBACK || _usage == EU_READBACK_DISCARD || _usage == EU_READBACK_STREAM )
	{
		// bind to the pack target for read buffers.
		GrBindPackPB( _glHandle );
		mapPtr = bglMapBuffer( GL_PIXEL_PACK_BUFFER, flags );
		GrBindPackPB( 0 );
	}
	else
	{
		// bind to the unpack target for write buffers.
		unsigned int prevPB = GrGetUnpackPB();
		GrBindUnpackPB( _glHandle );
		mapPtr = bglMapBuffer( GL_PIXEL_UNPACK_BUFFER, flags );
		GrBindUnpackPB( 0 );
	}

	// verify that the buffer was mapped successfully.
	CHECK_GL();

	// return the pointer to the mapped data.
	return mapPtr;
}

//----------------------------------------------------------
void
GrPixelBuffer::Unmap()
{
	// the buffer must be mapped in order to call this function!
	B_ASSERT( _mapped );
	_mapped = false;

	// unmap the buffer.
	if ( _usage == EU_READBACK || _usage == EU_READBACK_DISCARD )
	{
		// bind to the pack target for read buffers.
		GrBindPackPB( _glHandle );
		bglUnmapBuffer( GL_PIXEL_PACK_BUFFER );
		GrBindPackPB( 0 );
	}
	else
	{
		// bind to the unpack target for write buffers.
		GrBindUnpackPB( _glHandle );
		bglUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
		GrBindUnpackPB( 0 );
	}
}

//----------------------------------------------------------
void
GrPixelBuffer::PreContextReset()
{
	// do we need to preserve data?
	_resetStorage = new char [ _size ];
	void* data = Map();
	if ( data )
		MemCopy( _resetStorage, data, _size );
	Unmap();

	B_ASSERT( !_mapped );
	GrDeletePB( _glHandle );
	_glHandle = 0;
}

//----------------------------------------------------------
void
GrPixelBuffer::PostContextReset()
{
	AllocBuffer( _resetStorage );
	delete[] ( char* )_resetStorage;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPixelBuffer::AllocBuffer( void* data )
{
	B_ASSERT( _size > 0 );
	B_ASSERT( _glHandle == 0 );

	// generate the handle.
	_glHandle = GrGenPB();

	// now build the buffer object.
	if ( _usage == EU_READBACK || _usage == EU_READBACK_DISCARD )
	{
		GrBindPackPB( _glHandle );
		bglBufferData( GL_PIXEL_PACK_BUFFER_ARB, _size, data, _usageMap[ _usage ] );
		GrBindPackPB( 0 );
	}
	else
	{
		GrBindUnpackPB( _glHandle );
		bglBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, _size, data, _usageMap[ _usage ] );
		GrBindUnpackPB( 0 );
	}
	CHECK_GL();
}
