//----------------------------------------------------------
// File:        Utils.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To provide misc. utility functions for the 
//              managed layer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrImage;

namespace Bootstrap
{
    // string manipulation functions.
    extern tstring                          StrToTStr( System::String^ source );
    extern System::String^                  TStrToStr( const tstring& source );

    // bitmaps and images.
    extern GrImage*                         BitmapToGrImage( System::Drawing::Bitmap^ texture );
    extern System::Drawing::Bitmap^         GrImageToBitmap( GrImage* image );
}
