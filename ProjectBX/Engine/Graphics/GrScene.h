//----------------------------------------------------------
// File:        GrScene.h
// Author:      Kevin Bray
// Created:     11-09-04
//
// Purpose:     To manage the 3D scene.  Essentially a spatially relational
//              database.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrColor.h"
#include "UFastArray.h"
#include "GrLightSet.h"
#include "UPath.h"
#include "GrMesh.h"

// std c++ includes.
#include <vector>
#include <map>

class GrCamera;
class GrNode;
class GrMeshInst;
class GrSceneTraverser;
class MVec3;
class GrBSPTree;
class GrBSPSplit;
class GrPolygonGroup;
class GrPolygonBSPTree;
class GrRenderTarget;
class GrRenderList;
class GrSector;
class GrLight;
class UWriter;
class UReader;
class GrTexGen;
class GrModel;
class GrModelNode;
class UPath;
class GrRenderList;
class GrFrustum;

//**********************************************************
// class GrScene
//**********************************************************
class GrScene
{
public:
    // NOTE: The values of the flags in Scene.h and EdPick.h must match
    // the values of the corresponding flags in this enumeration.
    typedef unsigned int PickFlags;
    static const unsigned int kPickNone         = 0x00;
    static const unsigned int kPickBSP          = 0x01;
    static const unsigned int kPickModels       = 0x02;
    static const unsigned int kPickModelNodes   = 0x04;
    static const unsigned int kPickAll          = kPickBSP | kPickModels | kPickModelNodes;

    GrScene();
    ~GrScene();

    // returns the location of the current scene's SDF.
    const UPath&            GetSceneDefPath() const         {   return _sceneDefPath;   }

    // load a scene.
    bool                    LoadScene( const UPath& path, tstring& errors );

    // resets the scene.
    void                    Reset( const UPath& sceneDefPath, const tstring& name, GrBSPTree* bspTree, bool zUp );

    // returns true if the scene is Z up, false if Y up (both are right handed).
    bool                    GetZUp() const                                  {   return _zUp;                }

    // updates dynamic elements in the scene that are specific to the scene.
    void                    Update( unsigned int gameTimeDelta, unsigned int sysTimeDelta );

    // mutators.
    void                    SetName( const tstring& name )                  {   _name = name;               }

    // scene name.
    const tstring&          GetName() const                                 {   return _name;               }

    // clear color.
    void                    SetClearColor( const GrColor& color )           {   _clearColor = color;        }
    const GrColor&          GetClearColor() const                           {   return _clearColor;         }

    // scene time.
    unsigned int            GetSceneTime() const                            {   return _sceneTime;          }
    unsigned int            GetTimeDelta() const                            {   return _timeDelta;          }
    unsigned int            GetFrameId() const                              {   return _frameId;            }

    // perform a picking operation on the scene.
    PickFlags               Pick( GrModel*& model, GrModelNode*& modelNode, GrMeshInst*& meshInst, GrMesh::STriCollision& hit,
                                  const MRay& ray, PickFlags flags = kPickAll );

    // accessors.
    GrBSPTree*              GetBSPTree() const                              {   return _bspTree;            }
    GrModel*                GetRootModel() const                            {   return _rootModel;          }
    GrModel*                GetSkyboxModel() const                          {   return _skyboxModel;        }

    // light management.
    void                    AddLight( URef< GrLight > light );
    URef< GrLight >         FindLight( const char* id, int cmpLen = -1 ) const;
    bool                    RemoveLight( const char* id );
    bool                    RemoveLight( GrLight* light );
    unsigned int            GetLightCount() const;
    URef< GrLight >         GetLight( unsigned int idx ) const;

    // ambient lighting
    void                    SetGlobalAmbience( const GrColor& ambience )    {   _globalAmbience = ambience; }
    const GrColor&          GetGlobalAmbience() const                       {   return _globalAmbience;     }

    // enable/disable all lights.
    void                    SetLightEnable( bool enabled )                  {   _lightEnable = enabled;     }

    // debug functionality
    void                    DbgSetWireframe( bool wireframe )               {   _wireframe = wireframe;     }
    bool                    DbgGetWireframe() const                         {   return _wireframe;          }
    void                    DbgDrawHierarchy();

    void                    ShowAffected( GrLight* light );     // show scene elements affected by the light.

    // caches or evicts geometry.
    void                    Cache();
    void                    Evict();

    // render the entire scene using the camera passed in.
    void                    Render( const GrCamera* camera );

    // render the scene onto a texture.  Note that the texture is expected to
    // be the currently bound framebuffer.
    void                    RenderTexture( const GrCamera* camera, const MVec3& pvsEyePos );

    // empty out the scene.
    void                    Clear();

    // scene object traversal.
    void                    Traverse( GrSceneTraverser* traverser, PickFlags elems = kPickAll );

    // builds a list of meshInsts that overlap a frustum.
    void                    FindMeshInstsInFrustum( GrRenderList& meshInsts, const GrFrustum& frustum );

private:
    typedef std::vector< GrSector* > SectorVector;
    typedef UFastArray< GrModel* > ModelArray;

    void                    ClearFramebuffer();
    void                    RenderLights( const GrCamera* camera, const MVec3& pvsEyePos );
    void                    Clean();

    // file the scene came from.
    UPath                   _sceneDefPath;

    // name of the current scene.
    tstring                 _name;

    // global ambient light color.
    GrColor                 _globalAmbience;

    GrBSPTree*              _bspTree;
    URef< GrModel >         _rootModel;
    URef< GrModel >         _skyboxModel;

    GrLightSet              _lights;
    GrLightSet              _worldLights;

    GrColor                 _clearColor;
    unsigned int            _timeDelta;
    unsigned int            _sceneTime;
    unsigned int            _frameId;
    bool                    _inLightRenderTex;
    bool                    _wireframe;
    bool                    _lightEnable;
    bool                    _zUp;
//  SectorVector            _sectors;
};
extern GrScene* gGrScene;
