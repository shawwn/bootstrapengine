//---------------------------------------------------------------------------------------------
// File:        Scene.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//---------------------------------------------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Scene.h"

// editlib headers.
#include "editlib_afx.h"

// project headers.
#include "Light.h"
#include "Model.h"
#include "ModelNode.h"
#include "MeshInst.h"
#include "Utils.h"
#include "Camera.h"
#include "Color.h"
#include "Ray.h"

// engine headers.
#include "RRsrcMgr.h"
#include "EdPick.h"
#include "EdImportDAE.h"
#include "EdSave.h"
#include "GrScene.h"
#include "GrRenderList.h"
#include "GrBSPTree.h"
#include "GrModel.h"
#include "GrLight.h"

// std C++ headers.
#include <set>

// .NET namespaces.
using namespace System;
using namespace System::Collections::Generic;

//**********************************************************
// ref class Scene
//**********************************************************

//----------------------------------------------------------
Bootstrap::Scene::Scene()
{
}

//----------------------------------------------------------
Bootstrap::Scene::~Scene()
{
    this->!Scene();
}

//----------------------------------------------------------
Bootstrap::Scene::!Scene()
{
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::Scene::ClearColor::get()
{
    return Bootstrap::Color( gGrScene->GetClearColor() );
}

//----------------------------------------------------------
void 
Bootstrap::Scene::ClearColor::set( Color value )
{
    gGrScene->SetClearColor( value.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Scene::WorldUp::get()
{
    return gGrScene->GetZUp() ? Vector3( 0, 0, 1 ) : Vector3( 0, 1, 0 );
}

//----------------------------------------------------------
bool 
Bootstrap::Scene::ZUp::get()
{
    return gGrScene->GetZUp();
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Scene::Name::get()
{
    return TStrToStr( gGrScene->GetName() );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Scene::ShortName::get()
{
    tstring name = gGrScene->GetName();

    // strip all characters before the last slash.
    size_t lastSlashPos = name.find_last_of( '/' );
    if ( lastSlashPos != tstring::npos )
    {
        name = name.substr( lastSlashPos+1 );
    }

    return TStrToStr( name );
}

//----------------------------------------------------------
Bootstrap::Model^
Bootstrap::Scene::SceneModel::get()
{
    GrModel* model = gGrScene->GetRootModel()->FindModelByInstName( GR_SCENE_MODEL_INST_NAME );

    if ( model )
        return gcnew Model( model );

    return nullptr;
}

//----------------------------------------------------------
int
Bootstrap::Scene::LightCount::get()
{
    return gGrScene->GetLightCount();
}

//----------------------------------------------------------
void
Bootstrap::Scene::AddLight( Light^ light )
{
    B_ASSERT( light != nullptr );
    gGrScene->AddLight( light->_light );
}

//----------------------------------------------------------
Bootstrap::Light^
Bootstrap::Scene::FindLight( System::String^ id )
{
    URef< GrLight > light = gGrScene->FindLight( StrToTStr( id ).c_str() );

    if ( light )
        return gcnew Light( light );

    return nullptr;
}

//----------------------------------------------------------
bool
Bootstrap::Scene::RemoveLight( System::String^ id )
{
    return gGrScene->RemoveLight( StrToTStr( id ).c_str() );
}

//----------------------------------------------------------
bool
Bootstrap::Scene::RemoveLight( Light^ light )
{
    B_ASSERT( light != nullptr );
    return gGrScene->RemoveLight( light->_light );
}

//----------------------------------------------------------
Bootstrap::Light^
Bootstrap::Scene::GetLight( int idx )
{
    URef< GrLight > light = gGrScene->GetLight( idx );

    if ( light )
        return gcnew Light( light );

    return nullptr;
}

//----------------------------------------------------------
Bootstrap::Scene::PickResult 
Bootstrap::Scene::Pick( Ray ray, float distance, Entities whichEntities )
{
    Scene::PickResult result;
    EdPick pick( 1.0f ); // TODO: re-examine this parameter.
    pick.Pick( ray.ToMRay(), (EdPick::Flag)whichEntities, distance );

    // set the resulting type of the pick operation.
    result.PickedType = (Entities)pick.GetHitType();

    switch( pick.GetHitType() )
    {
        case EdPick::kPickModels:   
            result.PickedEntity = gcnew Model( pick.GetHitModel() ); 
            break;

        case EdPick::kPickModelNodes:
            result.PickedEntity = gcnew ModelNode( pick.GetHitModelNode() );
            break;

        case EdPick::kPickBSP:      
            result.PickedEntity = nullptr;
            /* TODO */
            break;

        case EdPick::kPickLights:   
            result.PickedEntity = gcnew Light( pick.GetHitLight() ); 
            break;
    }

    const EdPick::STriHit& hit = pick.GetTriHitInfo();

    // get the GrMeshInst the ray intersected with.
    GrMeshInst* meshInst = pick.GetHitMeshInst();
    if( meshInst != 0 )
    {
        result.PickedMeshInst = MeshInst::Create( meshInst );
        result.PickedMeshInst->Mesh->GetTriInfo( result.TriangleInfo, hit.triIdx );
    }
    result.HitLocation = hit.hitLoc;
    result.HitNormal = hit.hitNormal;
    result.TriangleIndex = hit.triIdx;
    result.U = hit.u;
    result.V = hit.v;
    result.W = hit.w;

    return result;
}

//----------------------------------------------------------
IList< Bootstrap::MeshInst^ >^ 
Bootstrap::Scene::GetVisibleMeshInsts( Camera^ camera )
{
    GrRenderList renderList;

    // gather MeshInsts from the BSP tree, the model root, and the terrain.
    if( GrBSPTree* bspTree = gGrScene->GetBSPTree() )
        bspTree->GetLitObjects( renderList, 0, camera->_camera->GetFrustum(), gGrScene->GetFrameId() );
    if( GrModel* model = gGrScene->GetRootModel() )
        model->GetLitObjects( renderList, 0, camera->_camera->GetFrustum(), gGrScene->GetFrameId() );

    // filter out extra MeshInsts.
    std::set< GrMeshInst* > meshInsts;
    for( unsigned int i = 0; i < renderList.GetItemCount(); ++i )
        meshInsts.insert( renderList.GetItem( i ).meshInst );

    // build an array of MeshInsts.
    array< MeshInst^ >^ result = gcnew array< MeshInst^ >( meshInsts.size() );
    unsigned int i = 0;
    for( std::set< GrMeshInst* >::iterator it = meshInsts.begin(); it != meshInsts.end(); ++it )
    {
        GrMeshInst* meshInst = *it;
        result[ i++ ] = MeshInst::Create( meshInst );
    }

    // return the list of mesh instances.
    return (IList< MeshInst^ >^)result;
}

//----------------------------------------------------------
IList< Bootstrap::MeshInst^ >^ 
Bootstrap::Scene::GetBSPMeshInsts()
{
    // get the mesh instances.
    const GrBSPTree::MeshInstVec& meshInsts = gGrScene->GetBSPTree()->GetMeshInsts();

    // build an array of MeshInsts.
    array< MeshInst^ >^ result = gcnew array< MeshInst^ >( meshInsts.size() );
    unsigned int i = 0;
    GrBSPTree::MeshInstVec::const_iterator iter = meshInsts.begin();
    GrBSPTree::MeshInstVec::const_iterator end = meshInsts.end();
    for( ; iter != end; ++iter )
        result[ i++ ] = MeshInst::Create( *iter );

    // return the list of mesh instances.
    return (IList< MeshInst^ >^)result;
}

//----------------------------------------------------------
bool 
Bootstrap::Scene::ImportArtModel( String^ name, String^ targetPackageName, String^ pathToArtFile, bool findNormalMaps, bool importAnimations )
{
    EdImportDAE importer; 
    if( !importer.ImportModel( StrToTStr( name ).c_str(), StrToTStr( targetPackageName ).c_str(), StrToTStr( pathToArtFile ).c_str(), findNormalMaps, importAnimations ) )
    {
        // TODO: Improve this error message.
        EdPrintF( "Failed to load model %s into package %s.", StrToTStr( pathToArtFile + "/" + name ).c_str(), StrToTStr( targetPackageName ).c_str() );
        return false;
    }
    return true;
}

//----------------------------------------------------------
bool 
Bootstrap::Scene::Load( String^ name )
{
    // attempt to load the scene.
    tstring errors;
    if ( !gRRsrcMgr->LoadScript( StrToTStr( name ), GR_KW_SCENEDEF, errors ) )
    {
        HandleError( "%s\n", errors.c_str() );
        return false;
    }

    // note that the scene loaded successfully.
    _loaded = true;

    // get the root model.
    _root = gcnew Model( gGrScene->GetRootModel() );

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
void 
Bootstrap::Scene::Save()
{
    EdSave save;
    save.SaveSceneData();
    save.SaveLightData();
    save.SaveMaterialData();
}

//----------------------------------------------------------
void 
Bootstrap::Scene::Clear()
{
    _root = nullptr;
}

//----------------------------------------------------------
IList< Bootstrap::Light^ >^ 
Bootstrap::Scene::GetLights()
{
    array< Light^ >^ result = gcnew array< Light^ >( gGrScene->GetLightCount() );
    for( size_t i = 0; i < gGrScene->GetLightCount(); ++i )
    {
        result[i] = gcnew Light( gGrScene->GetLight( i ) );
    }
    return (IList< Light^ >^)result;
}