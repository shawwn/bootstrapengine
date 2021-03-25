//----------------------------------------------------------
// File:		GrSoftwareSampler.h
// Author:		Kevin Bray
// Created:		09-07-08
//
// Purpose:		To sample textures when rendering using the software
//				rasterizer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrImage;
struct SVec2;
struct SVec3;
struct SVec4;

#define SAMPLER_CACHE_LINE_COUNT	4

//**********************************************************
// class GrSoftwareSampler
//**********************************************************
class GrSoftwareSampler
{
public:
	GrSoftwareSampler();
	~GrSoftwareSampler();

	// sets the texture to the sampler.  At some point, we probably want
	// to use a different layout than the standard scanline based image
	// layout (eg, swizzled).
	void				SetTexture( GrImage* image );
	void				SetColor( unsigned int bgraColor );
	
	bool				HasTexture() const			{	return ( _image != 0 );		}

	// samples the image as BGRA
	unsigned int		Sample( const SVec2& uv );
	unsigned int		Sample( const SVec3& uv );
	unsigned int		Sample( const SVec4& uv );
	unsigned int		Sample( float u, float v );

	// perform texture sampling with filtering.  This function only samples
	// the blue channel.
	SVec2				SampleBump( const SVec4& uv, const SVec4& gradDX, const SVec4& gradDY, float bumpScale );
	SVec2				SampleBump( const SVec3& uv, const SVec3& gradDX, const SVec3& gradDY, float bumpScale );
	SVec2				SampleBump( const SVec2& uv, const SVec2& gradDX, const SVec2& gradDY, float bumpScale );

private:
	void				FillCache( unsigned int cacheLine, unsigned int x, unsigned int y );
	void				FillCache4( unsigned int cacheLine, unsigned int x, unsigned int y );
	void				FillCache3( unsigned int cacheLine, unsigned int x, unsigned int y );
	void				FillCache2( unsigned int cacheLine, unsigned int x, unsigned int y );
	void				FillCache1( unsigned int cacheLine, unsigned int x, unsigned int y );
	unsigned int		Fetch( unsigned int x, unsigned int y );

	// local cache.
	unsigned int*		_cache[ 4 ];
	unsigned int		_cacheX[ SAMPLER_CACHE_LINE_COUNT ];
	unsigned int		_cacheY[ SAMPLER_CACHE_LINE_COUNT ];
	unsigned int		_mipLevel[ SAMPLER_CACHE_LINE_COUNT ];
	unsigned int		_lastUsed[ SAMPLER_CACHE_LINE_COUNT ];
	unsigned int		_sampleCount;
	unsigned int		_color;

	// image and related information.
	GrImage*			_image;
	unsigned int		_bytesPerPel;
	unsigned int		_width;
	unsigned int		_height;
	float				_uScale;
	float				_vScale;
	unsigned int		_stride;
	unsigned int		_maskU;
	unsigned int		_maskV;
};
