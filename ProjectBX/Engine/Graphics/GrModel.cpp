//----------------------------------------------------------
// File:        GrModel.h
// Author:      Kevin Bray
// Created:     01-25-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrModel.h"

// project includes.
#include "GrModelNode.h"
#include "GrLight.h"
#include "GrMeshInst.h"
#include "GrFrustum.h"
#include "GrPolygonNode.h"
#include "GrPolygonMesh.h"
#include "GrAnimPlayer.h"
#include "GrSceneTraverser.h"
#include "GrKFAnim.h"
#include "GrKFAnimMgr.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrScene.h"
#include "GrSkin.h"
#include "GrModelSkinController.h"
#include "GrRenderList.h"
#include "GrModelMgr.h"
#include "GrLightMgr.h"
#include "GLSubsys.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "MRay.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "GrUtil.h"

GrModel*
GrModel::_rootModel = 0;

//**********************************************************
// class GrModel
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrModel::GrModel( const tstring& name )
: _fileName( "" )
, _instName( name )
, _parent( 0 )
, _parentNode( 0 )
, _root( 0 )
, _animPlayer( 0 )
, _defaultAnim( 0 )
, _skinControllers( 0 )
, _skinControllerCount( 0 )
, _lastVisibleFrameId( ~0 )
, _userPtr( 0 )
, _dirty( true )
, _inScene( true )
, _pickable( true )
{
    // store this model as the root model.
    _rootModel = this;

    // build the model node tree from the polygon node tree.
    _root = new GrModelNode( this, new GrPolygonNode( name, MMat4x4() ) );

    // create the skin controllers.
    CreateSkinControllers();
}

//----------------------------------------------------------
GrModel::GrModel( const UPath& instName, GrPolygonNode* root )
: _fileName( "" )
, _instName( instName )
, _parent( 0 )
, _parentNode( 0 )
, _root( 0 )
, _animPlayer( 0 )
, _defaultAnim( 0 )
, _skinControllers( 0 )
, _skinControllerCount( 0 )
, _lastVisibleFrameId( ~0 )
, _userPtr( 0 )
, _dirty( true )
, _inScene( false )
, _pickable( true )
{
    // build the model node tree from the polygon node tree.
    _root = new GrModelNode( this, root );

    // create the skin controllers.
    CreateSkinControllers();
}

//----------------------------------------------------------
GrModel::GrModel()
: _fileName( "" )
, _instName( "" )
, _parent( 0 )
, _parentNode( 0 )
, _root( 0 )
, _animPlayer( 0 )
, _defaultAnim( 0 )
, _skinControllers( 0 )
, _skinControllerCount( 0 )
, _lastVisibleFrameId( ~0 )
, _userPtr( 0 )
, _dirty( true )
, _inScene( false )
, _pickable( true )
{

}

//----------------------------------------------------------
GrModel::GrModel( const UPath& fileName, const UPath& instName, UReader& reader )
: _fileName( fileName )
, _instName( instName )
, _parent( 0 )
, _parentNode( 0 )
, _root( 0 )
, _animPlayer( 0 )
, _defaultAnim( 0 )
, _skinControllers( 0 )
, _skinControllerCount( 0 )
, _lastVisibleFrameId( ~0 )
, _userPtr( 0 )
, _dirty( true )
, _inScene( false )
, _pickable( true )
{
    Load( fileName, reader );
}

//----------------------------------------------------------
GrModel::~GrModel()
{
    // detach from the parent if we're deleted.
    Clean();
    if ( this == _rootModel )
        _rootModel = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrModel*
GrModel::Clone( const UPath& instName, bool deep, bool lights ) const
{
    // there shouldn't be any reason to clone the root model.
    B_ASSERT( this != _rootModel );

    // create a new model.
    GrModel* newModel = new GrModel;

    // copy the settings.
    newModel->_fileName = _fileName;
    newModel->_instName = instName;
    newModel->_source = _source;
    newModel->_author = _author;
    newModel->_pickable = _pickable;

    // clone the node tree.
    newModel->_root = _root->Clone( newModel, deep );

    // clone the default animation.
    if ( _defaultAnim )
    {
        // clone the default animation.
        newModel->_defaultAnim = ( GrKFAnim* )_defaultAnim->Clone();

        // now create a new animation player with the default animation
        // and get it started.
        GrAnimPlayer* newAnimPlayer = new GrAnimPlayer( newModel );
        unsigned int id = newAnimPlayer->AddAnim( "default", _defaultAnim );
        newAnimPlayer->SetActiveAnim( id );
        newAnimPlayer->Play();

        // store the new animation player to the new model.
        newModel->_animPlayer = newAnimPlayer;
    }

    // clone the lights.
    if ( lights )
    {
        tstring errors;
        unsigned int lightCount = ( unsigned int )_lights.size();
        for ( unsigned int i = 0; i < lightCount; ++i )
        {
            const UPath& originalLightName = _lights[ i ].light->GetId();
            UPath newLightName( originalLightName.GetPathString() + instName.GetPathString() );

            // get or clone the current light.
            URef< GrLight > light = gGrLightMgr->GetOrCloneLight( newLightName, originalLightName, errors );
            if ( !light )
                continue;

            // now get the current light's parent name.
            tstring parentName = _lights[ i ].parent->GetName();
            GrModelNode* parent = newModel->_root->FindNode( parentName.c_str(), false );

            // add the light to the new model.
            newModel->AddLight( light, parent );
        }
    }

    // return the newly created model.
    return newModel;
}

//----------------------------------------------------------
GrModel*
GrModel::CloneFile( const UPath& fileName, const UPath& instName ) const
{
    // clone the model.
    GrModel* model = Clone( instName, true, true );

    // fill out the model's file information.
    model->_fileName = fileName;

    // save the file.
    UWriter writer;
    model->Save( writer );

    // get the file for writing.
    URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagWrite );
    if ( !file )
    {
        // delete the model and return null to indicate failure.
        delete model;
        return 0;
    }

    // write the data to the disk.
    file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );

    // add the new model to the model manager.
    gGrModelMgr->AddModel( model );

    // return the newly created model.
    return model;
}

//----------------------------------------------------------
GrAnimPlayer*
GrModel::GetAnimPlayer( bool create )
{
    if ( create && !_animPlayer )
        _animPlayer = new GrAnimPlayer( this );
    return _animPlayer;
}

//----------------------------------------------------------
GrAnimPlayer*
GrModel::GetAnimPlayer() const
{
    return _animPlayer;
}

//----------------------------------------------------------
void
GrModel::SetLocal( const MMat4x4& transform )
{
    _local = transform;
    _dirty = true;
}

//----------------------------------------------------------
void
GrModel::SetWorld( const MMat4x4& transform )
{
    // grab our parent's worldspace transform.
    MMat4x4 parentWorld = MMat4x4::Identity();
    if ( _parentNode )
        parentWorld = _parentNode->GetWorld();
    else if ( _parent )
        parentWorld = _parent->GetWorld();

    // invert it.
    MMat4x4 parentWorldInv;
    bool success = parentWorld.Inverse( parentWorldInv );
    B_ASSERT( success );
    if ( success )
    {
        // transform the incoming matrix into localspace and store it.
        _local = parentWorldInv * transform;
        _dirty = true;
    }
}

//----------------------------------------------------------
MMat4x4
GrModel::GetWorld() 
{
    // ensure that our transforms are up-to-date.
    EnsureUpdated();

    // if we have a parent node, multiply our transform by it.
    if ( _parentNode )
        return _parentNode->GetWorld() * _local;

    // if we have a parent model, multiply our transform by it.
    if ( _parent )
        return _parent->GetWorld() * _local;

    // otherwise, we have no parent, so return our local transform 
    // since it's also our worldspace transform.
    return _local;
}

//----------------------------------------------------------
void
GrModel::AddChild( URef< GrModel > model, GrModelNode* attachTo )
{
    // if no node was specified, then use the root node.
    if ( attachTo == 0 )
        attachTo = _root;

    // the node the child is being attached to must be owned by this model!
    // also ensure that we're not trying to attach to ourselves!
    B_ASSERT( attachTo->GetOwner() == this );
    B_ASSERT( model != _rootModel );
    B_ASSERT( model != this );
    B_ASSERT( model != 0 );

    // set the parent information on the child.
    model->_parent = this;
    model->_parentNode = attachTo;
    model->_dirty = true;
    if ( model->_inScene != _inScene )
        model->SetInScene( _inScene );

    // add the child node to our child list.
    _children.Push( model );
}

//----------------------------------------------------------
unsigned int
GrModel::GetChildCount() const
{
    return _children.GetElemCount();
}

//----------------------------------------------------------
URef< GrModel >
GrModel::GetChild( unsigned int idx )
{
    B_ASSERT( idx < _children.GetElemCount() );
    return _children[ idx ];
}

//----------------------------------------------------------
bool
GrModel::RemoveChild( GrModel* model )
{
    // the root model cannot have any parents.  Therefor, it can't possibly
    // be in our list and this call is unnecessary.
    B_ASSERT( model != _rootModel );

    // try to find the child and remove it from our child list.
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        if ( _children[ i ] == model )
        {
            // if we're in the scene, then we need to tell the child model
            // that it's being removed from the scene.
            if ( _inScene )
                model->SetInScene( false );

            // clear all of the parent related stuff.
            model->_parent = 0;
            model->_parentNode = 0;
            model->_dirty = true;

            // remove the child from our list.
            _children.Erase( i );
            return true;
        }
    }

    // the child wasn't found.
    return false;
}

//----------------------------------------------------------
void
GrModel::RemoveChildren()
{
    // try to find the child and remove it from our child list.
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        // get the current model.
        GrModel* curModel = _children[ i ];

        // if we're in the scene, then we need to tell the child model
        // that it's being removed from the scene.
        if ( _inScene )
            curModel->SetInScene( false );

        // clear the current model's parent info.
        curModel->_parent = 0;
        curModel->_parentNode = 0;
        curModel->_dirty = true;
    }
    _children.Clear();
}

//----------------------------------------------------------
void
GrModel::AddLight( URef< GrLight > light, GrModelNode* attachTo )
{
    // if the light is invalid, skip it.  This can happen when a model file
    // references an undefined light.
    if ( !light )
        return;

    if ( !attachTo )
        attachTo = _root;

    SLightAttachment attachment;
    attachment.parent = attachTo;
    attachment.light = light;
    _lights.push_back( attachment );

    if ( _inScene )
        gGrScene->AddLight( light );
}

//----------------------------------------------------------
unsigned int
GrModel::GetLightCount() const
{
    return ( unsigned int )_lights.size();
}

//----------------------------------------------------------
URef< GrLight >
GrModel::GetLight( unsigned int idx )
{
    return _lights[ idx ].light;
}

//----------------------------------------------------------
GrModelNode*
GrModel::GetLightParent( unsigned int idx )
{
    return _lights[ idx ].parent;
}

//----------------------------------------------------------
bool
GrModel::RemoveLight( GrLight* light )
{
    // try to find the light specified and remove it from the light
    // attachment array.
    LightVec::iterator iter = _lights.begin();
    LightVec::iterator end = _lights.end();
    for ( ; iter != end; ++iter )
    {
        if ( iter->light == light )
        {
            FastVectorErase( _lights, iter );
            return true;
        }
    }

    if ( _inScene )
        gGrScene->RemoveLight( light );

    // return false to indicate failure.
    return false;
}

//----------------------------------------------------------
void
GrModel::RemoveLights()
{
    if ( _inScene )
    {
        LightVec::iterator iter = _lights.begin();
        LightVec::iterator end = _lights.end();
        for ( ; iter != end; ++iter )
            gGrScene->RemoveLight( iter->light );
    }
    _lights.resize( 0 );
}

//----------------------------------------------------------
URef< GrModel >
GrModel::FindModelByInstName( const UPath& instName )
{
    // try to find the child and remove it from our child list.
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        // is the current model the one we're looking for?
        GrModel* curModel = _children[ i ];
        if ( curModel->GetInstName() == instName )
            return curModel;

        // recurse.
        curModel->FindModelByInstName( instName );
    }

    // not found!
    return 0;
}

//----------------------------------------------------------
void
GrModel::FindModelsByFileName( UFastArray< URef< GrModel > >& models, const UPath& fileName )
{
    // try to find the child and remove it from our child list.
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        // is the current model the one we're looking for?
        GrModel* curModel = _children[ i ];
        if ( curModel->GetFileName() == fileName )
            models.Push( curModel );

        // recurse.
        curModel->FindModelsByFileName( models, fileName );
    }
}

//----------------------------------------------------------
void
GrModel::GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts )
{
    GetMeshInsts( meshInsts, _root );
}

//----------------------------------------------------------
void
GrModel::GetAllMeshInsts( UFastArray< GrMeshInst* >& meshInsts )
{
    // gather our meshinsts.
    GetMeshInsts( meshInsts, _root );

    // recurse to children.
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        GrModel* child = _children[ i ];
        child->GetAllMeshInsts( meshInsts );
    }
}

//----------------------------------------------------------
bool
GrModel::Update( bool forceXFormUpdate )
{
    // update and apply animations if any exist.
    if ( _animPlayer )
    {
        // update animation if necessary.
        unsigned int timeDelta = gGrScene->GetTimeDelta();
        if ( timeDelta > 0 )
            _animPlayer->ApplyPose( float( timeDelta ) / 1000.0f );
    }

    // update transforms.
    forceXFormUpdate = forceXFormUpdate || _dirty;
    if ( forceXFormUpdate )
        UpdateXForms();

    // update skin instances.
    for ( unsigned int i = 0; i < _skinControllerCount; ++i )
        _skinControllers[ i ].Update();

    // calculate a new bounds if necessary.
    if ( forceXFormUpdate )
    {
        _bounds.SetCenter( _local.GetTranslate() );
        _bounds.SetHalfExts( MVec3() );
        _root->CalcBoundsExact( _bounds );
    }

    // update children.
    bool boundsUpdate = forceXFormUpdate;
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        // update the child and adjust our bounds if necessary.
        if ( _children[ i ]->Update( forceXFormUpdate ) )
        {
            boundsUpdate = _bounds.AddBoxToVolume( _children[ i ]->GetBounds() ) || boundsUpdate;
        }
    }

    // update lights.
    LightVec::iterator iter = _lights.begin();
    LightVec::iterator end = _lights.end();
    for ( ; iter != end; ++iter )
        iter->light->SetParentTransform( iter->parent->GetWorld() );

    // return whether or not the bounds has changed.
    return boundsUpdate;
}

//----------------------------------------------------------
void
GrModel::UpdateXForms()
{
    // clear the dirty flag.
    _dirty = false;

    // update the node tree.
    if ( _parentNode )
        _root->Update( _parentNode->GetWorld() * _local );
    else
        _root->Update( _local );
}

//----------------------------------------------------------
void
GrModel::EnsureUpdated()
{
    // if we have a parent, update it; otherwise, update ourselves.
    if ( _parent )
        _parent->EnsureUpdated();
    else
        Update();
}

//----------------------------------------------------------
bool
GrModel::Pick( float& dist, const MRay& ray )
{
    // try to hit against this model.
    GrModel* model = 0;
    GrMeshInst* meshInst = 0;
    GrMesh::STriCollision hit;
    if ( !Pick( model, meshInst, hit, ray ) )
        return false;

    // calculate the hit distance.
    dist = ray.GetStart().Dist( hit.hitLoc );

    // return true to indicate a hit.
    return true;
}

//----------------------------------------------------------
bool
GrModel::Pick( GrModel*& model, GrMeshInst*& mesh, GrMesh::STriCollision& hit, const MRay& ray )
{
    // make sure the model is up to date with regards to it's transforms.
    Update( true );

    // check the intersection of the ray and the bounds.  If the bounds
    // doesn't intersect with the bounds, we can simply return false.
    if ( !ray.Intersect( GetBounds() ) )
        return false;

    // useful vars.
    float curDistSqr = 65536.0f;
    bool rayHitSomething = false;

    // do we need to pick against meshes?
    if ( _pickable )
    {
        rayHitSomething = PickNode( mesh, hit, _root, ray );
        if ( rayHitSomething )
            curDistSqr = ray.GetStart().DistSqr( hit.hitLoc );
    }

    // recurse children.
    GrMeshInst* childMesh = 0;
    GrMesh::STriCollision childHit;
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        // get the current child and pick.
        GrModel* curChild = _children[ i ];
        if ( curChild->Pick( model, childMesh, childHit, ray ) )
        {
            float childDistSqr = ray.GetStart().DistSqr( childHit.hitLoc );
            if ( childDistSqr < curDistSqr )
            {
                rayHitSomething = true;
                curDistSqr = childDistSqr;
                hit = childHit;
                mesh = childMesh;
                model = curChild; 
            }
        }
    }

    // return true if the ray hit something.
    return rayHitSomething;
}

//----------------------------------------------------------
void
GrModel::Traverse( GrSceneTraverser* traverser )
{
    // check to see if we should even traverse this model.
    if ( !traverser->OnNode( _bounds ) )
        return;

    // check all meshes in the node higherarchy.
    TraverseNode( traverser, _root );

    // recurse.
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        _children[ i ]->Traverse( traverser );
}

//----------------------------------------------------------
void
GrModel::DbgDrawHierarchy()
{
    GrBindShader( 0 );
    GrSetState( 0 );
    GrSetWriteEnable( GR_RGBA );
    bglBegin( GL_LINES );
    bglColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
    DbgDrawNode( _root );
    bglEnd();
}

//----------------------------------------------------------
void
GrModel::Cache()
{
    _root->Cache();
}

//----------------------------------------------------------
void
GrModel::Evict()
{
    _root->Evict();
}

//----------------------------------------------------------
void
GrModel::GetLitObjectsAndMarkVisible( GrRenderList& lightReceivers, const GrFrustum& frustum, unsigned int frameId ) const
{
    // if we're not inside of the frustum passed in, then we're not visible.
    if ( !frustum.IsAABoxInside( _bounds ) )
        return;

    // set the last visible frame ID and add nodes to the renderer.
    _lastVisibleFrameId = frameId;
    _root->GetRenderableObjects( lightReceivers, 0 );

    // recurse children.
    const unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        B_ASSERT( _children[ i ] != 0 );
        _children[ i ]->GetLitObjectsAndMarkVisible( lightReceivers, frustum, frameId );
    }
}

//----------------------------------------------------------
void
GrModel::GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const MSphere& sphere,
                        unsigned int frameId ) const
{
    if ( !shadowCasters && _lastVisibleFrameId != frameId )
        return;

    if ( !_bounds.Intersect( sphere ) )
        return;

    // add lit objects to the renderer.
    _root->GetRenderableObjects( lightReceivers, shadowCasters );

    // recurse children.
    const unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        B_ASSERT( _children[ i ] != 0 );
        _children[ i ]->GetLitObjects( lightReceivers, shadowCasters, sphere, frameId );
    }
}

//----------------------------------------------------------
void
GrModel::GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const GrFrustum& frustum,
                        unsigned int frameId ) const
{
    if ( !shadowCasters && _lastVisibleFrameId != frameId )
        return;

    // are we inside of the sphere?
    if ( !frustum.IsAABoxInside( _bounds ) )
        return;

    // add all meshes objects to the renderer.
    _root->GetRenderableObjects( lightReceivers, shadowCasters );

    // recurse children.
    const unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        B_ASSERT( _children[ i ] != 0 );
        _children[ i ]->GetLitObjects( lightReceivers, shadowCasters, frustum, frameId );
    }
}

//----------------------------------------------------------
void                    
GrModel::GetObjectsInFrustum( GrRenderList& objects, const GrFrustum& frustum ) const
{
    // if the model needs to be updated, abort.
    B_ASSERT( !_dirty );
    if ( _dirty )
        return;

    // are we inside of the sphere?
    if ( !frustum.IsAABoxInside( _bounds ) )
        return;

    // add all meshes objects to the renderer.
    _root->GetRenderableObjects( objects, 0 );

    // recurse children.
    const unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        B_ASSERT( _children[ i ] != 0 );
        _children[ i ]->GetObjectsInFrustum( objects, frustum );
    }
}

//----------------------------------------------------------
void
GrModel::Save( UWriter& writer ) const
{
    // model tag.
    writer.WriteString( "MDL" );    // MDL\0

    // save misc info.
    writer.WriteInt( 0x00010001 );  // version.
    writer.WriteString( _source );  // source file.
    writer.WriteString( _author );  // author.

    // save the model transform.
    _local.Save( writer );

    // write out the ambient animation path.
    writer.WriteString( _defaultAnim ? _defaultAnim->GetName() : "" );

    // get all meshes so we can save them out.
    UFastArray< URef< GrMesh > > meshArray;
    _root->BuildMeshList( meshArray );
    const unsigned int meshCount = meshArray.GetElemCount();

    // build a list of all skins.
    UFastArray< URef< GrSkin > > skinArray;
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        GrSkin* curSkin = meshArray[ i ]->GetSkin();
        if ( curSkin != 0 )
        {
            unsigned int j = 0;
            for ( ; j < skinArray.GetElemCount(); ++j )
            {
                if ( skinArray[ j ]->GetName() == curSkin->GetName() )
                    break;
            }
            if ( j == skinArray.GetElemCount() )
                skinArray.Push( curSkin );
        }
    }

    // now save out skins.
    const unsigned int skinCount = skinArray.GetElemCount();
    writer.WriteInt( skinCount );
    for ( unsigned int i = 0; i < skinCount; ++i )
        skinArray[ i ]->Save( writer );

    // save out meshes.
    writer.WriteInt( meshCount );
    for ( unsigned int i = 0; i < meshCount; ++i )
        meshArray[ i ]->Save( writer );

    // save the node tree.
    _root->Save( writer );

    // save out instanceable lights
    unsigned int lightCount = ( unsigned int )_lights.size();
    writer.WriteInt( lightCount );
    for ( unsigned int i = 0; i < lightCount; ++i )
    {
        writer.WriteString( _lights[ i ].light->GetName() );
        writer.WriteString( _lights[ i ].parent->GetName() );
    }
}

//----------------------------------------------------------
void
GrModel::MarkAsDirty()
{
    // flag this model as dirty, as well as all of it's
    // child models.
    _dirty = true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrModel::SetInScene( bool inScene )
{
    // someone should have checked this before calling!
    B_ASSERT( _inScene != inScene );

    // notify the nodes that we've been added to the scene.
    LightVec::iterator iter = _lights.begin();
    LightVec::iterator end = _lights.end();
    if ( inScene )
    {
        for ( ; iter != end; ++iter )
            gGrScene->AddLight( iter->light );
    }
    else
    {
        for ( ; iter != end; ++iter )
            gGrScene->RemoveLight( iter->light );
    }

    // set the _inScene flag and recurse to children.
    _inScene = inScene;
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        _children[ i ]->SetInScene( inScene );
}

//----------------------------------------------------------
void
GrModel::Load( const UPath& fileName, UReader& reader )
{
    Clean();

    // make sure the file we're loading is valid.
    tstring type = reader.ReadString();
    B_ASSERT( type == "MDL" );

    // check the version.
    unsigned short minorVer = reader.ReadShort();
    unsigned short majorVer = reader.ReadShort();
    B_ASSERT( majorVer == 1 );

    // load misc info.
    _fileName = fileName;
    _source = reader.ReadString();
    _author = reader.ReadString();

    // read in the initial transform.
    _local.Load( reader );

    // read in the ambient animation name.
    tstring defaultAnimPath = reader.ReadString();
    if ( defaultAnimPath.length() > 0 )
        _defaultAnim = gGrKFAnimMgr->GetKFAnim( defaultAnimPath );

    // read in skins.
    UFastArray< URef< GrSkin > > skinArray;
    const unsigned int skinCount = reader.ReadInt();
    for ( unsigned int i = 0; i < skinCount; ++i )
        skinArray.Push( new GrSkin( reader ) );

    // read in meshes.
    UFastArray< URef< GrMesh > > meshArray;
    const unsigned int meshCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < meshCount; ++i )
        meshArray.Push( new GrMesh( reader, skinArray ) );

    // create the model's node tree.
    _root = new GrModelNode( this, reader, meshArray, majorVer, minorVer );

    // load in instanceable lights
    if ( majorVer == 1 && minorVer >= 1 )
    {
        tstring errors;
        unsigned int lightCount = reader.ReadUInt();
        for ( unsigned int i = 0; i < lightCount; ++i )
        {
            // get the light's name.
            tstring lightName = reader.ReadString();
            tstring parentName = reader.ReadString();

            // try to load the light.
            URef< GrLight > light = gGrLightMgr->GetLight( lightName, errors );

            // try to get the light's parent.
            GrModelNode* parent = 0;
            if ( parentName.length() > 0 )
                parent = _root->FindNode( parentName.c_str(), false );

            // add the light to the model.
            AddLight( light, parent );
        }
    }

    // if we have a default animation, then create an animation player
    // and start it playing the default animation.
    if ( _defaultAnim != 0 )
    {
        _animPlayer = new GrAnimPlayer( this );
        unsigned int id = _animPlayer->AddAnim( "default", _defaultAnim );
        _animPlayer->SetActiveAnim( id );
        _animPlayer->Play();
    }

    // create the skin controllers.
    CreateSkinControllers();
}

//----------------------------------------------------------
void
GrModel::CreateSkinControllers()
{
    // get the number of skin controllers.
    _skinControllerCount = GetSkinInstCount( _root );

    // create an array of skin controllers and initialize.
    if ( _skinControllerCount )
    {
        _skinControllers = new GrModelSkinController[ _skinControllerCount ];
        InitSkinControllers( _root, _skinControllers );
    }
}

//----------------------------------------------------------
unsigned int
GrModel::GetSkinInstCount( GrModelNode* root )
{
    unsigned int skinInstCount = 0;
    for ( unsigned int i = 0; i < root->GetMeshInstCount(); ++i )
    {
        // get the current mesh instance and increment the skin instance count.
        GrMeshInst* curMeshInst = root->GetMeshInst( i );
        if ( curMeshInst->GetSkinInst() != 0 )
            ++skinInstCount;
    }

    // get the number of skinned instances on children.
    for ( unsigned int i = 0; i < root->GetChildCount(); ++i )
        skinInstCount += GetSkinInstCount( root->GetChild( i ) );

    // return the number of skinned mesh instances.
    return skinInstCount;
}

//----------------------------------------------------------
unsigned int
GrModel::InitSkinControllers( GrModelNode* root, GrModelSkinController* controllers )
{
    // record the initial controller.
    GrModelSkinController* start = controllers;

    // initialize skin controllers for every skin instance we find.
    for ( unsigned int i = 0; i < root->GetMeshInstCount(); ++i )
    {
        // initialize the current skin controller if the current mesh has
        // a skin instance.
        GrMeshInst* curMeshInst = root->GetMeshInst( i );
        if ( curMeshInst->GetSkinInst() != 0 )
        {
            // initialize the current controller and advance to the next
            // controller.
            controllers->Init( curMeshInst->GetSkinInst(), _root );
            ++controllers;
        }
    }

    // get the number of skinned instances on children.
    for ( unsigned int i = 0; i < root->GetChildCount(); ++i )
        controllers += InitSkinControllers( root->GetChild( i ), controllers );

    // return the number of skinned mesh instances we've encountered for
    // the node hierarchy passed in.
    return ( unsigned int )( controllers - start );
}

//----------------------------------------------------------
void
GrModel::Optimize( GrModelNode* root )
{
    // optimize any meshes on the current node.
    root->Optimize();

    // recurse.
    for ( unsigned int i = 0; i < root->GetChildCount(); ++i )
        Optimize( root->GetChild( i ) );
}

//----------------------------------------------------------
void
GrModel::GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModelNode* root )
{
    // get all mesh instances from the node.
    unsigned int meshInstCount = root->GetMeshInstCount();
    for ( unsigned int i = 0; i < meshInstCount; ++i )
        meshInsts.Push( root->GetMeshInst( i ) );

    // recurse.
    unsigned int childCount = root->GetChildCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        GetMeshInsts( meshInsts, root->GetChild( i ) );
}

//----------------------------------------------------------
bool
GrModel::PickNode( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, GrModelNode* root, const MRay& ray )
{
    // pick against any meshes.
    bool meshHit = false;
    unsigned int meshCount = root->GetMeshInstCount();
    float distSqr = FLT_MAX;
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        // get the current mesh instance.
        GrMeshInst* curMeshInst = root->GetMeshInst( i );

        // pick against the current mesh instance.
        GrMesh::STriCollision curHit;
        if ( curMeshInst->Pick( curHit, ray ) )
        {
            // calculate the distance to the current intersection.
            float curDistSqr = curHit.hitLoc.DistSqr( ray.GetStart() );
            if ( curDistSqr < distSqr )
            {
                meshHit = true;
                meshInst = curMeshInst;
                distSqr = curDistSqr;
                hit = curHit;
            }
        }
    }

    // recurse children.
    unsigned int childCount = root->GetChildCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        // get the current child node.
        GrModelNode* curChild = root->GetChild( i );

        // pick against the current child.
        GrMeshInst* curMeshInst;
        GrMesh::STriCollision curHit;
        if ( PickNode( curMeshInst, curHit, curChild, ray ) )
        {
            // calculate the distance to the current intersection.
            float curDistSqr = curHit.hitLoc.DistSqr( ray.GetStart() );
            if ( curDistSqr < distSqr )
            {
                meshHit = true;
                meshInst = curMeshInst;
                distSqr = curDistSqr;
                hit = curHit;
            }
        }
    }

    // return true if we hit something.
    return meshHit;
}

//----------------------------------------------------------
void
GrModel::TraverseNode( GrSceneTraverser* traverser, GrModelNode* root )
{
    // traverse all meshes.
    unsigned int meshCount = root->GetMeshInstCount();
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        GrMeshInst* meshInst = root->GetMeshInst( i );
        unsigned int rangeCount = meshInst->GetRangeCount();
        for ( unsigned int r = 0; r < rangeCount; ++r )
            traverser->OnMeshInstRange( meshInst, r );
    }

    // recurse children.
    unsigned int childCount = root->GetChildCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        TraverseNode( traverser, root->GetChild( i ) );
}

//----------------------------------------------------------
void
GrModel::DbgDrawNode( GrModelNode* node )
{
    MVec3 pos = node->GetWorld().GetTranslate();
    if ( node->GetParent() )
    {
        MVec3 start = node->GetParent()->GetWorld().GetTranslate();
        bglVertex3f( start.GetX(), start.GetY(), start.GetZ() );
        bglVertex3f( pos.GetX(), pos.GetY(), pos.GetZ() );
    }
    else if ( _parentNode )
    {
        MVec3 start = _parentNode->GetWorld().GetTranslate();
        bglVertex3f( start.GetX(), start.GetY(), start.GetZ() );
        bglVertex3f( pos.GetX(), pos.GetY(), pos.GetZ() );
    }

    unsigned int i = node->GetChildCount();
    while ( i-- > 0 )
        DbgDrawNode( node->GetChild( i ) );

    i = _children.GetElemCount();
    while ( i-- > 0 )
        _children[ i ]->DbgDrawNode( _children[ i ]->_root );
}

//----------------------------------------------------------
void
GrModel::Clean()
{
    // ensure that we're no longer a child of our parent.
    if ( _parent != 0 )
        _parent->RemoveChild( this );

    // detach our children.
    RemoveChildren();

    // detach our lights.
    RemoveLights();

    // free the anim player if there is one.
    if ( _animPlayer )
        delete _animPlayer;

    // free the default animation if we own a clone.
    delete _defaultAnim;
    _defaultAnim = 0;

    // delete the skin controllers.
    delete[] _skinControllers;
    _skinControllers = 0;

    // delete our node tree.
    delete _root;
    _root = 0;
}
