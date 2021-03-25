//----------------------------------------------------------
// File:		Brush.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide an shading setup for painting to
//				ubertextures.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "URef.h"

// engine forward declarations.
class EdUberTexRasterizer;
class EdBrush;
enum ESTAGE;

namespace Bootstrap
{
	// .NET forward declarations.
	ref class VertexProgram;
	ref class FragmentProgram;

    //**********************************************************
    // ref class Brush
    //**********************************************************
	public ref class Brush
	{
	internal:
		Brush( EdBrush* brush );

		// native brush.
		EdBrush*		_brush;

	public:
		enum class Stage
		{
			Bump		= 0,
			Normal		= 1,
			Diffuse		= 2,
			Specular	= 3,
			Emissive	= 4,
			Ambient		= 5,
			User		= 6,
		};

		// create a brush of the given name.
		Brush( System::String^ name );
		~Brush();
		!Brush();

		// creates a new brush with the name specified in the category specified.
		static Brush^                       Create( System::String^ name, System::String^ category );

		// returns a list of all loaded brush names.
		static array< System::String^ >^    GetBrushNameList();

		// saves all brushes.
		static void		                    SaveBrushes();

		property System::String^            Name 
        { 
            System::String^                     get(); 
        }

		property System::String^            ShortName 
        { 
            System::String^                     get(); 
        }

		// brush image params.
		void			                    SetImage( Stage stage, System::String^ fileName );
		System::String^	                    GetImageName( Stage stage );
		bool			                    HasImage( Stage stage );

		// brush mask params.
		void			                    SetMask( System::String^ fileName );
		System::String^	                    GetMaskName();
		bool			                    HasMask();

		// applies to the rasterizer.
		void			                    Apply( EdUberTexRasterizer* rasterizer );

		// returns the vertex program.
        property Bootstrap::VertexProgram^  VertexProgram   
        { 
            Bootstrap::VertexProgram^		    get(); 
        }

		// returns the fragment program.
		property Bootstrap::FragmentProgram^ FragmentProgram 
        { 
            Bootstrap::FragmentProgram^         get(); 
        }

	private:
		ESTAGE			                    GetEngineStage( Stage stage );
	};
}
