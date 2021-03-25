//----------------------------------------------------------
// File:		GrUberTileAtlas.cpp
// Author:		Kevin Bray
// Created:		12-16-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUberTileAtlas.h"

// project includes.
#include "GrTexture.h"

// atlas dimensions.
#define LOWRES_ATLAS_DIM			256
#define DETAIL_ATLAS_DIM			256

// atlas stride.
#define LOWRES_ATLAS_STRIDE			( 4 * LOWRES_ATLAS_DIM )
#define DETAIL_ATLAS_STRIDE			( 4 * DETAIL_ATLAS_DIM )

// the size of allocations within the detail atlas.
#define DETAIL_ATLAS_BLOCK_DIM		32

// detail atlas size in blocks.
#define DETAIL_ATLAS_DIM_IN_BLOCKS	( DETAIL_ATLAS_DIM / DETAIL_ATLAS_BLOCK_DIM )

// the detail mip goes from 0 to 3, where the lowres mip goes from 4
// to 6.
#define LOWRES_MIP					5

// macros.
#define BLUE( pel )					pel[ 0 ]
#define GREEN( pel )				pel[ 1 ]
#define RED( pel )					pel[ 2 ]
#define ALPHA( pel )				pel[ 3 ]

#define BLUE_SHIFT( v )				( ( unsigned int )( v ) )
#define GREEN_SHIFT( v )			( ( unsigned int )( v ) <<  8 )
#define RED_SHIFT( v )				( ( unsigned int )( v ) << 16 )
#define ALPHA_SHIFT( v )			( ( unsigned int )( v ) << 24 )

unsigned int SignedShiftRight( unsigned int value, int shift )
{
	// determine which operation to use.
	int right = ( shift > 0 );
	int left = 1 - right;

	// return value shifted left or right, depending on the sign of
	// shift.
	return ( right * ( value >> shift ) + left * ( value << shift ) );
}

//**********************************************************
// class GrUberTileAtlas
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUberTileAtlas::GrUberTileAtlas()
: lowResStore( 0 )
, detailStore( 0 )
, detailFree( 0 )
{
	// allocate resources.
	lowResStore = new unsigned char[ LOWRES_ATLAS_DIM * LOWRES_ATLAS_DIM * 4 ];
	detailStore = new unsigned char[ DETAIL_ATLAS_DIM * DETAIL_ATLAS_DIM * 4 ];
}

//----------------------------------------------------------
GrUberTileAtlas::~GrUberTileAtlas()
{
	delete[] lowResStore;
	delete[] detailStore;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrUberTileAtlas::Clear()
{
	// clear the lowres store to zero.
	unsigned int clearValue = 0;
	unsigned int lowResMemSize = LOWRES_ATLAS_DIM * LOWRES_ATLAS_DIM;
	MemSet( lowResStore, 0, lowResMemSize );

	// clear the detail store.
	detailFree = 0;
}

//----------------------------------------------------------
unsigned int
GrUberTileAtlas::AddGroup( unsigned int mip, unsigned int minX, unsigned int minY,
						   unsigned int maxX, unsigned int maxY )
{
	// calculate the width and height of the group.
	unsigned int groupWidth = maxX - minX + 1;
	unsigned int groupHeight = maxY - minY + 1;

	// make sure that the group isn't too large.
	B_ASSERT( groupWidth < DETAIL_ATLAS_BLOCK_DIM );
	B_ASSERT( groupHeight < DETAIL_ATLAS_BLOCK_DIM );

	// scale the values so that they fit within the lowres map
	// correctly.
	int shift = ( LOWRES_MIP - ( int )mip );
	unsigned int lowresStartX = SignedShiftRight( minX, shift );
	unsigned int lowresStartY = SignedShiftRight( minY, shift );
	unsigned int lowresEndX = lowresStartX + SignedShiftRight( groupWidth, shift );
	unsigned int lowresEndY = lowresStartY + SignedShiftRight( groupWidth, shift );

	// calculate the mip scale.
	unsigned char mipScale = ( unsigned char )( 128 >> mip );

	// if the mip is less than the lowres mip, then we need to place
	// the data in the detail atlas.
	if ( mip < LOWRES_MIP )
	{
		// scan over pixels in the lowres map.
		for ( unsigned int lowY = lowresStartY; lowY < lowresEndY; ++lowY )
		{
			for ( unsigned int lowX = lowresStartX; lowX < lowresEndX; ++lowX )
			{
				// check the current lowres block to see if we need
				// to upload data to it.
				
			}
		}
	}
	else
	{
		// scan over pixels in the lowres map.
		unsigned char* lowResPel = lowResStore + lowresStartY * DETAIL_ATLAS_STRIDE;
		for ( unsigned int lowY = lowresStartY; lowY < lowresEndY; ++lowY, lowResPel += DETAIL_ATLAS_STRIDE )
		{
			unsigned char* curLowResScan = lowResPel;
			for ( unsigned int lowX = lowresStartX; lowX < lowresEndX; ++lowX, curLowResScan += 4 )
			{
				// check the current lowres block to see if we need
				// to upload data to it.

				// if the current pixel is an indirection into the
				// detail atlas, then we need to write the value to
				// the detail atlas.
				if ( ALPHA( curLowResScan ) == 255 )
				{
					// fill our value into the detail atlas.
					
				}
				else
				{
					// if the current mip-level is of higher detail
					// than the mip-level already in the atlas, then
					// we need to overwrite the current pixel.
					if ( mipScale > RED( curLowResScan ) )
					{
						// write the mip-scale.
						RED( curLowResScan ) = mipScale;

						// write the placement in the cache.
					}
				}
			}
		}
	}

	// return 0 for now.
	return 0;
}

//----------------------------------------------------------
void
GrUberTileAtlas::AddGroupTile( unsigned int group, unsigned int mip, unsigned int x,
							   unsigned int y )
{
	
}

//----------------------------------------------------------
void
GrUberTileAtlas::Flush()
{

}

//----------------------------------------------------------
void
GrUberTileAtlas::BindLowResAtlas( unsigned int sampler )
{

}

//----------------------------------------------------------
void
GrUberTileAtlas::BindDetailAtlas( unsigned int sampler )
{

}



/*
	Mip scale calculation notes.

	2   =  1:1		(2048:2048), shift = 6
	4   =  2:1		(2048:4096), shift = 5
	8   =  4:1		(2048:8192), shift = 4
	16  =  8:1		(2048:16384), shift = 3
	32  = 16:1		(2048:32768), shift = 2
	64  = 32:1		(2048:65536), shift = 1
	128 = 64:1		(2048:131072), shift = 0

	mipValue, valueWritten.
	0 = 128
	1 = 64
	2 = 32
	3 = 16
	4 = 8
	5 = 4
	6 = 2
	// everything under this is handled with a fallback texture.
*/

