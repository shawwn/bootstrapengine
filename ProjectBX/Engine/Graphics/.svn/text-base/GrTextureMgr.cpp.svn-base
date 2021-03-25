//----------------------------------------------------------
// File:		GrTextureMgr.cpp
// Author:		Kevin Bray
// Created:		10-20-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTextureMgr.h"

// project includes.
#include "GrImage.h"
#include "RRsrcMgr.h"
#include "GLSubsys.h"
#include "GrTextureCube.h"
#include "GrTexture.h"
#include "GrTexture3D.h"
#include "MVec3.h"
#include "GrTypes.h"
#include "RFileMgr.h"
#include "URand.h"

#include "GrDebug.h"

//----------------------------------------------------------
static void	GenNormalizationCube( void* px, void* nx, void* py, void* ny, void* pz, void* nz, unsigned int size );
static void GenJitterVolume( void* ptr, unsigned int size, unsigned int jitterSamples );
static void GenAttenImage( void* ptr, unsigned int dim );
static void GenZLookup( void* ptr, unsigned int len, float near, float max );
static void GenSpotFalloffConst( void* ptr, unsigned int dim );
static void GenSpotFalloffLin( void* ptr, unsigned int dim );
static void GenSpotFalloffSqr( void* ptr, unsigned int dim );
static void GenFaceImagesX( float* px, float* nx, float* py, float* ny, float* pz, float* nz );
static void GenFaceImagesY( float* px, float* nx, float* py, float* ny, float* pz, float* nz );

// singleton pointer.
GrTextureMgr *gGrTextureMgr = 0;

//**********************************************************
// class GrTextureMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureMgr::GrTextureMgr( EGRQUALITY texturingQuality )
: _texturingQuality( texturingQuality )
{
	B_ASSERT( gGrTextureMgr == 0 );
	gGrTextureMgr = this;

	// create software generated textures.
	CreateSysTextures();
}

//----------------------------------------------------------
GrTextureMgr::~GrTextureMgr()
{
	_textures.clear();
	_textureVec.clear();
	gGrTextureMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTextureMgr::Update()
{
	// collect garbage.

	// update textures.
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetTextureFile( const UPath& imageName, GrTextureBase::EFORMAT format, GrTextureBase::Flags flags, bool cacheImage, bool cache )
{
	// see if the file was already loaded.
	URef< GrTextureBase > texture;
	if ( cache )
	{
		if ( format == GrTextureBase::EF_GUESS )
			texture = FindTexture( imageName );
		else
			texture = FindTexture( imageName, format, flags );

		if ( texture != 0 )
			return texture;
	}

	// return NULL if the image doesn't exist.
	if ( !gRFileMgr->Exists( imageName ) )
		return 0;

	// create the texture and add it to the texture database.
	texture = new GrTexture( imageName, imageName, format, flags, cacheImage );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetTextureImage( GrImage* image, GrTextureBase::EFORMAT format, GrTextureBase::Flags flags, bool cache )
{
	const UPath& imageName = image->GetName();

	// try to find the texture.
	B_ASSERT( image != 0 );
	URef< GrTextureBase > texture;
	if ( cache )
	{
		texture = FindTexture( imageName, format, flags );
		if ( texture != 0 )
		{
			delete image;
			return texture;
		}
	}

	// create the texture and add it to the texture database.
	texture = new GrTexture( imageName, image, format, flags );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetEmptyTexture( const UPath& name, unsigned int width, unsigned int height,
							   GrTextureBase::EFORMAT format, GrTextureBase::Flags flags, bool cache )
{
	// try to find the texture.
	URef< GrTextureBase > texture = 0;
	if ( cache )
	{
		texture = FindTexture( name, format, flags );
		if ( texture != 0 )
			return texture;
	}

	// create the texture and add it to the texture database.
	texture = new GrTexture( name, format, width, height, flags );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetTextureCubeFiles( const UPath** imageNames, GrTextureBase::EFORMAT format,
								   GrTextureBase::Flags flags, bool cacheImages, bool cache )
{
	tstring imageNameStr = imageNames[ 0 ]->GetPathString();
	imageNameStr += imageNames[ 1 ]->GetPathString();
	imageNameStr += imageNames[ 2 ]->GetPathString();
	imageNameStr += imageNames[ 3 ]->GetPathString();
	imageNameStr += imageNames[ 4 ]->GetPathString();
	imageNameStr += imageNames[ 5 ]->GetPathString();
	UPath imageName( imageNameStr );

	// try to find the texture.
	URef< GrTextureBase > texture;
	if ( cache )
	{
		texture = FindTexture( imageName, format, flags );
		if ( texture != 0 )
			return texture;
	}

	// create the texture and add it to the texture database.
	texture = new GrTextureCube( imageName, imageNames, format, flags, cacheImages );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetTextureCubeImages( GrImage** images, unsigned int width, GrTextureBase::EFORMAT format,
								    GrTextureBase::Flags flags, bool cache )
{
	UPath imageName = images[ 0 ]->GetName();
	imageName += images[ 1 ]->GetName();
	imageName += images[ 2 ]->GetName();
	imageName += images[ 3 ]->GetName();
	imageName += images[ 4 ]->GetName();
	imageName += images[ 5 ]->GetName();

	// try to find the texture.
	URef< GrTextureBase > texture;
	if ( cache )
	{
		texture = FindTexture( imageName, format, flags );
		if ( texture != 0 )
		{
			for ( unsigned int i = 0; i < 6; ++i )
				delete[] images[ i ];
			return texture;
		}
	}

	// create the texture and add it to the texture database.
	texture = new GrTextureCube( imageName, images, format, flags );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::GetEmptyTextureCube( const UPath& name, unsigned int width, GrTextureBase::EFORMAT format,
								   GrTextureBase::Flags flags, bool cache )
{
	// try to find the texture.
	URef< GrTextureBase > texture;
	if ( cache )
	{
		texture = FindTexture( name, format, flags );
		if ( texture != 0 )
			return texture;
	}

	// create the texture and add it to the texture database.
	texture = new GrTextureCube( name, format, width, flags );
	_textureVec.push_back( texture );
	if ( cache )
		InsertTexture( texture );

	// return the newly created texture.
	return texture;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::FindTexture( const UPath& imageName, GrTextureBase::EFORMAT format, GrTextureBase::Flags flags ) const
{
	// find one with a matching format.
	TextureMap::const_iterator iter = _textures.find( imageName );
	while ( iter != _textures.end() && iter->first == imageName )
	{
		if ( iter->second->GetFormat() == format && iter->second->GetFlags() == flags )
			return iter->second;

		++iter;
	}
	return 0;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrTextureMgr::FindTexture( const UPath& imageName ) const
{
	// find one with a matching format.
	TextureMap::const_iterator iter = _textures.find( imageName );
	if ( iter != _textures.end() )
	{
		return iter->second;
	}
	return 0;
}

//----------------------------------------------------------
void
GrTextureMgr::PreContextReset()
{
	// notify all textures of the context reset.
	TextureVec::iterator iter = _textureVec.begin();
	while( iter != _textureVec.end() )
	{
		// delete the texture if necessary.
		if ( ( *iter )->GetRef() == 1 && ( *iter )->GetUnloadable() )
		{
			iter = _textureVec.erase( iter );
			continue;
		}

		// notify the texture of the pending reset.
		( *iter )->PreContextReset();
		++iter;
	}
}

//----------------------------------------------------------
void
GrTextureMgr::PostContextReset()
{
	// notify all textures of the context reset.
	TextureVec::iterator iter = _textureVec.begin();
	const TextureVec::iterator end = _textureVec.end();
	for ( ; iter != end; ++iter )
	{
		// notify the texture that the reset is finished.
		( *iter )->PostContextReset();
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTextureMgr::CreateSysTextures()
{
	// GR_SYSTEX_BLACK
	GrImage* black = new GrImage( GR_SYSTEX_BLACK, 0x00000000 );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_BLACK, black, GrTexture::EF_RGBA32, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// GR_SYSTEX_WHITE
	GrImage* white = new GrImage( GR_SYSTEX_WHITE, 0xFFFFFFFF );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_WHITE, white, GrTexture::EF_RGBA32, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// GR_SYSTEX_GRAY
	GrImage* gray = new GrImage( GR_SYSTEX_GRAY, 0xFF7F7F7F );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_GRAY, gray, GrTexture::EF_RGBA32, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// GR_SYSTEX_BLACKWHITE
	char* data = new char[ 2 ];
	data[ 0 ] = 0;
	data[ 1 ] = 1;
	GrImage* blackWhite = new GrImage( GR_SYSTEX_BLACKWHITE, GrImage::EF_I8, 2, 1, data );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_BLACKWHITE, blackWhite, GrTexture::EF_L8, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	delete[] data;

	// GR_SYSTEX_ERROR... just use red for now.
	GrImage* error = new GrImage( GR_SYSTEX_ERROR, 0xFF0000FF );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_ERROR, error, GrTexture::EF_RGBA32, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// GR_SYSTEX_DEPTH
	float* depthData = new float[ 1 ];
	depthData[ 0 ] = 1.0f;
	GrImage* depth = new GrImage( GR_SYSTEX_DEPTH, GrImage::EF_DEPTH, 1, 1, depthData );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_DEPTH, depth, GrTexture::EF_DEPTH, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	delete[] depthData;

	// GR_SYSTEX_NORMAL
	GrImage* normal = new GrImage( GR_SYSTEX_NORMAL, 0x7F7F7FFF );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_NORMAL, normal, GrTexture::EF_RGBA32, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// GR_SYSTEX_LINEAR
	data = new char[ 256 ];
	for ( unsigned int i = 0; i < 256; ++i )
		data[ i ] = 255-i;
	GrImage* linear = new GrImage( GR_SYSTEX_LINEAR, GrImage::EF_I8, 256, 1, data );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_LINEAR, linear, GrTexture::EF_I8, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	delete[] data;

	// GR_SYSTEX_QUADRATIC
	data = new char[ 256 ];
	for ( unsigned int i = 0; i < 256; ++i )
		data[ i ] = ( ( 255-i ) * ( 255-i ) ) >> 8;
	GrImage* quadratic = new GrImage( GR_SYSTEX_QUADRATIC, GrImage::EF_I8, 256, 1, data );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_QUADRATIC, quadratic, GrTexture::EF_I8, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	delete[] data;

	// TODO: Make the normalization cube map valid!
	// create system texture cubes.
	GrImage* blackImages[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	GrImage* whiteImages[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	GrImage* errorImages[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	for ( unsigned int i = 0; i < 6; ++i )
		blackImages[ i ] = new GrImage( GR_SYSTEXCUBE_BLACK, 0x00000000 );

	for ( unsigned int i = 0; i < 6; ++i )
		whiteImages[ i ] = new GrImage( GR_SYSTEXCUBE_WHITE, 0xFFFFFFFF );

	for ( unsigned int i = 0; i < 6; ++i )
		errorImages[ i ] = new GrImage( GR_SYSTEXCUBE_ERROR, 0xFFFF0000 );

	AddAndInsertTexture( new GrTextureCube( GR_SYSTEXCUBE_BLACK, blackImages, GrTexture::EF_RGBA32, 
		GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	AddAndInsertTexture( new GrTextureCube( GR_SYSTEXCUBE_WHITE, whiteImages, GrTexture::EF_RGBA32,
		GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	AddAndInsertTexture( new GrTextureCube( GR_SYSTEXCUBE_ERROR, errorImages, GrTexture::EF_RGBA32,
		GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// build the normalization cube map.
	const unsigned int kNormalizeCubeSize = 512;
	void* ptrs[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	GrImage* normalImages[ 6 ] = { 0, 0, 0, 0, 0, 0 };

	// allocate memory.
	for ( unsigned int i = 0; i < 6; ++i )
		ptrs[ i ] = new char[ kNormalizeCubeSize * kNormalizeCubeSize * 3 ];

	GenNormalizationCube( ptrs[ 0 ], ptrs[ 1 ], ptrs[ 2 ], ptrs[ 3 ], ptrs[ 4 ], ptrs[ 5 ], kNormalizeCubeSize );

	// create the images.
	for ( unsigned int i = 0; i < 6; ++i )
	{
		normalImages[ i ] = new GrImage( GR_SYSTEXCUBE_NORMALIZE, GrImage::EF_BGR24, kNormalizeCubeSize, kNormalizeCubeSize, ptrs[ i ] );
		delete[] ptrs[ i ];
	}

	// create the normalization cube texture.
	AddAndInsertTexture( new GrTextureCube( GR_SYSTEXCUBE_NORMALIZE, normalImages, GrTexture::EF_RGB24, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );

	// build the jitter volume texture.
	const unsigned int kJitterVolSize = 32;
	const unsigned int kJitterSampleCount = 64;

	// kJitterVolSize * kJitterVolSize * kJitterSampleCount * kJitterSampleCount / 2 * 4.
	unsigned int jitterVolDepth = kJitterSampleCount/2;
	unsigned char* ptr = new unsigned char[ kJitterVolSize*kJitterVolSize*jitterVolDepth*4 ];
	GenJitterVolume( ptr, kJitterVolSize, kJitterSampleCount );

	// create the jitter volume texture.
	AddAndInsertTexture( new GrTexture3D( GR_SYSTEX3D_JITTER, GrTextureBase::EF_RGBA32, kJitterVolSize, kJitterVolSize,
		jitterVolDepth, ptr ) );
	delete[] ptr;

	// build the spot textures.
	const unsigned int kSpotDim = 256;
	ptr = new unsigned char[ kSpotDim*kSpotDim ];
	GenSpotFalloffConst( ptr, kSpotDim );
	GrImage* falloffConst = new GrImage( GR_SYSTEX_SPOT_CONSTANT, GrImage::EF_I8, kSpotDim, kSpotDim, ptr );
	delete[] ptr;

	ptr = new unsigned char[ kSpotDim*kSpotDim ];
	GenSpotFalloffLin( ptr, kSpotDim );
	GrImage* falloffLin = new GrImage( GR_SYSTEX_SPOT_LINEAR, GrImage::EF_I8, kSpotDim, kSpotDim, ptr );
	delete[] ptr;

	ptr = new unsigned char[ kSpotDim*kSpotDim ];
	GenSpotFalloffSqr( ptr, kSpotDim );
	GrImage* falloffSqr = new GrImage( GR_SYSTEX_SPOT_QUADRATIC, GrImage::EF_I8, kSpotDim, kSpotDim, ptr );
	delete[] ptr;

	AddAndInsertTexture( new GrTexture( GR_SYSTEX_SPOT_CONSTANT, falloffConst, GrTextureBase::EF_I8, GrTexture::kNoMipMap ) );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_SPOT_LINEAR, falloffLin, GrTextureBase::EF_I8, GrTexture::kNoMipMap ) );
	AddAndInsertTexture( new GrTexture( GR_SYSTEX_SPOT_QUADRATIC, falloffSqr, GrTextureBase::EF_I8, GrTexture::kNoMipMap ) );

	// create the stipple mask textures.
	static const char* stippleNames[4] = {
		GR_SYSTEX_STIPPLE0,
		GR_SYSTEX_STIPPLE1,
		GR_SYSTEX_STIPPLE2,
		GR_SYSTEX_STIPPLE3,
	};
	for ( unsigned int i = 0; i < 4; ++i )
	{
		static const unsigned int kStippleDim = 2;
		ptr = new unsigned char[ kStippleDim * kStippleDim ];
		MemSet( ptr, 0, kStippleDim * kStippleDim );
		( ( unsigned char* )ptr )[ i ] = 255;
		GrImage* stipple = new GrImage( stippleNames[ i ], GrImage::EF_I8, kStippleDim, kStippleDim, ptr );
		AddAndInsertTexture( new GrTexture( stippleNames[ i ], stipple, GrTextureBase::EF_I8, GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
		delete[] ptr;
	}

	// create the cubemap face lookup textures.
	float* cubeDataX[ 6 ];
	float* cubeDataY[ 6 ];
	for ( unsigned int i = 0; i < 6; ++i )
	{
		cubeDataX[ i ] = new float[ 4 ];
		cubeDataY[ i ] = new float[ 4 ];
	}
	GenFaceImagesX( cubeDataX[ 0 ], cubeDataX[ 1 ], cubeDataX[ 2 ], cubeDataX[ 3 ], cubeDataX[ 4 ], cubeDataX[ 5 ] );
	GenFaceImagesY( cubeDataY[ 0 ], cubeDataY[ 1 ], cubeDataY[ 2 ], cubeDataY[ 3 ], cubeDataY[ 4 ], cubeDataY[ 5 ] );

	// create the images for the face lookup.
	GrImage* cubeDataImagesX[ 6 ];
	GrImage* cubeDataImagesY[ 6 ];
	for ( unsigned int i = 0; i < 6; ++i )
	{
		cubeDataImagesX[ i ] = new GrImage( "cube_lookup_x", GrImage::EF_BGRA128F, 1, 1, cubeDataX[ i ] );
		cubeDataImagesY[ i ] = new GrImage( "cube_lookup_y", GrImage::EF_BGRA128F, 1, 1, cubeDataY[ i ] );
		delete[] cubeDataX[ i ];
		delete[] cubeDataY[ i ];
	}

	// add the face lookup textures.
	AddAndInsertTexture( new GrTextureCube( GR_SYSTEX_CUBE_X, cubeDataImagesX, GrTextureBase::EF_RGBA64F,
		GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
	AddAndInsertTexture( new GrTextureCube( GR_SYSTEX_CUBE_Y, cubeDataImagesY, GrTextureBase::EF_RGBA64F,
		GrTexture::kNoMipMap | GrTexture::kNoCompress ) );
}

//----------------------------------------------------------
void
GrTextureMgr::InsertTexture( URef< GrTextureBase > texture )
{
	B_ASSERT( texture != 0 );
	_textures.insert( std::pair< UPath, URef< GrTextureBase > >( texture->GetName(), texture ) );
}

//----------------------------------------------------------
void
GrTextureMgr::AddAndInsertTexture( URef< GrTextureBase > texture )
{
	B_ASSERT( texture != 0 );
	_textureVec.push_back( texture );
	_textures.insert( std::pair< UPath, URef< GrTextureBase > >( texture->GetName(), texture ) );
}

//----------------------------------------------------------
void
GenNormalizationCube( void* px, void* nx, void* py, void* ny, void* pz, void* nz, unsigned int size )
{
	// some useful variables
	float halfSize = float( size >> 1 );
	float offset = 0.5f;
	MVec3 tempVec;
	unsigned char* bytePtr;

	// positive x
	bytePtr = ( unsigned char* )px;
	for ( unsigned int j=0; j<size; ++j )
	{
		for ( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( halfSize );
			tempVec.SetY( -( j+offset-halfSize ) );
			tempVec.SetZ( -( i+offset-halfSize ) );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}

	// negative x
	bytePtr = ( unsigned char* )nx;
	for( unsigned int j=0; j<size; ++j )
	{
		for( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( -halfSize );
			tempVec.SetY( -( j+offset-halfSize ) );
			tempVec.SetZ( ( i+offset-halfSize ) );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}

	// positive y
	bytePtr = ( unsigned char* )py;
	for( unsigned int j=0; j<size; ++j )
	{
		for( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( i+offset-halfSize );
			tempVec.SetY( halfSize );
			tempVec.SetZ( ( j+offset-halfSize ) );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}

	// negative y
	bytePtr = ( unsigned char* )ny;
	for( unsigned int j=0; j<size; ++j )
	{
		for( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( i+offset-halfSize );
			tempVec.SetY( -halfSize );
			tempVec.SetZ( -( j+offset-halfSize ) );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}

	// positive z
	bytePtr = ( unsigned char* )pz;
	for( unsigned int j=0; j<size; ++j )
	{
		for( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( i+offset-halfSize );
			tempVec.SetY( -( j+offset-halfSize ) );
			tempVec.SetZ( halfSize );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}

	// negative z
	bytePtr = ( unsigned char* )nz;
	for( unsigned int j=0; j<size; ++j )
	{
		for( unsigned int i=0; i<size; ++i )
		{
			tempVec.SetX( -( i+offset-halfSize ) );
			tempVec.SetY( -( j+offset-halfSize ) );
			tempVec.SetZ( -halfSize );
			tempVec.Normalize();

			SByteVec3 byteVec( tempVec );
			bytePtr[ 0 ] = byteVec.z;
			bytePtr[ 1 ] = byteVec.y;
			bytePtr[ 2 ] = byteVec.x;

			bytePtr+=3;
		}
	}
}

//----------------------------------------------------------
void
GenJitterVolume( void* ptr, unsigned int size, unsigned int jitterSamples )
{
	// some useful variables.
	unsigned char* bytePtr = ( unsigned char* )ptr;
	float fJitterSamples = (float)jitterSamples;

	// random number generator.
	URand randGen( ( int )GetTickCount() );

	// fill out the 3D texture.
	for ( int i = 0; i < ( int )size; ++i )
	{
		for ( int j = 0; j < ( int )size; ++j )
		{
			for ( int k = 0; k < ( int )jitterSamples; ++k, bytePtr += 2 )
			{
				// generate a random point on a unit disk.
				float r = Sqrt( randGen.RandFloat() );
				float theta = randGen.RandFloat() * TWO_PI;
				float x = r * Cos( theta );
				float y = r * Sin( theta );

				// write the points to the texture data.
				bytePtr[ 0 ] = ( unsigned char )( 255.0f * ( 0.5f * x + 0.5f ) );
				bytePtr[ 1 ] = ( unsigned char )( 255.0f * ( 0.5f * y + 0.5f ) );
			}
		}
	}
}

//----------------------------------------------------------
#define _SHARP_ATTEN
void
GenAttenImage( void* ptr, unsigned int dim )
{
	const float kS = 1.0f;
	const float kE = 2.71828183f;

	float edge = 1.0f - ( 1.0f / float( dim ) );

	// generates an attenuation texture.
	unsigned char* dst = ( unsigned char* )ptr;
	for ( unsigned int y = 0; y < dim; ++y )
	{
		for ( unsigned int x = 0; x < dim; ++x, ++dst )
		{
			float nx = float( x ) / float( dim );
			float ny = float( y ) / float( dim );

			float d = Sqrt( nx*nx + ny*ny );
#ifdef _SHARP_ATTEN
			float c = Min( d, 1.0f );
			c = ( c < edge ) ? 1.0f : 0.0f;
#else
//			float c = Clamp( edge - d, 0.0f, 1.0f );
			float c = 1.0f - Clamp( d, 0.0f, 1.0f );
			c = Pow( c, 3.0f );
#endif
			*dst = ( unsigned char )( c*255.0f );

/*
			// 2D gaussian distribution.
			float nx = 2.5f * float( x ) / float( dim );
			float ny = 2.5f * float( y ) / float( dim );

			float exp = -0.5f * ( ( nx*nx+ny*ny ) / (kS*kS) );
			float c = Clamp( Pow( kE, exp ), 0.0f, 1.0f ) / TWO_PI;
			*dst = ( unsigned char )( c*255.0f );
*/
		}
	}
}

//----------------------------------------------------------
void
GenZLookup( void* ptr, unsigned int len, float nearZ, float max )
{
	B_ASSERT( max > 0.0f && max < 1.0f );

	float* dst = ( float* )ptr;
	float fLen = ( float )len;
	for ( unsigned int i = 0; i < len; ++i, ++dst )
	{
		float s = Min( ( float( i ) / fLen ), max );
		*dst = -nearZ / ( s - 1.0f );
	}
}

//----------------------------------------------------------
void GenSpotFalloffConst( void* ptr, unsigned int dim )
{
	B_ASSERT( IsPow2( dim ) && dim > 2 );

	// useful vars.
	float fInvDim = 1.0f / float( dim );
	float shift = 0.5f * fInvDim;
	float fHalfSub1 = float( ( dim >> 1 ) - 1 );
	unsigned char* dst = ( unsigned char* )ptr;

	// generate our map.
	for ( unsigned int y = 0; y < dim; ++y )
	{
		float vecY = float( fHalfSub1 - float( y ) ) / fHalfSub1;
		for ( unsigned int x = 0; x < dim; ++x, ++dst )
		{
			float vecX = ( fHalfSub1 - float( x ) ) / fHalfSub1;
			float len = Sqrt( vecX*vecX + vecY*vecY );
			*dst = ( unsigned char )( len < 1.0f ? 255 : 0 );
		}
	}
}

//----------------------------------------------------------
void GenSpotFalloffLin( void* ptr, unsigned int dim )
{
	B_ASSERT( IsPow2( dim ) && dim > 2 );

	// useful vars.
	float fInvDim = 1.0f / float( dim );
	float shift = 0.5f * fInvDim;
	float fHalfSub1 = float( ( dim >> 1 ) - 1 );
	unsigned char* dst = ( unsigned char* )ptr;

	// generate our map.
	for ( unsigned int y = 0; y < dim; ++y )
	{
		float vecY = float( fHalfSub1 - float( y ) ) / fHalfSub1;
		for ( unsigned int x = 0; x < dim; ++x, ++dst )
		{
			float vecX = ( fHalfSub1 - float( x ) ) / fHalfSub1;
			float vecZ = 1.0f-Sqrt( vecX*vecX + vecY*vecY );
			B_ASSERT( vecZ <= 1.0f );
			vecZ = Max( vecZ, 0.0f );
			*dst = ( unsigned char )( 255.0f * vecZ );
		}
	}
}

//----------------------------------------------------------
void GenSpotFalloffSqr( void* ptr, unsigned int dim )
{
	B_ASSERT( IsPow2( dim ) && dim > 2 );

	// useful vars.
	float fInvDim = 1.0f / float( dim );
	float shift = 0.5f * fInvDim;
	float fHalfSub1 = float( ( dim >> 1 ) - 1 );
	unsigned char* dst = ( unsigned char* )ptr;

	// generate our map.
	for ( unsigned int y = 0; y < dim; ++y )
	{
		float vecY = float( fHalfSub1 - float( y ) ) / fHalfSub1;
		for ( unsigned int x = 0; x < dim; ++x, ++dst )
		{
			float vecX = ( fHalfSub1 - float( x ) ) / fHalfSub1;
			float vecZ = 1.0f-Sqrt( vecX*vecX + vecY*vecY );
			B_ASSERT( vecZ <= 1.0f );
			vecZ = Max( vecZ, 0.0f );
			*dst = ( unsigned char )( 255.0f * vecZ * vecZ );
		}
	}
}

//----------------------------------------------------------
void GenFaceImagesX( float* px, float* nx, float* py, float* ny, float* pz, float* nz )
{
	px[ 0 ] =  0.0f;	// X
	px[ 1 ] =  0.0f;	// Y
	px[ 2 ] = -1.0f;	// Z
	px[ 3 ] =  0.0f;

	nx[ 0 ] =  0.0f;	// X
	nx[ 1 ] =  0.0f;	// Y
	nx[ 2 ] =  1.0f;	// Z
	nx[ 3 ] =  0.0f;

	py[ 0 ] =  1.0f;	// X
	py[ 1 ] =  0.0f;	// Y
	py[ 2 ] =  0.0f;	// Z
	py[ 3 ] =  0.0f;

	ny[ 0 ] =  1.0f;	// X
	ny[ 1 ] =  0.0f;	// Y
	ny[ 2 ] =  0.0f;	// Z
	ny[ 3 ] =  0.0f;

	pz[ 0 ] =  1.0f;	// X
	pz[ 1 ] =  0.0f;	// Y
	pz[ 2 ] =  0.0f;	// Z
	pz[ 3 ] =  0.0f;

	nz[ 0 ] = -1.0f;	// X
	nz[ 1 ] =  0.0f;	// Y
	nz[ 2 ] =  0.0f;	// Z
	nz[ 3 ] =  0.0f;
}

//----------------------------------------------------------
void GenFaceImagesY( float* px, float* nx, float* py, float* ny, float* pz, float* nz )
{
	px[ 0 ] =  0.0f;	// X
	px[ 1 ] = -1.0f;	// Y
	px[ 2 ] =  0.0f;	// Z
	px[ 3 ] =  0.0f;

	nx[ 0 ] =  0.0f;	// X
	nx[ 1 ] = -1.0f;	// Y
	nx[ 2 ] =  0.0f;	// Z
	nx[ 3 ] =  0.0f;

	py[ 0 ] =  0.0f;	// X
	py[ 1 ] =  0.0f;	// Y
	py[ 2 ] =  1.0f;	// Z
	py[ 3 ] =  0.0f;

	ny[ 0 ] =  0.0f;	// X
	ny[ 1 ] =  0.0f;	// Y
	ny[ 2 ] = -1.0f;	// Z
	ny[ 3 ] =  0.0f;

	pz[ 0 ] =  0.0f;	// X
	pz[ 1 ] = -1.0f;	// Y
	pz[ 2 ] =  0.0f;	// Z
	pz[ 3 ] =  0.0f;

	nz[ 0 ] =  0.0f;	// X
	nz[ 1 ] = -1.0f;	// Y
	nz[ 2 ] =  0.0f;	// Z
	nz[ 3 ] =  0.0f;
}
