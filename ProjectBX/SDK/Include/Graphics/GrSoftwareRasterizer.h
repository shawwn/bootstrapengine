//----------------------------------------------------------
// File:		GrSoftwareRasterizer.h
// Author:		Kevin Bray
// Created:		02-25-07
//
// Purpose:		To rasterize triangles with high performance.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrSoftwareSampler.h"
#include "GrPolygon.h"

// forward declarations.
class GrImage;
class GrSoftwareRTBase;
class IXNullRT;

//**********************************************************
// class GrSoftwareRasterizer
//**********************************************************
class GrSoftwareRasterizer
{
public:
	typedef void ( *VertexProgram )( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex );
	typedef void ( *FragmentProgram )( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
									   const SVec4* varying, const SVec4* gradX, const SVec4* gradY, GrSoftwareSampler* textures );

	enum {
		MAX_RENDER_TARGET_COUNT	= 8
	};

	GrSoftwareRasterizer();
	~GrSoftwareRasterizer();

	void				SetRenderLastPixel( bool enabled )			{	_lastPixel = enabled;		}

	void				SetRenderTarget( unsigned int targetIdx, GrSoftwareRTBase* target );
	void				SetVertexProgram( VertexProgram vertexProg );
	void				SetFragmentProgram( FragmentProgram fragmentProg );

	void				SetVertexConstant( unsigned int idx, const SVec4& value );
	void				SetVertexConstants( unsigned int idx, const MMat4x4& value );
	void				SetFragmentConstant( unsigned int idx, const SVec4& value );
	void				SetFragmentConstants( unsigned int idx, const MMat4x4& value );

	void				SetViewportTransform( const SVec2& scale, const SVec2& offset );

	void				SetSamplerTexture( unsigned int idx, GrImage* image );
	void				SetSamplerColor( unsigned int idx, unsigned color );

	void				RenderPolygon( const GrPolygon& polygon );
	void				RenderPolygons( const std::vector< GrPolygon* >& polygons );
	void				RenderTriangleList( const SPolygonVertex* vertices, unsigned int triCount );

private:
	void				RenderTriangle( const SPolygonVertex* vertices );
	void				FillTargetCache( unsigned int srcX, unsigned int srcY );
	void				FlushTargetCache( unsigned int dstX, unsigned int dstY );

	SVec2				_viewportScale;
	SVec2				_viewportOffset;
	SVec4				_vertexConstants[ 64 ];
	SVec4				_fragmentConstants[ 64 ];
	GrSoftwareSampler	_samplers[ 16 ];
	VertexProgram		_vertexProg;
	FragmentProgram		_fragmentProg;

	GrSoftwareRTBase*	_renderTarget[ MAX_RENDER_TARGET_COUNT ];
	IXNullRT*			_nullRT;
	int					_targetWidth;
	int					_targetHeight;
	unsigned int		_targetBPP[ MAX_RENDER_TARGET_COUNT ];
	int					_blockStride[ MAX_RENDER_TARGET_COUNT ];
	float				_floatTargetWidth;
	float				_floatTargetHeight;

	SPolygonVertex*		_scratchBuffer;
	bool				_lastPixel;
};
