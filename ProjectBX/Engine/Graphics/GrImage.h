//----------------------------------------------------------
// File:		GrImage.h
// Author:		Kevin Bray
// Created:		09-20-04
//
// Purpose:		To encapsulate texturing functionality.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "RRsrc.h"

// std c++ includes.
#include <list>

class UReader;

//**********************************************************
// class GrTexture
//**********************************************************
class GrImage
{
public:
	enum EFORMAT
	{
		EF_BGR24,
		EF_BGRA32,
		EF_BGR96F,
		EF_BGRA128F,
		EF_I8,
		EF_I16F,
		EF_LA16,
		EF_LA64F,
		EF_DEPTH,
		EF_BGR_DXT1,
		EF_BGRA_DXT5,
	};

	GrImage( const UPath& name, EFORMAT format, unsigned int width, unsigned int height, void* data = 0 );
	GrImage( const UPath& name, unsigned long bgraColor );		// single pixel BGRA image.
	GrImage( const UPath& name, UReader& reader );
	GrImage( const GrImage& image );
	~GrImage();

	const UPath&			GetName() const							{	return _name;			}
	EFORMAT					GetFormat();

	unsigned int			GetWidth() const						{	return _width;			}
	unsigned int			GetHeight() const						{	return _height;			}
	unsigned int			GetBytesPerPixel() const				{	return _bytesPerPel;	}
	unsigned int			GetMaxMipCount() const					{	return _maxMipCount;	}
	bool					HasMipMaps() const						{	return _mipCount != 1;	}

	// returns true if the image is in a compressed format.
	bool					IsCompressed() const;

	// converts a luminance image to alpha.  This is a no-op if the image is
	// not EF_L8.  The opposite is true for AlphaToLum.
	void					ConvertToI8( bool keepMipMaps = true );
	void					ConvertToLA16( bool keepMipMaps = true );
	void					LumToAlpha();
	void					AlphaToLum();
//	void					ConvertToLumAlpha();
	void					ConvertBGR24ToBGRA32();
	void					SwizzleToBXRG();		// this is used for DXT5 normal map compression!
	void					CompressDXT1( bool normalMap );
	void					CompressDXT5( bool normalMap );

	// this must be called before calling TexImage2D if mipmaps are desired.
	void					GenMipMaps( bool normalMap );

	// clears created mipmaps.
	void					ClearMipMaps();

	// sends the images (all mip-levels) to the currently bound OpenGL texture.
	void					TexImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
										unsigned int mipCount, bool normalMap );
	void					TexSubImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
										   unsigned int mipCount, bool normalMap );

	// sends the images (all mip-levels) to the currently bound OpenGL texture
	// in compressed form.  Note that the texture must be in a compressed format
	// for this to work.
	void					CompressedTexImage2D( unsigned int bindTarget, unsigned int glTexFormat, unsigned int startMip,
												  unsigned int mipCount, bool normalMap );

	// returns a pointer to the image data.
	void*					GetImageData() const					{	return _bits[ 0 ];		}
	void*					GetImageData( unsigned int mipLevel );

private:
	GrImage&				operator = ( const GrImage& image )		{	return *this;			}

	void					Load( const UPath& name, UReader& reader );
	void					ReadDDS( const UPath& name, UReader& reader );
	void					ReadTGA( const UPath& name, UReader& reader );
	unsigned int			CalcMaxMipCount() const;
	bool					IsFloatImage() const;
	void					EnsureMipLevel( unsigned int level, bool normalMap );

	UPath					_name;
	void**					_bits;
	unsigned int			_width;
	unsigned int			_height;
	unsigned int			_mipCount;	// is changed in GenMipmaps.
	unsigned int			_maxMipCount;
	unsigned int			_bytesPerPel;
	EFORMAT					_fmt;
};
