//----------------------------------------------------------
// File:        GrUberDetailCache.cpp
// Author:      Kevin Bray
// Created:     12-11-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUberDetailCache.h"

// project includes.
#include "GrTextureMgr.h"
#include "GrTexture.h"
#include "GrUTAtlas.h"
#include "GrUTLoader.h"
#include "RFileMgr.h"
#include "RFile.h"

// for std::sort.
#include <algorithm>

// tile cache singleton.
GrUberDetailCache* gGrUberDetailCache = 0;

// ubertexture atlas info.
#define UBERTEX_DETAIL_ATLAS_NAME           "UberTexture_DetailAtlas"
#define UBERTEX_DETAIL_ATLAS_WIDTH          ( 4 * ( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY ) )
#define UBERTEX_DETAIL_ATLAS_HEIGHT         ( 4 * ( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY ) )

// ubertexture cache size.
#define DETAIL_CACHE_BUMP_NAME              "UberDetailCache_Bump"
#define DETAIL_CACHE_DIFFUSE_NAME           "UberDetailCache_Diffuse"
#define DETAIL_CACHE_SPECULAR_NAME          "UberDetailCache_Specular"
#define DETAIL_CACHE_MASK_NAME              "UberDetailCache_Mask"
#define DETAIL_CACHE_TEXTURE_DIM            4096
#define DETAIL_CACHE_MIP_PAGE_DIM           2048
#define DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES  ( DETAIL_CACHE_MIP_PAGE_DIM / GR_UBERTEX_TILE_SIZE )
#define DETAIL_CACHE_MASK_DIM               ( DETAIL_CACHE_TEXTURE_DIM / GR_UBERTEX_TILE_SIZE )
#define DETAIL_CACHE_MASK_MIP_PAGE_DIM      DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES
#define DETAIL_CACHE_MASK_DATA_SIZE         ( ( DETAIL_CACHE_MASK_DIM * DETAIL_CACHE_MASK_DIM ) / 8 )
#define DETAIL_CACHE_MASK_DXT5R_BLOCK_COUNT 256

// DXT5R compressed mask data.
DECLARE_ALIGNED_16( __int64, _maskTempDXT5R[ DETAIL_CACHE_MASK_DXT5R_BLOCK_COUNT ] );

// DXT5R compression lookup tables
__int64* _maskCodeLUT = 0;


//**********************************************************
// class GrUberDetailCache
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUberDetailCache::GrUberDetailCache()
: _curPackX( 0 )
, _curPackY( 0 )
, _curPackMaxY( 0 )
{
    // allocate resources.
    gGrUberDetailCache = this;

    // build the DXT5R lookup table.
    _maskCodeLUT = ( __int64* )AlignedAlloc( 16, 65536 * sizeof( __int64 ) );
    for ( unsigned int i = 0; i < 65536; ++i )
    {
        __int64 value = 0xFF00;
        for ( unsigned int j = 0; j < 16; ++j )
        {
            // build the DXT code for the current value.
            int bit = ( ( i >> j ) & 1 );
            value |= ( bit << ( ( j * 3 ) + 16 ) );
        }

        // store the current DXT code in the lookup table.
        _maskCodeLUT[ i ] = value;
    }

    // initialize packing state.
    ClearPackingState();

    // allocate memory.
    AllocDetailTextures();
}

//----------------------------------------------------------
GrUberDetailCache::~GrUberDetailCache()
{
    // clear the singleton pointer.
    gGrUberDetailCache = 0;

    // free the mask data.
    delete[] _maskData;

    // free the mask-code lookup table.
    AlignedFree( _maskCodeLUT );
    _maskCodeLUT = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrUberDetailCache::ReserveRegion( unsigned int& tileX, unsigned int& tileY, unsigned int width, unsigned int height )
{
    // make sure the width and height are acceptable sizes.
    B_ASSERT( width < DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES );
    B_ASSERT( height < DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES );

    // check to see if the region can fit in the current row.  If
    // not, advance to the next free row.
    unsigned int endX = _curPackX + width;
    if ( endX > DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES )
    {
        // advance to the next row.
        _curPackX = 0;
        _curPackY = _curPackMaxY;
        endX = width;
    }

    // make sure there is enough room in the cache for the region
    // requested.  If there isn't, simply return false to indicate
    // allocation failure.
    unsigned int endY = _curPackY + height;
    if ( endY > DETAIL_CACHE_MIP_PAGE_DIM_IN_TILES )
        return false;

    // store the outgoing positions.
    tileX = _curPackX;
    tileY = _curPackY;

    // update the packing state.
    _curPackX = endX;
    _curPackMaxY = Max( _curPackMaxY, endY );

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
void
GrUberDetailCache::UploadTile( unsigned int lod, unsigned int tileX, unsigned int tileY, const void* bump,
                               const void* diffuse, const void* specular )
{
    // upload the tile to the texture.
    unsigned int pageX = ( lod & 1 );
    unsigned int pageY = ( lod & 2 );

    // translate the destination to the correct page in the cache.
    unsigned int pelX = tileX * GR_UBERTEX_TILE_SIZE + pageX * DETAIL_CACHE_MIP_PAGE_DIM;
    unsigned int pelY = tileY * GR_UBERTEX_TILE_SIZE + pageY * DETAIL_CACHE_MIP_PAGE_DIM;

    // upload the tile.
    // bump.
    _detailBump->UploadPixelDataCompressed( 0, pelX, pelY, GR_UBERTEX_TILE_SIZE, GR_UBERTEX_TILE_SIZE,
        bump, GL_COMPRESSED_RED_RGTC1, 2048 );

    // diffuse.
    _detailDiffuse->UploadPixelDataCompressed( 0, pelX, pelY, GR_UBERTEX_TILE_SIZE, GR_UBERTEX_TILE_SIZE,
        diffuse, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 4096 );

    // specular.
    _detailSpecular->UploadPixelDataCompressed( 0, pelX, pelY, GR_UBERTEX_TILE_SIZE, GR_UBERTEX_TILE_SIZE,
        specular, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 4096 );

    // calculate the mask X and Y values.
    unsigned int maskX = tileX + pageX * DETAIL_CACHE_MASK_MIP_PAGE_DIM;
    unsigned int maskY = tileY + pageY * DETAIL_CACHE_MASK_MIP_PAGE_DIM;

    // store the mask value.
    unsigned int maskWordIdx = ( ( maskY >> 2 ) * ( DETAIL_CACHE_MASK_DIM >> 2 ) ) + ( maskX >> 2 );
    unsigned int maskBitIdx = ( ( maskY & 3 ) << 2 ) | ( maskX & 3 );
    _maskData[ maskWordIdx ] |= ( 1 << maskBitIdx );
}

//----------------------------------------------------------
void
GrUberDetailCache::PrepareDetailCache()
{
    // build the mask data in DXT5R format.
    for ( unsigned int i = 0; i < DETAIL_CACHE_MASK_DXT5R_BLOCK_COUNT; ++i )
        _maskTempDXT5R[ i ] = _maskCodeLUT[ _maskData[ i ] ];

    // upload the mask data.
    _detailMask->UploadPixelDataCompressed( 0, 0, 0, DETAIL_CACHE_MASK_DIM, DETAIL_CACHE_MASK_DIM, _maskTempDXT5R,
        GL_COMPRESSED_RED_RGTC1, 2048 );

    // clear the packing state.
    ClearPackingState();

    // clear the mask data.
    MemSet( _maskData, 0, DETAIL_CACHE_MASK_DATA_SIZE );
}

//----------------------------------------------------------
void
GrUberDetailCache::BindDetailTextures( unsigned int maskSampler, unsigned int bumpSampler,
                                       unsigned int diffuseSampler, unsigned int specularSampler )
{
    _detailMask->Bind( maskSampler );
    _detailBump->Bind( bumpSampler );
    _detailDiffuse->Bind( diffuseSampler );
    _detailSpecular->Bind( specularSampler );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrUberDetailCache::AllocDetailTextures()
{
    // allocate the detail cache textures.
    const GrTextureBase::Flags kFlags = GrTextureBase::kNoMipMap | GrTextureBase::kNoCompress;

    // bump.
    _detailBump = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( DETAIL_CACHE_BUMP_NAME,
        DETAIL_CACHE_TEXTURE_DIM, DETAIL_CACHE_TEXTURE_DIM, GrTextureBase::EF_DXT5R, kFlags, false );

    // diffuse.
    _detailDiffuse = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( DETAIL_CACHE_DIFFUSE_NAME,
        DETAIL_CACHE_TEXTURE_DIM, DETAIL_CACHE_TEXTURE_DIM, GrTextureBase::EF_DXT5, kFlags, false );

    // specular.
    _detailSpecular = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( DETAIL_CACHE_SPECULAR_NAME,
        DETAIL_CACHE_TEXTURE_DIM, DETAIL_CACHE_TEXTURE_DIM, GrTextureBase::EF_DXT5, kFlags, false );

    // mask texture.
    _detailMask = ( GrTexture* )( GrTextureBase* )gGrTextureMgr->GetEmptyTexture( DETAIL_CACHE_MASK_NAME,
        DETAIL_CACHE_MASK_DIM, DETAIL_CACHE_MASK_DIM, GrTextureBase::EF_DXT5R, kFlags, false );

    // allocate mask memory.
    _maskData = new unsigned short[ DETAIL_CACHE_MASK_DATA_SIZE / 2 ];
    MemSet( _maskData, 0, DETAIL_CACHE_MASK_DATA_SIZE );
}

//----------------------------------------------------------
void
GrUberDetailCache::ClearPackingState()
{
    _curPackX = 0;
    _curPackY = 0;
    _curPackMaxY = 0;
}
