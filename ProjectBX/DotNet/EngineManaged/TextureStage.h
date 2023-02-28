//----------------------------------------------------------
// File:		TextureStage.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage parameters to a texture stage.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "GrConfig.h"
#include "GrTextureStage.h"

// forward declarations.
class GrMaterialPass;

namespace Bootstrap
{
    // .NET forward declarations.
    value class Color;

    //**********************************************************
    // ref class TextureStage
    //**********************************************************
	public ref class TextureStage
	{
	internal:
		TextureStage( GrMaterialPass* pass, ESTAGE stage );

	public:
		~TextureStage();
		!TextureStage();

        //**********************************************************
        // enum class AddressType
        //**********************************************************
		enum class AddressType
		{
			Repeat          = GrTextureStage::EAM_REPEAT,
			Clamp           = GrTextureStage::EAM_CLAMP,
		};

        //**********************************************************
        // enum class FilterType
        //**********************************************************
		enum class FilterType
		{
			Anisotropic     = GrTextureStage::EFM_ANISOTROPIC,
			Trilinear       = GrTextureStage::EFM_TRILINEAR,
			Bilinear        = GrTextureStage::EFM_BILINEAR,
			Nearest         = GrTextureStage::EFM_NEAREST,
		};

        //**********************************************************
        // enum class GenTextureFlag
        //**********************************************************
		[System::Flags]
		enum class GenTextureFlag
		{
			GenEnvCube      = GrTextureStage::kGenEnvCube,
			GenEnvDepthCube = GrTextureStage::kGenEnvDepthCube,
			GenCurRender    = GrTextureStage::kGenCurRender,
			GenCurDepth     = GrTextureStage::kGenCurDepth,
			GenReflection   = GrTextureStage::kGenReflection,
		};

		property System::String^	        Texture
		{
			System::String^                     get();
			void                                set( System::String^ value );
		}

		property AddressType		        AddressModeS
		{
			AddressType                         get();
			void                                set( AddressType mode );
		}

		property AddressType		        AddressModeT
		{
			AddressType                         get();
			void                                set( AddressType mode );
		}

		property FilterType			        FilterMode
		{
			FilterType                          get();
			void                                set( FilterType value );
		}

		property float				        MaxAnisotropy
		{
			float                               get();
			void                                set( float value );
		}

		property bool				        MipmapEnable
		{
			bool                                get();
			void                                set( bool value );
		}

		property Color				        TexColorMod
		{
			Color                               get();
			void                                set( Color color );
		};

		property Color				        TexColorAdd
		{
			Color                               get();
			void                                set( Color color );
		};

		property GenTextureFlag		        GenTextureFlags
		{
			GenTextureFlag                      get();
			void                                set( GenTextureFlag value );
		};

	private:
		GrMaterialPass*			            _pass;
		ESTAGE					            _stage;
	};
}