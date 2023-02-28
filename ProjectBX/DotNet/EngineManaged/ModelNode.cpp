//----------------------------------------------------------
// File:        ModelNode.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "ModelNode.h"

// project headers.
#include "Utils.h"
#include "MeshInst.h"
#include "Mesh.h"
#include "Model.h"
#include "Light.h"
#include "Matrix.h"
#include "AABox.h"

// engine headers.
#include "MMat3x3.h"
#include "GrLight.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrModelMgr.h"
#include "GrScene.h"

//**********************************************************
// ref class ModelNode
//**********************************************************

//----------------------------------------------------------
Bootstrap::ModelNode::ModelNode( GrModelNode* modelNode )
: SceneEntity( modelNode->GetLocal(), true )
, _modelNode( modelNode )
{
    B_ASSERT( modelNode != 0 );
}

//----------------------------------------------------------
Bootstrap::ModelNode::~ModelNode()
{
    this->!ModelNode();
}

//----------------------------------------------------------
Bootstrap::ModelNode::!ModelNode()
{
}

//----------------------------------------------------------
System::String^ 
Bootstrap::ModelNode::Name::get()
{
    return TStrToStr( _modelNode->GetName() );
}

//----------------------------------------------------------
Bootstrap::Model^ 
Bootstrap::ModelNode::Owner::get()
{
    // if there is an owner, return a reference to it.
    GrModel* owner = _modelNode->GetOwner();
    if ( owner )
        return gcnew Model( owner );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::ModelNode::Clone( Model^ newOwner, bool deep, ModelNode^ parent )
{
    // validate incoming data.
    B_ASSERT( newOwner != nullptr );
    if ( newOwner == nullptr )
        return nullptr;

    // try to acquire the parent.
    GrModelNode* parentNode = 0;
    if ( parent != nullptr )
        parentNode = parent->_modelNode;

    // perform the clone.
    GrModelNode* clonedNode = _modelNode->Clone( newOwner->_model, deep, parentNode );

    // if the clone was successful, return a reference to it.
    if ( clonedNode )
        return gcnew ModelNode( clonedNode );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::ModelNode::LocalTransform::get()
{
    return Matrix( _modelNode->GetLocal() );
}

//----------------------------------------------------------
void 
Bootstrap::ModelNode::LocalTransform::set( Matrix mat )
{
    _modelNode->SetLocal( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::ModelNode::WorldTransform::get()
{
    return Matrix( _modelNode->GetWorld() );
}

//----------------------------------------------------------
void 
Bootstrap::ModelNode::WorldTransform::set( Matrix mat )
{
    _modelNode->SetWorld( mat.ToMMat4x4() );
}

//----------------------------------------------------------
int 
Bootstrap::ModelNode::ChildCount::get()
{
    return ( int )_modelNode->GetChildCount();
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::ModelNode::GetChild( int idx )
{
    if ( ( unsigned int )idx > _modelNode->GetChildCount() )
        return nullptr;
    return gcnew ModelNode( _modelNode->GetChild( idx ) );
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::ModelNode::FindNode( System::String^ name, bool ignoreCase )
{
    // try to find the node.
    GrModelNode* found = _modelNode->FindNode( StrToTStr( name ).c_str(), ignoreCase );

    // if the find operation was successful, return a reference to it.
    if ( found )
        return gcnew ModelNode( found );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
Bootstrap::ModelNode^ 
Bootstrap::ModelNode::Parent::get()
{
    // try to get the parent.
    GrModelNode* parent = _modelNode->GetParent();

    // if there is a parent, return a reference to it.
    if ( parent )
        return gcnew ModelNode( parent );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
int 
Bootstrap::ModelNode::MeshInstCount::get()
{
    return ( int )_modelNode->GetMeshInstCount();
}

//----------------------------------------------------------
Bootstrap::MeshInst^ 
Bootstrap::ModelNode::GetMeshInst( int idx )
{
    if ( ( unsigned int )idx > _modelNode->GetMeshInstCount() )
        return nullptr;
    return MeshInst::Create( _modelNode->GetMeshInst( idx ) );
}

//----------------------------------------------------------
Bootstrap::AABox 
Bootstrap::ModelNode::Bounds::get()
{
    // compute and return the bounds that includes the origin.
    MAABox bounds;
    _modelNode->CalcBounds( bounds );
    return AABox( bounds );
}

//----------------------------------------------------------
Bootstrap::AABox 
Bootstrap::ModelNode::BoundsExact::get()
{
    // compute and return the bounds that includes the origin.
    MAABox bounds;
    _modelNode->CalcBoundsExact( bounds );
    return AABox( bounds );
}

//----------------------------------------------------------
array< Bootstrap::Mesh^ >^ 
Bootstrap::ModelNode::MeshList::get()
{ 
    // compute a list of all unique meshes in the node's hierarchy.
    UFastArray< URef< GrMesh > > meshes;
    _modelNode->BuildMeshList( meshes );

    // transform it into a .NET array.
    unsigned int count = meshes.GetElemCount();
    array< Mesh^ >^ result = gcnew array< Mesh^ >( count );
    for ( unsigned int i = 0; i < count; ++i )
    {
        // validate that there is actually a valid mesh.
        URef< GrMesh > mesh( meshes[ i ] );
        B_ASSERT( mesh );
        if ( mesh )
        {
            result[ i ] = gcnew Mesh( mesh );
        }
    }

    // return the array of .NET mesh references.
    return result;
}

//----------------------------------------------------------
void 
Bootstrap::ModelNode::Optimize()
{
    // optimize meshes on this node. 
    _modelNode->Optimize();
}

//----------------------------------------------------------
bool
Bootstrap::ModelNode::Equals( System::Object^ obj )
{
    if ( obj == nullptr )
        return false;

    if( obj->GetType() == ModelNode::GetType() )
    {
        ModelNode^ modelNode = (ModelNode^)obj;
        return (_modelNode == modelNode->_modelNode);
    }
    return false;
}

//----------------------------------------------------------
void 
Bootstrap::ModelNode::ApplyTransform( Matrix mat )
{
    // apply the new worldspace transform to the model node.
    _modelNode->SetWorld( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::ModelNode::GetTransform()
{
    // return the model node's worldspace transform.
    return Matrix( _modelNode->GetWorld() );
}