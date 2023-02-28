//----------------------------------------------------------
// File:        GrUtil.cpp
// Author:      Kevin Bray
// Created:     08-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// file header.
#include "GrUtil.h"

// project includes.
#include "GLSubsys.h"
#include "GrConfig.h"
#include "MMat4x4.h"
#include "MVec3.h"
#include "MOBox.h"
#include "GrColor.h"
#include "GrCamera.h"

#define MATRIX_STACK_SIZE       256
#define STATE_STACK_SIZE        256

B_STATIC_ASSERT( ETF_COUNT == 15 );
static GLenum _textureFormatLUT[] =
{
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // ETF_DXT1,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // ETF_DXT5,
    GL_RGB5,                            // ETF_RGB16,
    GL_RGB8,                            // ETF_RGB24,
    GL_RGB5_A1,                         // ETF_RGB15_A1,
    GL_RGBA8,                           // ETF_RGBA32,
    GL_RGB16F_ARB,                      // ETF_RGB48F,
    GL_RGBA16F_ARB,                     // ETF_RGBA64F,
    GL_RGBA32F_ARB,                     // ETF_RGBA128F,
    GL_COMPRESSED_RED_RGTC1_EXT,        // ETF_R_RGTC1,
    GL_COMPRESSED_SIGNED_RED_RGTC1_EXT, // ETF_R_RGTC1_SIGNED,
    GL_COMPRESSED_RED_GREEN_RGTC2_EXT,  // ETF_RG_RGTC2,
    GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,   // ETF_RG_RGTC2_SIGNED,
    ETF_DEPTH,                          // ETF_DEPTH,
    ETF_DEPTHSTENCIL,                   // ETF_DEPTHSTENCIL,
};

B_STATIC_ASSERT( EIF_COUNT == 11 );
static GLenum _imageFormatLUT[] =
{
    GL_RED,                             // EIF_R,
    GL_RGB,                             // EIF_RGB,
    GL_RGBA,                            // EIF_RGBA,
    GL_BGR,                             // EIF_BGR,
    GL_BGRA,                            // EIF_BGRA,
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    // EIF_DXT1,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   // EIF_DXT5,
    GL_COMPRESSED_RED_RGTC1_EXT,        // EIF_R_RGTC1,
    GL_COMPRESSED_SIGNED_RED_RGTC1_EXT, // EIF_R_RGTC1_SIGNED,
    GL_COMPRESSED_RED_GREEN_RGTC2_EXT,  // EIF_RG_RGTC2,
    GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,   // EIF_RG_RGTC2_SIGNED,
};

B_STATIC_ASSERT( ET_COUNT == 8 );
static GLenum _imageFormatTypeLUT[] =
{
    GL_UNSIGNED_BYTE,                   // ET_UNSIGNED_BYTE,
    GL_BYTE,                            // ET_SIGNED_BYTE,
    GL_UNSIGNED_SHORT,                  // ET_UNSIGNED_SHORT,
    GL_SHORT,                           // ET_SIGNED_SHORT,
    GL_UNSIGNED_INT,                    // ET_UNSIGNED_INT,
    GL_INT,                             // ET_SIGNED_INT,
    GL_FLOAT,                           // ET_FLOAT,
    GL_HALF_FLOAT,                      // ET_HALF_FLOAT,
};

B_STATIC_ASSERT( EPT_COUNT == 5 );
static GLenum _primTypeLUT[] =
{
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_LINES,
    GL_LINE_STRIP,
    GL_QUADS,
};


//==========================================================
// global vars.
//==========================================================
bool _GrFilterStates = true;
bool _GrFilterStateModes = true;
bool _GrFilterMasks = true;


//==========================================================
// static global vars.
//==========================================================

// initialized?
static bool _stateInit = false;

// render state management.
static unsigned int _currentState;
static unsigned int _stateStack[ STATE_STACK_SIZE ];
static unsigned int _stateStackTop;

// matrix state management.
static MMat4x4 _modelView;
static MMat4x4 _matrixStack[ MATRIX_STACK_SIZE ];
static unsigned int _matrixStackTop;

// render state mode management.
static EGRBLENDMODE _srcFactorRGB;
static EGRBLENDMODE _srcFactorAlpha;
static EGRBLENDMODE _dstFactorRGB;
static EGRBLENDMODE _dstFactorAlpha;
static EGRSTENCILMODE _stencilMode;
static unsigned int _stencilMask = ~0;
static EGRALPHATESTMODE _alphaTestMode;
static float _alphaTestRef;
static EGRCULLMODE _cullMode;
static EGRDEPTHMODE _depthMode;
static float _slopeBias = 0.0f;
static float _constBias = 0.0f;

// client state enables.
static unsigned int _currentArrayState;

// buffer write enables.
static unsigned int _writeEnable;

// OpenGL state.
static unsigned int _activeTexUnit;
static unsigned int _curTextures[ 16 ];
static unsigned int _curTexTargets[ 16 ];
static unsigned int _curVB;
static unsigned int _curIB;
static unsigned int _curPackPB;
static unsigned int _curUnpackPB;
static unsigned int _curShader;

// viewport state.
static int _viewportX = 0;
static int _viewportY = 0;
static int _viewportWidth = 0;
static int _viewportHeight = 0;

// scissor state.
static int _scissorX = 0;
static int _scissorY = 0;
static int _scissorWidth = 0;
static int _scissorHeight = 0;

// value queries.
static int _maxTextureSize = 0;
static int _maxFramebufferSize = 0;
static int _maxAttribCount = 0;

// internal resources.
static unsigned int _simpleShader = 0;

// internal functions.
static void RestoreSampler( unsigned int texUnit );

//==========================================================
// macros.
//==========================================================
#define CHECK_BIT( b, m )       ( ( ( b ) & ( m ) ) != 0 ? ( GL_TRUE ) : ( GL_ZERO ) )

//**********************************************************
// functions implementations.
//**********************************************************

//----------------------------------------------------------
void
GrStateStart( unsigned int initScreenWidth, unsigned int initScreenHeight )
{
    // initialize render states.
    _currentState = GR_DEPTHTEST | GR_CULLFACE;
    _stateStackTop = 0;
    _currentArrayState = 0;

    // initialize render state modes.
    _srcFactorRGB = EBM_ONE;
    _srcFactorAlpha = EBM_ONE;
    _dstFactorRGB = EBM_ZERO;
    _dstFactorAlpha = EBM_ZERO;
    _stencilMode = ESM_USESTENCIL;
    _alphaTestMode = EAM_GREATER;
    _alphaTestRef = 0.5f;
    _cullMode = ECM_BACK;
    _depthMode = EDM_LESSEQUAL;
    _writeEnable = GR_RGBA | GR_DEPTH;

    // initialize viewport state.
    _viewportX = 0;
    _viewportY = 0;
    _viewportWidth = initScreenWidth;
    _viewportHeight = initScreenHeight;
    bglViewport( 0, 0, _viewportWidth, _viewportHeight );

    // initialize scissor state.
    _scissorX = 0;
    _scissorY = 0;
    _scissorWidth = initScreenWidth;
    _scissorHeight = initScreenHeight;
    bglScissor( 0, 0, _scissorWidth, _scissorHeight );

    // initialize our OpenGL state cache.
    MemSet( _curTextures, 0, sizeof( _curTextures ) );
    MemSet( _curTexTargets, 0, sizeof( _curTexTargets ) );
    _curVB = 0;
    _curIB = 0;
    _curPackPB = 0;
    _curUnpackPB = 0;
    _curShader = 0;

    // initialize our OpenGL matrix stack.
    _matrixStackTop = 0;

    // initialize OpenGL render states.
    bglEnable( GL_DEPTH_TEST );
    bglEnable( GL_CULL_FACE );
    bglDisable( GL_BLEND );
    bglDisable( GL_ALPHA_TEST );
    bglDisable( GL_SCISSOR_TEST );
    bglDisable( GL_STENCIL_TEST );
    bglPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    bglUseProgram( 0 );

    // initialize the OpenGL matrix mode.
    bglMatrixMode( GL_MODELVIEW );

    // initialize OpenGL render state modes.
    bglBlendFunc( GL_ONE, GL_ZERO );
    bglStencilFunc( GL_LESS, 0, _stencilMask );
    bglAlphaFunc( GL_GREATER, 0.5f );
    bglCullFace( GL_BACK );
    bglDepthFunc( GL_LEQUAL );

    // initialize OpenGL write masks.
    bglColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    bglDepthMask( GL_TRUE );
    bglStencilMask( 0 );

    // set the initial draw buffer.
    bglDrawBuffer( GL_BACK_LEFT );

    // clear texture units.
    for ( unsigned int i = 0; i < 16; ++i )
    {
        bglActiveTexture( GL_TEXTURE0+i );
        bglBindTexture( GL_TEXTURE_2D, 0 );
        bglBindTexture( GL_TEXTURE_3D, 0 );
        bglBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    }

    _activeTexUnit = 0;
    bglActiveTexture( GL_TEXTURE0 );

    // clear buffer objects.
    bglBindBuffer( GL_ARRAY_BUFFER, 0 );
    bglBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // query values from OpenGL.
    bglGetIntegerv( GL_MAX_TEXTURE_SIZE, &_maxTextureSize );
    bglGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &_maxFramebufferSize );
    bglGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &_maxAttribCount );
    _maxAttribCount = Min( _maxAttribCount, 32 );

    // mark as initialized and return.
    _stateInit = true;

    // force all client arrays to be disabled.
    _currentArrayState = ~0;
    GrStreamDisableArrays( _currentArrayState );

    // make sure all is well with OpenGL.
    CHECK_GL();
}
//----------------------------------------------------------
bool 
GrStateStarted()
{
    return _stateInit;
}

//----------------------------------------------------------
void
GrStateStop()
{
    _stateInit = false;
}

//----------------------------------------------------------
void
GrEnable( unsigned int flags )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // states that we need to enable.
    unsigned int enable = flags & ~_currentState;
    if ( enable == 0 )
        return;

    // enable states.
    if ( enable & GR_DEPTHTEST )
        bglEnable( GL_DEPTH_TEST );

    if ( enable & GR_CULLFACE )
        bglEnable( GL_CULL_FACE );

    if ( enable & GR_BLEND )
        bglEnable( GL_BLEND );

    if ( enable & GR_ALPHATEST )
        bglEnable( GL_ALPHA_TEST );

    if ( enable & GR_SCISSORTEST )
        bglEnable( GL_SCISSOR_TEST );

    if ( enable & GR_STENCILTEST )
        bglEnable( GL_STENCIL_TEST );

    if ( enable & GR_DEPTHBIAS )
        bglEnable( GL_POLYGON_OFFSET_FILL );

    if ( enable & GR_WIREFRAME )
        bglPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // currently enabled.
    _currentState |= flags;
}

//----------------------------------------------------------
void
GrDisable( unsigned int flags )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // states that we need to disable.
    unsigned int disable = flags & _currentState;
    if ( disable == 0 )
        return;

    // enable states.
    if ( disable & GR_DEPTHTEST )
        bglDisable( GL_DEPTH_TEST );

    if ( disable & GR_CULLFACE )
        bglDisable( GL_CULL_FACE );

    if ( disable & GR_BLEND )
        bglDisable( GL_BLEND );

    if ( disable & GR_ALPHATEST )
        bglDisable( GL_ALPHA_TEST );

    if ( disable & GR_SCISSORTEST )
        bglDisable( GL_SCISSOR_TEST );

    if ( disable & GR_STENCILTEST )
        bglDisable( GL_STENCIL_TEST );

    if ( disable & GR_DEPTHBIAS )
        bglDisable( GL_POLYGON_OFFSET_FILL );

    if ( disable & GR_WIREFRAME )
        bglPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // currently enabled.
    _currentState &= ~flags;
}

//----------------------------------------------------------
void
GrPushState()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // push the current state.
    B_ASSERT( _stateStackTop < STATE_STACK_SIZE );
    _stateStack[ _stateStackTop++ ] = _currentState;
}

//----------------------------------------------------------
void
GrPopState()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( _stateStackTop > 0 );
    GrSetState( _stateStack[ --_stateStackTop ] );
}

//----------------------------------------------------------
unsigned int
GrGetState()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _currentState;
}

//----------------------------------------------------------
void
GrSetState( unsigned int state )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    GrEnable( state );
    GrDisable( ~state );
}

//----------------------------------------------------------
void
GrStreamEnableArrays( unsigned int arrayFlags )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // states that we need to enable.
    unsigned int enable = arrayFlags & ~_currentArrayState;
    if ( enable == 0 )
        return;

    // enable the arrays specified.
    unsigned int flag = 1;
    for ( int i = 0; i < _maxAttribCount; ++i, flag += flag )
    {
        if ( enable & flag )
            bglEnableVertexAttribArray( i );
    }

    // currently enabled.
    _currentArrayState |= enable;
    CHECK_GL();
}

//----------------------------------------------------------
void
GrStreamDisableArrays( unsigned int arrayFlags )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // states that we need to enable.
    unsigned int disable = arrayFlags & _currentArrayState;
    if ( disable == 0 )
        return;

    // disable the arrays specified.
    unsigned int flag = 1;
    for ( int i = 0; i < _maxAttribCount; ++i, flag += flag )
    {
        if ( disable & flag )
            bglDisableVertexAttribArray( i );
    }

    // clear the bits for the streams specified.
    _currentArrayState &= ~disable;
    CHECK_GL();
}

//----------------------------------------------------------
unsigned int
GrStreamGetArrayState()
{
    B_ASSERT( _stateInit );
    return _currentArrayState;
}

//----------------------------------------------------------
void
GrStreamSetArrayState( unsigned int arrayFlags )
{
    B_ASSERT( _stateInit );
    GrStreamEnableArrays( arrayFlags );
    GrStreamDisableArrays( ~arrayFlags );
}

//----------------------------------------------------------
void
GrStreamArrayPointer( unsigned int arrayIndex, unsigned int elemCount, EGRTYPE elemType,
                      bool normalized, unsigned int stride, const void* pointer )
{
    bglVertexAttribPointer( arrayIndex, elemCount, _imageFormatTypeLUT[ elemType ],
        normalized ? GL_TRUE : GL_FALSE, stride, pointer );
}


//==========================================================
// Resource management functions.
//==========================================================

//----------------------------------------------------------
unsigned int
GrGenTexture()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    unsigned int handle;
    bglGenTextures( 1, &handle );
    return handle;
}

//----------------------------------------------------------
void
GrDeleteTexture( unsigned int handle )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    bglDeleteTextures( 1, &handle );
}

//----------------------------------------------------------
unsigned int
GrGenVB()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    unsigned int handle;
    bglGenBuffers( 1, &handle );
    return handle;
}

//----------------------------------------------------------
void
GrDeleteVB( unsigned int handle )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    bglDeleteBuffers( 1, &handle );
}

//----------------------------------------------------------
unsigned int
GrGenIB()
{
    // index buffer share the same namespace as vertex buffers.
    return GrGenVB();
}

//----------------------------------------------------------
void
GrDeleteIB( unsigned int handle )
{
    // index buffer share the same namespace as vertex buffers.
    GrDeleteVB( handle );
}

//----------------------------------------------------------
unsigned int
GrGenPB()
{
    // pixel buffer share the same namespace as vertex buffers.
    return GrGenVB();
}

//----------------------------------------------------------
void
GrDeletePB( unsigned int handle )
{
    // pixel buffer share the same namespace as vertex buffers.
    GrDeleteVB( handle );
}


//==========================================================
// Data upload functions.
//==========================================================

//----------------------------------------------------------
void
GrTexImage2D( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
              unsigned int width, unsigned int height, EGRIMAGEFORMAT format, EGRTYPE type,
              const void* data )
{
    // bind the texture, upload the image, and restore the previous texture.
    bglBindTexture( GL_TEXTURE_2D, handle );
    bglTexImage2D( GL_TEXTURE_2D, miplevel, _textureFormatLUT[ internalFormat ], width,
        height, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], data );
    RestoreSampler( _activeTexUnit );
}

//----------------------------------------------------------
void
GrTexImage3D( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
              unsigned int width, unsigned int height, unsigned int depth, EGRIMAGEFORMAT format,
              EGRTYPE type, const void* data )
{
    // bind the texture, upload the image, and restore the previous texture.
    bglBindTexture( GL_TEXTURE_3D, handle );
    bglTexImage3D( GL_TEXTURE_3D, miplevel, _textureFormatLUT[ internalFormat ], width,
        height, depth, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], data );
    RestoreSampler( _activeTexUnit );
}

//----------------------------------------------------------
void
GrTexImageCube( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
                unsigned int dim, EGRIMAGEFORMAT format, EGRTYPE type, const void* dataPX,
                const void* dataNX, const void* dataPY, const void* dataNY, const void* dataPZ,
                const void* dataNZ )
{
    // bind the texture, upload the images, and restore the previous texture.
    bglBindTexture( GL_TEXTURE_CUBE_MAP, handle );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataPX );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataNX );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataPY );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataNY );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataPZ );
    bglTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, miplevel, _textureFormatLUT[ internalFormat ], dim,
        dim, 0, _imageFormatLUT[ format ], _imageFormatTypeLUT[ type ], dataNZ );
    RestoreSampler( _activeTexUnit );
}


//==========================================================
// Bind state functions.
//==========================================================

//----------------------------------------------------------
void
GrBindTexture( unsigned int texUnit, unsigned int target, unsigned int handle )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( texUnit < 16 );

    // set active texture unit and bind if necessary.
    if ( _activeTexUnit != texUnit )
    {
        bglActiveTexture( GL_TEXTURE0 + texUnit );
        _activeTexUnit = texUnit;
    }

    // do nothing if the texture is already bound.
//  if ( _curTextures[ texUnit ] == handle )
//      return;

    // store the texture we're binding.
    _curTextures[ texUnit ] = handle;
    _curTexTargets[ texUnit ] = target;

    bglBindTexture( target, handle );
    CHECK_GL();
}

//----------------------------------------------------------
unsigned int
GrGetTexture( unsigned int texUnit )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( texUnit < 16 );
    return _curTextures[ texUnit ];
}

//----------------------------------------------------------
unsigned int
GrGetTextureTarget( unsigned int texUnit )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( texUnit < 16 );
    return _curTexTargets[ texUnit ];   
}

//----------------------------------------------------------
void
GrBindVB( unsigned int handle )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
//  if ( _curVB == handle )
//      return;
    bglBindBuffer( GL_ARRAY_BUFFER, handle );
    _curVB = handle;
}

//----------------------------------------------------------
unsigned int
GrGetVB()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _curVB;
}

//----------------------------------------------------------
void
GrBindIB( unsigned int handle )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
//  if ( _curIB == handle )
//      return;
    bglBindBuffer( GL_ELEMENT_ARRAY_BUFFER, handle );
    _curIB = handle;
}

//----------------------------------------------------------
unsigned int
GrGetIB()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _curIB;
}

//----------------------------------------------------------
void GrBindPackPB( unsigned int handle )
{
    B_ASSERT( _stateInit );
    bglBindBuffer( GL_PIXEL_PACK_BUFFER, handle );
    _curPackPB = handle;
}

//----------------------------------------------------------
unsigned int GrGetPackPB()
{
    B_ASSERT( _stateInit );
    return _curPackPB;
}

//----------------------------------------------------------
void GrBindUnpackPB( unsigned int handle )
{
    B_ASSERT( _stateInit );
    bglBindBuffer( GL_PIXEL_UNPACK_BUFFER, handle );
    _curUnpackPB = handle;
}

//----------------------------------------------------------
unsigned int GrGetUnpackPB()
{
    B_ASSERT( _stateInit );
    return _curUnpackPB;
}

//----------------------------------------------------------
void GrBindShader( unsigned int handle )
{
    if ( _curShader != handle )
    {
        bglUseProgram( handle );
        _curShader = handle;
    }
}

//----------------------------------------------------------
unsigned int GrGetShader()
{
    return _curShader;
}


//==========================================================
// Render state modes
//==========================================================

//----------------------------------------------------------
void
GrGetBlendMode( EGRBLENDMODE& srcFactorRGB, EGRBLENDMODE& dstFactorRGB, EGRBLENDMODE& srcFactorAlpha, EGRBLENDMODE& dstFactorAlpha )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    srcFactorRGB = _srcFactorRGB;
    srcFactorAlpha = _srcFactorAlpha;
    dstFactorRGB = _dstFactorRGB;
    dstFactorAlpha = _dstFactorAlpha;
}

//----------------------------------------------------------
void
GrSetBlendMode( EGRBLENDMODE srcFactorRGB, EGRBLENDMODE dstFactorRGB )
{
    GrSetBlendMode( srcFactorRGB, dstFactorRGB, srcFactorRGB, dstFactorRGB );
}

//----------------------------------------------------------
void
GrSetBlendMode( EGRBLENDMODE srcFactorRGB, EGRBLENDMODE dstFactorRGB, EGRBLENDMODE srcFactorAlpha, EGRBLENDMODE dstFactorAlpha )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( srcFactorRGB <= EBM_SRC_ALPHA_SAT );
    B_ASSERT( srcFactorAlpha <= EBM_SRC_ALPHA_SAT );
    B_ASSERT( dstFactorRGB <= EBM_SRC_ALPHA_SAT );
    B_ASSERT( dstFactorAlpha <= EBM_SRC_ALPHA_SAT );
    static const GLenum modes[ EBM_SRC_ALPHA_SAT+1 ] = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_SRC_ALPHA_SATURATE,
    };

    if ( srcFactorRGB != _srcFactorRGB || srcFactorAlpha != _srcFactorAlpha ||
         dstFactorRGB != _dstFactorRGB || dstFactorAlpha != _dstFactorAlpha )
    {
        bglBlendFuncSeparate( modes[ srcFactorRGB ], modes[ dstFactorRGB ], modes[ srcFactorAlpha ], modes[ dstFactorAlpha ] );
        _srcFactorRGB = srcFactorRGB;
        _srcFactorAlpha = srcFactorAlpha;
        _dstFactorRGB = dstFactorRGB;
        _dstFactorAlpha = dstFactorAlpha;
    }
}

//----------------------------------------------------------
EGRSTENCILMODE
GrGetStencilMode()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _stencilMode;
}

//----------------------------------------------------------
void
GrSetStencilMode( EGRSTENCILMODE mode, unsigned int mask )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( mode < 4 && mode >= 0 );
    static const GLenum zfail[ 4 ] =
    {
        GL_INCR_WRAP_EXT,
        GL_DECR_WRAP_EXT,
        GL_KEEP,
        GL_KEEP,
    };
    static const GLenum zpass[ 4 ] =
    {
        GL_KEEP,
        GL_KEEP,
        GL_INCR_WRAP_EXT,
        GL_KEEP,
    };
    static const GLenum func[ 4 ] =
    {
        GL_ALWAYS,
        GL_ALWAYS,
        GL_ALWAYS,
        GL_LESS,
    };

    if ( _stencilMode != mode || _stencilMask != mask )
    {
        bglStencilFunc( func[ mode ], 0, mask );
        bglStencilOp( GL_KEEP, zfail[ mode ], zpass[ mode ]);
        _stencilMode = mode;
    }
}

//----------------------------------------------------------
EGRALPHATESTMODE
GrGetAlphaTestMode()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _alphaTestMode;
}

//----------------------------------------------------------
void
GrSetAlphaTestMode( EGRALPHATESTMODE mode, float ref )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( mode < 6 && mode >= 0 );
    static const GLenum modes[ 6 ] =
    {
        GL_LESS,
        GL_LEQUAL,
        GL_GREATER,
        GL_GEQUAL,
        GL_EQUAL,
        GL_NOTEQUAL,
    };

    if ( mode != _alphaTestMode || ref != _alphaTestRef )
    {
        bglAlphaFunc( modes[ mode ], ref );
        _alphaTestMode = mode;
        _alphaTestRef = ref;
    }
}

//----------------------------------------------------------
EGRCULLMODE
GrGetCullMode()
{
    return _cullMode;
}

//----------------------------------------------------------
void
GrSetCullMode( EGRCULLMODE mode )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    B_ASSERT( mode < 2 && mode >= 0 );
    static const GLenum modes[ 2 ] =
    {
        GL_FRONT,
        GL_BACK,
    };
    if ( mode != _cullMode )
    {
        bglCullFace( modes[ mode ] );
        _cullMode = mode;
    }
}

//----------------------------------------------------------
EGRDEPTHMODE
GrGetDepthMode()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _depthMode;
}

//----------------------------------------------------------
void
GrSetDepthMode( EGRDEPTHMODE mode )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    static const GLenum modes[ 3 ] =
    {
        GL_LEQUAL,
        GL_EQUAL,
        GL_ALWAYS,
    };
    if ( mode != _depthMode )
    {
        bglDepthFunc( modes[ mode ] );
        _depthMode = mode;
    }
}

//----------------------------------------------------------
void
GrGetDepthBias( float& slopeBias, float& constBias )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    slopeBias = _slopeBias;
    constBias = _constBias;
}

//----------------------------------------------------------
void
GrSetDepthBias( float slopeBias, float constBias )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    _slopeBias = slopeBias;
    _constBias = constBias;
    bglPolygonOffset( slopeBias, constBias );
}


//==========================================================
// Viewport management.
//==========================================================

//----------------------------------------------------------
void
GrViewport( int x, int y, int width, int height )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    _viewportX = x;
    _viewportY = y;
    _viewportWidth = width;
    _viewportHeight = height;
    bglViewport( x, y, width, height );
}

//----------------------------------------------------------
void
GrGetViewport( int& x, int& y, int& width, int& height )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    x = _viewportX;
    y = _viewportY;
    width = _viewportWidth;
    height = _viewportHeight;
}


//==========================================================
// Scissor management.
//==========================================================

//----------------------------------------------------------
void
GrScissor( int x, int y, int width, int height )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    _scissorX = x;
    _scissorY = y;
    _scissorWidth = width;
    _scissorHeight = height;
    bglScissor( x, y, width, height );
}

//----------------------------------------------------------
void
GrGetScissor( int& x, int& y, int& width, int& height )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    x = _scissorX;
    y = _scissorY;
    width = _scissorWidth;
    height = _scissorHeight;
}


//==========================================================
// Transform management.
//==========================================================

//----------------------------------------------------------
void GrSetProjectionMatrix( const MMat4x4& proj )
{
    // setup our global matrix states.
    bglMatrixMode( GL_PROJECTION );
    bglLoadTransposeMatrixf( proj );
    bglMatrixMode( GL_MODELVIEW );
}

//----------------------------------------------------------
void GrSetModelViewMatrix( const MMat4x4& modelView )
{
    _modelView = modelView;
    bglLoadTransposeMatrixf( _modelView );
}

//----------------------------------------------------------
void GrMultiplyModelViewMatrix( const MMat4x4& matrix )
{
    _modelView = _modelView * matrix;
    bglLoadTransposeMatrixf( _modelView );
}

//----------------------------------------------------------
void GrPushModelViewMatrix()
{
    B_ASSERT( _matrixStackTop < MATRIX_STACK_SIZE );
    _matrixStack[ _matrixStackTop++ ] = _modelView;
}

//----------------------------------------------------------
void GrPopModelViewMatrix()
{
    B_ASSERT( _matrixStackTop > 0 );
    _modelView = _matrixStack[ --_matrixStackTop ];
    bglLoadTransposeMatrixf( _modelView );  
}


//==========================================================
// Buffer write enables
//==========================================================

//----------------------------------------------------------
unsigned int
GrGetWriteEnable()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _writeEnable;
}

//----------------------------------------------------------
void
GrSetWriteEnable( unsigned int mask )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    unsigned int change = mask ^ _writeEnable;
    if ( change & GR_RGBA )
        bglColorMask( CHECK_BIT( mask, GR_RED ), CHECK_BIT( mask, GR_GREEN ), CHECK_BIT( mask, GR_BLUE ), CHECK_BIT( mask, GR_ALPHA ) );

    if ( change & GR_DEPTH )
        bglDepthMask( CHECK_BIT( mask, GR_DEPTH ) );

    if ( change & GR_STENCIL )
        bglStencilMask( ( mask & GR_STENCIL ) != 0 ? ~0 : 0 );

    _writeEnable = mask;
}


//==========================================================
// Draw buffer management
//==========================================================

//----------------------------------------------------------
void
GrDrawBuffers( unsigned int buffers )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    CHECK_GL();
    // set the buffers we're drawing to.
    GLint n = 0;
    GLenum bufs[4];
    if ( buffers & GR_FRONT )
        bufs[n++] = GL_FRONT_LEFT;
    if ( buffers & GR_BACK )
        bufs[n++] = GL_BACK_LEFT;
    if ( buffers & GR_AUX0 )
        bufs[n++] = GL_AUX0;
    if ( buffers & GR_AUX1 )
        bufs[n++] = GL_AUX1;
    if ( buffers & GR_AUX2 )
        bufs[n++] = GL_AUX2;
    if ( buffers & GR_COLOR_ATTACH0 )
        bufs[n++] = GL_COLOR_ATTACHMENT0_EXT;
    if ( buffers & GR_COLOR_ATTACH1 )
        bufs[n++] = GL_COLOR_ATTACHMENT1_EXT;
    if ( buffers & GR_COLOR_ATTACH2 )
        bufs[n++] = GL_COLOR_ATTACHMENT2_EXT;
    if ( buffers & GR_COLOR_ATTACH3 )
        bufs[n++] = GL_COLOR_ATTACHMENT3_EXT;
    if ( n == 0 )
        bufs[n++] = GL_NONE;

    // set the draw buffer.
    if ( n == 1 )
        bglDrawBuffer( bufs[ 0 ] );
    else
        bglDrawBuffers( n, bufs );
    CHECK_GL();
}

//----------------------------------------------------------
void
GrReadBuffer( unsigned int buffer )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    // can only pass in one flag!
    B_ASSERT( IsPow2( buffer ) || buffer == 0 );
    if ( buffer == GR_FRONT )
        bglReadBuffer( GL_FRONT_LEFT );
    else if ( buffer == GR_BACK )
        bglReadBuffer( GL_BACK_LEFT );
    else if ( buffer == GR_AUX0 )
        bglReadBuffer( GL_AUX0 );
    else if ( buffer == GR_AUX1 )
        bglReadBuffer( GL_AUX1 );
    else if ( buffer == GR_AUX1 )
        bglReadBuffer( GL_AUX1 );
    else if ( buffer == GR_COLOR_ATTACH0 )
        bglReadBuffer( GL_COLOR_ATTACHMENT0_EXT );
    else if ( buffer == GR_COLOR_ATTACH1 )
        bglReadBuffer( GL_COLOR_ATTACHMENT1_EXT );
    else if ( buffer == GR_COLOR_ATTACH2 )
        bglReadBuffer( GL_COLOR_ATTACHMENT2_EXT );
    else if ( buffer == GR_COLOR_ATTACH3 )
        bglReadBuffer( GL_COLOR_ATTACHMENT3_EXT );
    else if ( buffer == 0 )
        bglReadBuffer( GL_NONE );
}


//==========================================================
// Rendering functions
//==========================================================

//----------------------------------------------------------
void
GrClear( float red, float green, float blue, float alpha, float depth, unsigned int stencil )
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );

    unsigned int clearFlags = 0;
    if ( _writeEnable & GR_RGBA )
    {
        bglClearColor( red, green, blue, alpha );
        clearFlags |= GL_COLOR_BUFFER_BIT;
    }
    if ( _writeEnable & GR_DEPTH )
    {
        bglClearDepth( depth );
        clearFlags |= GL_DEPTH_BUFFER_BIT;
    }
    if ( _writeEnable & GR_STENCIL )
    {
        bglClearStencil( stencil );
        clearFlags |= GL_STENCIL_BUFFER_BIT;
    }
    if ( clearFlags )
        bglClear( clearFlags );

    CHECK_GL();
}

//----------------------------------------------------------
void
GrDrawArrays( EGRPRIMTYPE primType, unsigned int start, unsigned int count )
{
    bglDrawArrays( _primTypeLUT[ primType ], start, count );
}

//----------------------------------------------------------
void
GrDrawRangeArrays( EGRPRIMTYPE primType, unsigned int minIndex, unsigned int maxIndex,
                   unsigned int count, EGRTYPE indexType, void* indices )
{
    bglDrawRangeElements( _primTypeLUT[ primType ], minIndex, maxIndex, count,
        _imageFormatTypeLUT[ indexType ], indices );
}


//==========================================================
// Miscellaneous
//==========================================================

//----------------------------------------------------------
int
GrGetMaxTextureSize()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _maxTextureSize;
}

//----------------------------------------------------------
int
GrGetMaxFramebufferSize()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _maxFramebufferSize;
}

//----------------------------------------------------------
int
GrGetMaxAttribCount()
{
    // must call GrStateStart() first!
    B_ASSERT( _stateInit );
    return _maxAttribCount;
}

//**********************************************************
// File local functions.
//**********************************************************

//----------------------------------------------------------
void
RestoreSampler( unsigned int texUnit )
{
    bglBindTexture( _curTexTargets[ texUnit ], _curTextures[ texUnit ] );
    CHECK_GL();
}
