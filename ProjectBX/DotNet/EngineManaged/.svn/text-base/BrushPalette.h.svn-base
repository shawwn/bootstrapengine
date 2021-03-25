//----------------------------------------------------------
// File:		BrushPalette.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide an interface for holding a set brushes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "URef.h"

// engine forward declarations.
class EdBrushPalette;

namespace Bootstrap
{
	// managed forward declarations.
	ref class Brush;

    //**********************************************************
    // ref class BrushPalette
    //**********************************************************
	public ref class BrushPalette
	{
	internal:
		BrushPalette( EdBrushPalette* palette );

	public:
		// create a brush palette of the given name.
		BrushPalette( System::String^ name );
		~BrushPalette();
		!BrushPalette();

		// creates a new brush palette with the name specified.
		static BrushPalette^				Create( System::String^ name );
		static void							Remove( System::String^ name );

        // tries to find a brush palette with the name specified.  If none exists,
        // then it tries to load a brush palette with the name specified.  
        // If the load fails, null is returned.
        static BrushPalette^                TryLoad( System::String^ name );

		// returns a list of the names of all loaded brush palettes.
		static array< System::String^ >^	GetBrushPaletteNameList();

		// saves all brush palettes.
		static void							SaveBrushPalettes();

		// brush palette name.
		property System::String^            Name 
        { 
            System::String^                     get(); 
        }

		// brush collection manipulators.
		unsigned int		                GetBrushCount();
		Brush^				                GetBrush( unsigned int idx );
		array< Brush^ >^	                GetBrushes();
		bool				                AddBrush( Brush^ brush );
		void				                RemoveBrush( unsigned int idx );
		void				                RemoveBrush( System::String^ name );

	private:

		// brush palette.
		EdBrushPalette*		                _palette;
	};
}
