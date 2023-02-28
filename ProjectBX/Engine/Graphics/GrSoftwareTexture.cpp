//----------------------------------------------------------
// File:		GrSoftwareTexture.cpp
// Author:		Shawn Presser
// Created:		09-13-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSoftwareTexture.h"

// project headers.
#include "GrImage.h"

//----------------------------------------------------------
GrSoftwareTexture::GrSoftwareTexture( GrImage* image )
: _image( image )
{
    B_ASSERT( image );
}

//----------------------------------------------------------
GrSoftwareTexture::~GrSoftwareTexture()
{
    delete _image;
}

//----------------------------------------------------------
GrImage*        
GrSoftwareTexture::GetOwnedImage()
{
    return _image;
}