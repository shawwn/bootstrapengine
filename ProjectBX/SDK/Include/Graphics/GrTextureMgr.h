//----------------------------------------------------------
// File:		GrTextureMgr.h
// Author:		Kevin Bray
// Created:		10-20-04
//
// Purpose:		To allow manipulation of texture pools.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"
#include "GrConfig.h"
#include "GrTextureBase.h"
#include "UPath.h"

// std c++ includes.
#include <map>
#include <vector>

class GrImage;
class GrTextureCube;

//**********************************************************
// class GrTextureMgr
//**********************************************************
class GrTextureMgr
{
public:
	GrTextureMgr( EGRQUALITY texturingQuality );
	~GrTextureMgr();

	// call once every frame.
	void					Update();

	// requires a context reset to take effect on already created textures.
	void					SetTexturingQuality( EGRQUALITY quality )	{	_texturingQuality = quality;	}
	EGRQUALITY				GetTexturingQuality() const					{	return _texturingQuality;		}

	URef< GrTextureBase >	GetTextureFile( const UPath& imageName, GrTextureBase::EFORMAT format,
											GrTextureBase::Flags flags, bool cacheImage = false, bool cache = true );

	URef< GrTextureBase >	GetTextureImage( GrImage* image, GrTextureBase::EFORMAT format, GrTextureBase::Flags flags, bool cache = true );

	URef< GrTextureBase >	GetEmptyTexture( const UPath& name, unsigned int width, unsigned int height,
											 GrTextureBase::EFORMAT format, GrTextureBase::Flags flags, bool cache = true );

	URef< GrTextureBase >	GetTextureCubeFiles( const UPath** imageNames, GrTextureBase::EFORMAT format,
												 GrTextureBase::Flags flags, bool cacheImages = false, bool cache = true );
	URef< GrTextureBase >	GetTextureCubeImages( GrImage** image, unsigned int dim, GrTextureBase::EFORMAT format,
												  GrTextureBase::Flags flags, bool cache = true );

	URef< GrTextureBase >	GetEmptyTextureCube( const UPath& name, unsigned int dim, GrTextureBase::EFORMAT format,
												 GrTextureBase::Flags flags, bool cache = true );

	URef< GrTextureBase	>	FindTexture( const UPath& imageName, GrTextureBase::EFORMAT format, unsigned int flags ) const;
	URef< GrTextureBase	>	FindTexture( const UPath& imageName ) const;

/*
	// gets a texture.  Creates it from a file if it doesn't exist.
	GrTextureInst			LoadTexture( const UPath& texName, const UPath& imageName, GrTextureBase::EFORMAT format,
										 GrTextureInst::EFILTERMODE filter, unsigned int flags );

	// gets a texture.  Creates it with the parameters given if it doesn't exist.
	URef< GrTexture >		GetTexture( const UPath& texName, GrTextureBase::EFORMAT format, unsigned int width,
									    unsigned int height, GrTextureBase::EFILTERMODE filterMode, unsigned int flags );

	// finds a texture based on it's name and parameters.
	URef< GrTexture >		FindTexture( const UPath& texName, GrTextureBase::EFORMAT format, GrTextureBase::EFILTERMODE filter,
										 unsigned int flags ) const;

	// loads, creates, or adds a cube texture.
	URef< GrTextureCube >	CreateTextureCube( const UPath& texName, GrTextureBase::EFORMAT format, unsigned int dim,
											   unsigned int flags, bool filter = false );
	URef< GrTextureCube >	GetTextureCube( const UPath& posX, const UPath& negX, const UPath& posY, const UPath& negY,
											const UPath& posZ, const UPath& negZ, bool mipmap, bool filter = false );
	URef< GrTextureCube >	GetSysTextureCube( const UPath& name );
*/
	// called when the context is recreated ( display mode changes, etc ).  This can also be used to force
	// all textures to be destroyed and then recached.  This is useful for quality hint changes.
	void					PreContextReset();
	void					PostContextReset();

private:

	typedef std::multimap< UPath, URef< GrTextureBase > > TextureMap;
	typedef std::vector< URef< GrTextureBase > > TextureVec;
//	typedef std::map< tstring, URef< GrTextureCube > > TextureCubeMap;
/*
	URef< GrTexture >		AddTexture( GrImage* image, GrTextureBase::EFORMAT format, GrTexture::EFILTERMODE filter,
										unsigned int flags );
	void					CreateSysTextures();
	void					RebuildSysTexture( URef< GrTexture > texture );
	void					BuildSimpleTextureCube( URef< GrImage > image );
	void					BuildTextureName( UPath& out, const UPath& imageName, GrTexture::EFORMAT format,
											  GrTexture::EFILTERMODE filter, unsigned int flags );
	TextureMap::const_iterator	FindTextureIter( const UPath& imageName, GrTexture::EFORMAT format, GrTexture::EFILTERMODE filter,
												 unsigned int flags ) const;
*/
	void					CreateSysTextures();
	void					InsertTexture( URef< GrTextureBase > texture );
	void					AddAndInsertTexture( URef< GrTextureBase > texture );

	TextureMap				_textures;
	TextureVec				_textureVec;
//	TextureCubeMap			_textureCubes;
//	TextureCubeMap			_sysTextureCubes;

	EGRQUALITY				_texturingQuality;
};

extern GrTextureMgr *gGrTextureMgr;
