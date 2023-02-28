//----------------------------------------------------------
// File:        GrSoftwareTexture.h
// Author:      Shawn Presser
// Created:     09-13-08
//
// Purpose:     To manage a GrImage for software rasterization purposes;
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"

// forward declarations.
class GrImage;

//**********************************************************
// class GrSoftwareTexture
//**********************************************************
class GrSoftwareTexture : public URefCount
{
public:
    GrSoftwareTexture( GrImage* image );
    ~GrSoftwareTexture();

    // returns the image owned by this software texture.
    GrImage*        GetOwnedImage();

private:
    GrImage*        _image;
};