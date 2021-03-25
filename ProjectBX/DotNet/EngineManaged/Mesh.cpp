//----------------------------------------------------------
// File:		Mesh.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Mesh.h"

// project includes.
#include "Utils.h"
#include "MeshVB.h"
#include "MeshIB.h"

// engine includes.
#include "GrMesh.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

//-----------------------
Bootstrap::Mesh::Mesh( GrMesh* mesh )
	: _mesh( mesh )
{
	B_ASSERT( mesh != 0 );

	_mesh->IncRef();
}

//-----------------------
Bootstrap::Mesh::~Mesh()
{
	this->!Mesh();
}

//-----------------------
Bootstrap::Mesh::!Mesh()
{
	_mesh->DecRef();
}

//-----------------------
System::String^ 
Bootstrap::Mesh::Name::get()
{
	return TStrToStr( _mesh->GetName() );
}

//-----------------------
Bootstrap::MeshVB^ 
Bootstrap::Mesh::VertexBuffer::get()
{
	// cache the mesh's vertex buffer.
	if( _meshVB == nullptr )
		_meshVB = gcnew MeshVB( _mesh->GetVertexData() );

	return _meshVB;
}

//-----------------------
Bootstrap::MeshIB^ 
Bootstrap::Mesh::IndexBuffer::get()
{
	// cache the mesh's index buffer.
	if( _meshIB == nullptr )
		_meshIB = gcnew MeshIB( _mesh->GetIndexData() );

	return _meshIB;
}

//-----------------------
int 
Bootstrap::Mesh::VertexCount::get()
{
	return _mesh->GetVertexCount();
}

//-----------------------
int
Bootstrap::Mesh::IndexCount::get()
{
	return _mesh->GetIndexCount();
}

//-----------------------
int 
Bootstrap::Mesh::RangeCount::get()
{
	return _mesh->GetRangeCount();
}

//-----------------------
bool 
Bootstrap::Mesh::GetRange( [Out] RangeInfo% rangeInfo, int rangeIdx )
{
	// if the range index is invalid, fail.
	if( rangeIdx > RangeCount )
		return false;

	// get the GrMesh's SRange.
	const GrMesh::SRange& range = _mesh->GetRange( rangeIdx );

	// translate the SRange into a RangeInfo.
	rangeInfo.Name = "";
	if( range.name.length() > 0 )
		rangeInfo.Name = TStrToStr( range.name );
	rangeInfo.Start = range.start;
	rangeInfo.Count = range.count;
	rangeInfo.MinIndex = range.minIndex;
	rangeInfo.MaxIndex = range.maxIndex;
	rangeInfo.AlignedBoundingBox = AABox( range.aaBox );

	// return true to indicate success.
	return true;
}

//-----------------------
bool 
Bootstrap::Mesh::GetTriInfo( [Out] TriInfo% triInfo, int triIdx )
{
	// return false if the triangle index passed in isn't valid.
	if ( ( unsigned int )triIdx >= _mesh->GetTriCount() )
		return false;

	// clear the incoming triInfo.
	triInfo = TriInfo();

	// get the GrMesh's STriInfo.
	GrMesh::STriInfo meshTriInfo;
	_mesh->GetTriInfo( meshTriInfo, triIdx );

	// translate the STriInfo into a TriInfo.
	triInfo.VertIdx0 = meshTriInfo.vertIdx0;
	triInfo.VertIdx1 = meshTriInfo.vertIdx1;
	triInfo.VertIdx2 = meshTriInfo.vertIdx2;
	triInfo.Range = meshTriInfo.range;

	// return true to indicate success.
	return true;
}