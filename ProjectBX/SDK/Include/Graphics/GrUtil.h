//----------------------------------------------------------
// File:        GrUtil.h
// Author:      Kevin Bray
// Created:     08-20-05
//
// Purpose:     To provide simple rendering utilities.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec3;
class MMat4x4;
class GrColor;
class GrCamera;
class GrFrustum;

//==========================================================
// render states.
#define GR_DEPTHTEST                0x0001
#define GR_CULLFACE                 0x0002
#define GR_BLEND                    0x0004
#define GR_ALPHATEST                0x0008
#define GR_SCISSORTEST              0x0010
#define GR_STENCILTEST              0x0020
#define GR_DEPTHBIAS                0x0040
#define GR_WIREFRAME                0x8000

//==========================================================
// write masks.
#define GR_RED                      0x01
#define GR_BLUE                     0x02
#define GR_GREEN                    0x04
#define GR_ALPHA                    0x08
#define GR_RGB                      0x07
#define GR_RGBA                     0x0F
#define GR_DEPTH                    0x10
#define GR_STENCIL                  0x20

//==========================================================
// draw buffers.
#define GR_FRONT                    0x0001
#define GR_BACK                     0x0002
#define GR_AUX0                     0x0010
#define GR_AUX1                     0x0020
#define GR_AUX2                     0x0040
#define GR_COLOR_ATTACH0            0x0100
#define GR_COLOR_ATTACH1            0x0200
#define GR_COLOR_ATTACH2            0x0400
#define GR_COLOR_ATTACH3            0x0800

//==========================================================
// true/false definitions.
#define GR_TRUE                     1
#define GR_FALSE                    0

//==========================================================
// render state modes
enum EGRBLENDMODE
{
    EBM_ZERO,
    EBM_ONE,
    EBM_SRC_COLOR,
    EBM_DST_COLOR,
    EBM_ONE_MINUS_SRC_COLOR,
    EBM_ONE_MINUS_DST_COLOR,
    EBM_SRC_ALPHA,
    EBM_DST_ALPHA,
    EBM_ONE_MINUS_SRC_ALPHA,
    EBM_ONE_MINUS_DST_ALPHA,
    EBM_SRC_ALPHA_SAT,
    EGRBLENDMODE_DWORD = 0xFFFFFFFF,
};

enum EGRSTENCILMODE
{
    ESM_ZFAILINCR,      // increments the stencil if the z-test fails for back faces.
    ESM_ZFAILDECR,      // decrementss the stencil if the z-test fails for front faces.
    ESM_ZPASSINCR,      // increments the stencil if the z-test passes for front faces.
    ESM_USESTENCIL,     // uses the stencil buffer to render ( renders to areas where stencil>0 ).
    EGRSTENCILMODE_DWORD = 0xFFFFFFFF,
};

enum EGRALPHATESTMODE
{
    EAM_LESS,
    EAM_LESSEQUAL,
    EAM_GREATER,
    EAM_GREATEREQUAL,
    EAM_EQUAL,
    EAM_NOTEQUAL,
    EGRALPHATESTMODE_DWORD = 0xFFFFFFFF,
};

enum EGRCULLMODE
{
    ECM_FRONT,
    ECM_BACK,
    EGRCULLMODE_DWORD = 0xFFFFFFFF,
};

enum EGRDEPTHMODE
{
    EDM_LESSEQUAL,
    EDM_EQUAL,
    EDM_ALWAYS,
    EGRDEPTHMODE_DWORD = 0xFFFFFFFF,
};

enum EGRTEXTUREFORMAT
{
    ETF_DXT1,
    ETF_DXT5,
    ETF_RGB16,
    ETF_RGB24,
    ETF_RGB15_A1,           // 5:5:5:1
    ETF_RGBA32,
    ETF_RGB48F,
    ETF_RGBA64F,
    ETF_RGBA128F,
    ETF_R_RGTC1,            // red.
    ETF_R_RGTC1_SIGNED,     // red.
    ETF_RG_RGTC2,           // red-green.
    ETF_RG_RGTC2_SIGNED,    // red-green.
    ETF_DEPTH,
    ETF_DEPTHSTENCIL,

    // invalid texture format.
    ETF_COUNT,
};

enum EGRIMAGEFORMAT
{
    EIF_R,
    EIF_RGB,
    EIF_RGBA,
    EIF_BGR,
    EIF_BGRA,
    EIF_DXT1,
    EIF_DXT5,
    EIF_R_RGTC1,
    EIF_R_RGTC1_SIGNED,
    EIF_RG_RGTC2,
    EIF_RG_RGTC2_SIGNED,

    // invalid image format.
    EIF_COUNT,
};

enum EGRTYPE
{
    ET_UNSIGNED_BYTE,
    ET_SIGNED_BYTE,
    ET_UNSIGNED_SHORT,
    ET_SIGNED_SHORT,
    ET_UNSIGNED_INT,
    ET_SIGNED_INT,
    ET_FLOAT,
    ET_HALF_FLOAT,

    // invalid format type.
    ET_COUNT,
};

enum EGRPRIMTYPE
{
    EPT_TRIANGLES,
    EPT_TRIANGLE_STRIP,
    EPT_LINES,
    EPT_LINE_STRIP,
    EPT_QUADS,

    // invalid primitive type.
    EPT_COUNT,
};


// turn state filtering on and off.
//extern bool _GrForceSetState;

//**********************************************************
// functions prototypes.
//**********************************************************

//==========================================================
// Render state management.
extern void GrStateStart( unsigned int initScreenWidth, unsigned int initScreenHeight );
extern bool GrStateStarted();
extern void GrStateStop();
extern void GrEnable( unsigned int state );
extern void GrDisable( unsigned int state );
extern void GrPushState();
extern void GrPopState();
extern unsigned int GrGetState();
extern void GrSetState( unsigned int state );

//==========================================================
// Vertex stream management.
extern void GrStreamEnableArrays( unsigned int arrayFlags );
extern void GrStreamDisableArrays( unsigned int arrayFlags );
extern unsigned int GrStreamGetArrayState();
extern void GrStreamSetArrayState( unsigned int arrayFlags );
extern void GrStreamArrayPointer( unsigned int arrayIndex, unsigned int elemCount, EGRTYPE elemType,
                                  bool normalized, unsigned int stride, const void* pointer );

//==========================================================
// Render state modes.
extern void GrGetBlendMode( EGRBLENDMODE& srcFactorRGB, EGRBLENDMODE& dstFactorRGB,
                            EGRBLENDMODE& srcFactorAlpha, EGRBLENDMODE& dstFactorAlpha );
extern void GrSetBlendMode( EGRBLENDMODE srcFactorRGB, EGRBLENDMODE dstFactorRGB );
extern void GrSetBlendMode( EGRBLENDMODE srcFactorRGB, EGRBLENDMODE dstFactorRGB,
                            EGRBLENDMODE srcFactorAlpha, EGRBLENDMODE dstFactorAlpha );
extern EGRSTENCILMODE GrGetStencilMode();
extern void GrSetStencilMode( EGRSTENCILMODE mode, unsigned int mask );
extern EGRALPHATESTMODE GrGetAlphaTestMode();
extern void GrSetAlphaTestMode( EGRALPHATESTMODE mode, float ref );
extern EGRCULLMODE GrGetCullMode();
extern void GrSetCullMode( EGRCULLMODE mode );
extern EGRDEPTHMODE GrGetDepthMode();
extern void GrSetDepthMode( EGRDEPTHMODE mode );
extern void GrGetDepthBias( float& slopeBias, float& constBias );
extern void GrSetDepthBias( float slopeBias, float constBias );

//==========================================================
// Viewport management.
extern void GrViewport( int x, int y, int width, int height );
extern void GrGetViewport( int& x, int& y, int& width, int& height );

//==========================================================
// Scissor management.
extern void GrScissor( int x, int y, int width, int height );
extern void GrGetScissor( int& x, int& y, int& width, int& height );

//==========================================================
// Transform management.
/*
extern void GrSetProjectionMatrix( const MMat4x4& proj );
extern void GrSetModelViewMatrix( const MMat4x4& modelView );
extern void GrMultiplyModelViewMatrix( const MMat4x4& matrix );
extern void GrPushModelViewMatrix();
extern void GrPopModelViewMatrix();
*/
//==========================================================
// Buffer write enables.
extern unsigned int GrGetWriteEnable();
extern void GrSetWriteEnable( unsigned int mask );

//==========================================================
// Resource management functions.
extern unsigned int GrGenTexture();
extern void GrDeleteTexture( unsigned int handle );
extern unsigned int GrGenVB();
extern void GrDeleteVB( unsigned int handle );
extern unsigned int GrGenIB();
extern void GrDeleteIB( unsigned int handle );
extern unsigned int GrGenPB();
extern void GrDeletePB( unsigned int handle );

//==========================================================
// Resource data functions.
extern void GrTexImage2D( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
                          unsigned int width, unsigned int height, EGRIMAGEFORMAT format, EGRTYPE type,
                          const void* data );
extern void GrTexImage3D( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
                          unsigned int width, unsigned int height, unsigned int depth, EGRIMAGEFORMAT format,
                          EGRTYPE type, const void* data );
extern void GrTexImageCube( unsigned int handle, unsigned int miplevel, EGRTEXTUREFORMAT internalFormat,
                            unsigned int dim, EGRIMAGEFORMAT format, EGRTYPE type, const void* dataPX,
                            const void* dataNX, const void* dataPY, const void* dataNY, const void* dataPZ,
                            const void* dataNZ );

//==========================================================
// Bind state functions.
extern void GrBindTexture( unsigned int texUnit, unsigned int target, unsigned int handle );
extern unsigned int GrGetTexture( unsigned int texUnit );
extern unsigned int GrGetTextureTarget( unsigned int texUnit );
extern void GrBindVB( unsigned int handle );
extern unsigned int GrGetVB();
extern void GrBindIB( unsigned int handle );
extern unsigned int GrGetIB();
extern void GrBindPackPB( unsigned int handle );
extern unsigned int GrGetPackPB();
extern void GrBindUnpackPB( unsigned int handle );
extern unsigned int GrGetUnpackPB();
extern void GrBindShader( unsigned int handle );
extern unsigned int GrGetShader();

//==========================================================
// Buffer draw enable.
extern void GrDrawBuffers( unsigned int buffers );
extern void GrReadBuffer( unsigned int buffer );

//==========================================================
// Rendering functions.
extern void GrClear( float red=0.0f, float green=0.0f, float blue=0.0f, float alpha=0.0f, float depth=1.0f, unsigned int stencil=0 );
extern void GrDrawArrays( EGRPRIMTYPE primType, unsigned int start, unsigned int vertexCount );
extern void GrDrawRangeArrays( EGRPRIMTYPE primType, unsigned int minIndex, unsigned int maxIndex,
                               unsigned int indexCount, EGRTYPE indexType, void* indices );

//==========================================================
// Value queries.
extern int GrGetMaxTextureSize();
extern int GrGetMaxFramebufferSize();
extern int GrGetMaxAttribCount();
