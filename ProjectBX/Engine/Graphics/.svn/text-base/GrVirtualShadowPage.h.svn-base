//----------------------------------------------------------
// File:		GrVirtualShadowPage.h
// Author:		Kevin Bray
// Created:		04-12-07
//
// Purpose:		To manage a page of shadow maps.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrCamera.h"
#include "GrTypes.h"
#include "GrTextureCube.h"
#include "GrVirtualShadowSegmentSet.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrRenderTarget;
class GrLight;
class GrShader;
class GrTextureBase;
class GrVirtualShadowSegmentSet;

//**********************************************************
// class GrVirtualShadowPage
//**********************************************************
class GrVirtualShadowPage
{
public:
	struct SShadowInfo
	{
		GrCamera		camera;
		int				shadowMapIdx;
		int				width;
		int				height;
	};

	struct SRenderData
	{
		URef< GrTextureBase > indirCube;
		MMat4x4 shadowMatrix;
	};

	GrVirtualShadowPage( unsigned int shadowPageSize, unsigned int shadowMapSize, unsigned int indirTexSize );
	~GrVirtualShadowPage();

	// functions that enable rendering to the shadow map.
	void					GenShadowInfo( std::vector< SShadowInfo >& shadows, const GrCamera& viewerCamera,
								   GrVirtualShadowSegmentSet& segments, const GrLight& light );

	void					SetActiveShadowMap( const SShadowInfo& shadowMapInfo );

	const SRenderData&		Finish();
	const SRenderData&		GetNoShadowData() const			{	return _noShadowData;		}

private:
	// 2D Axis-Aligned box
	struct SAABox2D
	{
		SVec2 lowerLeft;
		SVec2 upperRight;
		float z;
	};

	void					GetOrderedFaces( GrTextureCube::ECUBEFACE* faceLut, const GrCamera& viewerCamera, const GrLight& light );
	unsigned int			GetLastSegment( const GrVirtualShadowSegmentSet& segments );
	unsigned int			AllocateShadowMaps( std::vector< SShadowInfo >& shadows, const GrVirtualShadowSegmentSet& segments,
												const GrCamera& indirCamera, unsigned int startShadowMapIdx, unsigned int maxShadowIdx );
	void					AllocateShadowMap( SShadowInfo& shadowInfo, const GrVirtualShadowSegmentSet& segments,
											   unsigned int segment, const GrCamera& indirCamera, unsigned int shadowMapIndex );
	void					RenderSegmentInfo( const GrVirtualShadowSegmentSet::SAABox2D& aaBox, unsigned int shadowMapIdx,
											   unsigned int width, unsigned int height );

	void					CalcLightCubeCameras( GrCamera* cameras, const GrLight& light );
	void					CalcProjCamera( GrCamera& camera, const GrLight& light );
	void					CalcShadowModAdd( SVec2& mod, SVec2& add, unsigned int shadowMapIdx, unsigned int width, unsigned int height );

	MMat4x4					_texGenBias;
	URef< GrRenderTarget >	_shadowIndirCube;
	URef< GrRenderTarget >	_shadowPage;
	GrRenderTarget*			_prevRT;
	URef< GrShader >		_shadowIndirShader;
	SRenderData				_shadowData;
	SRenderData				_noShadowData;
	unsigned int			_shadowPageSize;
	unsigned int			_shadowMapCount;
	unsigned int			_shadowMapSize;
	unsigned int			_shadowMapBorder;
	float					_texAddrStep;
};
