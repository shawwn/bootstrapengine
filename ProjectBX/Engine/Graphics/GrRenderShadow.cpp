//----------------------------------------------------------
// File:		GrRenderShadow.cpp
// Author:		Kevin Bray
// Created:		04-07-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderShadow.h"

// project includes.
#include "GrTextureMgr.h"
#include "GrTextureBase.h"
#include "GrRenderTargetMgr.h"
#include "GrRenderTarget.h"
#include "GrRenderbuffer.h"
#include "GrRenderUtil.h"
#include "GrShaderMgr.h"
#include "GrShader.h"
#include "GrCamera.h"
#include "GrImage.h"
#include "GrLight.h"
#include "GLSubsys.h"
#include "GrTypes.h"
#include "GrUtil.h"
#include "GrRenderList.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrMeshInst.h"
#include "GrStreamingIB.h"
#include "MQuat.h"
#include "GrDebug.h"
#include "GrVirtualShadowSegmentSet.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"


#define SHADOW_VERTEX_COMPONENTS_ALPHA	( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TANGENT_MASK | GR_ATTRIB_NORMAL_MASK | \
										  GR_ATTRIB_BINORMAL_MASK | GR_ATTRIB_TEXCOORD_MASK )
#define DEPTH_MERGE_SAMPLER_LAYER0		0
#define DEPTH_MERGE_SAMPLER_LAYER1		1
#define DEPTH_PEEL_SAMPLER_TOPLAYER		15

static const unsigned int kShadowAddressTransformParam = 1;

static const unsigned int kShadowPageSize = 2048;
static const unsigned int kShadowMapSize = 512;
static const unsigned int kVirtualCubeSize = 1024;
static const unsigned int kMapsPerPage = kShadowPageSize / kShadowMapSize;
static const float kPageUVIncrement = ( float )kShadowMapSize / ( float )kShadowPageSize;

//**********************************************************
// class GrRenderShadow
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderShadow::GrRenderShadow( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize )
: _shadowPage( kShadowPageSize, kShadowMapSize, kVirtualCubeSize )
, _shadowPageSize( kShadowPageSize )
, _shadowMapSize( kShadowMapSize )
, _streamingIB( streamingIB )
, _maxIndexBatchSize( maxIndexBatchSize )
{
	// get the default bump texture.
	_defaultBump = gGrTextureMgr->FindTexture( GR_SYSTEX_GRAY );

	// load the pass-through shader.
	_depth = gGrShaderMgr->GetShader( "VSM/depth" );
	_depthPeel = gGrShaderMgr->GetShader( "VSM/depthPeel" );
	_depthPeelAlpha = gGrShaderMgr->GetShader( "VSM/depthPeelAlpha" );
	_mergeDepth = gGrShaderMgr->GetShader( "VSM/depthMerge" );

	// setup samplers on our shaders.
	_depthPeel->BindUserSampler( "s_TopLayer", DEPTH_PEEL_SAMPLER_TOPLAYER );
	_depthPeelAlpha->BindUserSampler( "s_TopLayer", DEPTH_PEEL_SAMPLER_TOPLAYER );
	_mergeDepth->BindUserSampler( "s_Layer0", DEPTH_MERGE_SAMPLER_LAYER0 );
	_mergeDepth->BindUserSampler( "s_Layer1", DEPTH_MERGE_SAMPLER_LAYER1 );

	// allocate the shadow pages.
	// page 0.
	URef< GrTextureBase > shadow0 = gGrRenderTargetMgr->CreateRendertexture( _shadowMapSize, _shadowMapSize, GrRenderTargetMgr::EF_DEPTH,
		GrTextureBase::kNoMipMap, GrTextureBase::ETT_2D );
	_shadowCasterPage = gGrRenderTargetMgr->CreateRenderTarget( 0, shadow0 );

	// page 1.
	URef< GrTextureBase > shadow1 = gGrRenderTargetMgr->CreateRendertexture( _shadowMapSize, _shadowMapSize, GrRenderTargetMgr::EF_DEPTH,
		GrTextureBase::kNoMipMap, GrTextureBase::ETT_2D );
	_shadowReceiverPage = gGrRenderTargetMgr->CreateRenderTarget( 0, shadow1 );

	// turn off the hardware shadow test for shadow 0 and shadow 1.
	GrTexture* shadow2D0 = ( GrTexture* )( GrTextureBase* )shadow0;
	GrTexture* shadow2D1 = ( GrTexture* )( GrTextureBase* )shadow1;
	shadow2D0->Bind( 0 );
	shadow2D0->SetHWShadowEnable( false );
	shadow2D1->Bind( 0 );
	shadow2D1->SetHWShadowEnable( false );

	// page 2.
	URef< GrTextureBase > shadow2 = gGrRenderTargetMgr->CreateRendertexture( _shadowPageSize, _shadowPageSize, GrRenderTargetMgr::EF_DEPTH,
		GrTextureBase::kNoMipMap, GrTextureBase::ETT_2D );
	_shadowMidpointPage = gGrRenderTargetMgr->CreateRenderTarget( 0, shadow2 );

	// turn on the hardware shadow test for shadow 2 (the midpoint buffer).
	GrTexture* shadow2D2 = ( GrTexture* )( GrTextureBase* )shadow2;
	shadow2D2->Bind( 0 );
	shadow2D2->SetHWShadowEnable( true );

	// make sure everything is okay with the GL.
	CHECK_GL();
}

//----------------------------------------------------------
GrRenderShadow::~GrRenderShadow()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrTextureBase >
GrRenderShadow::RenderShadow( GrVirtualShadowPage::SRenderData& shadowData, const GrRenderList& shadowCasters,
							  const GrCamera& camera, const GrLight& light, unsigned int time, EGRQUALITY quality )
{
	// get the current render target.
	GrRenderTarget* prevRT = GrRenderTarget::GetCurrentRT();

	// begin rendering the shadow.
	GrVirtualShadowSegmentSet segments( camera, 10, 2.0f );
	_shadowPage.GenShadowInfo( _virtualShadowCache, camera, segments, light );

	// now go through all of the virtual shadows returned.
	unsigned int vsmCount = ( unsigned int )_virtualShadowCache.size();
	for ( unsigned int i = 0; i < vsmCount; ++i )
	{
		// render shadow casters.
		RenderShadowCasters( shadowCasters, _virtualShadowCache[ i ].camera, time, quality );

		// render shadow receivers.
		RenderShadowReceivers( shadowCasters, _virtualShadowCache[ i ].camera, time, quality );

#if 1
		bool saveCurFace = false;
		if ( saveCurFace )
		{
			char imageName[] = "shadowmap.tga";

			unsigned int dim = _shadowMapSize;
			unsigned char* pels = new unsigned char[ dim * dim ];
			bglReadPixels( 0, 0, dim, dim, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pels );
			GrSaveTGA( imageName, pels, dim, dim, 1 );
			saveCurFace = false;
		}
#endif

		// now render to the shadow page.
		_shadowMidpointPage->Bind( false );
		_shadowPage.SetActiveShadowMap( _virtualShadowCache[ i ] );
		GrSetWriteEnable( GR_DEPTH );
		GrSetState( GR_DEPTHTEST );
		GrSetDepthMode( EDM_ALWAYS );

		// now combine into the correct shadow map in the shadow page.
		CalcShadowMidpoint( quality );
	}

	// all done.
	shadowData = _shadowPage.Finish();

	// clear the VSM cache.
	_virtualShadowCache.resize( 0 );

	// bind the previous render target and clear the prevRT.
	prevRT->Bind();

	// all done rendering the shadow.
	return _shadowMidpointPage->GetDepthTex();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrRenderShadow::RenderShadowCasters( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
									 unsigned int time, EGRQUALITY quality )
{
	// bind the shadow caster page.
	_shadowCasterPage->Bind();

	// setup our initial state.
	GrSetState( GR_DEPTHTEST );
	GrSetWriteEnable( GR_DEPTH );
	GrSetDepthMode( EDM_LESSEQUAL );

	// clear the caster page.
	GrClear();

	// bind the depth shader.
	_depth->Bind( quality );

	// render regular objects.
	unsigned int startObj = 0;
	while ( startObj < shadowCasters.GetItemCount() )
	{
		// get the current material span.
		unsigned int endObj = 0;
		const GrMaterial* material = GetShadowSpan( endObj, shadowCasters, startObj );

		// skip the current object if the material does not cast a shadow.
		if ( !material->CastShadow() )
		{
			startObj = endObj;
			continue;
		}

		// for each pass, render all objects.
		const unsigned int passCount = material->GetPassCount();
		for ( unsigned int pass = 0; pass < passCount; ++pass )
		{
			// get the current pass.
			const GrMaterialPass* curPass = material->GetPass( pass );

			// skip the current pass if it doesn't cast a shadow.
			if ( !curPass->CastShadow() )
				continue;

			// apply states used by all objects.
			curPass->ApplyStates( GR_ALPHATEST | GR_CULLFACE, GR_DEPTH );

			// apply any states necessary.
			bool alphaTest = curPass->AlphaTest();
			if ( alphaTest )
			{
				// apply relevant states.
				curPass->GetTextureSet().ApplyGlobalState( _depth );
				curPass->GetTextureSet().ApplyStageState( ES_BUMP, GR_TEX_BUMP, _depth, GrShader::EP_BUMP_MOD, GrShader::EP_BUMP_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE, GR_TEX_DIFFUSE, _depth, GrShader::EP_DIFFUSE_MOD, GrShader::EP_DIFFUSE_ADD );
			}

			// render the objects.
			RenderObjects( shadowCasters, startObj, endObj, shadowCamera, _depth, time, alphaTest );

			// if we hit a pass that doesn't have alpha test, then we've rendered
			// all depth information.  At this point, doing another pass would be
			// useless.
			if ( !alphaTest )
				break;
		}

		// move to the next material.
		startObj = endObj;
	}
}

//----------------------------------------------------------
void
GrRenderShadow::RenderShadowReceivers( const GrRenderList& shadowReceivers, const GrCamera& shadowCamera,
									   unsigned int time, EGRQUALITY quality )
{
	_shadowReceiverPage->Bind();

	// setup our initial state.
	GrSetWriteEnable( GR_DEPTH );
	GrSetState( GR_DEPTHTEST );
	GrSetDepthMode( EDM_LESSEQUAL );

	// clear the receiver page.
	GrClear();

	// bind the shadow casters as the depth-peel layer.
	_shadowCasterPage->GetDepthTex()->Bind( DEPTH_PEEL_SAMPLER_TOPLAYER );

	// render regular objects.
	GrShader* curShader = 0;
	unsigned int startObj = 0;
	while ( startObj < shadowReceivers.GetItemCount() )
	{
		// get the current material span.
		unsigned int endObj = 0;
		const GrMaterial* material = GetShadowSpan( endObj, shadowReceivers, startObj );

		// skip the current object if the material does not receive a shadow.
		if ( !material->ReceiveShadow() )
		{
			startObj = endObj;
			continue;
		}

		// for each pass, render all objects.
		const unsigned int passCount = material->GetPassCount();
		for ( unsigned int pass = 0; pass < passCount; ++pass )
		{
			// get the current pass.
			const GrMaterialPass* curPass = material->GetPass( pass );

			// skip the current pass if it doesn't receive a shadow.
			if ( !curPass->ReceiveShadow() )
				continue;

			// apply states used by all objects.
			curPass->ApplyStates( GR_ALPHATEST | GR_CULLFACE, GR_DEPTH );

			// apply any states necessary.
			bool alphaTest = curPass->AlphaTest();
			if ( alphaTest )
			{
				if ( curShader != _depthPeelAlpha )
				{
					_depthPeelAlpha->Bind( quality );
					curShader = _depthPeelAlpha;
				}

				// apply relevant states.
				curPass->GetTextureSet().ApplyGlobalState( _depthPeelAlpha );
				curPass->GetTextureSet().ApplyStageState( ES_BUMP, GR_TEX_BUMP, curShader, GrShader::EP_BUMP_MOD, GrShader::EP_BUMP_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE, GR_TEX_DIFFUSE, curShader, GrShader::EP_DIFFUSE_MOD, GrShader::EP_DIFFUSE_ADD );
			}
			else
			{
				if ( curShader != _depthPeel )
				{
					_depthPeel->Bind( quality );
					curShader = _depthPeel;
				}
			}

			RenderObjects( shadowReceivers, startObj, endObj, shadowCamera, curShader, time, alphaTest );

			// if we hit a pass that doesn't have alpha test, then we've rendered
			// all depth information.  At this point, doing another pass would be
			// pointless.
			if ( !alphaTest )
				break;
		}

		// move to the next material.
		startObj = endObj;
	}
}

//----------------------------------------------------------
void
GrRenderShadow::RenderShadowMap( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
								 GrShader* depthShader, GrShader* alphaTestShader, unsigned int writeMask,
								 unsigned int time, EGRQUALITY quality, bool honorCastShadow )
{
	// setup our initial state.
	GrSetState( GR_DEPTHTEST );
	GrSetWriteEnable( writeMask );
	GrSetDepthMode( EDM_LESSEQUAL );

	// render the shadow faces.
	RenderShadowFaces( shadowCasters, shadowCamera, depthShader, alphaTestShader, writeMask, time,
		quality, honorCastShadow );
}

//----------------------------------------------------------
void
GrRenderShadow::RenderShadowFaces( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
								   GrShader* depthShader, GrShader* alphaTestShader, unsigned int writeMask,
								   unsigned int time, EGRQUALITY quality, bool honorCastShadow )
{
	// get the projection matrix so we can screw with the near clip
	// plane and get a virtual bias.
	MMat4x4 projMat = shadowCamera.GetProj().GetMatrix();

	// render regular objects.
	GrShader* curShader = 0;
	unsigned int startObj = 0;
	while ( startObj < shadowCasters.GetItemCount() )
	{
		// get the current material span.
		unsigned int endObj = 0;
		const GrMaterial* material = GetShadowSpan( endObj, shadowCasters, startObj );

		// skip the current object if the material does not cast a shadow.
		if ( !material->CastShadow() && honorCastShadow )
		{
			startObj = endObj;
			continue;
		}

		// for each pass, render all objects.
		const unsigned int passCount = material->GetPassCount();
		for ( unsigned int pass = 0; pass < passCount; ++pass )
		{
			// get the current pass.
			const GrMaterialPass* curPass = material->GetPass( pass );

			// skip the current pass if it doesn't cast a shadow.
			if ( !curPass->CastShadow() && honorCastShadow )
				continue;

			// apply states used by all objects.
			curPass->ApplyStates( GR_ALPHATEST | GR_CULLFACE, GR_DEPTH );

			// apply any states necessary.
			bool alphaTest = curPass->AlphaTest();
			if ( alphaTest )
			{
				if ( !alphaTestShader )
				{
					continue;
				}

				if ( curShader != alphaTestShader )
				{
					alphaTestShader->Bind( quality );
					curShader = alphaTestShader;
				}

#if GR_OPTION_ALPHATEST_COLOR_WRITE
				// turn on color writes to deal with a bug on some cards.
				GrSetWriteEnable( writeMask | GR_RGBA );
#endif

				// apply relevant states.
				curPass->GetTextureSet().ApplyGlobalState( alphaTestShader );
				curPass->GetTextureSet().ApplyStageState( ES_BUMP, GR_TEX_BUMP, curShader, GrShader::EP_BUMP_MOD, GrShader::EP_BUMP_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE, GR_TEX_DIFFUSE, curShader, GrShader::EP_DIFFUSE_MOD, GrShader::EP_DIFFUSE_ADD );
			}
			else
			{
				if ( curShader != depthShader )
				{
					depthShader->Bind( quality );
					curShader = depthShader;
				}

#if GR_OPTION_ALPHATEST_COLOR_WRITE
				// note that we only have to reset the write mask if we turned on
				// color writes.
				GrSetWriteEnable( writeMask );
#endif
			}

			RenderObjects( shadowCasters, startObj, endObj, shadowCamera, curShader, time, alphaTest );

			// if we hit a pass that doesn't have alpha test, then we've rendered
			// all depth information.  At this point, doing another pass would be
			// useless.
			if ( !alphaTest )
				break;
		}

		// move to the next material.
		startObj = endObj;
	}
}

//----------------------------------------------------------
void
GrRenderShadow::RenderObjects( const GrRenderList& shadowCasters, unsigned int start, unsigned int end,
							   const GrCamera& shadowCamera, GrShader* curShader, unsigned int time, bool alphaTest )
{
	// shade objects until we hit an object that does not use the current
	// material.
	for ( unsigned int obj = start; obj < end; ++obj )
	{
		// get the current renderable.
		const GrRenderList::SItem& renderable = shadowCasters[ obj ];
		if ( renderable.preTransformed )
		{
			// apply parameters needed when alpha testing is enabled.
			if ( alphaTest )
				curShader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, shadowCamera.GetPos() );

			// render the shadow.
			curShader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, shadowCamera.GetViewProjMatrix() );
			if ( renderable.meshInst->GetMesh()->GetRange( renderable.range ).count < _maxIndexBatchSize )
				obj = BatchRender( shadowCasters, obj, end, time, SHADOW_VERTEX_COMPONENTS_ALPHA );
			else
				renderable.meshInst->RenderRange( renderable.range, SHADOW_VERTEX_COMPONENTS_ALPHA, time );
		}
		else
		{
			// calculate local space parameters.
			const MMat4x4& world = renderable.meshInst->GetTransform();

			// apply parameters for alpha testing if it's enabled.
			const MMat4x4& invWorld = renderable.meshInst->GetInvTransform();
			if ( alphaTest )
			{
				MVec3 localEyePos = invWorld.TransformCoord( shadowCamera.GetPos() );
				curShader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, localEyePos );
			}

			// render the object.
			curShader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, shadowCamera.GetViewProjMatrix() * world );
			renderable.meshInst->RenderRange( renderable.range, SHADOW_VERTEX_COMPONENTS_ALPHA, time );
		}
	}
}

//----------------------------------------------------------
void
GrRenderShadow::CalcShadowMidpoint( EGRQUALITY quality )
{
	// setup textures.
	_mergeDepth->Bind( quality );
	_shadowCasterPage->GetDepthTex()->Bind( DEPTH_MERGE_SAMPLER_LAYER0 );
	_shadowReceiverPage->GetDepthTex()->Bind( DEPTH_MERGE_SAMPLER_LAYER1 );

	// draw a quad.
	gGrRenderUtil->DrawScreenQuad();
//	GrDrawScreenQuad( GR_ATTRIB_TEXCOORD_INDEX );
}

//----------------------------------------------------------
const GrMaterial*
GrRenderShadow::GetShadowSpan( unsigned int& spanEnd, const GrRenderList& list, unsigned int startObj )
{
	// this is a specialized version of GetMaterialSpan.  It will return a run of
	// materials that have compatible shadows (alpha-test, pretransformed).
	B_ASSERT( startObj < list.GetItemCount() );
	const GrMaterial* curMaterial = list[ startObj ].meshInst->GetMaterial( list[ startObj ].range );
	spanEnd = startObj+1;
	while ( spanEnd < list.GetItemCount() )
	{
		const GrMaterial* material2 = list[ spanEnd ].meshInst->GetMaterial( list[ spanEnd ].range );
		if ( !IsCompatibleShadow( curMaterial, material2 ) )
			break;
		else
			curMaterial = material2;
		++spanEnd;
	}
	return curMaterial;
}

//----------------------------------------------------------
bool
GrRenderShadow::IsCompatibleShadow( const GrMaterial* material1, const GrMaterial* material2 )
{
	// check early out conditions.
	if ( material1 == material2 )
		return true;

	// if they're both alpha test or non-alpha test and both cast a shadow.
	return material1->AlphaTest() == material2->AlphaTest() && material1->CastShadow() == material2->CastShadow();
}

//----------------------------------------------------------
unsigned int
GrRenderShadow::BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx, unsigned int time,
							 unsigned int vertexComponents )
{
	// make sure the start object is pre-transformed, that it's not skinned (should
	// be impossible?), and that it doesn't have any deformers associated with it.
	B_ASSERT( renderableArray[ startIdx ].preTransformed );
	B_ASSERT( renderableArray[ startIdx ].meshInst->GetSkinInst() == 0 );
	B_ASSERT( renderableArray[ startIdx ].meshInst->GetMesh()->GetIndexData() != 0 );

	// make sure the current range isn't too big.
	B_ASSERT( renderableArray[ startIdx ].meshInst->GetMesh()->GetRange( renderableArray[ startIdx ].range ).count < _maxIndexBatchSize )

	// get the mesh data from the mesh instance.
	URef< GrMesh > mesh = renderableArray[ startIdx ].meshInst->GetMesh();
	unsigned int minIndex = 0xFFFF;
	unsigned int maxIndex = 0;
	unsigned int count = 0;

	// lock our index buffer.
	_streamingIB->Bind();
	unsigned int offset = _streamingIB->GetOffset();

	// batch everything until we hit an object that is either not pre-transformed,
	// or we hit the end index.
	do
	{
		// get the current renderable.
		const GrRenderList::SItem& curRenderable = renderableArray[ startIdx ];

		// check to see if there is too much data in the range to batch.
		if ( ( count + mesh->GetRange( curRenderable.range ).count ) > _maxIndexBatchSize )
			break;

		// batch the current range.
		count += curRenderable.meshInst->BatchRange( *_streamingIB, curRenderable.range, time );

		// update min/max.
		minIndex = Min( mesh->GetRange( curRenderable.range ).minIndex, minIndex );
		maxIndex = Max( mesh->GetRange( curRenderable.range ).maxIndex, maxIndex );

		// next object.
		++startIdx;
	}
	while (
		startIdx < endIdx &&
		renderableArray[ startIdx ].preTransformed &&
		renderableArray[ startIdx ].meshInst->GetMesh() == mesh
		);

	// make sure all is well with GL.
	CHECK_GL();

	// now we need to bind the vertex data for rendering.
	mesh->GetVertexData()->Bind( vertexComponents );
	CHECK_GL();

	// render data.
	bglDrawRangeElements( 
		GL_TRIANGLES,
		minIndex,
		maxIndex,
		count,
		GL_UNSIGNED_SHORT,
		( ( unsigned short* )0 ) + offset );
	CHECK_GL();

	// return the ending index.
	return startIdx-1;
}
