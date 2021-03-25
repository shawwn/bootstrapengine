//----------------------------------------------------------
// File:		GrRenderTarget.h
// Author:		Kevin Bray
// Created:		08-20-05
//
// Purpose:		To manage an OpenGL framebuffer object.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"
#include "UPath.h"
#include "URect.h"
#include "GrTextureCube.h"
#include "GrTexture.h"

// forward declarations.
class GrRenderbuffer;
class IXRenderable;		// IX = internal only.

//**********************************************************
// class GrRenderTarget
//**********************************************************
class GrRenderTarget : public URefCount
{
public:
	static const unsigned int kMaxRTs = 4;

	enum ECOLORFORMAT
	{
		ECF_RGB24,
		ECF_RGBA32,
		ECF_RGB48F,
		ECF_RGBA64F,
		ECF_NONE,
	};

	enum EDEPTHFORMAT
	{
		EDF_DEPTH,
		EDF_DEPTHSTENCIL,
	};

	// texture render target.  Note: use the render target manager to create render targets.
	GrRenderTarget( unsigned int width, unsigned int height );
	GrRenderTarget( URef<GrTextureBase>* colorTargets, URef< GrRenderbuffer > depthBuffer, unsigned int colorTargetCount );
	GrRenderTarget( URef<GrTextureBase>* colorTargets, URef< GrTextureBase > depthTexture, unsigned int colorTargetCount );
	GrRenderTarget( URef<GrRenderbuffer> colorBuffer, URef< GrRenderbuffer > depthBuffer );
//	GrRenderTarget( const UPath& name, URef< GrTextureBase >* colorTargets, URef< GrTextureBase > depthTexture,
//					unsigned int colorTargetCount );
//	GrRenderTarget( const UPath& name, URef< GrRenderbuffer >* colorBuffers, URef< GrTextureBase > depthTexture,
//					unsigned int colorTargetCount );

	~GrRenderTarget();

	// binds back to the primary render target.
	static GrRenderTarget*		GetCurrentRT();
	static void					ResizePrimaryRT( unsigned int width, unsigned int height );

	// render target management.
	unsigned int				GetWidth() const						{	return _width;				}
	unsigned int				GetHeight() const						{	return _height;				}

	bool						IsValid() const							{	return _isValid;			}
	bool						IsCubeTarget() const					{	return _cubeTarget;			}

	bool						HasColorTex();
	unsigned int				GetColorTexCount() const				{	return _targetCount;		}
	URef< GrTextureBase >		GetColorTex( unsigned int idx ) const;

	bool						HasDepthTex();
	URef< GrTextureBase >		GetDepthTex() const;

	// sets the viewport rectangle associated with the render target.
	void						SetViewportRect( const URect& viewportRect );

	// binds this render target.
	void						Bind( bool setVP = true );
	void						SetActiveFace( GrTextureCube::ECUBEFACE face );

	// context reset notification.
	void						PreContextReset();
	void						PostContextReset();

private:
	void						GetTextureDim( unsigned int& width, unsigned int& height, URef< GrTextureBase > texture );
	void						SetColorTargets( URef< GrTextureBase >* colorTargets, unsigned int colorTargetCount );
	IXRenderable*				CreateRenderable( URef< GrTextureBase > texture );
	void						LoseBind();
	void						CreateFBO();
	void						DestroyFBO();
	void						BindRT();

	// current render target bound.
	static GrRenderTarget*		_curBound;
	static GrRenderTarget*		_primaryRT;

	// render target data.
	unsigned int				_width;
	unsigned int				_height;
	unsigned int				_targetCount;
	URect						_viewportRect;
	IXRenderable*				_colorRenderables[ kMaxRTs ];
	IXRenderable*				_depthRenderable;
	IXRenderable*				_stencilRenderable;
	unsigned int				_fbo;
	GrTextureCube::ECUBEFACE	_curRenderFace;
	bool						_cubeTarget;
	bool						_autoGenMipmaps;
	bool						_isValid;
	bool						_isPrimary;
};
