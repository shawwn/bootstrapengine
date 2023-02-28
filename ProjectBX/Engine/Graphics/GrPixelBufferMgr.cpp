//----------------------------------------------------------
// File:		GrPixelBufferMgr.cpp
// Author:		Kevin Bray
// Created:		04-11-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPixelBufferMgr.h"

// project includes.
#include "GrPixelBuffer.h"
#include "GLSubsys.h"

GrPixelBufferMgr* gGrPixelBufferMgr = 0;

//**********************************************************
// GrPixelBufferMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPixelBufferMgr::GrPixelBufferMgr()
{
	B_ASSERT( gGrPixelBufferMgr == 0 );
	gGrPixelBufferMgr = this;
}

//----------------------------------------------------------
GrPixelBufferMgr::~GrPixelBufferMgr()
{
	// clear the pixel buffer manager.
	gGrPixelBufferMgr = 0;
	CollectGarbage();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrPixelBuffer >
GrPixelBufferMgr::CreatePixelBuffer( unsigned int width, unsigned int height,
									 GrPixelBuffer::EFORMAT format, GrPixelBuffer::EUSAGE usage )
{
	// create a new pixel buffer.
	GrPixelBuffer* buf = new GrPixelBuffer( width, height, format, usage );

	// add it to our list of managed pixel buffers and return it.
	_pixelBuffers.AddItem( buf );
	return buf;
}

//----------------------------------------------------------
void
GrPixelBufferMgr::CollectGarbage()
{
	_pixelBuffers.CollectGarbage();
}

//----------------------------------------------------------
void
GrPixelBufferMgr::PreContextReset()
{
	// collect garbage.
	CollectGarbage();

	// notify buffers of the pending reset.
	for ( unsigned int i = 0; i < _pixelBuffers.GetElemCount(); ++i )
		_pixelBuffers[ i ]->PreContextReset();
}

//----------------------------------------------------------
void
GrPixelBufferMgr::PostContextReset()
{
	// notify buffers of the completed reset.
	for ( unsigned int i = 0; i < _pixelBuffers.GetElemCount(); ++i )
		_pixelBuffers[ i ]->PostContextReset();
}
