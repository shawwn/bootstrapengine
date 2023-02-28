//----------------------------------------------------------
// File:        Model.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Model.h"

// project headers.
#include "Utils.h"
#include "MeshInst.h"
#include "ModelNode.h"
#include "Light.h"

// engine headers.
#include "UWriter.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "MMat3x3.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrModelMgr.h"
#include "GrLight.h"
#include "GrScene.h"
#include "EdUberTexBuilder.h"

// .NET namespaces.
using namespace System::Collections::Generic;

//**********************************************************
// ref class Model
//**********************************************************

//----------------------------------------------------------
Bootstrap::Model::Model( GrModel* model )
: SceneEntity( model->GetRootNode()->GetWorld(), true )
, _model( model )
{
    B_ASSERT( model != 0 );

    _model->IncRef();
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::Create( System::String^ filePath, System::String^ instName, bool clone )
{
    // if there is no active scene, fail.
    if( !gGrScene )
        return nullptr;

    // try to create the model.
    UPath uFilePath( StrToTStr( filePath ) );
    UPath uInstName( StrToTStr( instName ) );
    URef< GrModel > model = gGrModelMgr->GetModel( uFilePath, uInstName, clone, true );

    // force the model to be updated.
    model->Update( true );

    // if the model couldn't be created, fail.
    if( !model )
        return nullptr;

    return gcnew Model( model );
}

//----------------------------------------------------------
Bootstrap::Model::~Model()
{
    this->!Model();
}

//----------------------------------------------------------
Bootstrap::Model::!Model()
{
    _model->DecRef();
}

//----------------------------------------------------------
Bootstrap::AABox 
Bootstrap::Model::Bounds::get()
{
    return AABox( _model->GetBounds() );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Model::FileName::get()
{
    return TStrToStr( _model->GetFileName().GetPathString() );      
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Model::InstanceName::get()
{
    return TStrToStr( _model->GetInstName().GetPathString() );
}

//----------------------------------------------------------
bool 
Bootstrap::Model::Pickable::get()
{
    return _model->GetPickable(); 
}

//----------------------------------------------------------
void 
Bootstrap::Model::Pickable::set( bool value )
{
    _model->SetPickable( value );
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::Clone( System::String^ instName, bool deep )
{
    // try to clone the model (and possibly its hierarchy).
    GrModel* model = _model->Clone( StrToTStr( instName ), deep );

    // if the clone was successful, return a reference to the cloned model.
    if ( model )
        return gcnew Model( model );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::CloneFile( System::String^ fileName, System::String^ instName )
{
    // try to clone the model and the file it originated from.
    URef< GrModel > model = _model->CloneFile( StrToTStr( fileName ), StrToTStr( instName ) );

    // if the clone was successful, return a reference to the cloned model.
    if ( model )
        return gcnew Model( model );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
bool 
Bootstrap::Model::InSkybox::get()
{
    // walk the chain of parents.
    GrModel* parent = _model->GetParent();
    while ( parent != 0 )
    {
        // if the current parent is the skybox model, return true.
        if ( parent == gGrScene->GetSkyboxModel() )
            return true;

        // otherwise, move to the next parent.
        parent = parent->GetParent();
    }

    // no parent was the skybox model, so return false.
    return false;
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::Parent::get()
{
    // try to get the parent.
    GrModel* model = _model->GetParent();

    // if there is a model, return a reference to it.
    if ( model )
        return gcnew Model( model );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::Model::ParentNode::get()
{
    // try to get the parent node.
    GrModelNode* node = _model->GetParentNode();

    // if there is a node, return a reference to it.
    if ( node )
        return gcnew ModelNode( node );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Model::LocalTransform::get()
{
    // return the local transform of the model.
    return Matrix( _model->GetLocal() );
}

//----------------------------------------------------------
void 
Bootstrap::Model::LocalTransform::set( Matrix mat )
{
    _model->SetLocal( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Model::WorldTransform::get()
{
    // return the worldspace transform of the model.
    return Matrix( _model->GetWorld() );
}

//----------------------------------------------------------
void 
Bootstrap::Model::WorldTransform::set( Matrix mat )
{
    _model->SetWorld( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::Model::RootNode::get()
{
    // try to get the root node.
    GrModelNode* node = _model->GetRootNode();

    // if there is a node, return a reference to it.
    if ( node )
        return gcnew ModelNode( node );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
int 
Bootstrap::Model::ChildCount::get()
{
    return _model->GetChildCount();
}

//----------------------------------------------------------
int 
Bootstrap::Model::LightCount::get()
{
    return _model->GetLightCount();
}

//----------------------------------------------------------
void 
Bootstrap::Model::AddChild( Model^ model, ModelNode^ attachTo )
{
    // validate the incoming data.
    B_ASSERT( model != nullptr );
    if ( model == nullptr )
        return;

    // try to acquire 'attachTo'.
    GrModelNode* attachToNode = 0;
    if ( attachTo != nullptr )
        attachToNode = attachTo->_modelNode;

    // add the child.
    _model->AddChild( model->_model, attachToNode );
}

//----------------------------------------------------------
void
Bootstrap::Model::AddLight( Light^ light, ModelNode^ attachTo )
{
    B_ASSERT( light != nullptr );
    _model->AddLight( light->_light, ( attachTo != nullptr ) ? attachTo->_modelNode : 0 );
}

//----------------------------------------------------------
Bootstrap::Light^
Bootstrap::Model::GetLight( int idx )
{
    URef< GrLight > light = _model->GetLight( idx );

    if ( light )
        return gcnew Light( light );

    return nullptr;
}

//----------------------------------------------------------
Bootstrap::ModelNode^
Bootstrap::Model::GetLightParent( int idx )
{
    GrModelNode* parent = _model->GetLightParent( idx );

    if ( parent )
        return gcnew ModelNode( parent );

    return nullptr;
}

//----------------------------------------------------------
bool
Bootstrap::Model::RemoveLight( Light^ light )
{
    return _model->RemoveLight( light->_light );
}

//----------------------------------------------------------
void
Bootstrap::Model::RemoveLights()
{
    _model->RemoveLights();
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::GetChild( int idx )
{
    if ( idx >= ( int )_model->GetChildCount() )
        return nullptr;
    return gcnew Model( _model->GetChild( idx ) );
}

//----------------------------------------------------------
bool 
Bootstrap::Model::RemoveChild( Model^ model )
{
    // validate the incoming data.
    B_ASSERT( model != nullptr );
    if ( model == nullptr )
        return false;

    // remove the child.
    return _model->RemoveChild( model->_model );
}

//----------------------------------------------------------
void 
Bootstrap::Model::RemoveChildren()
{
    // remove all children.
    _model->RemoveChildren();
}

//----------------------------------------------------------
void 
Bootstrap::Model::AddToSkybox()
{
    if ( this->Parent != nullptr )
        this->Parent->RemoveChild( this );

    if ( gGrScene && !InSkybox )
        gGrScene->GetSkyboxModel()->AddChild( _model );     
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::Model::FindModelByInstName( System::String^ instName )
{
    // try to find the model.
    URef< GrModel > found = _model->FindModelByInstName( StrToTStr( instName ) );

    // if a model was found, return a reference to it.
    if ( found != nullptr )
        return gcnew Model( found );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
array< Bootstrap::Model^ >^ 
Bootstrap::Model::FindModelsByFileName( System::String^ fileName )
{
    // find all models that originated from the incoming filename;
    UFastArray< URef< GrModel > > models;
    _model->FindModelsByFileName( models, StrToTStr( fileName ) );

    // convert the list of models into a .NET array and return it.
    unsigned int count = models.GetElemCount();
    array< Model^ >^ result = gcnew array< Model^ >( count );
    for ( unsigned int i = 0; i < count; ++i )
    {
        // assert that the model is valid.
        GrModel* model = models[ i ];
        B_ASSERT( model );

        // create a .NET model wrapper and assign it to a slot in the array.
        result[ i ] = gcnew Model( model );
    }

    // return the array of models.
    return result;
}

//----------------------------------------------------------
array< Bootstrap::MeshInst^ >^ 
Bootstrap::Model::MeshInsts::get()
{ 
    // get all mesh instances from the model.
    UFastArray< GrMeshInst* > meshInsts;
    _model->GetMeshInsts( meshInsts );

    // add them to a .NET array and return it.
    unsigned int count = meshInsts.GetElemCount();
    array< MeshInst^ >^ result = gcnew array< MeshInst^ >( count );
    for( unsigned int i = 0; i < count; ++i )
    {
        GrMeshInst* inst = meshInsts[ i ];

        // assert that there is actually a meshInst.
        B_ASSERT( inst );

        // create a reference to that meshInst and assert that the meshInst 
        // reference was actually successfully created.
        MeshInst^ meshInst = MeshInst::Create( inst );
        B_ASSERT( meshInst != nullptr );

        // assign the meshIsnt.
        result[ i ] = meshInst;
    }
    return result;
}

//----------------------------------------------------------
void 
Bootstrap::Model::DbgDrawHierarchy()
{
    // visualize the hierarchy.
    _model->DbgDrawHierarchy();
}

//----------------------------------------------------------
void 
Bootstrap::Model::Save()
{
    // grab the save data.
    UWriter writer;
    _model->Save( writer );

    // write the file to disk.
    URef< RFile > file = gRFileMgr->GetFile( _model->GetFileName(), RFileMgr::kFlagWrite );
    file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
}

//----------------------------------------------------------
bool
Bootstrap::Model::Equals( System::Object^ obj )
{
    if ( obj == nullptr )
        return false;

    if( obj->GetType() == Model::GetType() )
    {
        Model^ model = (Model^)obj;
        return (_model == model->_model);
    }
    return false;
}

//----------------------------------------------------------
void 
Bootstrap::Model::ApplyTransform( Matrix mat )
{
    // apply the worldspace transform to the model.
    _model->SetWorld( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Model::GetTransform()
{
    // return the model's worldspace transform.
    return Matrix( _model->GetWorld() );
}