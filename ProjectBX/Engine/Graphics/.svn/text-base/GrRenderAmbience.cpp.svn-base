//----------------------------------------------------------
// File:		GrRenderAmbience.cpp
// Author:		Kevin Bray
// Created:		04-07-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderAmbience.h"

// project includes.
#include "GrRenderList.h"
#include "GrCamera.h"
#include "GrColor.h"
#include "GrStreamingIB.h"
#include "GLSubsys.h"
#include "GrMeshInst.h"
#include "GrShader.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrShaderMgr.h"
#include "GrUberPatchSet.h"
#include "GrUberDetailCache.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

#define AMBIENT_VERTEX_COMPONENTS		\
		( GR_ATTRIB_POSITION_MASK	|	\
		  GR_ATTRIB_NORMAL_MASK		|	\
		  GR_ATTRIB_BINORMAL_MASK	|	\
		  GR_ATTRIB_TANGENT_MASK	|	\
		  GR_ATTRIB_TEXCOORD_MASK )

//**********************************************************
// class GrRenderAmbience
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderAmbience::GrRenderAmbience( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize )
: _streamingIB( streamingIB )
, _maxIndexBatchSize( maxIndexBatchSize )
, _wireframe( false )
, _colorTextureEnable( true )
, _batchEnable( true )
{
	// get the normalization texture cube.
	_normalizeCube = gGrTextureMgr->FindTexture( GR_SYSTEXCUBE_NORMALIZE );

	// get the default bump map.
	_defaultBump = gGrTextureMgr->FindTexture( GR_SYSTEX_GRAY );

	// get the stipple masks.
	_stipple[ 0 ] = gGrTextureMgr->FindTexture( GR_SYSTEX_STIPPLE0 );
	_stipple[ 1 ] = gGrTextureMgr->FindTexture( GR_SYSTEX_STIPPLE1 );
	_stipple[ 2 ] = gGrTextureMgr->FindTexture( GR_SYSTEX_STIPPLE2 );
	_stipple[ 3 ] = gGrTextureMgr->FindTexture( GR_SYSTEX_STIPPLE3 );

	// get the default white texture.
	_white = gGrTextureMgr->FindTexture( GR_SYSTEX_WHITE );

	// get the default black texture.
	_black = gGrTextureMgr->FindTexture( GR_SYSTEX_BLACK );
}

//----------------------------------------------------------
GrRenderAmbience::~GrRenderAmbience()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderAmbience::RenderAmbience( GrRenderList& postRenderObjects, const GrRenderList& objects, const GrCamera& camera,
								  const GrColor& ambientColor, unsigned int time, EGRQUALITY quality )
{
	// is there anything to do?
	if ( !objects.GetItemCount() )
		return;

	// notify the ubertexture cache of tiles.
	unsigned int uberPatchSetCount = objects.GetUberPatchSetCount();
	for ( unsigned int i = 0; i < uberPatchSetCount; ++i )
		objects.GetUberPatchSet( i )->CacheTiles( &camera, 512, 512 );
	gGrUberDetailCache->PrepareDetailCache();

	// configure render states for rendering.
	GrSetState( GR_DEPTHTEST | ( _wireframe ? GR_WIREFRAME : 0 ) );
	GrSetWriteEnable( GR_DEPTH | GR_RGBA );
	GrSetDepthMode( EDM_LESSEQUAL );

	// bind our system shaders and various system textures
	_normalizeCube->Bind( GR_TEX_CUBE_NORMALIZE, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// get the view projection matrix.
	const MMat4x4& viewProjMatrix = camera.GetViewProjMatrix();

	// calculate useful information.
	const MVec3& worldCameraPos = camera.GetPos();

	// sort objects by state.
	objects.Sort( SortCmp );

	// if texturing isn't enabled, bind default textures.
	if ( !_colorTextureEnable )
	{
		_defaultBump->Bind( GR_TEX_BUMP );
		_white->Bind( GR_TEX_DIFFUSE );
		_black->Bind( GR_TEX_EMISSIVE );
	}

	// render all of the objects.
	GrShader* prevShader = 0;
	unsigned int stippleMask = 0;
	const unsigned int kStateMask = ~GR_WIREFRAME;
	unsigned int startObj = 0;
	unsigned int objCount = objects.GetItemCount();
	while ( startObj < objCount )
	{
		// get the current material span.
		unsigned int endObj = 0;
		const GrMaterial* material = GetMaterialSpan( endObj, objects, startObj );

		// add all objects using this material to the post render list if necessary.
		if ( material->GetPostProcessPassCount() > 0 )
		{
			for ( unsigned int obj = startObj; obj < endObj; ++obj )
				postRenderObjects.Add( objects[ obj ] );
		}

		// do we need to bind the stipple mask texture?
		if ( material->GetStipple() )
		{
			// bind the stipple texture.
			_stipple[ stippleMask ]->Bind( GR_TEX_STIPPLE, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT );

			// get the next stipple texture.
			++stippleMask;
			stippleMask %= 4;
		}
		else
		{
			_white->Bind( GR_TEX_STIPPLE, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT );
		}

		// for each pass in the current material, render all objects in our span.
		const unsigned int passCount = material->GetPassCount();
		for ( unsigned int pass = 0; pass < passCount; ++pass )
		{
			// get the current pass for rendering and apply texture stages.
			const GrMaterialPass* curPass = material->GetPass( pass );

			// get the ambient shader.
			GrShader* curShader = curPass->GetAmbientShader();
			B_ASSERT( curShader );

			// bind the current shader.
			if ( curShader != prevShader )
			{
				curShader->Bind( quality );
				curShader->SetEngineParam4fv( GrShader::EP_AMBIENT_COLOR, ambientColor );
				prevShader = curShader;
			}

			// apply texture stages and vertex params.
			curPass->ApplyStates( kStateMask, GR_DEPTH | GR_RGBA );
			curPass->ApplyAmbientBlendStates( kStateMask );

			curPass->GetTextureSet().ApplyGlobalState( curShader );
			if ( _colorTextureEnable )
			{
				curPass->GetTextureSet().ApplyStageState( ES_BUMP,		GR_TEX_BUMP,		curShader,	GrShader::EP_BUMP_MOD,		GrShader::EP_BUMP_ADD		);
				curPass->GetTextureSet().ApplyStageState( ES_DIFFUSE,	GR_TEX_DIFFUSE,		curShader,	GrShader::EP_DIFFUSE_MOD,		GrShader::EP_DIFFUSE_ADD	);
				curPass->GetTextureSet().ApplyStageState( ES_EMISSIVE,	GR_TEX_EMISSIVE,	curShader,	GrShader::EP_EMISSIVE_MOD,	GrShader::EP_EMISSIVE_ADD	);
				curPass->GetTextureSet().ApplyStageState( ES_AMBIENT,	GR_TEX_AMBIENT,		curShader,	GrShader::EP_AMBIENT_MOD,		GrShader::EP_AMBIENT_ADD	);
			}
			curPass->GetTextureSet().ApplyStageState( ES_AMBIENT,	GR_TEX_AMBIENT,		curShader,	GrShader::EP_AMBIENT_MOD,		GrShader::EP_AMBIENT_ADD	);
			curPass->ApplyVertexParams( curShader );

			// shade each object.
			for ( unsigned int obj = startObj; obj < endObj; ++obj )
			{
				// get the current renderable.
				const GrRenderList::SItem& renderable = objects[ obj ];

				// is the mesh pretransformed?
				if ( renderable.preTransformed && _batchEnable )
				{
					curShader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, viewProjMatrix );
					curShader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, worldCameraPos );
					if ( renderable.meshInst->GetMesh()->GetRange( renderable.range ).count < _maxIndexBatchSize )
						obj = BatchRender( objects, obj, endObj, AMBIENT_VERTEX_COMPONENTS, time );
					else
						renderable.meshInst->RenderRange( renderable.range, AMBIENT_VERTEX_COMPONENTS, time );
				}
				else
				{
					// TODO: optimize this.
					// calculate local space parameters.
					const MMat4x4& world = renderable.meshInst->GetTransform();
					const MMat4x4& invWorld = renderable.meshInst->GetInvTransform();
					MVec3 localEyePos = invWorld.TransformCoord( worldCameraPos );

					// apply local space parameters.
					curShader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, viewProjMatrix * world );
					curShader->SetEngineParam4fv( GrShader::EP_LOCAL_VIEW_POSITION, localEyePos );

					// render the object.
					renderable.meshInst->RenderRange( renderable.range, AMBIENT_VERTEX_COMPONENTS, time );
				}
			}
		}
		startObj = endObj;
	}

	CHECK_GL();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrRenderAmbience::BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx,
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
const GrMaterial*
GrRenderAmbience::GetMaterialSpan( unsigned int& spanEnd, const GrRenderList& list, unsigned int startObj )
{
	B_ASSERT( startObj < list.GetItemCount() );
	const GrMaterial* curMaterial = list[ startObj ].meshInst->GetMaterial( list[ startObj ].range );
	spanEnd = startObj+1;
	while ( spanEnd < list.GetItemCount() && curMaterial == list[ spanEnd ].meshInst->GetMaterial( list[ spanEnd ].range ) )
		++spanEnd;
	return curMaterial;
}

//----------------------------------------------------------
bool
GrRenderAmbience::SortCmp( const GrRenderList::SItem& left, const GrRenderList::SItem& right )
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
