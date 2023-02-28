//----------------------------------------------------------
// File:		UTX.cpp
// Author:		Kevin Bray
// Created:		11-25-07
//
// Purpose:		To provide the entry point for the ubertexture server.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "ubertexture_afx.h"

// file header.
#include "UTX.h"

// project includes.
#include "UTConfig.h"
#include "common_afx.h"

#include <assert.h>

// I need std::sort.
#include <algorithm>

#define PROFILE_DISK_SPEED		0

#if PROFILE_DISK_SPEED
#include <mmsystem.h>
#endif

#ifdef _MSC_VER
#  pragma warning ( disable : 4996 )
#else
#  define min std::min
#  define max std::max
#endif

// debugging data.
#define UT_TRACK_STATS			1

#define UT_ALIGNMENT			2048
#define UT_ALIGNMENT_MASK		2047

#define ADDR_MAP_COUNT			12

// the DCT matrix.
static const float kDCTMatrix[ 64 ] =
{
	0.35355338f,	0.35355338f,	0.35355338f,	0.35355338f,	0.35355338f,	0.35355338f,	0.35355338f,	0.35355338f,
	0.49039266f,	0.41573483f,	0.27778521f,	0.097545296f,	-0.097544990f,	-0.27778488f,	-0.41573471f,	-0.49039257f,
	0.46193978f,	0.19134183f,	-0.19134153f,	-0.46193966f,	-0.46193990f,	-0.19134222f,	0.19134136f,	0.46193948f,
	0.41573483f,	-0.097544990f,	-0.49039257f,	-0.27778542f,	0.27778462f,	0.49039274f,	0.097545967f,	-0.41573435f,
	0.35355344f,	-0.35355324f,	-0.35355368f,	0.35355300f,	0.35355389f,	-0.35355261f,	-0.35355395f,	0.35355240f,
	0.27778521f,	-0.49039257f,	0.097544737f,	0.41573521f,	-0.41573435f,	-0.097546041f,	0.49039292f,	-0.27778384f,
	0.19134183f,	-0.46193990f,	0.46193948f,	-0.19134107f,	-0.19134280f,	0.46194023f,	-0.46193913f,	0.19134019f,
	0.097545296f,	-0.27778542f,	0.41573521f,	-0.49039286f,	0.49039245f,	-0.41573399f,	0.27778399f,	-0.097542964f,
};

static const float kInvDCTMatrix[ 64 ] =
{
	0.35355338f,	0.49039266f,	0.46193978f,	0.41573483f,	0.35355344f,	0.27778521f,	0.19134183f,	0.097545296f,
	0.35355338f,	0.41573483f,	0.19134183f,	-0.097544990f,	-0.35355324f,	-0.49039257f,	-0.46193990f,	-0.27778542f,
	0.35355338f,	0.27778521f,	-0.19134153f,	-0.49039257f,	-0.35355368f,	0.097544737f,	0.46193948f,	0.41573521f,
	0.35355338f,	0.097545296f,	-0.46193966f,	-0.27778542f,	0.35355300f,	0.41573521f,	-0.19134107f,	-0.49039286f,
	0.35355338f,	-0.097544990f,	-0.46193990f,	0.27778462f,	0.35355389f,	-0.41573435f,	-0.19134280f,	0.49039245f,
	0.35355338f,	-0.27778488f,	-0.19134222f,	0.49039274f,	-0.35355261f,	-0.097546041f,	0.46194023f,	-0.41573399f,
	0.35355338f,	-0.41573471f,	0.19134136f,	0.097545967f,	-0.35355395f,	0.49039292f,	-0.46193913f,	0.27778399f,
	0.35355338f,	-0.49039257f,	0.46193948f,	-0.41573435f,	0.35355240f,	-0.27778384f,	0.19134019f,	-0.097542964f,
};

static float kBaseQualityMatrix[ 64 ] =
{
		16.0f,		11.0f,		10.0f,		16.0f,		24.0f,		40.0f,		51.0f,		61.0f,
		12.0f,		12.0f,		14.0f,		19.0f,		26.0f,		58.0f,		60.0f,		55.0f,
		14.0f,		13.0f,		16.0f,		24.0f,		40.0f,		57.0f,		69.0f,		56.0f,
		14.0f,		17.0f,		22.0f,		29.0f,		51.0f,		87.0f,		80.0f,		62.0f,
		18.0f,		22.0f,		37.0f,		56.0f,		68.0f,		109.0f,		103.0f,		77.0f,
		24.0f,		35.0f,		55.0f,		64.0f,		81.0f,		104.0f,		113.0f,		92.0f,
		49.0f,		64.0f,		78.0f,		87.0f,		103.0f,		121.0f,		120.0f,		101.0f,
		72.0f,		92.0f,		95.0f,		98.0f,		112.0f,		100.0f,		103.0f,		99.0f,
};

static const unsigned int kDCTBlockSwizzle[ 64 ] =
{
	0,
	1,	8,
	16,	9,	2,
	3,	10,	17,	24,
	32,	25,	18,	11,	4,
	5,	12,	19,	26,	33,	40,
	48,	41,	34,	27,	20,	13,	6,
	7,	14,	21,	28,	35,	42,	49,	56,
	57,	50,	43,	36,	29,	22,	15,
	23,	30,	37,	44,	51,	58,
	59,	52,	45,	38,	31,
	39,	46,	53,	60,
	61,	54,	47,
	55,	62,
	63,
};

#ifdef __GNUC__
 #pragma pack ( push, 1 )
#else
 #pragma pack ( push )
 #pragma pack ( 1 )
#endif
//----------------------------------------------------------
// UTHeader:  the header for an ubertexture.
//----------------------------------------------------------
struct UTHeader
{
	int magic;				// "UTEX"
	int version;
	int width;
	int height;
	int numStages;
	int tileSize;
};

//----------------------------------------------------------
// UTStageHeader:  the header for an ubertexture stage.
//----------------------------------------------------------
struct UTStageHeader
{
	UTFlags flags;
	int numChannels;
};

//----------------------------------------------------------
// UTChannelHeader:  the header for an ubertexture channel.
//----------------------------------------------------------
struct UTChannelHeader
{
	// flags for the current channel.
	UTChannelFlags flags;
};

//----------------------------------------------------------
// UTRowHeader:  the header for each row of tiles in the ubertexture.
//----------------------------------------------------------
struct UTRowHeader
{
	int prevRow;
	int rowSize;
};

//----------------------------------------------------------
// UTSortInfo:  a structure that can be used to sort tiles by their address.
//----------------------------------------------------------
struct UTSortInfo
{
	unsigned int addr;
	unsigned int tile;
};

static bool UTSortInfoCompare( const UTSortInfo& lhs, const UTSortInfo& rhs )
{
	return ( lhs.addr < rhs.addr );
}

//----------------------------------------------------------
// UTTileAddrMap:  a cache that maps x and y to a linear address.
//----------------------------------------------------------
struct UTTileAddrMap
{
	int* table;
};

// table of address maps.  To use this, use i = Log2( widthInTiles ), j = Log2( heightInTiles )
// _addrMaps[ i ][ j ]
static UTTileAddrMap* _addrMaps[ UTX_MAX_MIP_LEVEL_COUNT ][ UTX_MAX_MIP_LEVEL_COUNT ];

// global tile cache.
static bool _utxInit = false;
static void* _buffer = 0;
static unsigned int _bufferSize = 0;

//**********************************************************
// File local forward declarations.
//**********************************************************
static void CheckInit();
static void OnExit();
static void EnsureBuffer( unsigned int size );
static void SetStreamAddrMap( UTX_stream_t* stream );
static int GetTileAddr( UTX_stream_t* stream, int mip, int x, int y );
static void GenAddrMap( UTX_stream_t* stream );
static void BuildTileIndexMap( UTX_stream_t* stream, int& offset, int mip, int x, int y );
static char* CopyStr( const char* src );
static int CalcMipCount( int width, int height );
static bool WriteStream( UTX_stream_t* stream, void* src, unsigned int bytes );
static bool AlignStream( UTX_stream_t* stream );
static bool ReadStream( void* dst, UTX_stream_t* stream, unsigned int bytes );
static bool SetStreamPos( UTX_stream_t* stream, __int64 offset );
static bool SeekStream( UTX_stream_t* stream, __int64 offset );
static bool SeekToTile( UTX_stream_t* stream, int x, int y, int mipLevel );
static void Swizzle( unsigned char* stream, int channelCount );
static void Deswizzle( unsigned char* stream, int channelCount );
static unsigned int WriteCompressedTileData( UTX_stream_t* stream, void* src, float* qualityMatrix );
static void WriteTileData( UTX_stream_t* stream, void* src );

//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
void UTX_InitStream( UTX_stream_t* stream )
{
	stream->file = INVALID_HANDLE_VALUE;
	stream->fileName = 0;
	stream->mipLevels = 0;
	stream->header = 0;
	stream->channelHeaders = 0;
	stream->offset = 0;
}

//----------------------------------------------------------
bool UTX_IsStream( UTX_stream_t* stream )
{
	// make sure the file was successfully created.
	return ( stream->file != INVALID_HANDLE_VALUE );
}

//----------------------------------------------------------
bool UTX_CreateStream( UTX_stream_t* stream, const char* fileName, int width, int height, int stageCount,
					   UTFlags* flags, int* channels )
{
	CheckInit();

	// zero out the values in the stream structure passed in.
	memset( stream, 0, sizeof( UTX_stream_t ) );

	// verify incoming parameters.
	if ( fileName == 0 || strlen( fileName ) == 0 )
		return false;
	if ( width < UTX_TILE_SIZE || height < UTX_TILE_SIZE )
		return false;
	if ( width > UTX_MAX_SIZE || height > UTX_MAX_SIZE )
		return false;
	for ( int i = 0; i < stageCount; ++i )
	{
		if ( channels[ i ] < 1 || channels[ i ] > UTX_MAX_CHANNELS )
			return false;
		if ( flags[ i ] & UTX_FLAGS_FINALIZED )
			return false;
	}

	// create a new file.
	HANDLE file = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL );
	stream->file = ( void* )file;
	stream->offset = 0;

	// make sure the file was successfully created.
	if ( file == INVALID_HANDLE_VALUE )
		return false;

	// make a copy of the file name.
	stream->fileName = CopyStr( fileName );

	// build and write out the file header.
	UTHeader* header = ( UTHeader* )malloc( sizeof( UTHeader ) );
	UTStageHeader* stageHeaders = ( UTStageHeader* )malloc( stageCount * sizeof( UTStageHeader ) );
	UTChannelHeader** channelHeaders = ( UTChannelHeader** )malloc( stageCount * sizeof( UTChannelHeader* ) );

	// allocate channel headers and fill out stage headers.
	int totalChannelCount = 0;
	for ( int i = 0; i < stageCount; ++i )
	{
		channelHeaders[ i ] = ( UTChannelHeader* )malloc( channels[ i ] * sizeof( UTChannelHeader ) );
		for ( int j = 0; j < channels[ i ]; ++j )
			channelHeaders[ i ][ j ].flags = 0;
		stageHeaders[ i ].numChannels = channels[ i ];
		stageHeaders[ i ].flags = flags[ i ];
		totalChannelCount += channels[ i ];
	}

	// calculate the size of each tile, in bytes.
	unsigned int tileSize = totalChannelCount * UTX_CHANNEL_SIZE_IN_BYTES;
	tileSize += UT_ALIGNMENT_MASK;
	tileSize &= ~UT_ALIGNMENT_MASK;

	// set the headers to the stream.
	stream->header = header;
	stream->stageHeaders = stageHeaders;
	stream->channelHeaders = channelHeaders;

	// fill out the header.
	header->magic = UTX_MAGIC;
	header->version = UTX_FILE_VERSION;
	header->width = width;
	header->height = height;
	header->numStages = stageCount;
	header->tileSize = tileSize;

	// now store the header in a temporary write buffer.
	unsigned char writeBuffer[ UT_ALIGNMENT ];
	unsigned char* writeBufferDst = writeBuffer;
	MemSet( writeBuffer, 0, UT_ALIGNMENT );

	// write the data to the write buffer.
	MemCopy( writeBufferDst, header, sizeof( UTHeader ) );
	writeBufferDst += sizeof( UTHeader );

	// store the stage headers along with the channel headers.
	for ( int i = 0; i < stageCount; ++i )
	{
		// store the stage header.
		MemCopy( writeBufferDst, stageHeaders + i, sizeof( UTStageHeader ) );
		writeBufferDst += sizeof( UTStageHeader );

		// store the channel headers.
		int channelCount = stageHeaders[ i ].numChannels;
		int writeSize = channelCount * sizeof( UTChannelHeader );
		MemCopy( writeBufferDst, channelHeaders[ i ], writeSize );
		writeBufferDst += writeSize;
	}

	// now write the header to the file.
	WriteStream( stream, writeBuffer, UT_ALIGNMENT );

	// convert the width and height into blocks.
	int widthInTiles = width / UTX_TILE_SIZE;
	int heightInTiles = height / UTX_TILE_SIZE;

	// calculate the total number of mip levels.
	stream->widthInTiles = widthInTiles;
	stream->heightInTiles = heightInTiles;
	stream->mipLevels = CalcMipCount( widthInTiles, heightInTiles );

	// build the tile lookup table.
	SetStreamAddrMap( stream );

	// allocate a buffer to contain temporary default data.
	void* buffer = AlignedAlloc( UT_ALIGNMENT, tileSize );
	memset( buffer, 0x7F, tileSize );

	// calculate the total tile count for the entire file.
	int tileCount = 0;
	for ( int m = 0; m < stream->mipLevels; ++m )
	{
		// calculate how many tiles reside in the current mip level.
		int mipWidth = Max( stream->widthInTiles >> m, 1 );
		int mipHeight = Max( stream->heightInTiles >> m, 1 );

		// add the tile count to the running total.
		tileCount += mipWidth * mipHeight;
	}

	// now fill the rest of the file with default data.
	for ( int t = 0; t < tileCount; ++t )
	{
		// write out the current row of tiles.
		if ( !WriteStream( stream, buffer, tileSize ) )
		{
			UTX_CloseStream( stream );
			return false;
		}
	}

	// free the temporary buffer containing default data.
	AlignedFree( buffer );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTX_OpenStream( UTX_stream_t* stream, const char* fileName )
{
	CheckInit();

	// zero out the values in the stream structure passed in.
	memset( stream, 0, sizeof( UTX_stream_t ) );

	// verify incoming parameters.
	if ( fileName == 0 )
		return false;

	// create a new file.
	HANDLE file = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL );
	stream->file = ( void* )file;
	stream->offset = 0;

	// make sure the file was successfully created.
	if ( file == INVALID_HANDLE_VALUE )
		return false;

	// make a copy of the file name.
	stream->fileName = CopyStr( fileName );

	// read in the header information to a local buffer.
	unsigned char readBuffer[ UT_ALIGNMENT ];
	unsigned char* readBufferSrc = readBuffer;
	if ( !ReadStream( readBuffer, stream, UT_ALIGNMENT ) )
	{
		UTX_CloseStream( stream );
		return false;
	}

	// read in the file header.
	UTHeader* header = ( UTHeader* )malloc( sizeof( UTHeader ) );
	stream->header = header;
	MemCopy( header, readBufferSrc, sizeof( UTHeader ) );
	readBufferSrc += sizeof( UTHeader );

	// verify the header.
	if ( header->magic != UTX_MAGIC || header->version != UTX_FILE_VERSION )
	{
		UTX_CloseStream( stream );
		return false;
	}

	// read in the stage headers.
	UTStageHeader* stageHeaders = ( UTStageHeader* )malloc( header->numStages * sizeof( UTStageHeader ) );
	UTChannelHeader** channelHeaders = ( UTChannelHeader** )malloc( header->numStages * sizeof( UTChannelHeader* ) );
	for ( int i = 0; i < header->numStages; ++i )
	{
		// read in the stage header.
		MemCopy( stageHeaders + i, readBufferSrc, sizeof( UTStageHeader ) );
		readBufferSrc += sizeof( UTStageHeader );

		// read in the current stage's channel headers.
		int channelCount = stageHeaders[ i ].numChannels;
		channelHeaders[ i ] = ( UTChannelHeader* )malloc( channelCount * sizeof( UTChannelHeader ) );

		int readSize = channelCount * sizeof( UTChannelHeader );
		MemCopy( channelHeaders[ i ], readBufferSrc, readSize );
		readBufferSrc += readSize;
	}

	stream->stageHeaders = stageHeaders;
	stream->channelHeaders = channelHeaders;

	// calculate the number of mip levels.
	int widthInTiles = header->width / UTX_TILE_SIZE;
	int heightInTiles = header->height / UTX_TILE_SIZE;
	stream->widthInTiles = widthInTiles;
	stream->heightInTiles = heightInTiles;
	stream->mipLevels = CalcMipCount( widthInTiles, heightInTiles );

	// set the stream's address map.
	SetStreamAddrMap( stream );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
int UTX_GetWidth( UTX_stream_t* stream )
{
	if ( !stream->header )
		return -1;
	return stream->header->width;
}

//----------------------------------------------------------
int UTX_GetHeight( UTX_stream_t* stream )
{
	if ( !stream->header )
		return -1;
	return stream->header->height;
}

//----------------------------------------------------------
int UTX_NumStages( UTX_stream_t* stream )
{
	if ( !stream->header )
		return -1;
	return stream->header->numStages;
}

//----------------------------------------------------------
UTFlags UTX_GetStageFlags( UTX_stream_t* stream, int stage )
{
	assert( stage < stream->header->numStages );
	return stream->stageHeaders[ stage ].flags;
}

//----------------------------------------------------------
int UTX_NumStageChannels( UTX_stream_t* stream, int stage )
{
	assert( stage < stream->header->numStages );
	return stream->stageHeaders[ stage ].numChannels;
}

//----------------------------------------------------------
int UTX_CalcStageTileSize( UTX_stream_t* stream, int stage )
{
	assert( stage < stream->header->numStages );
	return ( stream->stageHeaders[ stage ].numChannels * UTX_CHANNEL_SIZE_IN_BYTES );
}

//----------------------------------------------------------
int UTX_CalcTileSize( UTX_stream_t* stream )
{
	// accumulate the size of each stage.
	int size = 0;
	int stageCount = UTX_NumStages( stream );
	for ( int s = 0; s < stageCount; ++s )
		size += UTX_CalcStageTileSize( stream, s );

	// return the tile's size.
	return size;
}

//----------------------------------------------------------
void UTX_CloseStream( UTX_stream_t* stream )
{
	if ( stream->file != INVALID_HANDLE_VALUE )
		CloseHandle( ( HANDLE )stream->file );

	if ( stream->fileName )
		free( stream->fileName );

	if ( stream->header )
		free( stream->header );

	if ( stream->channelHeaders )
		free( stream->channelHeaders );

	UTX_InitStream( stream );
}

//----------------------------------------------------------
void UTX_OptimizeReadOrder( UTX_stream_t* stream, unsigned int* packedTiles, unsigned int count )
{
	// verify that our buffer is large enough to hold the sort information.
	EnsureBuffer( count * 2 * sizeof( UTSortInfo ) );

	// first, pack all of our data into sort structures.
	UTSortInfo* sortInfo = ( UTSortInfo* )_buffer;
	UTSortInfo* sortInfoDst = sortInfo;
	for ( unsigned int i = 0; i < count; ++i, ++sortInfoDst )
	{
		// decode the packed tile info.
		int mip = packedTiles[ i ] & 0xFF;
		int x = ( packedTiles[ i ] >>  8 ) & 0xFFF;
		int y = ( packedTiles[ i ] >> 20 ) & 0xFFF;

		// store everything into the sortInfo structure.
		sortInfoDst->tile = packedTiles[ i ];
		sortInfoDst->addr = GetTileAddr( stream, mip, x, y );
	}

	// now sort.
	std::sort( sortInfo, sortInfo + count, UTSortInfoCompare );

	// now copy back to the original array.
	UTSortInfo* sortInfoSrc = sortInfo;
	unsigned int* packedTilesDst = packedTiles;
	for ( unsigned int i = 0; i < count; ++i, ++sortInfoSrc, ++packedTilesDst )
		*packedTilesDst = sortInfoSrc->tile;
}

//----------------------------------------------------------
bool UTX_ReadTile( UTX_stream_t* stream, void* vdest, int x, int y, int mipLevel, int* bytesRead )
{
	char* dest = ( char* )vdest;

	// seek to the tile in the texture.
	if ( !SeekToTile( stream, x, y, mipLevel ) )
		return false;

	// get the size of the tile.
	int tileSize = stream->header->tileSize;
	if ( bytesRead )
		*bytesRead += tileSize;

	// allocate temporary storage for the compressed data as well
	// as one stage of decompressed data.
	EnsureBuffer( tileSize + UTX_MAX_TILE_SIZE_IN_BYTES );

#if PROFILE_DISK_SPEED
	unsigned int readStartTime = timeGetTime();
#endif

	// TODO:  If vdest is null, route the read data to the encoder
	// thread.
	// read in compressed data.
	ReadStream( _buffer, stream, tileSize );

#if PROFILE_DISK_SPEED
	unsigned int readEndTime = timeGetTime();
	unsigned int readTotalTime = readEndTime - readStartTime;
	PrintF( "Tile disk access time: %d\n", readTotalTime );
#endif

#if 0
	MemCopy( dest, _buffer, tileSize );
#else
	// perform decompression on each channel and interleave texture
	// data.
	char* compressed = ( char* )_buffer;
	char* decompressed = ( char* )_buffer + tileSize;
	UTChannelHeader* channelHeader = 0;
	UTChannelFlags channelFlags = 0;
	int numStages = stream->header->numStages;
	for ( int s = 0; s < numStages; ++s )
	{

		// now decompress the texture data.
		char* dst = ( char* )decompressed;
		int numChannels = stream->stageHeaders[ s ].numChannels;
		for ( int i = 0; i < numChannels; ++i, dst += UTX_CHANNEL_SIZE_IN_BYTES )
		{
			// get the current channel's flags.
			channelHeader = stream->channelHeaders[ s ] + i;
			channelFlags = channelHeader->flags;

			// decompress the current channel if necessary.
			if ( channelFlags & UTX_CHANNELFLAGS_DCT )
			{
				// TODO: implement me!
				B_ASSERT( false );
			}
			else
			{
				// perform a raw copy of the current channel.
				MemCopy( dst, compressed, UTX_CHANNEL_SIZE_IN_BYTES );

				// advance the source data.
				compressed += UTX_CHANNEL_SIZE_IN_BYTES;
			}
		}

		// interleave data.
		char* src = decompressed;
		dst = ( char* )dest;
		for ( int c = 0; c < numChannels; ++c, src += UTX_CHANNEL_SIZE_IN_BYTES, ++dst )
			for ( int p = 0; p < UTX_CHANNEL_SIZE_IN_BYTES; ++p )
				dst[ p * numChannels ] = src[ p ];

		// advance the destination pointer.
		dest += numChannels * UTX_CHANNEL_SIZE_IN_BYTES;
	}
#endif

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTX_WriteTile( UTX_stream_t* stream, int x, int y, int mipLevel, const void* vsource, int* bytesWritten )
{
	assert( ( x << mipLevel ) < stream->widthInTiles );
	assert( ( y << mipLevel ) < stream->heightInTiles );

	// make sure the stream isn't compressed.
	int numStages = stream->header->numStages;

	// check all channels and all stages.
	for ( int i = 0; i < numStages; ++i )
	{
		int numChannels = stream->stageHeaders[ i ].numChannels;
		for ( int j = 0; j < numChannels; ++j )
		{
			if ( stream->channelHeaders[ i ][ j ].flags != 0 )
				return false;
		}
	}

	// seek to the tile in the texture.
	if ( !SeekToTile( stream, x, y, mipLevel ) )
		return false;
	if ( bytesWritten )
		*bytesWritten += stream->header->tileSize;

	// make sure the scratch buffer is large enough to do what we want.
	EnsureBuffer( UTX_MAX_CHANNELS * UTX_CHANNEL_SIZE_IN_BYTES );

	// de-interleave the data.
	char* source = ( char* )vsource;
	for ( int i = 0; i < numStages; ++i )
	{
		// get the current stage header.
		UTStageHeader* stageHeader = stream->stageHeaders + i;

		// now separate out the different channels.
		int numChannels = stageHeader->numChannels;
		char* src = ( char* )source;
		char* dst = ( char* )_buffer;
		for ( int c = 0; c < numChannels; ++c, dst += UTX_CHANNEL_SIZE_IN_BYTES, ++src )
		{
			for ( int p = 0; p < UTX_CHANNEL_SIZE_IN_BYTES; ++p )
				dst[ p ] = src[ p * numChannels ];
		}

		// write the data to the stream.
		WriteStream( stream, ( char* )_buffer, ( unsigned int )( dst - ( char* )_buffer ) );

		// advance the source pointer.
		source += numChannels * UTX_CHANNEL_SIZE_IN_BYTES;
	}

	// return true to indicate success.
	return true;
}


//**********************************************************
// File local functions
//**********************************************************

//----------------------------------------------------------
void CheckInit()
{
	if ( _utxInit )
		return;
	_utxInit = true;
	memset( _addrMaps, 0, sizeof( _addrMaps ) );
	atexit( OnExit );
}

//----------------------------------------------------------
void OnExit()
{
	// free address maps.
	for ( unsigned int i = 0; i < UTX_MAX_MIP_LEVEL_COUNT; ++i )
	{
		for ( unsigned int j = 0; j < UTX_MAX_MIP_LEVEL_COUNT; ++j )
		{
			if ( _addrMaps[ i ][ j ] != 0 )
				free( _addrMaps[ i ][ j ] );
		}
	}

	// free the buffer.
	AlignedFree( _buffer );
}

//----------------------------------------------------------
void SetStreamAddrMap( UTX_stream_t* stream )
{
	// calculate the address map to use.
	int i = ( int )Log2( ( unsigned int )stream->widthInTiles );
	int j = ( int )Log2( ( unsigned int )stream->heightInTiles );

	// do we need to generate the address map?
	if ( _addrMaps[ i ][ j ] == 0 )
	{
		// allocate space for the address map.
		unsigned int mipCount = stream->mipLevels;
		UTTileAddrMap* newAddrMaps = ( UTTileAddrMap* )malloc( mipCount * sizeof( UTTileAddrMap ) );
		_addrMaps[ i ][ j ] = newAddrMaps;

		// allocate the mip levels.
		int mipWidth = stream->widthInTiles;
		int mipHeight = stream->heightInTiles;
		for ( unsigned int i = 0; i < mipCount; ++i )
		{
			newAddrMaps[ i ].table = ( int* )malloc( mipWidth * mipHeight * sizeof( int ) );
			mipWidth >>= 1;
			mipHeight >>= 1;
		}

		// store the tile address maps.
		stream->tileAddrMaps = newAddrMaps;

		// now generate the address map.
		GenAddrMap( stream );
	}
	else
	{
		stream->tileAddrMaps = _addrMaps[ i ][ j ];
	}
}

//----------------------------------------------------------
int GetTileAddr( UTX_stream_t* stream, int mip, int x, int y )
{
    B_ASSERT( mip < stream->mipLevels );

	// due to the way this mapping is calculated, we need to swap
	// x and y to ensure correct mapping for non-square textures.
	int* addrMap = stream->tileAddrMaps[ mip ].table;

	// now calculate the lookup.
	int mipWidth = stream->widthInTiles >> mip;

	// return the tile's address.
	return addrMap[ y * mipWidth + x ];
}

//----------------------------------------------------------
void GenAddrMap( UTX_stream_t* stream )
{
	int offset = 1;
	int mip = stream->mipLevels - 1;
	BuildTileIndexMap( stream, offset, mip, 0, 0 );
}

//----------------------------------------------------------
void BuildTileIndexMap( UTX_stream_t* stream, int& offset, int mip, int x, int y )
{
	// store the current offset for the given tile.
	unsigned int mipWidth = stream->widthInTiles >> mip;
	stream->tileAddrMaps[ mip ].table[ y * mipWidth + x ] = offset;

	// advance the offset.
	++offset;

	// recurse if possible.
	if ( mip > 0 )
	{
		// calculate the new mip/tile info.
		unsigned int nx0 = x + x;
		unsigned int ny0 = y + y;
		unsigned int nx1 = nx0 + 1;
		unsigned int ny1 = ny0 + 1;
		unsigned int nm = mip - 1;

		// mipmap size.
		unsigned int width = Max( stream->widthInTiles >> nm, 1 );
		unsigned int height = Max( stream->heightInTiles >> nm, 1 );

		// recurse.
		BuildTileIndexMap( stream, offset, nm, nx0, ny0 );
		if ( nx1 < width )
			BuildTileIndexMap( stream, offset, nm, nx1, ny0 );
		if ( ny1 < height )
			BuildTileIndexMap( stream, offset, nm, nx0, ny1 );
		if ( nx1 < width && ny1 < height )
			BuildTileIndexMap( stream, offset, nm, nx1, ny1 );
	}
}

//----------------------------------------------------------
void EnsureBuffer( unsigned int size )
{
	// resize the buffer if needed.
	if ( size > _bufferSize )
	{
		// resize the current buffer.
		size += UT_ALIGNMENT;
		if ( _buffer )
		{
			AlignedFree( _buffer );
			_buffer = AlignedAlloc( UT_ALIGNMENT, size );
		}
		else
			_buffer = AlignedAlloc( UT_ALIGNMENT, size );

		// update the buffer size.
		_bufferSize = size;
	}
}

//----------------------------------------------------------
char* CopyStr( const char* src )
{
	size_t len = strlen( src );
	char* str = ( char* )malloc( len+1 );
	strcpy( str, src );
	return str;
}

//----------------------------------------------------------
int CalcMipCount( int width, int height )
{
	// calculate the total number of mip levels.
	int mipDim = Max( width, height );
	int mipLevels = 0;
	while ( mipDim > 0 )
	{
		++mipLevels;
		mipDim >>= 1;
	}
	return mipLevels;
}

//----------------------------------------------------------
int CalcRowOffsetIdx( UTX_stream_t* stream, int mip, int row )
{
	// calculate the row-offset's index.
	int heightInRows = stream->heightInTiles;
	int idx = row;
	while ( mip-- > 0 )
		idx += ( heightInRows >> mip );

	// return the index.
	return idx;
}

//----------------------------------------------------------
bool WriteStream( UTX_stream_t* stream, void* src, unsigned int bytes )
{
	// write the data to the file.
	DWORD bytesWritten = 0;
	BOOL ret = WriteFile( ( HANDLE )stream->file, src, bytes, &bytesWritten, NULL );
	// check the return value and make sure all of the bytes were written.
	if ( !ret || bytesWritten < bytes )
	{
		UTX_CloseStream( stream );
		return false;
	}

	// advance the stream offset.
	stream->offset += bytes;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool ReadStream( void* dst, UTX_stream_t* stream, unsigned int bytes )
{
	// write the data to the file.
	DWORD bytesRead = 0;
	BOOL ret = ReadFile( ( HANDLE )stream->file, dst, bytes, &bytesRead, NULL );

	// check the return value and make sure all of the bytes were written.
	if ( !ret || bytesRead < bytes )
	{
		UTX_CloseStream( stream );
		return false;
	}

	// advance the stream offset.
	stream->offset += bytes;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool SetStreamPos( UTX_stream_t* stream, __int64 offset )
{
	long highWord = ( long )( offset >> 32 );
	DWORD ret = SetFilePointer( ( HANDLE )stream->file, ( long )offset & 0xFFFFFFFF,
		&highWord, FILE_BEGIN );
	if ( ret == INVALID_SET_FILE_POINTER )
	{
		// do we actually have an error or is it BS?
		DWORD error = GetLastError();
		if ( error != NO_ERROR )
		{
			// return false to indicate failure.
			UTX_CloseStream( stream );
			return false;
		}
	}

	// convert back to the full size to make sure that the file pointer was
	// correctly set.
	__int64 newAddr = highWord;
	newAddr <<= 32;
	newAddr |= ret;

	// did the position change fail?
	if ( newAddr != offset )
	{
		// return false to indicate failure.
		UTX_CloseStream( stream );
		return false;
	}

	// set the offset to the stream.
	stream->offset = offset;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool SeekStream( UTX_stream_t* stream, __int64 offset )
{
	long highWord = ( long )( offset >> 32 );
	DWORD ret = SetFilePointer( ( HANDLE )stream->file, ( long )offset & 0xFFFFFFFF,
		&highWord, FILE_CURRENT );
	if ( ret == INVALID_SET_FILE_POINTER )
	{
		// do we actually have an error or is it BS?
		DWORD error = GetLastError();
		if ( error != NO_ERROR )
		{
			// return false to indicate failure.
			UTX_CloseStream( stream );
			return false;
		}
	}

	// convert back to the full size to make sure that the file pointer was
	// correctly set.
	__int64 newAddr = highWord;
	newAddr <<= 32;
	newAddr |= ret;

	// adjust the stream offset.
	stream->offset += offset;

	// did the position change fail?
	if ( stream->offset != newAddr )
	{
		// return false to indicate failure.
		UTX_CloseStream( stream );
		return false;
	}

	// return true to indicate success.
	return true;	
}

//----------------------------------------------------------
bool SeekToTile( UTX_stream_t* stream, int x, int y, int mip )
{
	// look up the tile's address.
	__int64 addr = GetTileAddr( stream, mip, x, y );

	// calculate the physical offset.
	__int64 offset = addr * stream->header->tileSize;

	// simply return if there is no need to do anything.
	if ( stream->offset == offset )
		return true;

#if PROFILE_DISK_SPEED
	PrintF( "Disk seek occured.\n" );
#endif

	// seek to the correct location in the file.
	return SetStreamPos( stream, offset );
}
/*
//----------------------------------------------------------
void Swizzle( unsigned char* stream, int channelCount )
{
	// are we already in BGRA?
	if ( channelCount == 4 )
		return;

	// swizzle a channel to BGRA.
	unsigned char* src = stream + channelCount * UTX_CHANNEL_SIZE_IN_BYTES - channelCount;
	unsigned char* dst = stream + UTX_MAX_CHANNELS * UTX_CHANNEL_SIZE_IN_BYTES - UTX_MAX_CHANNELS;

	// now swizzle.
	if ( channelCount == 3 )
	{
		do
		{
			// copy BGR, set alpha to 255.
			dst[ 0 ] = src[ 0 ];
			dst[ 1 ] = src[ 1 ];
			dst[ 2 ] = src[ 2 ];
			dst[ 3 ] = 255;

			// next.
			src -= 3;
			dst -= UTX_MAX_CHANNELS;
		}
		while ( dst != stream );
	}
	else if ( channelCount == 2 )
	{
		do
		{
			// copy LA.
			dst[ 0 ] = src[ 0 ];
			dst[ 1 ] = src[ 0 ];
			dst[ 2 ] = src[ 0 ];
			dst[ 3 ] = src[ 1 ];

			// next.
			src -= 2;
			dst -= UTX_MAX_CHANNELS;
		}
		while ( dst != stream );
	}
	else // channelCount == 1.
	{
		do
		{
			// copy I.
			dst[ 0 ] = src[ 0 ];
			dst[ 1 ] = src[ 0 ];
			dst[ 2 ] = src[ 0 ];
			dst[ 3 ] = src[ 0 ];

			// next.
			--src;
			dst -= UTX_MAX_CHANNELS;
		}
		while ( dst != stream );
	}
}

//----------------------------------------------------------
void Deswizzle( unsigned char* stream, int channelCount )
{
	// are we already in BGRA?
	if ( channelCount == 4 )
		return;

	// swizzle a channel to BGRA.
	unsigned char* src = stream;
	unsigned char* dst = stream;

	// now deswizzle.
	if ( channelCount == 3 )
	{
		// copy BGR, set alpha to 255.
		for ( int i = 0; i < UTX_CHANNEL_SIZE_IN_BYTES; ++i, dst += 3 )
		{
			dst[ 0 ] = src[ 4*i+0 ];
			dst[ 1 ] = src[ 4*i+1 ];
			dst[ 2 ] = src[ 4*i+2 ];
		}
	}
	else if ( channelCount == 2 )
	{
		// copy LA
		for ( int i = 0; i < UTX_CHANNEL_SIZE_IN_BYTES; ++i )
		{
			dst[ 2*i+0 ] = src[ 4*i+0 ];
			dst[ 2*i+1 ] = src[ 4*i+3 ];
		}
	}
	else // channelCount == 1.
	{
		// copy I.
		for ( int i = 0; i < UTX_CHANNEL_SIZE_IN_BYTES; ++i )
			dst[ i ] = src[ 4*i ];
	}
}
*/
//----------------------------------------------------------
unsigned int CompressTileData( UTX_stream_t* stream, void* src, float* qualityMatrix )
{
	// writes out compressed tile data to the end of the file stream.
	

	// return the number of bytes written.
	return 0;
}
