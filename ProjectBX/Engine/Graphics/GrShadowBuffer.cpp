//----------------------------------------------------------
// File:        GrShadowBuffer.cpp
// Author:      Kevin Bray
// Created:     11-27-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrShadowBuffer.h"

// project includes.
#include "GrRenderTargetMgr.h"
#include "GrRenderTarget.h"
#include "GrRenderbuffer.h"
#include "GrTexture.h"
#include "GLSubsys.h"
#include "GrUtil.h"


//**********************************************************
// class GrShadowBuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrShadowBuffer::GrShadowBuffer( unsigned int width, unsigned int height )
: _prevRT( 0 )
{
    B_ASSERT( IsPow2( width ) && width <= 2048 );
    B_ASSERT( IsPow2( height ) && height <= 2048 );
    BuildRenderTarget( width, height );
}

//----------------------------------------------------------
GrShadowBuffer::~GrShadowBuffer()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrShadowBuffer::GetWidth() const
{
    return _renderTarget->GetWidth();
}

//----------------------------------------------------------
unsigned int
GrShadowBuffer::GetHeight() const
{
    return _renderTarget->GetHeight();
}

//----------------------------------------------------------
void
GrShadowBuffer::Bind()
{
    B_ASSERT( _prevRT == 0 );
    B_ASSERT( GrRenderTarget::GetCurrentRT() != _renderTarget );

    // save the current render target and bind our own.
    _prevRT = GrRenderTarget::GetCurrentRT();
    _renderTarget->Bind();
    B_ASSERT( _prevRT );

    // clear the current contents.
    GrDisable( GR_SCISSORTEST );
    GrSetWriteEnable( GR_DEPTH );
    GrClear();
}

//----------------------------------------------------------
URef< GrTexture >
GrShadowBuffer::Finish()
{
    B_ASSERT( GrRenderTarget::GetCurrentRT() == _renderTarget );
    B_ASSERT( _prevRT );

    CHECK_GL();

    URef< GrTexture > ret;
    if ( _copyDepthTex )
    {
        // copy the contents of the depth buffer to our depth texture.
        _copyDepthTex->Bind( 0 );
        bglCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, _copyDepthTex->GetWidth(), _copyDepthTex->GetHeight() );
        ret = _copyDepthTex;
        CHECK_GL();
    }
    else
    {
        // simply return the depth texture in the render target.
        B_ASSERT( _renderTarget->HasDepthTex() );
        ret = ( GrTexture* )( GrTextureBase* )_renderTarget->GetDepthTex();
    }

    // bind the previous render target.
    _prevRT->Bind();
    _prevRT = 0;

    return ret;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrShadowBuffer::BuildRenderTarget( unsigned int width, unsigned int height )
{
    // create the depth render texture.
    URef<GrTextureBase> depthTex = gGrRenderTargetMgr->CreateRendertexture( width, height, GrRenderTargetMgr::EF_DEPTH16,
        GrTextureBase::kNoMipMap | GrTextureBase::kNoCompress, GrTextureBase::ETT_2D );
    B_ASSERT( depthTex->GetType() == GrTextureBase::ETT_2D );

    // create a color render texture.
#if GR_OPTION_ALPHATEST_COLOR_WRITE
    URef<GrTextureBase> colorTex = gGrRenderTargetMgr->CreateRendertexture( width, height, GrRenderTargetMgr::EF_RGBA32,
        GrTextureBase::kNoMipMap | GrTextureBase::kNoCompress, GrTextureBase::ETT_2D );
    B_ASSERT( colorTex->GetType() == GrTextureBase::ETT_2D );

    // try to create the shadow buffer render target.
    _renderTarget = gGrRenderTargetMgr->CreateRenderTarget( colorTex, depthTex );
#else
    // try to create the shadow buffer render target.
    _renderTarget = gGrRenderTargetMgr->CreateRenderTarget( 0, ( GrTextureBase* )depthTex );
#endif

    if ( !_renderTarget )
    {
        // we must not support render to depth texture!
        // create a depth renderbuffer and a color render buffer.
        URef< GrRenderbuffer > colorBuf = gGrRenderTargetMgr->CreateRenderbuffer( width, height, GrRenderTargetMgr::EF_RGBA32 );
        URef< GrRenderbuffer > depthBuf = gGrRenderTargetMgr->CreateRenderbuffer( width, height, GrRenderTargetMgr::EF_DEPTH );

        // create the render target using a color and depth buffer.
        _renderTarget = gGrRenderTargetMgr->CreateRenderTarget( colorBuf, depthBuf );
        if ( !_renderTarget )
        {
            B_ERROR( "Unable to find a valid shadow buffer format!" );
        }

        // set the copy depth texture target.
        _copyDepthTex = ( GrTexture* )( GrTextureBase* )depthTex;
    }
}
