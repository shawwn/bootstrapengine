//----------------------------------------------------------
// File:		GrRenderLight.cpp
// Author:		Kevin Bray
// Created:		12-09-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderLight.h"

// project includes.
#include "GrTextureBase.h"
#include "GrTextureMgr.h"
#include "GrRenderTarget.h"
#include "GrStreamingIB.h"
#include "GrLight.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrMeshInst.h"
#include "GrMesh.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrRenderList.h"
#include "GrCamera.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

#define LIGHT_VERTEX_COMPONENTS			( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TANGENT_MASK | GR_ATTRIB_NORMAL_MASK | \
										  GR_ATTRIB_BINORMAL_MASK | GR_ATTRIB_TEXCOORD_MASK )

//**********************************************************
// class GrRenderLight
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderLight::GrRenderLight( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize )
: _renderShadow( streamingIB, maxIndexBatchSize )
, _streamingIB( streamingIB )
, _maxIndexBatchSize( maxIndexBatchSize )
, _wireframe( false )
, _colorTextureEnable( true )
{
	// get the default white texture.
	_white = gGrTextureMgr->FindTexture( GR_SYSTEX_WHITE );

	// get the normalization cube map.
	_normalizeCube = gGrTextureMgr->FindTexture( GR_SYSTEXCUBE_NORMALIZE );

	// get the cube lookup X.
	_cubeLookupX = gGrTextureMgr->FindTexture( GR_SYSTEX_CUBE_X );

	// get the cube lookup Y.
	_cubeLookupY = gGrTextureMgr->FindTexture( GR_SYSTEX_CUBE_Y );

	// get the default black texture.
	_black = gGrTextureMgr->FindTexture( GR_SYSTEX_BLACK );

	// get the default bump texture.
	_defaultBump = gGrTextureMgr->FindTexture( GR_SYSTEX_GRAY );

	// get the default normal texture.
	_defaultNormal = gGrTextureMgr->FindTexture( GR_SYSTEX_NORMAL );

	// get the jitter texture.
	_jitterVolume = gGrTextureMgr->FindTexture( GR_SYSTEX3D_JITTER );

	// get the no shadow texture.
	_noShadow = gGrTextureMgr->FindTexture( GR_SYSTEX_DEPTH );
}

//----------------------------------------------------------
GrRenderLight::~GrRenderLight()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderLight::RenderLight( const GrRenderList& lightReceivers, const GrRenderList& shadowCasters,
							const GrCamera& camera, const GrLight& light, unsigned int time, EGRQUALITY quality )
{
	// sort the shadow casters and adjustedLight recievers by render state.
	shadowCasters.Sort( SortCmp );
	lightReceivers.Sort( SortCmp );

	// update the adjustedLight's position if it's supposed to track the
	// camera.
	GrLight adjustedLight = light;
	if ( adjustedLight.GetGlobal() )
		adjustedLight.SetParentTransform( MMat4x4( camera.GetPos() ) );

	// generate the shadow if necessary.
	URef< GrTextureBase > shadow;
	GrVirtualShadowPage::SRenderData shadowData;
	if ( adjustedLight.CastShadow() )
	{
		shadow = _renderShadow.RenderShadow( shadowData, shadowCasters, camera, adjustedLight, time, quality );
	}
	else
	{
		shadow = _noShadow;
		shadowData = _renderShadow.GetNoShadowData();
	}

	// now render the adjustedLight.
	CHECK_GL();

	// render the adjustedLight region passed in.
	// get the projected texture.
	URef< GrTextureBase > projTex = adjustedLight.GetProjTexture();
	if ( !projTex )
		projTex = _white;

	// get/calculate adjustedLight information.
	const float kLightRange = adjustedLight.GetCachedRange();
	const MVec3 kWorldLightPos = adjustedLight.GetPos();
	bool kLightParallel = adjustedLight.IsParallel();
	const MVec3& kWorldCameraPos = camera.GetPos();
	const GrColor& kLightColor = adjustedLight.GetCachedColor();
	const float kShadowWidth = adjustedLight.GetWidth() / Max( adjustedLight.GetProjection().GetWidth(), adjustedLight.GetProjection().GetHeight() );
	const float kShadowWidthParams[ 4 ] = { kShadowWidth, kShadowWidth, -kShadowWidth, 0.0f };
	const float kLightHaze[ 4 ] = { adjustedLight.GetHaziness(), 1.0f - adjustedLight.GetHaziness(), 0.0f, 0.0f };
	const float kLightFalloff[ 4 ] = { adjustedLight.GetSqrAtten(), adjustedLight.GetLinAtten(), 0.0f, adjustedLight.GetConstAtten() };
	const float kLightRangeParams[ 4 ] = { kLightRange, kLightRange, kLightRange, 1.0f / kLightRange };
	const MMat4x4& kProjTexGenMatrix = adjustedLight.GetTexGenMatrix();
	float kRadialDirMask = kLightParallel ? 0.0f : 1.0f;
	float kInvRadialDirMask = kLightParallel ? 1.0f : 0.0f;
	const MVec3 kWorldLightDir = kInvRadialDirMask * adjustedLight.GetDir();
	const float kWorldLightDirOverride[ 4 ] = { kWorldLightDir.GetX(), kWorldLightDir.GetY(), kWorldLightDir.GetZ(), kRadialDirMask };
	const float shadowNearD = ( adjustedLight.IsParallel() || ( adjustedLight.GetProjection().IsOrtho() && adjustedLight.GetProjTexture() != 0 ) ) ? 0.0f : adjustedLight.GetProjection().GetNearClip().GetD();
	const float kShadowNearClip[ 4 ] = { shadowNearD, shadowNearD, shadowNearD, shadowNearD };

	// apply global render states.
	GrSetState( GR_DEPTHTEST | ( _wireframe ? GR_WIREFRAME : 0 ) );
	GrSetWriteEnable( GR_RGBA );
	GrSetDepthMode( EDM_EQUAL );

	// bind global textures.
	_normalizeCube->Bind( GR_TEX_CUBE_NORMALIZE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	_cubeLookupX->Bind( GR_TEX_CUBE_FACE_X, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	_cubeLookupY->Bind( GR_TEX_CUBE_FACE_Y, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	_jitterVolume->Bind( GR_TEX_JITTER, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT );
	projTex->Bind( GR_TEX_PROJECTION, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

	// bind the VSM texture cube.
	shadowData.indirCube->Bind( GR_TEX_SHADOW_VSM_CUBE, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// if texturing isn't enabled, bind default textures.
	if ( !_colorTextureEnable )
	{
		_defaultBump->Bind( GR_TEX_BUMP );
		_white->Bind( GR_TEX_DIFFUSE );
		_black->Bind( GR_TEX_SPECULAR );
		_defaultNormal->Bind( GR_TEX_NORMAL );
	}

	// used to cache various states.
	GrTextureBase* curShadowTex = 0;
	GrShader* prevShader = 0;
	EGRQUALITY masterQuality = quality;
	bool shaderChanged = false;

	// render all objects.
	unsigned int objCount = lightReceivers.GetItemCount();
	unsigned int startObj = 0;
	while ( startObj < objCount )
	{
		// get the current material span.
		unsigned int endObj = 0;
		const GrMaterial* material = GetMaterialSpan( endObj, lightReceivers, startObj );

		// set the blending mode based on the surface type.
		if ( material->GetSortType() == GrMaterial::EST_DECAL )
			GrSetDepthMode( EDM_LESSEQUAL );
		else
			GrSetDepthMode( EDM_EQUAL );

		// get whether or not the material as a whole should receive shadow.
		bool materialReceiveShadow = material->GetReceiveShadow();

		// for each pass, render all objects.
		const unsigned int passCount = material->GetPassCount();
		for ( unsigned int pass = 0; pass < passCount; ++pass )
		{
			// get the current material pass.
			const GrMaterialPass* curPass = material->GetPass( pass );

			// calculate the quality setting for the current pass.
			EGRQUALITY passQuality = curPass->ReceiveShadow() ? masterQuality : EGQ_LOW;

			// check to see if the current pass responds to this color of
			// adjustedLight.
			if ( !adjustedLight.GetColor().IsBetween( curPass->GetMinLightResponse(), curPass->GetMaxLightResponse() ) )
				continue;

			// apply the shader and it's params.
			GrShader* shader = curPass->GetShader();
			B_ASSERT( shader != 0 );
			if ( shader != prevShader )
			{
				curPass->ApplyShader( passQuality );
				shader->SetEngineParam4fv( GrShader::EP_LIGHT_COLOR, kLightColor );
				shader->SetEngineParam4fv( GrShader::EP_LIGHT_ATTEN, kLightFalloff );
				shader->SetEngineParam4fv( GrShader::EP_LIGHT_HAZE, kLightHaze );
				shader->SetEngineParam4fv( GrShader::EP_LIGHT_WIDTH, kShadowWidthParams );
				shader->SetEngineParam4fv( GrShader::EP_SHADOW_NEAR_CLIP, kShadowNearClip );
				prevShader = shader;
				shaderChanged = true;
			}
			curPass->GetUserParams().ApplyUserParams();
			curPass->GetUserParams().ApplyUserSamplers();

			// apply pass states.
			curPass->ApplyStates( ~GR_WIREFRAME, GR_RGBA );
			curPass->ApplyLightBlendStates( ~GR_WIREFRAME );
			curPass->GetTextureSet().ApplyGlobalState( shader );

			// apply relevant pass texture stages.
			if ( _colorTextureEnable )
			{
				curPass->GetTextureSet().ApplyStageState( ES_BUMP, GR_TEX_BUMP, shader, GrShader::EP_BUMP_MOD, GrShader::EP_BUMP_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_NORMAL, GR_TEX_NORMAL );
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE, GR_TEX_DIFFUSE, shader, GrShader::EP_DIFFUSE_MOD, GrShader::EP_DIFFUSE_ADD );
				curPass->GetTextureSet().ApplyStageState( ES_SPECULAR, GR_TEX_SPECULAR, shader, GrShader::EP_SPECULAR_MOD, GrShader::EP_SPECULAR_ADD );
			}

			// check to see if we should receive a shadow.
			if ( curPass->ReceiveShadow() && materialReceiveShadow )
			{
				if ( curShadowTex != shadow )
				{
					shadow->Bind( GR_TEX_SHADOW, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
					curShadowTex = shadow;
				}
			}
			else
			{
				if ( curShadowTex != _noShadow )
				{
					_noShadow->Bind( GR_TEX_SHADOW, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
					curShadowTex = _noShadow;
				}
			}

			// shade all objects
			bool previousPreTransformed = false;
			for ( unsigned int obj = startObj; obj < endObj; ++obj )
			{
				const GrRenderList::SItem& renderable = lightReceivers[ obj ];

				// render the object.
				if ( renderable.preTransformed )
				{
					// apply local space parameters.
					if ( !previousPreTransformed || shaderChanged )
					{
						shader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, camera.GetViewProjMatrix() );
						shader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, kWorldCameraPos );
						shader->SetEngineParam4fv( GrShader::EP_LOCAL_LIGHT_POSITION, kWorldLightPos );
						shader->SetEngineParam4fv( GrShader::EP_LOCAL_LIGHT_DIR_OVERRIDE, kWorldLightDirOverride );
						shader->SetEngineParam4fv( GrShader::EP_LIGHT_RANGE, kLightRangeParams );
						shader->SetEngineParam4x4fv( GrShader::EP_LIGHT_PROJECTION_MATRIX, kProjTexGenMatrix.GetDataStart() );
						shader->SetEngineParam4x4fv( GrShader::EP_SHADOW_PROJECTION_MATRIX, shadowData.shadowMatrix );
						previousPreTransformed = true;
						shaderChanged = false;
					}

					// render the object.
					if ( renderable.meshInst->GetMesh()->GetRange( renderable.range ).count < _maxIndexBatchSize )
						obj = BatchRender( lightReceivers, obj, endObj, LIGHT_VERTEX_COMPONENTS, time );
					else
						renderable.meshInst->RenderRange( renderable.range, LIGHT_VERTEX_COMPONENTS, time );
				}
				else
				{
					previousPreTransformed = false;

					// calculate local space parameters.
					const MMat4x4& world = renderable.meshInst->GetTransform();
					const MMat4x4& invWorld = renderable.meshInst->GetInvTransform();
					const MMat3x3& invWorldRotate = renderable.meshInst->GetInvTransposeRot();
					MVec3 worldPos;
					MVec3 worldScales;
					world.GetTranslate( worldPos );
					world.GetScale( worldScales );
					float worldScale = worldScales.Max();
					float invWorldScale = 1.0f / worldScales.Max();

					MVec3 localEyePos = invWorld.TransformCoord( kWorldCameraPos );
					MVec3 localLightPos = invWorld.TransformCoord( kWorldLightPos );
					MVec3 localLightDir = invWorldRotate.RotatePoint( kWorldLightDir );
					MMat4x4 localProjTexgen = kProjTexGenMatrix * world;
					MMat4x4 localShadowTexgen = shadowData.shadowMatrix * world;

					const float kLocalLightDirOverride[ 4 ] = {
						( 1.0f - kRadialDirMask ) * localLightDir.GetX(),
						( 1.0f - kRadialDirMask ) * localLightDir.GetY(),
						( 1.0f - kRadialDirMask ) * localLightDir.GetZ(),
						kRadialDirMask
					};

					// apply local space parameters.
					float localLightRangeParams[ 4 ] = {
						invWorldScale * kLightRangeParams[ 0 ],
						invWorldScale * kLightRangeParams[ 1 ],
						invWorldScale * kLightRangeParams[ 2 ],
						worldScale * kLightRangeParams[ 3 ],
					};

					shader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, camera.GetViewProjMatrix() * world );
					shader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, localEyePos );
					shader->SetEngineParam4fv( GrShader::EP_LOCAL_LIGHT_POSITION, localLightPos );
					shader->SetEngineParam4fv( GrShader::EP_LOCAL_LIGHT_DIR_OVERRIDE, kLocalLightDirOverride );
					shader->SetEngineParam4fv( GrShader::EP_LIGHT_RANGE, localLightRangeParams );
					shader->SetEngineParam4x4fv( GrShader::EP_LIGHT_PROJECTION_MATRIX, localProjTexgen.GetDataStart() );
					shader->SetEngineParam4x4fv( GrShader::EP_SHADOW_PROJECTION_MATRIX, localShadowTexgen.GetDataStart() );

					// render the object.
					renderable.meshInst->RenderRange( renderable.range, LIGHT_VERTEX_COMPONENTS, time );
				}
			}
		}
		startObj = endObj;
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
const GrMaterial*
GrRenderLight::GetMaterialSpan( unsigned int& spanEnd, const GrRenderList& list, unsigned int startObj )
{
	B_ASSERT( startObj < list.GetItemCount() );
	const GrMaterial* curMaterial = list[ startObj ].meshInst->GetMaterial( list[ startObj ].range );
	spanEnd = startObj+1;
	while ( spanEnd < list.GetItemCount() && curMaterial == list[ spanEnd ].meshInst->GetMaterial( list[ spanEnd ].range ) )
		++spanEnd;
	return curMaterial;
}

//----------------------------------------------------------
unsigned int
GrRenderLight::BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx,
							unsigned int vertexComponents, unsigned int time )
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

//----------------------------------------------------------
bool
GrRenderLight::SortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right )
{
	// get the materials for object.
	const GrMaterial* leftMat = left.meshInst->GetMaterial( left.range );
	const GrMaterial* rightMat = right.meshInst->GetMaterial( right.range );
	B_ASSERT( leftMat != 0 && rightMat != 0 );

	// smaller sort type comes first.
	if ( leftMat->GetSortType() != rightMat->GetSortType() )
		return leftMat->GetSortType() < rightMat->GetSortType();

	// don't care, so sort by material.
	return ( size_t )leftMat < ( size_t )rightMat;	// false;
}
