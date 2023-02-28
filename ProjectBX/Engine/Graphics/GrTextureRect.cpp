//----------------------------------------------------------
// File:        GrTextureRect.cpp
// Author:      Kevin Bray
// Created:     04-12-06
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTextureRect.h"

// project includes.
#include "GLSubsys.h"

const GLenum _internalFormatMap[ GrTextureBase::EF_COUNT ] = {
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // EF_DXT1
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EF_DXT5
    GL_COMPRESSED_RED_RGTC1,            // EF_DXT5R
    GL_RGB5,                            // EF_RGB16
    GL_RGB8,                            // EF_RGB24
    GL_RGB5_A1,                         // EF_RGBA16
    GL_RGBA8,                           // EF_RGBA32
    GL_RGB16F_ARB,                      // EF_RGB48F
    GL_RGBA16F_ARB,                     // EF_RGBA64F
    GL_RGBA32F_ARB,                     // EF_RGBA128F
    GL_R8,                              // EF_R8
    GL_RG8,                             // EF_RG16
    GL_LUMINANCE8,                      // EF_L8
    GL_INTENSITY8,                      // EF_I8, cannot compress
    GL_INTENSITY16F_ARB,                // EF_I16F, cannot compress
    GL_ALPHA8,                          // EF_A8
    GL_LUMINANCE_ALPHA,                 // EF_LA16
    GL_LUMINANCE_ALPHA16F_ARB,          // EF_LA32F
    GL_RGBA8,                           // EF_NORMALMAP
    GL_RGBA16F_ARB,                     // EF_HIGHDEFNORMALMAP
    GL_DEPTH_COMPONENT,                 // EF_DEPTH
    GL_DEPTH_STENCIL_EXT,               // EF_DEPTHSTENCIL
    GL_RGBA8,                           // EF_GUESS
};


//**********************************************************
// class GrTextureRect
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureRect::GrTextureRect( const UPath& name, EFORMAT format, unsigned int width, unsigned int height )
: GrTextureBase( name, GL_TEXTURE_RECTANGLE_ARB, format, kNoCompress | kNoMipMap, ETT_RECT )
, _width( width )
, _height( height )
{
    // create the texture object for the handle we've allocated.
    Create();
}

//----------------------------------------------------------
GrTextureRect::~GrTextureRect()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
GrTextureRect::BindToFramebuffer( unsigned int attachment )
{
    CHECK_GL();

    B_ASSERT( GetTexHandle() != 0 );
    bglFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_RECTANGLE_ARB, GetTexHandle(), 0 );
    CHECK_GL();
}

//----------------------------------------------------------
void
GrTextureRect::UploadPixelData( int dstX, int dstY, int width, int height, const void* srcPels,
                                GLenum srcFormat, GLenum srcType )
{
    // currently, we do not support compressed textures!  In the future,
    // we need to either add another function to upload compressed data
    // or compress the incoming data automatically on the fly (or both?).
    B_ASSERT( GetFormat() != EF_DXT1 && GetFormat() != EF_DXT5 );
    B_ASSERT( GetGLFormat( 0 ) != GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
    B_ASSERT( GetGLFormat( 0 ) != GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );

    // bind to a texture unit and upload data.
    Bind( 0 );
    bglTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, dstX, dstY, width, height, srcFormat, srcType, srcPels );
    CHECK_GL();
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
void
GrTextureRect::UploadImage( bool mipmap )
{
    // specify the size and format of the texture.
    unsigned int dataFmt = ( ( GetFormat() == EF_DEPTH || GetFormat() == EF_DEPTHSTENCIL ) ? GL_DEPTH_COMPONENT : GL_RGBA );
    bglTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, _internalFormatMap[ GetFormat() ], _width, _height, 0, dataFmt, GL_BYTE, 0 );
    CHECK_GL();
}
