//----------------------------------------------------------
// File:		GrRenderAmbience.h
// Author:		Kevin Bray
// Created:		04-07-07
//
// Purpose:		To render ambience for a set of objects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrRenderList.h"

// forward declarations.
class GrStreamingIB;
class GrCamera;
class GrColor;
class GrTextureBase;
class GrMaterial;
class GrShader;

//**********************************************************
// class GrRenderAmbience
//**********************************************************
class GrRenderAmbience
{
public:
	GrRenderAmbience( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize );
	~GrRenderAmbience();

	void					SetWireframeEnable( bool wireframe )	{	_wireframe = wireframe;			}
	void					SetColorTextureEnable( bool enable )	{	_colorTextureEnable = enable;	}

	void					RenderAmbience( GrRenderList& postRenderObjects, const GrRenderList& objects, const GrCamera& camera,
											const GrColor& ambientColor, unsigned int time, EGRQUALITY quality );

private:
	unsigned int			BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx,
										 unsigned int vertexComponents, unsigned int time );
	const GrMaterial*		GetMaterialSpan( unsigned int& endObj, const GrRenderList& list, unsigned int startObj );

	static bool				SortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right );

	URef< GrTextureBase >	_normalizeCube;
	URef< GrTextureBase >	_defaultBump;		// 1x1, grey.
	URef< GrTextureBase >	_white;				// 1x1, white.
	URef< GrTextureBase >	_black;				// 1x1, black.
	URef< GrTextureBase >	_stipple[ 4 ];		// 4 2x2 stipple textures.

	URef< GrShader >		_ambient;			// default ambient shader.

	GrStreamingIB*			_streamingIB;
	unsigned int			_maxIndexBatchSize;

	bool					_wireframe;
	bool					_colorTextureEnable;
	bool					_batchEnable;
};
