//----------------------------------------------------------
// File:        UTConfig.h
// Author:      Kevin Bray
// Created:     11-24-07
//
// Purpose:     To provide configuration details for the client/server.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//==========================================================
// Enumerations
//==========================================================

// supported color transforms.  Note that the enums are named
// as ECT_source_target.  Also, the transforms require that the
// source color format have at least the required components.
enum ECOLORTRANSFORM
{
    ECT_NONE,
    ECT_BGR_YCoCg,
    ECT_BGRA_YCoCgA,

    // this is an experimental format that encodes an exponent
    // and 2 6-bit XY values, with 1 bit implied.  Format is:
    // E:2, X:s1m5, Y:s1m5.  E is implicitly negative and biased
    // by -1, meaning the largest exponent value is -1, with the
    // smallest being -4.  The largest absolute value that can
    // be represented by either X or Y is +/-0.984375f, while
    // the smallest absolute value is 0.001953125.  If m == 0,
    // and s == 0, then the value is treated as 0.
    // This is not currently supported, but might be in the
    // future if some compression strategies turn out to be a
    // win in this format.
    ECT_XY_EXY,
};


//==========================================================
// Server compile options
//==========================================================

#define UT_MAX_MESSAGE_SIZE         131072
#define UT_HANDSHAKE_CONNECT_LOCAL  "LOCALCX"
#define UT_HANDSHAKE_CONNECT_REMOTE "CONNECT"
#define UT_HANDSHAKE_ACCEPT         "ACCEPT"
#define UT_CLIENT_MUTEX_NAME        "UTXCLTMUTEX"
#define UT_SERVER_MUTEX_NAME        "UTXSRVMUTEX"
#define UT_HANDSHAKE_CONNECT_SIZE   8
#define UT_HANDSHAKE_ACCEPT_SIZE    7
#define UT_HANDSHAKE_BUFFER_SIZE    256
#define UT_CONNECTION_MAX           64
#define UT_UPDATE_INTERVAL          0
#define UT_STREAMS_MAX              1024
#define UT_MULTI_READ_MAX           1024
#define UT_LOCAL_MEMORY_SIZE        1048576
#define UT_LOCAL_MEMORY_NAME        "Local\\ServerBuffer"
#define UT_LOCAL_MEMORY_NAME_SIZE   19
#define UT_PROFILE_CONNECTION       1
#define UT_ALLOW_LOCAL              1
#define UT_MAX_TILE_READ_COUNT      2048
#define UT_MAX_TILE_WRITE_COUNT     2048


//==========================================================
// Ubertexture compile options
//==========================================================

#define UTX_MAGIC               'aXTU'
#define UTX_FILE_VERSION        0x0100
#define UTX_MAX_SIZE            131072
#define UTX_TILE_SIZE           64
#define UTX_MAX_MIP_LEVEL_COUNT 11
#define UTX_CHANNEL_SIZE_IN_BYTES   ( UTX_TILE_SIZE * UTX_TILE_SIZE )
#define UTX_MAX_TILE_SIZE_IN_BYTES  ( UTX_MAX_CHANNELS * UTX_CHANNEL_SIZE_IN_BYTES )
#define UTX_MAX_LAYER_COUNT     8
#define UTX_MAX_CHANNELS        4
#define UTX_DCT_BLOCK_SIZE      8
#define UTX_DCT_QUALITY         90
#define UTX_TILE_CACHE_SIZE     0x10000
#define UTX_TILE_CACHE_MASK     0x0FFFF
