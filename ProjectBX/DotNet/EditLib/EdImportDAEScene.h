//----------------------------------------------------------
// File:        EdImportDAEScene.h
// Author:      Kevin Bray
// Created:     11-28-06
//
// Purpose:     To import a scene.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// std c++ includes.
#include <vector>

// forward declarations.
class EdImportDAEGeometries;
class EdImportDAEMaterials;
class FCDGeometryInstance;
class FCDEntityInstance;
class GrPolygonNode;
class GrPolygonMesh;
class FCDocument;
class FCDSceneNode;
class FCDLight;
class FMMatrix44;
class GrKFAnim;

//**********************************************************
// class EdImportDAEScene
//**********************************************************
class EdImportDAEScene
{
public:
    struct SLight
    {
        tstring parentName;
        tstring newName;
        FCDEntityInstance* lightInst;
    };

    struct SAnimNode;
    typedef std::vector< SAnimNode > NodeVector;

    EdImportDAEScene();
    ~EdImportDAEScene();

    // imports the scene.
    GrPolygonNode*          ImportScene( GrKFAnim** anim, FCDocument* doc, const UPath& outputPath, const EdImportDAEGeometries& geometryImporter,
                                         const EdImportDAEMaterials& materialImporter, const tstring& animName, bool overwriteLights );
    GrPolygonNode*          ImportRawGeometry( FCDocument* doc, const EdImportDAEGeometries& geometryImporter );

    unsigned int            GetLightCount() const;
    SLight*                 GetLight( unsigned int idx ) const;

private:
    typedef std::vector< SLight* > LightVec;

    GrKFAnim*               ImportAnimation( const tstring& animName ) const;
    GrPolygonNode*          ImportNode( FCDSceneNode* node, const UPath& outputPath, const EdImportDAEGeometries& geometryImporter,
                                        const EdImportDAEMaterials& materialImporter );
    GrPolygonNode*          ImportRawNode( FCDSceneNode* node, const EdImportDAEGeometries& geometryImporter );
    GrPolygonMesh*          ImportMesh( FCDGeometryInstance* instance, const EdImportDAEGeometries& geometryImporter,
                                        const EdImportDAEMaterials& materialImporter );
    GrPolygonMesh*          ImportRawMesh( FCDGeometryInstance* instance, const EdImportDAEGeometries& geometryImporter );
    void                    ImportLight( const UPath& outputPath, FCDEntityInstance* instance );
    void                    WriteLights( const UPath& outputPath, bool overwriteExisting );

    NodeVector              _animatedNodes;
    FCDSceneNode*           _root;
    LightVec                _lights;
};
