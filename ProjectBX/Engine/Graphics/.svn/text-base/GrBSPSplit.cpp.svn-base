//----------------------------------------------------------
// File:		GrBSPSplit.cpp
// Author:		Kevin Bray
// Created:		11-06-06
//
// Purpose:		To represent a renderable BSP split.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrBSPSplit.h"

// project includes.
#include "GrBSPArea.h"
#include "GrSceneTraverser.h"
#include "GrPolygonBSPSplit.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrFrustum.h"
#include "MRay.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"


//**********************************************************
// class GrBSPSplit
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrBSPSplit::GrBSPSplit( UReader& reader )
: _front( 0 )
, _frontLeaf( 0 )
, _back( 0 )
, _backLeaf( 0 )
{
	Load( reader );
}

//----------------------------------------------------------
GrBSPSplit::GrBSPSplit( CombinerArray& meshCombiners, const GrPolygonBSPSplit& polygonBSPSplit,
					    const GrPolygonUberUVMapper& mapper )
: _front( 0 )
, _frontLeaf( 0 )
, _back( 0 )
, _backLeaf( 0 )
{
	// get our split.
	_split = polygonBSPSplit.GetPlane();

	// create front child/leaf.
	if ( polygonBSPSplit.GetFrontChild() )
		_front = new GrBSPSplit( meshCombiners, *polygonBSPSplit.GetFrontChild(), mapper );
	else if ( polygonBSPSplit.GetFrontLeaf() )
		_frontLeaf = new GrBSPArea( meshCombiners, *polygonBSPSplit.GetFrontLeaf(), mapper );

	// create back child/leaf.
	if ( polygonBSPSplit.GetBackChild() )
		_back = new GrBSPSplit( meshCombiners, *polygonBSPSplit.GetBackChild(), mapper );
	else if ( polygonBSPSplit.GetBackLeaf() )
		_backLeaf = new GrBSPArea( meshCombiners, *polygonBSPSplit.GetBackLeaf(), mapper );

	CalcBounds();
}

//----------------------------------------------------------
GrBSPSplit::~GrBSPSplit()
{
	delete _front;
	delete _frontLeaf;
	delete _back;
	delete _backLeaf;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrBSPArea*
GrBSPSplit::GetArea( const MVec3& pos )
{
	int side = _split.GetSide( pos, 0.0f );
	if ( side >= 0 )
	{
		if ( _front )
			return _front->GetArea( pos );
		else
			return _frontLeaf;
	}
	else
	{
		if ( _back )
			return _back->GetArea( pos );
		else
			return _backLeaf;
	}
}

//----------------------------------------------------------
void
GrBSPSplit::GetAreas( std::vector< GrBSPArea* >& areas )
{
	if ( _front )
		_front->GetAreas( areas );
	else if ( _frontLeaf )
		areas.push_back( _frontLeaf );

	if ( _back )
		_back->GetAreas( areas );
	else if ( _backLeaf )
		areas.push_back( _backLeaf );
}

//----------------------------------------------------------
bool
GrBSPSplit::Pick( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshInsts, const MRay& ray )
{
	// does the ray intersect with our bounds?
	if ( !ray.Intersect( _bounds ) )
		return false;

	// determine which side to recurse to first.
	int side = _split.GetSide( ray.GetStart(), GR_BSP_PLANE_EPSILON );
	if ( side >= 0 )
	{
		// pick front, then back.
		if ( PickSide( meshInst, hit, meshInsts, _front, _frontLeaf, ray ) )
			return true;

		if ( PickSide( meshInst, hit, meshInsts, _back, _backLeaf, ray ) )
			return true;
	}
	else
	{
		// pick back, then front.
		if ( PickSide( meshInst, hit, meshInsts, _back, _backLeaf, ray ) )
			return true;

		if ( PickSide( meshInst, hit, meshInsts, _front, _frontLeaf, ray ) )
			return true;
	}

	// we didn't hit anything!
	return false;
}

//----------------------------------------------------------
void
GrBSPSplit::Traverse( GrSceneTraverser* traverser, const MeshInstVec& meshes )
{
	// check to see if we should continue recursion.
	if ( !traverser->OnNode( _bounds ) )
		return;

	// recurse the front if necessary.
	if ( _front )
		_front->Traverse( traverser, meshes );
	else if ( _frontLeaf )
		_frontLeaf->Traverse( traverser, meshes );

	// recurse the back if necessary.
	if ( _back )
		_back->Traverse( traverser, meshes );
	else if ( _backLeaf )
		_backLeaf->Traverse( traverser, meshes );
}

//----------------------------------------------------------
void
GrBSPSplit::Load( UReader& reader )
{
	_split.Load( reader );

	// get the flags and read in what follows it.
	unsigned int flags = reader.ReadUInt();
	if ( flags & 1 )
		_front = new GrBSPSplit( reader );
	if ( flags & 2 )
		_frontLeaf = new GrBSPArea( reader );
	if ( flags & 4 )
		_back = new GrBSPSplit( reader );
	if ( flags & 8 )
		_backLeaf = new GrBSPArea( reader );

	// update our bounds.
	CalcBounds();
}

//----------------------------------------------------------
void
GrBSPSplit::Save( UWriter& writer )
{
	_split.Save( writer );

	// use flags to determine what we're saving.
	unsigned int flags = 0;
	flags |= _front		? 1 : 0;
	flags |= _frontLeaf ? 2 : 0;
	flags |= _back		? 4 : 0;
	flags |= _backLeaf	? 8 : 0;
	writer.WriteUInt( flags );

	// save children.
	if ( _front )
		_front->Save( writer );
	if ( _frontLeaf )
		_frontLeaf->Save( writer );
	if ( _back )
		_back->Save( writer );
	if ( _backLeaf )
		_backLeaf->Save( writer );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
GrBSPSplit::PickSide( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshInsts,
					  GrBSPSplit* split, GrBSPArea* area, const MRay& ray )
{
	// pick!
	if ( split )
	{
		if ( split->Pick( meshInst, hit, meshInsts, ray ) )
			return true;
	}
	else if ( area )
	{
		if ( area->Pick( meshInst, hit, meshInsts, ray ) )
			return true;
	}
	return false;
}

//----------------------------------------------------------
void
GrBSPSplit::CalcBounds()
{
	// calculate the bounds for the current split.
	bool merge = false;
	merge = MergeBounds( _front, _frontLeaf, merge );
	MergeBounds( _back, _backLeaf, merge );
}

//----------------------------------------------------------
bool
GrBSPSplit::MergeBounds( GrBSPSplit* split, GrBSPArea* area, bool merge )
{
	if ( split )
	{
		if ( merge )
			_bounds.AddBoxToVolume( split->GetBounds() );
		else
			_bounds = split->GetBounds();

		// return true to indicate that the bounds was modified.
		return true;
	}
	else if ( area )
	{
		if ( merge )
			_bounds.AddBoxToVolume( area->GetBoundingBox() );
		else
			_bounds = area->GetBoundingBox();

		// return true to indicate that the bounds was modified.
		return true;
	}

	// return false since we couldn't update the bounds.
	return false;
}
