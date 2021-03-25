//----------------------------------------------------------
// File:		EdSimplePolygonGroup.h
// Author:		Kevin Bray
// Created:		08-18-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdSimplePolygonGroup.h"

// project includes.
#include "EdSimplePolygon.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrScene.h"

//**********************************************************
// class EdSimplePolygonGroup
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdSimplePolygonGroup::EdSimplePolygonGroup()
{

}

//----------------------------------------------------------
EdSimplePolygonGroup::~EdSimplePolygonGroup()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdSimplePolygonGroup::ReservePolygons( unsigned int count )
{
	_polygons.reserve( count );
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::ReserveVertices( unsigned int count )
{
	_vertices.reserve( count );
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::ReserveNormals( unsigned int count )
{
	_normals.reserve( count );
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::AddVertex( const SVec3& vertex )
{
	_vertices.push_back( vertex );
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::AddNormal( const SVec3& normal )
{
	_vertices.push_back( normal );
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::AddPolygon( const unsigned int* vertexIndices, unsigned int count )
{
	_polygons.push_back( EdSimplePolygon() );
	_polygons.back().SetVertices( this, vertexIndices, count );
}

//----------------------------------------------------------
const EdSimplePolygon&
EdSimplePolygonGroup::GetPolygon( unsigned int idx ) const
{
	return _polygons[ idx ];
}

//----------------------------------------------------------
unsigned int
EdSimplePolygonGroup::GetPolygonCount() const
{
	return ( unsigned int )_polygons.size();
}

//----------------------------------------------------------
const SVec3&
EdSimplePolygonGroup::GetVertex( unsigned int idx ) const
{
	B_ASSERT( idx < _vertices.size() );
	return _vertices[ idx ];
}

//----------------------------------------------------------
unsigned int
EdSimplePolygonGroup::GetVertexCount() const
{
	return ( unsigned int )_vertices.size();
}

//----------------------------------------------------------
bool
EdSimplePolygonGroup::HasVertexNormals() const
{
	return ( _normals.size() > 0 );
}

//----------------------------------------------------------
const SVec3&
EdSimplePolygonGroup::GetVertexNormal( unsigned int idx ) const
{
	B_ASSERT( idx < _normals.size() );
	return _normals[ idx ];
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::SetVertexNormal( unsigned int idx, const SVec3& normal )
{
	B_ASSERT( idx < _normals.size() );
	_normals[ idx ] = normal;
}

//----------------------------------------------------------
unsigned int
EdSimplePolygonGroup::GetVertexNormalCount() const
{
	return ( unsigned int )_normals.size();
}

//----------------------------------------------------------
void
EdSimplePolygonGroup::GenVertexNormals()
{
	// generate the default vertex normals.
	_normals.resize( _vertices.size(), SVec3( 0.0f, 0.0f, 0.0f ) );

	// now iterate through all polygons and accumulate the face normal.
	unsigned int polygonCount = ( unsigned int )_polygons.size();
	for ( unsigned int i = 0; i < polygonCount; ++i )
	{
		// get the current polygon and generate normals.
		EdSimplePolygon& curPoly = _polygons[ i ];
		curPoly.GenNormals();
	}

	// now iterate through all normals and normalize them.
	unsigned int normalCount = GetVertexNormalCount();
	for ( unsigned int i = 0; i < normalCount; ++i )
		_normals[ i ].Normalize();
}
/*
//----------------------------------------------------------
void
EdSimplePolygonGroup::DebugRender( const GrCamera& camera )
{
	GrBindVB( 0 );
	GrBindIB( 0 );
	GrBindShader( 0 );
	GrClientActiveTexture( 0 );
	bglDisable( GL_TEXTURE_2D );
	bglBegin( GL_QUADS );
	{
		for ( unsigned int i = 0; i < _polygons.size(); ++i )
		{
			for ( unsigned int j = 0; j < 4; ++j )
			{
				const SVec3& vertex = _polygons[ i ].GetVertex( j );
				const SVec3& normal = _polygons[ i ].GetVertexNormal( j );
				bglColor4f( normal.z, normal.z, normal.z, 1.0f );
				bglVertex3fv( vertex.v );
			}
		}
	}
	bglEnd();
}
*/