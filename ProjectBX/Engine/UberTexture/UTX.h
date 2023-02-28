//----------------------------------------------------------
// File:		UTX.h
// Author:		Kevin Bray
// Created:		11-25-07
//
// Purpose:		To provide the entry point for the ubertexture server.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

struct UTHeader;
struct UTStageHeader;
struct UTChannelHeader;
struct UTRowOffset;
struct UTTileOffset;
struct UTTileAddrMap;

//----------------------------------------------------------
// UTFlags:  these are flags that pertain to the entire ubertexture.
//----------------------------------------------------------
typedef unsigned int UTFlags;

// UTX_FLAGS_RGBA means the channels contain raw RGBA data.  The way the channels
// are interpreted depends on how many channels are present.  
// 1 channel:	intensity
// 2 channels:	luminance, alpha
// 3 channels:  blue, green, red
// 4 channels:  blue, green, red, alpha
//
// If this flag is not present, it means the channels contain HSLA data.
// The way the channels are interpreted depends on how many channels are
// present.
// 1 channel:	luminance, hue = 0, saturation = 0
// 2 channels:	luminance, hue, saturation = 1
// 3 channels:  luminance, hue, saturation
// 4 channels:	luminance, hue, saturation, alpha
#define UTX_FLAGS_RGBA					1

// UTX_FLAGS_FINALIZED means that the texture is compressed and subsequently read
// only.
#define	UTX_FLAGS_FINALIZED				2

//----------------------------------------------------------
// UTChannelFlags:  these are flags that a present on a per-channel basis.
//----------------------------------------------------------
typedef unsigned int UTChannelFlags;

// kSubSampled means that the channel is half the width and height
// of the texture.
#define UTX_CHANNELFLAGS_SUBSAMPLED		1

// UTX_kDCT means that the tiles in the channel are compressed using
// an 8x8 block DCT compression scheme.
#define UTX_CHANNELFLAGS_DCT			2

#ifndef _MSC_VER
#  define __int64 long long
#endif
//----------------------------------------------------------
// UTX_stream_t: holds data associated with an ubertexture.
//----------------------------------------------------------
struct UTX_stream_t
{
	// file info.
	void* file;
	char* fileName;
	int widthInTiles;
	int heightInTiles;
	int mipLevels;
	UTHeader* header;
	UTStageHeader* stageHeaders;
	UTChannelHeader** channelHeaders;

	// internal caches.
	UTTileAddrMap* tileAddrMaps;

	// current file offset.
	__int64 offset;
};

// UTX_InitStream: fills out the stream to valid default values.
extern void UTX_InitStream( UTX_stream_t* stream );

// UTX_IsStream: returns true if the stream is a valid, open stream.
extern bool UTX_IsStream( UTX_stream_t* stream );

// UTX_CreateStream: creates an ubertexture for reading and writing.
extern bool UTX_CreateStream( UTX_stream_t* stream, const char* fileName, int width, int height, int stageCount,
							  UTFlags* flags, int* channels );

// UTX_OpenStream: opens an ubertexture for reading and writing.
extern bool UTX_OpenStream( UTX_stream_t* stream, const char* fileName );

// UTX_FinalizeStream: finalizes a stream by compressing it.  Once a stream
// is compressed, it can no longer be changed.
extern bool UTX_FinalizeStream( UTX_stream_t* stream, UTChannelFlags* channelFlags, unsigned int quality );

// UTX_GetWidth, UTX_GetHeight, UTX_NumChannels, UTX_GetFlags: returns stream
// data for valid streams.  If the stream is not valid, -1 is returned.
extern int UTX_GetWidth( UTX_stream_t* stream );
extern int UTX_GetHeight( UTX_stream_t* stream );
extern int UTX_NumStages( UTX_stream_t* stream );
extern UTFlags UTX_GetStageFlags( UTX_stream_t* stream, int stage );
extern int UTX_NumStageChannels( UTX_stream_t* stream, int stage );
extern int UTX_CalcStageTileSize( UTX_stream_t* stream, int stage );

// UTX_CalcTileSize: calculates the size of an uncompressed tile.
extern int UTX_CalcTileSize( UTX_stream_t* stream );

// UTX_CloseStream: closes the stream passed in.
extern void UTX_CloseStream( UTX_stream_t* stream );

// UTX_OptimizeReadOrder: reorders a list of packed tiles for optimal disk access.
// This function returns false if any invalid tiles were found.
extern void UTX_OptimizeReadOrder( UTX_stream_t* stream, unsigned int* packedTiles, unsigned int count );

// UTX_ReadTile: reads numStages of 64x64 tiles from the uber-texture.  The
// destination buffer must be large enough to hold numStages of uncompressed data.
extern bool UTX_ReadTile( UTX_stream_t* stream, void* dest, int x, int y, int mipLevel, int* bytesRead = 0 );

// UTX_WriteTile: writes numStages of 64x64 tiles to the uber-texture.  The
// source buffer must contain numStages of uncompressed pixel data.
// Note that the source buffer is trashed.
extern bool UTX_WriteTile( UTX_stream_t* stream, int x, int y, int mipLevel, const void* source, int* bytesWritten = 0 );
