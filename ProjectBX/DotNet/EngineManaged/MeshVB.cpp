//----------------------------------------------------------
// File:		MeshVB.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "MeshVB.h"

// project headers.
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"

// engine headers.
#include "GrTypes.h"
#include "GrMeshVB.h"

//**********************************************************
// ref class MeshVB
//**********************************************************

//----------------------------------------------------------
Bootstrap::MeshVB::MeshVB( GrMeshVB* meshVB )
	:  _meshVB( meshVB )
{
	B_ASSERT( _meshVB != 0 );

	_meshVB->IncRef();
}

//----------------------------------------------------------
Bootstrap::MeshVB::~MeshVB()
{
	this->!MeshVB();
}

//----------------------------------------------------------
Bootstrap::MeshVB::!MeshVB()
{
	_meshVB->DecRef();
}

//----------------------------------------------------------
int 
Bootstrap::MeshVB::VertexCount::get()
{
	return _meshVB->GetVertexCount();
}

//----------------------------------------------------------
Bootstrap::MeshComponents 
Bootstrap::MeshVB::UsedComponents::get()
{
	return (MeshComponents)_meshVB->GetUsedComponents();
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::MeshVB::Positions::get()
{
	if( (this->UsedComponents & MeshComponents::Positions) == MeshComponents::None )
		return nullptr;

	// cache the vertex positions.
	if( _positions == nullptr )
	{
		int count = _meshVB->GetVertexCount();
		_positions = gcnew array< Vector3 >( count );
		SVec3* pos = _meshVB->GetVertices();
		for( int i = 0; i < count; ++i, ++pos )
			_positions[i] = Vector3( *pos );
	}

	return _positions;
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::MeshVB::Normals::get()
{
	if( (this->UsedComponents & MeshComponents::Normals) == MeshComponents::None )
		return nullptr;

	// cache the vertex normals.
	if( _normals == nullptr )
	{
		int count = _meshVB->GetVertexCount();
		_normals = gcnew array< Vector3 >( count );
		SVec3* normal = _meshVB->GetNormals();
		for( int i = 0; i < count; ++i, ++normal )
			_normals[i] = Vector3( *normal );
	}

	return _normals;
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::MeshVB::BiNormals::get()
{
	if( (this->UsedComponents & MeshComponents::BiNormals) == MeshComponents::None )
		return nullptr;

	// cache the vertex binormals.
	if( _binormals == nullptr )
	{
		int count = _meshVB->GetVertexCount();
		_binormals = gcnew array< Vector3 >( count );
		SVec3* binormal = _meshVB->GetBiNormals();
		for( int i = 0; i < count; ++i, ++binormal )
			_binormals[i] = Vector3( *binormal );
	}

	return _binormals;
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::MeshVB::Tangents::get()
{
	if( (this->UsedComponents & MeshComponents::Tangents) == MeshComponents::None )
		return nullptr;

	// cache the vertex tangents.
	if( _tangents == nullptr )
	{
		int count = _meshVB->GetVertexCount();
		_tangents = gcnew array< Vector3 >( count );
		SVec3* tangent = _meshVB->GetTangents();
		for( int i = 0; i < count; ++i, ++tangent )
			_tangents[i] = Vector3( *tangent );
	}

	return _tangents;
}

//----------------------------------------------------------
array< UInt32 >^ 
Bootstrap::MeshVB::Colors::get()
{
	if( (this->UsedComponents & MeshComponents::Colors) == MeshComponents::None )
		return nullptr;

	// cache the vertex colors.
	if( _colors == nullptr )
	{
		int count = _meshVB->GetVertexCount();
		_colors = gcnew array< UInt32 >( count );
		unsigned long* color = _meshVB->GetColors();
		for( int i = 0; i < count; ++i, ++color )
			_colors[i] = *color;
	}

	return _colors;
}

//----------------------------------------------------------
array< Bootstrap::Vector2 >^ 
Bootstrap::MeshVB::TexCoords::get()
{
	if( (this->UsedComponents & MeshComponents::TexCoords) == MeshComponents::None )
		return nullptr;

	// cache the vertex texcoords.
	if( _texCoords == nullptr )
	{
		// cache the vertex texcoords.
		int count = _meshVB->GetVertexCount();
		_texCoords = gcnew array< Vector2 >( count );
		SVec2* texcoord = _meshVB->GetTexCoords();
		for( int i = 0; i < count; ++i, ++texcoord )
			_texCoords[i] = Vector2( *texcoord );
	}

	return _texCoords;
}