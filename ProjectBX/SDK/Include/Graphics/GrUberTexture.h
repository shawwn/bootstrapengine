//----------------------------------------------------------
// File:		GrUberTexture.h
// Author:		Kevin Bray
// Created:		12-21-08
//
// Purpose:		To manage an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ULRUHash.h"
#include "URefCount.h"
#include "UPath.h"

// forward declarations.
class GrUTAtlas;
class GrTexture;
class RFile;

//**********************************************************
// class GrUberTexture
//**********************************************************
class GrUberTexture : public URefCount
{
public:
	enum ECONSTANTS
	{
		MAX_ATLAS_WRITES = 4096
	};

	GrUberTexture( const UPath& filePath );
	GrUberTexture( const UPath& filePath, const void* tileUsageBitmap, const char* metaData = 0,
				   unsigned int metaDataSize = 0 );
	~GrUberTexture();

	// returns the name of the ubertexture.
	const UPath&		GetName() const				{	return _name;					}

	// returns true if the ubertexture is valid.
	bool				IsValid() const				{	return ( _fileAtlas != 0 );		}

	// caches the detail tiles for rendering.
	void				CacheDetailTiles( unsigned int startMip, unsigned int x, unsigned int y,
										  unsigned int width, unsigned int height,
										  unsigned int clipMinX, unsigned int clipMinY,
										  unsigned int clipMaxX, unsigned int clipMaxY );

	// binds the ubertexture lowres textures to the device.
	void				BindLowResTextures( unsigned int bumpSampler, unsigned int diffuseSampler,
											unsigned int specularSampler );

	// clears the detail cache atlas.  This should be called at
	// the beginning of each frame.
	void				ResetAtlas();

private:
	// tile key structure.
	union STileKey
	{
		struct
		{
			int mip : 10;
			int x : 11;
			int y : 11;
		} bits;
		int hash;

		bool operator == ( const STileKey& rhs ) const	{	return hash == rhs.hash;	}
	};

	// tile data structure.
	struct STileData
	{
		volatile int* lock;
		void* data;
	};

	// atlas erase queue structure.
	struct SAtlasErase
	{
		unsigned char x;
		unsigned char y;
		unsigned char endX;
		unsigned char endY;
	};

	// tile hash table.
	typedef ULRUHash< STileKey, STileData > TileHash;

	// finds a free location in the detail textures.
	void				Init();
	void				WriteAtlas( unsigned int x, unsigned int y, unsigned int width, unsigned int height,
									unsigned char cacheX, unsigned int cacheY );

	// file name.
	UPath				_name;

	// ubertexture database info.
	URef< RFile >		_file;
	GrUTAtlas*			_fileAtlas;

	// file caching info.
	TileHash			_tileCache;
	volatile int*		_tileLocks;

	// detail texture packing data.
	unsigned char*		_detailTileUsage[ 7 ];

	// detail atlas memory (2 miplevels per atlas).
	unsigned char*		_detailAtlasMem;
	SAtlasErase			_atlasEraseQueue[ MAX_ATLAS_WRITES ];
	unsigned int		_atlasEraseCount;

	// lowres textures.
	URef< GrTexture >	_lowResBump;
	URef< GrTexture >	_lowResDiffuse;
	URef< GrTexture >	_lowResSpecular;

	// atlas texture for details.
	URef< GrTexture >	_detailAtlas;
};
