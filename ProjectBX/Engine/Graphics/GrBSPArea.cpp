//----------------------------------------------------------
// File:		GrBSPArea.cpp
// Author:		Kevin Bray
// Created:		11-06-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrBSPArea.h"

// project includes.
#include "GrPolygonBSPArea.h"
#include "GrMeshCombiner.h"
#include "GrMeshInst.h"
#include "GrUberPatchSet.h"
#include "GrSceneTraverser.h"
#include "UReader.h"
#include "UWriter.h"
#include "MRay.h"

//**********************************************************
// class GrBSPArea
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrBSPArea::GrBSPArea( UReader& reader )
: _meshInsts( 0 )
, _meshInstCount( 0 )
, _uberPatches( 0 )
{
	Load( reader );
}

//----------------------------------------------------------
GrBSPArea::GrBSPArea( CombinerArray& meshCombiners, const GrPolygonBSPArea& polygonBSPArea,
					  const GrPolygonUberUVMapper& uberMapper )
: _meshInsts( 0 )
, _meshInstCount( 0 )
, _uberPatches( 0 )
{
	// get the visibility data.
	const std::vector< unsigned int >* visibility;
	polygonBSPArea.GetVisibilityData( &visibility );
	_visibleAreas = *visibility;

	// get the bounding box.
	_boundingBox = polygonBSPArea.GetBoundingAABox();

	// get the ubertexture data (if any).
	GrPolygonUberPatchGroup* patchGroup = polygonBSPArea.GetUberTexPatchGroup();
	if ( patchGroup )
		_uberPatches = new GrUberPatchSet( patchGroup, uberMapper );

	// generate meshes.
	MeshInstVec localMeshInsts;
	polygonBSPArea.GetPolygons().GenMeshes( localMeshInsts, "bspleaf" );

	// now try to merge the meshes to other meshes if possible.
	BuildMeshInsts( meshCombiners, localMeshInsts );
}

//----------------------------------------------------------
GrBSPArea::~GrBSPArea()
{
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrBSPArea::SetMeshInstLastVisibleFrameID( unsigned int idx, unsigned int frameId )
{
	B_ASSERT( idx < _meshInstCount );
	_meshInsts[ idx ].lastVisibleFrameId = frameId;
}

//----------------------------------------------------------
const MAABox&
GrBSPArea::GetBoundingBox() const
{
	return _boundingBox;
}

//----------------------------------------------------------
bool
GrBSPArea::Pick( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MeshInstVec& meshes, const MRay& ray )
{
	// if we don't intersect with the bounds, then we don't intersect
	// any tris.
	if ( !ray.Intersect( _boundingBox ) )
		return false;

	// pick against all of the meshes we own.
	bool meshHit = false;
	float hitDistSqr = FLT_MAX;
	for ( unsigned int i = 0; i < _meshInstCount; ++i )
	{
		// get the current mesh instance.
		SMeshInstInfo& curMeshInfo = _meshInsts[ i ];

		// get the current mesh.
		GrMeshInst* curMeshInst = meshes[ curMeshInfo.meshInstIdx ];
		GrMesh* curMesh = curMeshInst->GetMesh();

		// pick each range.
		GrMesh::TriHitArray hits;
		unsigned int rangeEnd = curMeshInfo.rangeStart + curMeshInfo.rangeCount;
		for ( unsigned int range = curMeshInfo.rangeStart; range < rangeEnd; ++range )
		{
			// pick against the current range.
			curMesh->PickRange( hits, range, ray );

			// did we get any hits?
			unsigned int hitCount = hits.GetElemCount();
			for ( unsigned int h = 0; h < hitCount; ++h )
			{
				// get the current hit.
				GrMesh::STriCollision& curTriHit = hits[ h ];
				float curDistSqr = ray.GetStart().DistSqr( curTriHit.hitLoc );

				// is this the new closest hit?
				if ( curDistSqr < hitDistSqr )
				{
					meshHit = true;
					hitDistSqr = curDistSqr;
					hit = curTriHit;
					meshInst = curMeshInst;
				}
			}
		}
	}

	// return true if a mesh was hit.
	return meshHit;
}

//----------------------------------------------------------
void
GrBSPArea::Traverse( GrSceneTraverser* traverser, const MeshInstVec& meshes )
{
	// traverse each mesh instance range in the area.
	for ( unsigned int i = 0; i < _meshInstCount; ++i )
	{
		// get information about the current mesh instance.
		const SMeshInstInfo& curMeshInstInfo = _meshInsts[ i ];
		GrMeshInst* curMeshInst = meshes[ curMeshInstInfo.meshInstIdx ];

		// iterate through all mesh ranges.
		unsigned int range = curMeshInstInfo.rangeStart;
		unsigned int rangeEnd = range + curMeshInstInfo.rangeCount;
		for ( ; range < rangeEnd; ++range )
			traverser->OnMeshInstRange( curMeshInst, range );
	}
}

//----------------------------------------------------------
void
GrBSPArea::Load( UReader& reader )
{
	// version.
	short minorVer = reader.ReadShort();
	short majorVer = reader.ReadShort();

	// clear out the list of visible areas.
	Clean();

	// read in PVS info.
	unsigned int visibleAreaCount = reader.ReadUInt();
	for ( unsigned int i = 0; i < visibleAreaCount; ++i )
		_visibleAreas.push_back( reader.ReadInt() );

	// read in all mesh instance info.
	_meshInstCount = reader.ReadUInt();
	_meshInsts = new SMeshInstInfo[ _meshInstCount ];
	for ( unsigned int i = 0; i < _meshInstCount; ++i )
	{
		_meshInsts[ i ].meshInstIdx = reader.ReadUInt();
		_meshInsts[ i ].rangeStart = reader.ReadUInt();
		_meshInsts[ i ].rangeCount = reader.ReadUInt();
		_meshInsts[ i ].lastVisibleFrameId = 0;
	}

	// read in the ubertexture data.
	unsigned int uberPatchFlags = reader.ReadUInt();
	if ( uberPatchFlags & 1 )
		_uberPatches = new GrUberPatchSet( reader );

	// read in our bounding volume.
	_boundingBox.Load( reader );
}

//----------------------------------------------------------
void
GrBSPArea::Save( UWriter& writer )
{
	// version.
	writer.WriteShort( 0x0000 );
	writer.WriteShort( 0x0001 );

	// save out PVS info.
	unsigned int visibleAreaCount = ( unsigned int )_visibleAreas.size();
	writer.WriteUInt( visibleAreaCount );
	for ( unsigned int i = 0; i < visibleAreaCount; ++i )
		writer.WriteUInt( _visibleAreas[ i ] );

	// read in all mesh instances.
	writer.WriteUInt( _meshInstCount );
	for ( unsigned int i = 0; i < _meshInstCount; ++i )
	{
		writer.WriteUInt( _meshInsts[ i ].meshInstIdx );
		writer.WriteUInt( _meshInsts[ i ].rangeStart );
		writer.WriteUInt( _meshInsts[ i ].rangeCount );
	}

	// save any uber patch data if necessary.
	writer.WriteUInt( ( _uberPatches != 0 ) ? 1 : 0 );
	if ( _uberPatches )
		_uberPatches->Save( writer );

	// read in our bounding volume.
	_boundingBox.Save( writer );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrBSPArea::BuildMeshInsts( CombinerArray& meshCombiners, const MeshInstVec& localMeshInsts )
{
	// allocate room for each range we need to keep.
	unsigned int localMeshCount = ( unsigned int )localMeshInsts.size();
	_meshInsts = new SMeshInstInfo[ localMeshCount ];
	_meshInstCount = localMeshCount;

	// merge meshes.
	unsigned int i = 0;
	MeshInstVec::const_iterator iter = localMeshInsts.begin();
	const MeshInstVec::const_iterator end = localMeshInsts.end();
	for ( ; iter != end; ++iter, ++i )
	{
		// get the current mesh instance.
		GrMeshInst* curMeshInst = ( *iter );

		// look for a mesh combiner we can add to.
		GrMeshCombiner* combiner = 0;
		unsigned int meshIdx = 0;
		const unsigned int meshCombinerCount = ( unsigned int )meshCombiners.size();
		for ( ; meshIdx < meshCombinerCount; ++meshIdx )
		{
			// check to see if we can add the current mesh to the current
			// combiner.
			if ( meshCombiners[ meshIdx ]->CanAddMesh( curMeshInst ) )
			{
				// store the index of the combiner we need to be added to.
				combiner = meshCombiners[ meshIdx ];
				break;
			}
		}

		// if we couldn't find a combiner to add to, then we need to create
		// a new one.
		if ( meshIdx == meshCombinerCount )
		{
			// create a new mesh combiner.
			combiner = new GrMeshCombiner;
			meshCombiners.push_back( combiner );
		}

		// add the mesh to the mesh combiner.
		unsigned int startRange = combiner->AddMesh( curMeshInst );

		// record what mesh combiner we're in, since there will be one mesh
		// created for every mesh combiner.  Also record what range we'll
		// be in when the final mesh for the combiner is generated.
		_meshInsts[ i ].meshInstIdx = meshIdx;
		_meshInsts[ i ].rangeStart = startRange;
		_meshInsts[ i ].rangeCount = curMeshInst->GetRangeCount();
		_meshInsts[ i ].lastVisibleFrameId = 0;
	}
}

//----------------------------------------------------------
void
GrBSPArea::Clean()
{
	// cleanup.
	delete[] _meshInsts;
	_meshInsts = 0;
	_visibleAreas.clear();
	delete _uberPatches;
}
