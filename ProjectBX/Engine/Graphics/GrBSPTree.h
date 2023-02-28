//----------------------------------------------------------
// File:        GrBSPTree.h
// Author:      Kevin Bray
// Created:     11-06-06
//
// Purpose:     To manage a renderable BSP tree.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrLightSet.h"
#include "MVec3.h"
#include "UPath.h"
#include "GrMesh.h"

// std c++ includes.
#include <vector>
#include <map>

class GrBSPArea;
class GrBSPSplit;
class GrPolygonBSPTree;
class GrPolygonUberUVMapper;
class GrFrustum;
class GrMeshInst;
class GrLight;
class MSphere;
class UReader;
class UWriter;
class GrRenderList;
class GrSceneTraverser;

//**********************************************************
// class GrBSPTree
//**********************************************************
class GrBSPTree
{
public:
    typedef std::vector< GrMeshInst* > MeshInstVec;

    GrBSPTree();
    GrBSPTree( const UPath& fileName, UReader& reader );
    GrBSPTree( const GrPolygonBSPTree& polygonBSPTree, const GrPolygonUberUVMapper& mapper );
    ~GrBSPTree();

    // returns the name of the file the BSP was loaded from (if any).
    const UPath&            GetFileName() const                 {   return _fileName;           }

    // sets the in scene flag to true if we're in the scene.  This also
    // causes lights to be added or removed from the scene.
    void                    SetInScene( bool inScene );

    // light management.
    const GrLightSet&       GetLightSet() const                 {   return _lights;             }
    void                    AddLight( URef< GrLight > light );
    URef< GrLight >         FindLight( const char* name, int cmpLen = -1 ) const;
    bool                    RemoveLight( const char* name );
    bool                    RemoveLight( GrLight* light );

    // returns the list of all mesh instances.
    const MeshInstVec&      GetMeshInsts() const                {   return _meshes;             }

    // picks the scene and returns the mesh and it's hit info.
    bool                    Pick( GrMeshInst*& mesh, GrMesh::STriCollision& hit, const MRay& ray );

    // traverses the BSP tree.
    void                    Traverse( GrSceneTraverser* traverser );

    // cache/evict meshes.
    void                    Cache();
    void                    Evict();

    // sends all objects in the frustum to the renderer and marks them as visible.
    // this should be called before any of the AddLitToRenderer calls.
    void                    GetLitObjectsAndMarkVisible( GrRenderList& lightReceivers, const GrFrustum& frustum, const MVec3& eyePos,
                                                         unsigned int frameId ) const;
    bool                    GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const MSphere& region,
                                           unsigned int frameId ) const;
    bool                    GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const GrFrustum& region,
                                           unsigned int frameId ) const;

    // query services.
    bool                    GetObjectsInFrustum( GrRenderList& objects, const GrFrustum& region ) const;

    // load/save.
    void                    Load( UReader& reader );
    void                    Save( UWriter& writer );

private:
    void                    AddLightsToScene();
    void                    RemoveLightsFromScene();
    void                    Clean();

    struct SLightCacheEntry
    {
        SLightCacheEntry() : area( 0 ) { }
        SLightCacheEntry( const MVec3& oldPos, GrBSPArea* area )
            : oldPos( oldPos ), area( area ) { }

        MVec3 oldPos;
        GrBSPArea* area;
    };

    typedef std::vector< GrBSPArea* > AreaVector;
    typedef std::map< const GrLight*, SLightCacheEntry > LightBSPCache;

    UPath                   _fileName;
    GrBSPSplit*             _root;
    AreaVector              _areas;
    MeshInstVec             _meshes;
    GrLightSet              _lights;
    bool                    _inScene;
};
