//----------------------------------------------------------
// File:		GrSoftwareTextureMgr.h
// Author:		Shawn Presser
// Created:		09-13-08
//
// Purpose:		To provide a centralized location to load GrSoftwareTextures.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "URef.h"
#include "UPath.h"

// std c++ headers.
#include <map>

// forward declarations.
class GrSoftwareTexture;

//**********************************************************
// class GrSoftwareTextureMgr
//**********************************************************
class GrSoftwareTextureMgr
{
public:
    GrSoftwareTextureMgr();
    ~GrSoftwareTextureMgr();

    // tries to find and return an existing software texture.  
    // If one doesn't exist, it is constructed, added, then returned.
    URef< GrSoftwareTexture >       GetTexture( const UPath& filePath );

    // tries to find and return an existing software texture.
    URef< GrSoftwareTexture >       FindTexture( const UPath& filePath );

private:
    typedef std::map< UPath, URef< GrSoftwareTexture > > TextureContainer;

    TextureContainer        _textures;
};
extern GrSoftwareTextureMgr* gGrSoftwareTextureMgr;