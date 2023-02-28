//----------------------------------------------------------
// File:		TextureSet.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage the texture stages in a material pass.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "GrConfig.h"

// forward declarations.
class GrMaterialPass;

namespace Bootstrap
{
	// .NET forward declarations.
	ref class TextureStage;
	ref class Expr;

    //**********************************************************
    // ref class TextureSet
    //**********************************************************
	public ref class TextureSet
	{
	internal:
		TextureSet( GrMaterialPass* materialPass );

	public:
		~TextureSet();
		!TextureSet();

        //**********************************************************
        // enum class TextureSet
        //**********************************************************
		enum class Stage
		{
			Bump        = 0,
			Normal      = 1,
			Diffuse     = 2,
			Specular    = 3,
			Emissive    = 4,
			Ambient     = 5,
			Count       = 6,
		};

		property Expr^			            ScaleS
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            ScaleT
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            Rotate
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            RotateCenterS
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            RotateCenterT
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            TranslateS
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		property Expr^			            TranslateT
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		// return the texture stage.
		TextureStage^			            GetTextureStage( Stage stage );

	private:
		GrMaterialPass*			            _pass;
		array< TextureStage^ >^	            _stages;
	};
};
