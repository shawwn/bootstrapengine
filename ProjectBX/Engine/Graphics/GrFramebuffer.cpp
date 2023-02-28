//----------------------------------------------------------
// File:		GrFramebuffer.cpp
// Author:		Kevin Bray
// Created:		06-18-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrFramebuffer.h"

// project includes.
#include "GrTexture.h"
#include "GrTextureRect.h"
#include "GrRenderbuffer.h"
#include "GrRenderTarget.h"
#include "GrRenderTargetMgr.h"
#include "GrConfig.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrTextureMgr.h"
#include "GrShader.h"
#include "GrShaderMgr.h"
#include "GrRenderUtil.h"

#include <mmsystem.h>

// other OpenGL includes.
#include <gl/glu.h>

// file local function to cast from GrTextureBase* to GrTexture*
static GrTextureRect* GetTextureRect( GrTextureBase* texture );

GrFramebuffer* gGrFramebuffer = 0;

#define PP_SCALE_ST					0
#define PP_OFFSET_ST				1
#define PP_BLUR_DIR					2
#define PP_BLOOM_START				3
#define PP_BLOOM_STRENGTH			4
#define PP_HIPASS_OFFSET			5
#define PP_LUM_OFFSET				2

#define PP_CURRENDER_OFFSET_ST		1
#define PP_BLUR_OFFSET_ST			2
#define PP_BLUR_SCALE_ST			3

// user param handles for HDR shaders.
const unsigned int kTexCoordScaleHandle = 1;
const unsigned int kBlurVecHandle = 2;

// blur vectors.
float kHorzVec[4] = {	1.0f, 0.0f, 0.0f, 0.0f	};
float kVertVec[4] = {	0.0f, 1.0f, 0.0f, 0.0f	};

//**********************************************************
// class GrFramebuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrFramebuffer::GrFramebuffer( unsigned int width, unsigned int height, EGRQUALITY quality, bool hdrEnable )
: _width( width )
, _height( height )
, _quality( quality )
, _hdrFormat( GrRenderTargetMgr::EF_RGBA64F )
, _hdrEnable( hdrEnable )
{
	// make sure we haven't already been created, then set the
	// singleton pointer.
	B_ASSERT( gGrFramebuffer == 0 );
	gGrFramebuffer = this;

	// create our render targets.
	AllocResources();
}

//----------------------------------------------------------
GrFramebuffer::~GrFramebuffer()
{
	gGrFramebuffer = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrRenderTargetMgr::EFORMAT
GrFramebuffer::GetColorFormat() const
{
	// get the format.
	if ( _hdrRT && _hdrEnable )
		return _hdrFormat;

	// front buffer format.
	return GrRenderTargetMgr::EF_RGBA32;
}

//----------------------------------------------------------
GrRenderTargetMgr::EFORMAT
GrFramebuffer::GetDepthFormat() const
{
	return GrRenderTargetMgr::EF_DEPTH24STENCIL8;
}

//----------------------------------------------------------
unsigned int
GrFramebuffer::GetCurRenderWidth() const
{
	return GetTextureRect( _currentRender )->GetWidth();
}

//----------------------------------------------------------
unsigned int
GrFramebuffer::GetCurRenderHeight() const
{
	return GetTextureRect( _currentRender )->GetHeight();
}

//----------------------------------------------------------
void
GrFramebuffer::BeginFrame()
{
	// bind the primary target.
	if ( _hdrRT && _hdrEnable )
		_hdrRT->Bind();
	else
		_mainRT->Bind();
}

//----------------------------------------------------------
void
GrFramebuffer::EndFrame( unsigned int time )
{
	// do we need to process the HDR render target?
	if ( _hdrRT && _hdrEnable )
	{
		// process the HDR info.
		ToneMap( time - _lastTime );
	}

	// update the last time.
	_lastTime = time;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrFramebuffer::CopyCurrentRender( int srcX, int srcY, int width, int height )
{
	B_ASSERT( srcX >= 0 && srcY >= 0 && width > 0 && height > 0 );

	// bind the texture to texture unit 0.
	_currentRender->Bind( 0 );
	CHECK_GL();

	// copy the contents of the framebuffer to the current-render texture.
	GrTextureRect* currentRender = GetTextureRect( _currentRender );
	B_ASSERT( currentRender->GetWidth() >= ( unsigned int )width );
	B_ASSERT( currentRender->GetHeight() >= ( unsigned int )height );

	bglCopyTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, srcX, srcY, width, height );
	CHECK_GL();

	// return the current render texture.
	return _currentRender;
}

//----------------------------------------------------------
URef< GrTextureBase	>
GrFramebuffer::CopyCurrentDepth( int srcX, int srcY, int width, int height )
{
	B_ASSERT( srcX >= 0 && srcY >= 0 && width > 0 && height > 0 );

	// bind the texture to texture unit 0.
	_currentDepth->Bind( 0 );
	CHECK_GL();

	// copy the contents of the framebuffer to the current-render texture.
	GrTextureRect* currentDepth = GetTextureRect( _currentDepth );
	B_ASSERT( currentDepth->GetWidth() >= ( unsigned int )width );
	B_ASSERT( currentDepth->GetHeight() >= ( unsigned int )height );

	bglCopyTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, srcX, srcY, width, height );
	CHECK_GL();

	// return the current render texture.
	return _currentDepth;
}

//----------------------------------------------------------
void
GrFramebuffer::PreContextReset()
{
	// free them.
	_currentRender = 0;
	_currentDepth = 0;
	_hdrRT = 0;
	_mainRT = 0;
}

//----------------------------------------------------------
void
GrFramebuffer::PostContextReset( unsigned int width, unsigned int height )
{
	// reset the width and height.
	_width = width;
	_height = height;

	// reallocate resources.
	AllocResources( false );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrFramebuffer::ToneMap( unsigned int delta )
{
	// make sure the square texture is actually square and that it's a
	// power of 2.
	B_ASSERT( IsPow2( _squareRT->GetWidth() ) );
	B_ASSERT( _squareRT->GetWidth() == _squareRT->GetHeight() );

	// setup state for performing our post processing work.
	GrSetState( 0 );
	GrSetWriteEnable( GR_RGBA );

	// downsample the image to a square texture taking the log_e (ln(x)) of
	// the intensity of every pixel.
	ProcessRT( _squareRT, _hdrLogAvgShader, _hdrRT, GL_LINEAR );

	// ping pong back and forth using filtering to average the log_e of the
	// luminance.
	GrRenderTarget* src = _squareRT;
	GrRenderTarget* dst = _halfSquareRT;
	unsigned int dim = _squareRT->GetWidth();

	// downsample taking the average.
	unsigned int i = 1;
	while ( dim > 1 )
	{
		// downsample from the source to the destination.
		ProcessRT( dst, _hdrPassThruShader, src, GL_LINEAR, i & ~1, ( i - 1 ) & ~1 );

		// swap src and dst, divide dim by 2.
		Swap( src, dst );
		dim >>= 1;

		// next.
		++i;
	}

	// bind the source luminance as the input.
	GrTextureBase* lumTex = src->GetColorTex( 0 );
	B_ASSERT( lumTex );
	lumTex->Bind( 1, GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP );

	// tone map the HDR data.
	ProcessRT( _hdrScratch, _hdrToneMapShader, _hdrRT, GL_NEAREST );

	// process the tone mapped HDR buffer with a high pass filter into a
	// half resolution offscreen buffer that we can blur.
	ProcessRT( _downSampleRT, _hdrBrightPassShader, _hdrScratch, GL_LINEAR );

	// blur the downsampled image into the HDR buffer.
	_hdrBlurShader->Bind( _quality );

	// perform the star.

	// perform the horizontal bloom pass.
	_hdrBlurShader->SetUserParam4fv( kBlurVecHandle, kVertVec );
	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 0, 0 );

	// perform the vertical bloom pass and modulate with the horizontal results.
	_hdrBlurShader->SetUserParam4fv( kBlurVecHandle, kHorzVec );
	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 0 );

//	_hdrBlurShader->SetUserParam4fv( kBlurVecHandle, kVertVec );
//	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 1, 0 );
//	_hdrBlurShader->SetUserParam4fv( kBlurVecHandle, kHorzVec );
//	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 1 );

//	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 1, 0 );
//	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 1 );
//	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 1, 0 );
//	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 1 );
//	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 1, 0 );
//	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 1 );
//	ProcessRT( _downSampleRT2, _hdrBlurShader, _downSampleRT, GL_LINEAR, 1, 0 );
//	ProcessRT( _downSampleRT, _hdrBlurShader, _downSampleRT2, GL_LINEAR, 0, 1 );

	_mainRT->Bind();

	// bind the bloom texture as input.
	BindRTRectTex( 1, GL_LINEAR, _downSampleRT );
//	GrTextureBase* bloomTex = _downSampleRT->GetColorTex( 0 );
//	B_ASSERT( bloomTex );
//	bloomTex->Bind( 1, GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP );

	// bind the star texture as input.


	// add the bloom in.
	ProcessRT( _mainRT, _hdrCombineShader, _hdrScratch, GL_NEAREST );
}

//----------------------------------------------------------
void
GrFramebuffer::ProcessRT( GrRenderTarget* destRT, GrShader* shader, GrRenderTarget* sourceRT, GLenum filter,
						  unsigned int vpSizeShift, unsigned int srcShift )
{
	// make sure that the source and destination are different.
	B_ASSERT( sourceRT != destRT );

	// bind the target.
	destRT->Bind( vpSizeShift == 0 );
	if ( vpSizeShift )
		GrViewport( 0, 0, destRT->GetWidth() >> vpSizeShift, destRT->GetHeight() >> vpSizeShift );

	// bind the shader.
	shader->Bind( _quality );

	// set the texture coordinate scale.
	SetTexCoordScale( shader, kTexCoordScaleHandle, sourceRT, srcShift );

	// bind the source render target as a texture.
	BindRTRectTex( 0, filter, sourceRT );
//	GrTextureBase* srcTex = sourceRT->GetColorTex( 0 );
//	B_ASSERT( srcTex );
//	srcTex->Bind( 0, filter, filter, GL_CLAMP, GL_CLAMP );

	// render a screen aligned quad.
	gGrRenderUtil->DrawScreenQuad();
//	GrDrawScreenQuad( GR_ATTRIB_TEXCOORD_INDEX );
}

//----------------------------------------------------------
void
GrFramebuffer::BindRTRectTex( GLenum texUnit, GLenum filter, URef< GrRenderTarget > renderTarget )
{
	B_ASSERT( texUnit < 16 );
	GrTextureBase* srcTex = renderTarget->GetColorTex( 0 );
	B_ASSERT( srcTex );
	srcTex->Bind( texUnit, filter, filter, GL_CLAMP, GL_CLAMP );
}

//----------------------------------------------------------
void
GrFramebuffer::SetTexCoordScale( GrShader* shader, unsigned int paramHandle, GrRenderTarget* sourceRT,
								 unsigned int sizeShift )
{
	// set the params.
	float kUVScale[ 4 ] = {
		( float )( sourceRT->GetWidth() >> sizeShift ),
		( float )( sourceRT->GetHeight() >> sizeShift ),
		0.0f,
		0.0f
	};
	shader->SetUserParam4fv( paramHandle, kUVScale );
}

//----------------------------------------------------------
void
GrFramebuffer::AllocResources( bool allocShaders )
{
	B_ASSERT( _currentRender == 0 );

	// maximum screen resolution.
	static unsigned int kMaxRes = 2048;

	// get the primary render target.
	_mainRT = gGrRenderTargetMgr->GetPrimaryRenderTarget();

	// if the render target size is too small, then we need to shut off
	// HDR.
	bool hdrEnable = _hdrEnable;
	if ( _width < 4 || _height < 4 )
		hdrEnable = false;

	// calculate the current render texture width.
	unsigned int widthPow2 = FindPow2( _width );
	unsigned int heightPow2 = FindPow2( _height );

	// try to create the main HDR render target.
	if ( hdrEnable )
		_hdrRT = CreateHDRRenderTarget( _width, _height, true );

	if ( _hdrRT && hdrEnable )
	{
		// create all render targets required for HDR rendering.
		{
			unsigned int squareDim = Max( widthPow2, heightPow2 );
			_hdrScratch		= CreateHDRRenderTarget( _width, _height, false );
			_downSampleRT	= CreateHDRRenderTarget( _width >> 1, _height >> 1, false );
			_downSampleRT2	= CreateHDRRenderTarget( _width >> 1, _height >> 1, false );
			_squareRT		= CreateHDRRenderTarget( squareDim >> 1, squareDim >> 1, false );
			_halfSquareRT	= CreateHDRRenderTarget( squareDim >> 2, squareDim >> 2, false );
		}

		// create "current render" textures.
		{
			// create the current color texture.
			_currentRender = gGrRenderTargetMgr->CreateRendertexture( _width, _height,
				GrRenderTargetMgr::EF_RGBA64F, GrTexture::kNoMipMap, GrTextureBase::ETT_RECT );
			CHECK_GL();

			// create the current depth texture.
			_currentDepth = gGrRenderTargetMgr->CreateRendertexture( _width, _height,
					GrRenderTargetMgr::EF_DEPTH, GrTexture::kNoMipMap, GrTextureBase::ETT_RECT );
			CHECK_GL();
		}

		// load HDR shaders.
		if ( allocShaders )
		{
			// pass through shader.
			_hdrPassThruShader = gGrShaderMgr->GetShader( "HDR/PassThru" );
			_hdrPassThruShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrPassThruShader->BindUserSampler( "s_CurrentRender", 0 );

			// luminance calculation.
			_hdrLogAvgShader = gGrShaderMgr->GetShader( "HDR/CalcLum" );
			_hdrLogAvgShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrLogAvgShader->BindUserSampler( "s_CurrentRender", 0 );

			// tone mapping shader.
			_hdrToneMapShader = gGrShaderMgr->GetShader( "HDR/ToneMap" );
			_hdrToneMapShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrToneMapShader->BindUserSampler( "s_CurrentRender", 0 );
			_hdrToneMapShader->BindUserSampler( "s_CurrentLum", 1 );

			// high pass shader.
			_hdrBrightPassShader = gGrShaderMgr->GetShader( "HDR/BrightPass" );
			_hdrBrightPassShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrBrightPassShader->BindUserSampler( "s_CurrentRender", 0 );

			// blur shader.
			_hdrBlurShader = gGrShaderMgr->GetShader( "HDR/Blur" );
			_hdrBlurShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrBlurShader->InitUserParam( "u_BlurVec", kBlurVecHandle );
			_hdrBlurShader->BindUserSampler( "s_CurrentRender", 0 );

			// combine shader.
			_hdrCombineShader = gGrShaderMgr->GetShader( "HDR/Combine" );
			_hdrCombineShader->InitUserParam( "u_TexCoordScale", kTexCoordScaleHandle );
			_hdrCombineShader->BindUserSampler( "s_CurrentRender", 0 );
			_hdrCombineShader->BindUserSampler( "s_CurrentBloom", 1 );
		}
	}
	else
	{
		// create the current color texture.
		_currentRender = gGrRenderTargetMgr->CreateRendertexture( _width, _height,
			GrRenderTargetMgr::EF_RGBA32, GrTexture::kNoMipMap, GrTextureBase::ETT_RECT );
		CHECK_GL();

		// create the current depth texture.
		_currentDepth = gGrRenderTargetMgr->CreateRendertexture( _width, _height,
				GrRenderTargetMgr::EF_DEPTH, GrTexture::kNoMipMap, GrTextureBase::ETT_RECT );
		CHECK_GL();
	}
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrFramebuffer::CreateHDRRenderTarget( unsigned int width, unsigned int height, bool depthBuffer )
{
	// create the color target.
	URef< GrTextureBase > hdrTex = gGrRenderTargetMgr->CreateRendertexture( width, height,
		GrRenderTargetMgr::EF_RGBA64F, GrTexture::kNoMipMap, GrTextureBase::ETT_RECT );

	// create the depth buffer if necessary.
	URef< GrRenderbuffer > depth = 0;
	if ( depthBuffer )
		depth = gGrRenderTargetMgr->CreateRenderbuffer( width, height, GrRenderTargetMgr::EF_DEPTHSTENCIL );

	// create the render target and return it.
	URef< GrRenderTarget > newTarget = gGrRenderTargetMgr->CreateRenderTarget( hdrTex, depth );
	return newTarget;
}


//**********************************************************
// file local functions
//**********************************************************

//----------------------------------------------------------
GrTextureRect*
GetTextureRect( GrTextureBase* texture )
{
	B_ASSERT( texture->GetType() == GrTextureBase::ETT_RECT );
	return ( ( GrTextureRect* )( GrTextureBase* )texture );
}
