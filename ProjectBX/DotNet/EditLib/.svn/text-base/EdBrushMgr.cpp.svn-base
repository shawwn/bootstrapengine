//----------------------------------------------------------
// File:		EdBrushMgr.h
// Author:		Kevin Bray
// Created:		05-17-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBrushMgr.h"

// project includes.
#include "EdBrush.h"
#include "EdBrushImage.h"
#include "EdParse.h"
#include "EdConfig.h"

// engine includes.
#include "RRsrcMgr.h"

// singleton pointer.
EdBrushMgr* gEdBrushMgr = 0;

//**********************************************************
// class EdBrushMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBrushMgr::EdBrushMgr( const UPath& brushRoot )
: _brushRoot( brushRoot )
{
	// set the singleton pointer.
	B_ASSERT( gEdBrushMgr == 0 );
	gEdBrushMgr = this;

	// register the brush parsing method with the resource manager.
	gRRsrcMgr->RegisterScriptParser( ED_KW_BRUSH, ParseBrush );
}

//----------------------------------------------------------
EdBrushMgr::~EdBrushMgr()
{
	gEdBrushMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< EdBrush >
EdBrushMgr::SetBrush( const tstring& name, const EdBrush::SInfo& info )
{
	// check to see if the brush is already being used.
	BrushMap::iterator iter = _brushMap.find( name );
	if ( iter != _brushMap.end() )
	{
		// rebuild the brush.
		iter->second->Rebuild( info );
		return iter->second;
	}

	// simply create a new brush.
	EdBrush* brush = new EdBrush( name, info );
	_brushMap[ name ] = brush;
	_brushes.push_back( brush );

	// return the brush.
	return brush;
}

//----------------------------------------------------------
URef< EdBrush >
EdBrushMgr::GetBrush( const UPath& name, tstring& error )
{
	// check to see if the material is already being used.
	BrushMap::iterator iter = _brushMap.find( name );
	if ( iter != _brushMap.end() )
		return iter->second;

	// if we didn't find it, determine what script we need to load.
	tstring scriptName;
	GetScriptName( scriptName, name.GetPathString() );

	// load the script needed.
	if ( !gRRsrcMgr->LoadScript( scriptName, ED_KW_BRUSH, error ) )
		return 0;

	// try to find the brush now that the script was loaded.
	iter = _brushMap.find( name );
	if ( iter != _brushMap.end() )
		return iter->second;

	// return an error since the material was not found.
	error = "Brush " + name.GetPathString() + " not found.";
	return 0;
}

//----------------------------------------------------------
unsigned int
EdBrushMgr::GetBrushCount() const
{
	return ( unsigned int )_brushes.size();
}

//----------------------------------------------------------
URef< EdBrush >
EdBrushMgr::GetBrushByIdx( unsigned int idx ) const
{
	B_ASSERT( idx < GetBrushCount() );
	return _brushes[ idx ];
}

//----------------------------------------------------------
URef< EdBrushImage >
EdBrushMgr::GetBrushImage( const UPath& name )
{
	// make sure the name passed in is valid.
	B_ASSERT( name.GetPathString().length() > 0 );

	// check for the image.
	BrushImageMap::iterator iter = _brushImageMap.find( name );
	if ( iter != _brushImageMap.end() )
		return iter->second;

	// if we didn't find it, create a new one and add it to our
	// cache.
	EdBrushImage* image = new EdBrushImage( name );
	_brushImageMap[ name ] = image;

	// return the new image.
	return image;
}

//----------------------------------------------------------
void
EdBrushMgr::GetScriptName( tstring& scriptName, const tstring& brushName )
{
	// if we didn't find it, determine what script we need to load.
	UPath scriptNamePath = _brushRoot;
	size_t scriptPathEnd = brushName.find_last_of( '/' );
	if ( scriptPathEnd != tstring::npos )
	{
		scriptName = brushName;
		scriptName = scriptName.substr( 0, scriptPathEnd );
		scriptPathEnd = scriptName.find_last_of( '/' );
		if ( scriptPathEnd != tstring::npos )
			scriptName = scriptName.substr( 0, scriptPathEnd + 1 );
	}
	else
		scriptName = "";

	scriptNamePath += scriptName;
	scriptNamePath += ED_BRUSH_FILENAME;
	scriptName = scriptNamePath.GetPathString();
}

//----------------------------------------------------------
void
EdBrushMgr::CollectGarbage()
{
	
}
