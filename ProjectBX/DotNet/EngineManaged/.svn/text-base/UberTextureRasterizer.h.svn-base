//----------------------------------------------------------
// File:		UberTextureRasterizer.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To rasterize triangles to an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine headers.
#include "GrSoftwareRasterizer.h"

// forward declarations.
class GrMeshInst;
class EdUberTexRasterizer;

namespace Bootstrap
{
	// .NET forward declarations.
	ref class UberTexture;
	ref class MeshVB;
	ref class MeshInst;
	ref class Brush;
	ref class VertexProgram;
	ref class FragmentProgram;
	value class Matrix;
	value class Vector2;
	value class Vector3;
	value class Vector4;

    //**********************************************************
    // ref class UberTextureRasterizer
    //**********************************************************
	public ref class UberTextureRasterizer
	{
	public:
		UberTextureRasterizer();

        // delegate declarations.
		delegate void                       CastProgressCallback(int triCount, int trisProcess, int raysCast);

		// clears data.
		void						        Clear();
		
		// bakes the contents of the write caches to the ubertexture targets.
		void						        Bake();

		// the current brush.
        property Bootstrap::Brush^	        Brush
		{ 
			Bootstrap::Brush^	                get(); 
            void	                            set( Bootstrap::Brush^ brush ); 
		}

		// the current render target.
		property MeshInst^					RenderTarget 
		{ 
			MeshInst^				            get(); 
			void				                set( MeshInst^ target );
		}

		// bakes a high poly model into the normal/bump maps.
		void						        CastGeometry( CastProgressCallback^ callback, System::String^ objFilePath,
        												  float maxRayDist, bool normal, bool bump, float bumpScale,
        												  bool smooth );

		// set the auto bake.
		void						        SetAutoBake( bool bake );

		// sets up matrices for rendering.
		void						        SetVertexConstant( unsigned int index, Vector4 vec );
		void						        SetVertexConstants( unsigned int index, Matrix matrix );
		void						        SetFragmentConstant( unsigned int index, Vector4 vec );
		void						        SetFragmentConstants( unsigned int index, Matrix matrix );

		// vertex/fragment shader states.
        property Bootstrap::VertexProgram^		VertexProgram	
        { 
            Bootstrap::VertexProgram^               get();		
            void                                    set( Bootstrap::VertexProgram^ val ); 
        }

		property Bootstrap::FragmentProgram^	FragmentProgram	
        { 
            Bootstrap::FragmentProgram^             get();	
            void                                    set( Bootstrap::FragmentProgram^ val ); 
        }

		// renders triangles to ubertextures.
        void                                RenderTriangleList( MeshVB^ meshVB, array< unsigned short >^ indices,
                                                                unsigned int indexOffset, unsigned int indexCount );
        void                                RenderTriangle( Vector3 position0, Vector2 texCoord0,
                                                            Vector3 position1, Vector2 texCoord1,
                                                            Vector3 position2, Vector2 texCoord2 );

	internal:
		UberTextureRasterizer( EdUberTexRasterizer* rasterizer );

		EdUberTexRasterizer*		        GetEdUberTexRasterizer()            {	return _rasterizer;		}

	private:
		EdUberTexRasterizer*		        _rasterizer;
        Bootstrap::Brush^			        _currentBrush;
		MeshInst^							_currentRenderTarget;
		Bootstrap::VertexProgram^	        _currentVertexProgram;
		Bootstrap::FragmentProgram^	        _currentFragmentProgram;
	};

    //**********************************************************
    // ref class VertexProgram
    //**********************************************************
	public ref class VertexProgram
	{
	internal:
		VertexProgram( GrSoftwareRasterizer::VertexProgram program )
			: _program( program )
		{
		}

		GrSoftwareRasterizer::VertexProgram _program;
	};

    //**********************************************************
    // ref class FragmentProgram
    //**********************************************************
	public ref class FragmentProgram
	{
	internal:
		FragmentProgram( GrSoftwareRasterizer::FragmentProgram program )
			: _program( program )
		{
		}

		GrSoftwareRasterizer::FragmentProgram _program;
	};
}