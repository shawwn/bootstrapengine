//----------------------------------------------------------
// File:		GrPolygonUberMapping.h
// Author:		Kevin Bray
// Created:		12-27-08
//
// Purpose:		To place a UV group into an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrPolygonUberPatch;
class GrUberTexture;
class UPath;

//**********************************************************
// class GrPolygonUberMapping
//**********************************************************
class GrPolygonUberMapping
{
public:
	GrPolygonUberMapping( unsigned int uberTexIdx );
	~GrPolygonUberMapping();

	// adds an ubertexture to the current patch.
	bool					AddPatch( GrPolygonUberPatch* patch );

	// patch accessors.
	unsigned int			GetPatchCount() const;
	GrPolygonUberPatch*		GetPatch( unsigned int idx ) const;

	// builds an ubertexture out of the currently added geometry.
	URef< GrUberTexture >	BuildUberTexture( const UPath& name ) const;

private:
	typedef std::vector< GrPolygonUberPatch* > PatchVector;

	// mapping state.
	PatchVector				_patches;
	unsigned int			_curX;
	unsigned int			_curY;
	unsigned int			_maxY;
	unsigned int			_uberTexIdx;
};
