//----------------------------------------------------------
// File:		GrPolygonNode.h
// Author:		Kevin Bray
// Created:		01-16-06
//
// Purpose:		To represent a hierarchy of transforms and polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrPolygonMesh.h"
#include "UFastArray.h"

// forward declarations.
class GrNode;

//**********************************************************
// class GrPolygonNode
//**********************************************************
class GrPolygonNode
{
public:
	GrPolygonNode( const tstring& name, MMat4x4 local );
	~GrPolygonNode();

	const tstring&			GetName() const						{	return _name;		}

	// transform management.
	void					SetLocal( const MMat4x4& local )	{	_local = local;		}
	const MMat4x4&			GetLocal() const					{	return _local;		}

	// child management.
	unsigned int			GetChildCount()	const				{	return _children.GetElemCount();	}
	GrPolygonNode*			GetChild( unsigned int idx );
	void					AddChild( GrPolygonNode* child );
	void					RemoveChild( unsigned int idx );
	bool					DeleteChildTree( const char* name, bool ignoreCase );

	// finds a node by it's name.
	GrPolygonNode*			FindNode( const char* name, bool ignoreCase );

	// polygon group management.
	unsigned int			GetMeshCount() const				{	return _meshes.GetElemCount();		}
	URef< GrPolygonMesh >	GetMesh( unsigned int idx );
	void					AddMesh( URef< GrPolygonMesh > mesh );
	void					RemoveMesh( unsigned int idx );

	// builds a list of all meshes used.  Each mesh will only show up once in the
	// resulting list.
	void					BuildMeshList( UFastArray< URef< GrPolygonMesh > >& meshes );

	// tree optimization.  Returns world space polygons in the group passed in.
	// All polygons returned in worldPolygons are removed from the true.  Returns
	// true if all polygons in the hierarchy were placed in worldPolygons.
	void					SetOptimizeEnable( bool enabled )	{	_optimize = enabled;}
	bool					GetOptimizeEnable() const			{	return _optimize;	}
	bool					Optimize( GrPolygonGroup& worldPolygons, const tstring& noOptimizePostfix );

	// removes unnecessary nodes from the tree.
	void					Prune( const tstring& noOptimizePostfix );

	// updates transforms and bounding volumes.
	void					Update( const MMat4x4& parentWorld = MMat4x4() );

private:
	void					PruneInternal( GrPolygonNode* root, const MMat4x4& transform, const tstring& noOptimizePostfix );
	bool					CanOptimize( const tstring& noOptimizePostfix );

	typedef UFastArray< GrPolygonNode* > ChildArray;
	typedef UFastArray< URef< GrPolygonMesh > > MeshArray;

	tstring					_name;
	MeshArray				_meshes;
	MMat4x4					_local;
	MMat4x4					_world;
	ChildArray				_children;
	bool					_optimize;
};
