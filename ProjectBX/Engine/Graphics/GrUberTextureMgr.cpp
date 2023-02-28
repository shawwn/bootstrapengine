//----------------------------------------------------------
// File:        GrUberTextureMgr.cpp
// Author:      Kevin Bray
// Created:     01-01-09
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUberTextureMgr.h"

// singleton pointer.
GrUberTextureMgr* gGrUberTextureMgr = 0;

//**********************************************************
// class GrUberTextureMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUberTextureMgr::GrUberTextureMgr()
{
    B_ASSERT( gGrUberTextureMgr == 0 );
    gGrUberTextureMgr = this;
}

//----------------------------------------------------------
GrUberTextureMgr::~GrUberTextureMgr()
{
    B_ASSERT( gGrUberTextureMgr != 0 );
    gGrUberTextureMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrUberTexture >
GrUberTextureMgr::CreateUberTexture( const UPath& name, const void* tileUsageBitmap, const char* metaData,
                                     unsigned int metaDataSize )
{
    // check to see if the ubertexture already exists and is loaded.
    // If so, we need to return null.
    UberTextureMap::iterator iter = _uberTextures.find( name );
    if ( iter != _uberTextures.end() )
        return 0;

    // create a new ubertexture.
    GrUberTexture* texture = new GrUberTexture( name, tileUsageBitmap, metaData, metaDataSize );

    // cache it and return the new ubertexture.
    _uberTextures[ name ] = texture;

    // return the new ubertexture.
    return texture;
}

//----------------------------------------------------------
URef< GrUberTexture >
GrUberTextureMgr::GetUberTexture( const UPath& name )
{
    // check to see if the ubertexture is already loaded.
    UberTextureMap::iterator iter = _uberTextures.find( name ); 
    if ( iter != _uberTextures.end() )
        return iter->second;

    // load the ubertexture from disk and cache it if it's not
    // already loaded.
    GrUberTexture* texture = new GrUberTexture( name );

    // cache it and return the new ubertexture.
    _uberTextures[ name ] = texture;

    // return the texture.
    return texture;
}

//----------------------------------------------------------
void
GrUberTextureMgr::CollectGarbage()
{
    UberTextureMap::iterator iter = _uberTextures.begin();
    while ( iter != _uberTextures.end() )
    {
        if ( iter->second->GetRef() == 1 )
            iter = _uberTextures.erase( iter );
        else
            ++iter;
    }
}
