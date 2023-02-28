//----------------------------------------------------------
// File:		GrTextureCube.h
// Author:		Kevin Bray
// Created:		07-14-05
//
// Purpose:		To maintain an OpenGL cube texture object.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

#include "GrTextureBase.h"

// forward declarations.
class GrImage;

//**********************************************************
// class GrTextureCube
//**********************************************************
class GrTextureCube : public GrTextureBase
{
public:
	enum ECUBEFACE
	{
		CF_POSITIVEX,
		CF_NEGATIVEX,
		CF_POSITIVEY,
		CF_NEGATIVEY,
		CF_POSITIVEZ,
		CF_NEGATIVEZ,
	};

	// always uses a linear magnification filter, nearest minification filter,
	// and linear mipmap filter.  For the constructor that takes images, 6 images
	// must be specified.  The images passed in must all have the same width and
	// height, and must also be square.
	GrTextureCube( const UPath& name, GrTextureBase::EFORMAT format, unsigned int width, unsigned int flags );
	GrTextureCube( const UPath& name, const UPath** images, GrTextureBase::EFORMAT format, unsigned int flags, bool cacheImages );
	GrTextureCube( const UPath& name, GrImage** images, GrTextureBase::EFORMAT format, unsigned int flags );
	~GrTextureCube();

	// returns true if the images meet the criteria to create a cube map.
	static bool			IsValidImageSet( GrImage** images );

	// returns the dimensions of the cube.
	unsigned int		GetDim() const				{	return _width;			}

	// binds to the actively selected framebuffer at the attachment passed in.
	void				BindToFramebuffer( unsigned int attachment, ECUBEFACE cubeFace );

private:
	void				UploadImage( bool mipmap );
	GrImage*			LoadImg( const UPath& fileName );

	GrImage*			_images[ 6 ];
	UPath				_rsrcPaths[ 6 ];
	unsigned int		_width;
	bool				_fromFile;
	bool				_ownImages;
};
