//----------------------------------------------------------
// File:		EdSimplePolygon.h
// Author:		Kevin Bray
// Created:		08-18-08
//
// Purpose:		To represent a simple convex 3D polygon.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrTypes.h"

// forward declarations.
class EdSimplePolygonGroup;

//**********************************************************
// class EdSimplePolygon
//**********************************************************
class EdSimplePolygon
{
public:
	enum
	{
		E_MAX_VERTEX_COUNT = 3
	};

	EdSimplePolygon();
	EdSimplePolygon( EdSimplePolygonGroup* vertexPool, const unsigned int* vertexIndices,
					 unsigned int count );
	~EdSimplePolygon();

	// ray intersection info.
	struct SRayIntersectInfo
	{
		SVec3 hitLoc;
		float hitDist;
		float u;
		float v;
		unsigned int triIdx;
	};

	// sets the vertex data for the polygon.
	void						SetVertices( EdSimplePolygonGroup* vertexPool, const unsigned int* vertexIndices, unsigned int count );
	void						GenNormals();

	// vertex access.
	unsigned int				GetVertexCount() const			{	return _vertexCount;	}
	const SVec3&				GetVertex( unsigned int idx ) const;

	// vertex normal access.
	const SVec3&				GetVertexNormal( unsigned int idx ) const;

	// normal access.
	const SVec3&				GetFaceNormal() const			{	return _faceNormal;		}
	const SVec3&				GetCenter() const				{	return _center;			}

	// intersects the polygon with the ray.
	bool						Intersect( SRayIntersectInfo& info, const SVec3& rayStart,
										   const SVec3& rayDir, float maxDist ) const;

	// calculates the interpolated normal at the position specified.
	void						CalcIntersectNormal( SVec3& normal, const SRayIntersectInfo& info ) const;
	void						CalcIntersectNormalHQ( SVec3& normal, const SRayIntersectInfo& info ) const;

private:
	void						CalcFaceNormal();
	void						CalcCenter();

	EdSimplePolygonGroup*		_group;
	unsigned int				_vertexIndices[ E_MAX_VERTEX_COUNT ];
	unsigned int				_normalIndices[ E_MAX_VERTEX_COUNT ];
	SVec3						_faceNormal;
	SVec3						_center;
	unsigned int				_vertexCount;
};
