//----------------------------------------------------------
// File:		EdUberTexBuilder.cpp
// Author:		Kevin Bray
// Created:		12-20-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdUberTexBuilder.h"

// project includes.
#include "EdBrushMgr.h"
#include "EdBrush.h"
#include "EdBrushImage.h"

// engine includes.
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrModel.h"
#include "GrTexture.h"
#include "GrMeshInst.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrImage.h"
#include "GrColor.h"
#include "GrConfig.h"

static void TexVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants,
							  const SPolygonVertex& vertex );
static void TexFragmentProgram( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
								const SVec4* varying, const SVec4* gradDX, const SVec4* gradDY, GrSoftwareSampler* textures );

//**********************************************************
// class EdUberTexBuilder
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdUberTexBuilder::EdUberTexBuilder()
{

}

//----------------------------------------------------------
EdUberTexBuilder::~EdUberTexBuilder()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdUberTexBuilder::CreateUberTexture( const UPath& fileName, GrTexture* fillBump, GrTexture* fillDiffuse,
									 GrTexture* fillSpecular )
{

}


//==========================================================
// private methods
//==========================================================


//**********************************************************
// File local functions.
//**********************************************************

//----------------------------------------------------------
void
TexVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants,
				  const SPolygonVertex& vertex )
{
	SVec2 outputUV;
	outputUV.x = vertexConstants[ 0 ].x * vertex.uv.x + vertexConstants[ 0 ].y * vertex.uv.y + vertexConstants[ 0 ].w;
	outputUV.y = vertexConstants[ 1 ].x * vertex.uv.x + vertexConstants[ 1 ].y * vertex.uv.y + vertexConstants[ 1 ].w;

	output.x = 2.0f * outputUV.x - 1.0f;
	output.y = 2.0f * outputUV.y - 1.0f;
	output.z = 0;
	output.w = 0;

	varying[ 0 ].x = vertex.uv.x;
	varying[ 0 ].y = vertex.uv.y;
}

//----------------------------------------------------------
void
TexFragmentProgram( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
					const SVec4* varying, const SVec4* gradDX, const SVec4* gradDY, GrSoftwareSampler* samplers )
{
	// simply return if the current pixel has already been written.
	if ( *output[ 7 ] != 0 )
		return;

	// write out the mask value.
	*output[ 7 ] = 255;

	// calculate source texture coordinates.
	for ( unsigned int i = 0; i <= ES_COUNT; ++i )
	{
#if 1
		// fetch the current stage color.
		unsigned int color = samplers[ i ].Sample( varying[ 0 ] );

		// break down the current color into it's components.
		unsigned int temp[ 4 ];
		temp[ 0 ] = ( color >>  0 ) & 0xFF;
		temp[ 1 ] = ( color >>  8 ) & 0xFF;
		temp[ 2 ] = ( color >> 16 ) & 0xFF;
		temp[ 3 ] = ( color >> 24 ) & 0xFF;

		// if we're dealing with the normal map stage, make sure
		// we swizzle into the correct format.
		if ( i == ES_NORMAL )
		{
			unsigned int b = ( color >> 0 ) & 0xFF;
			unsigned int g = ( color >> 1 ) & 0xFF;
			unsigned int r = ( color >> 2 ) & 0xFF;

			float nz = 2.0f * ( temp[ 0 ] / 255.0f ) - 1.0f;
			float ny = 2.0f * ( temp[ 1 ] / 255.0f ) - 1.0f;
			float nx = 2.0f * ( temp[ 2 ] / 255.0f ) - 1.0f;
			float invZ = 1.0f / nz;

			temp[ 0 ] = ( unsigned int )Clamp( ( 0.5f * nx * invZ + 0.5f ) * 255.0f, 0.0f, 255.0f );	// X
			temp[ 1 ] = 255;
			temp[ 2 ] = 255;
			temp[ 3 ] = ( unsigned int )Clamp( ( 0.5f * ny * invZ + 0.5f ) * 255.0f, 0.0f, 255.0f );	// Y
		}

		// encode the write value.
		unsigned char* dst = output[ i ];
		unsigned int dstBPP = outputBPP[ i ];
		switch( dstBPP )
		{
		case 4:
			dst[ 3 ] = ( unsigned char )temp[ 3 ];
		case 3:
			dst[ 2 ] = ( unsigned char )temp[ 2 ];
			dst[ 1 ] = ( unsigned char )temp[ 1 ];
			dst[ 0 ] = ( unsigned char )temp[ 0 ];
			break;

		case 2:
			dst[ 1 ] = ( unsigned char )temp[ 3 ];
		case 1:
			dst[ 0 ] = ( unsigned char )temp[ 0 ];
			break;
		}
#else
		unsigned char* dst = ( unsigned char* )( output[ i ] );
		dst[ 0 ] = 0xFF;
		dst[ 1 ] = 0xFF;
		dst[ 2 ] = 0xFF;
		dst[ 3 ] = 0xFF;
#endif
	}
}
