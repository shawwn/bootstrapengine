//----------------------------------------------------------
// File:        GrPolygonUberMapping.cpp
// Author:      Kevin Bray
// Created:     12-27-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonUberMapping.h"

// project includes.
#include "GrPolygonUberPatch.h"
#include "GrPolygonGroup.h"
#include "GrPolygon.h"
#include "GrUberTextureMgr.h"
#include "GrUberTexture.h"
#include "GrConfig.h"
#include "GrDebug.h"
#include "RFileMgr.h"
#include "RFile.h"

// size of each ubertexture in patches.
#define UBERTEX_SIZE_IN_PATCHES         ( GR_UBERTEX_SIZE_IN_TEXELS / GR_UBERTEX_PATCH_BOUNDARY )
#define UBERTEX_PATCH_SIZE_IN_TILES     ( GR_UBERTEX_PATCH_BOUNDARY / GR_UBERTEX_TILE_SIZE )

#define UBERTEX_FLAGS                   ( RFileMgr::kFlagCreateAlways | RFileMgr::kFlagRead | RFileMgr::kFlagWrite | RFileMgr::kFlagDMA )

#define DEBUG_RASTERIZER                0

// debug rasterizer switches.
#if DEBUG_RASTERIZER

unsigned char* _debugImage = 0;

#define INIT_DEBUG_IMAGE()              { _debugImage = ( unsigned char* )AlignedAlloc( 16, 2048 * 2048 ); MemSet( _debugImage, 0, 2048 * 2048 ); }
#define FREE_DEBUG_IMAGE()              { AlignedFree( _debugImage ); }
#define SAVE_DEBUG_IMAGE()              { GrSaveTGA( "ubermask.tga", _debugImage, 2048, 2048, 1 ); }
#define DEBUG_IMAGE_WRITE( x, y )       { ( _debugImage[ ( y ) * 2048 + ( x ) ] = 255 ); }

#else

#define INIT_DEBUG_IMAGE()              { }
#define FREE_DEBUG_IMAGE()              { }
#define SAVE_DEBUG_IMAGE()              { }
#define DEBUG_IMAGE_WRITE( x, y )       { }

#endif

void BitMaskRasterize( unsigned char* bitMask, SPolygonVertex* verts, unsigned int* mortonTable );

//**********************************************************
// class GrPolygonUberMapping
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonUberMapping::GrPolygonUberMapping( unsigned int uberTexIdx )
: _uberTexIdx( uberTexIdx )
, _curX( 0 )
, _curY( 0 )
, _maxY( 0 )
{

}

//----------------------------------------------------------
GrPolygonUberMapping::~GrPolygonUberMapping()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrPolygonUberMapping::AddPatch( GrPolygonUberPatch* patch )
{
    // make sure the patch passed in is valid!  If it's too big or
    // too small, then the patch cannot be packed into an
    // ubertexture.
    B_ASSERT( patch->GetUVSizeInTiles().x > 0.0f && patch->GetUVSizeInTiles().x <= GR_UBERTEX_SIZE_IN_TILES );
    B_ASSERT( patch->GetUVSizeInTiles().y > 0.0f && patch->GetUVSizeInTiles().y <= GR_UBERTEX_SIZE_IN_TILES );

    // calculate the size of the patch in patch-tiles (a patch tile
    // is the size of GR_UBERTEX_PATCH_BOUNDARY).
    unsigned int patchWidth = ( int )Ceil( UBERTEX_SIZE_IN_PATCHES * ( patch->GetUVSizeInTiles().x / GR_UBERTEX_SIZE_IN_TILES ) );
    unsigned int patchHeight = ( int )Ceil( UBERTEX_SIZE_IN_PATCHES * ( patch->GetUVSizeInTiles().y / GR_UBERTEX_SIZE_IN_TILES ) );

    // determine if there is enough room for the patch in the current
    // row.
    unsigned int endX = ( _curX + patchWidth );
    if ( endX >= UBERTEX_SIZE_IN_PATCHES )
    {
        // advance to the next line.
        _curX = 0;
        _curY += _maxY;
        endX = patchWidth;
    }

    // determine if there is enough room for the patch in the current
    // ubertexture.
    unsigned int endY = ( _curY + patchHeight );
    if ( endY >= UBERTEX_SIZE_IN_PATCHES )
        return false;

    // pack the patch at the current location.
    patch->AssignToUberTexture( _uberTexIdx, _curX * UBERTEX_PATCH_SIZE_IN_TILES,
        _curY * UBERTEX_PATCH_SIZE_IN_TILES );

    // advance beyond the current patch and record the maximum
    // height.
    _curX = endX;
    _maxY = Max( _maxY, patchHeight );

    // add the patch to our vector of patches.
    _patches.push_back( patch );

    // return true to indicate that the patch was successfully added
    // to the ubertexture.
    return true;
}

//----------------------------------------------------------
unsigned int
GrPolygonUberMapping::GetPatchCount() const
{
    return ( unsigned int )_patches.size();
}

//----------------------------------------------------------
GrPolygonUberPatch*
GrPolygonUberMapping::GetPatch( unsigned int idx ) const
{
    B_ASSERT( idx < GetPatchCount() );
    return _patches[ idx ];
}

//----------------------------------------------------------
URef< GrUberTexture >
GrPolygonUberMapping::BuildUberTexture( const UPath& name ) const
{
    // initialize the rasterizer debugging image.
    INIT_DEBUG_IMAGE();

    // allocate a bitmask (one bit per tile) and clear it.
    unsigned int bitMaskSize = ( GR_UBERTEX_SIZE_IN_TILES / 8 ) * GR_UBERTEX_SIZE_IN_TILES;
    unsigned char* bitMask = new unsigned char[ bitMaskSize ];
    MemSet( bitMask, 0, bitMaskSize );

    // generate the morton table that's used to address individual
    // ubertexture tiles.
    unsigned int mortonTable[ 2048 ];
    for ( unsigned int i = 0; i < 2048; ++i )
    {
        mortonTable[ i ] = 0;
        unsigned int mask = 1;
        for ( unsigned int b = 0; b < 11; ++b, mask += mask )
            mortonTable[ i ] |= ( i & mask ) << b;
    }

    // iterate through all of the faces and generate a tile usage
    // bitmask.
    unsigned int patchCount = GetPatchCount();
    for ( unsigned int i = 0; i < patchCount; ++i )
    {
        // get the current patch.
        GrPolygonUberPatch* curPatch = GetPatch( i );

        // iterate through each polygon in the current patch.
        GrPolygonGroup* polygons = curPatch->GetPolygonGroup();
        unsigned int polyCount = curPatch->GetPolygonCount();
        for ( unsigned int j = 0; j < polyCount; ++j )
        {
            // get the current polygon.
            unsigned int curPolygonIdx = curPatch->GetPolygonIndex( j );
            const GrPolygon& curPolygon = polygons->GetPolygon( curPolygonIdx );

            // triangulate the current polygon.
            unsigned int triCount = curPolygon.GetTriangleCount();
            SPolygonVertex* triVerts = ( SPolygonVertex* )TempAlloc( 3 * triCount * sizeof( SPolygonVertex ) );
            curPolygon.Triangulate( triVerts );

            // iterate through each triangle in the polygon.
            for ( unsigned int k = 0; k < triCount; ++k )
            {
                // rasterize the current triangle.
                BitMaskRasterize( bitMask, triVerts + 3 * k, mortonTable );
            }

            // free the temp verts that we've allocated.
            TempFree( triVerts );
        }
    }

    // save the rasterizer debugging image.
    SAVE_DEBUG_IMAGE();
    FREE_DEBUG_IMAGE();

    // build a new ubertexture.
    GrUberTexture* newTexture = gGrUberTextureMgr->CreateUberTexture( name, bitMask );

    // free the tile usage bitmask.
    delete[] bitMask;

    // return it.
    return 0;
}


//**********************************************************
// File local functions
//**********************************************************

// line check.  0 if on positive side, 1 if on negative side.
#define LINE_SIDE( a, b, c, x, y )          ( ( ( a ) * ( ( double )( x ) ) + ( b ) * ( ( double )( y ) + 0 ) + ( c ) ) < 0.0 )

//----------------------------------------------------------
void
BitMaskRasterize( unsigned char* bitMask, SPolygonVertex* verts, unsigned int* mortonTable )
{
    double x0 = verts[ 0 ].uv.x * GR_UBERTEX_SIZE_IN_TILES;
    double y0 = verts[ 0 ].uv.y * GR_UBERTEX_SIZE_IN_TILES;
    double x1 = verts[ 1 ].uv.x * GR_UBERTEX_SIZE_IN_TILES;
    double y1 = verts[ 1 ].uv.y * GR_UBERTEX_SIZE_IN_TILES;
    double x2 = verts[ 2 ].uv.x * GR_UBERTEX_SIZE_IN_TILES;
    double y2 = verts[ 2 ].uv.y * GR_UBERTEX_SIZE_IN_TILES;

    // calculate line equations.
    double a0 = y1 - y0;
    double b0 = x0 - x1;
    double c0 = -( a0 * x0 + b0 * y0 );

    // determine if we need to flip the winding order.
    if ( ( a0 * x2 + b0 * y2 + c0 ) < 0 )
    {
        // swap vertex 0 and 1.
        Swap( x0, x1 );
        Swap( y0, y1 );

        // recalculate the line equation.
        a0 = -a0;
        b0 = -b0;
        c0 = -( a0 * x0 + b0 * y0 );
    }

    // calculate the line equation for edge 1.
    double a1 = y2 - y1;
    double b1 = x1 - x2;
    double c1 = -( a1 * x1 + b1 * y1 );

    // calculate the line equation for edge 2.
    double a2 = y0 - y2;
    double b2 = x2 - x0;
    double c2 = -( a2 * x2 + b2 * y2 );

    // calculate the bounding box for the current triangle.
    int minX = ( int )Floor( ( float )Min3( x0, x1, x2 ) );
    int minY = ( int )Floor( ( float )Min3( y0, y1, y2 ) );
    int maxX = ( int )Ceil( ( float )Max3( x0, x1, x2 ) );
    int maxY = ( int )Ceil( ( float )Max3( y0, y1, y2 ) );

    // iterate over blocks.
    int coverage0 = 0;
    int coverage1 = 0;
    int coverage2 = 0;
    int minBlockX = minX & ~7;
    int minBlockY = minY & ~7;
    int maxBlockX = maxX & ~7;
    int maxBlockY = maxY & ~7;
    for ( int by = minBlockY; by <= maxBlockY; by += 8 )
    {
        for ( int bx = minBlockX; bx <= maxBlockX; bx += 8 )
        {
            // check the coverage of the current block.
            coverage0 =  LINE_SIDE( a0, b0, c0, bx + 0, by + 0 );
            coverage0 += LINE_SIDE( a0, b0, c0, bx + 8, by + 0 );
            coverage0 += LINE_SIDE( a0, b0, c0, bx + 0, by + 8 );
            coverage0 += LINE_SIDE( a0, b0, c0, bx + 8, by + 8 );
            if ( coverage0 == 4 )
                continue;

            coverage1 =  LINE_SIDE( a1, b1, c1, bx + 0, by + 0 );
            coverage1 += LINE_SIDE( a1, b1, c1, bx + 8, by + 0 );
            coverage1 += LINE_SIDE( a1, b1, c1, bx + 0, by + 8 );
            coverage1 += LINE_SIDE( a1, b1, c1, bx + 8, by + 8 );
            if ( coverage1 == 4 )
                continue;

            coverage2 =  LINE_SIDE( a2, b2, c2, bx + 0, by + 0 );
            coverage2 += LINE_SIDE( a2, b2, c2, bx + 8, by + 0 );
            coverage2 += LINE_SIDE( a2, b2, c2, bx + 0, by + 8 );
            coverage2 += LINE_SIDE( a2, b2, c2, bx + 8, by + 8 );
            if ( coverage2 == 4 )
                continue;

            // if we cover the entire block.
            if ( ( coverage0 + coverage1 + coverage2 ) == 0 )
            {
                // calculate the starting location of the current
                // tile mask.
                unsigned int xBits = mortonTable[ bx ];
                unsigned int yBits = mortonTable[ by ];
                unsigned int index = ( yBits + yBits ) | xBits;

                // divide the index by the number of bits per byte.
                index >>= 3;

                // mark all 64 tiles as filled by performing a linear
                // write of 8-bytes.
                bitMask[ index + 0 ] = ~0;
                bitMask[ index + 1 ] = ~0;
                bitMask[ index + 2 ] = ~0;
                bitMask[ index + 3 ] = ~0;
                bitMask[ index + 4 ] = ~0;
                bitMask[ index + 5 ] = ~0;
                bitMask[ index + 6 ] = ~0;
                bitMask[ index + 7 ] = ~0;

#if DEBUG_RASTERIZER
                for ( unsigned int i = 0; i < 8; ++i )
                {
                    for ( unsigned int j = 0; j < 8; ++j )
                    {
                        DEBUG_IMAGE_WRITE( bx + j, by + i );
                    }
                }
#endif

                // move to the next block.
                continue;
            }
            else
            {
                // we cover a portion of the block, so we need to
                // rasterize the triangle over each pixel.
                int y = by;
                for ( int i = 0; i < 8; ++i, ++y )
                {
                    int x = bx;
                    for ( int j = 0; j < 8; ++j, ++x )
                    {
                        // check for coverage of the current pixel.
                        coverage0 =  LINE_SIDE( a0, b0, c0, x + 0, y + 0 );
                        coverage0 += LINE_SIDE( a0, b0, c0, x + 1, y + 0 );
                        coverage0 += LINE_SIDE( a0, b0, c0, x + 0, y + 1 );
                        coverage0 += LINE_SIDE( a0, b0, c0, x + 1, y + 1 );
                        if ( coverage0 == 4 )
                            continue;

                        coverage1 =  LINE_SIDE( a1, b1, c1, x + 0, y + 0 );
                        coverage1 += LINE_SIDE( a1, b1, c1, x + 1, y + 0 );
                        coverage1 += LINE_SIDE( a1, b1, c1, x + 0, y + 1 );
                        coverage1 += LINE_SIDE( a1, b1, c1, x + 1, y + 1 );
                        if ( coverage1 == 4 )
                            continue;

                        coverage2 =  LINE_SIDE( a2, b2, c2, x + 0, y + 0 );
                        coverage2 += LINE_SIDE( a2, b2, c2, x + 1, y + 0 );
                        coverage2 += LINE_SIDE( a2, b2, c2, x + 0, y + 1 );
                        coverage2 += LINE_SIDE( a2, b2, c2, x + 1, y + 1 );
                        if ( coverage2 == 4 )
                            continue;

                        // if the current pixel is at least partially
                        // covered, then we need to write a single
                        // value.
                        unsigned int xBits = mortonTable[ bx ];
                        unsigned int yBits = mortonTable[ by ];
                        unsigned int index = ( yBits + yBits ) | xBits;

                        // determine the byte offset and bit index.
                        unsigned int byteOffset = index >> 3;
                        unsigned int bitIndex = index & 7;

                        // set the current bit to 1.
                        bitMask[ byteOffset ] |= ( 1 << bitIndex );

                        // mark the debug image.
                        DEBUG_IMAGE_WRITE( x, y );
                    }
                }
            }
        }
    }
}
