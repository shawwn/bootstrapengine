//----------------------------------------------------------
// File:		EdBrushImage.h
// Author:		Kevin Bray
// Created:		08-07-05
//
// Purpose:		To manage an image that's used a source for a brush.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "URef.h"
#include "UPath.h"

// forward declarations.
class GrImage;
class RFile;

//**********************************************************
// class EdBrushImage
//**********************************************************
class EdBrushImage : public URefCount
{
public:
	EdBrushImage( const UPath& filePath );
	~EdBrushImage();

	// returns true if the image is a valid file.
	bool				Exists() const;

	GrImage*			GetImage();
	const UPath&		GetFilePath() const		{	return _filePath;	}

	void				PreCache();
	void				Evict();

private:
	UPath				_filePath;
	URef< RFile	>		_file;
	GrImage*			_image;
};
