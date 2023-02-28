//----------------------------------------------------------
// File:        GrTexture.h
// Author:      Kevin Bray
// Created:     03-28-05
//
// Purpose:     To maintain an OpenGL texture object.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTextureBase.h"
#include "GrConfig.h"
#include "UPath.h"
#include "GLSubsys.h"

// forward declarations.
class GrImage;

//**********************************************************
// class GrTexture
//**********************************************************
class GrTexture : public GrTextureBase
{
public:
    // this allocates an empty texture.  This is generally used with render-to-texture.
    GrTexture( const UPath& name, EFORMAT format, unsigned int width, unsigned int height,
               unsigned int flags );

    // this builds a texture from a file.  If cacheImage is true, then the image will be
    // kept around for the life of the texture.
    GrTexture( const UPath& name, const UPath& rsrcName, EFORMAT format, unsigned int flags, bool cacheImage );

    // this builds a texture from the image passed in.  The texture object will own this
    // image.
    GrTexture( const UPath& name, GrImage* image, EFORMAT format, unsigned int flags );

    // dtor
    ~GrTexture();

    // misc info.
    bool                FromFile() const                                    {   return _fromFile;           }
    const UPath&        GetResourceName() const                             {   return _rsrcPath;           }

    // returns the owned image.  Note that this is NULL if the texture came from a file and
    // the texture is not told to keep the image around.  It will also return NULL if the
    // texture is a render target.
    GrImage*            GetOwnedImage() const                               {   return _ownedImage;         }

    // this should only be called if the texture owns an image, and that owned image was
    // changed.
    void                OwnedImageChanged();

    // size info.
    unsigned int        GetWidth() const                                    {   return _width;              }
    unsigned int        GetHeight() const                                   {   return _height;             }

    // turns on hardware shadow mode.  This is only valid for depth/depth-stencil textures.
    void                SetHWShadowEnable( bool enable );

    // binds to the actively selected framebuffer at the attachment passed in.
    void                BindToFramebuffer( unsigned int attachment );

    // direct upload of pixel data.
    void                UploadPixelData( int dstLevel, int dstX, int dstY, int width, int height, const void* srcPels,
                                         GLenum srcFormat, GLenum srcType );

    void                UploadPixelDataCompressed( int dstLevel, int dstX, int dstY, int width, int height, const void* srcPels,
                                                   GLenum srcFormat, unsigned int srcDataSize );

    // dumps the texture data to a file.
    void                DebugSave( const char* fileName );

private:
    void                UploadImage( bool mipmap );
    GrImage*            LoadImg();      // note that LoadImage is kindly taken by the platform SDK.  M$ fuckheads and their macros.

    unsigned int        _width;
    unsigned int        _height;
    GrImage*            _ownedImage;    // non-null if we own an image.
    UPath               _rsrcPath;
    bool                _hwShadowEnable;
    bool                _fromFile;
    bool                _needSwizzle;
    bool                _valid;
};
