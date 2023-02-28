//----------------------------------------------------------
// File:		GrTexture.cpp
// Author:		Kevin Bray
// Created:		03-28-05	
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTexture.h"

// project includes.
#include "GrImage.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "RRsrcMgr.h"
#include "GrTextureMgr.h"
#include "RFileMgr.h"
#include "GrDebug.h"

unsigned int _baseMipLevel[ EGQ_COUNT ] =
{
	0,	// ultra
	0,	// high
	1,	// medium
	1,	// low
};

#define AUTO_GEN_MIPMAPS 0

//**********************************************************
// class GrTexture
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTexture::GrTexture( const UPath& name, EFORMAT format, unsigned int width, unsigned int height,
					  unsigned int flags )
: GrTextureBase( name, GL_TEXTURE_2D, format, flags, ETT_2D )
, _width( width )
, _height( height )
, _ownedImage( 0 )
, _hwShadowEnable( true )
, _fromFile( false )
, _needSwizzle( true )
, _valid( true )
{
	// allocate a texture object and upload data.
	Create();
}

//----------------------------------------------------------
GrTexture::GrTexture( const UPath& name, const UPath& rsrcName, EFORMAT format, unsigned int flags,
					  bool cacheImage )
: GrTextureBase( name, GL_TEXTURE_2D, format, flags, ETT_2D )
, _rsrcPath( rsrcName )
, _width( 1 )
, _height( 1 )
, _ownedImage( 0 )
, _hwShadowEnable( true )
, _fromFile( true )
, _needSwizzle( true )
, _valid( true )
{
	if ( cacheImage )
	{
		_ownedImage = LoadImg();
		if ( _ownedImage )
		{
			_width = _ownedImage->GetWidth();
			_height = _ownedImage->GetHeight();
		}
	}

	Create();
}

//----------------------------------------------------------
GrTexture::GrTexture( const UPath& name, GrImage* image, EFORMAT format, unsigned int flags )
: GrTextureBase( name, GL_TEXTURE_2D, format, flags, ETT_2D )
, _width( 1 )
, _height( 1 )
, _ownedImage( image )
, _hwShadowEnable( true )
, _fromFile( false )
, _needSwizzle( true )
, _valid( true )
{
	if ( image != 0 )
	{
		_width = image->GetWidth();
		_height = image->GetHeight();
	}
	Create();
}

//----------------------------------------------------------
GrTexture::~GrTexture()
{
	delete _ownedImage;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTexture::OwnedImageChanged()
{
	// must be from an owned image!
	B_ASSERT( _ownedImage != 0 );

	// bind to a texture unit and upload.
	Bind( 0 );
	UploadImage( ( GetFlags() & kNoMipMap ) == 0 );
}

//----------------------------------------------------------
void
GrTexture::SetHWShadowEnable( bool enable )
{
	// simply ignore if we're not of the appropriate format.
	if ( GetFormat() != EF_DEPTH && GetFormat() != EF_DEPTHSTENCIL )
	{
		// this is only a valid function call for depth textures!
		B_ASSERT( false );
		return;
	}

	if ( _hwShadowEnable != enable )
	{
		if ( enable )
			bglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
		else
			bglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );
	}
	_hwShadowEnable = enable;
}

//----------------------------------------------------------
void
GrTexture::BindToFramebuffer( unsigned int attachment )
{
	CHECK_GL();

	B_ASSERT( GetTexHandle() != 0 );
	bglFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, GetTexHandle(), 0 );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrTexture::UploadPixelData( int dstLevel, int dstX, int dstY, int width, int height, const void* srcPels,
							GLenum srcFormat, GLenum srcType )
{
	// currently, we do not support compressed textures!  In the future,
	// we need to either add another function to upload compressed data
	// or compress the incoming data automatically on the fly (or both?).
	B_ASSERT( GetFormat() != EF_DXT1 && GetFormat() != EF_DXT5 && GetFormat() != EF_DXT5R );
	B_ASSERT( GetGLFormat( 0 ) != GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
	B_ASSERT( GetGLFormat( 0 ) != GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
	B_ASSERT( GetGLFormat( 0 ) != GL_COMPRESSED_RED_RGTC1 );

	// bind to a texture unit and upload data.
	Bind( 0 );
	bglTexSubImage2D( GL_TEXTURE_2D, dstLevel, dstX, dstY, width, height, srcFormat, srcType, srcPels );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrTexture::UploadPixelDataCompressed( int dstLevel, int dstX, int dstY, int width, int height, const void* srcPels,
									  GLenum srcFormat, unsigned int srcDataSize )
{
	B_ASSERT( GetFormat() == EF_DXT1 || GetFormat() == EF_DXT5 || GetFormat() == EF_DXT5R );
	B_ASSERT( GetGLFormat( 0 ) == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || 
			  GetGLFormat( 0 ) == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT || 
			  GetGLFormat( 0 ) == GL_COMPRESSED_RED_RGTC1 );

	// bind to a texture unit and upload data.
	Bind( 0 );
	bglCompressedTexSubImage2D( GL_TEXTURE_2D, dstLevel, dstX, dstY, width, height, srcFormat, srcDataSize, srcPels );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrTexture::DebugSave( const char* fileName )
{
	// save the texture data to a targa.
	Bind( 0 );
	char* pels = new char[ _width * _height * 4 ];
	bglGetTexImage( GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pels );
	GrSaveTGA( fileName, pels, _width, _height, 4 );
	delete[] pels;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTexture::UploadImage( bool mipmap )
{
	// make sure everything is okay.
	CHECK_GL();

	// ensure that a texture handle has been reserved.
	B_ASSERT( GetTexHandle() != 0 );

	// get the image we need to upload.
	GrImage* image = 0;
	if ( _ownedImage != 0 )
	{
		image = _ownedImage;
	}
	else if ( _fromFile )
	{
		image = LoadImg();
		B_ASSERT( image != 0 );
		if ( image == 0 )
			HandleError( "%s, line %d:\n\nError loading image '%s'!", __FILE__, __LINE__, _rsrcPath.GetPathString().c_str() );

		_width = image->GetWidth();
		_height = image->GetHeight();
	}

	if ( ( GetFormat() == EF_DEPTH || GetFormat() == EF_DEPTHSTENCIL ) && _hwShadowEnable )
	{
		// HW shadow mapping compare mode defaults to disabled.
		if ( _hwShadowEnable )
			bglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
		else
			bglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );
		bglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );
		bglTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE );
	}

	// get the internal format we should use.
	unsigned int internalFormat = GetGLFormat( image );

	// upload an image if necessary.
	if ( image != 0 )
	{
		// ensure that the image is a power of 2.
		if ( !IsPow2( _width ) || !IsPow2( _height ) )
		{
			B_ERROR( "Unsupported texture image: dimensions must be a power of 2." );
		}

		// if we're an alpha format, convert the image to alpha before uploading.  Also,
		// if we're a luminance texture, make sure the image is a luminance image.
		bool isNormalMap = false;
		EFORMAT format = GetFormat();
		if ( format == EF_A8 || format == EF_L8 )
			image->ConvertToI8();
		else if ( format == EF_LA16 )
		{
			if ( image->GetFormat() == GrImage::EF_BGRA32 )
				image->ConvertToLA16();
			else if ( image->GetFormat() == GrImage::EF_BGR24 )
				image->ConvertToI8();
		}
		else if ( format == EF_NORMALMAP || format == EF_HIGHDEFNORMALMAP )
			isNormalMap = true;

		// do we need to generate mipmaps?
		unsigned int baseMipLevel = 0;
		if ( mipmap )
		{
			if ( image->GetWidth() > 128 && image->GetHeight() > 128 )
				baseMipLevel = _baseMipLevel[ gGrTextureMgr->GetTexturingQuality() ];

			// generate custom mip levels if we have a normal map.
			if ( isNormalMap )
			{
				image->GenMipMaps( isNormalMap );
			}
			else
			{
#if AUTO_GEN_MIPMAPS
				// since we're not a normal map, and no mipmaps were supplied, turn on
				// automatic mipmap generation.
				bglTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
#else
				image->GenMipMaps( false );
#endif
			}
		}

		// if we have a normal map and we don't own the image, swizzle it to XBGR.
		if ( isNormalMap && ( !_ownedImage || _needSwizzle ) )
		{
			image->SwizzleToBXRG();
			_needSwizzle = false;
		}

		// just let the driver do the texture compression for us.
#if 0
		// determine whether or not we should compress the image.
		if ( _ownedImage == 0 )
		{
			if ( internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT )
				image->CompressDXT1( isNormalMap );
			else if ( internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
				image->CompressDXT5( isNormalMap );
		}
#endif

		// get the mipmaps from the image.
#if AUTO_GEN_MIPMAPS
		if ( WasCreated() )
			image->TexSubImage2D( GL_TEXTURE_2D, internalFormat, baseMipLevel, isNormalMap ? 0 : 1, isNormalMap );
		else
			image->TexImage2D( GL_TEXTURE_2D, internalFormat, baseMipLevel, isNormalMap ? 0 : 1, isNormalMap );
#else
		// upload the image as compressed if it's compressed.
		if ( image->IsCompressed() )
		{
			image->CompressedTexImage2D( GL_TEXTURE_2D, internalFormat, baseMipLevel, 0, isNormalMap );
		}
		else
		{
			if ( WasCreated() )
				image->TexSubImage2D( GL_TEXTURE_2D, internalFormat, baseMipLevel, 0, isNormalMap );
			else
				image->TexImage2D( GL_TEXTURE_2D, internalFormat, baseMipLevel, 0, isNormalMap );
		}
#endif
		CHECK_GL();
	}
	else
	{
		unsigned int dataFmt = ( ( GetFormat() == EF_DEPTH || GetFormat() == EF_DEPTHSTENCIL ) ? GL_DEPTH_COMPONENT : GL_RGBA );
		if ( mipmap )
		{
			// turn on automatic texture generation if mipmapping is enabled for imageless
			// textures.
			bglTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );

			unsigned int mipLevel = 0;
			unsigned int mipWidth = _width;
			unsigned int mipHeight = _height;
			while ( mipWidth > 0 || mipHeight > 0 )
			{
				mipWidth = Max( mipWidth, 1U );
				mipHeight = Max( mipHeight, 1U );
				bglTexImage2D( GL_TEXTURE_2D, mipLevel, internalFormat, mipWidth, mipHeight, 0, dataFmt, GL_UNSIGNED_BYTE, 0 );
				CHECK_GL();
				mipWidth >>= 1;
				mipHeight >>= 1;
				++mipLevel;
			}
		}
		else
		{
			bglTexImage2D( GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, dataFmt, GL_UNSIGNED_BYTE, 0 );
			CHECK_GL();
		}
	}

	// delete the image if necessary.
	if ( !_ownedImage && _fromFile )
		delete image;
}

//----------------------------------------------------------
GrImage*
GrTexture::LoadImg()
{
	return CreateImage( _rsrcPath );
}
