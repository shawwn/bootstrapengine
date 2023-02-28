//----------------------------------------------------------
// File:		GrUTSysTileCache.h
// Author:		Kevin Bray
// Created:		11-22-2008
//
// Purpose:		To cache tiles in system memory.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class UPath;
class GrTexture;

//**********************************************************
// class GrUTSysTileCache
//**********************************************************
class GrUTSysTileCache
{
public:
	GrUTSysTileCache( unsigned int tileCount, bool dxtCompress );
	~GrUTSysTileCache();

	// opens an ubertexture and returns it's ID.
	unsigned int	OpenUberTexture( const UPath& path );
	void			CloseUberTexture( unsigned int id );

	// gets a tile's id.
	__int64			GetTileID( unsigned int texId, unsigned int mipLevel, unsigned int x, unsigned int y );

	// uploads a tile to the texture specified.  Note that this
	// function will wait for a tile to be fetched if it's not
	// already in the cache.
	void			Upload( GrTexture* target, unsigned int dstX, unsigned int dstY, __int64 tileId );

	// fetches a tile that's not in the cache.
	void			PreFetch( __int64 tileId );

	// evicts a tile from the cache.
	void			Evict( __int64 tileId );

	// returns true if a tile is resident in the cache.
	bool			IsResident( __int64 tileId ) const;

	// invalidates a tile in the cache.  This causes a tile that's in
	// the cache to be re-fetched.
	void			Invalidate( __int64 tileId ) const;

private:
	struct STile
	{
		unsigned short uberTexId;
		unsigned short mip;
		unsigned short x;
		unsigned short y;
		unsigned int bufferLoc;
	};

	// array of tile information.  There is a 1:1 corelation to tiles
	// in the memory cache.
	STile*			_tileInfo;
	void*			_tileMem;
	bool			_compressed;
};
