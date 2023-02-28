//----------------------------------------------------------
// File:        GrPixelBuffer.h
// Author:      Kevin Bray
// Created:     04-11-08
//
// Purpose:     To manage a pixel buffer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"
#include "GrTextureBase.h"

// forward declarations.
class GrTexture;
class GrTextureRect;

//**********************************************************
// class GrPixelBuffer
//**********************************************************
class GrPixelBuffer : public URefCount
{
public:
    // Note: these orderings are chosen for performance.  Typically,
    // graphics cards store their data in BGRA format, with each color
    // component comprising of a single byte.  While this is hidden
    // with textures, we denote this ordering explicitly here since direct
    // access to pixel buffer content is allowed.
    enum EFORMAT
    {
        EF_DXT1,
        EF_DXT5,
        EF_DXT5R,
        EF_BGR16,
        EF_BGR15_A1,
        EF_BGRX32,
        EF_BGRA32,
        EF_RGBX64F,
        EF_RGBA64F,
        EF_RGBA128F,
        EF_R8,
        EF_RG16,
        EF_L8,
        EF_I8,
        EF_A8,
        EF_LA16,
        EF_INVALID,
    };

    enum EUSAGE
    {
        // used to transfer data to the device.
        EU_STAGING,

        // used to transfer data to the device.  Data should be
        // updated all of the time.
        EU_STAGING_STREAM,

        // used to transfer data to the device.  Data will not be
        // preserved across context resets.
        EU_STAGING_DISCARD,

        // used to read data from the device.
        EU_READBACK,

        // used to read data from the device.  Data should be
        // read-back all of the time.
        EU_READBACK_STREAM,

        // used to read data from the device.  Data will not be
        // preserved across context resets.
        EU_READBACK_DISCARD,
    };

    // returns the matching format for a given texture format.  If no
    // matching format exists, EF_INVALID is returned.
    static EFORMAT          GetMatchingFormat( GrTextureBase::EFORMAT textureFormat );

    // note that the discardable flag indicates whether or not the
    // contents of the buffer must be preserved through a context
    // reset.
    GrPixelBuffer( unsigned int width, unsigned int height, EFORMAT format, EUSAGE usage );
    ~GrPixelBuffer();

    // accessors.
    unsigned int    GetWidth() const            {   return _width;      }
    unsigned int    GetHeight() const           {   return _height;     }
    EFORMAT         GetFormat() const           {   return _format;     }

    // reads pixels from the currently bound framebuffer.  Note that a
    // read must not go out of bounds from the current render target.
    void            ReadPixels( unsigned int srcX, unsigned int srcY ) const;
    void            UploadTexData( GrTexture* dst, unsigned int dstLevel, unsigned int dstX, unsigned int dstY ) const;
    void            UploadTexData( GrTextureRect* dst, unsigned int dstLevel, unsigned int dstX, unsigned int dstY ) const;

    // maps the data into the our address space.
    void*           Map();
    void*           Map( bool read, bool write );
    void            Unmap();

    // context reset notification.
    void            PreContextReset();
    void            PostContextReset();

private:
    void            AllocBuffer( void* data );

    unsigned int    _glHandle;
    unsigned int    _width;
    unsigned int    _height;
    unsigned int    _size;
    void*           _resetStorage;
    EFORMAT         _format;
    EUSAGE          _usage;
    bool            _mapped;
};
