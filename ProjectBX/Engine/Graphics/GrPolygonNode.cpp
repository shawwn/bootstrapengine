//----------------------------------------------------------
// File:        GrPolygonNode.cpp
// Author:      Kevin Bray
// Created:     01-16-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonNode.h"

// project includes.
#include "MMat3x3.h"

//**********************************************************
// class GrPolygonNode
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonNode::GrPolygonNode( const tstring& name, MMat4x4 local )
: _name( name )
, _local( local )
, _optimize( true )
{

}

//----------------------------------------------------------
GrPolygonNode::~GrPolygonNode()
{
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        delete _children[ i ];
    }
    _children.Clear();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrPolygonNode*
GrPolygonNode::GetChild( unsigned int idx )
{
    B_ASSERT( idx < _children.GetElemCount() );
    return _children[ idx ];
}

//----------------------------------------------------------
void
GrPolygonNode::AddChild( GrPolygonNode* child )
{
    B_ASSERT( child != 0 );
    _children.Push( child );
}

//----------------------------------------------------------
void
GrPolygonNode::RemoveChild( unsigned int idx )
{
    B_ASSERT( idx < _children.GetElemCount() );
    _children.Erase( idx );
}

//----------------------------------------------------------
bool
GrPolygonNode::DeleteChildTree( const char* name, bool ignoreCase )
{
    int ( *strxcmp )( const char* str1, const char* str2 );
    strxcmp = ignoreCase ? _stricmp : strcmp;
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        // did we find the tree?
        if ( strxcmp( _children[ i ]->_name.c_str(), name ) == 0 )
        {
            // delete and remove the child.
            delete _children[ i ];
            RemoveChild( i );

            // found it... return true.
            return true;
        }
        else
        {
            // recurse.
            if ( _children[ i ]->DeleteChildTree( name, ignoreCase ) )
                return true;
        }
    }

    // didn't find it... return false.
    return false;
}

//----------------------------------------------------------
GrPolygonNode*
GrPolygonNode::FindNode( const char* name, bool ignoreCase )
{
    // are we the node being searched for?
    int ret = 0;
    if ( ignoreCase )
        ret = _stricmp( _name.c_str(), name );
    else
        ret = strcmp( _name.c_str(), name );
    if ( ret == 0 )
        return this;

    // recurse.
    for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
    {
        GrPolygonNode* node = _children[ i ]->FindNode( name, ignoreCase );
        if ( node )
            return node;
    }

    // not found.
    return 0;
}

//----------------------------------------------------------
URef< GrPolygonMesh >
GrPolygonNode::GetMesh( unsigned int idx )
{
    B_ASSERT( idx < _meshes.GetElemCount() );
    return _meshes[ idx ];
}

//----------------------------------------------------------
void
GrPolygonNode::AddMesh( URef< GrPolygonMesh > mesh )
{
    B_ASSERT( mesh != 0 );
    _meshes.Push( mesh );
}

//----------------------------------------------------------
void
GrPolygonNode::RemoveMesh( unsigned int idx )
{
    B_ASSERT( idx < _meshes.GetElemCount() );
    _meshes.Erase( idx );
}

//----------------------------------------------------------
void
GrPolygonNode::BuildMeshList( UFastArray< URef< GrPolygonMesh > >& meshes )
{
    // add meshes to the mesh list if they aren't already added.
    const unsigned int meshCount = _meshes.GetElemCount();
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        // check to see if the mesh is already in the list.
        const unsigned int meshListCount = meshes.GetElemCount();
        unsigned int j = 0;
        for ( ; j < meshListCount; ++j )
        {
            // check to see if the mesh is already added.  If so,
            // break.
            if ( meshes[ j ] == _meshes[ i ] )
                break;
        }

        // add the mesh if it isn't already in the list.
        if ( j == meshListCount )
        {
            meshes.Push( _meshes[ i ] );
        }
    }

    // recurse children.
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        _children[ i ]->BuildMeshList( meshes );
    }
}

//----------------------------------------------------------
bool
GrPolygonNode::Optimize( GrPolygonGroup& worldPolygons, const tstring& noOptimizePostfix )
{
    if ( !CanOptimize( noOptimizePostfix ) )
        return false;

    // go through children.  Do not recurse any children with the noOptimizePostfix.
    bool retVal = true;
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        retVal = _children[ i ]->Optimize( worldPolygons, noOptimizePostfix ) && retVal;
    }

    // optimize this node into worldPolygons.
    MMat3x3 normalRot;
    bool valid = _world.GetRotate().InverseTransposed( normalRot );
    B_ASSERT( valid );

    // optimize meshes.
    unsigned int meshCount = _meshes.GetElemCount();
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        worldPolygons.AddGroup( _meshes[ i ]->GetPolygons(), _world, normalRot );
    }
    _meshes.Clear();

    return retVal;
}

//----------------------------------------------------------
void
GrPolygonNode::Prune( const tstring& noOptimizePostfix )
{
    if ( !CanOptimize( noOptimizePostfix ) )
        return;

    // call prune internal on our children.
    unsigned int i = 0;
    unsigned int childCount = _children.GetElemCount();
    for ( i = 0; i < childCount; ++i )
    {
        if ( _children[ i ]->CanOptimize( noOptimizePostfix ) )
        {
            _children[ i ]->PruneInternal( this, MMat4x4(), noOptimizePostfix );
        }
    }

    // now remove and delete all nodes that do not specify the no optimize tag!
    i = 0;
    while ( i < _children.GetElemCount() )
    {
        if ( _children[ i ]->CanOptimize( noOptimizePostfix ) )
        {
            delete _children[ i ];
            _children.Erase( i );
            continue;
        }
        ++i;
    }
}
/*
//----------------------------------------------------------
GrNode*
GrPolygonNode::ConvertToScene()
{
    GrNode* node = 0;
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
    {
        GrNode* child = _children[ i ]->ConvertToScene();
        if ( child )
        {
            if ( !node )
                node = new GrNode( _name, _local );

            node->AddChild( child );
        }
    }

    unsigned int meshCount = _meshes.GetElemCount();
    if ( meshCount > 0 && !node )
        node = new GrNode( _name, _local );
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        std::vector< GrMeshInst* > meshes;
        _meshes[ i ]->GenMeshes( meshes );
        std::vector< GrMeshInst* >::iterator iter = meshes.begin();
        std::vector< GrMeshInst* >::iterator end = meshes.end();
        for ( ; iter != end; ++iter )
        {
            node->AddMeshInst( *iter );
        }
    }
    return node;
}
*/
//----------------------------------------------------------
void
GrPolygonNode::Update( const MMat4x4& parentWorld )
{
    // update our world matrix.
    _world = parentWorld * _local;

    // recurse.
    unsigned int childCount = _children.GetElemCount();
    for ( unsigned int i = 0; i < childCount; ++i )
        _children[ i ]->Update( _world );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonNode::PruneInternal( GrPolygonNode* root, const MMat4x4& transform, const tstring& noOptimizePostfix )
{
    // move any non optimizable nodes to the root node.
    B_ASSERT( root );
    B_ASSERT( CanOptimize( noOptimizePostfix ) );

    // calculate the current transform to go into root space.
    MMat4x4 curTransform = _local * transform;

    // recurse on children.
    unsigned int i = 0;
    while ( i < _children.GetElemCount() )
    {
        if ( !_children[ i ]->CanOptimize( noOptimizePostfix ) )
        {
            root->AddChild( _children[ i ] );
            _children[ i ]->SetLocal( _children[ i ]->GetLocal() * curTransform );
            _children.Erase( i );
            continue;
        }

        _children[ i ]->PruneInternal( root, curTransform, noOptimizePostfix );
        ++i;
    }
}

//----------------------------------------------------------
bool
GrPolygonNode::CanOptimize( const tstring& noOptimizePostfix )
{
    // check the optimize flag.
    if ( !_optimize )
        return false;

    // if we're skinned, we cannot optimize into world space.
    unsigned int meshCount = _meshes.GetElemCount();
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        if ( _meshes[ i ]->GetPolygons().IsSkinned() )
            return false;
    }

    // if we've met the above criteria, we must check the name for
    // the no-optimize postfix.  If no postfix was specified, then
    // everything is fair game.
    if ( noOptimizePostfix.length() == 0 )
        return true;

    // check the name to see if it contains a the noOptimizePostfix.
    unsigned int postfixLen = ( unsigned int )noOptimizePostfix.length();
    unsigned int nameLen = ( unsigned int )_name.length();
    return ( nameLen < postfixLen || strcmp( _name.c_str() + ( nameLen - postfixLen ), noOptimizePostfix.c_str() ) != 0 );
}
