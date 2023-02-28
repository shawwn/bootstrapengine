//----------------------------------------------------------
// File:		GrPolygonUberUVMapper.cpp
// Author:		Kevin Bray
// Created:		12-11-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonUberUVMapper.h"

// project includes.
#include "GrConfig.h"
#include "GrPolygonUberPatchGroup.h"
#include "GrPolygonUberPatch.h"
#include "GrPolygonUberMapping.h"
#include "GrUberTexture.h"

#pragma warning( disable : 4996 )

// information about each patch, required for packing arbitrary
// patches into an ubertexture.
struct SPatchInfo
{
	SPatchInfo() { }
	SPatchInfo( GrPolygonUberPatch* _patch )
		: patch( _patch )
	{
		// get the patch's size in tiles.  Note that we always round
		// up.
		heightInTiles = ( int )Ceil( patch->GetUVSizeInTiles().y );
		widthInTiles = ( int )Ceil( patch->GetUVSizeInTiles().x );
	}

	GrPolygonUberPatch* patch;

	// hyper grid X & Y.
	int hyperGridX;
	int hyperGridY;

	// width and height of the current patch.
	unsigned int widthInTiles;
	unsigned int heightInTiles;
};

// sorts patches based on their grid position.
bool PatchSorter( const SPatchInfo& lhs, const SPatchInfo& rhs )
{
	// sort by the hyper grid position first.
	if ( lhs.hyperGridY != rhs.hyperGridY )
		return ( lhs.hyperGridY < rhs.hyperGridY );
	if ( lhs.hyperGridX != rhs.hyperGridX )
		return ( lhs.hyperGridX < rhs.hyperGridX );

	// sort by the local grid position second.
	return ( lhs.heightInTiles > rhs.heightInTiles );
}

void PackPatchGroup( SVec2& maxUVSize, const std::vector< SPatchInfo >& patches, unsigned int startIdx );


//**********************************************************
// class GrPolygonUberUVMapper
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonUberUVMapper::GrPolygonUberUVMapper()
{

}

//----------------------------------------------------------
GrPolygonUberUVMapper::~GrPolygonUberUVMapper()
{
	// free all mappings.
	for ( unsigned int i = 0; i < ( unsigned int )_mappings.size(); ++i )
		delete _mappings[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonUberUVMapper::PlaceGroupsSlow( std::vector< GrPolygonUberPatchGroup* > groups, EPLANE movementPlane,
									    float gridEntrySize )
{
	// scale to convert UVs to patch boundary increments.
	const float kUVToPatchScale = GR_UBERTEX_SIZE_IN_TEXELS / ( float )GR_UBERTEX_PATCH_BOUNDARY;
	const float kUVToTileScale = GR_UBERTEX_SIZE_IN_TILES;
	const unsigned int kPatchBoundaryMask = GR_UBERTEX_PATCH_BOUNDARY - 1;

	// mask to calculate the local grid and hyper grid positions for
	// each patch.
	unsigned int kLocalGridMask = 15;

	// an vector component lookup table for each plane's X and Y.
	const int kXAxis[ 3 ] = { 0, 0, 1 };
	const int kYAxis[ 3 ] = { 1, 2, 2 };

	// a useful value for scaling world space to grid space.
	const float kInvGridEntrySize = 1.0f / gridEntrySize;

	// pack all groups into the patches vector.
	std::vector< SPatchInfo > patches;
	unsigned int groupCount = ( unsigned int )groups.size();
	for ( unsigned int i = 0; i < groupCount; ++i )
	{
		// get the current group.
		GrPolygonUberPatchGroup* curGroup = groups[ i ];

		// get the number of patches in the current group.
		unsigned int patchCount = curGroup->GetPatchCount();
		if ( patchCount == 0 )
			continue;

		// iterate through each group and pack them into a vector.
		patches.resize( patchCount );
		for ( unsigned int i = 0; i < patchCount; ++i )
		{
			// get the current patch.
			SPatchInfo& patchInfo = patches[ i ];
			patchInfo.patch = curGroup->GetPatch( i );

			// calculate the patch's grid location.
			const MVec3& loc = patchInfo.patch->GetAABox().GetCenter();
			int worldGridX = ( int )( kInvGridEntrySize * loc[ kXAxis[ movementPlane ] ] );
			int worldGridY = ( int )( kInvGridEntrySize * loc[ kYAxis[ movementPlane ] ] );

			// calculate and store the hyper grid position.
			patchInfo.hyperGridX = worldGridX & ~kLocalGridMask;
			patchInfo.hyperGridY = worldGridY & ~kLocalGridMask;

			// calculate and store the local grid position.
			patchInfo.widthInTiles = ( unsigned int )Ceil( patchInfo.patch->GetUVSizeInTiles().x );
			patchInfo.heightInTiles = ( unsigned int )Ceil( patchInfo.patch->GetUVSizeInTiles().y );
		}
	}

	// sort patches based on their grid positions.
	sort( patches.begin(), patches.end(), PatchSorter );

	// get the total number of patches.
	unsigned int patchCount = ( unsigned int )patches.size();
	if ( patchCount == 0 )
		return;

	// scan through groups in each hyper-tile to determine the
	// largest patch group.
	_mappings.push_back( new GrPolygonUberMapping( ( unsigned int )_mappings.size() ) );
	for ( unsigned int i = 0; i < patchCount; ++i )
	{
		// get the current patch.
		SPatchInfo& curPatchInfo = patches[ i ];
		if ( curPatchInfo.patch->GetUVSizeInTiles().x < GR_UBERTEX_SIZE_IN_TILES &&
			 curPatchInfo.patch->GetUVSizeInTiles().y < GR_UBERTEX_SIZE_IN_TILES )
		{
			// try to pack the current patch into the mapping.
			if ( !_mappings.back()->AddPatch( curPatchInfo.patch ) )
			{
				// create a new mapping and add the patch to it.
				_mappings.push_back( new GrPolygonUberMapping( ( unsigned int )_mappings.size() ) );
				_mappings.back()->AddPatch( curPatchInfo.patch );
			}
		}
	}
}

//----------------------------------------------------------
unsigned int
GrPolygonUberUVMapper::GetTexturesRequired() const
{
	return ( unsigned int )_mappings.size();
}

//----------------------------------------------------------
void
GrPolygonUberUVMapper::GenUberTextures( const UPath& path )
{
	// iterate through all ubertexture mappings and build the
	// ubertextures.
	char buffer[ 12 ];
	for ( unsigned int i = 0; i < _mappings.size(); ++i )
	{
		// get the current mapping.
		GrPolygonUberMapping* curMapping = _mappings[ i ];

		// generate the file name.
		sprintf( buffer, "%02d%s", i, ".uber" );
		UPath fileName = path + buffer;

		// generate an acceptable file name.
		_uberTextures.push_back( curMapping->BuildUberTexture( fileName ) );
	}
}

//----------------------------------------------------------
unsigned int
GrPolygonUberUVMapper::GetUberTextureCount() const
{
	return 1;
}

//----------------------------------------------------------
URef< GrUberTexture >
GrPolygonUberUVMapper::GetUberTexture( unsigned int idx ) const
{
	return 0;
}


//**********************************************************
// File local functions.
//**********************************************************
/*
//----------------------------------------------------------
void
GrPolygonUberUVMapper::PackPatchGroup( SVec2& maxUVSize, const std::vector< SPatchInfo >& patches,
									   unsigned int startIdx )
{
	// get the current group's X & Y.
	unsigned int groupX = patches[ startIdx ].hyperGridX;
	unsigned int groupY = patches[ startIdx ].hyperGridY;

	unsigned int startSize = 0;

	// pack until we find another group.
	while( patches[ startIdx ].hyperGridX == groupX && patches[ startIdx ].hyperGridY == groupY )
	{
		// pack the current group.
		

		// next index.
		++startIdx;
	}
}
*/
