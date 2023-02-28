//----------------------------------------------------------
// File:		GrMeshCombiner.cpp
// Author:		Kevin Bray
// Created:		12-13-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMeshCombiner.h"

// project includes.
#include "GrMeshInst.h"
#include "GrMaterial.h"
#include "GrUberTexture.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

//**********************************************************
// class GrMeshCombiner
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMeshCombiner::GrMeshCombiner( unsigned int maxIndexCount, unsigned int maxVertexCount )
: _vertexCount( 0 )
, _indexCount( 0 )
, _maxVertexCount( maxVertexCount )
, _maxIndexCount( maxIndexCount )
{

}

//----------------------------------------------------------
GrMeshCombiner::~GrMeshCombiner()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrMeshCombiner::CanAddMesh( const GrMeshInst* meshInst )
{
	// it doesn't make any sense to batch meshes that aren't indexed.
	B_ASSERT( meshInst->GetMesh()->GetIndexData() );

	// if there are no meshes added, then any mesh can be added.
	if ( _meshInsts.size() == 0 )
		return true;

	// check the vertex count.
	if ( ( _vertexCount + meshInst->GetMesh()->GetVertexCount() ) > _maxVertexCount )
		return false;

	// check the index count.
	if ( ( _indexCount + meshInst->GetMesh()->GetIndexCount() ) > _maxIndexCount )
		return false;

	// check the uber-textures.
	if ( meshInst->GetMesh()->GetUberTexture() != _uberTexture )
		return false;

	// get our mesh components and the new mesh's components.
	unsigned int groupComponents = _meshInsts[ 0 ]->GetMesh()->GetVertexData()->GetUsedComponents();
	unsigned int meshComponents = meshInst->GetMesh()->GetVertexData()->GetUsedComponents();

	// return true if the mesh components match up.
	return meshComponents == groupComponents;
}

//----------------------------------------------------------
unsigned int
GrMeshCombiner::AddMesh( const GrMeshInst* meshInst )
{
	// ensure that a valid mesh was passed in.
	B_ASSERT( meshInst != 0 );

	// get the range that the new mesh will start on.
	unsigned int retVal = ( unsigned int )_ranges.size();

	// get the mesh we're working with.
	GrMesh* curMesh = meshInst->GetMesh();

	if ( _meshInsts.size() == 0 )
		_uberTexture = curMesh->GetUberTexture();

	// add all of the ranges to our list.
	for ( unsigned int i = 0; i < curMesh->GetRangeCount(); ++i )
	{
		// get the current range from the mesh.
		const GrMesh::SRange& curRange = curMesh->GetRange( i );

		// fill out the range info.
		GrMesh::SRange range = curRange;
		range.minIndex += _vertexCount;
		range.maxIndex += _vertexCount;
		range.start += _indexCount;

		// store material information.
		_rangeMaterials.push_back( meshInst->GetMaterial( i ) );

		// add the range to our vector of ranges.
		_ranges.push_back( range );
	}

#ifdef _DEBUG
	{
		// make sure the mesh passed in hasn't already been added!
		for ( unsigned int i = 0; i < _meshInsts.size(); ++i )
		{
			B_ASSERT( _meshInsts[ i ] != meshInst );
		}
	}
#endif

	// add the mesh to our list.
	_meshInsts.push_back( meshInst );
	_vertexCount += curMesh->GetVertexCount();
	_indexCount += curMesh->GetIndexCount();

	// make sure that all ranges have been accounted for.
	B_ASSERT( _rangeMaterials.size() == _ranges.size() );

	// return the range that the mesh starts on.
	return retVal;
}

//----------------------------------------------------------
GrMeshInst*
GrMeshCombiner::BuildFinalMesh( const tstring& name )
{
	// if there aren't any meshes to combine, simply return null.
	if ( !_meshInsts.size() )
		return 0;

	// get the vertex components that the meshes need.
	unsigned int vertexComponents = _meshInsts[ 0 ]->GetMesh()->GetVertexData()->GetUsedComponents();
/*
	// lets be a douche and overwrite the first range in the mesh so that it
	// encompasses the *entire mesh*.  This will let us verify the vertex/index
	// data is valid.
	_ranges.resize( 1 );
	GrMesh::SRange& firstRange = _ranges.front();
	firstRange.minIndex = 0;
	firstRange.maxIndex = _vertexCount-1;
	firstRange.start = 0;
	firstRange.count = _indexCount;
*/
	// allocate a new mesh.
	URef< GrMesh > mesh = new GrMesh( name, vertexComponents, _vertexCount, _indexCount, &_ranges.front(),
		( unsigned int )_ranges.size(), false, false );

	// get the vertex/index buffers that we need to copy to.
	GrMeshVB* dstVertexData = mesh->GetVertexData();
	GrMeshIB* dstIndexData = mesh->GetIndexData();

	// add all of the vertex data to the mesh.
	unsigned int meshInstCount = ( unsigned int )_meshInsts.size();
	unsigned int indexOffset = 0;
	unsigned int vertexOffset = 0;
	for ( unsigned int i = 0; i < meshInstCount; ++i )
	{
		// get the source vertex/index buffers.
		GrMeshVB* srcVertexData = _meshInsts[ i ]->GetMesh()->GetVertexData();
		GrMeshIB* srcIndexData = _meshInsts[ i ]->GetMesh()->GetIndexData();
		unsigned int srcVertexCount = srcVertexData->GetVertexCount();
		unsigned int srcIndexCount = srcIndexData->GetIndexCount();

		// copy vertex data.
		if ( dstVertexData->GetVertices() )
			MemCopy( dstVertexData->GetVertices() + vertexOffset, srcVertexData->GetVertices(), srcVertexCount * sizeof( SVec3 ) );

		if ( dstVertexData->GetNormals() )
			MemCopy( dstVertexData->GetNormals() + vertexOffset, srcVertexData->GetNormals(), srcVertexCount * sizeof( SVec3 ) );

		if ( dstVertexData->GetBiNormals() )
			MemCopy( dstVertexData->GetBiNormals() + vertexOffset, srcVertexData->GetBiNormals(), srcVertexCount * sizeof( SVec3 ) );

		if ( dstVertexData->GetTangents() )
			MemCopy( dstVertexData->GetTangents() + vertexOffset, srcVertexData->GetTangents(), srcVertexCount * sizeof( SVec3 ) );

		if ( dstVertexData->GetColors() )
			MemCopy( dstVertexData->GetColors() + vertexOffset, srcVertexData->GetColors(), srcVertexCount * sizeof( unsigned long ) );

		if ( dstVertexData->GetTexCoords() )
			MemCopy( dstVertexData->GetTexCoords() + vertexOffset, srcVertexData->GetTexCoords(), srcVertexCount * sizeof( SVec2 ) );

		// copy index data.
		for ( unsigned int j = 0; j < srcIndexCount; ++j )
		{
			unsigned int curIdx = srcIndexData->GetIndices()[ j ] & 0xFFFF;
			curIdx += vertexOffset;
			dstIndexData->GetIndices()[ j + indexOffset ] = ( unsigned short )curIdx;
		}

		// advance the write offsets.
		vertexOffset += srcVertexCount;
		indexOffset += srcIndexCount;
	}

#if 1
	// check that we got all ranges.
	{
		unsigned int indexCount = 0;
		unsigned int vertexCount = 0;
		unsigned int rangeCount = 0;
		for ( unsigned int i = 0; i < _meshInsts.size(); ++i )
		{
			vertexCount += _meshInsts[ i ]->GetMesh()->GetVertexCount();
			for ( unsigned int j = 0; j < _meshInsts[ i ]->GetRangeCount(); ++j )
			{
				B_ASSERT( _meshInsts[ i ]->GetMaterial( j ) == _rangeMaterials[ rangeCount ] );
				++rangeCount;
				indexCount += _meshInsts[ i ]->GetMesh()->GetRange( j ).count;
			}
		}
		B_ASSERT( rangeCount == _ranges.size() );
		B_ASSERT( vertexCount == _vertexCount );
		B_ASSERT( indexCount == _indexCount );
	}
#endif

	dstVertexData->MarkAsDirty( ~0 );
	dstIndexData->MarkAsDirty();
	mesh->VertexDataChanged();

	// make sure we've written every vertex!
	B_ASSERT( vertexOffset == _vertexCount );
	B_ASSERT( indexOffset == _indexCount );

	// create and return a new mesh instance.
	return new GrMeshInst( mesh, &_rangeMaterials.front(), MMat4x4() );
}
