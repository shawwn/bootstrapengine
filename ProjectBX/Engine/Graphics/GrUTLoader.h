//----------------------------------------------------------
// File:		GrUTLoader.h
// Author:		Kevin Bray
// Created:		10-28-2008
//
// Purpose:		To load and decode ubertexture tiles.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "FSDiskServ.h"
#include "UHuffman.h"

// forward declarations.
class GrUTAtlas;

//==========================================================
// class GrUTLoader
//==========================================================
class GrUTLoader
{
public:
	// This callback is called whenever progress is updated.  It's
	// called 256 times, where the value 255 means the operation was
	// just completed.
	typedef void ( *ProgressCallback )( const char* stepText, unsigned int stepCount, unsigned int step,
		unsigned int stepProgress );

	enum ECOLORMODE
	{
		EDM_BGRA,			// nx, ny, b, g, r, b, g, r
		EDM_BGRA_DXT,		// DXT5 (r = nx, a = ny), DXT5(b, g, r, a), DXT5(b, g, r, a)
		EDM_YCoCg,			// nx, ny, Y, Co, Cg, a, Y, Co, Cg, a
		EDM_YCoCg_DXT,		// DXT5 (r = nx, a = ny), DXT5(Co, Cg, 0, Y), DXT5(Co, Cg, 0, Y)
	};

	GrUTLoader( unsigned int workerThreadCount = 2 );
	~GrUTLoader();

	// builds an ubertexture using the data specified.  Note that the
	// tile-use mask contains a bit for every tile, and specifies 1
	// for tiles that are used, and 0 for tiles that are not.  Note
	// that tiles are in morton order (bit 0 = 0,0, bit 1 = 1,0,
	// bit 2 = 0,1, etc.)
	GrUTAtlas*			CreateUbertex( void* file, const void* tileUseBitMask, const void* metaData,
									   unsigned int metaDataSize, ProgressCallback callback );
	GrUTAtlas*			LoadUbertex( void* file );
	void				ChangeLayout( void* file, const void* tileUseBitMask, ProgressCallback callback );

	// call this to free an atlas.
	void				FreeAtlas( GrUTAtlas* atlas );

	// reads a tile from the ubertexture file.  Note that dxtEncode
	// is a boolean variable.  This causes all channels to be packed
	// as follows:  RGTC, DXT5, DXT5.
	void				DecodeChunk( void* dst, volatile int* lock, void* file, unsigned int chunkIndex,
									 ECOLORMODE dxtMode = GrUTLoader::EDM_BGRA_DXT );

	// writes a tile to the ubertexture file.  Note that the source
	// data must be packed as follows:
	// normal map X, Y
	// diffuse B, G, R, A
	// specular B, G, R, A
	void				WriteChunk( void* file, unsigned int chunkIndex, const void* src );

	// wait for all decode requests to finish.
	void				Flush();

private:
	void				FillChunks( void* file, GrUTAtlas* atlas, const void* tileData, unsigned int depth = 0 );

	UHuffman			_encoder;
	SFSFileWriteBuffer*	_writeBuffers;
};
extern GrUTLoader* gGrUTLoader;