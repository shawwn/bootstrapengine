//----------------------------------------------------------
// File:		EdImportDAEGeometries.h
// Author:		Kevin Bray
// Created:		11-27-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdImportDAEGeometries.h"

// project includes.
#include "GrPolygon.h"
#include "GrPolygonGroup.h"

// FCollada includes.
#pragma warning ( disable : 4996 )
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsInput.h"
#include "FCDocument/FCDGeometrySource.h"
#include "FUtils/FUDAEEnum.h"
#pragma warning ( default : 4996 )

//**********************************************************
// class EdImportDAEGeometries
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportDAEGeometries::EdImportDAEGeometries()
{

}

//----------------------------------------------------------
EdImportDAEGeometries::~EdImportDAEGeometries()
{
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdImportDAEGeometries::ImportGeometries( FCDocument* doc )
{
	// make sure we start fresh.
	Clean();

	// get the geometry library.
	FCDGeometryLibrary* geomLib = doc->GetGeometryLibrary();
	unsigned int geomCount = ( unsigned int )geomLib->GetEntityCount();
	for ( unsigned int i = 0; i < geomCount; ++i )
	{
		// get the current geometry.
		FCDGeometry* geom = geomLib->GetEntity( i );
		FCDGeometryMesh* geomMesh = geom->GetMesh();

		// note that we'll only read geometry meshes.
		if ( geomMesh == 0 )
		{
			EdPrintF( "Warning: Geometry %s is not a mesh.  Object skipped.\n",
				geom->GetDaeId().c_str() );
			continue;
		}

		// create a new geometry object to hold the ranges.
		SGeometry* geometry = new SGeometry;
		geometry->name = geomMesh->GetDaeId();

		// read data from the current geometry and shove into a polygon mesh.
		size_t rangeCount = geomMesh->GetPolygonsCount();
		for ( size_t j = 0; j < rangeCount; ++j )
		{
			// get the current polygon range.
			FCDGeometryPolygons* polygons = geomMesh->GetPolygons( j );

			// create a new range object to store the polygons.
			SGeometryRange* range = new SGeometryRange;

			// fill out a new group for our polygon mesh.
			if ( !ReadPolygonFaces( range->polygons, range->positionIndices, polygons ) )
			{
				delete range;
				continue;
			}

			// generate a tangent space basis on the polygons we just loaded.
			range->polygons.GenTSBasis();

			// add a new named range to the polygon mesh.
			range->materialSemantic = polygons->GetMaterialSemantic();

			// store our range.
			geometry->ranges.push_back( range );
		}

		// store the geometry.
		_geometries[ geometry->name ] = geometry;
	}

	return true;
}

//----------------------------------------------------------
EdImportDAEGeometries::SGeometry*
EdImportDAEGeometries::FindGeometry( const tstring& daeID ) const
{
	// try to find the geometry and return NULL if we can't find it.
	GeometryMap::const_iterator iter = _geometries.find( daeID );
	if ( iter == _geometries.end() )
		return 0;

	// return the geometry we found.
	return iter->second;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
EdImportDAEGeometries::ReadPolygonFaces( GrPolygonGroup& group, PolygonIndices& positionIndices, FCDGeometryPolygons* polygons )
{
	fstring daeId = polygons->GetParent()->GetDaeId();

	// make sure the inputs of the polygons passed in are valid.
	unsigned int polygonFlags = 0;
	if ( !GetPolygonInputFlags( polygonFlags, polygons ) )
	{
		// print an error and return.
		EdPrintF( "Warning: a polygon range in object %s does not have valid vertex components and was skipped!\n",
			daeId.c_str() );
		return false;
	}

	// go through each polygon face.
	const unsigned int kMaxPolygonVertexCount = 1024;

	// place to store vertices for each polygon.
	SPolygonVertex tempVertices[ kMaxPolygonVertexCount ];

	// go through each face and read in the vertices.
	bool zeroLenNormals = false;
	size_t faceCount = polygons->GetFaceCount();
	unsigned int curVertex = 0;
	for ( size_t i = 0; i < faceCount; ++i )
	{
		// for each vertex in the current face.
		size_t faceVertexCount = polygons->GetFaceVertexCount( i );
		if ( faceVertexCount > kMaxPolygonVertexCount )
		{
			EdPrintF( "Warning: a polygon in object %s contains too many vertices and was skipped!\n",
				daeId.c_str() );
			continue;
		}

		positionIndices.push_back( std::vector< unsigned int >( faceVertexCount ) );
		std::vector< unsigned int >& polygonPositionIndices = positionIndices.back();
		for ( unsigned int j = 0; j < faceVertexCount; ++j, ++curVertex )
		{
			// read in each vertex component.
			unsigned int uvCount = 0;
			unsigned int inputCount = ( unsigned int )polygons->GetInputCount();
			for ( unsigned int c = 0; c < inputCount; ++c )
			{
				// get the current input.
				FCDGeometryPolygonsInput* curInput = polygons->GetInput( c );

				// get the semantic, the source stream, and the index into the source
				// so we can read in the data.
				unsigned int curIndex = curInput->GetIndices()[ curVertex ];
				FUDaeGeometryInput::Semantic semantic = curInput->GetSemantic();
				FCDGeometrySource* source = curInput->GetSource();

				// get the indices for the current source.
				if ( semantic == FUDaeGeometryInput::POSITION )
				{
					// store the original index per vertex.
					polygonPositionIndices[ j ] = curIndex;

					// read in the position.
					ReadVec3( tempVertices[ j ].position, source, curIndex );
				}
				else if ( semantic == FUDaeGeometryInput::NORMAL )
				{
					ReadVec3( tempVertices[ j ].normal, source, curIndex );
					if ( tempVertices[ j ].normal.MagSqr() >= 0.0001f )
					{
						tempVertices[ j ].normal.Normalize();
					}
					else if ( !zeroLenNormals )
					{
						EdPrintF( "Warning: a polygon in object %s contains one or more zero length normals!\n",
							daeId.c_str() );
						zeroLenNormals = true;
					}

				}
				else if ( semantic == FUDaeGeometryInput::TEXCOORD )
				{
					if ( uvCount == 0 )
						ReadVec2( tempVertices[ j ].uv, source, curIndex );
					++uvCount;
				}
			}
		}

		// create a new polygon.
		GrPolygon polygon( polygonFlags, tempVertices, ( unsigned int )faceVertexCount, 0 );

		// set the normals to face as a fallback for bad normals.
		if ( zeroLenNormals )
		{
			const MVec3& faceNormal = polygon.GetNormal();
			for ( unsigned int i = 0; i < faceVertexCount; ++i )
				polygon.GetVertex( i ).normal = faceNormal;
		}

		// add the new polygon to our group.
		group.AddPolygon( polygon );
	}

	// success!
	return true;
}

//----------------------------------------------------------
void
EdImportDAEGeometries::ReadVec3( SVec3& vec3, FCDGeometrySource* source, unsigned int elemIdx )
{
	unsigned int idx = elemIdx * source->GetStride();
	float* data = source->GetData();
	vec3.x = data[ idx + 0 ];
	vec3.y = data[ idx + 1 ];
	vec3.z = data[ idx + 2 ];
}

//----------------------------------------------------------
void
EdImportDAEGeometries::ReadVec2( SVec2& vec2, FCDGeometrySource* source, unsigned int elemIdx )
{
	unsigned int idx = elemIdx * source->GetStride();
	float* data = source->GetData();
	vec2.x = data[ idx + 0 ];
	vec2.y = data[ idx + 1 ];
}

//----------------------------------------------------------
bool
EdImportDAEGeometries::GetPolygonInputFlags( unsigned int& flags, FCDGeometryPolygons* polygons )
{
	// clear the flags.
	flags = 0;

	// go through each input and verify that it's size is valid.
	unsigned int uvSet = 0;
	unsigned int inputCount = ( unsigned int )polygons->GetInputCount();
	for ( unsigned int c = 0; c < inputCount; ++c )
	{
		FCDGeometryPolygonsInput* curInput = polygons->GetInput( c );
		FCDGeometrySource* source = curInput->GetSource();
		FUDaeGeometryInput::Semantic semantic = curInput->GetSemantic();

		// positions and normals must be at least 3D, texture coordinates must be at least 2D.
		if ( semantic == FUDaeGeometryInput::POSITION )
		{
			if ( source->GetStride() < 3 )
				return false;
		}
		else if ( semantic == FUDaeGeometryInput::NORMAL )
		{
			flags |= GrPolygon::kNormal;
			if ( source->GetStride() < 3 )
				return false;
		}
		else if ( semantic == FUDaeGeometryInput::TEXCOORD )
		{
			if ( uvSet == 0 )
			{
				flags |= GrPolygon::kUV;
				if ( source->GetStride() < 2 )
					return false;
			}
			else if ( uvSet == 1 )
			{
				flags |= GrPolygon::kUniqueUV;
				if ( source->GetStride() < 2 )
					return false;
			}
			++uvSet;
		}
	}

	// if we got through all of the inputs, then we know we're valid.
	return true;
}

//----------------------------------------------------------
void
EdImportDAEGeometries::Clean()
{
	// go through each geometry, delete it's ranges as well as the geometry
	// itself.
	GeometryMap::iterator iter = _geometries.begin();
	const GeometryMap::iterator end = _geometries.end();
	for ( ; iter != end; ++iter )
	{
		// get the current geometry.
		SGeometry* curGeom = iter->second;

		// free up the ranges in the current geometry.
		unsigned int rangeCount = ( unsigned int )curGeom->ranges.size();
		for ( unsigned int i = 0; i < rangeCount; ++i )
			delete curGeom->ranges[ i ];

		// now free up the current geometry.
		delete curGeom;
	}

	// clear out our geometries.
	_geometries.clear();
}
