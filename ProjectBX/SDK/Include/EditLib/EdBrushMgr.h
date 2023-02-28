//----------------------------------------------------------
// File:		EdBrushMgr.h
// Author:		Kevin Bray
// Created:		05-17-08
//
// Purpose:		To manage brushes and brush images.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "EdBrush.h"

// engine includes.
#include "UPath.h"

// std c++ includes.
#include <vector>
#include <map>

//**********************************************************
// class EdBrushMgr
//**********************************************************
class EdBrushMgr
{
public:
	EdBrushMgr( const UPath& brushRoot );
	~EdBrushMgr();

	// loads all brush scripts.
	void					LoadBrushScripts();

	// brushes.
	URef< EdBrush >			SetBrush( const tstring& name, const EdBrush::SInfo& info );
	URef< EdBrush >			GetBrush( const UPath& name, tstring& error );

	// brush iteration.
	unsigned int			GetBrushCount() const;
	URef< EdBrush >			GetBrushByIdx( unsigned int idx ) const;

	// gets a brush image.
	URef< EdBrushImage >	GetBrushImage( const UPath& name );

	// returns the name of the script a material should reside in.
	void					GetScriptName( tstring& scriptName, const tstring& brushName );

	// collect any garbage.
	void					CollectGarbage();

private:
	typedef std::map< UPath, URef< EdBrush > > BrushMap;
	typedef std::vector< EdBrush* > BrushVec;

	typedef std::map< UPath, URef< EdBrushImage > > BrushImageMap;

	BrushImageMap			_brushImageMap;
	BrushMap				_brushMap;
	BrushVec				_brushes;
	UPath					_brushRoot;
};
extern EdBrushMgr* gEdBrushMgr;