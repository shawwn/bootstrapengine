//----------------------------------------------------------
// File:        GrUberDetailCache.h
// Author:      Kevin Bray
// Created:     12-11-08
//
// Purpose:     To cache ubertexture tiles.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ULRUHash.h"
#include "GrUTLoader.h"

// forward declarations.
class GrUTAtlas;
class GrTexture;
class UPath;
class RFile;

//**********************************************************
// class GrUberDetailCache
//**********************************************************
class GrUberDetailCache
{
public:
    GrUberDetailCache();
    ~GrUberDetailCache();

    // allocates a region in the detail cache.
    bool                ReserveRegion( unsigned int& tileX, unsigned int& tileY, unsigned int width, unsigned int height );

    // uploads a tile to the detail cache.  Note that the formats are
    // expected as such:  bump DXT5R, diffuse DXT5, specular DXT5.
    void                UploadTile( unsigned int lod, unsigned int tileX, unsigned int tileY, const void* bump,
                                    const void* diffuse, const void* specular );

    // prepares the L1-cache for rendering.
    void                PrepareDetailCache();

    // binds the detail textures to the graphics device.
    void                BindDetailTextures( unsigned int maskSampler, unsigned int bumpSampler,
                                            unsigned int diffuseSampler, unsigned int specularSampler );

private:
    // allocates detail textures.
    void                AllocDetailTextures();
    void                ClearPackingState();

    // packing state.
    unsigned int        _curPackX;
    unsigned int        _curPackY;
    unsigned int        _curPackMaxY;

    // mask memory.
    unsigned short*     _maskData;

    // detail cache.
    URef< GrTexture >   _detailMask;
    URef< GrTexture >   _detailBump;
    URef< GrTexture >   _detailDiffuse;
    URef< GrTexture >   _detailSpecular;
};
extern GrUberDetailCache* gGrUberDetailCache;
