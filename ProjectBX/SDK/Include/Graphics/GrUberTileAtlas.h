//----------------------------------------------------------
// File:		GrUberTileAtlas.h
// Author:		Kevin Bray
// Created:		12-16-08
//
// Purpose:		To manage a two-level tile atlas for an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrTexture;

//**********************************************************
// class GrUberTileAtlas
//**********************************************************
class GrUberTileAtlas
{
public:
	GrUberTileAtlas();
	~GrUberTileAtlas();

	// clears the atlas.
	void				Clear();

	// returns a number that identifies the group.
	unsigned int		AddGroup( unsigned int mip, unsigned int minX, unsigned int minY,
								  unsigned int maxX, unsigned int maxY );

	// adds a tile to the group specified.
	void				AddGroupTile( unsigned int group, unsigned int mip, unsigned int x,
									  unsigned int y );

	// flushes the tile data to the textures.
	void				Flush();

	// binds the atlas specified.
	//
	// Atlas format notes:
	// red = scale-UV, green = offset-U, blue = offset-V.
	//
	// in the shader:
	// - if red > 0.75, then use the fallback texture.
	// - if alpha == 0, then use the lowres atlas for UVs for the
	//	 cache.
	// - if alpha == 1, then use the lowres atlas for detail atlas
	//	 UVs, and use the detail atlas UVs for the cache.
	//
	// in all, this means there are 2 lookups for each texture
	// channel (bump, diffuse, specular), and 2 atlas lookups.  This
	// means that a total of 8 samplers are necessary, but things can
	// be done to limit bandwidth requirements.  For instance, when a
	// texture isn't used, we can sample from 0,0 to ensure that
	// we're not pulling data into the texture cache.
	void				BindLowResAtlas( unsigned int sampler );
	void				BindDetailAtlas( unsigned int sampler );

private:
	void				WriteData( unsigned char* dst, unsigned char scale, unsigned char offsetX,
								   unsigned char offsetY );

	unsigned char*		lowResStore;
	unsigned char*		detailStore;
	URef< GrTexture >	lowResTexture;
	URef< GrTexture >	detailTexture;

	// first free detail chunk (out of 64, 32x32 chunks).
	unsigned int		detailFree;
};
