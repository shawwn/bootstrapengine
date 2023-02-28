//----------------------------------------------------------
// File:		GrTSGen.cpp
// Author:		Kevin Bray
// Created:		10-30-04
// Copyright © 2004 Bootstrap Games
//----------------------------------------------------------
#include "engine_afx.h"

// class header.
#include "GrTSGen.h"

// project includes.
#include "MVec3.h"


//**********************************************************
// class GrTSGen
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTSGen::GrTSGen( const SVec3 *vertices, const SVec3 *normals, const SVec2 *uvs, unsigned int vertCount )
: _vertices( vertices )
, _normals( normals )
, _triangles( 0 )
, _uvs( uvs )
, _vertCount( vertCount )
, _triCount( vertCount / 3 )
, _faceTangents( 0 )
, _faceBinormals( 0 )
{
	B_ASSERT( vertCount % 3 == 0 );

	// create the triangle list.
	BuildTriList();
}

//----------------------------------------------------------
GrTSGen::~GrTSGen()
{
	delete[] _faceTangents;
	delete[] _faceBinormals;
	delete[] _triangles;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTSGen::GenTS( SVec3 *tangents, SVec3 *binormals )
{
	// make sure GenTS isn't called multiple times per mesh... this would
	// be horribly costly and inexcusably redundant.
	B_ASSERT( _faceTangents == 0 );
	B_ASSERT( _faceBinormals == 0 );

	// for each triangle face, generate the tangent space basis.
	_faceTangents = new MVec3[ _triCount ];
	_faceBinormals = new MVec3[ _triCount ];
	for ( unsigned int tri = 0; tri < _triCount; ++tri )
	{
		const MVec3 vert0 = _vertices[ _triangles[ tri ].index[ 0 ] ];
		const MVec3 vert1 = _vertices[ _triangles[ tri ].index[ 1 ] ];
		const MVec3 vert2 = _vertices[ _triangles[ tri ].index[ 2 ] ];
		const SVec2& uv0 = _uvs[ _triangles[ tri ].index[ 0 ] ];
		const SVec2& uv1 = _uvs[ _triangles[ tri ].index[ 1 ] ];
		const SVec2& uv2 = _uvs[ _triangles[ tri ].index[ 2 ] ];

		// figure out direction of S&T for the current triangle.
		MVec3 e0 = vert1 - vert0;
		MVec3 e1 = vert2 - vert0;

		// if we have an acceptable amount of area...
		if ( e0.Mag() > 0.00001f && e1.Mag() > 0.00001f && e0.Cross( e1 ).MagSqr() > 0.00001f )
		{
			float u0 = uv1.x - uv0.x;
			float u1 = uv2.x - uv0.x;
			float v0 = uv1.y - uv0.y;
			float v1 = uv2.y - uv0.y;

			MVec3 tangent = v1*e0 - v0*e1;	// u-tangent.
			MVec3 binormal = u0*e1 - u1*e0; // v-tangent.
			float scale = u0*v1 - u1*v0;
			if ( Abs( scale ) > 0.0001f )
			{
				tangent /= MVec3( scale, scale, scale );
				binormal /= MVec3( scale, scale, scale );
			}
			else
			{
				tangent = MVec3( 1.0f, 0.0f, 0.0f );
				binormal = MVec3( 0.0f, 0.0f, 1.0f );
			}

			_faceTangents[ tri ] = tangent.Normalized();
			_faceBinormals[ tri ] = binormal.Normalized();
		}
		else
		{
			// we have a degenerate triangle.
			_faceTangents[ tri ] = MVec3( 1.0f, 0.0f, 0.0f );
			_faceBinormals[ tri ] = MVec3( 0.0f, 0.0f, 1.0f );
		}
	}
	// now calculate the tangent and binormal for each vertex.  This
	// is similar to smoothing normals on a mesh.  Since the triangle
	// linking discriminates normals, we can simply smooth with
	// neighbors.
	for ( unsigned int tri = 0; tri < _triCount; ++tri )
	{
		// for each vertex...
		for ( unsigned int vert = 0; vert < 3; ++vert )
		{
			// try to find a triangle that has a similar vertex.
			unsigned int matchCount = 1;
			SVec3 curTangent = _faceTangents[ tri ];
			SVec3 curBinormal = _faceBinormals[ tri ];
			for ( unsigned int tri2 = 0; tri2 < _triCount; ++tri2 )
			{
				// skip this triangle.
				if ( tri == tri2 )
					continue;

				for ( unsigned int vert2 = 0; vert2 < 3; ++vert2 )
				{
					if ( _vertices[ _triangles[ tri ].index[ vert ] ] == _vertices[ _triangles[ tri2 ].index[ vert2 ] ] &&
						_normals[ _triangles[ tri ].index[ vert ] ] == _normals[ _triangles[ tri2 ].index[ vert2 ] ] &&
						_uvs[ _triangles[ tri ].index[ vert ] ] == _uvs[ _triangles[ tri2 ].index[ vert2 ] ] )
					{
						// found a match.
						curTangent += _faceTangents[ tri2 ];
						curBinormal += _faceBinormals[ tri2 ];
						++matchCount;
						break;
					}
				}
			}

			// record the averaged vectors.
			if ( curTangent.MagSqr() < 0.00001f )
			{
				// make something up... triangle is too small in UV space to do anything.
				curTangent = MVec3( 1.0f, 0.0f, 0.0f );
			}
			if ( curBinormal.MagSqr() < 0.00001f )
			{
				// make something up... triangle is too small in UV space to do anything.
				curBinormal = MVec3( 0.0f, 0.0f, 1.0f );
			}
			tangents[ _triangles[ tri ].index[ vert ] ] = MVec3( ( 1.0f / float( matchCount ) ) * curTangent ).Normalized();
			binormals[ _triangles[ tri ].index[ vert ] ] = MVec3( ( 1.0f / float( matchCount ) ) * curBinormal ).Normalized();
		}
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTSGen::BuildTriList()
{
	// build a list of all triangles...
	_triangles = new STriInfo[ _triCount ];	

	// for each triangle, record the current index.
	B_ASSERT( ( _triCount * 3 ) == _vertCount );
	for ( unsigned int i = 0; i < _triCount; ++i )
	{
		_triangles[ i ].index[ 0 ] = i * 3;
		_triangles[ i ].index[ 1 ] = i * 3 + 1;
		_triangles[ i ].index[ 2 ] = i * 3 + 2;
	}
}
