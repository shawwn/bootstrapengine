//----------------------------------------------------------
// File:		EdSimplePolygonGroup.h
// Author:		Kevin Bray
// Created:		08-18-08
//
// Purpose:		To represent a group of simple polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>

// forward declarations.
class EdSimplePolygon;
struct SVec3;

//**********************************************************
// class EdSimplePolygonGroup
//**********************************************************
class EdSimplePolygonGroup
{
public:
	EdSimplePolygonGroup();
	~EdSimplePolygonGroup();

	// reserves memory for some number of polygons.
	void					ReservePolygons( unsigned int count );
	void					ReserveVertices( unsigned int count );
	void					ReserveNormals( unsigned int count );

	// adds a vertex to the group.
	void					AddVertex( const SVec3& vertex );
	void					AddNormal( const SVec3& normal );

	// add a polygon to the group.
	void					AddPolygon( const unsigned int* vertexIndices, unsigned int count );

	// polygon traversal.
	const EdSimplePolygon&	GetPolygon( unsigned int idx ) const;
	unsigned int			GetPolygonCount() const;

	// gets a vertex from the vertex pool.
	const SVec3&			GetVertex( unsigned int idx ) const;
	unsigned int			GetVertexCount() const;

	// normal indices.
	bool					HasVertexNormals() const;
	const SVec3&			GetVertexNormal( unsigned int idx ) const;
	void					SetVertexNormal( unsigned int idx, const SVec3& normal );
	unsigned int			GetVertexNormalCount() const;

	// generates vertex normals.
	void					GenVertexNormals();

private:
	typedef std::vector< EdSimplePolygon > PolygonVec;
	typedef std::vector< SVec3 > VertexVec;
	typedef std::vector< SVec3 > NormalVec;

	// polygon vector.
	PolygonVec				_polygons;
	VertexVec				_vertices;
	NormalVec				_normals;
};
