//----------------------------------------------------------
// File:		GrUberPatchSet.cpp
// Author:		Kevin Bray
// Created:		12-09-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUberPatchSet.h"

// project includes.
#include "GrPolygonUberPatchGroup.h"
#include "GrPolygonUberPatch.h"
#include "GrPolygonUberUVMapper.h"
#include "GrConfig.h"
#include "GrCamera.h"
#include "MAABox.h"
#include "MOBox.h"
#include "UPath.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrUberDetailCache.h"
#include "GrRenderUtil.h"
#include "GrDebugSwitches.h"
#include "GrUberTextureMgr.h"
#include "GrColor.h"

#pragma warning( disable : 4996 )

// this angle can used to determine whether or not a patch is
// completely backfacing (no patch can deviate from the back
// facing patch's plane by more than 45 degrees).
#define PATCH_BACKFACE_ANGLE_COS		0.70710678f

//**********************************************************
// class GrUberPatchSet
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUberPatchSet::GrUberPatchSet( GrPolygonUberPatchGroup* uberData, const GrPolygonUberUVMapper& uberMapper )
: _uberTextures( 0 )
, _uberTextureCount( 0 )
, _patchCount( 0 )
, _tilesPerMeter( 0.0f )
, _localBounds( 0 )
, _localPlanes( 0 )
, _localToTextureMatrices( 0 )
, _worldBounds( 0 )
, _worldPlanes( 0 )
, _worldToTextureMatrices( 0 )
, _patchTileInfo( 0 )
{
	// build the data out of the uberData.
	Build( uberData, uberMapper );
}

//----------------------------------------------------------
GrUberPatchSet::GrUberPatchSet( UReader& reader )
: _uberTextures( 0 )
, _uberTextureCount( 0 )
, _patchCount( 0 )
, _tilesPerMeter( 0.0f )
, _localBounds( 0 )
, _localPlanes( 0 )
, _localToTextureMatrices( 0 )
, _worldBounds( 0 )
, _worldPlanes( 0 )
, _worldToTextureMatrices( 0 )
, _patchTileInfo( 0 )
{
	// build the data out of the uberData.
	Load( reader );
}

//----------------------------------------------------------
GrUberPatchSet::~GrUberPatchSet()
{
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrUberPatchSet::SetWorldXForm( const MMat4x4& worldXForm, const MMat3x3& worldNormalRot,
							   const MMat4x4& invWorldXForm )
{
	// set the world transform.
	_invWorldXForm = invWorldXForm;

	// calculate world space data.
	for ( unsigned int i = 0; i < _patchCount; ++i )
	{
		_worldBounds[ i ] = MAABox( MOBox( worldXForm, _localBounds[ i ] ) );
		_worldPlanes[ i ] = _localPlanes[ i ].Transform( worldXForm, worldNormalRot );
		_worldToTextureMatrices[ i ] = _localToTextureMatrices[ i ] * _invWorldXForm;
	}
}

//----------------------------------------------------------
void
GrUberPatchSet::CacheTiles( const GrCamera* camera, unsigned int screenWidth, unsigned int screenHeight )
{
	// bias.
	const float kBias = 2.0f;

	// get the current look direction.  We can use this to perform an
	// early rejection of some patches (we can simply exclude patches
	// facing away from the camera).
	MVec3 lookDir = camera->GetLookDir();
	const MVec3& cameraPos = camera->GetPos();
	const MMat4x4& localToProjMat = camera->GetViewProjMatrix();

	// calculate minimum width and height (one pixel).
	float minWidth = 1.0f / screenWidth;
	float minHeight = 1.0f / screenHeight;

	// get the field of view and calculate LOD distance function.
	float fov = camera->GetProj().GetFOV();
	float fovScale = 1.0f / Tan( fov );

	static bool printStats = false;
	if ( printStats )
		PrintF( "Printing ubertexture tile stats...\n" );

	if ( r_DrawUberPatchAABBs )
		gGrRenderUtil->SetupColoredRender( *camera, true, false );

	// for each patch.
	MVec3 corners[ 8 ];
	for ( unsigned int i = 0; i < _patchCount; ++i )
	{
		// determine if we can reject the current patch entirely.
		if ( _worldPlanes[ i ].GetNormal().Dot( lookDir ) > PATCH_BACKFACE_ANGLE_COS )
			continue;

		// check to see if the current patch is in the view frustum.
		// If not, we can simply skip the current patch.
		if ( !camera->GetFrustum().IsAABoxInside( _worldBounds[ i ] ) )
			continue;

		// calculate the number of texels in the patch along each axis.
		unsigned int texelWidth = ( _patchTileInfo[ i ].endTileX - _patchTileInfo[ i ].startTileX + 1 ) * GR_UBERTEX_TILE_SIZE;
		unsigned int texelHeight = ( _patchTileInfo[ i ].endTileY - _patchTileInfo[ i ].startTileY + 1 ) * GR_UBERTEX_TILE_SIZE;

		// determine the maximum LOD and the falloff function based
		// on the size of the bounding box on the screen.
		float widthInPels = ( float )( int )texelWidth;
		float heightInPels = ( float )( int )texelHeight;
		if ( !_worldBounds[ i ].IsInside( cameraPos ) )
		{
			// get the corners of the box, transformed into screen
			// space.
			_localBounds[ i ].GetCorners( corners );
			for ( unsigned int j = 0; j < 8; ++j )
				localToProjMat.TransformCoordFast( corners[ j ] );

			// calculate the bounding volume in screen space.
			MVec3 boundsMin = corners[ 0 ];
			MVec3 boundsMax = corners[ 0 ];
			for ( unsigned int j = 1; j < 8; ++j )
			{
				boundsMin.AssignMin( corners[ j ] );
				boundsMax.AssignMax( corners[ j ] );
			}

			// cache all tiles based on their LODs.
			MVec3 size = boundsMax - boundsMin;
			widthInPels = ( 0.5f * size.GetX() ) * screenWidth;
			heightInPels = ( 0.5f * size.GetY() ) * screenHeight;
			if ( widthInPels < minWidth || heightInPels < minHeight )
				continue;
		}

		// draw the current AA-Box.
		if ( r_DrawUberPatchAABBs )
			gGrRenderUtil->DrawColoredAABox( _worldBounds[ i ], GrColor( 1.0f, 1.0f, 0.0f, 1.0f ) );

		// finally calculate the LOD for the current patch.
		float maxRatio = kBias * Max( texelWidth / widthInPels, texelHeight / heightInPels );
		float lod = Log2( Max( maxRatio, 1.0f ) );

		// project the position on the approximate plane into texture
		// space.
		MVec3 tsCameraPos = cameraPos;
		_worldToTextureMatrices[ i ].TransformCoordNoPerspFast( tsCameraPos );
		int lodCenterX = ( int )tsCameraPos.GetX();
		int lodCenterY = ( int )tsCameraPos.GetY();

		// calculate the size of the LOD boxes in texture space.
		int startSphere = ( int )lod;
		for ( unsigned int j = startSphere; j < 7; ++j )
		{
			// intersect the approximate plane with the sphere to
			// determine the falloff size.
			float radius = ( 1 << j ) * fovScale;
			float dist = _worldBounds[ i ].Dist( cameraPos );
			if ( dist < radius )
			{
				// calculate the circle's radius in tile space.
				int circleRadius = ( int )( _tilesPerMeter * Sqrt( Sqr( radius ) - Sqr( dist ) ) );

				// clamp the circle radius at 8 so that we never
				// end up with groups larger than 16x16 tiles.
				circleRadius = Min( circleRadius, 8 << j );

				// simply use the AA-box of the circle, clipped to
				// the current patch's bounds, for tile caching.
				int minX = Max( lodCenterX - circleRadius, ( int )_patchTileInfo[ i ].startTileX );
				int minY = Max( lodCenterY - circleRadius, ( int )_patchTileInfo[ i ].startTileY );
				int maxX = Min( lodCenterX + circleRadius, ( int )_patchTileInfo[ i ].endTileX );
				int maxY = Min( lodCenterY + circleRadius, ( int )_patchTileInfo[ i ].endTileY );

				// skip the current group if the current tile is
				// outside of patch.
				if ( minX > _patchTileInfo[ i ].endTileX )
					continue;
				if ( minY > _patchTileInfo[ i ].endTileY )
					continue;
				if ( maxX < _patchTileInfo[ i ].startTileX )
					continue;
				if ( maxY < _patchTileInfo[ i ].startTileY )
					continue;

				// adjust tiles by the LOD.
				minX >>= j;
				minY >>= j;
				maxX >>= j;
				maxY >>= j;

				// print out statistics if necessary.
				if ( printStats )
					PrintF( "Tile region (mip %d): %4d, %4d\n", j, maxX - minX, maxY - minY );

				// request tiles.
//				gGrUberDetailCache->CacheTiles( _patchTileInfo[ i ].uberTexId, j, minX, minY, maxX, maxY );
			}
		}
	}
}

//----------------------------------------------------------
void
GrUberPatchSet::Load( UReader& reader )
{
	// free anything that was previously allocated.
	Clean();

	// read in the patch count.
	_patchCount = reader.ReadUInt();

	// read in the total number of tiles per meter.
	_tilesPerMeter = reader.ReadFloat();

	// allocate space for patch data and read it in.
	AllocArrays();

	// load each ubertexture.
	_uberTextureCount = reader.ReadUInt();
	_uberTextures = new URef< GrUberTexture >[ _uberTextureCount ];
	for ( unsigned int i = 0; i < _uberTextureCount; ++i )
	{
		UPath uberTexName = reader.ReadString();
		_uberTextures[ i ] = gGrUberTextureMgr->GetUberTexture( uberTexName );
	}

	// now read the data.
	for ( unsigned int i = 0; i < _patchCount; ++i )
	{
		// get the ubertexture patch info.
		_patchTileInfo[ i ].uberTexIndex = reader.ReadInt();
		_patchTileInfo[ i ].startTileX = reader.ReadShort();
		_patchTileInfo[ i ].startTileY = reader.ReadShort();
		_patchTileInfo[ i ].endTileX = reader.ReadShort();
		_patchTileInfo[ i ].endTileY = reader.ReadShort();

		// load world data.
		_localBounds[ i ].Load( reader );
		_localPlanes[ i ].Load( reader );
		_localToTextureMatrices[ i ].Load( reader );
	}

	// set the world transform to identity by default.
	SetWorldXForm( MMat4x4(), MMat3x3(), MMat4x4() );
}

//----------------------------------------------------------
void
GrUberPatchSet::Save( UWriter& writer )
{
	// write out the patch count.
	writer.WriteUInt( _patchCount );

	// write out the total number of tiles per meter.
	writer.WriteFloat( _tilesPerMeter );

	// write out the name of each ubertexture.
	writer.WriteUInt( _uberTextureCount );
	for ( unsigned int i = 0; i < _uberTextureCount; ++i )
		writer.WriteString( _uberTextures[ i ]->GetName().GetPathString() );

	// now write the data.
	for ( unsigned int i = 0; i < _patchCount; ++i )
	{
		// get the ubertexture patch info.
		writer.WriteShort( _patchTileInfo[ i ].uberTexIndex );
		writer.WriteShort( _patchTileInfo[ i ].startTileX );
		writer.WriteShort( _patchTileInfo[ i ].startTileY );
		writer.WriteShort( _patchTileInfo[ i ].endTileX );
		writer.WriteShort( _patchTileInfo[ i ].endTileY );

		// load world data.
		_localBounds[ i ].Save( writer );
		_localPlanes[ i ].Save( writer );
		_localToTextureMatrices[ i ].Save( writer );
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrUberPatchSet::Build( GrPolygonUberPatchGroup* uberData, const GrPolygonUberUVMapper& uberMapper )
{
	// get the number of patches we need to deal with.
	_patchCount = uberData->GetPatchCount();

	// get the number of tiles per meter.
	_tilesPerMeter = uberData->GetTexelsPerMeter() / GR_UBERTEX_TILE_SIZE;

	// allocate space for all of our data.
	AllocArrays();

	// get the ubertextures.
	_uberTextureCount = uberMapper.GetUberTextureCount();
	_uberTextures = new URef< GrUberTexture >[ _uberTextureCount ];
	for ( unsigned int i = 0; i < _uberTextureCount; ++i )
		_uberTextures[ i ] = uberMapper.GetUberTexture( i );

	// now copy the data.
	for ( unsigned int i = 0; i < _patchCount; ++i )
	{
		// get the current patch.
		GrPolygonUberPatch* curPatch = uberData->GetPatch( i );

		// get the ubertexture patch info.
		_patchTileInfo[ i ].uberTexIndex = curPatch->GetUberTextureIndex();
		_patchTileInfo[ i ].startTileX = curPatch->GetUberTileX();
		_patchTileInfo[ i ].startTileY = curPatch->GetUberTileY();
		_patchTileInfo[ i ].endTileX = curPatch->GetUberTileX() + curPatch->GetUberTileWidth();
		_patchTileInfo[ i ].endTileY = curPatch->GetUberTileY() + curPatch->GetUberTileHeight();

		// store data.
		_localBounds[ i ] = curPatch->GetAABox();
		_localPlanes[ i ] = curPatch->GetSurfacePlane();
		_localToTextureMatrices[ i ] = curPatch->GetLocalToUVMatrix();
	}
}

//----------------------------------------------------------
void
GrUberPatchSet::AllocArrays()
{
	// allocate patches to a multiple of 4.
	unsigned int arraySize = ( _patchCount + 3 ) & ~3;

	// allocate space for the local space patch data.
	_localBounds = new MAABox[ arraySize ];
	_localPlanes = new MPlane[ arraySize ];
	_localToTextureMatrices = new MMat4x4[ arraySize ];

	// allocate space for the world space patch data.
	_worldBounds = new MAABox[ arraySize ];
	_worldPlanes = new MPlane[ arraySize ];
	_worldToTextureMatrices = new MMat4x4[ arraySize ];

	// allocate space for the uber-texture space patch data.
	_patchTileInfo = new SPatchTileInfo[ arraySize ];
}

//----------------------------------------------------------
void
GrUberPatchSet::Clean()
{
	// free local-space patch data.
	delete[] _localBounds;
	delete[] _localPlanes;
	delete[] _localToTextureMatrices;

	// free world-space patch data.
	delete[] _worldBounds;
	delete[] _worldPlanes;
	delete[] _worldToTextureMatrices;

	// free ubertexture space patch data.
	delete[] _patchTileInfo;

	_localBounds = 0;
	_localPlanes = 0;
	_localToTextureMatrices = 0;

	_worldBounds = 0;
	_worldPlanes = 0;
	_worldToTextureMatrices = 0;
}
