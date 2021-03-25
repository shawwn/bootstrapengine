//----------------------------------------------------------
// File:		GrDXT.h
// Author:		Kevin Bray
// Created:		04-03-08
//
// Purpose:		To perform DXT compression in realtime.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// returns the size of the image data for a compressed image of
// the specified format.
extern unsigned int CalcImageSizeDXT1( unsigned int width, unsigned int height );
extern unsigned int CalcImageSizeDXT5( unsigned int width, unsigned int height );

// BGRX to DXT1.
extern void CompressDXT1( unsigned int* dst, unsigned int width, unsigned int height, void* src );
extern void CompressDXT1BGR( unsigned int* dst, unsigned int width, unsigned int height, void* src );

// BGRA to DXT5.
extern void CompressDXT5( unsigned int* dst, unsigned int width, unsigned int height, void* src );
extern void CompressDXT5XY1( unsigned int* dst, unsigned int width, unsigned int height, void* src );

// fills a DXT encoded image with the color specified.
extern void FillDXT1( void* dst, unsigned int width, unsigned int height, unsigned char* bgrxColor );
extern void FillDXT5( void* dst, unsigned int width, unsigned int height, unsigned char* bgraColor );