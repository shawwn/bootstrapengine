//----------------------------------------------------------
// File:        GrTextureBase.cpp
// Author:      Kevin Bray
// Created:     10-01-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// include the texture base class.
#include "GrTextureBase.h"

// project includes.
#include "GrTextureMgr.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrImage.h"
#include "RFileMgr.h"
#include "RFile.h"


// textures whose dimensions ( width *or* height ) >= is greater than this size
// honor the _mipLevelIgnore settings.
const unsigned int kMinLevelIgnoreSize = 1024;

// _compress[ _texturingQuality ]
const bool _compressMap[] = {
    false,      // ultra
    true,       // high
    true,       // medium
    true,       // low
};

// enum->value maps.
const bool _canCompressMap[ GrTextureBase::EF_COUNT ] = {
    false,              // EF_DXT1
    false,              // EF_DXT5
    false,              // EF_DXT5R
    true,               // EF_RGB16
    true,               // EF_RGB24
    true,               // EF_RGB15_A1
    true,               // EF_RGBA32
    false,              // EF_RGB48F
    false,              // EF_RGBA64F
    false,              // EF_RGBA128F
    true,               // EF_R8
    true,               // EF_RG16
    false,              // EF_L8
    false,              // EF_I8
    false,              // EF_I16F
    false,              // EF_A8
    false,              // EF_LA16
    false,              // EF_LA32F
    true,               // EF_NORMALMAP
    false,              // EF_HIGHDEFNORMALMAP
    false,              // EF_DEPTH
    false,              // EF_DEPTHSTENCIL
    true,               // EF_GUESS (invalid)
};

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

const GLenum _compressedInternalFormatMap[ GrTextureBase::EF_COUNT ] = {
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // EF_DXT1
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EF_DXT5
    GL_COMPRESSED_RED_RGTC1,            // EF_DXT5R
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // EF_RGB16
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // EF_RGB24
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   // EF_RGB15_A1
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EF_RGBA32
    GL_RGB16F_ARB,                      // EF_RGB48F, connot compress
    GL_RGBA16F_ARB,                     // EF_RGBA64F, connot compress
    GL_RGBA32F_ARB,                     // EF_RGBA128F, connot compress
    GL_COMPRESSED_RED_RGTC1,            // EF_R8,
    GL_COMPRESSED_RG_RGTC2,             // EF_RG16,
    GL_LUMINANCE8,                      // EF_L8, connot compress
    GL_INTENSITY8,                      // EF_I8, cannot compress
    GL_INTENSITY16F_ARB,                // EF_I16F, cannot compress
    GL_ALPHA8,                          // EF_A8, connot compress
    GL_LUMINANCE_ALPHA,                 // EF_LA16, connot compress
    GL_LUMINANCE_ALPHA16F_ARB,          // EF_LA32F, connot compress
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EF_NORMALMAP
    GL_RGBA16F_ARB,                     // EF_HIGHDEFNORMALMAP
    GL_DEPTH_COMPONENT,                 // EF_DEPTH, connot compress
    GL_DEPTH_STENCIL_EXT,               // EF_DEPTHSTENCIL, connot compress
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EF_GUESS, invalid
};

/*
// _filterMap[ _texturingQuality ][ _filterQual ][ min=0, mag=1 ]
static const GLenum _filterMap[ EGQ_COUNT ][ GrTextureBase::EFM_COUNT ][ 2 ] =
{
    // ultra texture quality.
    {
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // anisotropic.
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // trilinear
        {   GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,    },  // bilinear
        {   GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,  },  // nearest
    },

    // high texture quality.
    {
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // anisotropic.
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // trilinear
        {   GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,    },  // bilinear
        {   GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,  },  // nearest
    },

    // medium texture quality.
    {
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // anisotropic.
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // trilinear
        {   GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,    },  // bilinear
        {   GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,  },  // nearest
    },

    // low texture quality.
    {
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // anisotropic.
        {   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,     },  // trilinear
        {   GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,    },  // bilinear
        {   GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,  },  // nearest
    },
};
*/
static const float _maxAnisotropy[ EGQ_COUNT ][ GrTextureBase::EFM_COUNT ] =
{
    { 16.0f, 16.0f, 1.0f, 1.0f },
    { 16.0f, 16.0f, 1.0f, 1.0f },
    { 16.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
};

//**********************************************************
// class GrTextureBase
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureBase::GrTextureBase( const UPath& name, unsigned int bindTarget, EFORMAT format,
                              Flags flags, ETEXTURETYPE type )
: _name( name )
, _glHandle( 0 )
, _bindTarget( bindTarget )
, _flags( flags )
, _glMinFilter( ( flags & kNoMipMap ) ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST )
, _glMagFilter( GL_NEAREST )
, _glAddressS( ( bindTarget == GL_TEXTURE_2D ) ? GL_REPEAT : GL_CLAMP )
, _glAddressT( ( bindTarget == GL_TEXTURE_2D ) ? GL_REPEAT : GL_CLAMP )
, _redBits( 0 )
, _greenBits( 0 )
, _blueBits( 0 )
, _alphaBits( 0 )
, _luminanceBits( 0 )
, _mipBias( 0 )
, _maxAnisotropy( 1.0f )
, _format( format )
, _type( type )
, _unloadable( true )
, _created( false )
{

}

//----------------------------------------------------------
GrTextureBase::~GrTextureBase()
{
    // delete the texture handle.
    GrDeleteTexture( _glHandle );
    _glHandle = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTextureBase::SetFlags( Flags flags )
{
    _flags = flags;
}

//----------------------------------------------------------
void
GrTextureBase::Bind( unsigned int texUnit )
{
    B_ASSERT( texUnit < 16 );
    B_ASSERT( _glHandle != 0 );
    GrBindTexture( texUnit, _bindTarget, _glHandle );
    CHECK_GL();
}

//----------------------------------------------------------
void
GrTextureBase::Bind( unsigned int texUnit, unsigned int glMinFilter, unsigned int glMagFilter, unsigned int glAddressS,
                    unsigned int glAddressT, float maxAnisotropy, int mipBias )
{
    CHECK_GL();

    B_ASSERT( texUnit < 16 );
    B_ASSERT( _glHandle != 0 );
    B_ASSERT( maxAnisotropy >= 1.0f );
    GrBindTexture( texUnit, _bindTarget, _glHandle );
    CHECK_GL();

    if ( GetFormat() == EF_DEPTH || GetFormat() == EF_DEPTHSTENCIL )
    {
        glMinFilter = GL_NEAREST;
        glMagFilter = GL_NEAREST;
    }

    if ( _glMinFilter != glMinFilter )
    {
        _glMinFilter = glMinFilter;
        bglTexParameteri( _bindTarget, GL_TEXTURE_MIN_FILTER, glMinFilter );
        CHECK_GL();
    }
    if ( _glMagFilter != glMagFilter )
    {
        _glMagFilter = glMagFilter;
        bglTexParameteri( _bindTarget, GL_TEXTURE_MAG_FILTER, glMagFilter );
        CHECK_GL();
    }

    if ( _glAddressS != glAddressS )
    {
        _glAddressS = glAddressS;
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_S, glAddressS );
    }
    if ( _glAddressT != glAddressT )
    {
        _glAddressT = glAddressT;
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_T, glAddressT );
    }
    if ( _maxAnisotropy != maxAnisotropy )
    {
        _maxAnisotropy = maxAnisotropy;
        bglTexParameterf( _bindTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
    }
    if ( ( ( _flags & kNoMipMap ) == 0 ) && _mipBias != mipBias )
    {
        _mipBias = mipBias;
        bglTexParameteri( _bindTarget, GL_TEXTURE_LOD_BIAS_EXT, _mipBias );
    }
}

//----------------------------------------------------------
void
GrTextureBase::Unbind()
{
    for ( unsigned int i = 0; i < 15; ++i )
    {
        if ( GrGetTexture( i ) == _glHandle )
            GrBindTexture( i, _bindTarget, 0 );
    }
}

//----------------------------------------------------------
void
GrTextureBase::AutoGenMipmaps()
{
    // cannot be called if we're not using mipmaps!
    B_ASSERT( ~_flags & kNoMipMap );

    // bind the texture and generate the mipmap chain.
    Bind( 0 );
    bglGenerateMipmapEXT( _bindTarget );
    CHECK_GL();
}

//----------------------------------------------------------
void
GrTextureBase::Recreate()
{
    PreContextReset();
    PostContextReset();
}

//----------------------------------------------------------
void
GrTextureBase::PreContextReset()
{
    GrDeleteTexture( _glHandle );
    _glHandle = 0;

}

//----------------------------------------------------------
void
GrTextureBase::PostContextReset()
{
    // clear the created handle so that textures get recreated.
    _created = false;
    Create();

    // set all texture object state.
    bglTexParameteri( _bindTarget, GL_TEXTURE_MIN_FILTER, _glMinFilter );
    bglTexParameteri( _bindTarget, GL_TEXTURE_MAG_FILTER, _glMagFilter );
    bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_S, _glAddressS );
    bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_T, _glAddressT );
    bglTexParameterf( _bindTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, _maxAnisotropy );
    bglTexParameteri( _bindTarget, GL_TEXTURE_LOD_BIAS_EXT, _mipBias );

    // make sure everything is okay in GL land.
    CHECK_GL();
}


//==========================================================
// protected methods
//==========================================================

//----------------------------------------------------------
void
GrTextureBase::Create()
{
    B_ASSERT( _glHandle == 0 );

    // create the handle and to texture stage 7.
    _glHandle = GrGenTexture();
    Bind( 0 );

    // initialize the filter modes.
    bglTexParameteri( _bindTarget, GL_TEXTURE_MIN_FILTER, _glMinFilter );
    bglTexParameteri( _bindTarget, GL_TEXTURE_MAG_FILTER, _glMagFilter );
    CHECK_GL();

    // set texture addressing.
    if ( _bindTarget == GL_TEXTURE_RECTANGLE_ARB )
    {
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_S, GL_CLAMP );
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_T, GL_CLAMP );
        CHECK_GL();
    }
    else
    {
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_S, _glAddressS );
        bglTexParameteri( _bindTarget, GL_TEXTURE_WRAP_T, _glAddressT );
        CHECK_GL();
    }

    UploadImage( ( _flags & kNoMipMap ) == 0 );
    CHECK_GL();

    unsigned int levelBindTarget = _bindTarget;
    if ( _bindTarget == GL_TEXTURE_CUBE_MAP )
    {
        levelBindTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }

    // store color information about the texture.
    bglGetTexLevelParameteriv( levelBindTarget, 0, GL_TEXTURE_RED_SIZE, ( GLint* )&_redBits );
    bglGetTexLevelParameteriv( levelBindTarget, 0, GL_TEXTURE_GREEN_SIZE, ( GLint* )&_greenBits );
    bglGetTexLevelParameteriv( levelBindTarget, 0, GL_TEXTURE_BLUE_SIZE, ( GLint* )&_blueBits );
    bglGetTexLevelParameteriv( levelBindTarget, 0, GL_TEXTURE_ALPHA_SIZE, ( GLint* )&_alphaBits );
    bglGetTexLevelParameteriv( levelBindTarget, 0, GL_TEXTURE_LUMINANCE_SIZE, ( GLint* )&_luminanceBits );
    CHECK_GL();

    _created = true;
}

//----------------------------------------------------------
unsigned int
GrTextureBase::GetGLFormat( GrImage* image )
{
    EFORMAT format = _format;
    if ( format == EF_GUESS )
    {
        static EFORMAT guessMap[] =
        {
            EF_RGB24,   // GrImage::EF_BGR24,
            EF_RGBA32,  // GrImage::EF_BGRA32,
            EF_RGB48F,  // GrImage::EF_RGB96F,
            EF_RGBA64F, // GrImage::EF_RGBA128F,
            EF_I8,      // GrImage::EF_I8,
            EF_I16F,    // GrImage::EF_I16F,
            EF_LA16,    // GrImage::EF_LA16,
            EF_LA32F,   // GrImage::EF_LA64F,
            EF_DEPTH,   // GrImage::EF_DEPTH,
        };
        if ( image )
            format = guessMap[ image->GetFormat() ];
        else
            format = EF_RGBA32;
    }

    // determine whether or not we should use a compressed format.
    EGRQUALITY texQuality = gGrTextureMgr->GetTexturingQuality();
    bool compress = _compressMap[ texQuality ] && !( _flags & kNoCompress ) &&
        _canCompressMap[ format ];

    // determine the storage format.
    unsigned int glFormat = 0;
    if ( compress )
        glFormat = _compressedInternalFormatMap[ format ];
    else
        glFormat = _internalFormatMap[ format ];

    // return the format we selected.
    return glFormat;
}

//----------------------------------------------------------
GrImage*
GrTextureBase::CreateImage( const UPath& name )
{
    // read in the file data and pass it to a reader.
    URef< RFile > file = gRFileMgr->GetFile( name, RFileMgr::kFlagRead );
    if ( !file )
    {
        // if we were unable to open the file, simply
        // return null.
        return 0;
    }

    // get the file's data.
    const void* fileData = file->GetData();
    unsigned int size = ( unsigned int )file->GetSize();
    if ( !size )
        return 0;

    // create the image.
    UReader reader( fileData, size, false );
    GrImage* image = new GrImage( name, reader );

    // free up the file data.
    file->Purge();

    // return the image.
    return image;
}
