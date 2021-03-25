//----------------------------------------------------------
// File:		GrSoftwareSampler.cpp
// Author:		Kevin Bray
// Created:		09-07-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSoftwareSampler.h"

// project includes.
#include "GrImage.h"
#include "GrTypes.h"

// cache dimensions.
#define CACHE_DIM			8U
#define CACHE_MASK			( CACHE_DIM - 1 )
#define INV_CACHE_MASK		( ~CACHE_MASK )

//**********************************************************
// class GrSoftwareSampler
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSoftwareSampler::GrSoftwareSampler()
: _sampleCount( 0 )
, _color( 0xFFFFFFFF )
, _image( 0 )
, _bytesPerPel( 0 )
, _width( 0 )
, _height( 0 )
, _uScale( 0.0f )
, _vScale( 0.0f )
, _stride( 0 )
, _maskU( 0 )
, _maskV( 0 )
{
	for ( unsigned int i = 0; i < SAMPLER_CACHE_LINE_COUNT; ++i )
	{
		// allocate memory for the cache.
		_cache[ i ] = ( unsigned int* )AlignedAlloc( 16, CACHE_DIM * CACHE_DIM * sizeof( unsigned int ) );
		_cache[ i ][ 0 ] = _color;

		// initialize the current cache line's info.
		_cacheX[ i ] = 0;
		_cacheY[ i ] = 0;
		_mipLevel[ i ] = 0;
		_lastUsed[ i ] = 0;
	}
}

//----------------------------------------------------------
GrSoftwareSampler::~GrSoftwareSampler()
{
	for ( unsigned int i = 0; i < SAMPLER_CACHE_LINE_COUNT; ++i )
		AlignedFree( _cache[ i ] );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrSoftwareSampler::SetTexture( GrImage* image )
{
	// verify that the incoming image is valid.
	if ( image )
	{
		B_ASSERT( IsPow2( image->GetWidth() ) );
		B_ASSERT( IsPow2( image->GetHeight() ) );

		// calculate what's need to quickly convert normalized texture
		// coordinates to denormalized coordinates.
		_uScale = ( float )image->GetWidth();
		_vScale = ( float )image->GetHeight();
		_width = image->GetWidth();
		_height = image->GetHeight();
		_maskU = _width - 1;
		_maskV = _height - 1;
		_bytesPerPel = image->GetBytesPerPixel();
		_stride = _bytesPerPel * image->GetWidth();
		_image = image;

		// fill the caches.
		for ( unsigned int i = 0; i < SAMPLER_CACHE_LINE_COUNT; ++i )
			FillCache( 0, 0, 0 );
	}
	else
	{
		_uScale = 1.0f;
		_vScale = 1.0f;
		_width = 1;
		_height = 1;
		_maskU = 0;
		_maskV = 0;
		_bytesPerPel = 0;
		_stride = 0;
		_image = 0;
		_cache[ 0 ][ 0 ] = _color;
	}
}

//----------------------------------------------------------
void
GrSoftwareSampler::SetColor( unsigned int bgraColor )
{
	_color = bgraColor;
	if ( !_image )
		_cache[ 0 ][ 0 ] = _color;
}

//----------------------------------------------------------
unsigned int
GrSoftwareSampler::Sample( const SVec2& uv )
{
	return Sample( uv.x, uv.y );
}

//----------------------------------------------------------
unsigned int
GrSoftwareSampler::Sample( const SVec3& uv )
{
	return Sample( uv.x, uv.y );
}

//----------------------------------------------------------
unsigned int
GrSoftwareSampler::Sample( const SVec4& uv )
{
	return Sample( uv.x, uv.y );
}

//----------------------------------------------------------
unsigned int
GrSoftwareSampler::Sample( float u, float v )
{
	// denormalize the incoming UVs.
	unsigned int x = ( unsigned int )( int )( u * _uScale - 0.5f );
	unsigned int y = ( unsigned int )( int )( v * _vScale - 0.5f );

	// fetch the pixel and return.
	return Fetch( x, y );
}

//----------------------------------------------------------
SVec2
GrSoftwareSampler::SampleBump( const SVec4& uv, const SVec4& gradDX, const SVec4& gradDY, float bumpScale )
{
	return SampleBump( *( const SVec2* )&uv, *( const SVec2* )&gradDX, *( const SVec2* )&gradDY, bumpScale );
}

//----------------------------------------------------------
SVec2
GrSoftwareSampler::SampleBump( const SVec3& uv, const SVec3& gradDX, const SVec3& gradDY, float bumpScale )
{
	return SampleBump( *( const SVec2* )&uv, *( const SVec2* )&gradDX, *( const SVec2* )&gradDY, bumpScale );
}

//----------------------------------------------------------
SVec2
GrSoftwareSampler::SampleBump( const SVec2& uv, const SVec2& gradDX, const SVec2& gradDY, float bumpScale )
{
	// calculate the sample coordinates.
	float tcX0 = uv.x * _uScale - 0.5f;
	float tcY0 = uv.y * _vScale - 0.5f;

	// get the gradient.
	SVec2 gradDXNorm( gradDX.x, gradDX.y );
	SVec2 gradDYNorm( gradDY.x, gradDY.y );

    // normalize the gradient if it's nonzero.
    if ( Abs( gradDX.x ) > 0.0001f || Abs( gradDX.y ) > 0.0001f )
        gradDXNorm.Normalize();
    if ( Abs( gradDY.x ) > 0.0001f || Abs( gradDY.y ) > 0.0001f )
        gradDYNorm.Normalize();

	// calculate fetch offsets.
	SVec2 tcDX = gradDXNorm;
	SVec2 tcDY = gradDYNorm;

#if 1
	// calculate fetch offsets.
	int xDom = Abs( tcDX.y ) < Abs( tcDX.x );
	int xDomInv = xDom ^ 1;
	int yDom = Abs( tcDY.y ) < Abs( tcDY.x );
	int yDomInv = yDom ^ 1;

	// scale the fetch offsets so that they address the neighboring
	// texel.
    float divDX = Abs( tcDX[ xDomInv ] );
    float divDY = Abs( tcDY[ yDomInv ] );
    float invDivDX = 1.0f;
    float invDivDY = 1.0f;
    if ( divDX > 0.0001f )
        invDivDX /= divDX;
    if ( divDY > 0.0001f )
        invDivDY /= divDY;
	tcDX[ xDom ] *= invDivDX;
	tcDX[ xDomInv ] *= invDivDX;
	tcDY[ yDom ] *= invDivDY;
	tcDY[ yDomInv ] *= invDivDY;
#endif

	// calculate offset texture coordinates.
	float tcX1 = tcX0 + tcDX.x;
	float tcY1 = tcY0 + tcDX.y;
	float tcX2 = tcX0 + tcDY.x;
	float tcY2 = tcY0 + tcDY.y;

	// calculate physical texture coordinates.
	unsigned int x0 = ( unsigned int )( int )( tcX0 + 0.0625f );
	unsigned int y0 = ( unsigned int )( int )( tcY0 + 0.0625f );
	unsigned int x1 = ( unsigned int )( int )( tcX1 + 0.0625f );
	unsigned int y1 = ( unsigned int )( int )( tcY1 + 0.0625f );
	unsigned int x2 = ( unsigned int )( int )( tcX2 + 0.0625f );
	unsigned int y2 = ( unsigned int )( int )( tcY2 + 0.0625f );

	// get texel values.
	unsigned int v0  = Fetch( x0, y0 ) & 0xFF;
	unsigned int vDX = Fetch( x1, y1 ) & 0xFF;
	unsigned int vDY = Fetch( x2, y2 ) & 0xFF;

	// now calculate the bump value to return.
	float vf0  = 2.0f * ( ( float )( int )v0  / 255.0f ) - 1.0f;
	float vfDX = 2.0f * ( ( float )( int )vDX / 255.0f ) - 1.0f;
	float vfDY = 2.0f * ( ( float )( int )vDY / 255.0f ) - 1.0f;

	// calculate the final bump value.
	float dx = vf0 - vfDX;
	float dy = vf0 - vfDY;
	dx = Clamp( bumpScale * dx, -1.0f, 1.0f );
	dy = Clamp( bumpScale * dy, -1.0f, 1.0f );

	// return the final color value.
	return SVec2( dx, dy );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrSoftwareSampler::FillCache( unsigned int cacheLine, unsigned int x, unsigned int y )
{
	x &= INV_CACHE_MASK;
	y &= INV_CACHE_MASK;
	_cacheX[ cacheLine ] = x;
	_cacheY[ cacheLine ] = y;
	_mipLevel[ cacheLine ] = 0;

	// determine what format conversion is necessary.
	switch ( _bytesPerPel )
	{
	case 4:
		FillCache4( cacheLine, x, y );
		break;

	case 3:
		FillCache3( cacheLine, x, y );
		break;

	case 2:
		FillCache2( cacheLine, x, y );
		break;

	case 1:
		FillCache1( cacheLine, x, y );
		break;

	default:
		break;
	};
}

//----------------------------------------------------------
void
GrSoftwareSampler::FillCache4( unsigned int cacheLine, unsigned int x, unsigned int y )
{
	// simply copy each pixel.
	unsigned char* row = ( unsigned char* )_image->GetImageData() + y * _stride + ( x * _bytesPerPel );
	unsigned int* dst = _cache[ cacheLine ];
	unsigned int endY = Min( _height, CACHE_DIM );
	for ( unsigned int cy = 0; cy < endY; ++cy, ++y, row += _stride )
	{
		unsigned int* src = ( unsigned int* )row;
		unsigned int endX = Min( _width, CACHE_DIM );
		for ( unsigned int cx = 0; cx < endX; ++cx, ++x, ++dst, ++src )
			*dst = *src;
	}
}

//----------------------------------------------------------
void
GrSoftwareSampler::FillCache3( unsigned int cacheLine, unsigned int x, unsigned int y )
{
	// move to the first pixel.
	unsigned char* row = ( unsigned char* )_image->GetImageData() + y * _stride + ( x * _bytesPerPel );
	unsigned int* dst = _cache[ cacheLine ];

	// simply copy each pixel.
	unsigned int endY = Min( _height, CACHE_DIM );
	for ( unsigned int cy = 0; cy < endY; ++cy, row += _stride )
	{
		// calculate an initial value of 0xFF000000
		unsigned int initValue = 0xFF000000;
//		initValue ^= initValue;
//		initValue -= 1;
//		initValue =  initValue << 24;

		// iterate through each pixel along x.
		unsigned int endX = Min( _width, CACHE_DIM );
		unsigned char* src = row;
		for ( unsigned int cx = 0; cx < endX; ++cx, src += 3, ++dst )
		{
			*dst =  initValue;
			*dst |= ( unsigned int )src[ 0 ] <<  0;
			*dst |= ( unsigned int )src[ 1 ] <<  8;
			*dst |= ( unsigned int )src[ 2 ] << 16;
		}
	}
}

//----------------------------------------------------------
void
GrSoftwareSampler::FillCache2( unsigned int cacheLine, unsigned int x, unsigned int y )
{
	// simply copy each pixel.
	unsigned char* row = ( unsigned char* )_image->GetImageData() + y * _stride + ( x * _bytesPerPel );
	unsigned int* dst = _cache[ cacheLine ];
	unsigned int endY = Min( _height, CACHE_DIM );
	for ( unsigned int cy = 0; cy < endY; ++cy, ++y, row += _stride )
	{
		unsigned char* src = ( unsigned char* )row;
		unsigned int endX = Min( _width, CACHE_DIM );
		for ( unsigned int cx = 0; cx < CACHE_DIM; ++cx, ++x, ++dst, src += 2 )
		{
			*dst =  ( unsigned int )src[ 0 ] <<  0;
			*dst |= ( unsigned int )src[ 0 ] <<  8;
			*dst |= ( unsigned int )src[ 0 ] << 16;
			*dst |= ( unsigned int )src[ 1 ] << 24;
		}
	}
}

//----------------------------------------------------------
void
GrSoftwareSampler::FillCache1( unsigned int cacheLine, unsigned int x, unsigned int y )
{
	// simply copy each pixel.
	unsigned char* row = ( unsigned char* )_image->GetImageData() + y * _stride + ( x * _bytesPerPel );
	unsigned int* dst = _cache[ cacheLine ];
	unsigned int endY = Min( _height, CACHE_DIM );
	for ( unsigned int cy = 0; cy < endY; ++cy, ++y, row += _stride )
	{
		unsigned char* src = ( unsigned char* )row;
		unsigned int endX = Min( _width, CACHE_DIM );
		for ( unsigned int cx = 0; cx < endX; ++cx, ++x, ++dst, ++src )
		{
			*dst =  ( unsigned int )src[ 0 ] <<  0;
			*dst |= ( unsigned int )src[ 0 ] <<  8;
			*dst |= ( unsigned int )src[ 0 ] << 16;
			*dst |= ( unsigned int )src[ 0 ] << 24;
		}
	}
}

//----------------------------------------------------------
unsigned int
GrSoftwareSampler::Fetch( unsigned int x, unsigned int y )
{
	// wrap x and y.
	x &= _maskU;
	y &= _maskV;

	// increment the sample counter.
	++_sampleCount;

	// determine if the point is in the cache.
	unsigned int oldest = 0;
	unsigned int lastUsed = _sampleCount;
	for ( unsigned int i = 0; i < SAMPLER_CACHE_LINE_COUNT; ++i )
	{
		unsigned int cacheX = x - _cacheX[ i ];
		unsigned int cacheY = y - _cacheY[ i ];
		if ( cacheX >= CACHE_DIM || cacheY >= CACHE_DIM )
		{
			if ( _lastUsed[ i ] < lastUsed )
			{
				lastUsed = _lastUsed[ i ];
				oldest = i;
			}
		}
		else
		{
			_lastUsed[ i ] = _sampleCount;
			return _cache[ i ][ cacheY * CACHE_DIM + cacheX ];
		}
	}

	// now fill the last used cache.
	FillCache( oldest, x, y );
	_lastUsed[ oldest ] = _sampleCount;

	// calculate the texel's location relative to the current
	// cache line.
	unsigned int cacheX = x - _cacheX[ oldest ];
	unsigned int cacheY = y - _cacheY[ oldest ];

	// return the color in the cache.
	return _cache[ oldest ][ cacheY * CACHE_DIM + cacheX ];
}
