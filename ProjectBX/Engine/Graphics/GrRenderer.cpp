//----------------------------------------------------------
// File:		GrRenderer.cpp
// Author:		Kevin Bray
// Created:		01-04-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderer.h"

// project includes.
#include "GrShaderMgr.h"
#include "GrShader.h"
#include "GrTextureMgr.h"
#include "GrTexture3D.h"
#include "GrTextureCube.h"
#include "GrTexture.h"
#include "GrMeshInst.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrUtil.h"
#include "GLSubsys.h"
#include "GrCamera.h"
#include "GrLight.h"
#include "MMat3x3.h"
#include "GrShadowBuffer.h"
#include "GrMeshBufferMgr.h"
#include "GrMeshIB.h"
#include "GrRenderTarget.h"
#include "GrFramebuffer.h"
#include "GrImage.h"

//#define __CPU_SHADOW
#ifdef __CPU_SHADOW
unsigned int _shadowMaskGLTex = 0;
#endif

GrRenderer* gGrRenderer = 0;


#define MAX_INDEX_BATCH_SIZE		1048576

// GL support flags.
const unsigned int		kDepthBoundsTest = 0x0001;

// texgen matrix for rendering with no shadow.
static const MMat4x4 kNoShadowTexgen( 0.0f, 0.0f, 0.0f, 0.0f,
									  0.0f, 0.0f, 0.0f, 0.0f,
									  0.0f, 0.0f, 0.0f, 0.0f,
									  0.0f, 0.0f, 0.0f, 1.0f );

// texgen matrix for rendering with shadow.
static const MMat4x4 kProjBias( 0.5f, 0.0f, 0.0f, 0.5f,
								0.0f, 0.5f, 0.0f, 0.5f,
								0.0f, 0.0f, 0.5f, 0.5f,
								0.0f, 0.0f, 0.0f, 1.0f );

#define POST_PROCESS_VERTEX_COMPONENTS	( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TANGENT_MASK | GR_ATTRIB_NORMAL_MASK | \
										  GR_ATTRIB_BINORMAL_MASK | GR_ATTRIB_TEXCOORD_MASK )

//**********************************************************
// class GrRenderer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderer::GrRenderer( unsigned int screenWidth, unsigned int screenHeight, EGRQUALITY initQuality )
: _streamingIB( 2095152, 524288 )
, _gameTime( 0 )
, _sysTime( 0 )
, _quality( initQuality )
, _wireframe( false )
, _colorTextureEnable( true )
, _renderAmbience( &_streamingIB, MAX_INDEX_BATCH_SIZE )
, _renderLight( &_streamingIB, MAX_INDEX_BATCH_SIZE )
{
	B_ASSERT( gGrRenderer == 0 );
	gGrRenderer = this;

	// get the normalization texture cube.
	GrTextureBase* tex = gGrTextureMgr->FindTexture( GR_SYSTEXCUBE_NORMALIZE );
	B_ASSERT( tex != 0 && tex->GetType() == GrTextureBase::ETT_CUBE );
	_normalizeCube = ( GrTextureCube* )tex;
}

//----------------------------------------------------------
GrRenderer::~GrRenderer()
{
	gGrRenderer = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderer::Update( unsigned int gameTimeDelta, unsigned int sysTimeDelta )
{
	_gameTime += gameTimeDelta;
	_sysTime += sysTimeDelta;
}

//----------------------------------------------------------
void
GrRenderer::BeginRender()
{

}

//----------------------------------------------------------
void
GrRenderer::RenderAmbience( const GrRenderList& objects, const GrCamera& camera, const GrColor& ambientColor )
{
	_renderAmbience.SetWireframeEnable( _wireframe );
	_renderAmbience.SetColorTextureEnable( _colorTextureEnable );
	_renderAmbience.RenderAmbience( _postRenderObjects, objects, camera, ambientColor, _gameTime, _quality );
}

//----------------------------------------------------------
void
GrRenderer::RenderLight( const GrRenderList& lightReceivers, const GrRenderList& shadowCasters, const GrCamera& camera, const GrLight& light )
{
	// make sure all is well with GL.
	CHECK_GL();

	// is there anything to do?
	if ( !lightReceivers.GetItemCount() )
		return;

	_renderLight.RenderLight( lightReceivers, shadowCasters, camera, light, _gameTime, _quality );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrRenderer::EndRender( const GrCamera& camera )
{
	CHECK_GL();

	// get the number of objects that require a post frame step.
	unsigned int postRenderObjCount = _postRenderObjects.GetItemCount();
	if ( !postRenderObjCount )
	{
		_streamingIB.Reset();
		return;
	}

	// setup all render states.
	GrSetState( GR_DEPTHTEST | GR_CULLFACE );
	GrSetDepthMode( EDM_LESSEQUAL );
	GrSetCullMode( ECM_BACK );
	GrSetWriteEnable( GR_RGBA );

	// get the current viewport.
	int vpX = 0;
	int vpY = 0;
	int vpWidth = 0;
	int vpHeight = 0;
	GrGetViewport( vpX, vpY, vpWidth, vpHeight );

	// calculate useful information.
	const MVec3& worldCameraPos = camera.GetPos();

	// bind textures.
	_normalizeCube->Bind( GR_TEX_CUBE_NORMALIZE, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// process all objects in the postRender array.
	GrShader* shader = 0;
	GrMaterial* prevMaterial = 0;
	const unsigned int stateMask = ~GR_WIREFRAME;
	bool needNextPass = false;
	unsigned int pass = 0;
	do
	{
		// clear the needNextPass flag.
		needNextPass = false;

		// get the current color/depth.  This paradigm allows the current render
		// to be available to each pass.
		URef< GrTextureBase	> curRender = gGrFramebuffer->CopyCurrentRender( vpX, vpY, vpWidth, vpHeight );
		URef< GrTextureBase	> curDepth = gGrFramebuffer->CopyCurrentDepth( vpX, vpY, vpWidth, vpHeight );
		curRender->Bind( GR_TEX_CUR_RENDER, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		curDepth->Bind( GR_TEX_CUR_DEPTH, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

		// go through all objects.
		unsigned int startObj = 0;
		unsigned int objCount = _postRenderObjects.GetItemCount();
		while ( startObj < objCount )
		{
			// get the current material span.
			unsigned int endObj = 0;
			const GrMaterial* material = GetMaterialSpan( endObj, _postRenderObjects, startObj );

			// for each post-process pass in the current material, render all objects in our span.
			if ( pass < material->GetPostProcessPassCount() )
			{
				// check to see if we need another pass.
				needNextPass = needNextPass || ( ( material->GetPostProcessPassCount() - 1 ) > pass );

				// get the current pass for rendering and apply texture stages.
				const GrMaterialPass* curPass = material->GetPostProcessPass( pass );

				// apply the current pass's post-render shader.
				shader = curPass->GetShader();
				B_ASSERT( shader );

				// bind the shader.
				shader->Bind( _quality );

				// apply texture stages and vertex params.
				curPass->ApplyStates( stateMask, GR_RGBA | GR_DEPTH );
				curPass->ApplyAmbientBlendStates( stateMask );
				curPass->GetUserParams().ApplyUserParams();
				curPass->GetUserParams().ApplyUserSamplers();

				// apply relevant pass texture stages.
				curPass->GetTextureSet().ApplyGlobalState( shader );
				curPass->GetTextureSet().ApplyStageState( ES_BUMP, GR_TEX_BUMP, shader, GrShader::EP_BUMP_MOD, GrShader::EP_BUMP_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_NORMAL, GR_TEX_NORMAL );
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE, GR_TEX_DIFFUSE, shader, GrShader::EP_DIFFUSE_MOD, GrShader::EP_DIFFUSE_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_SPECULAR, GR_TEX_SPECULAR, shader, GrShader::EP_SPECULAR_MOD, GrShader::EP_SPECULAR_ADD );

				// render the objects.
				for ( unsigned int obj = startObj; obj < endObj; ++obj )
				{
					// get the current renderable.
					const GrRenderList::SItem& renderable = _postRenderObjects[ obj ];

					// setup object specific gentextures.
//					URef< GrTextureBase > reflectTex = renderable.meshInst->GetReflection( renderable.range );
//					if ( reflectTex )
//						reflectTex->Bind( GR_TEX_REFLECTION, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

					// now render the current object with the post-render shader.
					if ( renderable.preTransformed )
					{
						shader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, camera.GetViewProjMatrix() );
						shader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, worldCameraPos );
						renderable.meshInst->RenderRange( renderable.range, POST_PROCESS_VERTEX_COMPONENTS, _gameTime );
					}
					else
					{
						// calculate local space parameters.
						const MMat4x4& world = renderable.meshInst->GetTransform();
						const MMat4x4& invWorld = renderable.meshInst->GetInvTransform();
						MVec3 localEyePos = invWorld.TransformCoord( worldCameraPos );

						// apply local space parameters.
						shader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, localEyePos );
						shader->SetEngineParam4x4fv( GrShader::EP_LOCAL_VIEW_POSITION, camera.GetViewProjMatrix() * world );

						// render the object.
						renderable.meshInst->RenderRange( renderable.range, POST_PROCESS_VERTEX_COMPONENTS, _gameTime );
					}
				}
			}
			startObj = endObj;
		}

		// next pass.
		++pass;
	} while ( needNextPass );

	// clear the post render list if there is anything in it.
	_postRenderObjects.Clear();

	// reset our streaming index buffer.
	_streamingIB.Reset();
}

//----------------------------------------------------------
void
GrRenderer::PreContextReset()
{

}

//----------------------------------------------------------
void
GrRenderer::PostContextReset( unsigned int screenWidth, unsigned int screenHeight )
{

}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
const GrMaterial*
GrRenderer::GetMaterialSpan( unsigned int& spanEnd, const GrRenderList& list, unsigned int startObj )
{
	B_ASSERT( startObj < list.GetItemCount() );
	const GrMaterial* curMaterial = list[ startObj ].meshInst->GetMaterial( list[ startObj ].range );
	spanEnd = startObj+1;
	while ( spanEnd < list.GetItemCount() && curMaterial == list[ spanEnd ].meshInst->GetMaterial( list[ spanEnd ].range ) )
		++spanEnd;
	return curMaterial;
}
