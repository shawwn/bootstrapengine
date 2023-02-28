//----------------------------------------------------------
// File:		GrBSPSplit.h
// Author:		Kevin Bray
// Created:		11-06-06
//
// Purpose:		To represent a renderable BSP split.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MPlane.h"
#include "GrMesh.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrPolygonBSPSplit;
class GrPolygonUberUVMapper;
class GrSceneTraverser;
class GrBSPArea;
class GrMeshInst;
class GrMeshCombiner;
class UReader;
class UWriter;
class GrFrustum;
class MRay;

//**********************************************************
// class GrBSPSplit
//**********************************************************
class GrBSPSplit
{
public:
	typedef std::vector< GrMeshInst* > MeshInstVec;
	typedef std::vector< GrMeshCombiner* > CombinerArray;

	GrBSPSplit( UReader& reader );
	GrBSPSplit( CombinerArray& meshCombiners, const GrPolygonBSPSplit& polygonBSPSplit,
				const GrPolygonUberUVMapper& mapper );
	~GrBSPSplit();

	// return the bounds.
	const MAABox&	GetBounds() const			{	return _bounds;		}

	// gets the area that a given position lies in.
	GrBSPArea*		GetArea( const MVec3& pos );

	// gets all areas in the BSP tree.
	void			GetAreas( std::vector< GrBSPArea* >& areas );

	// returns true to indicate a ray-mesh interesection.
	bool			Pick( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshInsts, const MRay& ray );

	// traverses the scene.
	void			Traverse( GrSceneTraverser* traverser, const MeshInstVec& meshes );

	// load/save.
	void			Load( UReader& reader );
	void			Save( UWriter& writer );

private:
	bool			PickSide( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshInsts,
							  GrBSPSplit* split, GrBSPArea* area, const MRay& ray );
	void			CalcBounds();
	bool			MergeBounds( GrBSPSplit* split, GrBSPArea* area, bool merge );

	MAABox			_bounds;
	MPlane			_split;
	GrBSPSplit*		_front;
	GrBSPSplit*		_back;
	GrBSPArea*		_frontLeaf;
	GrBSPArea*		_backLeaf;
};
