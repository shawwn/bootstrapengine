//----------------------------------------------------------
// File:		GrUberPatchSet.h
// Author:		Kevin Bray
// Created:		12-09-08
//
// Purpose:		To manage a set of ubertexture patches.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"

// forward declarations.
class GrPolygonUberPatchGroup;
class GrPolygonUberUVMapper;
class GrUberTexture;
class GrCamera;
class MAABox;
class MPlane;
class UPath;

//**********************************************************
// class GrUberPatchSet
//**********************************************************
class GrUberPatchSet
{
public:
	GrUberPatchSet( GrPolygonUberPatchGroup* uberData, const GrPolygonUberUVMapper& uberMapper );
	GrUberPatchSet( UReader& reader );
	~GrUberPatchSet();

	// sets the world transform.
	void					SetWorldXForm( const MMat4x4& worldXForm, const MMat3x3& worldNormalRot,
										   const MMat4x4& invWorldXForm );

	// calculates tile LOD and then caches tiles.
	void					CacheTiles( const GrCamera* camera, unsigned int screenWidth, unsigned int screenHeight );

	// load/save functionality.
	void					Load( UReader& reader );
	void					Save( UWriter& writer );

private:
	struct SPatchTileInfo
	{
		unsigned short uberTexIndex;
		unsigned short startTileX;
		unsigned short startTileY;
		unsigned short endTileX;
		unsigned short endTileY;
	};

	void					Build( GrPolygonUberPatchGroup* uberData, const GrPolygonUberUVMapper& uberMapper );
	void					AllocArrays();
	void					Clean();

	// ubertexture.
	URef< GrUberTexture >*	_uberTextures;
	unsigned int			_uberTextureCount;

	// patch data.
	MMat4x4					_invWorldXForm;
	unsigned int			_patchCount;

	// ubertexture tiles per meter.
	float					_tilesPerMeter;

	// local space data for each patch.
	MAABox*					_localBounds;
	MPlane*					_localPlanes;
	MMat4x4*				_localToTextureMatrices;

	// world space data for each patch.
	MAABox*					_worldBounds;
	MPlane*					_worldPlanes;
	MMat4x4*				_worldToTextureMatrices;

	// patch tile info.
	SPatchTileInfo*			_patchTileInfo;
};
