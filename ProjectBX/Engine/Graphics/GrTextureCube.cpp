//----------------------------------------------------------
// File:		GrTextureCube.cpp
// Author:		Kevin Bray
// Created:		07-14-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTextureCube.h"

// project includes.
#include "GrImage.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "RRsrcMgr.h"

const GLenum cubeFaceMap[] = 
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};


//**********************************************************
// class GrTextureCube
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureCube::GrTextureCube( const UPath& name, GrTextureBase::EFORMAT format, unsigned int width, unsigned int flags )
: GrTextureBase( name, GL_TEXTURE_CUBE_MAP, format, flags, ETT_CUBE )
, _width( width )
, _fromFile( false )
, _ownImages( false )
{
	MemSet( _images, 0, sizeof( _images ) );
	Create();
}

//----------------------------------------------------------
GrTextureCube::GrTextureCube( const UPath& name, const UPath** images, GrTextureBase::EFORMAT format, unsigned int flags,
							  bool cacheImages )
: GrTextureBase( name, GL_TEXTURE_CUBE_MAP, format, flags, ETT_CUBE )
, _width( 0 )
, _fromFile( true )
, _ownImages( cacheImages )
{
	// make sure that the images passed in form a valid image set.
	B_ASSERT( images );
	B_ASSERT( images[ 0 ] != 0 );

	// copy the resource paths.
	for ( unsigned int face = 0; face < 6; ++face )
		_rsrcPaths[ face ] = *images[ face ];

	// store the images.
	if ( cacheImages )
	{
		for ( unsigned int face = 0; face < 6; ++face )
			_images[ face ] = LoadImg( _rsrcPaths[ face ] );
	}

	Create();
}

//----------------------------------------------------------
GrTextureCube::GrTextureCube( const UPath& name, GrImage** images, GrTextureBase::EFORMAT format, unsigned int flags )
: GrTextureBase( name, GL_TEXTURE_CUBE_MAP, format, flags, ETT_CUBE )
, _width( 0 )
, _fromFile( false )
, _ownImages( true )
{
	// make sure that the images passed in form a valid image set.
	B_ASSERT( images );
	B_ASSERT( images[ 0 ] != 0 );
	B_ASSERT( IsValidImageSet( images ) );

	// get the width of the cube.
	_width = images[ 0 ]->GetWidth();

	// store the images.
	for ( unsigned int face = 0; face < 6; ++face )
		_images[ face ] = images[ face ];

	Create();
}

//----------------------------------------------------------
GrTextureCube::~GrTextureCube()
{
	CHECK_GL();
	for ( unsigned int i = 0; i < 6; ++i )
		delete _images[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrTextureCube::IsValidImageSet( GrImage** images )
{
	B_ASSERT( images );
	B_ASSERT( images[ 0 ] != 0 );

	// get info about the first image.
	unsigned int width = images[ 0 ]->GetWidth();
	bool hasMipMaps = images[ 0 ]->HasMipMaps();
	GrImage::EFORMAT format = images[ 0 ]->GetFormat();

	// make sure formats and mipmap counts match up.
	for ( unsigned int face = 1; face < 6; ++face )
	{
		if ( width != images[ face ]->GetWidth() )
			return false;
		if ( images[ face ]->GetWidth() != images[ face ]->GetHeight() )
			return false;
		if ( format != images[ face ]->GetFormat() )
			return false;
		if ( images[ face ]->HasMipMaps() != hasMipMaps )
			return false;
	}

	// valid!
	return true;
}

//----------------------------------------------------------
void
GrTextureCube::BindToFramebuffer( unsigned int attachment, ECUBEFACE cubeFace )
{
	// this is the only thing really supported at the moment.
	B_ASSERT( attachment == GL_COLOR_ATTACHMENT0_EXT );

	// bind the root mipmap level.
	bglFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, cubeFaceMap[ cubeFace ], GetTexHandle(), 0 );
	CHECK_GL();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTextureCube::UploadImage( bool mipmap )
{
	// make sure everything is okay.
	CHECK_GL();

	// ensure that a texture handle has been reserved.
	B_ASSERT( GetTexHandle() != 0 );

	for ( unsigned int face = 0; face < 6; ++face )
	{

		// get the image we need to upload.
		GrImage* image = 0;
		if ( _ownImages )
		{
			image = _images[ face ];
		}
		else if ( _fromFile )
		{
			image = LoadImg( _rsrcPaths[ face ] );
		}

		// get the internal format for the texture.
		unsigned int internalFormat = GetGLFormat( image );

		// upload an image if necessary.
		if ( image != 0 )
		{
			// ensure that the image is a power of 2.
			if ( !IsPow2( _width ) )
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
				// if we don't have mipmaps, determine whether or not we need to generate them
				// or turn on automatic mipmap generation.
				if ( !image->HasMipMaps() )
				{
					// generate mip levels for the cube map.
					image->GenMipMaps( isNormalMap );
				}
			}

			// if we have a normal map, swizzle the image to XGBR.
			if ( isNormalMap )
				image->SwizzleToBXRG();

			// get the mipmaps from the image.
			image->TexImage2D( cubeFaceMap[ face ], internalFormat, baseMipLevel, mipmap ? 0 : 1, isNormalMap );
		}
		else
		{
			if ( mipmap )
			{
				unsigned int mipLevel = 0;
				unsigned int mipDim = _width;
				while ( mipDim != 1 )
				{
					bglTexImage2D( cubeFaceMap[ face ], mipLevel, internalFormat, mipDim, mipDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
					mipDim >>= 1;
					++mipLevel;
				}
			}
			else
			{
				bglTexImage2D( cubeFaceMap[ face ], 0, internalFormat, _width, _width, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			}
		}
		CHECK_GL();
	}
}

//----------------------------------------------------------
GrImage*
GrTextureCube::LoadImg( const UPath& fileName )
{
	return CreateImage( fileName );
}
