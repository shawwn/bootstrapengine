//----------------------------------------------------------
// File:		GrUberTextureMgr.h
// Author:		Kevin Bray
// Created:		01-01-09
//
// Purpose:		To manage ubertextures.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrUberTexture.h"

// std c++ includes.
#include <map>

//**********************************************************
// class GrUberTextureMgr
//**********************************************************
class GrUberTextureMgr
{
public:
	GrUberTextureMgr();
	~GrUberTextureMgr();

	// creates a new ubertexture.  Returns null if the ubertexture
	// already exists.
	URef< GrUberTexture >	CreateUberTexture( const UPath& name, const void* tileUsageBitMap, const char* metaData = 0,
											   unsigned int metaDataSize = 0 );

	// gets an existing ubertexture.
	URef< GrUberTexture >	GetUberTexture( const UPath& name );

	// collect garbage.
	void					CollectGarbage();

private:
	typedef std::map< UPath, URef< GrUberTexture > > UberTextureMap;

	UberTextureMap			_uberTextures;
};
extern GrUberTextureMgr* gGrUberTextureMgr;
