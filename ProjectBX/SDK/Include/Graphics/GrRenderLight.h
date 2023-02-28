//----------------------------------------------------------
// File:		GrRenderLight.h
// Author:		Kevin Bray
// Created:		04-07-07
//
// Purpose:		To render a light.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrRenderShadow.h"
#include "GrRenderList.h"

// forward declarations.
class GrRenderTarget;
class GrTextureBase;
class GrStreamingIB;
class GrRenderList;
class GrCamera;
class GrLight;

//**********************************************************
// class GrRenderLight
//**********************************************************
class GrRenderLight
{
public:
	GrRenderLight( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize );
	~GrRenderLight();

	void					SetWireframeEnable( bool wireframe )	{	_wireframe = wireframe;			}
	void					SetColorTextureEnable( bool enable )	{	_colorTextureEnable = enable;	}

	void					RenderLight( const GrRenderList& lightReceivers, const GrRenderList& shadowCasters,
										 const GrCamera& camera, const GrLight& light, unsigned int time, EGRQUALITY quality );

private:
	const GrMaterial*		GetMaterialSpan( unsigned int& endObj, const GrRenderList& list, unsigned int startObj );
	unsigned int			BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx,
										 unsigned int time, unsigned int vertexComponents );

	// sort compare.
	static bool				SortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right );

	GrRenderShadow			_renderShadow;

	GrStreamingIB*			_streamingIB;
	unsigned int			_maxIndexBatchSize;


	// default textures.
	URef< GrTextureBase >	_jitterVolume;
	URef< GrTextureBase >	_normalizeCube;
	URef< GrTextureBase >	_cubeLookupX;
	URef< GrTextureBase >	_cubeLookupY;
	URef< GrTextureBase >	_white;
	URef< GrTextureBase >	_black;
	URef< GrTextureBase >	_noShadow;
	URef< GrTextureBase >	_defaultNormal;
	URef< GrTextureBase >	_defaultBump;

	bool					_wireframe;
	bool					_colorTextureEnable;
};
