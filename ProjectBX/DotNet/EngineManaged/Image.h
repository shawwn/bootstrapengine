//----------------------------------------------------------
// File:		Image.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To encapsulate texturing functionality.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrImage;

namespace Bootstrap
{
    //**********************************************************
    // ref class Image
    //**********************************************************
	public ref class Image
	{
	internal:
		// NOTE!  The image MUST be allocated within this module!
		Image( GrImage* image );

        GrImage*		_image;

	public:
		Image( System::Drawing::Bitmap^ texture );
		Image( System::String^ fileName );
		~Image();
		!Image();

		enum class Format
		{
			// note that these are assigned to their size in bytes.
			Unknown = 0,
			I8 = 1,
			LA16 = 2,
			BGR24 = 3,
			BGRA32 = 4,
		};

		// returns the name of the image.
		System::String^	                    GetName();

		property System::String^            Name
		{
			System::String^                     get()               {   return GetName();       }
		}

		// returns information about the dimensions and format.
		unsigned int	                    GetWidth();
		unsigned int	                    GetHeight();
		unsigned int	                    GetBPP();
		Format			                    GetFormat();
		bool			                    IsValid();

		System::Drawing::Bitmap^	        ToBitmap(int width, int height);

		// generates a 64x64 thumbnail of the image.
		Image^			                    BuildThumbnail( unsigned int dim );

	private:
		void			                    CalcFormat();

		Format			                    _format;
		bool			                    _valid;
	};
}
