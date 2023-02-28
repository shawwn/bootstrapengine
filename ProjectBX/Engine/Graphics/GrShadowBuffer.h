//----------------------------------------------------------
// File:		GrShadowBuffer.h
// Author:		Kevin Bray
// Created:		11-27-05
//
// Purpose:		To encapsulate/abstract shadow buffer functionality.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

class GrRenderTarget;
class GrTexture;

//**********************************************************
// class GrShadowBuffer
//**********************************************************
class GrShadowBuffer
{
public:
	GrShadowBuffer( unsigned int width, unsigned int height );
	~GrShadowBuffer();

	unsigned int			GetWidth() const;
	unsigned int			GetHeight() const;

	void					Bind();
	URef< GrTexture >		Finish();

private:
	void					BuildRenderTarget( unsigned int width, unsigned int height );

	URef< GrRenderTarget >	_renderTarget;
	URef< GrTexture >		_copyDepthTex;
	GrRenderTarget*			_prevRT;
};
