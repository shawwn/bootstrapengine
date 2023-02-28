//----------------------------------------------------------
// File:		GrSkin.cpp
// Author:		Kevin Bray
// Created:		06-21-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSkin.h"

// project includes.
#include "GrMesh.h"
#include "MMat4x4.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

//**********************************************************
// class GrSkin
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSkin::GrSkin( UReader& reader )
: _name( "" )
, _bindMesh( 0 )
, _vertexWeights( 0 )
, _vertexBoneIds( 0 )
, _boneCount( 0 )
, _boneNames( 0 )
, _invBindMatrices( 0 )
{
	Load( reader );
}

//----------------------------------------------------------
GrSkin::GrSkin( const tstring& name, URef< GrMesh > bindMesh, const SVec3* vertexWeights, const unsigned int* vertexBoneIds,
			    const tstring* boneNames, const MMat4x4* invBindMatrices, unsigned int boneCount )
: _name( name )
, _bindMesh( bindMesh )
, _vertexWeights( new SVec3[ bindMesh->GetVertexData()->GetVertexCount() ] )
, _vertexBoneIds( new unsigned int[ bindMesh->GetVertexData()->GetVertexCount() ] )
, _boneCount( boneCount )
, _boneNames( new tstring[ boneCount ] )
, _invBindMatrices( new MMat4x4[ boneCount ] )
{
	// the mesh passed in must not contain skinning data.
	B_ASSERT( _bindMesh->GetSkin() == 0 );
	memcpy( _vertexWeights, vertexWeights, _bindMesh->GetVertexData()->GetVertexCount() * sizeof( SVec3 ) );
	memcpy( _vertexBoneIds, vertexBoneIds, _bindMesh->GetVertexData()->GetVertexCount() * sizeof( unsigned int ) );

	// copy bone names and inverse bind matrices.
	for ( unsigned int i = 0; i < boneCount; ++i )
	{
		_boneNames[ i ] = boneNames[ i ];
		_invBindMatrices[ i ] = invBindMatrices[ i ];
	}
}

//----------------------------------------------------------
GrSkin::~GrSkin()
{
	delete[] _boneNames;
	delete[] _invBindMatrices;
	delete[] _vertexWeights;
	delete[] _vertexBoneIds;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrSkin*
GrSkin::Clone( const tstring& name )
{
	// return a new skin.
	return new GrSkin( _name, _bindMesh, _vertexWeights, _vertexBoneIds, _boneNames, _invBindMatrices, _boneCount );
}

//----------------------------------------------------------
GrSkin*
GrSkin::CloneRange( const tstring& name, URef< GrMesh > bindMesh, unsigned int vertexStart, unsigned int vertexCount )
{
	// ensure that the range passed in is valid.
	B_ASSERT( ( vertexStart + vertexCount ) <= _bindMesh->GetVertexData()->GetVertexCount() );
	B_ASSERT( vertexStart < _bindMesh->GetVertexData()->GetVertexCount() );

	// there should be one skin element for every vertex in the
	// mesh passed in.
	B_ASSERT( vertexCount == bindMesh->GetVertexData()->GetVertexCount() );

	// return a new skin.
	return new GrSkin( _name, bindMesh, _vertexWeights + vertexStart, _vertexBoneIds + vertexStart, _boneNames,
		_invBindMatrices, _boneCount );
}

//----------------------------------------------------------
unsigned int
GrSkin::GetBoneId( const char* name ) const
{
	// find the bone with the name specified.
	for ( unsigned int i = 0; i < _boneCount; ++i )
	{
		if ( strcmp( _boneNames[ i ].c_str(), name ) == 0 )
			return i;
	}

	// not found.
	return ~0;
}

//----------------------------------------------------------
const tstring&
GrSkin::GetBoneName( unsigned int id ) const
{
	B_ASSERT( id < GetBoneCount() );
	return _boneNames[ id ];
}

//----------------------------------------------------------
const MMat4x4&
GrSkin::GetInvBindMatrix( unsigned int id ) const
{
	B_ASSERT( id < _boneCount );
	return _invBindMatrices[ id ];
}

//----------------------------------------------------------
URef< GrMesh >
GrSkin::GetBindMesh() const
{
	return _bindMesh;
}

//----------------------------------------------------------
void
GrSkin::ReplaceVertexSkinData( SVec3* newWeights, unsigned int* newBoneIds )
{
	delete[] _vertexWeights;
	delete[] _vertexBoneIds;
	_vertexWeights = newWeights;
	_vertexBoneIds = newBoneIds;
}

//----------------------------------------------------------
void
GrSkin::RemapVertexSkinData( unsigned short* remapping, unsigned int newCount, unsigned int prevCount )
{
	SVec3* newVertexWeights = new SVec3[ newCount ];
	unsigned int* newVertexBoneIds = new unsigned int[ newCount ];

	// remap the vertices.
	for ( unsigned int i = 0; i < prevCount; ++i )
	{
		newVertexWeights[ remapping[ i ] ] = _vertexWeights[ i ];
		newVertexBoneIds[ remapping[ i ] ] = _vertexBoneIds[ i ];
	}

	delete[] _vertexWeights;
	delete[] _vertexBoneIds;
	_vertexWeights = newVertexWeights;
	_vertexBoneIds = newVertexBoneIds;
}

//----------------------------------------------------------
void
GrSkin::Save( UWriter& writer ) const
{
	// write out the scrubber.
	writer.WriteUInt( *( ( unsigned int* )"SKIN" ) );

	// write out the name.
	writer.WriteString( _name );

	// write out the bone data.
	writer.WriteUInt( _boneCount );
	for ( unsigned int i = 0; i < _boneCount; ++i )
	{
		writer.WriteString( _boneNames[ i ] );
		_invBindMatrices[ i ].Save( writer );
	}

	// save out the bind mesh.
	_bindMesh->Save( writer );

	// write out the vertex data.
	unsigned int vertexCount = _bindMesh->GetVertexData()->GetVertexCount();
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		writer.WriteUInt( _vertexBoneIds[ i ] );
		writer.WriteFloat( _vertexWeights[ i ].x );
		writer.WriteFloat( _vertexWeights[ i ].y );
		writer.WriteFloat( _vertexWeights[ i ].z );
	}
}

//----------------------------------------------------------
void
GrSkin::Load( UReader& reader )
{
	// cleanup any existing data.
	delete[] _vertexWeights;
	delete[] _vertexBoneIds;
	delete[] _boneNames;
	delete[] _invBindMatrices;

	// read in the scrubber.
	unsigned int magic = reader.ReadUInt();
	B_ASSERT( strncmp( ( ( const char* )&magic ), "SKIN", 4 ) == 0 );

	// read in the name.
	_name = reader.ReadString();

	// read in the bone data.
	_boneCount = reader.ReadUInt();
	_boneNames = new tstring[ _boneCount ];
	_invBindMatrices = new MMat4x4[ _boneCount ];
	for ( unsigned int i = 0; i < _boneCount; ++i )
	{
		_boneNames[ i ] = reader.ReadString();
		_invBindMatrices[ i ].Load( reader );
	}

	// read in the bind mesh.
	_bindMesh = new GrMesh( reader );

	// read in the vertex data.
	unsigned int vertexCount = _bindMesh->GetVertexData()->GetVertexCount();
	_vertexWeights = new SVec3[ vertexCount ];
	_vertexBoneIds = new unsigned int[ vertexCount ];
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		_vertexBoneIds[ i ] = reader.ReadUInt();
		_vertexWeights[ i ].x = reader.ReadFloat();
		_vertexWeights[ i ].y = reader.ReadFloat();
		_vertexWeights[ i ].z = reader.ReadFloat();
	}
}
