//----------------------------------------------------------
// File:        EdBrushPaletteMgr.cpp
// Author:      Kevin Bray
// Created:     06-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBrushPaletteMgr.h"

// project includes.
#include "EdBrushPalette.h"
#include "EdConfig.h"
#include "EdParse.h"

// engine includes.
#include "RRsrcMgr.h"

// singleton pointer.
EdBrushPaletteMgr* gEdBrushPaletteMgr = 0;

//**********************************************************
// class EdBrushPaletteMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBrushPaletteMgr::EdBrushPaletteMgr( const UPath& brushPaletteRoot )
: _brushPaletteRoot( brushPaletteRoot )
{
    B_ASSERT( gEdBrushPaletteMgr == 0 );
    gEdBrushPaletteMgr = this;

    // register the brush parsing method with the resource manager.
    gRRsrcMgr->RegisterScriptParser( ED_KW_BRUSHPALETTE, ParseBrushPalette );
}

//----------------------------------------------------------
EdBrushPaletteMgr::~EdBrushPaletteMgr()
{
    // clear the singleton pointer.
    gEdBrushPaletteMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdBrushPaletteMgr::LoadBrushPaletteScripts()
{
    // load the brush palette script.
    tstring error;
    return gRRsrcMgr->LoadScript( ED_BRUSH_PALETTE_FILENAME, ED_KW_BRUSHPALETTE, error );
}

//----------------------------------------------------------
URef< EdBrushPalette >
EdBrushPaletteMgr::SetBrushPalette( const UPath& name, const BrushVec& brushes )
{
    BrushPaletteMap::iterator iter = _brushPaletteMap.find( name );
    if ( iter != _brushPaletteMap.end() )
    {
        // rebuild the brush palette.
        iter->second->Rebuild( brushes );
        return iter->second;
    }

    // simply create a new brush palette.
    EdBrushPalette* palette = new EdBrushPalette( name.GetPathString(), brushes );
    _brushPaletteMap[ name ] = palette;
    _brushPalettes.push_back( palette );

    // return the brush palette.
    return palette;
}

//----------------------------------------------------------
URef< EdBrushPalette >
EdBrushPaletteMgr::GetBrushPalette( const UPath& name, tstring& error )
{
    // check to see if the palette is already being used.
    BrushPaletteMap::iterator iter = _brushPaletteMap.find( name );
    if ( iter != _brushPaletteMap.end() )
        return iter->second;

    // if we didn't find it, determine what script we need to load.
    tstring scriptName;
    GetScriptName( scriptName, name.GetPathString() );

    // load the script needed.
    if ( !gRRsrcMgr->LoadScript( scriptName, ED_KW_BRUSHPALETTE, error ) )
        return 0;

    // try to find the brush now that the script was loaded.
    iter = _brushPaletteMap.find( name );
    if ( iter != _brushPaletteMap.end() )
        return iter->second;

    // return an error since the palette was not found.
    error = "Brush " + name.GetPathString() + " not found.";
    return 0;
}

//----------------------------------------------------------
void
EdBrushPaletteMgr::RemoveBrushPalette( const UPath& name )
{
    // NOTE:  This function is *broken*!  This deletion is not recorded (serialized).
    // This means that the next time that brush palettes are reloaded, the deleted
    // brush palette will reappear.

    // check to see if the palette is already being used.
    BrushPaletteMap::iterator iter = _brushPaletteMap.find( name );
    if ( iter != _brushPaletteMap.end() )
    {
        // remove the palette.
        for( BrushPaletteVec::iterator it = _brushPalettes.begin(); it != _brushPalettes.end(); ++it )
        {
            if( *it == iter->second )
            {
                _brushPalettes.erase( it );
                break;
            }
        }
        _brushPaletteMap.erase( iter );
    }
}

//----------------------------------------------------------
unsigned int
EdBrushPaletteMgr::GetBrushPaletteCount() const
{
    return ( unsigned int )_brushPalettes.size();
}

//----------------------------------------------------------
URef< EdBrushPalette >
EdBrushPaletteMgr::GetBrushPaletteByIdx( unsigned int idx ) const
{
    B_ASSERT( idx < GetBrushPaletteCount() );
    return _brushPalettes[ idx ];
}

//----------------------------------------------------------
void
EdBrushPaletteMgr::GetScriptName( tstring& scriptName, const tstring& brushPaletteName )
{
    // if we didn't find it, determine what script we need to load.
    UPath scriptNamePath = _brushPaletteRoot;
    size_t scriptPathEnd = brushPaletteName.find_last_of( '/' );
    if ( scriptPathEnd != tstring::npos )
    {
        scriptName = brushPaletteName;
        scriptName = scriptName.substr( 0, scriptPathEnd );
        scriptPathEnd = scriptName.find_last_of( '/' );
        if ( scriptPathEnd != tstring::npos )
            scriptName = scriptName.substr( 0, scriptPathEnd + 1 );
    }
    else
        scriptName = "";

    scriptNamePath += scriptName;
    scriptNamePath += ED_BRUSH_PALETTE_FILENAME;
    scriptName = scriptNamePath.GetPathString();
}

