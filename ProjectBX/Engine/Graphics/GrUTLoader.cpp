//----------------------------------------------------------
// File:        GrUTLoader.cpp
// Author:      Kevin Bray
// Created:     10-28-2008
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrUTLoader.h"

// project includes.
#include "GrUTCompress.h"
#include "GrUTAtlas.h"
#include "GrConfig.h"
#include "URLE.h"

// std c includes.
#include <process.h>
#include <stddef.h>

// useful constants.
#define UBERTEX_FILE_MAGIC              "\x90UTX\r\n\x1A\0"
#define UBERTEX_FILE_CHUNK_SIZE         2048
#define WORKER_QUEUE_SIZE               1024
#define MAX_THREADS                     32U
#define UBERTEX_USAGE_MASK_SIZE         ( ( GR_UBERTEX_MAX_SIZE_IN_TILES * GR_UBERTEX_MAX_SIZE_IN_TILES ) / 8 )
#define RAW_TILE_SIZE                   32768
#define RAW_ATLAS_SIZE                  524288
#define WRITE_BUFFER_COUNT              32


//==========================================================
// internal structures.
//==========================================================

// ubertexture header info.
#pragma pack( push )
#pragma pack( 1 )
struct SUTHeader
{
    // ubertexture data.
    unsigned char magic[ 8 ];       // must be '\0x90''U''T''X''\r''\n''\0x1A''\n'
    unsigned char minorVer;         // must be 0
    unsigned char majorVer;         // must be 1
    unsigned short metaDataStart;   // location of any extra data.
    unsigned short metaDataSize;    // size in bytes of any extra data.
    unsigned short atlasStart;      // location in the file where the atlas starts.
    unsigned short atlasSize;       // size of the atlas in 2K chunks.
    unsigned short chunkStart;      // the first 2K chunk containing tile data.
    unsigned int basicCheckSum;     // 32-bit checksum of 1.0 header data.
};
#pragma pack( pop )

struct SReadOp
{
    // flag to indicate whether or not the current entry is
    // being used.
    volatile int inUse;

    // File read buffer.
    SFSFileReadBuffer readBuf;

    // target location.
    void* dst;
    volatile int* lock;

    // tile information.
    void* file;
    unsigned int chunkIndex;

    // target format information.
    GrUTLoader::ECOLORMODE dxtMode;
};


//==========================================================
// Worker thread.
//==========================================================

static SReadOp _queue[ WORKER_QUEUE_SIZE ];
static volatile int _threadQuit = 0;
static void ThreadMain( void* );
static void ProcessTile( void* dest, UHuffman* decoder, GrUTLoader::ECOLORMODE dxtMode );
static uintptr_t _threadHandles[ MAX_THREADS ];
static unsigned int _threadCount = 0;


//==========================================================
// Internal tables declarations.
//==========================================================

//----------------------------------------------------------
// bit-lengths for entropy encoding.
static DECLARE_ALIGNED_16( const unsigned char, kCodeLengths[ 256 ] ) =
{
    3, 3, 4, 5, 6, 6, 7, 7, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 10,
    10, 10, 10, 10, 10, 9, 9, 9, 10, 9, 9, 9, 9, 9, 9, 8, 9, 9, 9, 9, 10, 9, 9, 9, 9, 9, 9, 9, 8, 9, 9, 9,
    10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 11, 10, 10, 10, 10, 11, 11, 10, 11, 11, 12, 11, 11, 11, 11, 11,
    10, 11, 11, 10, 10, 10, 10, 11, 12, 12, 13, 13, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    4, 3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 13,
    13, 11, 10, 9, 9, 8, 9, 9, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 7, 7, 7, 6, 5, 5, 3,
};

//----------------------------------------------------------
// tile count lookup table, maps atlas-tree depth to node count.
unsigned int kTotalTileCount[ 12 ] =
{
    5592405,
    1398101,
    349525,
    87381,
    21845,
    5461,
    1365,
    341,
    85,
    21,
    5,
    1,
};

//==========================================================
// Global variables.
//==========================================================

GrUTLoader* gGrUTLoader = 0;


//**********************************************************
// class GrUTLoader
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrUTLoader::GrUTLoader( unsigned int workerThreadCount )
{
    // set the singleton pointer.
    B_ASSERT( gGrUTLoader == 0 );
    gGrUTLoader = this;

    // initialize all queue entries.
    MemSet( _queue, 0, sizeof( _queue ) );

    // allocate memory for tile data.
    for ( unsigned int i = 0; i < WORKER_QUEUE_SIZE; ++i )
        _queue[ i ].readBuf.buffer = new unsigned char[ UBERTEX_FILE_CHUNK_SIZE ];

    // initialize the compression services.
    GrUTInitCompressor();

    // allocate write buffers.
    _writeBuffers = new SFSFileWriteBuffer[ WRITE_BUFFER_COUNT ];
    for ( unsigned int i = 0; i < WRITE_BUFFER_COUNT; ++i )
    {
        _writeBuffers[ i ].buffer = AlignedAlloc( 2048, 2048 );
        _writeBuffers[ i ].complete = 1;
    }

    // initialize the encoder and the decoder.
    _encoder.SetCompCodeLengths( kCodeLengths );

    // start the worker thread.
    _threadCount = Min( workerThreadCount, MAX_THREADS );
    for ( unsigned int i = 0; i < workerThreadCount; ++i )
        _threadHandles[ i ] = _beginthread( ThreadMain, 1024, 0 );

#if 0
    unsigned char compStream[ 2 ];
    for ( unsigned int i = 0; i < 256; ++i )
    {
        _entropyEncoder.SetCompStream( compStream, kCodeLengths );
        _entropyEncoder.WriteValue( i );
        _entropyEncoder.SetDecompStream( kCodeLengths, compStream, _entropyEncoder.FinishWriting() );
        if ( _entropyEncoder.ReadValue() != i )
            PrintF( "Entropy error on value %d.\n", i );
    }
    bool breakpt = false;
    breakpt = breakpt;
#endif
}

//----------------------------------------------------------
GrUTLoader::~GrUTLoader()
{
    // wait for everything to finish and free memory.
    for ( unsigned int i = 0; i < WORKER_QUEUE_SIZE; ++i )
    {
        // wait for the current entry to finish.
        while ( LockCompareAndSwap( &_queue[ i ].inUse, 0, 1 ) == 1 )
            LockPause();

        // free memory.
        delete[] _queue[ i ].readBuf.buffer;
    }

    // issue the stop command for the worker thread.
    for ( unsigned int i = 0; i < _threadCount; ++i )
    {
        // issue the quit command and wait until a thread consumes
        // it.  Once it's consumed, repeat until all threads have
        // processed the event.
        while ( LockSwap( &_threadQuit, LOCK_TAKEN ) == LOCK_TAKEN )
            LockPause();
    }

    // free write buffers.
    for ( unsigned int i = 0; i < WRITE_BUFFER_COUNT; ++i )
        AlignedFree( ( void* )_writeBuffers[ i ].buffer );
    delete[] _writeBuffers;

    // clear the singleton pointer.
    gGrUTLoader = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrUTAtlas*
GrUTLoader::CreateUbertex( void* file, const void* tileUseBitMask, const void* metaData,
                           unsigned int metaDataSize, ProgressCallback callback )
{
    // flush any pending read requests.
    Flush();

    // calculate the maximum amount of data that can be written by
    // RLE compression.
    URLE rleEncoder;
    unsigned int rleBufSize = rleEncoder.CalcMaxCompSize( UBERTEX_USAGE_MASK_SIZE );

    // allocate space for RLE compressed tile usage data and compress.
    unsigned char* atlasData = new unsigned char[ rleBufSize ];
    unsigned int atlasDataSize = rleEncoder.Compress( atlasData, ( const unsigned char* )tileUseBitMask, UBERTEX_USAGE_MASK_SIZE );

    // calculate data starting locations.
    unsigned int metaDataStart = sizeof( SUTHeader );
    unsigned int atlasDataStart = metaDataStart + metaDataSize;
    unsigned int chunkStart = ( atlasDataStart + atlasDataSize + 2047 ) / 2048;

    // allocate a temporary buffer for writing to the file.
    unsigned int dataSize = sizeof( SUTHeader ) + metaDataSize + atlasDataSize;
    unsigned int fileBufSize = ( ( dataSize + 2047 ) / 2048 ) * 2048;
    unsigned char* fileBuf = ( unsigned char* )AlignedAlloc( 2048, fileBufSize );

    // write out the header.
    SUTHeader* header = ( SUTHeader* )fileBuf;
    memcpy( header->magic, UBERTEX_FILE_MAGIC, 8 );
    header->minorVer = 0;
    header->majorVer = 1;
    header->metaDataStart = metaDataStart;
    header->metaDataSize = metaDataSize;
    header->atlasStart = atlasDataStart;
    header->atlasSize = atlasDataSize;
    header->chunkStart = chunkStart;
    header->basicCheckSum = HashMem32( header, offsetof( SUTHeader, basicCheckSum ) );

    // copy the meta data.
    if ( metaDataSize > 0 )
        MemCopy( fileBuf + metaDataStart, metaData, metaDataSize );

    // copy the atlas data.
    MemCopy( fileBuf + atlasDataStart, atlasData, atlasDataSize );

    // write any additional data.
    unsigned int paddingStart = atlasDataStart + atlasDataSize;
    unsigned int paddingSize = fileBufSize - paddingStart;
    MemSet( fileBuf + paddingStart, 0, paddingSize );

    // flush the header data to the disk.
    SFSFileWriteBuffer_t writeBuf;
    writeBuf.buffer = fileBuf;
    writeBuf.complete = 0;
    FS_WriteFile( file, 0, &writeBuf, fileBufSize );
    FS_Wait( &writeBuf.complete );

    // delete the buffer.
    AlignedFree( fileBuf );
    delete[] atlasData;

    // now build an atlas and fill out the chunks with default data.
    GrUTAtlas* atlas = new GrUTAtlas( ( unsigned int* )tileUseBitMask, chunkStart );

    // fill out a default tile data.
    unsigned char* tileData = new unsigned char [ RAW_TILE_SIZE ];
    MemSet( tileData, 0x7F, RAW_TILE_SIZE );

    // write out all chunks in the file.
    FillChunks( file, atlas, tileData );
    delete[] tileData;

    // return the atlas.
    return atlas;
}

//----------------------------------------------------------
GrUTAtlas*
GrUTLoader::LoadUbertex( void* file )
{
    // allocate some temporary space on the stack for the header.
    DECLARE_ALIGNED( unsigned char, headerBuf[ UBERTEX_FILE_CHUNK_SIZE ], UBERTEX_FILE_CHUNK_SIZE );

    // initialize an RLE decoder so that we can decode the atlas.
    URLE rleDecoder;

    // initial read buffer size.  First, we read in the header only.
    // If we determine that the meta data and the atlas data can fit
    // into the buffer, then we simply continue with our read.
    const unsigned int kInitReadBufSize = 65536;

    // read in the ubertexture header.
    SFSFileReadBuffer readBuf;
    readBuf.buffer = headerBuf;
    readBuf.complete = 0;
    FS_ReadFile( &readBuf, file, 0, UBERTEX_FILE_CHUNK_SIZE );
    FS_Wait( &readBuf.complete );

    // determine how big various tables and such are.  Free the
    // previous block of memory and allocate a larger block for
    // the meta data and atlas data.
    SUTHeader* header = ( SUTHeader* )headerBuf;

    // first, verify the checksum.
    unsigned int checkSum = HashMem32( header, offsetof( SUTHeader, basicCheckSum ) );
    if ( checkSum != header->basicCheckSum )
        return 0;

    // verify that the file magic is valid.  The odds of this
    // happening with the file checksum being valid is extremely
    // small.
    if ( memcmp( header->magic, UBERTEX_FILE_MAGIC, 8 ) != 0 )
        return 0;

    // verify the version is readable.
    if ( header->majorVer > 1 )
        return 0;

    // use the header data to figure out how many chunks we need to
    // read to get the rest of the header information, including the
    // meta data and the tile-use atlas.
    unsigned int fullHeaderSize = header->chunkStart * UBERTEX_FILE_CHUNK_SIZE;

    // allocate a buffer for the meta data and atlas data and copy
    // the header data to it.
    unsigned char* headerReadBuf = ( unsigned char* )AlignedAlloc( UBERTEX_FILE_CHUNK_SIZE, fullHeaderSize );
    MemCopy( headerReadBuf, headerBuf, UBERTEX_FILE_CHUNK_SIZE );

    // read in rest of the meta data and atlas data.
    readBuf.buffer = headerReadBuf + UBERTEX_FILE_CHUNK_SIZE;
    readBuf.complete = 0;
    FS_ReadFile( &readBuf, file, UBERTEX_FILE_CHUNK_SIZE, fullHeaderSize - UBERTEX_FILE_CHUNK_SIZE );
    FS_Wait( &readBuf.complete );

    // now decompress the atlas into the scratch memory buffer.
    void* atlasBitmap = TempAlloc( RAW_ATLAS_SIZE );
    rleDecoder.Decompress( ( unsigned char* )atlasBitmap, RAW_ATLAS_SIZE, headerReadBuf + header->atlasStart );

    // free our read buffer.
    AlignedFree( headerReadBuf );

    // now build an atlas out of the atlas bitmap.
    GrUTAtlas* atlas = new GrUTAtlas( ( unsigned int* )atlasBitmap, header->chunkStart );

    // free the scratch memory buffer.
    TempFree( atlasBitmap );

    // return the newly created atlas.
    return atlas;
}

//----------------------------------------------------------
void
GrUTLoader::ChangeLayout( void* file, const void* tileUseBitMask, ProgressCallback callback )
{
    // flush any pending read requests.
    Flush();

    // create a backup of the current file.
    

    //
    
}

//----------------------------------------------------------
void
GrUTLoader::FreeAtlas( GrUTAtlas* atlas )
{
    delete atlas;
}

//----------------------------------------------------------
void
GrUTLoader::DecodeChunk( void* dst, volatile int* lock, void* file, unsigned int chunkIndex, ECOLORMODE dxtMode )
{
    // find an empty slot in the queue.
    for ( unsigned int i = 0; i < WORKER_QUEUE_SIZE; ++i )
    {
        // try to gain ownership over the entry.
        if ( !TryTakeLock( &_queue[ i ].inUse ) )
            continue;

        // set the lock flag to LOCK_TAKEN
        TakeLockWait( lock );

        // fill out the current queue entry.
        _queue[ i ].dst = dst;
        _queue[ i ].lock = lock;

        // fill out file and tile information.
        _queue[ i ].file = file;
        _queue[ i ].chunkIndex = chunkIndex;

        // look-up the location in the file.  Note that we add one to
        // advance past the header.
        FSsize offset = chunkIndex * UBERTEX_FILE_CHUNK_SIZE;

        // issue the read request.  The worker thread will detect
        // when data is ready and will begin processing tile data as
        // soon as possible.
        FS_ReadFile( &_queue[ i ].readBuf, file, offset, UBERTEX_FILE_CHUNK_SIZE );
    }
}

//----------------------------------------------------------
void
GrUTLoader::WriteChunk( void* file, unsigned int chunkIndex, const void* src )
{
    // calculate tile data starting addresses.
    unsigned char* bump = ( ( unsigned char* )src ) + 0;
    unsigned char* diffuse = ( ( unsigned char* )src ) + 4096;
    unsigned char* specular = ( ( unsigned char* )src ) + 20480;

    // wait for a free write slot.
    unsigned int slot = 0;
    while( true )
    {
        // check to see if the current slot is ready.  If so, stop
        // spinning.
        if ( FS_IsComplete( &_writeBuffers[ slot ].complete ) )
            break;

        // advance to the next slot.
        slot = ( slot + 1 ) % WRITE_BUFFER_COUNT;
    }

    // each tile is 2048 bytes on disk.  This means that there are
    // a maximum of 8 * 2048 bits that can be written.
    const unsigned int kMaxBitCount = 16384;

    // compress the data.
    unsigned char chop = 0;
    unsigned int bitCount = 0;
    do
    {
        // encode data into the write buffer.
        _encoder.SetCompStreamFast( ( unsigned char* )_writeBuffers[ slot ].buffer );

        // compress the data into the encoder.
        GrUTCompressA( &_encoder, bump, 1, chop );
        GrUTCompressBGRX( &_encoder, diffuse, chop );
        GrUTCompressA( &_encoder, diffuse + 3, 4, chop );
        GrUTCompressBGRX( &_encoder, specular, chop );

        // finish writing the data and calculate the total size of
        // the data.
        bitCount = _encoder.FinishWriting();

        // increment chop in the case that we need to recompress the
        // data at a higher compression ratio.
        ++chop;
    } while( bitCount > kMaxBitCount );

    // write the data in the write buffer.
    FS_WriteFile( file, chunkIndex * 2048, _writeBuffers + slot, 2048 );
}

//----------------------------------------------------------
void
GrUTLoader::Flush()
{
    // wait for all commands to be completed.
    for ( unsigned int i = 0; i < WORKER_QUEUE_SIZE; ++i )
        WaitLockFree( &_queue[ i ].inUse );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrUTLoader::FillChunks( void* file, GrUTAtlas* atlas, const void* tileData, unsigned int depth )
{
    // if we're empty, then there isn't anything to do.
    if ( atlas->IsEmpty() )
        return;

    // if we're a leaf, then we need to fill all of the chunks that
    // this leaf encompasses.  This can be figured out by the depth
    // of the node.
    if ( atlas->IsLeaf() )
    {
        // write all chunks represented by the current leaf.
        unsigned int chunkIndex = atlas->GetChunk();
        unsigned int chunkEnd = chunkIndex + kTotalTileCount[ depth ];
        for ( ; chunkIndex != chunkEnd; ++chunkIndex )
            WriteChunk( file, chunkIndex, tileData );
    }
    else
    {
        // write the current mip-level.
        unsigned int chunkIndex = atlas->GetChunk();
        WriteChunk( file, chunkIndex, tileData );

        // recurse if necessary.
        for ( unsigned int i = 0; i < 4; ++i )
            FillChunks( file, atlas->GetChild( i ), tileData, depth + 1 );      
    }
}


//**********************************************************
// File global functions
//**********************************************************

//----------------------------------------------------------
void ThreadMain( void* )
{
    // initialize our decoder.
    UHuffman decoder;
    decoder.SetDecompCodeLengths( kCodeLengths );

    // scan through the queue looking for work to do.
    while ( LockSwap( &_threadQuit, LOCK_FREE ) == LOCK_FREE )
    {
        // iterate through the worker queue and look for data that's
        // ready for processing.
        for ( unsigned int i = 0; i < WORKER_QUEUE_SIZE; ++i )
        {
            // simply ignore the current item if it isn't ready.
            if ( !FS_IsComplete( &_queue[ i ].readBuf.complete ) )
                continue;

            // process the tile's data.
            decoder.SetDecompStreamFast( ( unsigned char* )_queue[ i ].readBuf.buffer, 8 * 2048 );

            // iterate over blocks.  Decode DCT and encode to DXT if
            // necessary.
            ProcessTile( _queue[ i ].dst, &decoder, _queue[ i ].dxtMode );
    
            // mark the operation as complete.
            FreeLock( _queue[ i ].lock );
            FreeLock( &_queue[ i ].inUse );
        }
    }
}

//----------------------------------------------------------
void ProcessTile( void* dest, UHuffman* decoder, GrUTLoader::ECOLORMODE dxtMode )
{
    // calculate tile data starting addresses.
    unsigned char* bump = ( ( unsigned char* )dest ) + 0;
    unsigned char* diffuse = ( ( unsigned char* )dest ) + 4096;
    unsigned char* specular = ( ( unsigned char* )dest ) + 20480;

    // decompress the data.
    GrUTDecompressA( bump, 1, decoder );
    GrUTDecompressBGRX( diffuse, decoder );
    GrUTDecompressA( diffuse + 3, 4, decoder );
    GrUTDecompressBGRX( specular, decoder );
}


#if 0
#include "GrImage.h"
#include "GrDebug.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "UReader.h"
//----------------------------------------------------------
void
GrUTLoader::Test( const char** images, unsigned int count )
{
#if 1
    // compress the images with the existing matrices to get error
    // metrics.
    for ( unsigned int i = 0; i < count; ++i )
    {
        PrintF( "Compressing image %s... ", images[ i ] );
        float mse[ 3 ] = { 0.0f, 0.0f, 0.0f };
        float psnr[ 3 ] = { 0.0f, 0.0f, 0.0f };
        CalcImageError( psnr, mse, images[ i ], kQuantY, kQuantCo, kQuantCg, true );
        PrintF( "[done]\n" );
    }
#endif

#if 0
    // Quantization matrix generation code.

    // quantization matrix.
    DECLARE_ALIGNED_16( unsigned short, yQuantMat[ 65 ] );
    DECLARE_ALIGNED_16( unsigned short, coQuantMat[ 65 ] );
    DECLARE_ALIGNED_16( unsigned short, cgQuantMat[ 65 ] );

    // clear the quantization matrix to 16.
    for ( unsigned int j = 0; j < 65; ++j )
    {
        yQuantMat[ j ] = 16;
        coQuantMat[ j ] = 16;
        cgQuantMat[ j ] = 16;
    }

    float avgMSE[ 65 ][ 3 ];
    float avgPSNR[ 65 ][ 3 ];
    MemSet( avgMSE, 0, sizeof( avgMSE ) );
    MemSet( avgPSNR, 0, sizeof( avgPSNR ) );

    // iterate through all of the files passed in.
    const char* kComponentLUT[ 3 ] = { "Y\n", "Co\n", "Cg\n" };
    const char* kComponentLUT2[ 3 ] = { "Y", "Co", "Cg" };
    unsigned short* quantTables[ 3 ] = { yQuantMat, coQuantMat, cgQuantMat };
    for ( unsigned int file = 0; file < count; ++file )
    {
        for ( unsigned int c = 0; c < 3; ++c )
        {
            PrintF( "Calculating %s quantization data for image %s... [", kComponentLUT2[ c ], images[ file ] );
            unsigned short* quantMat = quantTables[ c ];
            for ( unsigned int i = 0; i < 65; ++i )
            {
                // overwrite the current component we want to test.
                quantMat[ i ] = 128;

                // add a star for our progress meter...
                if ( ( i & 15 ) == 0 && i > 0 )
                    PrintF( "*" );

                // calculate the error for the current image.
                float mse[ 3 ] = { 0.0f, 0.0f, 0.0f };
                float psnr[ 3 ] = { 0.0f, 0.0f, 0.0f };
                CalcImageError( psnr, mse, images[ file ], yQuantMat, coQuantMat, cgQuantMat, false );

                // store the average MSE.
                avgMSE[ i ][ c ] += mse[ 0 ];
                avgMSE[ i ][ c ] += mse[ 1 ];
                avgMSE[ i ][ c ] += mse[ 2 ];
                avgPSNR[ i ][ c ] += psnr[ 0 ];
                avgPSNR[ i ][ c ] += psnr[ 1 ];
                avgPSNR[ i ][ c ] += psnr[ 2 ];

                // restore the component we tested.
                quantMat[ i ] = 16;
            }
            PrintF( "]\n" );
        }
    }

    // we need this to take averages.
    float invCount = 1.0f / ( 3 * count );
    for ( unsigned int i = 0; i < 65; ++i )
    {
        avgMSE[ i ][ 0 ] *= invCount;
        avgMSE[ i ][ 1 ] *= invCount;
        avgMSE[ i ][ 2 ] *= invCount;
        avgPSNR[ i ][ 0 ] *= invCount;
        avgPSNR[ i ][ 1 ] *= invCount;
        avgPSNR[ i ][ 2 ] *= invCount;
    }

    // write out PSNR scores based on the quantization info.
    tstring quantInfo;
    quantInfo += "PSNR Info:\n";
    for ( unsigned int c = 0; c < 3; ++c )
    {
        quantInfo += kComponentLUT[ c ];
        for ( unsigned int i = 0; i < 65; ++i )
        {
            float psnrDif = avgPSNR[ i ][ c ]; // - avgPSNR[ 64 ][ c ];
            quantInfo << psnrDif;

            unsigned int eol = ( i & 7 );
            if ( eol == 7 )
                quantInfo += ",\n";
            else
                quantInfo += ", ";
        }
        quantInfo += "\n";
    }

    // write out MSE scores based on the quantization info.
    quantInfo += "\n";
    quantInfo += "MSE Info:\n";
    for ( unsigned int c = 0; c < 3; ++c )
    {
        quantInfo += kComponentLUT[ c ];
        for ( unsigned int i = 0; i < 65; ++i )
        {
            float mseDif = avgMSE[ i ][ c ]; // - avgMSE[ 64 ][ c ];
            quantInfo << mseDif;

            unsigned int eol = ( i & 7 );
            if ( eol == 7 )
                quantInfo += ",\n";
            else
                quantInfo += ", ";
        }
        quantInfo += "\n";
    }

    quantInfo += "\nRecommended Quantization Coefficients:\n\n";

    // calculate coefficients based on each score.
    for ( unsigned int c = 0; c < 3; ++c )
    {
        // write out the current component's name.
        quantInfo += kComponentLUT[ c ];

        // get the minimum component.
        float minComponent = FLT_MAX;
        float maxComponent = 0.0f;
        for ( unsigned int i = 1; i < 65; ++i )
        {
            minComponent = Min( avgMSE[ i ][ c ], minComponent );
            maxComponent = Max( avgMSE[ i ][ c ], maxComponent );
        }

        // the DC coefficient should be 16.
        quantInfo << 8;
        quantInfo += ", ";

        // get calculate 1/range.
        float invRange = 1.0f / ( maxComponent - minComponent );
        for ( unsigned int i = 1; i < 64; ++i )
        {
            // calculate the current coefficient.
            float t = invRange * ( avgMSE[ i ][ c ] - minComponent );
            float coeff = Lerp( 128.0f, 8.0f, Sqrt( t ) );

            // write out the current coefficient.
            quantInfo << ( int )coeff;
            unsigned int eol = ( i & 7 );
            if ( eol == 7 )
                quantInfo += ",\n";
            else
                quantInfo += ", ";
        }
        quantInfo += "\n";
    }

    // dump the data to a file.
    URef< RFile > file = gRFileMgr->GetFile( "quant.txt", RFileMgr::kFlagCreate | RFileMgr::kFlagWrite );
    file->WriteData( 0, quantInfo.c_str(), ( unsigned int )quantInfo.length() );
#endif
}

//----------------------------------------------------------
void GrUTLoader::CalcImageError( float* psnr, float* mse, const char* fileName, const unsigned short* yQuant,
                                 const unsigned short* coQuant, const unsigned short* cbQuant, bool saveResult )
{
    // temporary buffers to hold BGRA data.
    DECLARE_ALIGNED_16( unsigned char, bgra[ 256 ] );

    // temporary buffers to hold YCoCg data.
    DECLARE_ALIGNED_16( short, Y0[ 64 ] );
    DECLARE_ALIGNED_16( short, Y1[ 64 ] );
    DECLARE_ALIGNED_16( short, Y2[ 64 ] );
    DECLARE_ALIGNED_16( short, Y3[ 64 ] );
    DECLARE_ALIGNED_16( short, Co[ 64 ] );
    DECLARE_ALIGNED_16( short, Cg[ 64 ] );

    // temporary buffers to hold compressed data.
    DECLARE_ALIGNED_16( unsigned char, cmpCo[ 64 ] );
    DECLARE_ALIGNED_16( unsigned char, cmpCg[ 64 ] );
    DECLARE_ALIGNED_16( unsigned char, cmpY0[ 64 ] );
    DECLARE_ALIGNED_16( unsigned char, cmpY1[ 64 ] );
    DECLARE_ALIGNED_16( unsigned char, cmpY2[ 64 ] );
    DECLARE_ALIGNED_16( unsigned char, cmpY3[ 64 ] );

    // load an image and create a backup copy.
    URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagRead );
    UReader reader( file->GetData(), ( unsigned long )file->GetSize(), false );
    GrImage image( fileName, reader );
    reader.Reset();
    GrImage backup( fileName, reader );

    // get information about the current image.
    void* data = image.GetImageData();
    unsigned int width = image.GetWidth();
    unsigned int height = image.GetHeight();
    unsigned int maxSizeInBytes = 0;
    unsigned int totalSizeInBytes = 0;

    // iterate through lots of different quantization matrices to
    // determine which components affect the end result, and by how
    // much.

    // copy the data into chunks, compress, then decompress.
    unsigned int bitCount = 0;
    unsigned int bytesPerPel = image.GetBytesPerPixel();
    for ( unsigned int y = 0; y < height; y += 16 )
    {
        for ( unsigned int x = 0; x < width; x += 16 )
        {
            // extract four blocks and convert to YCoCg.
            ExtractBlockBGR( bgra, ( unsigned char* )data, width, bytesPerPel, x + 0, y + 0 );
            PackYCoCg( Y0, Co +  0, Cg +  0, bgra );

            ExtractBlockBGR( bgra, ( unsigned char* )data, width, bytesPerPel, x + 8, y + 0 );
            PackYCoCg( Y1, Co +  4, Cg +  4, bgra );

            ExtractBlockBGR( bgra, ( unsigned char* )data, width, bytesPerPel, x + 0, y + 8 );
            PackYCoCg( Y2, Co + 32, Cg + 32, bgra );

            ExtractBlockBGR( bgra, ( unsigned char* )data, width, bytesPerPel, x + 8, y + 8 );
            PackYCoCg( Y3, Co + 36, Cg + 36, bgra );

            _entropyEncoder.SetCompStream( cmpCo, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Co, coQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpCo, bitCount );
            DecodeBlock( Co, &_entropyEncoder, coQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            _entropyEncoder.SetCompStream( cmpCg, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Cg, cbQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpCg, bitCount );
            DecodeBlock( Cg, &_entropyEncoder, cbQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            _entropyEncoder.SetCompStream( cmpY0, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Y0, yQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpY0, bitCount );
            DecodeBlock( Y0, &_entropyEncoder, yQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            _entropyEncoder.SetCompStream( cmpY1, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Y1, yQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpY1, bitCount );
            DecodeBlock( Y1, &_entropyEncoder, yQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            _entropyEncoder.SetCompStream( cmpY2, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Y2, yQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpY2, bitCount );
            DecodeBlock( Y2, &_entropyEncoder, yQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            _entropyEncoder.SetCompStream( cmpY3, kCodeLengths );
            bitCount = EncodeBlock( &_entropyEncoder, Y3, yQuant, 0 );
            _entropyEncoder.SetDecompStream( kCodeLengths, cmpY3, bitCount );
            DecodeBlock( Y3, &_entropyEncoder, yQuant );
            maxSizeInBytes = Max( maxSizeInBytes, ( bitCount + 7 ) / 8 );
            totalSizeInBytes += ( bitCount + 7 ) / 8;

            // convert YCoCg to BGR and insert four blocks.
            UnpackYCoCg( bgra, Y0, Co +  0, Cg +  0 );
            InsertBlockBGR( ( unsigned char* )data, width, bytesPerPel, x + 0, y + 0, bgra );

            UnpackYCoCg( bgra, Y1, Co +  4, Cg +  4 );
            InsertBlockBGR( ( unsigned char* )data, width, bytesPerPel, x + 8, y + 0, bgra );

            UnpackYCoCg( bgra, Y2, Co + 32, Cg + 32 );
            InsertBlockBGR( ( unsigned char* )data, width, bytesPerPel, x + 0, y + 8, bgra );

            UnpackYCoCg( bgra, Y3, Co + 36, Cg + 36 );
            InsertBlockBGR( ( unsigned char* )data, width, bytesPerPel, x + 8, y + 8, bgra );
        }
    }

    // calculate the peak signal to noise ratio.
    float meanSqrError = 0.0f;
    unsigned char* lossy = ( unsigned char* )image.GetImageData();
    unsigned char* src = ( unsigned char* )backup.GetImageData();

    // verify the source data against the destination data.
    // calculate the mean squared error.
    double localMse[ 3 ] = { 0.0, 0.0, 0.0 };
    double invPelCount = 1.0 / ( width * height );
    for ( unsigned int y = 0; y < height; ++y )
        for ( unsigned int x = 0; x < width; ++x, lossy += bytesPerPel, src += bytesPerPel )
            for ( unsigned int c = 0; c < 3; ++c )
                localMse[ c ] += invPelCount * Sqr( ( double )lossy[ c ] - ( double )src[ c ] );

    // copy the mse.
    mse[ 0 ] = ( float )localMse[ 0 ];
    mse[ 1 ] = ( float )localMse[ 1 ];
    mse[ 2 ] = ( float )localMse[ 2 ];

    // calculate the peak signal to noise ratio.
    psnr[ 0 ] = psnr[ 1 ] = psnr[ 2 ] = 0;
    for ( unsigned int c = 0; c < 3; ++c )
        psnr[ c ] = 20.0f * Log( 255.0f / Sqrt( ( float )mse[ c ] ) );

#if 1
    PrintF( "Compression statistics for %s...\n", fileName );
    PrintF( " PSNR (BGR): %f, %f, %f\n", psnr[ 0 ], psnr[ 1 ], psnr[ 2 ] );

    PrintF( " Total image size in bytes: %d\n", totalSizeInBytes );
    PrintF( " Max block size in bytes: %d\n", maxSizeInBytes );
#endif

    if ( saveResult )
    {
        // save a targa so that we can visualize the result.
        // build a file name.
        char prefix[] = "cmp_";
        unsigned int fileNameMemSize = ( unsigned int )strlen( fileName ) + 1;
        char* tempBuf = new char[ sizeof( prefix ) + fileNameMemSize ];
        MemCopy( tempBuf, prefix, 4 );
        MemCopy( tempBuf + 4, fileName, fileNameMemSize );

        // store the result.
        GrSaveTGA( tempBuf, data, width, height, bytesPerPel );

        // free the file name.
        delete[] tempBuf;
    }
}
#endif
