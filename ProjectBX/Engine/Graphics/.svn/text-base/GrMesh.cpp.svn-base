//----------------------------------------------------------
// File:		GrMesh.cpp
// Author:		Kevin Bray
// Created:		10-04-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMesh.h"

// project headers.
#include "UReader.h"
#include "MVec3.h"
#include "MVec2.h"
#include "GrConfig.h"
#include "GLSubsys.h"
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "MVec3.h"
#include "MPlane.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrPolygonGroup.h"
#include "GrPolygon.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "GrSkin.h"
#include "GrUberTexture.h"
#include "MRay.h"
#include "GrTextureMgr.h"
#include "GrMeshBufferMgr.h"
#include "ULRUHash.h"

// external headers.
//#include "NVTriStrip/NVTriStrip.h"

// std c++ includes.
#include <vector>
#include <float.h>

#define EPSILON			1.0e-7

// use this to turn spin reporting on/off.
//#define _REPORT_OBJ_QUERY_SPIN
//#define _NO_RENDER

using std::vector;

//----------------------------------------------------------
MVec3 ComputeTriangleNormal( const MVec3& v0, const MVec3& v1, const MVec3& v2 )
{
	return (v1 - v0).Cross(v2 - v0).Normalized();
}


//**********************************************************
// class GrMesh
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMesh::GrMesh( UReader& reader )
: _name( "" )
, _rangeCount( 0 )
, _ranges( 0 )
, _aaBox( MVec3( 0.0f, 0.0f, 0.0f ), MVec3( 0.0f, 0.0f, 0.0f ) )
, _uberTexture( 0 )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	Load( reader, UFastArray< URef< GrSkin > >() );
}

//----------------------------------------------------------
GrMesh::GrMesh( UReader& reader, const UFastArray< URef< GrSkin > >& skins )
: _name( "" )
, _rangeCount( 0 )
, _ranges( 0 )
, _aaBox( MVec3( 0.0f, 0.0f, 0.0f ), MVec3( 0.0f, 0.0f, 0.0f ) )
, _uberTexture( 0 )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	Load( reader, skins );
}

//----------------------------------------------------------
GrMesh::GrMesh( const tstring& name,
				const SVec3 *verts,
				const SVec3 *normals,
				const SVec3 *binormals,
				const SVec3 *tangents,
				const unsigned long *colors,
				const SVec2 *uvs,
				unsigned long vertexCount,
				const SRange *ranges,
				unsigned int rangeCount,
				URef< GrUberTexture > uberTexture,
				unsigned int vbFlags )
: _name( name )
, _rangeCount( rangeCount )
, _ranges( new SRange[ rangeCount ] )
, _aaBox( MVec3( 0.0f, 0.0f, 0.0f ), MVec3( 0.0f, 0.0f, 0.0f ) )
, _uberTexture( uberTexture )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	// ensure we have a valid vertex count.
	B_ASSERT( vertexCount > 0 );
	B_ASSERT( rangeCount > 0 );

	// create the vertex buffer.
	_vertexBuffer = gGrMeshBufferMgr->CreateMeshVB( vertexCount, verts, normals, binormals, tangents, colors, uvs, vbFlags );

	// copy ranges.
	for ( unsigned int i = 0; i < _rangeCount; ++i )
		_ranges[ i ] = ranges[ i ];

	// calc bounds.
	CalcAABox();
	CalcBoundingSphere();
}

//----------------------------------------------------------
GrMesh::GrMesh( const tstring& name,
				const SVec3 *verts,
				const SVec3 *normals,
				const SVec3 *binormals,
				const SVec3 *tangents,
				const unsigned long *colors,
				const SVec2 *uvs,
				unsigned long vertexCount,
				unsigned short *indices,
				unsigned int indexCount,
				const SRange *ranges,
				unsigned int rangeCount,
				URef< GrUberTexture > uberTexture,
				unsigned int vbFlags,
				unsigned int ibFlags,
				unsigned int maxVertexCount,
				unsigned int maxIndexCount )
: _name( name )
, _ranges( new SRange[ rangeCount ] )
, _rangeCount( rangeCount )
, _aaBox( MVec3( 0.0f, 0.0f, 0.0f ), MVec3( 0.0f, 0.0f, 0.0f ) )
, _uberTexture( uberTexture )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	// ensure we have a valid vertex count.
	B_ASSERT( vertexCount > 0 );

	// create the vertex buffer.
	_vertexBuffer = gGrMeshBufferMgr->CreateMeshVB( vertexCount, verts, normals, binormals,
		tangents, colors, uvs, vbFlags, maxVertexCount );
	if ( indexCount > 0 )
		_indexBuffer = gGrMeshBufferMgr->CreateMeshIB( indexCount, indices, ibFlags, maxIndexCount );

	// copy ranges.
	for ( unsigned int i = 0; i < _rangeCount; ++i )
		_ranges[ i ] = ranges[ i ];

	// calc bounds.
	CalcAABox();
	CalcBoundingSphere();
}

//----------------------------------------------------------
GrMesh::GrMesh( const tstring& name,
				unsigned int vertexComponents,
				unsigned int vertexCount,
				unsigned int indexCount,
				const SRange* ranges,
				unsigned int rangeCount,
				URef< GrUberTexture > uberTexture,
				unsigned int vbFlags,
				unsigned int ibFlags )
: _name( name )
, _ranges( new SRange[ rangeCount ] )
, _rangeCount( rangeCount )
, _aaBox( MVec3( 0.0f, 0.0f, 0.0f ), MVec3( 0.0f, 0.0f, 0.0f ) )
, _uberTexture( uberTexture )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	// ensure we have a valid vertex count.
	B_ASSERT( vertexCount > 0 );

	// create the vertex buffer.
	_vertexBuffer = gGrMeshBufferMgr->CreateMeshVB( vertexCount, vertexComponents, vbFlags );
	if ( indexCount > 0 )
		_indexBuffer = gGrMeshBufferMgr->CreateMeshIB( indexCount, ibFlags );

	// copy ranges.
	for ( unsigned int i = 0; i < _rangeCount; ++i )
		_ranges[ i ] = ranges[ i ];
}

//----------------------------------------------------------
GrMesh::GrMesh( const tstring& name,
				SVec3* verts,
				SVec3* normals,
				SVec3* binormals,
				SVec3* tangents,
				unsigned long* colors,
				SVec2* uvs,
				unsigned long vertexCount,
				unsigned short* indices,
				unsigned int indexCount,
				SRange* ranges,
				unsigned int rangeCount,
				URef< GrUberTexture > uberTexture,
				const MAABox& aaBox,
				const MSphere& boundingSphere,
				bool vbDynamic,
				bool ibDynamic )
: _name( name )
, _ranges( new SRange[ rangeCount ] )
, _rangeCount( rangeCount )
, _aaBox( aaBox )
, _boundingSphere( boundingSphere )
, _uberTexture( uberTexture )
, _skin( 0 )
, _startSkinIdx( 0 )
, _skinVertexCount( 0 )
{
	B_ASSERT( vertexCount > 0 );
	B_ASSERT( indexCount > 0 || indices == 0 );

	// create the vertex/index buffer.
	_vertexBuffer = gGrMeshBufferMgr->CreateMeshVB( vertexCount, verts, normals, binormals,
		tangents, colors, uvs, vbDynamic );
	if ( indexCount > 0 )
		_indexBuffer = gGrMeshBufferMgr->CreateMeshIB( indexCount, indices, ibDynamic );

	// copy ranges.
	for ( unsigned int i = 0; i < _rangeCount; ++i )
		_ranges[ i ] = ranges[ i ];

	// calc bounds.
	CalcAABox();
	CalcBoundingSphere();
}

//----------------------------------------------------------
GrMesh::~GrMesh()
{
	CleanUp();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrMesh*
GrMesh::Clone()
{
	// create a new mesh.
	GrMesh* newMesh = new GrMesh( _name, _vertexBuffer->GetVertices(), _vertexBuffer->GetNormals(),
		_vertexBuffer->GetBiNormals(), _vertexBuffer->GetTangents(), _vertexBuffer->GetColors(),
		_vertexBuffer->GetTexCoords(), _vertexBuffer->GetVertexCount(), _indexBuffer->GetIndices(),
		_indexBuffer->GetIndexCount(), _ranges, _rangeCount, _uberTexture, _aaBox, _boundingSphere,
		( _vertexBuffer->GetFlags() & GrMeshVB::kDynamic ) != 0, ( _indexBuffer->GetFlags() & GrMeshIB::kDynamic ) != 0 );

	// set skinning info on the mesh if necessary.
	if ( _skin != 0 )
		newMesh->SetSkin( _skin );

	// return the new mesh.
	return newMesh;
}

//----------------------------------------------------------
GrMesh*
GrMesh::CloneRange( unsigned int rangeIdx )
{
	B_ASSERT( rangeIdx < _rangeCount );

	unsigned int start = _ranges[ rangeIdx ].start;
	unsigned int count = _ranges[ rangeIdx ].count;
	if ( _indexBuffer )
	{
		start = _ranges[ rangeIdx ].minIndex;
		count = _ranges[ rangeIdx ].maxIndex - _ranges[ rangeIdx ].minIndex + 1;
	}

	// copy range data.
	SRange range;
	range.name = _ranges[ rangeIdx ].name;
	range.start = 0;
	range.count = _ranges[ rangeIdx ].count;
	range.maxIndex = _ranges[ rangeIdx ].maxIndex - _ranges[ rangeIdx ].minIndex;
	range.minIndex = 0;

	// copy the indices.
	unsigned short* indices = 0;
	unsigned int indexCount = 0;
	if ( _indexBuffer )
	{
		// copy index data.
		indexCount = range.count;
		indices = new unsigned short[ indexCount ];
		MemCopy( indices, _indexBuffer->GetIndices() + _ranges[ rangeIdx ].start, indexCount * sizeof( unsigned short ) );

		// adjust the indices.
		for ( unsigned int i = 0; i < indexCount; ++i )
			indices[ i ] -= start;
	}

	// create a new mesh.
	GrMesh* newMesh = new GrMesh( _name, _vertexBuffer->GetVertices() + start, _vertexBuffer->GetNormals() + start,
		_vertexBuffer->GetBiNormals() + start, _vertexBuffer->GetTangents() + start, _vertexBuffer->GetColors() + start,
		_vertexBuffer->GetTexCoords() + start, count, indices, indexCount, &range, 1, _uberTexture, _aaBox, _boundingSphere, 0 );

	// delete indices if any were created.
	delete[] indices;

	// set skinning info on the mesh if necessary.
	if ( _skin != 0 )
		newMesh->SetSkin( _skin, start, count );

	// return the new mesh.
	return newMesh;
}

//----------------------------------------------------------
void
GrMesh::Cache()
{
	_vertexBuffer->Cache();
	if ( _indexBuffer )
		_indexBuffer->Cache();
}

//----------------------------------------------------------
void
GrMesh::Evict()
{
	_vertexBuffer->Evict();
	if ( _indexBuffer )
		_indexBuffer->Evict();
}

//----------------------------------------------------------
void
GrMesh::Pick( TriHitArray& triHits, const MRay& ray ) const
{
	// TODO: OPTIMIZE THIS.  We might want to cache face normals/plane equations
	// for each triangle!  Note that we can cache the dominant axis too... this
	// way we can eliminate the code in the inner loop that does this!

	// perform a check on each range to see if we need to test the triangles
	// for intersection.
	float localU = 0.0f;
	float localV = 0.0f;
	MVec3 point;
	unsigned int triangleVertex = ~0;
	for ( unsigned int range = 0; range < _rangeCount; ++range )
	{
		// if we intersect with the AABox in the range...
		if ( ray.Intersect( _ranges[ range ].aaBox ) )
		{
			// perform ray-triangle intersections with each triangle in the
			// range.
			const unsigned int vertexIncr = 3;

			// check to see if we're indexed or not.
			if ( _indexBuffer )
			{
				// get the indices.
				unsigned short* indices = _indexBuffer->GetIndices();

				// go through all of the triangles in the range.  Note that we're
				// going through vertices.
				for ( unsigned int v = 0; v < _ranges[ range ].count; v += 3 )
				{
					// get the vertices of the triangle.
					unsigned int vert = _ranges[ range ].start + v;
					MVec3 v0 = _vertexBuffer->GetVertices()[ indices[ vert ] ];
					MVec3 v1 = _vertexBuffer->GetVertices()[ indices[ vert+1 ] ];
					MVec3 v2 = _vertexBuffer->GetVertices()[ indices[ vert+2 ] ];

					if ( ray.Intersect( point, localU, localV, v0, v1, v2, false ) )
					{
						STriCollision& collision = triHits.Push();
						collision.u = localU;
						collision.v = localV;
						collision.w = 1.0f - localU - localV;
						collision.hitLoc = point;
						collision.hitNormal = ComputeTriangleNormal( v0, v1, v2 );
						collision.triIdx = vert / 3;
					}
				}
			}
			else
			{
				// go through all of the triangles in the range.  Note that we're
				// going through vertices.
				for ( unsigned int v = 0; v < _ranges[ range ].count; v += vertexIncr )
				{
					// get the vertices of the triangle.
					unsigned int vert = _ranges[ range ].start + v;
					MVec3 v0 = _vertexBuffer->GetVertices()[ vert ];
					MVec3 v1 = _vertexBuffer->GetVertices()[ vert+1 ];
					MVec3 v2 = _vertexBuffer->GetVertices()[ vert+2 ];

					if ( ray.Intersect( point, localU, localV, v0, v1, v2 ) )
					{
						STriCollision& collision = triHits.Push();
						collision.u = localU;
						collision.v = localV;
						collision.w = 1.0f - localU - localV;
						collision.hitLoc = point;
						collision.hitNormal = ComputeTriangleNormal( v0, v1, v2 );
						collision.triIdx = vert / 3;
					}
				}
			}
		}
	}
}

//----------------------------------------------------------
void
GrMesh::PickRange( TriHitArray& triHits, unsigned int range, const MRay& ray ) const
{
	float localU = 0.0f;
	float localV = 0.0f;
	MVec3 point;

	// if we intersect with the AABox in the range...
	if ( ray.Intersect( _ranges[ range ].aaBox ) )
	{
		// perform ray-triangle intersections with each triangle in the
		// range.
		const unsigned int vertexIncr = 3;

		// check to see if we're indexed or not.
		if ( _indexBuffer )
		{
			// get the indices.
			unsigned short* indices = _indexBuffer->GetIndices();

			// go through all of the triangles in the range.  Note that we're
			// going through vertices.
			for ( unsigned int v = 0; v < _ranges[ range ].count; v += 3 )
			{
				// get the vertices of the triangle.
				unsigned int vert = _ranges[ range ].start + v;
				MVec3 v0 = _vertexBuffer->GetVertices()[ indices[ vert ] ];
				MVec3 v1 = _vertexBuffer->GetVertices()[ indices[ vert+1 ] ];
				MVec3 v2 = _vertexBuffer->GetVertices()[ indices[ vert+2 ] ];

				if ( ray.Intersect( point, localU, localV, v0, v1, v2, false ) )
				{
					STriCollision& collision = triHits.Push();
					collision.u = localU;
					collision.v = localV;
					collision.w = 1.0f - localU - localV;
					collision.hitLoc = point;
					collision.hitNormal = ComputeTriangleNormal( v0, v1, v2 );
					collision.triIdx = vert / 3;
				}
			}
		}
		else
		{
			// go through all of the triangles in the range.  Note that we're
			// going through vertices.
			for ( unsigned int v = 0; v < _ranges[ range ].count; v += vertexIncr )
			{
				// get the vertices of the triangle.
				unsigned int vert = _ranges[ range ].start + v;
				MVec3 v0 = _vertexBuffer->GetVertices()[ vert ];
				MVec3 v1 = _vertexBuffer->GetVertices()[ vert+1 ];
				MVec3 v2 = _vertexBuffer->GetVertices()[ vert+2 ];

				if ( ray.Intersect( point, localU, localV, v0, v1, v2 ) )
				{
					STriCollision& collision = triHits.Push();
					collision.u = localU;
					collision.v = localV;
					collision.w = 1.0f - localU - localV;
					collision.hitLoc = point;
					collision.hitNormal = ComputeTriangleNormal( v0, v1, v2 );
					collision.triIdx = vert / 3;
				}
			}
		}
	}
}

//----------------------------------------------------------
unsigned int
GrMesh::GetTriCount() const
{
	// determine the total number of triangles.
	unsigned int count = 0;
	if ( _indexBuffer )
		count = _indexBuffer->GetIndexCount() / 3;
	else
		count = _vertexBuffer->GetVertexCount() / 3;

	// return the total number of triangles.
	return count;
}

//----------------------------------------------------------
void
GrMesh::GetTriInfo( STriInfo& triInfo, unsigned int triIdx ) const
{
	// make sure we were given a valid triangle index!
	B_ASSERT( triIdx < GetTriCount() );

	unsigned int start = 3 * triIdx;
	if ( _indexBuffer )
	{
		// get the vertex indices.
		unsigned short* indices = _indexBuffer->GetIndices();
		triInfo.vertIdx0 = indices[ start+0 ];
		triInfo.vertIdx1 = indices[ start+1 ];
		triInfo.vertIdx2 = indices[ start+2 ];
	}
	else
	{
		// get the vertex indices.
		triInfo.vertIdx0 = start+0;
		triInfo.vertIdx1 = start+1;
		triInfo.vertIdx2 = start+2;
	}

	// calculate the range.
	for ( unsigned int i = 0; i < _rangeCount; ++i )
	{
		// get the current range.
		SRange* curRange = _ranges + i;

		// check to see if the starting vertex falls within the current range.
		if ( ( start - curRange->start ) < curRange->count )
		{
			triInfo.range = i;
			break;
		}
	}
}

//----------------------------------------------------------
URef< GrMeshVB >
GrMesh::GetVertexData() const
{
	return _vertexBuffer;
}

//----------------------------------------------------------
URef< GrMeshIB >
GrMesh::GetIndexData() const
{
	return _indexBuffer;
}

//----------------------------------------------------------
URef< GrUberTexture >
GrMesh::GetUberTexture() const
{
	return _uberTexture;
}

//----------------------------------------------------------
void
GrMesh::VertexDataChanged()
{
	// calc bounds.
	CalcAABox();
	CalcBoundingSphere();
}

//----------------------------------------------------------
unsigned int
GrMesh::GetVertexCount() const
{
	return _vertexBuffer->GetVertexCount();
}

//----------------------------------------------------------
unsigned int
GrMesh::GetIndexCount() const
{
	return ( _indexBuffer ? _indexBuffer->GetIndexCount() : 0 );
}

//----------------------------------------------------------
void
GrMesh::SetSkin( URef< GrSkin > skin )
{
	// the skin can only be set once!  Once it's set, it cannot be changed!
	B_ASSERT( _skin == 0 );
	_skin = skin;
	_startSkinIdx = 0;
	_skinVertexCount = _vertexBuffer->GetVertexCount();
}

//----------------------------------------------------------
const MSphere&
GrMesh::GetRangeBoundingSphere( unsigned int range ) const
{
	B_ASSERT( range < _rangeCount );
	return _ranges[ range ].bounds;
}

//----------------------------------------------------------
const MAABox&
GrMesh::GetRangeAABox( unsigned int range ) const
{
	B_ASSERT( range < _rangeCount );
	return _ranges[ range ].aaBox;
}

//----------------------------------------------------------
void
GrMesh::GetRangePlane( MVec3& outNormal, MVec3& outPoint, unsigned int range ) const
{
	B_ASSERT( range < _rangeCount );

	// get the vertices.
	SVec3* vertices = _vertexBuffer->GetVertices();
	B_ASSERT( vertices != 0 );
	if ( _indexBuffer != 0 )
	{
		// get the indices.
		unsigned short* indices = _indexBuffer->GetIndices();

		// this should be impossible.
		B_ASSERT( _ranges[ range ].count >= 3 );
		if ( _ranges[ range ].count < 3 )
			return;

		unsigned int baseVert = _ranges[ range ].start;
		unsigned int triIndex = 0;
		unsigned int indexCount = _indexBuffer->GetIndexCount();
		while ( ( baseVert + 3 ) < indexCount )
		{
			MVec3 pt0 = vertices[ indices[ baseVert + 0 ] ];
			MVec3 pt1 = vertices[ indices[ baseVert + 1 ] ];
			MVec3 pt2 = vertices[ indices[ baseVert + 2 ] ];

			// figure out the normal.
			MVec3 normal = ( pt2 - pt1 ).Cross( pt0 - pt1 );

			// if non-zero length, compute the plane and return.
			if ( normal.MagSqr() > 0.01 )
			{
				normal.Normalize();

				// all done.
				outNormal = normal;
				outPoint = pt0;
				return;
			}

			// next triangle.
			baseVert += 3;
			++triIndex;
		}
	}
	else
	{
		// this should be impossible.
		B_ASSERT( _ranges[ range ].count >= 3 );
		if ( _ranges[ range ].count < 3 )
			return;

		// calculate the plane.
		unsigned int baseVert = _ranges[ range ].start;
		unsigned int triIndex = 0;
		unsigned int vertexCount = _vertexBuffer->GetVertexCount();
		while ( ( baseVert + 3 ) < vertexCount )
		{
			MVec3 pt0 = vertices[ baseVert + 0 ];
			MVec3 pt1 = vertices[ baseVert + 1 ];
			MVec3 pt2 = vertices[ baseVert + 2 ];

			// figure out the normal.
			MVec3 normal = ( pt0 - pt1 ).Cross( pt2 - pt1 );

			// if non-zero length, compute the plane and return.
			if ( normal.MagSqr() > 0.01 )
			{
				normal.Normalize();

				// all done.
				outNormal = normal;
				outPoint = pt0;
				return;
			}

			// next triangle.
			baseVert += 3;
			++triIndex;
		}
	}

	// no triangles with any area!  This isn't really killer... just kinda... shitty.
	B_ASSERT( false );
	outNormal = MVec3( 0.0f, 0.0f, 1.0f );
	outPoint = MVec3( 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------
void
GrMesh::CalcAABox()
{
	// get the vertices.
	SVec3* vertices = _vertexBuffer->GetVertices();
	unsigned int vertexCount = _vertexBuffer->GetVertexCount();

	// check for the min and max X, Y, and Z values.
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		minX = Min( vertices[ i ].x, minX );
		minY = Min( vertices[ i ].y, minY );
		minZ = Min( vertices[ i ].z, minZ );
		maxX = Max( vertices[ i ].x, maxX );
		maxY = Max( vertices[ i ].y, maxY );
		maxZ = Max( vertices[ i ].z, maxZ );
	}
	_aaBox = MAABox( MVec3( ( maxX + minX ) * 0.5f, ( maxY + minY ) * 0.5f, ( maxZ + minZ ) * 0.5f ),
					 MVec3( 0.5f * ( maxX - minX ), 0.5f * ( maxY - minY ), 0.5f * ( maxZ - minZ ) ) );

	// calculate the best fitting box for ranges.
	for ( unsigned int r = 0; r < _rangeCount; ++r )
	{
		unsigned int startVertex = GetRangeVertexStart( r );
		unsigned int endVertex = startVertex + GetRangeVertexCount( r );

		// calculate the AABB.
		minX = FLT_MAX;
		minY = FLT_MAX;
		minZ = FLT_MAX;
		maxX = -FLT_MAX;
		maxY = -FLT_MAX;
		maxZ = -FLT_MAX;
		for ( unsigned int v = startVertex; v < endVertex; ++v )
		{
			minX = Min( vertices[ v ].x, minX );
			minY = Min( vertices[ v ].y, minY );
			minZ = Min( vertices[ v ].z, minZ );
			maxX = Max( vertices[ v ].x, maxX );
			maxY = Max( vertices[ v ].y, maxY );
			maxZ = Max( vertices[ v ].z, maxZ );
		}
		_ranges[ r ].aaBox = MAABox( MVec3( ( maxX + minX ) * 0.5f, ( maxY + minY ) * 0.5f, ( maxZ + minZ ) * 0.5f ),
									 MVec3( 0.5f * ( maxX - minX ), 0.5f * ( maxY - minY ), 0.5f * ( maxZ - minZ ) ) );
	}
}

//----------------------------------------------------------
void
GrMesh::Optimize()
{

}

//----------------------------------------------------------
void
GrMesh::BeginRender( unsigned int vertexComponents )
{
	_vertexBuffer->Bind( vertexComponents );
	if ( _indexBuffer )
		_indexBuffer->Bind();
	else
		GrBindIB( 0 );
}

//----------------------------------------------------------
void
GrMesh::Render( unsigned int range, bool buildOccQuery )
{
#if !defined _NO_RENDER
	if ( _indexBuffer != 0 )
	{
		bglDrawRangeElements( 
			GL_TRIANGLES,
			_ranges[ range ].minIndex,
			_ranges[ range ].maxIndex,
			_ranges[ range ].count,
			GL_UNSIGNED_SHORT,
			( ( unsigned short* )0 ) + _ranges[ range ].start );
	}
	else
	{
		// strips are not supported if using non-indexed geometry.
		bglDrawArrays( GL_TRIANGLES, _ranges[ range ].start, _ranges[ range ].count );
	}
	CHECK_GL();
#endif
}

//----------------------------------------------------------
void
GrMesh::DrawNormals()
{
	SVec3* vertices = _vertexBuffer->GetVertices();
	SVec3* normals = _vertexBuffer->GetNormals();
	unsigned int vertexCount = _vertexBuffer->GetVertexCount();
	bglBegin( GL_LINES );
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		bglVertex3f( vertices[ i ].x, vertices[ i ].y, vertices[ i ].z );
		bglVertex3f( vertices[ i ].x+normals[ i ].x, vertices[ i ].y+normals[ i ].y, vertices[ i ].z+normals[ i ].z );
	}
	bglEnd();
}

//----------------------------------------------------------
void
GrMesh::DrawTangents()
{
	SVec3* vertices = _vertexBuffer->GetVertices();
	SVec3* tangents = _vertexBuffer->GetTangents();
	unsigned int vertexCount = _vertexBuffer->GetVertexCount();
	bglBegin( GL_LINES );
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		bglVertex3f( vertices[ i ].x, vertices[ i ].y, vertices[ i ].z );
		bglVertex3f( vertices[ i ].x+tangents[ i ].x, vertices[ i ].y+tangents[ i ].y, vertices[ i ].z+tangents[ i ].z );
	}
	bglEnd();
}

//----------------------------------------------------------
void
GrMesh::DrawBinormals()
{
	SVec3* vertices = _vertexBuffer->GetVertices();
	SVec3* biNormals = _vertexBuffer->GetBiNormals();
	unsigned int vertexCount = _vertexBuffer->GetVertexCount();
	bglBegin( GL_LINES );
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		bglVertex3f( vertices[ i ].x, vertices[ i ].y, vertices[ i ].z );
		bglVertex3f( vertices[ i ].x+biNormals[ i ].x, vertices[ i ].y+biNormals[ i ].y, vertices[ i ].z+biNormals[ i ].z );
	}
	bglEnd();
}

//----------------------------------------------------------
void
GrMesh::Save( UWriter& writer ) const
{
	writer.WriteInt( '\0HSM' );

	// write the minor version, then the major version.
	writer.WriteShort( 0 );
	writer.WriteShort( 2 );

	// save the name.
	writer.WriteString( _name );

	// save the VBO creation flags.
	writer.WriteInt( _vertexBuffer->GetFlags() );
	writer.WriteInt( _indexBuffer ? _indexBuffer->GetFlags() : 0 );

	// save the range information.
	writer.WriteInt( ( int )_rangeCount );
	for ( unsigned int i = 0; i < _rangeCount; ++i )
	{
		// save out each range.
		writer.WriteString( _ranges[ i ].name );
		writer.WriteInt( ( int )_ranges[ i ].start );
		writer.WriteInt( ( int )_ranges[ i ].count );
		writer.WriteInt( ( int )_ranges[ i ].minIndex );
		writer.WriteInt( ( int )_ranges[ i ].maxIndex );

		_ranges[ i ].bounds.Save( writer );
		_ranges[ i ].aaBox.Save( writer );
	}

	// store the vertex and index counts.
	writer.WriteInt( ( int )_vertexBuffer->GetVertexCount() );
	if ( _indexBuffer )
		writer.WriteInt( ( int )_indexBuffer->GetIndexCount() );
	else
		writer.WriteInt( 0 );

	// calculate the component flags...
	unsigned int flags = _vertexBuffer->GetUsedComponents();

	// store component flags.
	writer.WriteInt( ( int )flags );

	// write vertex data.
	unsigned int vertexCount = _vertexBuffer->GetVertexCount();
	writer.WriteBytes( _vertexBuffer->GetVertices(), sizeof( SVec3 ) * vertexCount );
	if ( _vertexBuffer->GetTangents() )
		writer.WriteBytes( _vertexBuffer->GetTangents(), sizeof( SVec3 ) * vertexCount );

	if ( _vertexBuffer->GetBiNormals() )
		writer.WriteBytes( _vertexBuffer->GetBiNormals(), sizeof( SVec3 ) * vertexCount );

	if ( _vertexBuffer->GetNormals() )
		writer.WriteBytes( _vertexBuffer->GetNormals(), sizeof( SVec3 ) * vertexCount );

	if ( _vertexBuffer->GetTexCoords() )
		writer.WriteBytes( _vertexBuffer->GetTexCoords(), sizeof( SVec2 ) * vertexCount );

	if ( _vertexBuffer->GetColors() )
		writer.WriteBytes( _vertexBuffer->GetColors(), sizeof( unsigned long ) * vertexCount );

	// write indices.
	if ( _indexBuffer )
		writer.WriteBytes( _indexBuffer->GetIndices(), sizeof( unsigned short ) * _indexBuffer->GetIndexCount() );

	// save the skin name.
	writer.WriteString( _skin != 0 ? _skin->GetName() : "" );
	writer.WriteUInt( _startSkinIdx );
	writer.WriteUInt( _skinVertexCount );

	// write the best fitting box.
	_aaBox.Save( writer );

	// write the bounding sphere.
	_boundingSphere.Save( writer );

	// TODO: Save UV data if necessary.
}

//----------------------------------------------------------
void
GrMesh::Load( UReader& reader, const UFastArray< URef< GrSkin > >& skins )
{
	// clean up...
	CleanUp();

	unsigned short majorVer = 0;
	unsigned short minorVer = 0;
	unsigned long tag = reader.ReadInt();
	if ( tag == '\0HSM' )
	{
		minorVer = reader.ReadShort();
		majorVer = reader.ReadShort();
	}
	else
	{
		B_ASSERT( tag == 'HSEM' );
	}

	// read the name.
	_name = reader.ReadString();

	// read in the VBO enable flag.
	unsigned int vbFlags = reader.ReadInt();
	unsigned int ibFlags = reader.ReadInt();

	// read the range information.
	_rangeCount = ( unsigned int )reader.ReadInt();
	_ranges = new SRange[ _rangeCount ];
	for ( unsigned int i = 0; i < _rangeCount; ++i )
	{
		// read in each range.
		_ranges[ i ].name = reader.ReadString();
		_ranges[ i ].start = ( unsigned int )reader.ReadInt();
		_ranges[ i ].count = ( unsigned int )reader.ReadInt();
		_ranges[ i ].minIndex = ( unsigned int )reader.ReadInt();
		_ranges[ i ].maxIndex = ( unsigned int )reader.ReadInt();

		// load the AO map.
		if ( majorVer < 2 )
			reader.ReadString();

		// read in the bounds.
		_ranges[ i ].bounds.Load( reader );
		_ranges[ i ].aaBox.Load( reader );
	}

	// read in the vertex and index counts.
	unsigned int vertexCount = ( unsigned int )reader.ReadInt();
	unsigned int indexCount = ( unsigned int )reader.ReadInt();

	// read in the component flags and create our vertex buffer.
	unsigned int flags = ( unsigned int )reader.ReadInt();

	// note that we do the & GR_VERTEX_ALL to remove any unsupported
	// components.  Unique UVs have been removed from the engine, and
	// this keeps old data loading.
	_vertexBuffer = gGrMeshBufferMgr->CreateMeshVB( vertexCount, flags & GR_ATTRIB_MASK_ALL,
		( vbFlags & GrMeshVB::kDynamic ) != 0 );

	// read the vertex data.
	if ( flags & GR_ATTRIB_POSITION_MASK )
		reader.ReadBytes( _vertexBuffer->GetVertices(), sizeof( SVec3 ) * vertexCount );

	if ( flags & GR_ATTRIB_TANGENT_MASK )
		reader.ReadBytes( _vertexBuffer->GetTangents(), sizeof( SVec3 ) * vertexCount );

	if ( flags & GR_ATTRIB_BINORMAL_MASK )
		reader.ReadBytes( _vertexBuffer->GetBiNormals(), sizeof( SVec3 ) * vertexCount );

	if ( flags & GR_ATTRIB_NORMAL_MASK )
		reader.ReadBytes( _vertexBuffer->GetNormals(), sizeof( SVec3 ) * vertexCount );

	if ( flags & GR_ATTRIB_TEXCOORD_MASK )
		reader.ReadBytes( _vertexBuffer->GetTexCoords(), sizeof( SVec2 ) * vertexCount );

	if ( flags & GR_ATTRIB_COLOR_MASK )
		reader.ReadBytes( _vertexBuffer->GetColors(), sizeof( unsigned long ) * vertexCount );

	// read indices
	if ( indexCount )
	{
		_indexBuffer = gGrMeshBufferMgr->CreateMeshIB( indexCount, ( ibFlags & GrMeshIB::kDynamic ) != 0 );
		reader.ReadBytes( _indexBuffer->GetIndices(), sizeof( unsigned short ) * indexCount );
	}

	// get the skin.
	tstring skinName = reader.ReadString();
	if ( skinName.length() > 0 )
	{
		for ( unsigned int i = 0; i < skins.GetElemCount(); ++i )
		{
			if ( skins[ i ]->GetName() == skinName )
			{
				_skin = skins[ i ];
				break;
			}
		}
	}
	_startSkinIdx = reader.ReadUInt();
	_skinVertexCount = reader.ReadUInt();

	// read in the best fitting box.
	_aaBox.Load( reader );

	// read the bounding sphere.
	_boundingSphere.Load( reader );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMesh::SetSkin( URef< GrSkin > skin, unsigned int startIdx, unsigned int skinVertexCount )
{
	_skin = skin;
	_startSkinIdx = startIdx;
	_skinVertexCount = skinVertexCount;
}

//----------------------------------------------------------
unsigned int
GrMesh::GetRangeVertexStart( unsigned int range ) const
{
	if ( _indexBuffer )
		return _ranges[ range ].minIndex;
	return _ranges[ range ].start;
}

//----------------------------------------------------------
unsigned int
GrMesh::GetRangeVertexCount( unsigned int range ) const
{
	if ( _indexBuffer )
		return _ranges[ range ].maxIndex - _ranges[ range ].minIndex + 1;
	return _ranges[ range ].count;
}

//----------------------------------------------------------
void
GrMesh::CalcBoundingSphere()
{
	// use the AA box to calculate the sphere.
	_boundingSphere.SetCenter( _aaBox.GetCenter() );
	_boundingSphere.SetRadius( _aaBox.GetHalfExts().Mag() );

	// now construct bounding spheres for the ranges.
	for ( unsigned int r = 0; r < _rangeCount; ++r )
	{
		SRange& curRange = _ranges[ r ];
		curRange.bounds.SetCenter( curRange.aaBox.GetCenter() );
		curRange.bounds.SetRadius( curRange.aaBox.GetHalfExts().Mag() );
	}
}

//----------------------------------------------------------
void
GrMesh::CleanUp()
{
	delete[] _ranges;
	_skin = 0;
	_ranges = 0;
	_vertexBuffer = 0;
	_indexBuffer = 0;
}
