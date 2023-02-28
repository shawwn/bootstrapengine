//----------------------------------------------------------
// File:		GrRenderTarget.cpp
// Author:		Kevin Bray
// Created:		08-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderTarget.h"

// project includes.
#include "GrTextureMgr.h"
#include "GrTexture.h"
#include "GrRenderbuffer.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrTextureRect.h"
#include "GrSubsys.h"


// primary render target.
GrRenderTarget*
GrRenderTarget::_primaryRT = 0;

// currently bound render target.
GrRenderTarget*
GrRenderTarget::_curBound = 0;

//**********************************************************
// class IXRenderable implemenation.
//**********************************************************
class IXRenderable
{
public:
	enum ETYPE
	{
		ET_TEXTURE,
		ET_TEXTURECUBE,
		ET_TEXTURERECT,
		ET_RENDERBUFFER,
	};

	IXRenderable() { }
	virtual ~IXRenderable() { }

	// must implement.
	virtual ETYPE					GetType() const = 0;
	virtual void					BindToFramebuffer( unsigned int fbo, GrTextureCube::ECUBEFACE face ) = 0;

	// optional function implementations.
	virtual URef< GrTextureBase >	GetTexture() const		{	return 0;								}
	virtual void					UnbindTexture()	const	{ }
	virtual void					AutoGenMipmaps() const	{ }
};


//**********************************************************
// class IXTextureRenderable implemenation.
//**********************************************************
class IXTextureRenderable : public IXRenderable
{
public:
	IXTextureRenderable( URef< GrTexture > texture );
	~IXTextureRenderable() { }

	ETYPE							GetType() const			{	return ET_TEXTURE;						}
	void							BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face );
	URef<GrTextureBase>				GetTexture() const		{	return ( GrTexture* )_texture;			}
	void							UnbindTexture()	const	{	_texture->Unbind();						}
	void							AutoGenMipmaps() const;

private:
	URef<GrTexture>					_texture;
	bool							_genMipmaps;
};

//----------------------------------------------------------
IXTextureRenderable::IXTextureRenderable( URef< GrTexture > texture )
: _texture( texture )
, _genMipmaps( false )
{
	_genMipmaps = ( ~texture->GetFlags() & GrTextureBase::kNoMipMap ) != 0;
}

//----------------------------------------------------------
void
IXTextureRenderable::BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face )
{
	_texture->BindToFramebuffer( attachment );
}

//----------------------------------------------------------
void
IXTextureRenderable::AutoGenMipmaps() const
{
	if ( _genMipmaps )
		_texture->AutoGenMipmaps();
}


//**********************************************************
// class IXTextureCubeRenderable implemenation.
//**********************************************************
class IXTextureCubeRenderable : public IXRenderable
{
public:
	IXTextureCubeRenderable( URef< GrTextureCube > textureCube );
	~IXTextureCubeRenderable() { }

	ETYPE							GetType() const			{	return ET_TEXTURECUBE;					}
	void							BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face );
	URef< GrTextureBase >			GetTexture() const		{	return ( GrTextureCube* )_texture;		}
	void							UnbindTexture()	const	{	_texture->Unbind();						}
	void							AutoGenMipmaps() const;

private:
	URef< GrTextureCube >			_texture;
	bool							_genMipmaps;
};

//----------------------------------------------------------
IXTextureCubeRenderable::IXTextureCubeRenderable( URef< GrTextureCube > textureCube )
: _texture( textureCube )
, _genMipmaps( false )
{
	_genMipmaps = ( ~textureCube->GetFlags() & GrTextureBase::kNoMipMap ) != 0;
}

//----------------------------------------------------------
void
IXTextureCubeRenderable::BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face )
{
	_texture->BindToFramebuffer( attachment, face );
}

//----------------------------------------------------------
void
IXTextureCubeRenderable::AutoGenMipmaps() const
{
	if ( _genMipmaps )
		_texture->AutoGenMipmaps();
}


//**********************************************************
// class IXTextureRectRenderable implemenation.
//**********************************************************
class IXTextureRectRenderable : public IXRenderable
{
public:
	IXTextureRectRenderable( URef< GrTextureRect > texture );
	~IXTextureRectRenderable() { }

	ETYPE							GetType() const			{	return ET_TEXTURERECT;					}
	void							BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face );
	URef<GrTextureBase>				GetTexture() const		{	return ( GrTextureRect* )_textureRect;	}
	void							UnbindTexture()	const	{	_textureRect->Unbind();					}
	void							AutoGenMipmaps() const	{ }

private:
	URef< GrTextureRect >			_textureRect;
};

//----------------------------------------------------------
IXTextureRectRenderable::IXTextureRectRenderable( URef< GrTextureRect > textureRect )
: _textureRect( textureRect )
{

}

//----------------------------------------------------------
void
IXTextureRectRenderable::BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face )
{
	_textureRect->BindToFramebuffer( attachment );
}


//**********************************************************
// class IXBufferRenderable implemenation.
//**********************************************************
class IXBufferRenderable : public IXRenderable
{
public:
	IXBufferRenderable( URef< GrRenderbuffer > renderBuffer ) : _renderBuffer( renderBuffer ) { }
	~IXBufferRenderable() { }

	ETYPE							GetType() const		{	return ET_RENDERBUFFER;					}
	void							BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face );

private:
	URef< GrRenderbuffer >			_renderBuffer;
};

//----------------------------------------------------------
void
IXBufferRenderable::BindToFramebuffer( unsigned int attachment, GrTextureCube::ECUBEFACE face )
{
	_renderBuffer->BindToFramebuffer( attachment );
}


//**********************************************************
// class GrRenderTarget
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderTarget::GrRenderTarget( unsigned int width, unsigned int height )
: _width( width )
, _height( height )
, _viewportRect( width, height )
, _targetCount( 0 )
, _depthRenderable( 0 )
, _stencilRenderable( 0 )
, _fbo( 0 )
, _curRenderFace( GrTextureCube::CF_NEGATIVEZ )
, _cubeTarget( false )
, _autoGenMipmaps( false )
, _isValid( true )
, _isPrimary( true )
{
	// we're the primary render target!
	B_ASSERT( _curBound == 0 );
	B_ASSERT( _primaryRT == 0 );
	_curBound = this;
	_primaryRT = this;
}

//----------------------------------------------------------
GrRenderTarget::GrRenderTarget( URef< GrTextureBase >* colorTargets, URef< GrRenderbuffer > depthBuffer,
							   unsigned int colorTargetCount )
: _width( 0 )
, _height( 0 )
, _viewportRect( 0, 0 )
, _targetCount( colorTargetCount )
, _depthRenderable( 0 )
, _stencilRenderable( 0 )
, _fbo( 0 )
, _curRenderFace( GrTextureCube::CF_NEGATIVEZ )
, _cubeTarget( false )
, _autoGenMipmaps( false )
, _isValid( true )
, _isPrimary( false )
{
	// 4 is a hardware limitation.
	B_ASSERT( colorTargetCount < kMaxRTs );
	B_ASSERT( colorTargetCount > 0 || depthBuffer != 0 );

	CHECK_GL();

	// initialize color renderables.
	MemSet( _colorRenderables, 0, sizeof( _colorRenderables ) );
	if ( colorTargetCount > 0 )
	{
		SetColorTargets( colorTargets, colorTargetCount );
		_cubeTarget = colorTargets[ 0 ]->GetType() == GrTextureBase::ETT_CUBE;
	}
	else if ( depthBuffer != 0 )
	{
		_width = depthBuffer->GetWidth();
		_height = depthBuffer->GetHeight();
		_cubeTarget = false;
	}

	// initialize the viewport rectangle.
	_viewportRect.SetWidth( _width );
	_viewportRect.SetHeight( _height );

	// create the depth/stencil renderables.
	if ( depthBuffer != 0 )
	{
		// make sure the depth buffer has valid dimensions.
		B_ASSERT( depthBuffer->GetWidth() == _width && depthBuffer->GetHeight() == _height );
		_depthRenderable = new IXBufferRenderable( depthBuffer );
		CHECK_GL();
		if ( depthBuffer->GetFormat() == GrRenderbuffer::EF_DEPTHSTENCIL ||
			depthBuffer->GetFormat() == GrRenderbuffer::EF_DEPTH24STENCIL8 )
		{
			_stencilRenderable = new IXBufferRenderable( depthBuffer );
			CHECK_GL();
		}
	}

	// now we can create the framebuffer object.
	CreateFBO();
}

//----------------------------------------------------------
GrRenderTarget::GrRenderTarget( URef< GrTextureBase >* colorTargets, URef< GrTextureBase > depthTexture, unsigned int colorTargetCount )
: _width( 0 )
, _height( 0 )
, _viewportRect( 0, 0 )
, _targetCount( colorTargetCount )
, _depthRenderable( 0 )
, _stencilRenderable( 0 )
, _fbo( 0 )
, _curRenderFace( GrTextureCube::CF_NEGATIVEZ )
, _cubeTarget( false )
, _autoGenMipmaps( false )
, _isValid( true )
, _isPrimary( false )
{
	// 4 is a hardware limitation.
	B_ASSERT( colorTargetCount < kMaxRTs );
	B_ASSERT( colorTargetCount > 0 || depthTexture != 0 );

	// initialize color renderables.
	MemSet( _colorRenderables, 0, sizeof( _colorRenderables ) );
	if ( colorTargetCount > 0 )
	{
		SetColorTargets( colorTargets, colorTargetCount );
		_cubeTarget = colorTargets[ 0 ]->GetType() == GrTextureBase::ETT_CUBE;
	}
	else if ( depthTexture != 0 )
	{
		GetTextureDim( _width, _height, depthTexture );
		_cubeTarget = depthTexture->GetType() == GrTextureBase::ETT_CUBE;
	}

	// initialize the viewport rectangle.
	_viewportRect.SetWidth( _width );
	_viewportRect.SetHeight( _height );

	// create the depth/stencil renderables.
	if ( depthTexture != 0 )
	{
		// create the appropriate renderable.
		_depthRenderable = CreateRenderable( depthTexture );

		// check the format to see if we need to create a stencil renderable.
		if ( depthTexture->GetFormat() == GrTextureBase::EF_DEPTHSTENCIL )
		{
			_stencilRenderable = CreateRenderable( depthTexture );
		}
	}

	// now we can create the framebuffer object.
	CreateFBO();
}

//----------------------------------------------------------
GrRenderTarget::GrRenderTarget( URef< GrRenderbuffer > colorBuffer, URef< GrRenderbuffer > depthBuffer )
: _width( 0 )
, _height( 0 )
, _viewportRect( 0, 0 )
, _targetCount( colorBuffer != 0 ? 1 : 0 )
, _depthRenderable( 0 )
, _stencilRenderable( 0 )
, _fbo( 0 )
, _curRenderFace( GrTextureCube::CF_NEGATIVEZ )
, _cubeTarget( false )
, _autoGenMipmaps( false )
, _isValid( true )
, _isPrimary( false )
{
	B_ASSERT( colorBuffer != 0 || depthBuffer != 0 );

	CHECK_GL();

	// initialize color renderables.
	MemSet( _colorRenderables, 0, sizeof( _colorRenderables ) );
	if ( colorBuffer != 0 )
	{
		_width = colorBuffer->GetWidth();
		_height = colorBuffer->GetHeight();
		_colorRenderables[ 0 ] = new IXBufferRenderable( colorBuffer );
	}
	else if ( depthBuffer != 0 )
	{
		_width = depthBuffer->GetWidth();
		_height = depthBuffer->GetHeight();
	}

	// initialize the viewport rectangle.
	_viewportRect.SetWidth( _width );
	_viewportRect.SetHeight( _height );

	// create the depth/stencil renderables.
	if ( depthBuffer != 0 )
	{
		// make sure the depth buffer has valid dimensions.
		B_ASSERT( depthBuffer->GetWidth() == _width && depthBuffer->GetHeight() == _height );
		_depthRenderable = new IXBufferRenderable( depthBuffer );
		CHECK_GL();
		if ( depthBuffer->GetFormat() == GrRenderbuffer::EF_DEPTHSTENCIL ||
			depthBuffer->GetFormat() == GrRenderbuffer::EF_DEPTH24STENCIL8 )
		{
			_stencilRenderable = new IXBufferRenderable( depthBuffer );
			CHECK_GL();
		}
	}

	// now we can create the framebuffer object.
	CreateFBO();
}

//----------------------------------------------------------
GrRenderTarget::~GrRenderTarget()
{
	if ( !_isPrimary )
	{
		DestroyFBO();
		for ( unsigned int i = 0; i < _targetCount; ++i )
			delete _colorRenderables[ i ];

		delete _depthRenderable;
		delete _stencilRenderable;
	}
	else
	{
		_primaryRT = 0;
	}

	if ( this == _curBound )
		_curBound = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrRenderTarget*
GrRenderTarget::GetCurrentRT()
{
	return _curBound;
}

//----------------------------------------------------------
void
GrRenderTarget::ResizePrimaryRT( unsigned int width, unsigned int height )
{
	_primaryRT->_width = width;
	_primaryRT->_height = height;
	_primaryRT->_viewportRect.SetWidth( width );
	_primaryRT->_viewportRect.SetHeight( height );
}

//----------------------------------------------------------
URef< GrTextureBase >
GrRenderTarget::GetColorTex( unsigned int idx ) const
{
	// return null if we don't have any color textures targets.
	if ( idx >= _targetCount )
		return 0;

	// cast to the appropriate interface and return the texture object.
	B_ASSERT( _colorRenderables[ idx ] != 0 );
	return _colorRenderables[ idx ]->GetTexture();
/*
	if ( renderable->GetType() == IXRenderable::ET_TEXTURE )
	{
		return ( GrTextureBase* )( ( IXTextureRenderable* )renderable )->GetTexture();
	}
	else if ( renderable->GetType() == IXRenderable::ET_TEXTURERECT )
	{
		return ( GrTextureBase* )( ( IXTextureRectRenderable* )renderable )->GetTexture();
	}
	else if ( renderable->GetType() == IXRenderable::ET_TEXTURECUBE )
	{
		return ( GrTextureBase* )( ( IXTextureCubeRenderable* )renderable )->GetTexture();
	}
	return 0;
*/
}

//----------------------------------------------------------
bool
GrRenderTarget::HasDepthTex()
{
	return _depthRenderable != 0 && _depthRenderable->GetType() == IXRenderable::ET_TEXTURE;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrRenderTarget::GetDepthTex() const
{
	// cast to the appropriate interface and return the texture object.
	IXRenderable* renderable = _depthRenderable;
	if ( renderable->GetType() == IXRenderable::ET_TEXTURE )
	{
		return ( GrTextureBase* )( ( IXTextureRenderable* )renderable )->GetTexture();
	}
	return 0;
}

//----------------------------------------------------------
void
GrRenderTarget::SetViewportRect( const URect& viewportRect )
{
	// this function cannot be used on the primary render target since
	// the primary render target's viewport comes from the primary
	// display surface (the window).
	B_ASSERT( this != _primaryRT );
	_viewportRect = viewportRect;
}

//----------------------------------------------------------
void
GrRenderTarget::Bind( bool setVP )
{
	BindRT();
	if ( setVP )
		GrViewport( _viewportRect.GetX(), _viewportRect.GetY(), _viewportRect.GetWidth(), _viewportRect.GetHeight() );
}

//----------------------------------------------------------
void
GrRenderTarget::SetActiveFace( GrTextureCube::ECUBEFACE face )
{
	// do not forget to check validity!
	B_ASSERT( _isValid );

	// we need to change the render target if the current render face changed.
	if ( _cubeTarget && _curRenderFace != face )
	{
		_curRenderFace = face;
		if ( _curBound == this )
		{
			// texture the appropriate cube faces.
			for ( unsigned int i = 0; i < _targetCount; ++i )
				_colorRenderables[ i ]->BindToFramebuffer( GL_COLOR_ATTACHMENT0_EXT + i, _curRenderFace );
		}
	}
}

//----------------------------------------------------------
void
GrRenderTarget::PreContextReset()
{
	if ( !_isPrimary )
		DestroyFBO();
}

//----------------------------------------------------------
void
GrRenderTarget::PostContextReset()
{
	if ( !_isPrimary )
	{
		CreateFBO();
	}
	else
	{
		_width = gGrSubsys->GetScreenWidth();
		_height = gGrSubsys->GetScreenHeight();
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrRenderTarget::GetTextureDim( unsigned int& width, unsigned int& height, URef< GrTextureBase > texture )
{
	GrTextureBase::ETEXTURETYPE type = texture->GetType();
	if ( type == GrTextureBase::ETT_2D )
	{
		width = ( ( GrTexture* )( GrTextureBase* )texture )->GetWidth();
		height = ( ( GrTexture* )( GrTextureBase* )texture )->GetHeight();
	}
	else if ( type == GrTextureBase::ETT_RECT )
	{
		width = ( ( GrTextureRect* )( GrTextureBase* )texture )->GetWidth();
		height = ( ( GrTextureRect* )( GrTextureBase* )texture )->GetHeight();
	}
	else if ( type == GrTextureBase::ETT_CUBE )
	{
		width = ( ( GrTextureCube* )( GrTextureBase* )texture )->GetDim();
		height = width;
	}
}

//----------------------------------------------------------
IXRenderable*
GrRenderTarget::CreateRenderable( URef< GrTextureBase > texture )
{
	// create the correct renderable for the given texture type.
	GrTextureBase::ETEXTURETYPE type = texture->GetType();
	if ( type == GrTextureBase::ETT_2D )
		return new IXTextureRenderable( ( GrTexture* )( GrTextureBase* )texture );
	else if ( type == GrTextureBase::ETT_RECT )
		return new IXTextureRectRenderable( ( GrTextureRect* )( GrTextureBase* )texture );
	
	B_ASSERT( type == GrTextureBase::ETT_CUBE );
	return new IXTextureCubeRenderable( ( GrTextureCube* )( GrTextureBase* )texture );
}

//----------------------------------------------------------
void
GrRenderTarget::SetColorTargets( URef< GrTextureBase >* colorTargets, unsigned int colorTargetCount )
{
	// return if there are no color targets.
	if ( colorTargetCount == 0 )
		return;

	CHECK_GL();

	// get information about the first color target.
	GetTextureDim( _width, _height, colorTargets[ 0 ] );

	// verify color targets are of a proper format, and generate renderables for
	// all of them.
	GrTextureBase::ETEXTURETYPE type = colorTargets[ 0 ]->GetType();
	for ( unsigned int i = 0; i < colorTargetCount; ++i )
		_colorRenderables[ i ] = CreateRenderable( colorTargets[ i ] );

	CHECK_GL();
}

//----------------------------------------------------------
void
GrRenderTarget::LoseBind()
{
	// go through render targets and generate their mipchains.
	if ( _autoGenMipmaps )
	{
		for ( unsigned int i = 0; i < _targetCount; ++i )
		{
			B_ASSERT( _colorRenderables[ i ] != 0 );
			_colorRenderables[ i ]->AutoGenMipmaps();
		}

		if ( _depthRenderable != 0 )
			_depthRenderable->AutoGenMipmaps();
	}
}

//----------------------------------------------------------
void
GrRenderTarget::CreateFBO()
{
	// ensure that things are valid.
	B_ASSERT( _width > 0 && _height > 0 );
	B_ASSERT( _targetCount >= 0 && _targetCount < 4 );

	// get the currently bound render target.
	int prevFBO = 0;
	bglGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &prevFBO );

	// create and bind the framebuffer object.
	bglGenFramebuffersEXT( 1, &_fbo );
	bglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _fbo );
	CHECK_GL();

	// bind a depth render buffer if necessary.
	if ( _depthRenderable != 0 )
	{
		_depthRenderable->BindToFramebuffer( GL_DEPTH_ATTACHMENT_EXT, _curRenderFace );
		CHECK_GL();
	}

	if ( _stencilRenderable != 0 )
	{
		_stencilRenderable->BindToFramebuffer( GL_STENCIL_ATTACHMENT_EXT, _curRenderFace );
		CHECK_GL();
	}

	for ( unsigned int i = 0; i < _targetCount; ++i )
	{
		// create the texture.
		_colorRenderables[ i ]->BindToFramebuffer( GL_COLOR_ATTACHMENT0_EXT + i, _curRenderFace );
		CHECK_GL();
	}

	// enable or disable read/write buffers.
	unsigned int drawBuffers = 0;
	for ( unsigned int i = 0; i < _targetCount; ++i )
		drawBuffers |= GR_COLOR_ATTACH0 << i;

	GrDrawBuffers( drawBuffers );
	GrReadBuffer( drawBuffers & GR_COLOR_ATTACH0 );
	CHECK_GL();

	// validate the render target.
	// validate the framebuffer state.
	GLenum status = bglCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	if ( status == GL_FRAMEBUFFER_UNSUPPORTED_EXT )
	{
		// no can do!
		PrintF( "Framebuffer creation failed; configuration not supported.\n" );
		_isValid = false;
	}
	else if ( status != GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		switch( status )
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete attachment!" );

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete missing attachment!" );

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete dimensions!" );

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete formats!" );

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete draw buffer!" );

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Incomplete read buffer!" );

		default:
			B_ERROR( "Framebuffer incomplete ( General programming error ): Unknown error!" );
		};
	}

	// make sure the framebuffer is complete.  Otherwise, we hit an OpenGL error ( this
	// means we're using the extension wrong ).
	B_ASSERT( status == GL_FRAMEBUFFER_COMPLETE_EXT || !_isValid );
	bglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, prevFBO );
}

//----------------------------------------------------------
void
GrRenderTarget::DestroyFBO()
{
	// we shouldn't be deleted if we're currently bound!  This helps ensure that
	// state is kept track of.
	B_ASSERT( _curBound != this );
	bglDeleteFramebuffersEXT( 1, &_fbo );
	_fbo = 0;
}

//----------------------------------------------------------
void
GrRenderTarget::BindRT()
{
	// do not forget to check validity!
	B_ASSERT( _isValid );

	// simply return if we're already bound.
	if ( _curBound == this )
		return;

	// switch to a different framebuffer.
	bglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _fbo );
	CHECK_GL();

	// do any post-unbind operations we need to.
	if ( _curBound )
		_curBound->LoseBind();

	// make sure no texture targets are bound.
	for ( unsigned int i = 0; i < _targetCount; ++i )
		_colorRenderables[ i ]->UnbindTexture();

	// set this as the current render target.
	_curBound = this;
	if ( _cubeTarget )
	{
		// bind the appropriate cube faces to the FBO.
		for ( unsigned int i = 0; i < _targetCount; ++i )
		{
			_colorRenderables[ i ]->BindToFramebuffer( GL_COLOR_ATTACHMENT0_EXT + i, _curRenderFace );
		}
	}
}
