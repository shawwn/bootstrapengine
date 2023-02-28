//----------------------------------------------------------
// File:		TGeomCompress.cpp
// Author:		Kevin Bray
// Created:		10-05-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "test_afx.h"

// file header.
#include "TGeomCompress.h"

// engine includes.
#include "GrScene.h"
#include "GrMaterialMgr.h"
#include "GrMeshInst.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrBSPTree.h"
#include "UFastArray.h"

// local function declarations.
static void GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModel* root );
static void GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModelNode* root );
static void GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrBSPTree* bspTree );
static void GetMeshes( UFastArray< GrMesh* >& meshes, UFastArray< GrMeshInst* >& meshInsts );
static bool MeshInstSortProc( GrMeshInst* lhs, GrMeshInst* rhs );

static void Compress( void* compressed, const void* bytes, unsigned int size );
static void Decompress( void* bytesOut, const void* compressed, unsigned int compressedSize );

class GrMeshData
{
public:
	GrMeshData( GrMesh* src );
	~GrMeshData() { Clean(); }

	unsigned int	GetCompressedSize() const			{	return _compressedSize;		}

	void			Decompress( GrMesh* dst );

private:
	void			Compress( GrMesh* src );
	void			Clean();

	unsigned int	_compressedSize;
	unsigned int*	_positions;		// 12
	unsigned char*	_normals;		// 4
	unsigned char*	_tangents;		// 4
	unsigned char*	_binormals;		// 4
	unsigned int*	_texcoords;		// 8
};

//**********************************************************
// Global Functions
//**********************************************************

//----------------------------------------------------------
void
T_CompressGeometryTest()
{
	// get all mesh instances from the scene.
	UFastArray< GrMeshInst* > meshInsts;
	GetMeshInsts( meshInsts, gGrScene->GetRootModel() );
	GetMeshInsts( meshInsts, gGrScene->GetBSPTree() );

	// get all meshes from the scene.
	UFastArray< GrMesh* > meshes;
	GetMeshes( meshes, meshInsts );

	UFastArray< GrMeshData* > meshDatas;

	// now compress geometry.
	unsigned int meshCount = meshes.GetElemCount();
	for ( unsigned int i = 0; i < meshCount; ++i )
	{
		// get the current mesh.
		GrMesh* mesh = meshes[ i ];

		// add the mesh data.
		meshDatas.Push( new GrMeshData( mesh ) );
	}

	// now decompress geometry.
	for ( unsigned int i = 0; i < meshCount; ++i )
	{
		// get the current mesh.
		GrMesh* mesh = meshes[ i ];

		// now decompress the mesh's data.
		GrMeshData* meshData = meshDatas[ i ];
		meshData->Decompress( mesh );
	}

	// general algorithm... partition triangles into groups based on
	// area, perhaps using an octree.

	// by simply recursing the octree, we can add triangles to the
	// compression stream in a localized way so that neighboring
	// triangles are likely to be close to eachother in the stream.

	// divide the compression stream into chunks of a specific size.
	// I'm currently guessing that 2048 or 4096 would provide the
	// best results, however this should be configurable.

	// Note that we should, at some point, take into account overdraw
	// optimizations.  We should re-order triangles within groups to
	// achieve maximum vertex cache coherence, as well as minimum
	// overdraw based on an analysis of the vertex data.

	// algorithm:
	//  - when adding vertices to compression stream, quantize all
	//	  vertex data to fixed point.
	//  - while vertices remaining
	//		- reserve space for a packet 'p'.
	//		- scan ahead until we hit a vertex whose position and UVs
	//		  cannot be quantized within an 8-bit value.
	//		- if number of compressable vertices is large enough to
	//		  be a win, compress data with 8-bit values.
	//			- check for other bit combinations.
	//			- choose best result.
	//		- each vertex should contain a small (1-bit?) header that
	//		  indicates whether or not there is a raw vertex
	//		  component or a back reference to a previous component.
	//		  ideally, this could potentially be packed into the
	//		  header as a series of bytes (1-bit reserved for each
	//		  vertex specified), or this would simply be somehow
	//		  encoded into the vertex itself.
	//		- potentially experiment with lossy normal encodings.  If
	//		  length is unaccounted for (assumed to be one), we could
	//		  potentially compress normals (and tangents and
	//		  binormals) beyond simple 8-bit quantization.  Note that
	//        there could be support for back referenced normals if
	//		  there is a bit reserved for each vertex component.

}


//**********************************************************
// File Local Functions
//**********************************************************

//----------------------------------------------------------
void
GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModel* root )
{
	// get the mesh instances from the nodes.
	GetMeshInsts( meshInsts, root->GetRootNode() );

	// recurse.
	unsigned int childCount = root->GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
		GetMeshInsts( meshInsts, root->GetChild( i ) );
}

//----------------------------------------------------------
void
GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrModelNode* root )
{
	// get the mesh instances from the current node.
	unsigned int meshInstCount = root->GetMeshInstCount();
	for ( unsigned int i = 0; i < meshInstCount; ++i )
		meshInsts.Push( root->GetMeshInst( i ) );

	// recurse.
	unsigned int childCount = root->GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
		GetMeshInsts( meshInsts, root->GetChild( i ) );
}

//----------------------------------------------------------
void
GetMeshInsts( UFastArray< GrMeshInst* >& meshInsts, GrBSPTree* bspTree )
{
	const GrBSPTree::MeshInstVec& bspMeshInsts = bspTree->GetMeshInsts();
	unsigned int meshInstCount = ( unsigned int )bspMeshInsts.size();
	for ( unsigned int i = 0; i < meshInstCount; ++i )
		meshInsts.Push( bspMeshInsts[ i ] );
}

//----------------------------------------------------------
void
GetMeshes( UFastArray< GrMesh* >& meshes, UFastArray< GrMeshInst* >& meshInsts )
{
	// simply return if there isn't anything to do.
	if ( meshInsts.GetElemCount() == 0 )
		return;

	// sort the mesh instances by their mesh pointer.
	meshInsts.Sort( MeshInstSortProc );

	// add first mesh occurances to the mesh list.  This will give
	// us all of the unique meshes in the scene.
	meshes.Push( meshInsts[ 0 ]->GetMesh() );
	unsigned int meshInstCount = meshInsts.GetElemCount();
	for ( unsigned int i = 0; i < meshInstCount; ++i )
	{
		if ( meshes.Last() != meshInsts[ i ]->GetMesh() )
			meshes.Push( meshInsts[ i ]->GetMesh() );
	}
}

//----------------------------------------------------------
bool
MeshInstSortProc( GrMeshInst* lhs, GrMeshInst* rhs )
{
	size_t left = ( size_t )( GrMesh* )lhs->GetMesh();
	size_t right = ( size_t )( GrMesh* )rhs->GetMesh();
	return left < right;
}


//**********************************************************
// class GrMeshData
//**********************************************************

//----------------------------------------------------------
GrMeshData::GrMeshData( GrMesh* src )
: _positions( 0 )
, _normals( 0 )
, _tangents( 0 )
, _binormals( 0 )
, _texcoords( 0 )
{
	Compress( src );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMeshData::Decompress( GrMesh* dst )
{
	// get the vertex data.
	URef< GrMeshVB > meshVB = dst->GetVertexData();
	SVec3* positions = meshVB->GetVertices();
	SVec3* normals = meshVB->GetNormals();
	SVec3* tangents = meshVB->GetTangents();
	SVec3* binormals = meshVB->GetBiNormals();
	SVec2* texcoords = meshVB->GetTexCoords();
	
	// iterate through the vertex data.
	unsigned int vertexCount = meshVB->GetVertexCount();

#define FIXED8_24_TO_FLOAT( x )		( ( float )( ( double )( x ) / 8388608.0 ) )
#define FIXED8_TO_FLOAT( x )		( ( float )( x ) / 128.0f )
/*
	// now expand the vertex data.
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		positions[ i ].x = FIXED8_24_TO_FLOAT( _positions[ 3 * i + 0 ] );
		positions[ i ].y = FIXED8_24_TO_FLOAT( _positions[ 3 * i + 1 ] );
		positions[ i ].z = FIXED8_24_TO_FLOAT( _positions[ 3 * i + 2 ] );

		normals[ i ].x = FIXED8_TO_FLOAT( _normals[ 3 * i + 0 ] );
		normals[ i ].y = FIXED8_TO_FLOAT( _normals[ 3 * i + 1 ] );
		normals[ i ].z = FIXED8_TO_FLOAT( _normals[ 3 * i + 2 ] );

		tangents[ i ].x = FIXED8_TO_FLOAT( _tangents[ 3 * i + 0 ] );
		tangents[ i ].y = FIXED8_TO_FLOAT( _tangents[ 3 * i + 1 ] );
		tangents[ i ].z = FIXED8_TO_FLOAT( _tangents[ 3 * i + 2 ] );

		binormals[ i ].x = FIXED8_TO_FLOAT( _binormals[ 3 * i + 0 ] );
		binormals[ i ].y = FIXED8_TO_FLOAT( _binormals[ 3 * i + 1 ] );
		binormals[ i ].z = FIXED8_TO_FLOAT( _binormals[ 3 * i + 2 ] );

		texcoords[ i ].x = FIXED8_24_TO_FLOAT( _texcoords[ 2 * i + 0 ] );
		texcoords[ i ].y = FIXED8_24_TO_FLOAT( _texcoords[ 2 * i + 1 ] );
	}
*/
	// force data to be resent to the card.
	meshVB->MarkAsDirty( GR_ATTRIB_MASK_ALL );
}

//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMeshData::Compress( GrMesh* src )
{
	// make sure nothing is currently allocated.
	Clean();

	// get the vertex components.
	URef< GrMeshVB > meshVB = src->GetVertexData();
	SVec3* positions = meshVB->GetVertices();
	SVec3* normals = meshVB->GetNormals();
	SVec3* tangents = meshVB->GetTangents();
	SVec3* binormals = meshVB->GetBiNormals();
	SVec2* texcoords = meshVB->GetTexCoords();

	// iterate through the vertex data.
	unsigned int vertexCount = meshVB->GetVertexCount();

	// allocate space for the vertex components.
	_positions = new unsigned int[ 3 * vertexCount ];
	_normals = new unsigned char[ 3 * vertexCount ];
	_tangents = new unsigned char[ 3 * vertexCount ];
	_binormals = new unsigned char[ 3 * vertexCount ];
	_texcoords = new unsigned int[ 2 * vertexCount ];

#define FLOAT_TO_FIXED8_24( x )		( ( int )( ( double )( x ) * 8388608.0 + 0.5 ) )
#define FLOAT_TO_FIXED8( x )		( ( char )( ( x ) * 128.0f ) )

	int testFixed = FLOAT_TO_FIXED8_24( -1.24f );
	float testFloat = FIXED8_24_TO_FLOAT( testFixed );

	// now quantize all components.
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		_positions[ 3 * i + 0 ] = FLOAT_TO_FIXED8_24( positions[ i ].x );
		_positions[ 3 * i + 1 ] = FLOAT_TO_FIXED8_24( positions[ i ].y );
		_positions[ 3 * i + 2 ] = FLOAT_TO_FIXED8_24( positions[ i ].z );

		_normals[ 3 * i + 0 ] = FLOAT_TO_FIXED8( normals[ i ].x );
		_normals[ 3 * i + 1 ] = FLOAT_TO_FIXED8( normals[ i ].y );
		_normals[ 3 * i + 2 ] = FLOAT_TO_FIXED8( normals[ i ].z );

		_tangents[ 3 * i + 0 ] = FLOAT_TO_FIXED8( tangents[ i ].x );
		_tangents[ 3 * i + 1 ] = FLOAT_TO_FIXED8( tangents[ i ].y );
		_tangents[ 3 * i + 2 ] = FLOAT_TO_FIXED8( tangents[ i ].z );

		_binormals[ 3 * i + 0 ] = FLOAT_TO_FIXED8( binormals[ i ].x );
		_binormals[ 3 * i + 1 ] = FLOAT_TO_FIXED8( binormals[ i ].y );
		_binormals[ 3 * i + 2 ] = FLOAT_TO_FIXED8( binormals[ i ].z );

		_texcoords[ 2 * i + 0 ] = FLOAT_TO_FIXED8_24( texcoords[ i ].x );
		_texcoords[ 2 * i + 1 ] = FLOAT_TO_FIXED8_24( texcoords[ i ].y );
	}
}

//----------------------------------------------------------
void
GrMeshData::Clean()
{
	delete[] ( unsigned int* )_positions;
	delete[] ( unsigned char* )_normals;
	delete[] ( unsigned char* )_tangents;
	delete[] ( unsigned char* )_binormals;
	delete[] ( unsigned int* )_texcoords;

	_positions = 0;
	_normals = 0;
	_tangents = 0;
	_binormals = 0;
	_texcoords = 0;
	_compressedSize = 0;
}
