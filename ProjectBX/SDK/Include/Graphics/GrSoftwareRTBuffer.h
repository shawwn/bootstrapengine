//----------------------------------------------------------
// File:		GrSoftwareRTBuffer.h
// Author:		Kevin Bray
// Created:		08-14-08
//
// Purpose:		To provide an in-memory render target.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class.
#include "GrSoftwareRTBase.h"

//**********************************************************
// class GrSoftwareRTBuffer
//**********************************************************
class GrSoftwareRTBuffer : public GrSoftwareRTBase
{
public:
	GrSoftwareRTBuffer( unsigned int width, unsigned int height, unsigned int bytesPerPel );
	~GrSoftwareRTBuffer();

	// return info about the target.
	unsigned int	GetBytesPerPel() const			{	return _bytesPerPel;		}
	unsigned int	GetWidth() const				{	return _width;				}
	unsigned int	GetHeight() const				{	return _height;				}
	unsigned int	GetStride() const				{	return _stride;				}
	unsigned int	GetTile8x8Stride() const		{	return _blockStride;		}

	// tile begin and end.
	unsigned char*	BeginTile8x8( unsigned int x, unsigned int y );
	unsigned char*	BeginTile8x8WriteOnly( unsigned int x, unsigned int y );
	void			EndTile8x8( unsigned int x, unsigned int y );

	// returns a pointer to the raw data.
	unsigned char*	GetData() const					{	return _buffer;				}

	// fills the buffer with the pelClearData.  Note that pelClearData must be
	// as large as _bytesPerPel.
	void			Clear( void* pelClearData );

private:
	unsigned int	_width;
	unsigned int	_height;
	unsigned int	_bytesPerPel;
	unsigned int	_stride;
	unsigned int	_blockStride;
	unsigned char*	_buffer;
	unsigned char*	_block;
};
