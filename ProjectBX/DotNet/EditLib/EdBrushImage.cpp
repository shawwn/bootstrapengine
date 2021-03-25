//----------------------------------------------------------
// File:		EdBrushImage.cpp
// Author:		Kevin Bray
// Created:		08-07-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBrushImage.h"

// engine includes.
#include "GrImage.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "UReader.h"


//**********************************************************
// class EdBrushImage
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBrushImage::EdBrushImage( const UPath& filePath )
: _filePath( filePath )
, _file( 0 )
, _image( 0 )
{
	B_ASSERT( filePath.GetPathString().length() > 0 );
	_file = gRFileMgr->GetFile( filePath, RFileMgr::kFlagRead );
}

//----------------------------------------------------------
EdBrushImage::~EdBrushImage()
{
	delete _image;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdBrushImage::Exists() const
{
	// return true if the file exists.
	return ( _file != 0 );
}

//----------------------------------------------------------
GrImage*
EdBrushImage::GetImage()
{
	PreCache();
	return _image;
}

//----------------------------------------------------------
void
EdBrushImage::PreCache()
{
	// simply return if the image is already cached.
	if ( _image )
		return;

	// if the file exists, try to create the image.
	if ( _file )
	{
		UReader reader( _file->GetData(), ( unsigned int )_file->GetSize(), false );
		_image = new GrImage( _filePath, reader );
		if ( _image->GetFormat() == GrImage::EF_BGR24 )
			_image->ConvertBGR24ToBGRA32();
		_file->Purge();
	}
	else
	{
		_image = new GrImage( _filePath, 0xFFFF0000 );
	}
}

//----------------------------------------------------------
void
EdBrushImage::Evict()
{
	// simply return if the image isn't cached.
	if ( !_image )
		return;

	// delete the image and set it to null.
	delete _image;
	_image = 0;
}
