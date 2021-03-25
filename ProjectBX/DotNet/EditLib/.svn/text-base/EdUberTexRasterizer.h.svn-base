//----------------------------------------------------------
// File:		EdUberTexRasterizer.h
// Author:		Kevin Bray
// Created:		02-25-07
//
// Purpose:		To rasterize triangles to an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrSoftwareRasterizer.h"
#include "GrConfig.h"
#include "GrImage.h"
#include "MMat4x4.h"

// forward declarations;
struct SVec3;
struct SVec2;
struct SPolygonVertex;
class GrImage;
class GrUTAtlas;
class EdBrush;
class EdBrushImage;
class GrMeshInst;

//**********************************************************
// class EdUberTexRasterizer
//**********************************************************
class EdUberTexRasterizer
{
public:
	typedef GrSoftwareRasterizer::VertexProgram VertexProgram;
	typedef GrSoftwareRasterizer::FragmentProgram FragmentProgram;

	EdUberTexRasterizer();
	~EdUberTexRasterizer();

	// enables/disables autobaking.
	void						SetAutoBake( bool enabled )			{	_autoBake = enabled;	}
	bool						GetAutoBake() const					{	return _autoBake;		}

	// clears data.
	void						Clear();

	// sets the current brush.
	void						SetBrush( URef< EdBrush > brush );

	// allows the user to set a color on the sampler for when no texture
	// is bound.
	void						SetSamplerColor( ESTAGE stage, unsigned int color );

	// mask and stencil textures.
	void						SetStencils( URef< EdBrushImage > planarStencil, URef< EdBrushImage > depthStencil );

	// input/output setup.
	void						SetRenderTarget( GrMeshInst* target );

	// sets up matrices for rendering.
	void						SetVertexConstant( unsigned int index, const SVec4& vec );
	void						SetVertexConstants( unsigned int index, const MMat4x4& matrix );
	void						SetFragmentConstant( unsigned int index, const SVec4& vec );
	void						SetFragmentConstants( unsigned int index, const MMat4x4& matrix );

	// vertex/fragment shader states.
	void						SetVertexProgram( VertexProgram vp );
	void						SetFragmentProgram( FragmentProgram fp );

	// renders triangles to ubertextures.
	void						RenderTriangleList( const SVec3* positions, const SVec2* texCoords,
													unsigned int triCount, const MMat4x4* uvXForm = 0 );
	void						RenderTriangleList( const SVec3* positions, const SVec2* texCoords,
													const unsigned short* indices, unsigned int triCount,
														const MMat4x4* uvXForm = 0 );
	void						RenderTriangle( const SPolygonVertex* vertices, const MMat4x4* uvXForm = 0 );

	// renders triangles to ubertextures and bakes immediately.
	void						RenderTriangleListAndBake( const SVec3* positions, const SVec2* texCoords,
														   unsigned int triCount );
	void						RenderTriangleListAndBake( const SVec3* positions, const SVec2* texCoords,
														   const unsigned short* indices, unsigned int triCount );
	void						RenderTrianglesAndBake( const SPolygonVertex* vertices, unsigned int triCount );

	// bakes the contents of the write caches to the ubertexture targets.
	void						Bake();

	// shaders.
//	static void ColorVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex );
//	static void ColorFragmentProgram( unsigned int** output, const SVec4* fragmentConstants, const SVec4* varying, GrImage** textures );

private:
	// rasterizer.
	GrSoftwareRasterizer		_rasterizer;

	// current render target.
	GrMeshInst*					_target;

	// current brush.
	URef< EdBrush >				_brush;

	// defaults.
	GrImage						_white;
	GrImage						_black;
	GrImage						_gray;

	// bakes whenever a layer is unavailable.
	bool						_autoBake;
};
