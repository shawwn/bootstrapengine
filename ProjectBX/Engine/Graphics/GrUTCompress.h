//----------------------------------------------------------
// File:		GrUTCompress.h
// Author:		Kevin Bray
// Created:		11-17-08
//
// Purpose:		To provide compression services for the ubertexture
//				worker thread.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class UHuffman;

//**********************************************************
// class GrUTCompress
//**********************************************************

// initializes the compressor(s).  Note that this must be called
// before any of the compression/decompression functions are used.
extern void GrUTInitCompressor();

// ubertexture compression services
extern void GrUTCompressBGRX( UHuffman* encoder, const unsigned char* bgrx, unsigned char chop );
extern void GrUTCompressA( UHuffman* encoder, const unsigned char* a, unsigned int bytesPerPel,
						   unsigned char chop );

// ubertexture decompression services.
extern void GrUTDecompressBGRX( unsigned char* bgrx, UHuffman* decoder );
extern void GrUTDecompressA( unsigned char* a, unsigned int bytesPerPel, UHuffman* decoder );

// ubertexture compression to DXT5.
extern void GrUTCompressDXT5( unsigned char* destSurface, unsigned int destStride, unsigned char* bgra );
extern void GrUTCompressDXT5XY( unsigned char* destSurface, unsigned int destStride, unsigned char* xy );
