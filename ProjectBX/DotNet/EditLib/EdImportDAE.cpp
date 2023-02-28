//----------------------------------------------------------
// File:        EdImportDAE.cpp
// Author:      Kevin Bray
// Created:     11-23-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdImportDAE.h"

// project includes.
#include "EdImportDAEMaterials.h"
#include "EdImportDAEGeometries.h"
#include "EdImportDAEScene.h"
#include "GrPolygonUberUVMapper.h"
#include "EdSettings.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrConfig.h"
#include "GrPolygonNode.h"
#include "UWriter.h"
#include "GrPolygonBSPTree.h"
#include "GrBSPTree.h"
#include "GrLightMgr.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrLight.h"
#include "GrKFAnim.h"

// FCollada includes.
#pragma warning ( disable : 4996 )
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDAsset.h"
#pragma warning ( default : 4996 )

#define HULL_NODE_NAME  "hull"
#define PROP_NODE_NAME  "props"

#define BSP_PATH        "BSPs"
#define BSP_NAME        "scene.bsp"
#define MODEL_PATH      "Models"
#define MODEL_NAME      "scene.mdl"
#define ANIM_PATH       "Anims"
#define ANIM_NAME       "scene.kfa"
#define ANIM_NAME_NOEXT "scene"

//**********************************************************
// class EdImportDAE
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportDAE::EdImportDAE()
: _doc( 0 )
{
    // startup collada.
    FCollada::Initialize();
}

//----------------------------------------------------------
EdImportDAE::~EdImportDAE()
{
    if ( _doc )
        _doc->Release();

    // shutdown collada.
    FCollada::Release();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdImportDAE::ImportScene( const char* name, const char* path, bool findNormalMaps, bool uberTexture,
                          bool overwriteMaterials, bool overwriteLights )
{
    if ( !LoadDAEDoc( path ) )
        return false;

    // calculate the output path from the name given.
    // remove any non-alpha numeric values.
    tstring sceneName = name;
    size_t len = sceneName.length();
    for ( size_t i = 0; i < len; ++i )
    {
        if ( !isalnum( sceneName[ i ] ) && sceneName[ i ] != '/' && sceneName[ i ] != '\\' )
        {
            B_ASSERT( sceneName[ i ] != '\n' && sceneName[ i ] != '\r' );
            sceneName[ i ] = '_';
        }
    }

    // create the target path.
    UPath targetPath = gEdSettings->GetWorkingFolder();
    UPath relativePath = "data/" + sceneName;

    // make a folder for the current scene if it doesn't already exist.
    targetPath += relativePath;

    // make sure the data folder exists.
    gRFileMgr->CreateDir( relativePath );
    gRFileMgr->CreateDir( relativePath + BSP_PATH );
    gRFileMgr->CreateDir( relativePath + MODEL_PATH );
    gRFileMgr->CreateDir( relativePath + ANIM_PATH );
    gRFileMgr->CreateDir( relativePath + "Textures" );
    gRFileMgr->CreateDir( relativePath + "Uber" );

//  EnsureFolders( targetPath );
//  EnsureFolders( targetPath + BSP_PATH );
//  EnsureFolders( targetPath + MODEL_PATH );
//  EnsureFolders( targetPath + ANIM_PATH );
//  EnsureFolders( targetPath + "Textures" );

//  GetLastError();

    // make sure the document was set properly.
    B_ASSERT( _doc != 0 );

    // import materials.
    EdImportDAEMaterials materialImporter;
    materialImporter.ImportMaterials( _doc, relativePath, findNormalMaps, overwriteMaterials );

    // import geometries.
    EdImportDAEGeometries geometryImporter;
    geometryImporter.ImportGeometries( _doc );

    // import node hierarchy.
    EdImportDAEScene sceneImporter;
    GrKFAnim* anim = 0;
    GrPolygonNode* nodeTree = sceneImporter.ImportScene( &anim, _doc, relativePath, geometryImporter, materialImporter,
        ( relativePath + ANIM_PATH"/"ANIM_NAME_NOEXT ).GetPathString(), overwriteLights );
    nodeTree->Update();

    // create the BSP tree and save.
    {
        // get various elements in the node hierarchy.
        GrPolygonNode* hullNode = nodeTree->FindNode( HULL_NODE_NAME, true );
        GrPolygonGroup hullPolygons;
        if ( hullNode )
        {
            hullNode->Optimize( hullPolygons, "" );
            nodeTree->DeleteChildTree( "hull", true );
//          hullPolygons.OptimizePolygons( 0.001f, 0.00001f );
        }

        // get the prop polygons and remove the hull geometry from the node tree.
        GrPolygonNode* propNode = nodeTree->FindNode( PROP_NODE_NAME, true );
        GrPolygonGroup propPolygons;
        if ( propNode )
        {
            propNode->Optimize( propPolygons, "" );
            nodeTree->DeleteChildTree( "props", true );
//          propPolygons.OptimizePolygons( 0.001f, 0.00001f );
        }

        // output scene statistics...
        EdPrintF(
            "DAE import process complete.\n"
            " Hull polygons: %i\n"
            " Prop polygons: %i\n"
            " Total: %i\n",
            hullPolygons.GetPolygonCount(),
            propPolygons.GetPolygonCount(),
            hullPolygons.GetPolygonCount() + propPolygons.GetPolygonCount()
            );

        const unsigned int kHullPolygon = 0x01000000;
        const unsigned int kPropPolygon = 0x02000000;

        // tag hull and prop polygons.
        for ( unsigned int i = 0; i < hullPolygons.GetPolygonCount(); ++i )
            hullPolygons.SetPolygonUserData( i, hullPolygons.GetPolygon( i ).GetUserData() | kHullPolygon );
        for ( unsigned int i = 0; i < propPolygons.GetPolygonCount(); ++i )
            propPolygons.SetPolygonUserData( i, propPolygons.GetPolygon( i ).GetUserData() | kPropPolygon );

        // There is no need for ambient occlusion UVs anymore, and at some
        // point, this should be removed entirely.
#if 0
        // generate AO UVs for the world.
        GrPolygonGroup mappedPolygons;
        GrPolygonUVMapper aoUVMapper( 1024, 0.15f );    // 1024x1024 map size, 0.1 unit texel size.
        aoUVMapper.AddPolygonGroup( hullPolygons );
        aoUVMapper.AddPolygonGroup( propPolygons );
        unsigned int textureCount = aoUVMapper.GenMappings( mappedPolygons, relativePath.GetPathString().c_str(), 0, 0xFFFFFF );
        aoUVMapper.Clear();

        // separate out the mapped polygons into the hull and prop groups.
        hullPolygons.Clear();
        propPolygons.Clear();
        for ( unsigned int i = 0; i < mappedPolygons.GetPolygonCount(); ++i )
        {
            const GrPolygon& curPoly = mappedPolygons.GetPolygon( i );
            if ( ( curPoly.GetUserData() & 0xFF000000 ) == kHullPolygon )
                hullPolygons.AddPolygon( curPoly );
            else
                propPolygons.AddPolygon( curPoly );

            B_ASSERT( ( curPoly.GetUserData() & 0xFF000000 ) == kHullPolygon ||
                      ( curPoly.GetUserData() & 0xFF000000 ) == kPropPolygon );
        }
#endif
        // add a plane from the bounding volume to help guide the BSP maker.
        MAABox sceneBounds = hullPolygons.GetAABox();
        sceneBounds.AddBoxToVolume( propPolygons.GetAABox() );
        MVec3 halfExts = sceneBounds.GetHalfExts();
        const MVec3& center = sceneBounds.GetCenter();

        // make sure the box is at least 128 x 128 x 128.
        halfExts.AssignMax( MVec3( 128.0f, 128.0f, 128.0f ) );

        // generate the initial split.
        SPolygonVertex verts[ 4 ];
        verts[ 0 ].position.x = center.GetX() + halfExts.GetX();
        verts[ 0 ].position.y = center.GetY() - halfExts.GetY();
        verts[ 0 ].position.z = center.GetZ() - halfExts.GetZ();

        verts[ 1 ].position.x = center.GetX() - halfExts.GetX();
        verts[ 1 ].position.y = center.GetY() - halfExts.GetY();
        verts[ 1 ].position.z = center.GetZ() - halfExts.GetZ();

        verts[ 2 ].position.x = center.GetX() - halfExts.GetX();
        verts[ 2 ].position.y = center.GetY() - halfExts.GetY();
        verts[ 2 ].position.z = center.GetZ() + halfExts.GetZ();

        verts[ 3 ].position.x = center.GetX() + halfExts.GetX();
        verts[ 3 ].position.y = center.GetY() - halfExts.GetY();
        verts[ 3 ].position.z = center.GetZ() + halfExts.GetZ();

        GrPolygon baseSplit( 0, verts, 4, 0 );
        hullPolygons.AddPolygon( baseSplit );

        // set the static and dynamic data to the scene.
        GrPolygonUberUVMapper mapper;
        GrPolygonBSPTree polygonBSPTree( hullPolygons, propPolygons, mapper, true, uberTexture );

        // generate ubertextures if necessary.
        if ( uberTexture )
            mapper.GenUberTextures( relativePath + "Uber/" );

        // create the renderable BSP tree and save it to disk.
        GrBSPTree* bspTree = new GrBSPTree( polygonBSPTree, mapper );

        // add lights to the BSP tree.  Only those lights whose parent was optimized
        // out get added.
        tstring error;
        for ( unsigned int i = 0; i < sceneImporter.GetLightCount(); ++i )
        {
            EdImportDAEScene::SLight* daeLight = sceneImporter.GetLight( i );
            GrPolygonNode* node = nodeTree->FindNode( daeLight->parentName.c_str(), false );
            if ( node == 0 )
            {
                URef< GrLight > light = gGrLightMgr->GetLight( daeLight->newName, error );
                if ( !light )
                {
                    // post an error message and go to the next light.
                    EdPrintF( "Error loading light %s... light ignored.\n\nError:\n\n%s",
                        daeLight->parentName.c_str(), error.c_str() );
                    continue;
                }
                bspTree->AddLight( light );
            }
        }

        // save the bsp tree.
        UWriter writer;
        bspTree->Save( writer );
        URef< RFile > file = gRFileMgr->GetFile( relativePath + BSP_PATH"/"BSP_NAME,
            RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
        file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
        delete bspTree;
    }

    // create a model from the remaining nodes/geometry.
    URef< GrModel > model = new GrModel( "scene", nodeTree );
#if 0
    // add lights to the model.
    tstring error;
    for ( unsigned int i = 0; i < sceneImporter.GetLightCount(); ++i )
    {
        EdImportDAEScene::SLight* daeLight = sceneImporter.GetLight( i );
        GrModelNode* node = model->GetRootNode()->FindNode( daeLight->parentName.c_str(), false );
        if ( node )
        {
            URef< GrLight > light = gGrLightMgr->GetLight( daeLight->newName, error );
            if ( !light )
            {
                // post an error message and go to the next light.
                EdPrintF( "Error loading light %s... light ignored.\n\nError:\n\n%s",
                    daeLight->parentName.c_str(), error.c_str() );
                continue;
            }
            node->SetLight( light );
        }
    }
#endif
    // save animation data if necessary.
    if ( anim )
    {
        // save our animation.
        UWriter writer;
        anim->Save( writer );
        URef< RFile > file = gRFileMgr->GetFile( relativePath + ANIM_PATH"/"ANIM_NAME,
            RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
        file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );

        // hook up the animation and the scene model.
        model->SetDefaultAnim( anim );
    }

    // save out the scene model.
    UWriter writer;
    model->Save( writer );
    URef< RFile > file = gRFileMgr->GetFile( relativePath + MODEL_PATH"/"MODEL_NAME,
        RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
    file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );

    // save out a scene definition file.
    WriteSceneDef( relativePath, model );

    // write out the success message.
    EdPrintF( "DAE scene import finished successfully.\n" );

    // return true on success.
    return true;
}

//----------------------------------------------------------
bool
EdImportDAE::ImportModel( const char* name, const char* targetPackageName, const char* path,
                          bool findNormalMaps, bool importAnims )
{
    EdPrintF( "Importing DAE model...\n" );
    if ( !LoadDAEDoc( path ) )
        return false;

    // calculate the output path from the name given.
    // remove any non-alpha numeric values.
    tstring modelName = name;
    size_t len = modelName.length();
    for ( size_t i = 0; i < len; ++i )
    {
        if ( !isalnum( modelName[ i ] ) && modelName[ i ] != '/' && modelName[ i ] != '\\' )
        {
            B_ASSERT( modelName[ i ] != '\n' && modelName[ i ] != '\r' );
            modelName[ i ] = '_';
        }
    }

    // determine the target model path.
    UPath relativeModelPath = "data";
    relativeModelPath += targetPackageName;
    relativeModelPath += "models/";
    UPath targetModelPath = gEdSettings->GetWorkingFolder();
    targetModelPath += relativeModelPath;

    // determine the target model file name.
    UPath relativeModelFilename = targetModelPath + "/" + name;

    // determine what to use as the path for materials and lights.
    UPath targetScenePath = gEdSettings->GetWorkingFolder();
    UPath relativeScenePath = "data/";
    relativeScenePath += targetPackageName;
    targetScenePath += relativeScenePath;

    // make sure the document was set properly.
    B_ASSERT( _doc != 0 );

    // import materials.
    EdImportDAEMaterials materialImporter;
    materialImporter.ImportMaterials( _doc, relativeScenePath, findNormalMaps, false );

    // import geometries.
    EdImportDAEGeometries geometryImporter;
    geometryImporter.ImportGeometries( _doc );

    // import node hierarchy.
    EdImportDAEScene sceneImporter;
    GrKFAnim* anim = 0;
    GrPolygonNode* nodeTree = sceneImporter.ImportScene( &anim, _doc, relativeScenePath, geometryImporter, materialImporter,
        ( relativeScenePath + ANIM_PATH"/"ANIM_NAME_NOEXT ).GetPathString(), false );
    nodeTree->Update();

    // create the model and save.
    {
        // create a model from the remaining nodes/geometry.
        URef< GrModel > model = new GrModel( "scene", nodeTree );

        // add lights to the model.
        tstring errors;
        for ( unsigned int i = 0; i < sceneImporter.GetLightCount(); ++i )
        {
            EdImportDAEScene::SLight* daeLight = sceneImporter.GetLight( i );

            // get the light.
            URef< GrLight > light = gGrLightMgr->GetLight( daeLight->newName, errors );
            if ( !light )
            {
                // post an error message and go to the next light.
                EdPrintF( "Error loading light %s... light ignored.\n\nError:\n\n%s",
                    daeLight->parentName.c_str(), errors.c_str() );
                continue;
            }

            // get the light's parent.
            GrModelNode* parent = model->GetRootNode()->FindNode( daeLight->parentName.c_str(), false );

            // add the light to the model.
            model->AddLight( light, parent );
        }

        // save animation data if necessary.
        if ( anim && importAnims )
        {
            tstring animFileName = name;
            animFileName += ".anm";
            UPath animPath = relativeScenePath + ANIM_PATH"/";
            animPath += animFileName;

            // save our animation.
            UWriter writer;
            anim->Save( writer );
            URef< RFile > file = gRFileMgr->GetFile( animPath, RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
            file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );

            // hook up the animation and the scene model.
            model->SetDefaultAnim( anim );
        }
        else
        {
            delete anim;
            anim = 0;
        }

        // determine the file name + extension.
        relativeModelFilename = ( relativeModelFilename.GetPathString() + ".mdl" );

        // save out the scene model.
        UWriter writer;
        model->Save( writer );
        URef< RFile > file = gRFileMgr->GetFile( relativeModelFilename, RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
        file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
    }

    // write out the success message.
    EdPrintF( "DAE model import finished successfully.\n" );

    // return true on success.
    return true;
}

//----------------------------------------------------------
GrPolygonNode*
EdImportDAE::ImportPolygonScene( const char* path )
{
    EdPrintF( "Importing DAE model...\n" );
    if ( !LoadDAEDoc( path ) )
        return false;

    // make sure the document was set properly.
    B_ASSERT( _doc != 0 );

    // import geometries.
    EdImportDAEGeometries geometryImporter;
    geometryImporter.ImportGeometries( _doc );

    // import node hierarchy.
    EdImportDAEScene sceneImporter;
    GrPolygonNode* nodeTree = sceneImporter.ImportRawGeometry( _doc, geometryImporter );
    nodeTree->Update();

    // write out the success message.
    EdPrintF( "DAE import finished successfully.\n" );

    // return the node tree.
    return nodeTree;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdImportDAE::LoadDAEDoc( const char* path )
{
    // create a new Collada document.
    if ( _doc )
        _doc->Release();
    _doc = FCollada::NewTopDocument();

    // try to load the collada document from a file.  Return false if
    // the file could not be loaded.
    return FCollada::LoadDocumentFromFile( _doc, path );
}

//----------------------------------------------------------
void
EdImportDAE::WriteSceneDef( const UPath& targetPath, GrModel* sceneModel )
{
    tstring sceneDef =
        "//----------------------------------------------------------\n"
        "// File:\t\tScene.sdf\n"
        "// Author:\t\tKevin Bray\n"
        "// Copyright © 2005 Bootstrap Studios\n"
        "//----------------------------------------------------------\n\t\n";

    sceneDef += targetPath.GetPathString();
    sceneDef += "\n{\n";

    // write out Z up or Y up.
    FMVector3 upAxis = _doc->GetAsset()->GetUpAxis();
    if ( upAxis.y > 0.5f )
        sceneDef += "\tUpAxis\t\t\t\tY\n";
    else
        sceneDef += "\tUpAxis\t\t\t\tZ\n";

    // write out default global ambience.
    sceneDef += "\tGlobalAmbience\t\t0.2, 0.2, 0.2, 1.0\n";

    // write out the BSP's path.
    sceneDef += "\tBSPPath\t\t\t\t";
    sceneDef += ( targetPath + BSP_PATH"/"BSP_NAME ).GetPathString();
    sceneDef += "\n";

    // write out the model's instance information.
    sceneDef += "\tModel scene_model\n\t{\n";
    sceneDef += "\t\tPath\t\t\t\t";
    sceneDef += ( targetPath + MODEL_PATH"/"MODEL_NAME ).GetPathString();
    sceneDef += "\n";

    // write out the model's lights.
    unsigned int lightCount = sceneModel->GetLightCount();
    for ( unsigned int i = 0; i < lightCount; ++i )
    {
        GrLight* light = sceneModel->GetLight( i );
        GrModelNode* parent = sceneModel->GetLightParent( i );

        sceneDef += "\t\tLight\t\t\t\t";
        sceneDef += light->GetId().GetPathString();
        if ( parent != sceneModel->GetRootNode() )
        {
            sceneDef += ", ";
            sceneDef += parent->GetName();
        }
        sceneDef += "\n";
    }

    // finish writing the scene model.
    sceneDef += "\t}\n";

    // write out the closing brace.
    sceneDef += "}\n";

    // write out the file.
    URef< RFile > file = gRFileMgr->GetFile( targetPath + GR_SCENEDEF_FILENAME,
        RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
    file->WriteData( 0, sceneDef.c_str(), ( unsigned int )sceneDef.length() );
}

//----------------------------------------------------------
void
EdImportDAE::EnsureFolders( const UPath& fullPath )
{
    const tstring& pathStr = fullPath.GetPathString();
    size_t end = 0;
    while ( true )
    {
        end = pathStr.find( '/', end );
        tstring curPath = pathStr.substr( 0, end );
        CreateDirectory( curPath.c_str(), NULL );
        if ( end == tstring::npos )
            break;
        else
            ++end;
    }
}

//----------------------------------------------------------
unsigned int
EdImportDAE::AddUniqueUVGroupIDs( GrPolygonNode* root, unsigned int start )
{
    // add unique UV group IDs.
    unsigned int meshCount = root->GetMeshCount();
    for ( unsigned int i = 0; i < meshCount; ++i, ++start )
    {
        // get the current mesh polygons.
        URef< GrPolygonMesh > mesh = root->GetMesh( i );
        GrPolygonGroup& meshPolyGroup = mesh->GetPolygons();

        // go through all polygons and add in the ID.
        unsigned int polyCount = meshPolyGroup.GetPolygonCount();
        for ( unsigned int j = 0; j < polyCount; ++j )
            meshPolyGroup.SetPolygonUserData( j, start );
    }

    // recurse.
    unsigned int childCount = root->GetChildCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        start += AddUniqueUVGroupIDs( root->GetChild( i ), start );

    // return.
    return start;
}
