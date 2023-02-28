//----------------------------------------------------------
// File:        GrUTAtlas.cpp
// Author:      Kevin Bray
// Created:     11-17-2008
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUTAtlas.h"

// project includes.
#include "GrConfig.h"
#include "UBitReader.h"

// means that the current node is a leaf.
#define GR_UT_LEAF                  0x0001

// means that the current leaf has no children.  If this is not
// specified, then the leaf is complete (has all children).
#define GR_UT_LEAF_EMPTY            0x0002

//----------------------------------------------------------
// tile count lookup table, maps atlas-tree depth to node count.
static unsigned int kTotalTileCount[ 12 ] =
{
    1,
    5,
    21,
    85,
    341,
    1365,
    5461,
    21845,
    87381,
    349525,
    1398101,
    5592405,
};

//**********************************************************
// class GrUTAtlas
//**********************************************************

//----------------------------------------------------------
GrUTAtlas::GrUTAtlas( unsigned int* tileUsage, unsigned int baseChunk )
: _chunk( 0 )
, _invDepth( 11 )
, _flags( 0 )
{
    unsigned int totalTileCount = GR_UBERTEX_MAX_SIZE_IN_TILES * GR_UBERTEX_MAX_SIZE_IN_TILES;
    BuildTreeFast( tileUsage, totalTileCount );
    CalcTileAddress( baseChunk, totalTileCount );
}

//----------------------------------------------------------
GrUTAtlas::GrUTAtlas( unsigned int* tileUsage, unsigned int maxTileCount, unsigned int depth )
: _chunk( 0 )
, _invDepth( depth )
, _flags( 0 )
{
    BuildTreeFast( tileUsage, maxTileCount );
}

//----------------------------------------------------------
GrUTAtlas::GrUTAtlas( UBitReader& bitReader, unsigned int maxTileCount, unsigned int depth )
: _chunk( 0 )
, _invDepth( depth )
, _flags( 0 )
{
    // at this point, there should be 16 or 4 or 1 tiles.
    B_ASSERT( maxTileCount <= 16 );

    // scan ahead to determine if all bits are set to the same value.
    UBitReader laBitReader = bitReader;
    unsigned int start = laBitReader.ReadBits( 1 );
    unsigned int i = 1;
    for ( ; i < maxTileCount; ++i )
    {
        if ( laBitReader.ReadBits( 1 ) != start )
            break;
    }

    // are we a leaf or a node?
    if ( i != maxTileCount )
    {
        // if not all tiles are present, then we need to build
        // children.
        unsigned int childInvDepth = _invDepth - 1;
        unsigned int childMaxTileCount = maxTileCount >> 2;
        unsigned int c = childMaxTileCount / 32;
        _children[ 0 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
        _children[ 1 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
        _children[ 2 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
        _children[ 3 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
    }
    else
    {
        // if all tiles present, simply set ourselves up as a leaf.
        _flags = GR_UT_LEAF;
        _flags |= ( ( start == 0 ) ? GR_UT_LEAF_EMPTY : 0 );

        // clear children.
        _children[ 0 ] = 0;
        _children[ 1 ] = 0;
        _children[ 2 ] = 0;
        _children[ 3 ] = 0;

        // skip ahead by the number of bits this node represents.
        bitReader.SkipBits( maxTileCount );
    }
}

//----------------------------------------------------------
GrUTAtlas::~GrUTAtlas()
{
    delete _children[ 0 ];
    delete _children[ 1 ];
    delete _children[ 2 ];
    delete _children[ 3 ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrUTAtlas::FindTileChunk( unsigned short mip, unsigned short x, unsigned short y ) const
{
    return FindTileChunkInternal( mip, x << mip, y << mip );
}

//----------------------------------------------------------
unsigned int
GrUTAtlas::GetMaxTileCount() const
{
    return kTotalTileCount[ _invDepth ];
}

//----------------------------------------------------------
bool
GrUTAtlas::IsLeaf() const
{
    return ( _flags & GR_UT_LEAF ) != 0;
}

//----------------------------------------------------------
bool
GrUTAtlas::IsEmpty() const
{
    return ( _flags & GR_UT_LEAF_EMPTY ) != 0;
}

//----------------------------------------------------------
GrUTAtlas*
GrUTAtlas::GetChild( unsigned int idx ) const
{
    B_ASSERT( idx < 4 );
    return _children[ idx ];
}

//----------------------------------------------------------
unsigned int
GrUTAtlas::GetChunk() const
{
    return _chunk;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrUTAtlas::FindTileChunkInternal( unsigned short mipLevel, unsigned short x, unsigned short y ) const
{
    // if we're an empty node, then the tile the caller is looking
    // for is not available.
    if ( IsEmpty() )
        return 0;

    // if this node represents the tile requested, simply return our
    // chunk.
    if ( mipLevel == _invDepth )
        return _chunk;

    // if we're not a leaf, continue searching by recursing the
    // correct child.
    unsigned int relAddr = _chunk;
    if ( !IsLeaf() )
    {
        // calculate the quadrant.
        unsigned int childDepth = _invDepth - 1;
        unsigned int bx = ( x >> childDepth ) & 1;
        unsigned int by = ( y >> childDepth ) & 1;

        // recurse and return the result.
        relAddr = _children[ ( by + by ) | bx ]->FindTileChunkInternal( mipLevel, x, y );
    }
    else
    {
        // if we're a leaf, we need to calculate the tile's chunk.
        unsigned int curMip = _invDepth;
        while ( curMip-- > mipLevel )
        {
            // add one each time we decrease the mip-level.
            relAddr += 1;

            // select the quadrant using a multiplier.
            unsigned int mx = ( x >> curMip ) & 1;
            unsigned int my = ( y >> curMip ) & 1;
            relAddr += ( ( my + my ) | mx ) * kTotalTileCount[ curMip ];
        }
    }

    // return the address of the tile requested.
    return relAddr;
}

//----------------------------------------------------------
void
GrUTAtlas::BuildTreeFast( unsigned int* tileUsage, unsigned int maxTileCount )
{
    // this constructor is only valid for quads that contain 64 or
    // more tiles.
    B_ASSERT( maxTileCount >= 64 );
    
    // determine if we have all or no children.
    unsigned int* src = tileUsage;

    // verify that all bits are either 1 or 0.
    unsigned int tileCount = maxTileCount;
    unsigned int start = src[ 0 ] + 1;
    while ( tileCount > 0 )
    {
        // get the current value and determine if it's ~0 or 0.
        unsigned int curValue = ( *src++ ) + 1;
        if ( curValue > 1 || curValue != start )
            break;
        tileCount -= 32;
    }

    // are we a leaf or a node in the hierarchy?
    if ( tileCount != 0 )
    {
        // if not all tiles are present, then we need to build
        // children.
        unsigned int childInvDepth = _invDepth - 1;
        unsigned int childMaxTileCount = maxTileCount >> 2;
        unsigned int c = childMaxTileCount / 32;
        if ( childMaxTileCount >= 64 )
        {
            _children[ 0 ] = new GrUTAtlas( tileUsage + 0 * c, childMaxTileCount, childInvDepth );
            _children[ 1 ] = new GrUTAtlas( tileUsage + 1 * c, childMaxTileCount, childInvDepth );
            _children[ 2 ] = new GrUTAtlas( tileUsage + 2 * c, childMaxTileCount, childInvDepth );
            _children[ 3 ] = new GrUTAtlas( tileUsage + 3 * c, childMaxTileCount, childInvDepth );
        }
        else
        {
            UBitReader bitReader( ( unsigned char* )tileUsage, maxTileCount );
            _children[ 0 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
            _children[ 1 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
            _children[ 2 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
            _children[ 3 ] = new GrUTAtlas( bitReader, childMaxTileCount, childInvDepth );
        }
    }
    else
    {
        // if all tiles present, simply set ourselves up as a leaf.
        _flags = GR_UT_LEAF;
        _flags |= ( ( *tileUsage == 0 ) ? GR_UT_LEAF_EMPTY : 0 );

        // clear children.
        _children[ 0 ] = 0;
        _children[ 1 ] = 0;
        _children[ 2 ] = 0;
        _children[ 3 ] = 0;
    }
}

//----------------------------------------------------------
void
GrUTAtlas::CalcTileAddress( unsigned int& startAddress, unsigned int sizeInTiles )
{
    // if we're an empty leaf, simply return.
    if ( _flags & GR_UT_LEAF_EMPTY )
        return;

    // we're either a full set of tiles or a partial set of tiles.
    // either way, we represent a mip-leaf.
    _chunk = startAddress++;

    // are we a node or a leaf?  If we're a node, we represent a
    // tile on a mip level.
    if ( _flags & GR_UT_LEAF )
    {
        // if we're a full, then we represent a mip-chain all of the
        // up to the highest mip-level.
        while ( sizeInTiles > 1 )
        {
            startAddress += sizeInTiles;
            sizeInTiles >>= 2;
        }
    }
    else
    {
        // recurse.
        unsigned int childSize = sizeInTiles >> 2;
        for ( unsigned int i = 0; i < 4; ++i )
            _children[ i ]->CalcTileAddress( startAddress, childSize );
    }
}
