//----------------------------------------------------------
// File:		GrMeshBufferMgr.h
// Author:		Kevin Bray
// Created:		10-13-06
//
// Purpose:		To manage mesh buffers.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "UFastArray.h"

//**********************************************************
// class GrMeshBufferMgr
//**********************************************************
class GrMeshBufferMgr
{
public:
	GrMeshBufferMgr();
	~GrMeshBufferMgr();

	// see the GrMeshIB/GrMeshVB classes for valid flags.  For components, see
	// the GR_ATTRIB_xxx defines in GrConfig.h.
	URef< GrMeshVB >	CreateMeshVB( unsigned int vertexCount, unsigned int usedComponents, unsigned int flags = 0 );
	URef< GrMeshVB >	CreateMeshVB( unsigned int vertexCount, const SVec3* verts, const SVec3* normals, const SVec3* binormals,
									  const SVec3* tangents, const unsigned long* colors, const SVec2* uvs, unsigned int flags = 0,
									  unsigned int maxVertexCount = 0 );

	URef< GrMeshIB >	CreateMeshIB( unsigned int indexCount, unsigned int flags = 0 );
	URef< GrMeshIB >	CreateMeshIB( unsigned int indexCount, const unsigned short* indices, unsigned int flags = 0,
									  unsigned int maxIndexCount = 0 );

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

private:
	typedef UFastArray< URef< GrMeshVB > > MeshVBArray;
	typedef UFastArray< URef< GrMeshIB > > MeshIBArray;

	unsigned int		CalcComponents( const SVec3* verts, const SVec3* normals, const SVec3* tangents, const SVec3* binormals,
										const unsigned long* colors, const SVec2* uvs );

	MeshVBArray			_meshVBs;
	MeshIBArray			_meshIBs;
};
extern GrMeshBufferMgr* gGrMeshBufferMgr;
