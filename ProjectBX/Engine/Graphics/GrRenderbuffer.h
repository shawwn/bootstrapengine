//----------------------------------------------------------
// File:		GrRenderbuffer.h
// Author:		Kevin Bray
// Created:		10-17-05
//
// Purpose:		To manage a render buffer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// project includes.
#include "URefCount.h"

//**********************************************************
// class GrRenderbuffer
//**********************************************************
class GrRenderbuffer : public URefCount
{
public:
	enum EFORMAT
	{
		EF_RGBA,
		EF_RGBA8,
		EF_RGBA16F,
		EF_RGBA32F,
		EF_DEPTH,
		EF_DEPTH16,
		EF_DEPTH24,
		EF_DEPTHSTENCIL,
		EF_DEPTH24STENCIL8,
		EF_COUNT,
	};

	GrRenderbuffer( unsigned int width, unsigned int height, EFORMAT format );
	~GrRenderbuffer();

	unsigned int		GetWidth() const		{	return _width;		}
	unsigned int		GetHeight() const		{	return _height;		}
	EFORMAT				GetFormat() const		{	return _format;		}

	void				BindToFramebuffer( unsigned int attachment );

	void				PreContextReset();
	void				PostContextReset();

private:
	void				CreateRB();
	void				DeleteRB();

	unsigned int		_glHandle;
	unsigned int		_glHandle2;
	unsigned int		_width;
	unsigned int		_height;
	EFORMAT				_format;
	bool				_emulatePackedFormat;
};
