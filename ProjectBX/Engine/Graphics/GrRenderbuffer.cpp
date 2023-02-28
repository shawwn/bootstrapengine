//----------------------------------------------------------
// File:		GrRenderbuffer.cpp
// Author:		Kevin Bray
// Created:		10-17-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderbuffer.h"

// project includes.
#include "GLSubsys.h"

static unsigned int formatMap[ GrRenderbuffer::EF_COUNT ] =
{
	GL_RGBA,
	GL_RGBA8,
	GL_RGBA16F_ARB,
	GL_RGBA32F_ARB,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT24,
	GL_DEPTH_STENCIL_EXT,
	GL_DEPTH24_STENCIL8_EXT,
};

//**********************************************************
// class GrRenderbuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderbuffer::GrRenderbuffer( unsigned int width, unsigned int height, EFORMAT format )
: _glHandle( 0 )
, _glHandle2( 0 )
, _width( width )
, _height( height )
, _format( format )
{
	CreateRB();
}

//----------------------------------------------------------
GrRenderbuffer::~GrRenderbuffer()
{
	DeleteRB();
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
GrRenderbuffer::BindToFramebuffer( unsigned int attachment )
{
	if ( _emulatePackedFormat )
	{
		// bind the stencil buffer if needed.
		if ( attachment == GL_STENCIL_ATTACHMENT_EXT )
			bglFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, _glHandle2 );
		else
			bglFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, _glHandle );	
	}
	else
	{
		bglFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, _glHandle );
	}
}

//----------------------------------------------------------
void
GrRenderbuffer::PreContextReset()
{
	DeleteRB();
}

//----------------------------------------------------------
void
GrRenderbuffer::PostContextReset()
{
	CreateRB();
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
void
GrRenderbuffer::CreateRB()
{
	CHECK_GL();
	_emulatePackedFormat = ( _format == EF_DEPTHSTENCIL || _format == EF_DEPTH24STENCIL8 );
	_emulatePackedFormat = _emulatePackedFormat && !IsExtSupported( "GL_EXT_packed_depth_stencil" );
	if ( _emulatePackedFormat )
	{
		bglGenRenderbuffersEXT( 1, &_glHandle );
		bglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _glHandle );
		bglRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _width, _height );

		bglGenRenderbuffersEXT( 1, &_glHandle2 );
		bglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _glHandle2 );
		bglRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, _width, _height );
	}
	else
	{
		bglGenRenderbuffersEXT( 1, &_glHandle );
		bglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _glHandle );
		bglRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, formatMap[ _format ], _width, _height );
	}
	CHECK_GL();
}

//----------------------------------------------------------
void
GrRenderbuffer::DeleteRB()
{
	bglDeleteRenderbuffersEXT( 1, &_glHandle );
	if ( _glHandle2 )
		bglDeleteRenderbuffersEXT( 1, &_glHandle2 );
}
