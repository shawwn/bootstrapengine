//----------------------------------------------------------
// File:		GrTSGen.h
// Author:		Kevin Bray
// Created:		10-30-04
//
// Purpose:		To generate the tangent space bases for a mesh.
//
// Copyright © 2004 Bootstrap Games
//----------------------------------------------------------
#pragma once

#include "GrTypes.h"

struct SVec3;
struct SVec2;

//**********************************************************
// class GrTSGen
//**********************************************************
class GrTSGen
{
public:
	// note, ensure that the pointers passed in persists through the life of
	// this object.  We do not assume ownership of the pointers or the data.
	// Also, the vertices passed in must be in a triangle list format.  Therefor,
	// count must be a multiple of 3.
	GrTSGen( const SVec3 *vertices, const SVec3 *normals, const SVec2 *uvs, unsigned int vertCount );
	~GrTSGen();

	// the pointers passed in must be able to contain a tangent and binormal
	// for every vertex specified by 'count' in the constructor.
	void			GenTS( SVec3 *tangents, SVec3 *binormals );

private:
	// triangle info.
	struct STriInfo
	{
		unsigned short	index[ 3 ];
	};

	void			BuildTriList();

	const SVec3 *	_vertices;
	const SVec3 *	_normals;
	const SVec2 *	_uvs;
	STriInfo *		_triangles;
	MVec3 *			_faceTangents;
	MVec3 *			_faceBinormals;
	unsigned int	_vertCount;
	unsigned int	_triCount;
};
