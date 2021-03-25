//----------------------------------------------------------
// File:		MaterialPass.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To contain parameters for a program and is intended
//				to contain a pass for a material.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "GrUtil.h"

// forward declarations.
class GrMaterialPass;

namespace Bootstrap
{
	// .NET forward declarations.
	ref class Expr;
	ref class TextureSet;
	ref class TextureStage;
    value class Color;

    //**********************************************************
    // ref class MaterialPass
    //**********************************************************
	public ref class MaterialPass
	{
	internal:
		MaterialPass( GrMaterialPass* pass );

	public:
		~MaterialPass();
		!MaterialPass();

        //**********************************************************
        // enum class RenderState
        //**********************************************************
		[System::Flags]
		enum class RenderState : unsigned int
		{
			DepthTest = GR_DEPTHTEST,
			CullFace = GR_CULLFACE,
			Blend = GR_BLEND,
			AlphaTest = GR_ALPHATEST,
			DepthBias = GR_DEPTHBIAS,
		};

        //**********************************************************
        // enum class WriteChannel
        //**********************************************************
		[System::Flags]
		enum class WriteChannel : unsigned int
		{
			Red = GR_RED,
			Blue = GR_BLUE,
			Green = GR_GREEN,
			Alpha = GR_ALPHA,
			RGB = GR_RGB,
			RGBA = GR_RGBA,
			Depth = GR_DEPTH,
		};

        //**********************************************************
        // enum class Face
        //**********************************************************
		enum class Face
		{
			Front = ECM_FRONT,
			Back = ECM_BACK,
		};

        //**********************************************************
        // enum class DepthTest
        //**********************************************************
		enum class DepthTest
		{
			LessEqual = EDM_LESSEQUAL,
			Equal = EDM_EQUAL,
			Always = EDM_ALWAYS,
		};

        //**********************************************************
        // enum class AlphaTest
        //**********************************************************
		enum class AlphaTest
		{
			Less = EAM_LESS,
			LessEqual = EAM_LESSEQUAL,
			Greater = EAM_GREATER,
			GreaterEqual = EAM_GREATEREQUAL,
			Equal = EAM_EQUAL,
			NotEqual = EAM_NOTEQUAL,
		};

        //**********************************************************
        // enum class BlendFactor
        //**********************************************************
		enum class BlendFactor
		{
			Zero = EBM_ZERO,
			One = EBM_ONE,
			SrcColor = EBM_SRC_COLOR,
			DstColor = EBM_DST_COLOR,
			OneMinusSrcColor = EBM_ONE_MINUS_SRC_COLOR,
			OneMinusDstColor = EBM_ONE_MINUS_DST_COLOR,
			SrcAlpha = EBM_SRC_ALPHA,
			DstAlpha = EBM_DST_ALPHA,
			OneMinusSrcAlpha = EBM_ONE_MINUS_SRC_ALPHA,
			OneMinusDstAlpha = EBM_ONE_MINUS_DST_ALPHA,
			SrcAlphaSat = EBM_SRC_ALPHA_SAT,
		};

		// shader.
		property System::String^	        Shader
		{
			System::String^                     get();
			void                                set( System::String^ value );
		}

		// ambient shader.
		property System::String^	        AmbientShader
		{
			System::String^                     get();
			void                                set( System::String^ value );
		}

		// pass enable/disable.
		property Expr^				        PassEnable
		{
			Expr^                               get();
			void                                set( Expr^ value );
		}

		// true if this pass is the first pass.
		property bool				        FirstPass
		{
			bool                                get();
		}

		// render states.
		property RenderState		        RenderStates
		{
			RenderState                         get();
			void                                set( RenderState value );
		}

		// cull face.
		property Face				        CullFace
		{
			Face                                get();
			void                                set( Face face );
		}

		property AlphaTest			        AlphaTestMode
		{
			AlphaTest                           get();
			void                                set( AlphaTest test );
		};

		// alpha test.
		property Expr^				        AlphaTestRef
		{
			Expr^                               get();
			void                                set( Expr^	value );
		}

		property float				        ConstDepthBias
		{
			float                               get();
			void                                set( float value );
		}

		property float				        SlopeDepthBias
		{
			float                               get();
			void                                set( float value );
		}

		// write masks.
		property WriteChannel		        WriteMask
		{
			WriteChannel                        get();
			void                                set( WriteChannel value );
		}

		// cast shadows.
		property bool				        CastShadow
		{
			bool                                get();
			void                                set( bool value );
		}

		// receive shadows.
		property bool				        ReceiveShadow
		{
			bool                                get();
			void                                set( bool value );
		}

		// vertex color modulate.
		property Color				        VertexColorMod
		{
			Color                               get();
			void                                set( Color value );
		}

		// vertex color add.
		property Color				        VertexColorAdd
		{
			Color                               get();
			void                                set( Color value );
		}

		// minimum light response.
		property Color				        MinLightResponse
		{
			Color                               get();
			void                                set( Color value );
		}

		// maximum light response.
		property Color				        MaxLightResponse
		{
			Color                               get();
			void                                set( Color value );
		}

		// ambient source color blend factor.
		property BlendFactor		        AmbientSrcColorBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// ambient destination color blend factor.
		property BlendFactor		        AmbientDstColorBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// ambient source alpha blend factor.
		property BlendFactor		        AmbientSrcAlphaBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// ambient destination alpha blend factor.
		property BlendFactor		        AmbientDstAlphaBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// light source color blend factor.
		property BlendFactor		        LightSrcColorBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// light destination color blend factor.
		property BlendFactor		        LightDstColorBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// light source alpha blend factor.
		property BlendFactor		        LightSrcAlphaBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// light destination alpha blend factor.
		property BlendFactor		        LightDstAlphaBlendFactor
		{
			BlendFactor                         get();
			void                                set( BlendFactor value );
		}

		// diffuse texture stage.
		property TextureStage^		        DiffuseStage
		{
			TextureStage^                       get();
		}

		// specular texture stage.
		property TextureStage^		        SpecularStage
		{
			TextureStage^                       get();
		}

		// normal texture stage.
		property TextureStage^		        NormalStage
		{
			TextureStage^                       get();
		}

		// bump texture stage.
		property TextureStage^		        BumpStage
		{
			TextureStage^                       get();
		}

		// emissive texture stage.
		property TextureStage^		        EmissiveStage
		{
			TextureStage^                       get();
		}

		// ambient texture stage.
		property TextureStage^		        AmbientStage
		{
			TextureStage^                       get();
		}

		// texture set.
		property TextureSet^		        Textures
		{
			TextureSet^                         get();
		}

	private:
		TextureSet^					_textures;
		GrMaterialPass*				_pass;
	};
}