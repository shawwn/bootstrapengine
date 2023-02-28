//----------------------------------------------------------
// File:		GrRenderTargetMgr.cpp
// Author:		Kevin Bray
// Created:		08-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderTargetMgr.h"

// project includes.
#include "GrRenderTarget.h"
#include "GrRenderbuffer.h"
#include "GrTexture.h"
#include "GrTextureRect.h"
#include "GLSubsys.h"

GrRenderTargetMgr* gGrRenderTargetMgr = 0;

GrRenderbuffer::EFORMAT bufferFmtMap[ GrRenderTargetMgr::EF_COUNT ] =
{
	GrRenderbuffer::EF_RGBA,
	GrRenderbuffer::EF_RGBA,
	GrRenderbuffer::EF_RGBA,
	GrRenderbuffer::EF_RGBA,
	GrRenderbuffer::EF_RGBA,
	GrRenderbuffer::EF_RGBA16F,
	GrRenderbuffer::EF_RGBA32F,
	GrRenderbuffer::EF_DEPTH,
	GrRenderbuffer::EF_DEPTH16,
	GrRenderbuffer::EF_DEPTH24,
	GrRenderbuffer::EF_DEPTHSTENCIL,
	GrRenderbuffer::EF_DEPTH24STENCIL8,
	GrRenderbuffer::EF_RGBA,
};

GrTextureBase::EFORMAT textureFmtMap[ GrRenderTargetMgr::EF_COUNT ] =
{
	GrTextureBase::EF_RGB16,
	GrTextureBase::EF_RGB24,
	GrTextureBase::EF_RGB15_A1,
	GrTextureBase::EF_RGBA32,
	GrTextureBase::EF_RGB48F,
	GrTextureBase::EF_RGBA64F,
	GrTextureBase::EF_RGBA128F,
	GrTextureBase::EF_DEPTH,
	GrTextureBase::EF_DEPTH,
	GrTextureBase::EF_DEPTH,
	GrTextureBase::EF_DEPTHSTENCIL,
	GrTextureBase::EF_DEPTHSTENCIL,
	GrTextureBase::EF_I8,
};

//**********************************************************
// class GrRenderTargetMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderTargetMgr::GrRenderTargetMgr( unsigned int primaryWidth, unsigned int primaryHeight )
{
	B_ASSERT( gGrRenderTargetMgr == 0 );
	gGrRenderTargetMgr = this;

	// create the primary render target representation.
	_primaryRT = new GrRenderTarget( primaryWidth, primaryHeight );
}

//----------------------------------------------------------
GrRenderTargetMgr::~GrRenderTargetMgr()
{
	// ensure that no render-targets are still in existance.
	gGrRenderTargetMgr = 0;
	CollectGarbage();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrRenderbuffer >
GrRenderTargetMgr::CreateRenderbuffer( unsigned int width, unsigned int height, EFORMAT format )
{
	// create a new renderbuffer, add it to our list of render buffers, and return.
	URef< GrRenderbuffer > newRB = new GrRenderbuffer( width, height, bufferFmtMap[ format ] );
	_renderBuffers.AddItem( newRB );
	return newRB;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrRenderTargetMgr::CreateRendertexture( unsigned int width, unsigned int height, EFORMAT format, GrTextureBase::Flags flags,
										GrTextureBase::ETEXTURETYPE type )
{
	// make sure our flags are valid.
	flags &= ~GrTextureBase::kBorder;
	flags |= GrTextureBase::kNoCompress;

	// create a new texture.
	URef< GrTextureBase > newTex;
	if ( type == GrTextureBase::ETT_CUBE )
	{
		// make sure the cubemap dimensions are valid.
		B_ASSERT( width == height );
		B_ASSERT( IsPow2( width ) );
		newTex = new GrTextureCube( "rendertexture", textureFmtMap[ format ], width, flags );
	}
	else if ( type == GrTextureBase::ETT_2D )
	{
		// simply create a texture with the parameters specified.
		newTex = new GrTexture( "rendertexture", textureFmtMap[ format ], width, height, flags );
	}
	else if ( type == GrTextureBase::ETT_RECT )
	{
		// simply create a texture with the parameters specified.
		newTex = new GrTextureRect( "rendertexture", textureFmtMap[ format ], width, height );		
	}

	// add the new texture to our list of render textures.
	_renderTextures.AddItem( newTex );

	// return the new texture.
	return newTex;
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::CreateRenderTarget( URef< GrTextureBase > colorTarget )
{
	B_ASSERT( colorTarget != 0 );
	URef< GrRenderTarget > newRT = new GrRenderTarget( &colorTarget, URef< GrRenderbuffer >( 0 ), 1 );
	return AddToRTList( newRT );
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::CreateRenderTarget( URef< GrTextureBase > colorTarget, URef< GrRenderbuffer > depthBuffer )
{
	URef< GrRenderTarget > newRT = new GrRenderTarget( &colorTarget, depthBuffer, colorTarget != 0 ? 1 : 0 );
	return AddToRTList( newRT );
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::CreateRenderTarget( URef< GrTextureBase > colorTarget, URef< GrTextureBase > depthTex )
{
	URef< GrRenderTarget > newRT = new GrRenderTarget( &colorTarget, depthTex, colorTarget != 0 ? 1 : 0 );
	return AddToRTList( newRT );
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::CreateRenderTarget( URef< GrTextureBase >* colorTargets, URef< GrRenderbuffer > depthTex,
									   unsigned int colorTargetCount)
{
	URef< GrRenderTarget > newRT = new GrRenderTarget( colorTargets, depthTex, colorTargetCount );
	return AddToRTList( newRT );
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::CreateRenderTarget( URef< GrRenderbuffer > colorBuffer, URef< GrRenderbuffer > depthBuffer )
{
	URef< GrRenderTarget > newRT = new GrRenderTarget( colorBuffer, depthBuffer );
	return AddToRTList( newRT );
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::GetPrimaryRenderTarget()
{
	return _primaryRT;
}

//----------------------------------------------------------
void
GrRenderTargetMgr::CollectGarbage()
{
	// perform garbage collection.
	_renderTargets.CollectGarbage();
	_renderBuffers.CollectGarbage();
	_renderTextures.CollectGarbage();
}

//----------------------------------------------------------
void
GrRenderTargetMgr::PreContextReset()
{
	// cannot reset *during* rendering!  The display window must be the target when rendering
	// is not occuring.
	B_ASSERT( GrRenderTarget::GetCurrentRT() == GetPrimaryRenderTarget() );
	_primaryRT->PreContextReset();

	// collect garbage.
	CollectGarbage();

	// notify render targets.
	for ( unsigned int i = 0; i < _renderTargets.GetElemCount(); ++i )
		_renderTargets[ i ]->PreContextReset();

	// notify render buffers.
	for ( unsigned int i = 0; i < _renderBuffers.GetElemCount(); ++i )
		_renderBuffers[ i ]->PreContextReset();

	// notify render textures.
	for ( unsigned int i = 0; i < _renderTextures.GetElemCount(); ++i )
		_renderTextures[ i ]->PreContextReset();
}

//----------------------------------------------------------
void
GrRenderTargetMgr::PostContextReset()
{
	// recreate our buffers.
	for ( unsigned int i = 0; i < _renderBuffers.GetElemCount(); ++i )
		_renderBuffers[ i ]->PostContextReset();

	// recreate our textures.
	for ( unsigned int i = 0; i < _renderTextures.GetElemCount(); ++i )
		_renderTextures[ i ]->PostContextReset();

	// tell the render target to recreate itself.
	for ( unsigned int i = 0; i < _renderTargets.GetElemCount(); ++i )
		_renderTargets[ i ]->PostContextReset();

	_primaryRT->PostContextReset();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
URef< GrRenderTarget >
GrRenderTargetMgr::AddToRTList( URef< GrRenderTarget > target )
{
	if ( target->IsValid() )
		_renderTargets.AddItem( target );
	else
		target = 0;
	return target;
}
