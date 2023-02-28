//----------------------------------------------------------
// File:		Image.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Image.h"

// project headers.
#include "Utils.h"

// engine headers.
#include "RFileMgr.h"
#include "RRsrcMgr.h"
#include "RFile.h"
#include "UReader.h"
#include "GrImage.h"

// .NET namespacs.
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

//**********************************************************
// ref class Image
//**********************************************************

//----------------------------------------------------------
Bootstrap::Image::Image( GrImage* image )
: _image( image )
, _format( Format::Unknown )
{
	CalcFormat();
	_valid = true;
}

//----------------------------------------------------------
Bootstrap::Image::Image( Bitmap^ texture )
{
	_image = BitmapToGrImage( texture );
	CalcFormat();
}

//----------------------------------------------------------
Bootstrap::Image::Image( String^ name )
: _image( 0 )
, _format( Format::Unknown )
{
	// calculate the image file's path.
	UPath fileName( StrToTStr( name ) );

	// read the entire file.
	URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagRead );
	if ( file )
	{
		// read in the image data.
		_image = new GrImage( fileName, UReader( file->GetData(), ( unsigned int )file->GetSize(), false ) );
		_valid = true;

		// free the file data.
		file->Purge();
	}
	else
	{
		_image = new GrImage( fileName, 0xFFFF0000 );
		_valid = false;
	}
	CalcFormat();
}

//----------------------------------------------------------
Bootstrap::Image::~Image()
{
	this->!Image();
}

//----------------------------------------------------------
Bootstrap::Image::!Image()
{
	delete _image;
}

//----------------------------------------------------------
String^	
Bootstrap::Image::GetName()
{
	return TStrToStr( _image->GetName().GetPathString() );
}

//----------------------------------------------------------
unsigned int 
Bootstrap::Image::GetWidth()
{
	return _image->GetWidth();
}

//----------------------------------------------------------
unsigned int 
Bootstrap::Image::GetHeight()
{
	return _image->GetHeight();
}

//----------------------------------------------------------
unsigned int 
Bootstrap::Image::GetBPP()
{
	return ( unsigned int )_format;
}

//----------------------------------------------------------
Bootstrap::Image::Format 
Bootstrap::Image::GetFormat()
{
	return _format;
}

//----------------------------------------------------------
bool 
Bootstrap::Image::IsValid()
{
	return _valid;
}

//----------------------------------------------------------
Bitmap^ 
Bootstrap::Image::ToBitmap( int width, int height )
{
	// create a bitmap from the GrImage.
	Bitmap^ bitmap = GrImageToBitmap( _image );

	// check to see if we should ignore the incoming width and height.
	if ( width <= 0 )
		width = bitmap->Width;
	if ( height <= 0 )
		height = bitmap->Height;

	// return the scaled bitmap if necessary.
	if ( width != bitmap->Width || height != bitmap->Height )
	{
		Bitmap^ scaled = gcnew Bitmap( bitmap, width, height );
		bitmap->~Bitmap();
		return scaled;
	}

	// return the bitmap.
	return bitmap;
}

//----------------------------------------------------------
Bootstrap::Image^ 
Bootstrap::Image::BuildThumbnail( unsigned int dim )
{
	if ( _image->GetFormat() != GrImage::EF_I8 && _image->GetFormat() != GrImage::EF_BGR24 &&
		_image->GetFormat() != GrImage::EF_BGRA32 && _image->GetFormat() != GrImage::EF_LA16 )
		return nullptr;

	// scale the image down to 64x64 (or pad it).
	unsigned int bytesPerPel = _image->GetBytesPerPixel();
	unsigned int mipWidth = _image->GetWidth();
	unsigned int mipHeight = _image->GetHeight();
	unsigned int maxMipCount = _image->GetMaxMipCount();
	unsigned int mip = 0;
	for ( ; mip < maxMipCount; ++mip )
	{
		// simply break when we find a mipmap that can be used as
		// a thumbnail.
		if ( mipWidth <= dim && mipHeight <= dim )
			break;

		// calculate the width and height of the next mipmap.
		mipWidth = Max( mipWidth >> 1, 1U );
		mipHeight = Max( mipHeight >> 1, 1U );
	}

	// use the mipmap to generate the thumbnail image.
	GrImage* thumbnail = new GrImage( _image->GetName() + "thumbnail", GrImage::EF_BGRA32, dim, dim, 0 );

	// fill the bitmap with data.
	unsigned char* dst = ( unsigned char* )thumbnail->GetImageData();
	unsigned char* src = ( unsigned char* )_image->GetImageData( mip );
	unsigned int srcStride = mipWidth * bytesPerPel;
	unsigned int dstStride = dim * thumbnail->GetBytesPerPixel();

	// fill in rows from the source image.
	for ( unsigned int y = 0; y < mipHeight; ++y, dst += dstStride, src += srcStride )
	{
		// fill the row.
		for ( unsigned int x = 0; x < mipWidth; ++x )
		{
			if ( bytesPerPel == 1 )
			{
				dst[ 4 * x + 0 ] = src[ x ];
				dst[ 4 * x + 1 ] = src[ x ];
				dst[ 4 * x + 2 ] = src[ x ];
				dst[ 4 * x + 3 ] = 255;
			}
			else if ( bytesPerPel == 2 )
			{
				dst[ 4 * x + 0 ] = src[ 2 * x + 0 ];
				dst[ 4 * x + 1 ] = src[ 2 * x + 0 ];
				dst[ 4 * x + 2 ] = src[ 2 * x + 0 ];
				dst[ 4 * x + 3 ] = src[ 2 * x + 1 ];
			}
			else if ( bytesPerPel == 3 )
			{
				dst[ 4 * x + 0 ] = src[ 2 * x + x + 0 ];
				dst[ 4 * x + 1 ] = src[ 2 * x + x + 1 ];
				dst[ 4 * x + 2 ] = src[ 2 * x + x + 2 ];
				dst[ 4 * x + 3 ] = 255;
			}
			else if ( bytesPerPel == 4 )
			{
				dst[ 4 * x + 0 ] = src[ 4 * x + 0 ];
				dst[ 4 * x + 1 ] = src[ 4 * x + 1 ];
				dst[ 4 * x + 2 ] = src[ 4 * x + 2 ];
				dst[ 4 * x + 3 ] = src[ 4 * x + 3 ];
			}
		}

		// fill in extra pixels in the current row.
		for ( unsigned int x = mipWidth; x < dim; ++x )
		{
			dst[ 4 * x + 0 ] = 0;
			dst[ 4 * x + 1 ] = 0;
			dst[ 4 * x + 2 ] = 0;
			dst[ 4 * x + 3 ] = 0;
		}
	}

	// fill in extra rows.
	for ( unsigned int y = mipHeight; y < dim; ++y, dst += dstStride )
		MemSet( dst, 0, dstStride );

	// return the new image.
	return gcnew Image( thumbnail );
}

//----------------------------------------------------------
void 
Bootstrap::Image::CalcFormat()
{
	// get the engine format.
	GrImage::EFORMAT format = _image->GetFormat();

	// try to convert to our own format.
	_format = Format::Unknown;
	if ( format == GrImage::EF_BGR24 )
		_format = Format::BGR24;
	else if ( format == GrImage::EF_BGRA32 )
		_format = Format::BGRA32;
	else if ( format == GrImage::EF_LA16 )
		_format = Format::LA16;
	else if ( format == GrImage::EF_I8 )
		_format = Format::I8;
}
