//----------------------------------------------------------
// File:		GrSoftwareTextureMgr.cpp
// Author:		Shawn Presser
// Created:		09-13-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSoftwareTextureMgr.h"

// engine headers.
#include "GrSoftwareTexture.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrImage.h"

GrSoftwareTextureMgr* gGrSoftwareTextureMgr;

//----------------------------------------------------------
GrSoftwareTextureMgr::GrSoftwareTextureMgr()
{
    // set the singleton pointer.
    B_ASSERT( !gGrSoftwareTextureMgr );
    gGrSoftwareTextureMgr = this;
}

//----------------------------------------------------------
GrSoftwareTextureMgr::~GrSoftwareTextureMgr()
{
    // remove the singleton pointer.
    B_ASSERT( gGrSoftwareTextureMgr );
    gGrSoftwareTextureMgr = 0;
}

//----------------------------------------------------------
URef< GrSoftwareTexture >       
GrSoftwareTextureMgr::GetTexture( const UPath& filePath )
{
    // try to find a texture with that path.
    TextureContainer::iterator it = _textures.find( filePath );

    // if it exists, return it.
    if ( it != _textures.end() )
        return it->second;

	// read the entire file.
	URef< RFile > file = gRFileMgr->GetFile( filePath, RFileMgr::kFlagRead );
	if ( !file )
		return false;

	const void *fileData = file->GetData();
	unsigned int fileSize = ( unsigned int )file->GetSize();

	// read in the image data.
	GrImage* image = new GrImage( filePath, UReader( fileData, fileSize, false ) );

	// purge the data from the file so that we don't keep it loaded.
	file->Purge();

    // construct the software texture and insert it into the map.
    URef< GrSoftwareTexture > texture( new GrSoftwareTexture( image ) );
    _textures.insert( std::make_pair( filePath, texture ) );

    // return the texture.
    return texture;
}

//----------------------------------------------------------
URef< GrSoftwareTexture >       
GrSoftwareTextureMgr::FindTexture( const UPath& filePath )
{
    // try to find a texture with that path.
    TextureContainer::iterator it = _textures.find( filePath );

    // if it exists, return it.
    if ( it != _textures.end() )
        return it->second;

    // otherwise, return null.
    return 0;
}