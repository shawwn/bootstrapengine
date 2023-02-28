//----------------------------------------------------------
// File:		GrMeshBufferMgr.cpp
// Author:		Kevin Bray
// Created:		10-13-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMeshBufferMgr.h"

// project includes.
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "GrTypes.h"

// singleton pointer.
GrMeshBufferMgr* gGrMeshBufferMgr = 0;


//**********************************************************
// class GrVertexBufferMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMeshBufferMgr::GrMeshBufferMgr()
{
	B_ASSERT( gGrMeshBufferMgr == 0 );
	gGrMeshBufferMgr = this;
}

//----------------------------------------------------------
GrMeshBufferMgr::~GrMeshBufferMgr()
{
	gGrMeshBufferMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrMeshVB >
GrMeshBufferMgr::CreateMeshVB( unsigned int vertexCount, unsigned int usedComponents, unsigned int flags )
{
	URef< GrMeshVB > meshVB = new GrMeshVB( vertexCount, usedComponents, flags );
	_meshVBs.Push( meshVB );
	return meshVB;
}

//----------------------------------------------------------
URef< GrMeshVB >
GrMeshBufferMgr::CreateMeshVB( unsigned int vertexCount, const SVec3* verts, const SVec3* normals, const SVec3* binormals,
							   const SVec3* tangents, const unsigned long* colors, const SVec2* uvs, unsigned int flags,
							   unsigned int maxVertexCount )
{
	// create the vertex buffer.
	unsigned int usedComponents = CalcComponents( verts, normals, tangents, binormals, colors, uvs );
	URef< GrMeshVB > meshVB = new GrMeshVB( vertexCount, verts, normals, binormals, tangents, colors, uvs,
		flags, maxVertexCount );
	_meshVBs.Push( meshVB );

	// return the new vertex buffer.
	return meshVB;
}

//----------------------------------------------------------
URef< GrMeshIB >
GrMeshBufferMgr::CreateMeshIB( unsigned int indexCount, unsigned int flags )
{
	URef< GrMeshIB > meshIB = new GrMeshIB( indexCount, flags );
	_meshIBs.Push( meshIB );
	return meshIB;
}

//----------------------------------------------------------
URef< GrMeshIB >
GrMeshBufferMgr::CreateMeshIB( unsigned int indexCount, const unsigned short* indices, unsigned int flags,
							   unsigned int maxIndexCount )
{
	URef< GrMeshIB > meshIB = new GrMeshIB( indexCount, indices, flags, maxIndexCount );
	_meshIBs.Push( meshIB );

	// return the new index buffer.
	return meshIB;
}

//----------------------------------------------------------
void
GrMeshBufferMgr::PreContextReset()
{
	// garbage collect/notify vertex buffers.
	unsigned int meshVBCount = _meshVBs.GetElemCount();
	unsigned int i = 0;
	while ( i < meshVBCount )
	{
		// collect garbage.
		if ( _meshVBs[ i ]->GetRef() == 1 )
		{
			_meshVBs.Erase( i );
			--meshVBCount;
			continue;
		}

		// the buffer needs to be reset.
		_meshVBs[ i ]->PreContextReset();
		++i;
	}

	// garbage collect/notify index buffers.
	unsigned int meshIBCount = _meshIBs.GetElemCount();
	i = 0;
	while ( i < meshIBCount )
	{
		// collect garbage.
		if ( _meshIBs[ i ]->GetRef() == 1 )
		{
			_meshIBs.Erase( i );
			--meshIBCount;
			continue;
		}

		// the buffer needs to be reset.
		_meshIBs[ i ]->PreContextReset();
		++i;
	}
}

//----------------------------------------------------------
void
GrMeshBufferMgr::PostContextReset()
{
	// notify vertex buffers of reset.
	unsigned int meshVBCount = _meshVBs.GetElemCount();
	for ( unsigned int i = 0; i < meshVBCount; ++i )
		_meshVBs[ i ]->PostContextReset();

	// notify index buffers of reset.
	unsigned int meshIBCount = _meshIBs.GetElemCount();
	for ( unsigned int i = 0; i < meshIBCount; ++i )
		_meshIBs[ i ]->PostContextReset();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrMeshBufferMgr::CalcComponents( const SVec3* verts, const SVec3* normals, const SVec3* tangents, const SVec3* binormals,
								 const unsigned long* colors, const SVec2* uvs )
{
	// calculate the vertex component flags from the params specified.
	unsigned int components = 0;
	if ( verts )
		components |= GR_ATTRIB_POSITION_MASK;
	if ( normals )
		components |= GR_ATTRIB_NORMAL_MASK;
	if ( tangents )
		components |= GR_ATTRIB_TANGENT_MASK;
	if ( binormals )
		components |= GR_ATTRIB_BINORMAL_MASK;
	if ( colors )
		components |= GR_ATTRIB_COLOR_MASK;
	if ( uvs )
		components |= GR_ATTRIB_TEXCOORD_MASK;

	// return the vertex components specified.
	return components;
}
