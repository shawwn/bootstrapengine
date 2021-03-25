//----------------------------------------------------------
// File:		GrTextureRect.h
// Author:		Kevin Bray
// Created:		04-12-06
//
// Purpose:		To manage a texture rectangle.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTextureBase.h"

//**********************************************************
// class GrTextureRect
//**********************************************************
class GrTextureRect : public GrTextureBase
{
public:
	GrTextureRect( const UPath& name, EFORMAT format, unsigned int width, unsigned int height );
	~GrTextureRect();

	// size info.
	unsigned int		GetWidth() const									{	return _width;				}
	unsigned int		GetHeight() const									{	return _height;				}

	// binds to the actively selected framebuffer at the attachment passed in.
	void				BindToFramebuffer( unsigned int attachment );

	// uploads data to the card.
	void				UploadPixelData( int dstX, int dstY, int width, int height, const void* srcPels,
										 GLenum srcFormat, GLenum srcType );

private:
	void				UploadImage( bool mipmap );

	unsigned int		_width;
	unsigned int		_height;
};
