//----------------------------------------------------------
// File:        GrTexture3D.cpp
// Author:      Kevin Bray
// Created:     12-03-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTexture3D.h"

// project includes.
#include "GLSubsys.h"


// data must be an unsigned byte sequence with pixel data arranged in the
// appropriate order according to this table.  The left most component in
// the name comes first.  For instance: GL_RGB = red, green, blue where red
// is first in the sequence.  The texture manager generates a noise texture
// and should provide a good example of usage as well.
static GLenum _dataFormats[] = {
    GL_RGB,                 // EF_RGB16             - RGB
    GL_RGB,                 // EF_RGB24             - RGB
    GL_RGBA,                // EF_RGBA16            - RGBA
    GL_RGBA,                // EF_RGBA32            - RGBA
    GL_RGBA,                // EF_RGB48F            - RGB
    GL_RGBA,                // EF_RGBA64F           - RGBA
    GL_LUMINANCE,           // EF_L8                - L
    GL_INTENSITY,           // EF_I8                - I
    GL_INTENSITY,           // EF_I16F              - I
    GL_ALPHA,               // EF_A8                - A
    GL_LUMINANCE_ALPHA,     // EF_LA16              - LA
    GL_LUMINANCE_ALPHA,     // EF_LA32F             - LA
    GL_RGBA,                // EF_NORMALMAP         - iYZX (i = ignored)
    GL_RGBA,                // EF_HIGHDEFNORMALMAP  - iYZX (i = ignored)
    0,                      // EF_DEPTH             - not supported
    0,                      // EF_DEPTHSTENCIL      - not supported
};

static unsigned int _elemsPerPixel[] = {
    3,                      // EF_RGB16             - RGB
    3,                      // EF_RGB24             - RGB
    4,                      // EF_RGBA16            - RGBA
    4,                      // EF_RGBA32            - RGBA
    4,                      // EF_RGB48F            - RGB
    4,                      // EF_RGBA64F           - RGBA
    1,                      // EF_L8                - L
    1,                      // EF_I8                - I
    1,                      // EF_I16F              - I
    1,                      // EF_A8                - A
    2,                      // EF_LA16              - LA
    2,                      // EF_LA32F             - LA
    4,                      // EF_NORMALMAP         - iYZX (i = ignored)
    4,                      // EF_HIGHDEFNORMALMAP  - iYZX (i = ignored)
    0,                      // EF_DEPTH             - not supported
    0,                      // EF_DEPTHSTENCIL      - not supported
};

//**********************************************************
// class GrTexture3D
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTexture3D::GrTexture3D( const UPath& name, EFORMAT format, unsigned int width, unsigned int height,
                          unsigned int depth, const void* data )
: GrTextureBase( name, GL_TEXTURE_3D, format, kNoMipMap | kNoCompress, ETT_3D )
, _width( width )
, _height( height )
, _depth( depth )
, _data( 0 )
{
    // allocate a copy of our data.
    unsigned int dataSize = _elemsPerPixel[ format ] * width * height * depth;
    _data = new unsigned char[ dataSize ];
    MemCopy( _data, data, dataSize );

    // create the texture.
    Create();
}

//----------------------------------------------------------
GrTexture3D::~GrTexture3D()
{
    delete[] ( unsigned char* )_data;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTexture3D::UploadImage( bool mipmap )
{
    // mipmapping must be turned off for 3D textures!  If this limitation needs to
    // be removed, then support must be added for 3D texture mipmap generation.
    B_ASSERT( !mipmap );

    unsigned int internalFormat = GetGLFormat( 0 );

    // upload the data.  Note that if this crashes, it's likely that the data the
    // texture was created with is not large enough to fill the whole texture.
    const UPath& test = GetName();
    EFORMAT format = GetFormat();
    bglTexImage3D( GL_TEXTURE_3D, 0, internalFormat, _width, _height, _depth, 0, _dataFormats[ format ], GL_UNSIGNED_BYTE, _data );
    CHECK_GL();
}
