//----------------------------------------------------------
// File:        EdImportDAE.h
// Author:      Kevin Bray
// Created:     11-23-06
//
// Purpose:     To import collada files.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// forward declarations.
class FCDocument;
class GrPolygonNode;
class GrModel;

//**********************************************************
// class EdImportDAE
//**********************************************************
class EdImportDAE
{
public:
    EdImportDAE();
    ~EdImportDAE();

    // imports an engine resource from a collada file.
    bool            ImportScene( const char* name, const char* srcPath, bool findNormalMaps, bool uberTexture,
                                 bool overwriteMaterials, bool overwriteLights );
    bool            ImportModel( const char* name, const char* targetPackageName, const char* srcPath,
                                 bool findNormalMaps, bool importAnims );
    GrPolygonNode*  ImportPolygonScene( const char* path );

private:
    bool            LoadDAEDoc( const char* path );
    void            ImportMaterials();
    void            ImportGeometries();
    void            ImportNodes();
    void            WriteSceneDef( const UPath& targetPath, GrModel* sceneModel );
    void            EnsureFolders( const UPath& fullPath );
    unsigned int    AddUniqueUVGroupIDs( GrPolygonNode* root, unsigned int start );

    // last document loaded.
    FCDocument*     _doc;
};
