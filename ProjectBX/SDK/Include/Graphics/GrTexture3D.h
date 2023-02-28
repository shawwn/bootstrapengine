//----------------------------------------------------------
// File:		GrTexture3D.h
// Author:		Kevin Bray
// Created:		12-03-05
//
// Purpose:		To maintain an OpenGL 3D texture object.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

#include "GrTextureBase.h"

// forward declarations.
class UPath;

//**********************************************************
// class GrTexture3D
//**********************************************************
class GrTexture3D : public GrTextureBase
{
public:
	// note that the constructor takes over ownership of the data pointer.  Note that
	// 3D textures are not compressed or mipmapped.  The lack of mipmapping is just a
	// design decision for now; it is not a hardware limitation.  For a list of data
	// formats supported, see the _dataFormats lookup table in the source file.  All
	// formats use unsigned 8-bit pixel components in the order specified.
	GrTexture3D( const UPath& name, EFORMAT format, unsigned int width, unsigned int height,
			     unsigned int depth, const void* data );
	// destructor.
	~GrTexture3D();

	// size info.
	unsigned int		GetWidth() const									{	return _width;				}
	unsigned int		GetHeight() const									{	return _height;				}
	unsigned int		GetDepth() const									{	return _depth;				}

private:
	void				UploadImage( bool mipmap );

	unsigned int		_width;
	unsigned int		_height;
	unsigned int		_depth;
	void*				_data;
};
