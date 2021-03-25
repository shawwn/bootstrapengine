//----------------------------------------------------------
// File:		GrScene.h
// Author:		Kevin Bray
// Created:		11-09-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrScene.h"

// project headers.
#include "RRsrcMgr.h"
#include "UExprCallback.h"
#include "UExprMgr.h"
#include "UWriter.h"
#include "UReader.h"
#include "MVec3.h"
#include "MRay.h"
#include "ThSection.h"
#include "GrLight.h"
#include "GrCamera.h"
#include "GrBSPSplit.h"
#include "GLSubsys.h"
#include "GrFramebuffer.h"
#include "GrMeshInst.h"
#include "GrUtil.h"
#include "GrTextureCube.h"
#include "GrSceneRender.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrLightMgr.h"
#include "GrBSPTree.h"
#include "GrRenderer.h"
#include "GrTexGen.h"
#include "GrTextureMgr.h"
#include "GrParse.h"

// flags for scene files.
static const unsigned int kHasBSPTree = 0x01;
static const unsigned int kHasNodeTree = 0x02;
static const unsigned int kHasStaticEffects = 0x04;

// file local functions.
static float GetTimeVarCallback();

GrScene* gGrScene = 0;

#ifdef _DEBUG
const float _clearColor[ 4 ] = { 0.5f, 0.5f, 0.5f, 0.5f };
#else
const float _clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
#endif

//**********************************************************
// utility functions.
//**********************************************************

// forward declarations.
GrModelNode*            FindModelNodeThatContainsMeshInstIter( GrModelNode* root, GrMeshInst* meshInst );

//----------------------------------------------------------
// returns the GrModelNode that contains the incoming meshInst.
GrModelNode* 
FindModelNodeThatContainsMeshInst( GrModel* root, GrMeshInst* meshInst )
{
    // try to find that meshInst within the model's node hierarchy.
    GrModelNode* result = FindModelNodeThatContainsMeshInstIter( root->GetRootNode(), meshInst );
    
    // if there was a result, return it.
    if ( result )
        return result;
    
    // recurse to children.
    for ( unsigned int i = 0; i < root->GetChildCount(); ++i )
    {
        GrModel* child = root->GetChild( i );
        GrModelNode* result = FindModelNodeThatContainsMeshInst( child, meshInst );

        // if there was a result, return it.
        if ( result )
            return result;
    }

    // otherwise, that meshInst doesn't exist in the current model's hierarchy.
    return 0;
}

//----------------------------------------------------------
// returns the GrModelNode that contains the incoming meshInst.
GrModelNode*
FindModelNodeThatContainsMeshInstIter( GrModelNode* root, GrMeshInst* meshInst )
{
    // if the model node contains that meshInst, return it.
    for ( unsigned int i = 0; i < root->GetMeshInstCount(); ++i )
    {
        if ( root->GetMeshInst( i ) == meshInst )
            return root;
    }

    // recurse to children.
    for ( unsigned int i = 0; i < root->GetChildCount(); ++i )
    {
        GrModelNode* child = root->GetChild( i );
        GrModelNode* result = FindModelNodeThatContainsMeshInstIter( child, meshInst );

        // if there was a result, return it.
        if ( result )
            return result;
    }

    // otherwise, that meshInst doesn't exist in the current node's hierarchy.
    return 0;
}

//**********************************************************
// class GrScene
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrScene::GrScene()
: _bspTree( new GrBSPTree() )
, _globalAmbience( 1.0f, 1.0f, 1.0f, 1.0f )
, _rootModel( new GrModel( "__root" ) )
, _skyboxModel( new GrModel( "__skybox" ) )
, _timeDelta( 0 )
, _sceneTime( 0 )
, _frameId( 0 )
, _wireframe( false )
, _lightEnable( true )
, _zUp( false )
, _clearColor( 0.0f, 0.0f, 0.0f, 1.0f )
{
	// make sure the resource manager is up and running.
	B_ASSERT( gRRsrcMgr != 0 );
	gRRsrcMgr->RegisterScriptParser( GR_KW_SCENEDEF, ParseSceneDef );

	// register some useful callbacks.
	gUExprMgr->CreateScriptCallback( "_sceneTime", GetTimeVarCallback );

	// make sure the scene hasn't already been created.
	B_ASSERT( gGrScene == 0 );
	gGrScene = this;
}

//----------------------------------------------------------
GrScene::~GrScene()
{
	Clean();
	gGrScene = 0;

// free the node tree.
//	delete _nodeTree;
//	_nodeTree = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrScene::LoadScene( const UPath& path, tstring& errors )
{
	_sceneDefPath = path + GR_SCENEDEF_FILENAME;
	return gRRsrcMgr->LoadScript( _sceneDefPath, "scene", errors );
}

//----------------------------------------------------------
void
GrScene::Reset( const UPath& sceneDefPath, const tstring& name, GrBSPTree* bspTree, bool zUp )
{
	// ensure that the parameters coming in are valid.
	B_ASSERT( bspTree != 0 );

	// clean the current scene and set the data.
	Clear();
	_sceneDefPath = sceneDefPath;
	_name = name;
	_bspTree = bspTree;
	_zUp = zUp;

	// notify the BSP tree that it's been added to the scene.
	bspTree->SetInScene( true );
}

//----------------------------------------------------------
GrScene::PickFlags
GrScene::Pick( GrModel*& model, GrModelNode*& modelNode, GrMeshInst*& meshInst, GrMesh::STriCollision& hit,
			   const MRay& ray, PickFlags flags )
{
	// pick against the BSP tree.
	GrMesh::STriCollision bspHit;
	GrMesh::STriCollision modelHit;
	GrMesh::STriCollision terrainHit;
	GrMeshInst* bspMeshInst = 0;
	GrMeshInst* modelMeshInst = 0;
	GrMeshInst* terrainMeshInst = 0;

	// push the ray out to a value too large.
	MVec3 virtualInfinity = ray.GetStart() + ( 2.0f * GR_WORLD_GEOMETRY_SCALE * ray.GetDir() );
	bspHit.hitLoc = virtualInfinity;
	modelHit.hitLoc = virtualInfinity;
	terrainHit.hitLoc = virtualInfinity;

	bool bspRet = false;
	bool modelRet = false;
	bool skyboxRet = false;
	bool terrainRet = false;

	// perform the pick operations.
	if ( ( flags & kPickBSP ) != 0 )
		bspRet = _bspTree->Pick( bspMeshInst, bspHit, ray );
	if ( ( flags & kPickModels ) != 0 )
	{
		GrMesh::STriCollision skyboxHit;
		GrModel* modelB = 0;
		GrMeshInst* meshInstB = 0;

		// TODO: clean this up.
		modelRet = _rootModel->Pick( model, meshInst, modelHit, ray );
		skyboxRet = _skyboxModel->Pick( modelB, meshInstB, skyboxHit, ray );
		if ( modelRet && skyboxRet )
		{
			modelRet = true;
			float modelDist = ray.GetStart().Dist( modelHit.hitLoc );
			float skyboxDist = ray.GetStart().Dist( skyboxHit.hitLoc );
			if ( skyboxDist < modelDist )
			{
				modelHit = skyboxHit;
				model = modelB;
				modelMeshInst = meshInstB;
				modelRet = true;
			}
		}
		else if ( skyboxRet )
		{
			modelHit = skyboxHit;
			model = modelB;
			modelMeshInst = meshInstB;
			modelRet = true;
		}
        else if ( modelRet )
        {
			modelMeshInst = meshInst;
        }
	}

	// check for an early out.
	if ( !bspRet && !modelRet )
		return kPickNone;

	// check distances.
	float bspDistSqr = bspRet ? bspHit.hitLoc.DistSqr( ray.GetStart() ) : FLT_MAX;
	float modelDistSqr = modelRet ? modelHit.hitLoc.DistSqr( ray.GetStart() ) : FLT_MAX;

	// get the closest hit.
	PickFlags hitType = kPickNone;
	if ( bspDistSqr < modelDistSqr )
	{
		hitType = kPickBSP;
		hit = bspHit;
		meshInst = bspMeshInst;
		model = 0;
	}
	else
	{
		hitType = kPickModels;
		hit = modelHit;
		meshInst = modelMeshInst;
	}

    // if we hit a model's meshInst and we're picking model nodes, 
    // determine which modelNode that meshInst belongs to.
    if ( hitType == kPickModels )
    {
        if ( flags & kPickModelNodes )
        {
            // validate the parameters.
            B_ASSERT( model );
            B_ASSERT( meshInst );

            // find the model node that contains that meshInst.
            modelNode = FindModelNodeThatContainsMeshInst( model, meshInst );

            // if the model node was found, update the hitType.
            if ( modelNode != 0 )
                hitType = kPickModelNodes;
        }
    }

	// return flags that indicate what we hit.
	return hitType;
}

//----------------------------------------------------------
void
GrScene::Update( unsigned int gameTimeDelta, unsigned int sysTimeDelta )
{
	B_ASSERT( _rootModel != 0 );

	// update the renderer's time.
	gGrRenderer->Update( gameTimeDelta, sysTimeDelta );

	// update the current time.
	_timeDelta = sysTimeDelta;
	_sceneTime += sysTimeDelta;

	// update the dynamic node tree.
	_rootModel->Update();

	// update lights.
	unsigned int lightCount = _lights.GetLightCount();
	for ( unsigned int i = 0; i < lightCount; ++i )
		_lights.GetLight( i )->Update();
}

//----------------------------------------------------------
void
GrScene::AddLight( URef< GrLight > light )
{
	B_ASSERT( light != 0 );

	// add the light.  Replace any existing light with the same name.
	_lights.AddLight( light );
}

//----------------------------------------------------------
URef< GrLight >
GrScene::FindLight( const char* id, int cmpLen ) const
{
	return _lights.FindLight( id, cmpLen );
}

//----------------------------------------------------------
bool
GrScene::RemoveLight( const char* id )
{
	return _lights.RemoveLight( id );
}

//----------------------------------------------------------
bool
GrScene::RemoveLight( GrLight* light )
{
	return _lights.RemoveLight( light );
}

//----------------------------------------------------------
unsigned int
GrScene::GetLightCount() const
{
	return _lights.GetLightCount();
}

//----------------------------------------------------------
URef< GrLight >
GrScene::GetLight( unsigned int idx ) const
{
	return _lights.GetLight( idx );
}

//----------------------------------------------------------
void
GrScene::DbgDrawHierarchy()
{
	_rootModel->DbgDrawHierarchy();
}

//----------------------------------------------------------
void
GrScene::Cache()
{
	B_ASSERT( _bspTree );
	_bspTree->Cache();
	_rootModel->Cache();
}

//----------------------------------------------------------
void
GrScene::Evict()
{
	B_ASSERT( _bspTree );
	_bspTree->Evict();
	_rootModel->Evict();
}

//----------------------------------------------------------
void
GrScene::Render( const GrCamera *camera )
{
	B_ASSERT( gGrFramebuffer );
	B_ASSERT( _bspTree );

	// clear the previous frame's statistics.
	ClearGLStats();

	// generate any procedural textures.
	gGrTexGen->GenTextures( camera );

	// start rendering frame.
	gGrFramebuffer->BeginFrame();

	// render the scene to the texture ( the framebuffer is a texture ).
	RenderTexture( camera, camera->GetPos() );

	// end rendering frame.
	gGrFramebuffer->EndFrame( _sceneTime );

	// draw out the hierarchy as a debug function.
//	DbgDrawHierarchy();
}

//----------------------------------------------------------
void
GrScene::RenderTexture( const GrCamera* camera, const MVec3& pvsEyePos )
{
	// clear the framebuffer.
	ClearFramebuffer();

	// increment the frame ID.
	++_frameId;

	// render lights.
	RenderLights( camera, pvsEyePos );
}

//----------------------------------------------------------
void
GrScene::Clear()
{
	Clean();
}

//----------------------------------------------------------
void
GrScene::Traverse( GrSceneTraverser* traverser, PickFlags elems )
{
	// iterate through elements on the BSP tree.
	if ( elems & kPickBSP )
		_bspTree->Traverse( traverser );
	if ( elems & kPickModels )
		_rootModel->Traverse( traverser );
}

//----------------------------------------------------------
void                    
GrScene::FindMeshInstsInFrustum( GrRenderList& meshInsts, const GrFrustum& frustum )
{
    // TODO: this is probably not the correct way to do this.
	if ( _bspTree )
		_bspTree->GetObjectsInFrustum( meshInsts, frustum );
	if ( _rootModel )
    {
        // update any dirty transforms.
        _rootModel->Update();
		_rootModel->GetObjectsInFrustum( meshInsts, frustum );
    }
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrScene::ClearFramebuffer()
{
//	GLenum fbStatus = bglCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
//	B_ASSERT( fbStatus == GL_FRAMEBUFFER_COMPLETE_EXT );

	CHECK_GL();

	// disable any state that could interfere with our clear.
	GrDisable( GR_SCISSORTEST );
	GrSetWriteEnable( GR_RGBA | GR_DEPTH | GR_STENCIL );
	GrClear( _clearColor.GetR(), _clearColor.GetG(), _clearColor.GetB(), _clearColor.GetA() );
}

//----------------------------------------------------------
void
GrScene::RenderLights( const GrCamera* camera, const MVec3& pvsEyePos )
{
	// move the skybox model so that it's based on the camera's position.
	_skyboxModel->SetLocal( MMat4x4( camera->GetPos() ) );
	_skyboxModel->Update();

	// begin accumulating lights.
	gGrRenderer->BeginRender();

	// render the ambient light.
	gGrSceneRender->RenderAmbience( camera, pvsEyePos, _bspTree, _rootModel, _skyboxModel,
		_globalAmbience, _frameId );

	// these should probably be grouped so we can do batch rendering of lights
	// that don't cast shadows.
	if ( _lightEnable )
	{
		unsigned int lightCount = _lights.GetLightCount();
		for ( unsigned int i = 0; i < lightCount; ++i )
		{
			// get the current light.
			GrLight* curLight = _lights.GetLight( i );

			// skip if it isn't influencing very much.
			if ( curLight->GetCachedRange() < 0.1f || curLight->GetCachedIntensity() < 0.001f )
				continue;

			// render the light.
			gGrSceneRender->RenderLight( camera, curLight, _bspTree, _rootModel, _frameId );
		}
	}

	gGrRenderer->EndRender( *camera );
/*
	// begin accumulating lights.
	gGrRenderer->BeginAccum( camera );

	// render the ambient light.
	gGrSceneRender->RenderAmbience( camera, _bspTree, _rootModel, _globalAmbience );

	// these should probably be grouped so we can do batch rendering of lights
	// that don't cast shadows.
	unsigned int lightCount = _lights.GetLightCount();
	for ( unsigned int i = 0; i < lightCount; ++i )
	{
		// get the current light.
		GrLight* curLight = _lights.GetLight( i );

		// skip if it isn't influencing very much.
		if ( curLight->GetCachedRange() < 0.1f )
			continue;

		// render the light.
		gGrSceneRender->RenderLight( camera, curLight, _bspTree, _rootModel );
	}

	gGrRenderer->EndAccum();
*/
}

//----------------------------------------------------------
void
GrScene::Clean()
{
	// cleanup the scene.
	delete _bspTree;
	_bspTree = 0;

	// clear lights.
	_lights.Clear();

	// clear the root model's children.
	_rootModel->RemoveChildren();

	// clear the skybox-model's children.
	_skyboxModel->RemoveChildren();
}


//**********************************************************
// file local functions.
//**********************************************************

//----------------------------------------------------------
float GetTimeVarCallback()
{
	float time = float( gGrScene->GetSceneTime() % 1000000 );
	return time / 1000.0f;
}
