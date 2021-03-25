//----------------------------------------------------------
// File:		GrBSPArea.h
// Author:		Kevin Bray
// Created:		11-06-06
//
// Purpose:		To represent a renderable BSP leaf.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MAABox.h"
#include "GrMesh.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrPolygonUberUVMapper;
class GrPolygonBSPArea;
class GrUberPatchSet;
class GrSceneTraverser;
class GrMeshInst;
class GrMeshCombiner;
class UReader;
class UWriter;

//**********************************************************
// class GrBSPArea
//**********************************************************
class GrBSPArea
{
public:
	typedef std::vector< unsigned int > IndexArray;
	typedef std::vector< GrMeshInst* > MeshInstVec;
	typedef std::vector< GrMeshCombiner* > CombinerArray;

	struct SMeshInstInfo
	{
		unsigned int meshInstIdx;
		unsigned int rangeStart;
		unsigned int rangeCount;
		unsigned int lastVisibleFrameId;
	};

	GrBSPArea( UReader& reader );
	GrBSPArea( CombinerArray& meshCombiners, const GrPolygonBSPArea& polygonBSPArea,
			   const GrPolygonUberUVMapper& uberMapper );
	~GrBSPArea();

	// set the last visible frame ID.
	void					SetLastVisibleFrameID( unsigned int id )	{	_lastVisibleFrameId = id;		}
	unsigned int			GetLastVisibleFrameID() const				{	return _lastVisibleFrameId;		}

	// adds all of the mesh instances to the renderer.
	unsigned int			GetMeshInstCount() const					{	return _meshInstCount;			}
	const SMeshInstInfo&	GetMeshInst( unsigned int idx ) const		{	return _meshInsts[ idx ];		}
	void					SetMeshInstLastVisibleFrameID( unsigned int idx, unsigned int frameId );

	// returns the bounding sphere of the area.
	const MAABox&			GetBoundingBox() const;

	// gets indices to the master list of visible areas.
	const IndexArray&		GetVisibleAreas() const						{	return _visibleAreas;			}

	// pick against the meshes in the BSP area.
	bool					Pick( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshes, const MRay& ray );
	void					Traverse( GrSceneTraverser* traverser, const MeshInstVec& meshes );

	// returns the uber-patch set for the BSP area.
	GrUberPatchSet*			GetUberPatchSet() const						{	return _uberPatches;			}

	// load/save.
	void					Load( UReader& reader );
	void					Save( UWriter& writer );

private:
	// private methods.
	void					BuildMeshInsts( CombinerArray& meshCombiners, const MeshInstVec& localMeshInsts );
	void					Clean();

	// areas potentially visible from within this area.
	IndexArray				_visibleAreas;

	// renderable mesh instances.
	unsigned int			_meshInstCount;
	SMeshInstInfo*			_meshInsts;

	// set of uberpatches.
	GrUberPatchSet*			_uberPatches;

	// bounding sphere for all of the meshes in the area.
	MAABox					_boundingBox;

	// last frame we're visible.
	unsigned int			_lastVisibleFrameId;
};
