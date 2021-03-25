//----------------------------------------------------------
// File:		EdBrushPaletteMgr.h
// Author:		Kevin Bray
// Created:		06-20-08
//
// Purpose:		To manage brush palettes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "UPath.h"

// std c++ includes.
#include <map>
#include <vector>

// forward declarations.
class UPath;
class EdBrush;
class EdBrushPalette;

//**********************************************************
// class EdBrushPaletteMgr
//**********************************************************
class EdBrushPaletteMgr
{
public:
	typedef std::vector< URef< EdBrush > > BrushVec;

	EdBrushPaletteMgr( const UPath& brushPaletteRoot );
	~EdBrushPaletteMgr();

	// loads all brush scripts.
	bool					LoadBrushPaletteScripts();

	// returns a brush palette.
	URef< EdBrushPalette >	SetBrushPalette( const UPath& name, const BrushVec& brushes );
	URef< EdBrushPalette >	GetBrushPalette( const UPath& name, tstring& error );

	// removes a brush palette.
	void					RemoveBrushPalette( const UPath& name );

	// brush palette iteration.
	unsigned int			GetBrushPaletteCount() const;
	URef< EdBrushPalette >	GetBrushPaletteByIdx( unsigned int idx ) const;

	// returns the script name from the brush name.
	void					GetScriptName( tstring& scriptName, const tstring& brushName );

private:
	typedef std::map< UPath, URef< EdBrushPalette > > BrushPaletteMap;
	typedef std::vector< EdBrushPalette* > BrushPaletteVec;

	BrushPaletteMap			_brushPaletteMap;
	BrushPaletteVec			_brushPalettes;
	UPath					_brushPaletteRoot;
};
extern EdBrushPaletteMgr* gEdBrushPaletteMgr;
