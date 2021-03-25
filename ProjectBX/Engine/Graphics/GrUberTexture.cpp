//----------------------------------------------------------
// File:		GrUberTexture.h
// Author:		Kevin Bray
// Created:		12-21-08
//
// Purpose:		To manage an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUberTexture.h"

// project includes.
#include "GrUberDetailCache.h"
#include "GrTextureMgr.h"
#include "GrTexture.h"
#include "GrUTAtlas.h"
#include "GrUTLoader.h"
#include "GrConfig.h"
#include "RFileMgr.h"
#include "RFile.h"

// tiles are 10K in size.
#define DETAIL_TILE_CACHE_SIZE			4096

// tile size in bytes.
#define DETAIL_TILE_BUMP_SIZE			2048
#define DETAIL_TILE_DIFFUSE_SIZE		4096
#define DETAIL_TILE_SPECULAR_SIZE		4096
#define DETAIL_TILE_SIZE				10240

// atlas width and height.
#define DETAIL_ATLAS_WIDTH				( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY )
#define DETAIL_ATLAS_HEIGHT				( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY )

#define DETAIL_ATLAS_MEM_PEL_SIZE		( 2 )
#define DETAIL_ATLAS_MEM_WIDTH			( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY * DETAIL_ATLAS_MEM_PEL_SIZE )
#define DETAIL_ATLAS_MEM_HEIGHT			( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY )
#define DETAIL_ATLAS_MEM_SIZE			( DETAIL_ATLAS_MEM_WIDTH * DETAIL_ATLAS_MEM_HEIGHT )

#define DETAIL_ATLAS_ADDR( ptr, x, y )	( ptr + y * DETAIL_ATLAS_MEM_WIDTH + x * DETAIL_ATLAS_MEM_PEL_SIZE )

// tile data offsets.
#define TILE_BUMP_OFFSET( ptr )			( ( unsigned char* )ptr +    0 )
#define TILE_DIFFUSE_OFFSET( ptr )		( ( unsigned char* )ptr + 2048 )
#define TILE_SPECULAR_OFFSET( ptr )		( ( unsigned char* )ptr + 6144 )

// lowres texture sizes.
#define LOWRES_TEXTURE_WIDTH			2048
#define LOWRES_TEXTURE_HEIGHT			2048

//**********************************************************
// class GrUberTexture
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUberTexture::GrUberTexture( const UPath& filePath )
: _fileAtlas( 0 )
, _tileCache( DETAIL_TILE_CACHE_SIZE )
, _tileLocks( 0 )
{
	// open an existing file.
	const unsigned int kOpenFlags =	RFileMgr::kFlagRead |
									RFileMgr::kFlagWrite |
									RFileMgr::kFlagDMA;

	// open the file.
	_file = gRFileMgr->GetFile( filePath, kOpenFlags );

	// prepare the ubertexture database for reading.
	_fileAtlas = gGrUTLoader->LoadUbertex( _file->GetHandle() );

	// allocate resources.
	Init();

	// TODO: fill out the base mip levels.
}

//----------------------------------------------------------
GrUberTexture::GrUberTexture( const UPath& filePath, const void* tileUsageBitmap, const char* metaData,
							  unsigned int metaDataSize )
: _fileAtlas( 0 )
, _tileCache( DETAIL_TILE_CACHE_SIZE )
, _tileLocks( 0 )
{
	// create a new file.
	const unsigned int kCreateFlags =	RFileMgr::kFlagCreateAlways |
										RFileMgr::kFlagRead |
										RFileMgr::kFlagWrite |
										RFileMgr::kFlagDMA;

	// open the file.
	_file = gRFileMgr->GetFile( filePath, kCreateFlags );

	// create the ubertexture database.
	_fileAtlas = gGrUTLoader->CreateUbertex( _file->GetHandle(), tileUsageBitmap, metaData,
		metaDataSize, 0 );

	// allocate resources.
	Init();

	// TODO: fill out the base mip levels.
}

//----------------------------------------------------------
GrUberTexture::~GrUberTexture()
{
	// free textures.

	// free the detail tile usage map.
	for ( unsigned int i = 0; i < 7; ++i )
		AlignedFree( _detailTileUsage[ i ] );

	// close the ubertexture.
	gGrUTLoader->FreeAtlas( _fileAtlas );

	// free the tile cache.
	unsigned char* mem = ( unsigned char* )~0;
	for ( unsigned int i = 0; i < DETAIL_TILE_CACHE_SIZE; ++i )
	{
		void* cur = _tileCache.GetOldest();
		STileData& tile = _tileCache.GetValue( cur );
		WaitLockFree( tile.lock );
		mem = Min( mem, ( unsigned char* )tile.data );
		_tileCache.Remove( cur );
	}

	// free the tile locks.
	delete[] _tileLocks;

	// free the tile memory.
	AlignedFree( mem );
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
GrUberTexture::CacheDetailTiles( unsigned int startMip, unsigned int x, unsigned int y,
								 unsigned int width, unsigned int height,
								 unsigned int clipMinX, unsigned int clipMinY,
								 unsigned int clipMaxX, unsigned int clipMaxY )
{
	// make sure the width and height are valid.
	B_ASSERT( width <= 16 && height <= 16 );
	B_ASSERT( startMip < 4 );

	// try to find a free location in the detail cache.
	unsigned int cacheTileX = 0;
	unsigned int cacheTileY = 0;
	if ( !gGrUberDetailCache->ReserveRegion( cacheTileX, cacheTileY, width, height ) )
		return;

	// write to the atlas.
	WriteAtlas( x, y, width, height, ( unsigned char )cacheTileX, ( unsigned char )cacheTileY );

	// iterate through each miplevel and pack data.
	for ( unsigned int mip = startMip; mip < 4; ++mip )
	{
		// calculate the tiles needed for the current mip-level.
		unsigned int curX = x >> mip;
		unsigned int curY = y >> mip;
		unsigned int endX = curX + width;
		unsigned int endY = curY + height;

		// clip the current region.
		curX = Max( curX, clipMinX >> mip );
		curY = Max( curY, clipMinY >> mip );
		endX = Min( endX, clipMaxX >> mip );
		endY = Min( endY, clipMaxY >> mip );

		// iterate through each tile.
		STileKey tileKey;
		tileKey.bits.mip = mip;
		for ( unsigned int localY = 0; curY < endY; ++curY, ++localY )
		{
			for ( unsigned int localX = 0; curX < endX; ++curX, ++localX )
			{
				// try to find the tile in the cache.
				tileKey.bits.x = curX;
				tileKey.bits.y = curY;
				void* tileIter = _tileCache.Find( tileKey.hash, tileKey );
				if ( tileIter )
				{
					// cache the tile.
					STileData& tileData = _tileCache.GetValue( tileIter );
					if ( IsLockFree( tileData.lock ) )
					{
						// upload the tile to the detail cache.
						gGrUberDetailCache->UploadTile( mip, cacheTileX + localX, cacheTileY + localY,
							TILE_BUMP_OFFSET( tileData.data ), TILE_DIFFUSE_OFFSET( tileData.data ),
							TILE_SPECULAR_OFFSET( tileData.data ) );
					}
				}
				else
				{
					// get the chunk in the file where the tile resides.
					unsigned int chunk = _fileAtlas->FindTileChunk( mip, curX, curY );
					if ( chunk == 0 )
						continue;

					// get the tile we're going to be replacing.
					void* oldestIter = _tileCache.GetOldest();
					STileData& tileData = _tileCache.GetValue( oldestIter );

					// make sure the tile isn't being loaded from disk.
					WaitLockFree( tileData.lock );

					// send a request for the tile data.
					gGrUTLoader->DecodeChunk( tileData.data, tileData.lock, _file->GetHandle(), chunk );

					// now add a new item into the cache.
					_tileCache.Insert( tileKey.hash, tileKey, tileData );
				}
			}
		}
	}
}

//----------------------------------------------------------
void
GrUberTexture::BindLowResTextures( unsigned int bumpSampler, unsigned int diffuseSampler,
								   unsigned int specularSampler )
{
	// bind the low resolution textures to the device.
	_lowResBump->Bind( bumpSampler );
	_lowResDiffuse->Bind( diffuseSampler );
	_lowResSpecular->Bind( specularSampler );
}

//----------------------------------------------------------
void
GrUberTexture::ResetAtlas()
{
	// get the current atlas memory.
	unsigned int atlasEraseCount = _atlasEraseCount;

	// do a "spot cleanup" of the atlas if possible.
	if ( atlasEraseCount <= MAX_ATLAS_WRITES )
	{
		for ( unsigned int i = 0; i < atlasEraseCount; ++i )
		{
			SAtlasErase& erase = _atlasEraseQueue[ i ];
			for ( unsigned int y = erase.y; y < erase.endY; ++y )
			{
				unsigned int* curPel = ( unsigned int* )DETAIL_ATLAS_ADDR( _detailAtlasMem, erase.x, y );
				for ( unsigned int x = erase.x; x < erase.endX; ++x, ++curPel )
					*curPel = 0xFFFFFFFF;
			}
		}
	}
	else
	{
		// clear the whole atlas.
		MemSet( _detailAtlasMem, 0xFF, DETAIL_ATLAS_MEM_SIZE );
	}
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
void
GrUberTexture::Init()
{
	// initialize the detail tile cache by filling it with tiles that
	// don't (can't) actually exist.
	STileKey key;
	key.bits.mip = ~0;

	// allocate the tile locks.
	_tileLocks = new int[ DETAIL_TILE_CACHE_SIZE ];

	// fill in the tile cache.
	unsigned char* tileMem = ( unsigned char* )AlignedAlloc( 2048, DETAIL_TILE_CACHE_SIZE * DETAIL_TILE_SIZE );
	STileData tileData;
	for ( unsigned int i = 0; i < DETAIL_TILE_CACHE_SIZE; ++i, tileMem += DETAIL_TILE_SIZE )
	{
		// fill out the rest of the current key.
		key.bits.x = i & 2043;
		key.bits.y = i >> 11;

		// initialize the tile data.
		tileData.data = tileMem;
		tileData.lock = InitLock( _tileLocks + i );

		// add the invalid tile to the cache.
		_tileCache.Insert( key.hash, key, tileData );
	}

	// allocate the detail atlas memory.
	_atlasEraseCount = 0;
	_detailAtlasMem = new unsigned char[ DETAIL_ATLAS_MEM_SIZE ];
	MemSet( _detailAtlasMem, 0xFF, DETAIL_ATLAS_MEM_SIZE );

	// allocate base miplevels.
	unsigned int kTexFlags = GrTextureBase::kNoCompress | GrTextureBase::kNoMipMap;
	_lowResBump = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( "lowres_bump",
		LOWRES_TEXTURE_WIDTH, LOWRES_TEXTURE_HEIGHT, GrTextureBase::EF_DXT5R, kTexFlags, false );
	_lowResDiffuse = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( "lowres_diffuse",
		LOWRES_TEXTURE_WIDTH, LOWRES_TEXTURE_HEIGHT, GrTextureBase::EF_DXT5, kTexFlags, false );
	_lowResSpecular = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( "lowres_specular",
		LOWRES_TEXTURE_WIDTH, LOWRES_TEXTURE_HEIGHT, GrTextureBase::EF_DXT5, kTexFlags, false );

	// allocate detail textures.
	_detailAtlas = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( "ubertex_atlas",
		DETAIL_ATLAS_WIDTH, DETAIL_ATLAS_HEIGHT, GrTextureBase::EF_RG16, kTexFlags, false );
}

//----------------------------------------------------------
void
GrUberTexture::WriteAtlas( unsigned int x, unsigned int y, unsigned int width, unsigned int height,
						   unsigned char cacheX, unsigned int cacheY )
{
	// determine which group the tiles are in.
	unsigned int patchX = x / GR_UBERTEX_PATCH_BOUNDARY;
	unsigned int patchY = y / GR_UBERTEX_PATCH_BOUNDARY;
	unsigned int patchEndX = ( x + width + GR_UBERTEX_PATCH_BOUNDARY - 1 ) / GR_UBERTEX_PATCH_BOUNDARY;
	unsigned int patchEndY = ( y + height + GR_UBERTEX_PATCH_BOUNDARY - 1 ) / GR_UBERTEX_PATCH_BOUNDARY;

	// queue an atlas erase entry.
	if ( _atlasEraseCount < 4096 )
	{
		SAtlasErase& curEntry = _atlasEraseQueue[ _atlasEraseCount ];
		curEntry.x = patchX;
		curEntry.y = patchY;
		curEntry.endX = patchEndX;
		curEntry.endY = patchEndY;

		// advance the erase count.
		_atlasEraseCount += 1;
	}
	else
		_atlasEraseCount = ~0;

	// index into the atlas.
	unsigned char* atlasMem = _detailAtlasMem;
	for ( ; patchY < patchEndY; ++patchY )
	{
		for ( ; patchX < patchEndX; ++patchX )
		{
			// get the address of the current pixel.
			unsigned char* atlasPel = DETAIL_ATLAS_ADDR( atlasMem, patchX, patchY );

			// store the indirection into our texture where RG = XY.
			atlasPel[ 0 ] = cacheX;
			atlasPel[ 1 ] = cacheY;
		}
	}
}
