//----------------------------------------------------------
// File:		GrRenderTargetMgr.h
// Author:		Kevin Bray
// Created:		08-20-05
//
// Purpose:		To manage render targets.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefArray.h"
#include "UFastArray.h"
#include "GrTextureBase.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrRenderTarget;
class GrRenderbuffer;
class GrTexture;

//**********************************************************
// class GrRenderTargetMgr
//**********************************************************
class GrRenderTargetMgr
{
public:
	enum EFORMAT
	{
		EF_RGB16,					// not supported for renderbuffers.
		EF_RGB24,					// not supported for renderbuffers.
		EF_RGBA16,					// not supported for renderbuffers.
		EF_RGBA32,
		EF_RGB48F,					// not supported for renderbuffers.
		EF_RGBA64F,					// not supported for renderbuffers.
		EF_RGBA128F,				// not supported for renderbuffers.
		EF_DEPTH,
		EF_DEPTH16,
		EF_DEPTH24,
		EF_DEPTHSTENCIL,			// currently not supported by GL drivers.
		EF_DEPTH24STENCIL8,
		EF_I8,						// currently not supported by hardware as a target.
		EF_COUNT
	};

	GrRenderTargetMgr( unsigned int primaryWidth, unsigned int primaryHeight );
	~GrRenderTargetMgr();

	// rendertarget component creation.
	URef< GrRenderbuffer >	CreateRenderbuffer( unsigned int width, unsigned int height, EFORMAT format );
	URef< GrTextureBase >	CreateRendertexture( unsigned int width, unsigned int height, EFORMAT format, GrTextureBase::Flags flags,
												 GrTextureBase::ETEXTURETYPE type );

	// create a render target.
	URef< GrRenderTarget >	CreateRenderTarget( URef< GrTextureBase > colorTarget );
	URef< GrRenderTarget >	CreateRenderTarget( URef< GrTextureBase > colorTarget, URef< GrRenderbuffer > depthBuffer );
	URef< GrRenderTarget >	CreateRenderTarget( URef< GrTextureBase > colorTarget, URef< GrTextureBase > depthTarget );
	URef< GrRenderTarget >	CreateRenderTarget( URef< GrTextureBase >* colorTargets, URef< GrRenderbuffer > depthTarget,
												unsigned int colorTargetCount );
	URef< GrRenderTarget >	CreateRenderTarget( URef< GrRenderbuffer > colorBuffer, URef< GrRenderbuffer > depthBuffer );

	// primary render target management.
	URef< GrRenderTarget >	GetPrimaryRenderTarget();

	// force garbage collection.
	void					CollectGarbage();

	// context reset notification.
	void					PreContextReset();
	void					PostContextReset();

private:
	typedef URefArray< GrRenderbuffer >		RenderBufferArray;
	typedef URefArray< GrTextureBase >		RenderTextureArray;
	typedef URefArray< GrRenderTarget >		RenderTargetArray;

	URef< GrRenderTarget >	AddToRTList( URef< GrRenderTarget > target );

	URef< GrRenderTarget >	_primaryRT;
	RenderTextureArray		_renderTextures;
	RenderBufferArray		_renderBuffers;
	RenderTargetArray		_renderTargets;
};
extern GrRenderTargetMgr* gGrRenderTargetMgr;
