//----------------------------------------------------------
// File:		GrRenderer.cpp
// Author:		Kevin Bray
// Created:		11-27-05 ( happy bday to me! )
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSceneRender.h"

// project includes.
#include "GrRenderer.h"
#include "GrTypes.h"
#include "MMat3x3.h"
#include "GrBSPTree.h"
#include "GrModel.h"
#include "GrLight.h"
#include "MVec3.h"
#include "GrCamera.h"
#include "GrShadowBuffer.h"
#include "GLSubsys.h"
//#include "MMat4x4.h"

GrSceneRender* gGrSceneRender = 0;

//**********************************************************
// class GrSceneRender
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSceneRender::GrSceneRender()
: _shadowEnable( true )
{
	B_ASSERT( gGrSceneRender == 0 );
	gGrSceneRender = this;
}

//----------------------------------------------------------
GrSceneRender::~GrSceneRender()
{
	gGrSceneRender = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrSceneRender::RenderAmbience( const GrCamera* camera, const MVec3& pvsEyePos, const GrBSPTree* bspRoot, const GrModel* modelRoot,
							   const GrModel* skyboxRoot, const GrColor& ambColor, unsigned int frameId )
{
	if ( bspRoot )
		bspRoot->GetLitObjectsAndMarkVisible( _lightReceivers, camera->GetFrustum(), pvsEyePos, frameId );
	if ( modelRoot )
		modelRoot->GetLitObjectsAndMarkVisible( _lightReceivers, camera->GetFrustum(), frameId );
	if ( skyboxRoot )
		skyboxRoot->GetLitObjectsAndMarkVisible( _lightReceivers, camera->GetFrustum(), frameId );

	// accumulate ambience.  This should be sorted in the order it was added to the render list.
	gGrRenderer->RenderAmbience( _lightReceivers, *camera, ambColor );
	_lightReceivers.Clear();
}

//----------------------------------------------------------
void
GrSceneRender::RenderLight( const GrCamera* camera, const GrLight* light, const GrBSPTree* bspRoot, const GrModel* modelRoot,
							unsigned int frameId )
{
	B_ASSERT( light != 0 );

	// check to see if the light is even visible.
	const MVec3& lightPos = light->GetPos();
	float range = light->GetCachedRange();
	MSphere lightBounds( lightPos, range );
	if ( range < 0.1f || light->GetCachedIntensity() < 0.01f )
		return;

	// send data to the renderer.
	GrRenderList* shadowCasters = light->CastShadow() ? &_shadowCasters : 0;
	if ( light->GetProjTexture() != 0 || light->IsParallel() )
	{
		// build a frustum using the light.
		GrCamera projCam( light->GetPos(), light->GetWorldTransform().GetRotate(), light->GetProjection() );

		// return if the frustum isn't visible.
		if ( !camera->GetFrustum().IsFrustumInside( projCam.GetFrustum() ) && !light->IsParallel() )
			return;

		// send data to the renderer.
		if ( bspRoot )
			bspRoot->GetLitObjects( _lightReceivers, shadowCasters, projCam.GetFrustum(), frameId );
		if ( modelRoot )
			modelRoot->GetLitObjects( _lightReceivers, shadowCasters, projCam.GetFrustum(), frameId );
	}
	else
	{
		// return if the sphere of influence isn't visible.
		if ( !camera->GetFrustum().IsSphereInside( lightBounds ) )
			return;

		// send data to the renderer.
		if ( bspRoot )
			bspRoot->GetLitObjects( _lightReceivers, shadowCasters, light->GetWorldBounds(), frameId );
		if ( modelRoot )
			modelRoot->GetLitObjects( _lightReceivers, shadowCasters, light->GetWorldBounds(), frameId );
	}

	// end accumulating the current light.
	gGrRenderer->RenderLight( _lightReceivers, _shadowCasters, *camera, *light );

	// clear lists.
	_lightReceivers.Clear();
	_shadowCasters.Clear();
}
