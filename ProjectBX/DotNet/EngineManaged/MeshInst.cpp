//----------------------------------------------------------
// File:        MeshInst.cpp
// Author:      Shawn Presser
// Created:     10-15-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "MeshInst.h"

// project includes.
#include "Utils.h"
#include "Mesh.h"
#include "Material.h"
#include "Sphere.h"
#include "OBox.h"

// engine includes.
#include "GrMeshInst.h"
#include "GrMaterial.h"

//----------------------------------------------------------
Bootstrap::MeshInst::MeshInst( GrMeshInst* meshInst )
    : SceneEntity( meshInst->GetTransform(), true )
    , _meshInst( meshInst )
{
    B_ASSERT( meshInst != 0 );

    // TODO: increment a GrMeshInst's ref count?
}

//----------------------------------------------------------
Bootstrap::MeshInst^
Bootstrap::MeshInst::Create( GrMeshInst* meshInst )
{
    if( _meshInstCache == nullptr )
        _meshInstCache = gcnew Dictionary< System::UInt64, MeshInst^ >();

    size_t key = (size_t)meshInst;

    // if a managed MeshInst that corresponds to this unmanaged MeshInst 
    // doesn't exist, cache it.
    if( !_meshInstCache->ContainsKey( key ) )
        _meshInstCache[ key ] = gcnew MeshInst( meshInst );

    return _meshInstCache[ key ];
}

//----------------------------------------------------------
Bootstrap::MeshInst::~MeshInst()
{
    this->!MeshInst();
}

//----------------------------------------------------------
Bootstrap::MeshInst::!MeshInst()
{
    // TODO: decrement a GrMeshInst's ref count?
}

//----------------------------------------------------------
Bootstrap::MeshInst^
Bootstrap::MeshInst::Clone( bool deep )
{
    // try to clone the meshInst.
    GrMeshInst* cloned = _meshInst->Clone( deep );

    // if the clone was successful, return a reference to it.
    if ( cloned )
        return gcnew MeshInst( cloned );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
void
Bootstrap::MeshInst::SetMesh( Bootstrap::Mesh^ mesh, IList< Material^ >^ materials )
{
    // validate the incoming data.
    B_ASSERT( mesh != nullptr );
    B_ASSERT( materials != nullptr );
    if ( mesh == nullptr || materials == nullptr )
        return;

    // transform the .NET materials array into a native array.
    UFastArray< URef< GrMaterial > > nativeMaterials;
    for each ( Material^ mat in materials )
    {
        B_ASSERT( mat );
        if ( mat )
        {
            nativeMaterials.Push( URef< GrMaterial >( mat->_material ) );
        }
    }

    // validate that the caller supplied enough materials.
    int matCount = (int)nativeMaterials.GetElemCount();
    B_ASSERT( matCount >= RangeCount );
    if ( matCount < RangeCount )
        return;

    // set the mesh instance data.
    _meshInst->SetMesh( mesh->_mesh, nativeMaterials.GetPtr() );
}

//----------------------------------------------------------
void
Bootstrap::MeshInst::SetMaterial( int range, Material^ material )
{
    // validate the incoming data.
    B_ASSERT( material != nullptr );
    B_ASSERT( range >= 0 && range < RangeCount );
    if ( material == nullptr || range < 0 || range >= RangeCount )
        return;

    // apply the material to the meshInst's range.
    _meshInst->SetMaterial( range, material->_material );
}

//----------------------------------------------------------
int
Bootstrap::MeshInst::RangeCount::get()
{
    return _meshInst->GetRangeCount(); 
}

//----------------------------------------------------------
Bootstrap::Mesh^
Bootstrap::MeshInst::Mesh::get()
{
    // validate the mesh.
    GrMesh* mesh = _meshInst->GetMesh();
    B_ASSERT( mesh );
    if ( !mesh )
        return nullptr;

    // return a mesh wrapper.
    return gcnew Bootstrap::Mesh( mesh );
}

//----------------------------------------------------------
Bootstrap::Material^
Bootstrap::MeshInst::GetMaterial( int rangeIdx )
{
    // validate the material.
    GrMaterial* material = _meshInst->GetMaterial( rangeIdx );
    B_ASSERT( material );
    if ( !material )
        return nullptr;

    // return a material wrapper.
    return gcnew Material( material );
}

//----------------------------------------------------------
Bootstrap::Matrix
Bootstrap::MeshInst::Transform::get()
{
    return Matrix( _meshInst->GetTransform() );
}

//----------------------------------------------------------
void
Bootstrap::MeshInst::Transform::set( Matrix transform )
{
    _meshInst->SetTransform( transform.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Sphere
Bootstrap::MeshInst::Sphere::get()
{
    return Bootstrap::Sphere( _meshInst->GetBoundingSphere() );
}

//----------------------------------------------------------
Bootstrap::OBox
Bootstrap::MeshInst::OBox::get()
{
    return Bootstrap::OBox( _meshInst->GetOBox() );
}

//----------------------------------------------------------
Bootstrap::Sphere
Bootstrap::MeshInst::GetRangeBoundingSphere( int rangeIdx )
{
    return Bootstrap::Sphere( _meshInst->GetRangeBoundingSphere( rangeIdx ) );
}

//----------------------------------------------------------
Bootstrap::OBox
Bootstrap::MeshInst::GetRangeOBox( int rangeIdx )
{
    return Bootstrap::OBox( _meshInst->GetRangeOBox( rangeIdx ) );
}

//----------------------------------------------------------
void
Bootstrap::MeshInst::RenderRange( int range, VertexComponents vertexComponents, unsigned int frameTime )
{
    _meshInst->RenderRange( range, (unsigned int)vertexComponents, frameTime );
}

//----------------------------------------------------------
bool
Bootstrap::MeshInst::Equals( System::Object^ obj )
{
    if ( obj == nullptr )
        return false;

    if( obj->GetType() == MeshInst::GetType() )
    {
        MeshInst^ meshInst = (MeshInst^)obj;
        return (_meshInst == meshInst->_meshInst);
    }
    return false;
}

//----------------------------------------------------------
int
Bootstrap::MeshInst::GetHashCode()
{
    // TODO-64: This may not work on 64-bit systems.
    return (int)_meshInst;
}

//----------------------------------------------------------
void
Bootstrap::MeshInst::ApplyTransform( Matrix mat )
{
    _meshInst->SetTransform( mat.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::Matrix
Bootstrap::MeshInst::GetTransform()
{
    return Matrix( _meshInst->GetTransform() );
}