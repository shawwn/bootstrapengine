//----------------------------------------------------------
// File:		EdImportDAEGeometries.h
// Author:		Kevin Bray
// Created:		11-27-06
//
// Purpose:		To import geometric objects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrPolygonGroup.h"

// std c++ includes.
#include <vector>
#include <map>

// forward declarations.
struct SVec3;
struct SVec2;
class GrPolygonGroup;
class FCDocument;
class FCDGeometryPolygons;
class FCDGeometrySource;

//**********************************************************
// class EdImportDAEGeometries
//**********************************************************
class EdImportDAEGeometries
{
public:
	typedef std::list< std::vector< unsigned int > > PolygonIndices;

	// list of polygons containing a certain material.
	struct SGeometryRange
	{
		tstring materialSemantic;
		GrPolygonGroup polygons;
		PolygonIndices positionIndices;

	};
	typedef std::vector< SGeometryRange* > GeometryRangeVec;

	// named geometry object with a vector of ranges.
	struct SGeometry
	{
		tstring name;
		GeometryRangeVec ranges;
	};

	EdImportDAEGeometries();
	~EdImportDAEGeometries();

	bool				ImportGeometries( FCDocument* doc );
	SGeometry*			FindGeometry( const tstring& daeID ) const;

private:
	// list of polygons containing a certain material.
	typedef std::map< tstring, SGeometry* > GeometryMap;

	bool				ReadPolygonFaces( GrPolygonGroup& group, PolygonIndices& indices, FCDGeometryPolygons* polygons );
	bool				GetPolygonInputFlags( unsigned int& polygonFlags, FCDGeometryPolygons* polygons );
	void				ReadVec3( SVec3& vec3, FCDGeometrySource* source, unsigned int elemIdx );
	void				ReadVec2( SVec2& vec2, FCDGeometrySource* source, unsigned int elemIdx );
	void				Clean();

	GeometryMap			_geometries;
};
