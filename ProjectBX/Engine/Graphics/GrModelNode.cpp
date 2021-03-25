//----------------------------------------------------------
// File:		GrModelNode.cpp
// Author:		Kevin Bray
// Created:		04-02-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrModelNode.h"

// project includes.
#include "GrModel.h"
#include "GrMeshInst.h"
#include "GrPolygonNode.h"
#include "GrPolygonMesh.h"
#include "UWriter.h"
#include "UReader.h"
#include "GrFrustum.h"
#include "MSphere.h"
#include "GrScene.h"
#include "GrLight.h"
#include "GrLightMgr.h"
#include "GrRenderList.h"


//**********************************************************
// class GrModelNode
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrModelNode::GrModelNode( GrModel* owner, GrModelNode* parent, const MMat4x4& local, GrMeshInst* meshInst )
: _owner( owner )
, _parent( parent )
, _local( local )
{
	B_ASSERT( owner != 0 );
}

//----------------------------------------------------------
GrModelNode::GrModelNode( GrModel* owner, GrPolygonNode* fromTree )
: _owner( owner )
, _parent( 0 )
{
	B_ASSERT( owner != 0 );

	// generate a list of meshes.
	UFastArray< URef< GrPolygonMesh > > polyMeshes;
	fromTree->BuildMeshList( polyMeshes );

	// build database mapping poly meshes to renderable meshes.
	std::map< tstring, std::vector< GrMeshInst* > > meshes;
	for ( unsigned int i = 0; i < polyMeshes.GetElemCount(); ++i )
	{
		// get the poly mesh we need to generate renderable meshes from.
		URef< GrPolygonMesh > curPolyMesh = polyMeshes[ i ];

		// skip if the mesh was already added to our database.
		if ( meshes.find( curPolyMesh->GetName() ) != meshes.end() )
			continue;

		// add an entry to our database.
		std::vector< GrMeshInst* >& meshVec = meshes[ curPolyMesh->GetName() ];
		curPolyMesh->GenMeshes( meshVec );
	}

	// now build the node tree.
	InitFromPolyNode( owner, fromTree, meshes );

	// free up our meshes.
	std::map< tstring, std::vector< GrMeshInst* > >::iterator iter = meshes.begin();
	for ( ; iter != meshes.end(); ++iter )
	{
		for ( unsigned int i = 0; i < iter->second.size(); ++i )
			delete iter->second[ i ];
	}
}

//----------------------------------------------------------
GrModelNode::GrModelNode( GrModel* owner, GrPolygonNode* fromTree, const std::map< tstring, std::vector< GrMeshInst* > >& meshes )
: _owner( owner )
, _parent( 0 )
{
	InitFromPolyNode( owner, fromTree, meshes );
}

//----------------------------------------------------------
GrModelNode::GrModelNode( GrModel* owner, UReader& reader, const UFastArray< URef< GrMesh > >& meshes,
						  unsigned short majorVer, unsigned short minorVer )
: _owner( owner )
, _parent( 0 )
{
	Load( owner, reader, meshes, majorVer, minorVer );
}

//----------------------------------------------------------
GrModelNode::~GrModelNode()
{
	unsigned int childCount = _children.GetElemCount();
	for ( unsigned int i = 0; i < childCount; ++i )
		delete _children[ i ];

	unsigned int meshCount = _meshes.GetElemCount();
	for ( unsigned int i = 0; i < meshCount; ++i )
		delete _meshes[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrModelNode*
GrModelNode::Clone( GrModel* newOwner, bool deep, GrModelNode* parent )
{
	// create a new model node.
	GrModelNode* newNode = new GrModelNode( newOwner, parent, _local );
	newNode->_name = _name;

	// clone mesh instances.
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
		newNode->_meshes.Push( _meshes[ i ]->Clone( deep ) );

	// recurse for children.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		newNode->_children.Push( _children[ i ]->Clone( newOwner, deep, newNode ) );

	// return the newly created node.
	return newNode;
}

//----------------------------------------------------------
void
GrModelNode::SetLocal( const MMat4x4& local )
{
	// mark the owner as dirty.
	_owner->MarkAsDirty();
	_local = local;
}

//----------------------------------------------------------
GrModelNode*
GrModelNode::GetChild( unsigned int idx )
{
	B_ASSERT( idx < _children.GetElemCount() );
	return _children[ idx ];
}

//----------------------------------------------------------
GrModelNode*
GrModelNode::FindNode( const char* name, bool ignoreCase )
{
	if ( ignoreCase )
	{
		if ( _stricmp( _name.c_str(), name ) == 0 )
			return this;
	}
	else
	{
		if ( strcmp( _name.c_str(), name ) == 0 )
			return this;
	}

	// recurse.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
	{
		GrModelNode* found = _children[ i ]->FindNode( name, ignoreCase );
		if ( found )
			return found;
	}

	// not found!
	return 0;
}

//----------------------------------------------------------
GrMeshInst*
GrModelNode::GetMeshInst( unsigned int idx ) const
{
	B_ASSERT( idx < GetMeshInstCount() );
	return _meshes[ idx ];
}

//----------------------------------------------------------
void
GrModelNode::Update( const MMat4x4& parentMatrix )
{
	// calculate our new world matrix.
	_world = parentMatrix * _local;

	// propagate the transform to the mesh instances if necessary.
	unsigned int meshCount = _meshes.GetElemCount();
	for ( unsigned int i = 0; i < meshCount; ++i )
		_meshes[ i ]->SetTransform( _world );

	// update children.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->Update( _world );
}

//----------------------------------------------------------
void
GrModelNode::CalcBounds( MAABox& bounds )
{
	// recurse to children.
	unsigned int childCount = _children.GetElemCount();
	for ( unsigned int i = 0; i < childCount; ++i )
		_children[ i ]->CalcBounds( bounds );

	// evaluate mesh bounds.
	unsigned int meshCount = _meshes.GetElemCount();
	for ( unsigned int i = 0; i < meshCount; ++i )
		bounds.AddBoxToVolume( _meshes[ i ]->GetOBox() );
}

//----------------------------------------------------------
bool
GrModelNode::CalcBoundsExact( MAABox& bounds, bool boundsInit )
{
	// recurse to children.
	unsigned int childCount = _children.GetElemCount();
	for ( unsigned int i = 0; i < childCount; ++i )
		boundsInit = _children[ i ]->CalcBoundsExact( bounds, boundsInit );

	// evaluate mesh bounds.
	if ( boundsInit )
	{
		unsigned int meshCount = _meshes.GetElemCount();
		for ( unsigned int i = 0; i < meshCount; ++i )
			bounds.AddBoxToVolume( _meshes[ i ]->GetOBox() );
	}
	else
	{
		unsigned int meshCount = _meshes.GetElemCount();
		for ( unsigned int i = 0; i < meshCount; ++i )
		{
			if ( !boundsInit )
			{
				bounds = MAABox( _meshes[ i ]->GetOBox() );
				boundsInit = true;
			}
			else
				bounds.AddBoxToVolume( _meshes[ i ]->GetOBox() );
		}
	}

	return boundsInit;
}

//----------------------------------------------------------
const
MMat4x4&
GrModelNode::GetWorld() const
{
	return _world;
}

//----------------------------------------------------------
void
GrModelNode::SetWorld( const MMat4x4& transform )
{
    // ensure that our owner has updated us.
    _owner->EnsureUpdated();

    // grab our parent's worldspace transform.
    MMat4x4 parentWorld;
    if ( _parent )
        parentWorld = _parent->GetWorld();
    else 
        parentWorld = _owner->GetWorld();

    // invert it.
    MMat4x4 parentWorldInv;
    bool success = parentWorld.Inverse( parentWorldInv );
    B_ASSERT( success );
    if ( success )
    {
        // transform the incoming matrix into localspace 
        // and store our new transforms.
        _local = parentWorldInv * transform;
        _world = transform;

        // mark the owner as dirty.
        _owner->MarkAsDirty();
    }
}	

//----------------------------------------------------------
void
GrModelNode::BuildMeshList( UFastArray< URef< GrMesh > >& meshes )
{
	// add any meshes to the mesh list that have not already been added.
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
	{
		unsigned int j = 0;
		for ( ; j < meshes.GetElemCount(); ++j )
		{
			if ( _meshes[ i ]->GetMesh() == meshes[ j ] )
				break;
		}
		if ( j == meshes.GetElemCount() )
		{
			meshes.Push( _meshes[ i ]->GetMesh() );
		}
	}

	// recurse.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->BuildMeshList( meshes );
}

//----------------------------------------------------------
void
GrModelNode::Optimize()
{
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
		_meshes[ i ]->GetMesh()->Optimize();
}

//----------------------------------------------------------
void
GrModelNode::Cache()
{
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
		_meshes[ i ]->Cache();

	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->Cache();
}

//----------------------------------------------------------
void
GrModelNode::Evict()
{
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
		_meshes[ i ]->Evict();

	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->Evict();
}

//----------------------------------------------------------
void
GrModelNode::Save( UWriter& writer ) const
{
	// save the name.
	writer.WriteString( _name );

	// save the local transform.
	_local.Save( writer );

	// save the light link.
//	writer.WriteString( _light ? _light->GetId().GetPathString() : "" );

	// save mesh links.
	writer.WriteInt( _meshes.GetElemCount() );
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
	{
		_meshes[ i ]->Save( writer );
	}

	// save children.
	writer.WriteInt( _children.GetElemCount() );
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
	{
		_children[ i ]->Save( writer );
	}
}

//----------------------------------------------------------
void
GrModelNode::GetRenderableObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters ) const
{
	for ( unsigned int i = 0; i < _meshes.GetElemCount(); ++i )
	{
		lightReceivers.AddMeshInst( _meshes[ i ], false );
		if ( shadowCasters )
			shadowCasters->AddMeshInst( _meshes[ i ], false );
	}

	// recurse.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->GetRenderableObjects( lightReceivers, shadowCasters );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrModelNode::InitFromPolyNode( GrModel* owner, GrPolygonNode* fromTree,
							   const std::map< tstring, std::vector< GrMeshInst* > >& meshes )
{
	// get our name.
	_name = fromTree->GetName();

	// store our transform.
	_local = fromTree->GetLocal();

	// add meshes.
	unsigned int meshCount = fromTree->GetMeshCount();
	for ( unsigned int i = 0; i < meshCount; ++i )
	{
		// get the poly mesh.
		URef< GrPolygonMesh > polyMesh = fromTree->GetMesh( i );
		const tstring& polyMeshName = polyMesh->GetName();

		// check to see if an entry for the poly mesh exists.
		std::map< tstring, std::vector< GrMeshInst* > >::const_iterator iter = meshes.find( polyMeshName );
		if ( iter != meshes.end() )
		{
			const std::vector< GrMeshInst* >& meshInstVec = iter->second;
			for ( unsigned int i = 0; i < meshInstVec.size(); ++i )
			{
				_meshes.Push( meshInstVec[ i ]->Clone( false ) );
			}
		}
	}

	// recurse for children.
	unsigned int childCount = fromTree->GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		// create the new child node.
		GrPolygonNode* curChild = fromTree->GetChild( i );
		GrModelNode* newChild = new GrModelNode( owner, curChild, meshes );

		// add the child to our list and set it's parent to this.
		_children.Push( newChild );
		newChild->_parent = this;
	}
}

//----------------------------------------------------------
void
GrModelNode::Load( GrModel* owner, UReader& reader, const UFastArray< URef< GrMesh > >& meshes,
				   unsigned short majorVer, unsigned short minorVer )
{
	// load the name.
	_name = reader.ReadString();

	// load the local transform.
	_local.Load( reader );

	// if we have an old version of the file, check for an attached
	// light.  Note that this simply gets ignored.
	if ( majorVer <= 1 && minorVer < 1 )
		reader.ReadString();

	// load mesh links.
	unsigned int meshInstCount = reader.ReadInt();
	for ( unsigned int i = 0; i < meshInstCount; ++i )
		_meshes.Push( new GrMeshInst( reader, meshes ) );

	// load children.
	unsigned int childCount = reader.ReadInt();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		GrModelNode* node = new GrModelNode( owner, reader, meshes, majorVer, minorVer );
		node->_parent = this;
		_children.Push( node );
	}
}
