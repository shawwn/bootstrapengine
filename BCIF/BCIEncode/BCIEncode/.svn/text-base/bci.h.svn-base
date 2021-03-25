// bci.h
// Copyright (C) 2007, Bootstrap Studios.
//
// For use, please see the accompanying readme.
//
#pragma once

#include <stdio.h>
#include "png.h"

// size of each chunk in pixels.
#define BCI_CHUNK_SIZE				128U

// BCI quality settings.
#define BCI_QUALITY_LOW				0
#define BCI_QUALITY_LOSSLESS		100

struct PNG_src_stream_t
{
	png_structp	png_ptr;
	png_infop	png_info_ptr;
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int interlace_type;
	int number_passes;
};

extern bool InitPNGStream( PNG_src_stream_t* stream, FILE* fp );
extern bool WriteBCIImage( FILE* fp, PNG_src_stream_t* source, unsigned int quality, bool normalmap );
extern void RepackBCIImage( FILE* fp );