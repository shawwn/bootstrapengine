//----------------------------------------------------------
// File:        GrModel.h
// Author:      Kevin Bray
//
// Purpose:     To manage a model consisting of multiple meshes.
//
// Created:     01-25-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UTable.h"
#include "MMat4x4Array.h"
#include "MMat4x4.h"
#include "MAABox.h"
#include "URef.h"
#include "UPath.h"
#include "GrMesh.h"

// std c++ includes.
#include <map>
#include <vector>

class GrPolygonNode;
class GrPolygonMesh;
class GrAttachment;
class GrMeshInst;
class GrMesh;
class GrFrustum;
class UReader;
class UWriter;
class GrModelNode;
class GrAnimPlayer;
class GrKFAnim;
class GrModelSkinController;
class GrRenderList;
class GrSceneTraverser;
class GrLight;
class MSphere;
class MRay;

//**********************************************************
// class GrModel
//**********************************************************
class GrModel : public URefCount
{
public:
    typedef UFastArray< URef< GrModel > > ModelArray;

    // this is a special constructor reserved for use by the scene only.
    GrModel( const tstring& name );

    // constructs a model out of the node tree passed in.
    GrModel( const UPath& instName, GrPolygonNode* root );
    GrModel( const UPath& fileName, const UPath& instName, UReader& reader );
    ~GrModel();

    // returns information about the model's name.
    const UPath&            GetFileName() const                         {   return _fileName;           }
    const UPath&            GetInstName() const                         {   return _instName;           }

    void                    SetPickable( bool pickable )                {   _pickable = pickable;       }
    bool                    GetPickable() const                         {   return _pickable;           }

    // clones the model.  If deep is true, then the geometry is cloned as well and
    // the object is treated as an original, not a clone.
    GrModel*                Clone( const UPath& instName, bool deep = false, bool lights = false ) const;
    GrModel*                CloneFile( const UPath& fileName, const UPath& instName ) const;

    GrModel*                GetParent() const                           {   return _parent;             }
    GrModelNode*            GetParentNode() const                       {   return _parentNode;         }

    // animation.  If the create flag is set to true, this will create an animation
    // player if one does not exist.
    GrAnimPlayer*           GetAnimPlayer( bool create = false );
    GrAnimPlayer*           GetAnimPlayer() const;

    // ambient animation.
    void                    SetDefaultAnim( GrKFAnim* anim )            {   _defaultAnim = anim;        }
    GrKFAnim*               GetDefaultAnim() const                      {   return _defaultAnim;        }

    // local transform.
    void                    SetLocal( const MMat4x4& transform );
    const MMat4x4&          GetLocal() const                            {   return _local;              }

    // world transform.  Avoid calling these in performance-critical code.  
    // They aren't terribly expensive, but they aren't free.
    void                    SetWorld( const MMat4x4& transform );
    MMat4x4                 GetWorld();

    // returns the current bounds of the model.
    const MAABox&           GetBounds()                                 {   return _bounds;             }

    // node manipulation.
    GrModelNode*            GetRootNode() const                         {   return _root;               }

    // child models.
    void                    AddChild( URef< GrModel > model, GrModelNode* attachTo = 0 );
    unsigned int            GetChildCount() const;
    URef< GrModel >         GetChild( unsigned int idx );
    bool                    RemoveChild( GrModel* model );
    void                    RemoveChildren();

    // child lights.
    void                    AddLight( URef< GrLight > light, GrModelNode* attachTo = 0 );
    unsigned int            GetLightCount() const;
    URef< GrLight >         GetLight( unsigned int idx );
    GrModelNode*            GetLightParent( unsigned int idx );
    bool                    RemoveLight( GrLight* light );
    void                    RemoveLights();

    // recursively finds a model by its instance name or its file name.
    URef< GrModel >         FindModelByInstName( const UPath& instName );
    void                    FindModelsByFileName( UFastArray< URef< GrModel > >& models, const UPath& fileName );

    // retreives all of the mesh instances from the model.
    void                    GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts );

    // retrieves all of the mesh instances from the model and from each of its children.
    void                    GetAllMeshInsts( UFastArray< GrMeshInst* >& meshInsts );

    // a once per frame update that must be called before rendering occurs.  This
    // should only be called if the model is in the scene.
    bool                    Update( bool forceXFormUpdate = false );

    // updates transforms.  This is should usually only be called internally.
    void                    UpdateXForms();

    // ensures that this model and any parent models / modelnodes are up-to-date.  
    // This should usually only be called internally.
    void                    EnsureUpdated();

    // pick against the mesh.
    bool                    Pick( float& dist, const MRay& ray );
    bool                    Pick( GrModel*& model, GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MRay& ray );

    // traverses meshes in the model higherarchy.
    void                    Traverse( GrSceneTraverser* traverser );

    // debug functionality.
    void                    DbgDrawHierarchy();

    // caches and evicts meshes.
    void                    Cache();
    void                    Evict();

    // rendering services.
    void                    GetLitObjectsAndMarkVisible( GrRenderList& lightReceivers, const GrFrustum& frustum, unsigned int frameId ) const;
    void                    GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const MSphere& sphere,
                                           unsigned int frameId ) const;
    void                    GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const GrFrustum& frustum,
                                           unsigned int frameId ) const;

    // query services.
    void                    GetObjectsInFrustum( GrRenderList& objects, const GrFrustum& frustum ) const;

    // load and save the model.  This does not save children.
    void                    Save( UWriter& writer ) const;

    // forces transforms/bounds to be updated.  Internal use only.
    void                    MarkAsDirty();

    // is this the scene model?  This should only be called from the scene.
    void                    SetSceneModel( bool sceneModel );

    // returns true if this model is currently in the scene.
    bool                    IsInScene() const               {   return _inScene;            }

    // user pointer value that can be set on models by someone other
    // than the graphics engine.
    void                    SetUserPtr( void* value )       {   _userPtr = value;           }
    void*                   GetUserPtr() const              {   return _userPtr;            }

private:
    typedef UFastArray< GrModelSkinController* > SkinControllerArray;

    // light attachment.
    struct SLightAttachment
    {
        GrModelNode* parent;
        URef< GrLight > light;
    };
    typedef std::vector< SLightAttachment > LightVec;

    // this is used by the cloning process.
    GrModel();

    void                    SetInScene( bool inScene );
    void                    Load( const UPath& fileName, UReader& reader );
    void                    CreateSkinControllers();
    unsigned int            GetSkinInstCount( GrModelNode* root );
    unsigned int            InitSkinControllers( GrModelNode* root, GrModelSkinController* controllers );
    void                    AddLightsToScene( GrModelNode* root );
    void                    RemoveLightsFromScene( GrModelNode* root );
    void                    Optimize( GrModelNode* root );
    void                    GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModelNode* root );
    bool                    PickNode( GrMeshInst*& mesh, GrMesh::STriCollision& hit, GrModelNode* root, const MRay& ray );
    void                    TraverseNode( GrSceneTraverser* traverser, GrModelNode* root );
    void                    DbgDrawNode( GrModelNode* node );
    void                    Clean();

    // pointer to the root model.
    static GrModel*         _rootModel;

    UPath                   _fileName;              // file we came from.
    UPath                   _instName;              // model name.
    tstring                 _source;                // imported from.
    tstring                 _author;                // creator.
    MMat4x4                 _local;
    LightVec                _lights;
    GrModel*                _parent;
    GrModelNode*            _parentNode;            // node we're attached to in the parent.
    GrModelNode*            _root;
    GrKFAnim*               _defaultAnim;           // default animation (for ambient animations).
    GrAnimPlayer*           _animPlayer;
    ModelArray              _children;              // child models.
    GrModelSkinController*  _skinControllers;       // skin controller array.
    unsigned int            _skinControllerCount;   // number of skin controllers.
    MAABox                  _bounds;
    mutable unsigned int    _lastVisibleFrameId;
    void*                   _userPtr;
    bool                    _inScene;
    bool                    _dirty;
    bool                    _pickable;

    // perhaps store joint info for nodes?  perhaps the scene should be what ties
    // everything together?
};
