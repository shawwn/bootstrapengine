//---------------------------------------------------------------------------------------------
// File:		Utils.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//---------------------------------------------------------------------------------------------
#include "EMAfx.h"

// system headers.
#include <vcclr.h>

// file header.
#include "Utils.h"

// engine headers.
#include "GrImage.h"
#include "RRsrcMgr.h"

// .NET namespaces.
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

//**********************************************************
// function definitions.
//**********************************************************

//----------------------------------------------------------
GrImage* 
Bootstrap::BitmapToGrImage( Bitmap^ texture )
{
	if( texture == nullptr )
		return 0;

	// verify that the bitmap is a power of two in size.
	E_ASSERT( IsPow2( texture->Width ) && IsPow2( texture->Height ) );

	// calculate the format of the image.
	PixelFormat format = texture->PixelFormat;
	GrImage::EFORMAT grFormat;
	int dstBytesPerPel = 0;
	int srcBytesPerPel = 0;

	switch( format )
	{

	case PixelFormat::Format24bppRgb:
		grFormat = GrImage::EF_BGR24;
		srcBytesPerPel = 3;
		dstBytesPerPel = 3;
		break;

	case PixelFormat::Format32bppRgb:
		grFormat = GrImage::EF_BGR24;
		srcBytesPerPel = 4;
		dstBytesPerPel = 3;
		break;

	case PixelFormat::Format32bppArgb:
		grFormat = GrImage::EF_BGRA32;
		srcBytesPerPel = 4;
		dstBytesPerPel = 4;
		break;

	default:
		return 0;
	}

	// note that images are specified from the bottom up
	// (byte 0 = bottom, left).  This flip handles that.
	texture->RotateFlip( RotateFlipType::RotateNoneFlipY );

	// lock.
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, texture->Width, texture->Height);
	BitmapData^ data = texture->LockBits( rect, 
		ImageLockMode::ReadWrite, 
		PixelFormat::Format32bppArgb );
	if( data == nullptr )
		return 0;

	// create a new image.
	GrImage* result = new GrImage( "Painting", grFormat, texture->Width, texture->Height );

	// clone the incoming data.
	unsigned char* dst = ( unsigned char* )result->GetImageData();
	unsigned char* srcRow = ( unsigned char* )data->Scan0.ToPointer();
	for ( int y = 0; y < texture->Height; ++y, srcRow += data->Stride )
	{
		unsigned char* src = srcRow;
		for ( int x = 0; x < texture->Width; ++x, src += srcBytesPerPel, dst += dstBytesPerPel )
			for ( int c = 0; c < dstBytesPerPel; ++c )
				dst[ c ] = src[ c ];
	}

	// unlock.
	texture->UnlockBits( data );

	// return the result.
	return result;
}

//----------------------------------------------------------
Bitmap^ 
Bootstrap::GrImageToBitmap( GrImage* image )
{
	// choose a good format.
	unsigned int srcBytesPerPel = 0;
	PixelFormat format = PixelFormat::Format32bppRgb;
	switch ( image->GetFormat() )
	{
	case GrImage::EF_I8:
		format = PixelFormat::Format32bppArgb;
		srcBytesPerPel = 1;
		break;
	case GrImage::EF_LA16:
		format = PixelFormat::Format32bppArgb;
		srcBytesPerPel = 2;
		break;
	case GrImage::EF_BGR24:
		format = PixelFormat::Format32bppRgb;
		srcBytesPerPel = 3;
		break;
	case GrImage::EF_BGRA32:
		format = PixelFormat::Format32bppArgb;
		srcBytesPerPel = 4;
		break;
	default:
		return nullptr;
	}

	// create a new bitmap and copy over the data.
	unsigned int width = image->GetWidth();
	unsigned int height = image->GetHeight();
	Bitmap^ bitmap = gcnew Bitmap( width, height, format );

	// lock the bitmap.
	System::Drawing::Rectangle rect( 0, 0, width, height );
	BitmapData^ lockInfo = bitmap->LockBits( rect, ImageLockMode::WriteOnly, format );

	// now copy over the bitmap data.
	unsigned char* src = ( unsigned char* )image->GetImageData();
	unsigned char* dstLine = ( unsigned char* )lockInfo->Scan0.ToPointer();
	unsigned int dstStride = lockInfo->Stride;
	for ( unsigned int y = 0; y < height; ++y, dstLine += dstStride )
	{
		unsigned char* dst = dstLine;
		for ( unsigned int x = 0; x < width; ++x, dst += 4, src += srcBytesPerPel )
		{
			switch ( image->GetFormat() )
			{
			case GrImage::EF_I8:
				dst[0] = dst[1] = dst[2] = dst[3] = src[0];
				break;
			case GrImage::EF_LA16:
				dst[0] = dst[1] = dst[2] = src[0];
				dst[3] = src[1];
				break;
			case GrImage::EF_BGR24:
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				break;
			case GrImage::EF_BGRA32:
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = src[3];
				break;
			}
		}
	}

	// unlock the bitmap and return it.
	bitmap->UnlockBits( lockInfo );
	bitmap->RotateFlip( RotateFlipType::RotateNoneFlipY );

	// return the bitmap.
	return bitmap;
}

//----------------------------------------------------------
// converts a System::String string to a tstring.
tstring 
Bootstrap::StrToTStr( String^ source )
{
	if ( source == nullptr )
		return "";

	tstring result;
	// pin the string to prevent garbage collection.
	pin_ptr<const wchar_t> wch = PtrToStringChars( source );

	// get its unicode length.
	size_t len = (( source->Length+1) * 2);

	// convert to ASCII.  In the future, convert to UTF-8.
	size_t numConverted;
	char *ch = new char[ len+1 ];
	bool success = wcstombs_s( &numConverted, ch, len, wch, len ) == 0;

	// if the conversion was successful, construct a tstring.
	// otherwise, leave it empty.
	if( success )
	{
		ch[ numConverted ] = 0; // null-terminate.
		result = tstring( ch );
	}

	delete ch;
	return result;
}

//----------------------------------------------------------
// converts a tstring to a System::String.
String^ 
Bootstrap::TStrToStr( const tstring& source )
{
	return gcnew String( source.c_str() );
}
