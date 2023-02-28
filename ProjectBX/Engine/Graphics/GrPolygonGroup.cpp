//----------------------------------------------------------
// File:		GrPolygonGroup.cpp
// Author:		Kevin Bray
// Created:		07-31-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonGroup.h"

// project includes.
#include "GrPolygon.h"
#include "GrMeshInst.h"
#include "GrMesh.h"
#include "MPlane.h"
#include "GrMaterial.h"
#include "GrTextureBase.h"
#include "MSphere.h"
#include "GrSkin.h"
#include "GrTexture.h"
#include "GrPolygonBITree.h"
#include "GrCalcTangents.h"
#include "GrUberTexture.h"

// std c++ includes.
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#include <set>
#endif

#define USE_APPROX_COMPARE				1
#define INDEXIFY_RANGES_SEPARATE		1

#define TO_FIXED_18( x )				( ( int )( ( double )( x ) * 262144.0 + 0.5 * Sign( x ) ) )
#define TO_FIXED_10( x )				( ( int )( ( double )( x ) * 1024.0 + 0.5 * Sign( x ) ) )
#define TO_FIXED_8( x )					( ( int )( ( double )( x ) * 256.0 + 0.5 * Sign( x ) ) )
#define TO_FIXED_0( x )					( ( int )( ( double )( x ) * 0.0 + 0.5 * Sign( x ) ) )

class PolyMatCmp
{
public:
	bool		operator () ( const GrPolygon* lhs, const GrPolygon* rhs )
	{
		// sort by material.
		intptr_t lhsMat = ( intptr_t )( GrMaterial* )lhs->GetMaterial();
		intptr_t rhsMat = ( intptr_t )( GrMaterial* )rhs->GetMaterial();
		return ( lhsMat < rhsMat );
	}
};

struct SSimplePolygon
{
	UFastArray< MVec3 >	positions;
	MVec3 tangent;
	MVec3 binormal;
	MVec3 normal;
	float texelWidth;
	float texelHeight;
};

struct SLineHeightRange
{
	float texelStart;		// first texel for the height.
	float texelHeight;		// height of the current texel.
};

template< typename _T, typename iterator > _T* CopyVector( iterator begin, iterator end, size_t count )
{
	// return NULL if there is nothing to copy.
	if ( begin == end || count == 0 )
		return 0;

	// allocate memory and copy the data.
	_T* mem = new _T[ count ];
	for ( size_t i = 0; i < count; ++i, ++begin )
	{
		B_ASSERT( begin != end );
		mem[ i ] = *begin;
	}

	// return the memory.
	return mem;
}

static bool PlaneSortCmp( const std::pair< float, MPlane >& left, const std::pair< float, MPlane >& right );
static int PolygonCmp( const SSimplePolygon* left, const SSimplePolygon* right );
static unsigned __int64 CalcEdgeHash( const SVec3& v0, const SVec3& v1 );
static inline bool IsPositionEqual( const SVec3& lhs, const SVec3& rhs );
static inline bool IsNormalEqual( const SVec3& lhs, const SVec3& rhs );
static inline bool IsTangentEqual( const SVec3& lhs, const SVec3& rhs );
static inline bool IsUVEqual( const SVec2& lhs, const SVec2& rhs );

//**********************************************************
// class GrPolygonGroup
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonGroup::GrPolygonGroup()
: _vertexCount( 0 )
, _aaBoxDirty( true )
, _boneCount( 0 ) 
, _boneNameArray( 0 )
, _invBindMatrices( 0 )
, _skinned( false )
{

}

//----------------------------------------------------------
GrPolygonGroup::GrPolygonGroup( const PolygonVec& list )
: _vertexCount( 0 )
, _aaBoxDirty( true )
, _boneCount( 0 ) 
, _boneNameArray( 0 )
, _invBindMatrices( 0 )
, _skinned( false )
{
	_polygons = list;
}

//----------------------------------------------------------
GrPolygonGroup::GrPolygonGroup( const GrPolygonGroup& group )
: _vertexCount( 0 )
, _boneNameArray( 0 )
, _invBindMatrices( 0 )
, _boneCount( 0 )
{
	Copy( group );
}

//----------------------------------------------------------
GrPolygonGroup::~GrPolygonGroup()
{
	Clear();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MSphere
GrPolygonGroup::GetBoundingSphere() const
{
	// ensure that the AA box is valid.
	EnsureAABox();
	float radius = _aaBox.GetHalfExts().Mag();
	return MSphere( _aaBox.GetCenter(), radius );
}

//----------------------------------------------------------
const MAABox&
GrPolygonGroup::GetAABox() const
{
	EnsureAABox();
	return _aaBox;
}

//----------------------------------------------------------
const GrPolygon&
GrPolygonGroup::GetPolygon( unsigned int i ) const
{
	B_ASSERT( i < GetPolygonCount() );
	return *_polygons[ i ];
}

//----------------------------------------------------------
void
GrPolygonGroup::SetPolygonMaterial( unsigned int i, URef< GrMaterial > material )
{
	B_ASSERT( i < GetPolygonCount() );
	_polygons[ i ]->SetMaterial( material );
}

//----------------------------------------------------------
void
GrPolygonGroup::SetPolygonUserData( unsigned int i, unsigned int userData )
{
	B_ASSERT( i < GetPolygonCount() );
	_polygons[ i ]->SetUserData( userData );	
}

//----------------------------------------------------------
void
GrPolygonGroup::SetGroupUserData( unsigned int userData )
{
	unsigned int polyCount = ( unsigned int )_polygons.size();
	for ( unsigned int i = 0; i < polyCount; ++i )
		_polygons[ i ]->SetUserData( userData );
}

//----------------------------------------------------------
void
GrPolygonGroup::SetPolygonUV( unsigned int polygon, unsigned int vertex, const SVec2& uv ) const
{
	// set the polygon's vertex data.
	B_ASSERT( polygon < ( unsigned int )_polygons.size() );
	GrPolygon& curPolygon = *_polygons[ polygon ];

	// set the vertex's uv.
	B_ASSERT( vertex < curPolygon.GetVertexCount() );
	_polygons[ polygon ]->GetVertex( vertex ).uv = uv;
}

//----------------------------------------------------------
void
GrPolygonGroup::GenFaceNormals()
{
	// go through each polygon and set vertex normals to the face normal.
	const unsigned int polyCount = ( unsigned int )( _polygons.size() );
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		// get the current polygon and make sure it's valid.
		GrPolygon& curPolygon = *_polygons[ i ];
		B_ASSERT( curPolygon.GetVertexCount() >= 3 );

		// set the vertex flags on the polygon to include a vertex normal.
		curPolygon.SetVertexFlags( _polygons[ i ]->GetVertexFlags() | GrPolygon::kNormal );

		// set the vertex normals to the face normal.
		const SVec3 faceNormal = curPolygon.GetPlane().GetNormal();
		const unsigned int vertexCount = curPolygon.GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
			curPolygon.GetVertex( j ).normal = faceNormal;
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::GenTSBasis()
{
	// create a place to hold face tangents and binormals.
	const unsigned int polyCount = ( unsigned int )( _polygons.size() );
	MVec3* faceTangents = new MVec3[ polyCount ];
	MVec3* faceBinormals = new MVec3[ polyCount ];

	// go through each polygon and create a face tangent and binormal.
	unsigned int newFlags = GrPolygon::kTangent | GrPolygon::kBiNormal;
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		// make sure we have a valid polygon for the TS basis.
		B_ASSERT( _polygons[ i ]->GetVertexCount() >= 3 );

		// the polygon MUST have a valid UV mapping.
		B_ASSERT( ( _polygons[ i ]->GetVertexFlags() & GrPolygon::kUV ) != 0 );

		// set the vertex flags on the polygon to include a TS basis.
		_polygons[ i ]->SetVertexFlags( _polygons[ i ]->GetVertexFlags() | newFlags );

		// generate the TS basis for the polygon face.
		if ( !GenPolygonTSBasis( faceTangents[ i ], faceBinormals[ i ], *_polygons[ i ] ) )
		{
			B_ASSERT( !"failed to generate tangents and binormals." );

			// if generating the TS basis failed, then we need to
			// initialize with default data.
			faceTangents[ i ] = MVec3( 1.0f, 0.0f, 0.0f );
			faceBinormals[ i ] = MVec3( 0.0f, 1.0f, 0.0f );

			// make sure we don't have any degenerate cases!
			if ( _polygons[ i ]->GetNormal().Dot( faceTangents[ i ] ) > 0.95f )
				faceTangents[ i ] = _polygons[ i ]->GetNormal().Cross( faceBinormals[ i ] );
			if ( _polygons[ i ]->GetNormal().Dot( faceBinormals[ i ] ) > 0.95f )
				faceBinormals[ i ] = _polygons[ i ]->GetNormal().Cross( faceTangents[ i ] );
		}

		B_ASSERT( faceTangents[ i ].MagSqr() > 0.9f && faceTangents[ i ].MagSqr() < 1.1f );
		B_ASSERT( faceBinormals[ i ].MagSqr() > 0.9f && faceBinormals[ i ].MagSqr() < 1.1f );
	}

	// build a spacial partition of our polygons.
	GrPolygonBITree partition( this, false );

	// this is a list that we build to contain neighboring polygons.
	UFastArray< unsigned int > neighbors;
	neighbors.Reserve( 10000 );

	// now calculate the tangent and binormal for each vertex.  This
	// is similar to smoothing normals on a mesh.  Since the triangle
	// linking discriminates normals, we can simply smooth with
	// neighbors (or join neighbors back to back).
#if 1
	for ( unsigned int poly = 0; poly < polyCount; ++poly )
	{
		// get the current polygon.
		const GrPolygon* curPoly = _polygons[ poly ];

		// get the number of vertices in the current polygon and make
		// sure it's valid.
		unsigned int vertexCount = curPoly->GetVertexCount();
		if ( vertexCount == 0 )
			continue;

		// calculate the bounding volume of the current polygon.
		SVec3 boxMin( curPoly->GetVertex( 0 ).position );
		SVec3 boxMax( curPoly->GetVertex( 0 ).position );
		for ( unsigned int vert = 1; vert < vertexCount; ++vert )
		{
			// get the current position.
			const SVec3& curPos = curPoly->GetVertex( vert ).position;

			// calculate the AABB's min corner.
			boxMin.x = Min( boxMin.x, curPos.x );
			boxMin.y = Min( boxMin.y, curPos.y );
			boxMin.z = Min( boxMin.z, curPos.z );

			// calculate the AABB's max corner.
			boxMax.x = Max( boxMax.x, curPos.x );
			boxMax.y = Max( boxMax.y, curPos.y );
			boxMax.z = Max( boxMax.z, curPos.z );
		}

		// push the box out by the epsilon amount specified so that we get
		// neighboring polygons.
		const float kBoundsEpsilon = 5.0f * GR_GENERIC_DISTANCE_EPSILON;
		boxMin.x -= kBoundsEpsilon;
		boxMin.y -= kBoundsEpsilon;
		boxMin.z -= kBoundsEpsilon;
		boxMax.x += kBoundsEpsilon;
		boxMax.y += kBoundsEpsilon;
		boxMax.z += kBoundsEpsilon;

		// now do a query for neighboring polygons.
		neighbors.Clear();
		partition.GetPolygons( neighbors, boxMin, boxMax );

		// for each vertex...
		for ( unsigned int vert = 0; vert < vertexCount; ++vert )
		{
			// try to find a polygon that has a similar vertex.
			unsigned int matchCount = 1;
			const MVec3& position1 = _polygons[ poly ]->GetVertex( vert ).position;
			const MVec3& normal1 = _polygons[ poly ]->GetVertex( vert ).normal;
			MVec3 orthoTangent1 = faceTangents[ poly ].Orthonormalized( normal1 );
			MVec3 orthoBinormal1 = faceBinormals[ poly ].Orthonormalized( normal1 );
			SVec3 curTangent = orthoTangent1;
			SVec3 curBinormal = orthoBinormal1;

			// iterate through each neighbor and smooth tangents and binormals.
			unsigned int neighborCount = neighbors.GetElemCount();
			for ( unsigned int neighbor = 0; neighbor < neighborCount; ++neighbor )
			{
				// get the index of the current neighbor polygon.
				unsigned int poly2 = neighbors[ neighbor ];

				// make sure we don't smooth with ourselves.
				if ( poly == poly2 )
					continue;

				// smooth with each vertex on the neighbor.
				unsigned int vertexCount2 = _polygons[ poly2 ]->GetVertexCount();
				for ( unsigned int vert2 = 0; vert2 < vertexCount2; ++vert2 )
				{
					// cosine of the tolerance angle (120 degrees).
					const float kTolerance = -0.5f;

					// if the positions and the normals are equal, then we need to calculate
					// orthonormalized tangents and binormals and check to see if they can be
					// smoothed.
					const MVec3& position2 = _polygons[ poly2 ]->GetVertex( vert2 ).position;
					const MVec3& normal2 = _polygons[ poly2 ]->GetVertex( vert2 ).normal;
					if ( IsPositionEqual( position1, position2 ) && IsNormalEqual( normal1, normal2 ) )
					{
						// Calculate orthonormal tangents and binormals here.  They only have to
						// be orthogonal to the normal.
						MVec3 orthoTangent2 = faceTangents[ poly2 ].Orthonormalized( normal2 );
						MVec3 orthoBinormal2 = faceBinormals[ poly2 ].Orthonormalized( normal2 );

						// Change this check to use the orthonormalized tangents and binormals.
						// If the tangents and binormals are within a certain margin of error,
						// then we need to smooth them.
						if ( ( orthoTangent1.Dot( orthoTangent2 ) > kTolerance ) &&
							 ( orthoBinormal1.Dot( orthoBinormal2 ) > kTolerance ) )
						{
							// change this to use the orthonormalized tangents and binormals.
							// found a match.
							curTangent += orthoTangent2;
							curBinormal += orthoBinormal2;
							++matchCount;
							break;
						}
					}
				}
			}

			// record the averaged vectors.
			// check to see if the triangle is too small in UV space.
			// If so, simply make something up.
			if ( curTangent.Mag() < MVec3::kNormalizeEpsilon )
				curTangent = MVec3( 1.0f, 0.0f, 0.0f );
			if ( curBinormal.Mag() < MVec3::kNormalizeEpsilon )
				curBinormal = MVec3( 0.0f, 1.0f, 0.0f );

			_polygons[ poly ]->GetVertex( vert ).tangent = curTangent.Normalized();
			_polygons[ poly ]->GetVertex( vert ).binormal = curBinormal.Normalized();

			// verify that the tangents created are valid.
			B_ASSERT( _polygons[ poly ]->GetVertex( vert ).tangent.MagSqr() > 0.0001f );
			B_ASSERT( _polygons[ poly ]->GetVertex( vert ).binormal.MagSqr() > 0.0001f );
		}
	}
#else
	for ( unsigned int poly = 0; poly < polyCount; ++poly )
	{
		// for each vertex...
		unsigned int vertexCount = _polygons[ poly ]->GetVertexCount();
		for ( unsigned int vert = 0; vert < vertexCount; ++vert )
		{
			// try to find a polygon that has a similar vertex.
			unsigned int matchCount = 1;
			SVec3 curTangent = faceTangents[ poly ];
			SVec3 curBinormal = faceBinormals[ poly ];

			for ( unsigned int poly2 = 0; poly2 < polyCount; ++poly2 )
			{
				// skip this triangle.
				if ( poly == poly2 )
					continue;

				unsigned int vertexCount2 = _polygons[ poly2 ]->GetVertexCount();
				for ( unsigned int vert2 = 0; vert2 < vertexCount2; ++vert2 )
				{
					// cosine of the tolerance angle (60 degrees).
					const float kTolerance = 0.5f;

					// if the position, the normals are equal, and the tangents/binormals for
					// each face aren't pointing in wildly different directions, then we can
					// account for the current tangent/binormal pair in the smooth.
					if ( ( _polygons[ poly ]->GetVertex( vert ).position == _polygons[ poly2 ]->GetVertex( vert2 ).position ) &&
						 ( _polygons[ poly ]->GetVertex( vert ).normal == _polygons[ poly2 ]->GetVertex( vert2 ).normal ) &&
						 ( faceTangents[ poly2 ].Dot( faceTangents[ poly ] ) > kTolerance ) &&
						 ( faceBinormals[ poly2 ].Dot( faceBinormals[ poly ] ) > kTolerance ) )
					{
						// found a match.
						curTangent += faceTangents[ poly2 ];
						curBinormal += faceBinormals[ poly2 ];
						++matchCount;
						break;
					}
				}
			}

			// record the averaged vectors.
			if ( curTangent.MagSqr() < 0.0001f )
			{
				// make something up... triangle is too small in UV space to do anything.
				curTangent = MVec3( 1.0f, 0.0f, 0.0f );
			}
			if ( curBinormal.MagSqr() < 0.0001f )
			{
				// make something up... triangle is too small in UV space to do anything.
				curBinormal = MVec3( 0.0f, 1.0f, 0.0f );
			}

			_polygons[ poly ]->GetVertex( vert ).tangent = curTangent.Normalized();
			_polygons[ poly ]->GetVertex( vert ).binormal = curBinormal.Normalized();

			B_ASSERT( _polygons[ poly ]->GetVertex( vert ).tangent.MagSqr() > 0.0001f );
			B_ASSERT( _polygons[ poly ]->GetVertex( vert ).binormal.MagSqr() > 0.0001f );
		}
	}
#endif

	// This makes sure that the tangent and binormal are right angles to the
	// surface normal.
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		unsigned int vertexCount = _polygons[ i ]->GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
		{
			SPolygonVertex& vertex = _polygons[ i ]->GetVertex( j );
			B_ASSERT( vertex.normal.MagSqr() > 0.0001f );
			B_ASSERT( vertex.tangent.MagSqr() > 0.0001f );
			B_ASSERT( vertex.binormal.MagSqr() > 0.0001f );

			// make sure the tangent is a right angle to the normal.
			{
				MVec3 temp = vertex.tangent;
				temp.Orthonormalize( vertex.normal );
				vertex.tangent = temp;
			}

#if 0
			// make sure the binormal is a right angle to the normal.
			{
				MVec3 temp = vertex.binormal;
				temp.Orthonormalize( vertex.normal );
				vertex.binormal = temp;
			}
#else
			// make sure that the tangent space basis orthogonal.
			// Note that this can be made better by finding the
			// average tangent and binormal that averages the error
			// across the tangent and the binormal.  Currently, all
			// of the error lies in the binormal.
			{
				// calculate the new binormal so that it's a right
				// angle to the tangent and the binormal.
				MVec3 newBinormal = vertex.normal.Cross( vertex.tangent );
				if ( newBinormal.Dot( vertex.binormal ) > 0.0f )
					vertex.binormal = newBinormal;
				else
					vertex.binormal = -newBinormal;

				// normalize the new binormal.
				vertex.binormal.Normalize();
			}
#endif
			// make sure the resulting tangents and binormals are valid.
			B_ASSERT( vertex.tangent.MagSqr() > 0.0001f );
			B_ASSERT( vertex.binormal.MagSqr() > 0.0001f );
		}
	}

	delete[] faceTangents;
	delete[] faceBinormals;
}

//----------------------------------------------------------
void
GrPolygonGroup::GenPlanarUVMap( const MPlane& projectionPlane )
{
	// calculate a default 2D origin for the new points.
	const MVec3& planeNormal = projectionPlane.GetNormal();
	MVec3 planarOrigin = -projectionPlane.GetD() * planeNormal;
	const MVec3 kUp( 0.0f, 1.0f, 0.0f );
	const MVec3 kRight( 1.0f, 0.0f, 0.0f );

	// check to see if we can use the up vector.
	MVec3 up;
	if ( Abs( planeNormal.Dot( kUp ) ) > 0.9f )
		up = kRight;
	else
		up = kUp;

	// calculate the right vector.
	MVec3 right = up.Cross( planeNormal ).Normalized();
	up = right.Cross( planeNormal );

	// these are needed to scale of the UVs.
	float minU = FLT_MAX;
	float minV = FLT_MAX;
	float maxU = -FLT_MAX;
	float maxV = -FLT_MAX;

	// go through each polygon and create a UV coordinate.
	const unsigned int polyCount = ( unsigned int )( _polygons.size() );
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		// get the current polygon.
		GrPolygon& curPolygon = *_polygons[ i ];

		// set the UV flag on the polygons.
		curPolygon.SetVertexFlags( curPolygon.GetVertexFlags() | GrPolygon::kUV );

		// go through all of the vertices on the polygon.
		unsigned int vertexCount = curPolygon.GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
		{
			// get the current vertex.
			SPolygonVertex& vertex = curPolygon.GetVertex( j );

			// calculate the UV by projecting the position to the plane.
			MVec3 position = vertex.position;
			vertex.uv.x = right.Dot( position );
			vertex.uv.y = up.Dot( position );

			minU = Min( vertex.uv.x, minU );
			minV = Min( vertex.uv.y, minV );
			maxU = Max( vertex.uv.x, maxU );
			maxV = Max( vertex.uv.y, maxV );
		}

	}

	// calculate the width.
	float scale = 1.0f / Max( maxU - minU, maxV - minV );

	// go through all polygon vertices again...
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		// get the current polygon.
		GrPolygon& curPolygon = *_polygons[ i ];

		// go through all of the vertices on the polygon.
		unsigned int vertexCount = curPolygon.GetVertexCount();
		for ( unsigned int j = 0; j < vertexCount; ++j )
		{
			// get the current vertex.
			SPolygonVertex& vertex = curPolygon.GetVertex( j );

			// scale and translate the UVs.
			vertex.uv.x = ( vertex.uv.x - minU ) * scale;
			vertex.uv.y = ( vertex.uv.y - minV ) * scale;
		}
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::GenEdgeList( EdgeVec& edges )
{
	typedef std::map< __int64, SEdge > EdgeMap;
	EdgeMap edgeMap;
	for ( unsigned int i = 0; i < _polygons.size(); ++i )
	{
		// get the current polygon and iterate through it's edges.
		GrPolygon& curPoly = *_polygons[ i ];
		unsigned int edgeCount = curPoly.GetVertexCount() - 1;
		for ( unsigned int j = 0; j < edgeCount; ++j )
		{
			SVec3 pos0 = curPoly.GetVertex( j ).position;
			SVec3 pos1 = curPoly.GetVertex( j+1 ).position;

			// note that the neighbor hash has to be from p1->p0 instead 
			// of p0->p1.  This is because of the winding order of neighboring
			// triangles.
			EdgeMap::iterator iter = edgeMap.find( CalcEdgeHash( pos1, pos0 ) );
			if ( iter == edgeMap.end() )
			{
				SEdge edge;
				edge.polygonIndices[ 0 ] = i;
				edge.polygonIndices[ 1 ] = ~0;
				edge.edges[ 0 ] = j;
				edge.edges[ 1 ] = ~0;
				edgeMap[ CalcEdgeHash( pos0, pos1 ) ] = edge;
			}
			else
			{
				iter->second.polygonIndices[ 1 ] = i;
				iter->second.edges[ 1 ] = j;
			}
		}
	}

	// copy edges into the edge vector.
	EdgeMap::iterator iter = edgeMap.begin();
	const EdgeMap::iterator end = edgeMap.end();
	for ( ; iter != end; ++iter )
		edges.push_back( iter->second );
}

//----------------------------------------------------------
void
GrPolygonGroup::AddPolygon( const GrPolygon& polygon )
{
	// all polygons should be added BEFORE skinning information is set.
	B_ASSERT( !_skinned );

	// add the polygon and adjust the aaBox.
	_aaBoxDirty = true;
	_polygons.push_back( new GrPolygon( polygon ) );
	B_ASSERT( polygon.GetVertexCount() != 0 );
	_vertexCount += polygon.GetVertexCount();
}

//----------------------------------------------------------
void
GrPolygonGroup::AddGroup( const GrPolygonGroup& group )
{
	// we cannot join skinned meshes!  All polygons should be added before
	// skinning information is set.
	B_ASSERT( _polygons.size() == 0 || _skinned == group._skinned );
	if ( _polygons.size() == 0 && group._skinned )
		_skinned = true;

	// copy over skinning data if needed.
	unsigned int* boneRemapping = 0;
	if ( _skinned )
	{
		// allocate some stack space for our bone remapping table.
		boneRemapping = ( unsigned int* )alloca( group._boneCount * sizeof( unsigned int ) );

		// expand the bone name array and the inverse bind matrix array to make room
		// for new bone info.  Note that we'll just give them the maximum size needed.
		tstring* newBoneNameArray = new tstring[ _boneCount + group._boneCount ];
		MMat4x4* newInvBindMatrices = new MMat4x4[ _boneCount + group._boneCount ];
		for ( unsigned int i = 0; i < _boneCount; ++i )
		{
			newBoneNameArray[ i ] = _boneNameArray[ i ];
			newInvBindMatrices[ i ] = _invBindMatrices[ i ];
		}

		// free the old memory and update the pointers.
		delete[] _boneNameArray;
		delete[] _invBindMatrices;
		_boneNameArray = newBoneNameArray;
		_invBindMatrices = newInvBindMatrices;

		// build the skin remapping table.
		unsigned int newBoneCount = _boneCount;
		for ( unsigned int i = 0; i < group._boneCount; ++i )
		{
			// check to see if the current bone already exists.
			unsigned int j = 0;
			for ( ; j < _boneCount; ++j )
			{
				if ( _boneNameArray[ j ] == group._boneNameArray[ i ] )
				{
					boneRemapping[ i ] = j;
					break;
				}
			}

			// check to see if the bone doesn't currently exist in this group.
			if ( j == _boneCount )
			{
				// not found, so add a new bone to our group.
				_boneNameArray[ newBoneCount ] = group._boneNameArray[ i ];
				_invBindMatrices[ newBoneCount ] = group._invBindMatrices[ i ];

				// set the new bone index in the bone remapping table.
				boneRemapping[ i ] = newBoneCount;

				// increment the number of bones we have.
				++newBoneCount;
			}
		}

		// store the new bone count.
		_boneCount = newBoneCount;
	}

	// add the polygons.
	_polygons.reserve( _polygons.size() + group._polygons.size() );
	_aaBoxDirty = true;
	_vertexCount += group.GetVertexCount();
	PolygonVec::const_iterator iter = group._polygons.begin();
	const PolygonVec::const_iterator end = group._polygons.end();
	for ( ; iter != end; ++iter )
	{
		_polygons.push_back( new GrPolygon( *( *iter ) ) );
		if ( _skinned )
			_polygons.back()->RemapBoneIndices( boneRemapping );
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::AddGroup( const GrPolygonGroup& group, const MMat4x4& transform, const MMat3x3& normalRot )
{
	// we cannot join skinned meshes!  All polygons should be added before
	// skinning information is set.
	B_ASSERT( !_skinned );

	// add the polygons.
	_aaBoxDirty = true;
	_vertexCount += group.GetVertexCount();
	_polygons.reserve( _polygons.size() + group._polygons.size() );
	PolygonVec::const_iterator iter = group._polygons.begin();
	const PolygonVec::const_iterator end = group._polygons.end();
	for ( ; iter != end; ++iter )
	{
		_polygons.push_back( new GrPolygon( ( *iter )->Transform( transform, normalRot ) ) );
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::Clear()
{
	for ( unsigned int i = 0; i < _polygons.size(); ++i )
		delete _polygons[ i ];
	_polygons.clear();
	_vertexCount = 0;
	_aaBox = MAABox();
	_aaBoxDirty = true;
	delete[] _boneNameArray;
	delete[] _invBindMatrices;
	_boneNameArray = 0;
	_invBindMatrices = 0;
	_boneCount = 0;
	_skinned = false;
}

//----------------------------------------------------------
void
GrPolygonGroup::SetSkinningInfo( SPolygonSkinVertex* skinVertArray, tstring* boneNames, MMat4x4* invBindMatrices, unsigned int count )
{
	// ensure that the parameters passed in are valid.
	B_ASSERT( skinVertArray != 0 );
	B_ASSERT( boneNames != 0 );
	B_ASSERT( invBindMatrices != 0 );
	B_ASSERT( count > 0 );

	// set the skinning flag to true.
	_skinned = true;

	// free the previous arrays if there were any.
	delete[] _boneNameArray;
	delete[] _invBindMatrices;

	// set up the data.
	_boneNameArray = boneNames;
	_invBindMatrices = invBindMatrices;
	_boneCount = count;

	// we assume that the array passed in has enough skinning info for
	// every polygon.  That said, go through each polygon and set the
	// skinning info for it.
	PolygonVec::iterator iter = _polygons.begin();
	PolygonVec::iterator end = _polygons.end();
	for ( ; iter != end; ++iter )
	{
		( *iter )->SetSkinningInfo( skinVertArray );
		skinVertArray += ( *iter )->GetVertexCount();
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::OptimizePolygons( float epsilon, float distEpsilonSqr )
{
	PolygonVec::iterator iter = _polygons.begin();
	PolygonVec::iterator end = _polygons.end();
	while ( iter != end )
	{
		// if the polygon is degenerate, remove it!
		if ( ( *iter )->IsDegenerate( epsilon ) )
		{
			_vertexCount -= ( *iter )->GetVertexCount();
			delete ( *iter );
			iter = _polygons.erase( iter );
			end = _polygons.end();
			continue;
		}

		// next polygon.
		++iter;
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::WeldVertices( float distEpsilonSqr )
{
	unsigned int polyCount = ( unsigned int )( _polygons.size() );
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		for ( unsigned int j = i + 1; j < polyCount; ++j )
		{
			_polygons[ i ]->WeldVertices( *_polygons[ j ], distEpsilonSqr );
		}
	}
}

//----------------------------------------------------------
void
GrPolygonGroup::FixTJuncs( const GrPolygonGroup& group, float epsilon )
{
	unsigned int polyCount = ( unsigned int )( _polygons.size() );
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		for ( unsigned int j = 0; j < group.GetPolygonCount(); ++j )
		{
			if ( _polygons[ i ]->GetUserFlags() & GrPolygon::EFV_EDGEPOLY )
				_polygons[ i ]->CheckTJunc( *group._polygons[ j ], epsilon );
		}
	}
}
/*
//----------------------------------------------------------
void
GrPolygonGroup::BuildCoplanarGroups( std::vector< GrCoplanarPolygonGroup >& groups, float epsilon ) const
{
	// ensure that there are no polygons in the vector of groups passed in.
	B_ASSERT( groups.size() == 0 );

	// go through all polygons and ensure they are in a group.
	unsigned int curPolyIdx = 0;
	PolygonVec::const_iterator polygonIter = _polygons.begin();
	const PolygonVec::const_iterator polygonEnd = _polygons.end();
	for ( ; polygonIter != polygonEnd; ++polygonIter, ++curPolyIdx )
	{
		// check to see if the current polygon goes into any of the existing groups.
		std::vector< GrCoplanarPolygonGroup >::iterator groupIter = groups.begin();
		const std::vector< GrCoplanarPolygonGroup >::iterator groupEnd = groups.end();
		for ( ; groupIter != groupEnd; ++groupIter )
		{
			if ( groupIter->IsCoplanar( *polygonIter ) )
			{
				groupIter->AddPolygon( curPolyIdx );
				break;
			}
		}

		// all done with the current polygon since it was added to an existing group.
		if ( groupIter != groupEnd )
			continue;

		// get the plane for the current polygon, create a new group and then add the
		// polygon.
		const MPlane& plane = polygonIter->GetPlane();
		groups.push_back( GrCoplanarPolygonGroup( plane, epsilon ) );
		groups.back().AddPolygon( curPolyIdx );
	}

	// sort the groups by surface area.
	std::sort( groups.begin(), groups.end(), GroupSortFunc );
}
*/
//----------------------------------------------------------
void
GrPolygonGroup::GenPlaneList( std::vector< MPlane >& planes, float epsilon ) const
{
	PrintF( "Building plane list..." );

	// make sure the incoming plane vector is empty.
	planes.clear();

	// create a place to store planes and their area.
	std::vector< std::pair< float, MPlane > > localPlaneVec;

	// go through all polygons and ensure they are in a group.
	unsigned int curPolyIdx = 0;
	PolygonVec::const_iterator polygonIter = _polygons.begin();
	const PolygonVec::const_iterator polygonEnd = _polygons.end();
	for ( ; polygonIter != polygonEnd; ++polygonIter, ++curPolyIdx )
	{
		// check to see if the current polygon goes into any of the existing groups.
		std::vector< std::pair< float, MPlane > >::iterator planeIter = localPlaneVec.begin();
		const std::vector< std::pair< float, MPlane > >::iterator planeEnd = localPlaneVec.end();
		for ( ; planeIter != planeEnd; ++planeIter )
		{
			if ( ( *polygonIter )->IsCoplanar( planeIter->second, epsilon ) )
			{
				planeIter->first += ( *polygonIter )->GetArea();
				break;
			}
		}

		// all done with the current polygon since it resides on an existing plane.
		if ( planeIter != planeEnd )
			continue;

		// get the plane for the current polygon and store.
		const MPlane& plane = ( *polygonIter )->GetPlane();
		localPlaneVec.push_back( std::pair< float, MPlane >( ( *polygonIter )->GetArea(), plane ) );
	}

	PrintF(
		" [done]\n"
		" Number of planes: %i\n", ( unsigned int )localPlaneVec.size() );

	// now sort the planes by area.
	std::sort( localPlaneVec.begin(), localPlaneVec.end(), PlaneSortCmp );

	// copy the planes into the output vector, this time sorted by area (largest to smallest).
	std::vector< std::pair< float, MPlane > >::iterator iter = localPlaneVec.begin();
	const std::vector< std::pair< float, MPlane > >::iterator end = localPlaneVec.end();
	for ( ; iter != end; ++iter )
		planes.push_back( iter->second );
}

//----------------------------------------------------------
int
GrPolygonGroup::Split( GrPolygonGroup& front, GrPolygonGroup& back, const MPlane& plane, float epsilon ) const
{
	// split polygons.
	GrPolygon frontPoly;
	GrPolygon backPoly;
	for ( unsigned int i = 0; i < _polygons.size(); ++i )
	{
		int side = _polygons[ i ]->Split( frontPoly, backPoly, plane, epsilon );
		if ( side >= 0 )
			front.AddPolygon( frontPoly );
		if ( side <= 0 )
			back.AddPolygon( backPoly );
	}

	// determine the return code.
	int ret = 0;
	if ( front.GetPolygonCount() > 0 )
		ret += 1;
	if ( back.GetPolygonCount() > 0 )
		ret -= 1;

	// all done.
	return ret;
}

//----------------------------------------------------------
int
GrPolygonGroup::Classify( GrPolygonGroup& front, GrPolygonGroup& back, const MPlane& plane ) const
{
	// classify polygons.
	for ( unsigned int i = 0; i < _polygons.size(); ++i )
	{
		// calculate the polygon's center point.
		MVec3 center;
		unsigned int vertCount = _polygons[ i ]->GetVertexCount();
		float invVertCount = 1.0f / vertCount;
		for ( unsigned int j = 0; j < vertCount; ++j )
			center += _polygons[ i ]->GetVertex( j ).position;

		// take the average position.
		center *= invVertCount;

		// classify the polygon based on the classification of it's
		// center.
		int side = plane.GetSide( center, GR_GENERIC_DISTANCE_EPSILON );
		if ( side >= 0 )
			front.AddPolygon( *_polygons[ i ] );
		if ( side < 0 )
			back.AddPolygon( *_polygons[ i ] );
	}

	// determine the return code.
	int ret = 0;
	if ( front.GetPolygonCount() > 0 )
		ret += 1;
	if ( back.GetPolygonCount() > 0 )
		ret -= 1;

	// all done.
	return ret;	
}

//----------------------------------------------------------
bool
GrPolygonGroup::IsConvexSet( float epsilon ) const
{
	std::vector< MPlane > planes;
	GenPlaneList( planes, epsilon );

	// check to see if any polygons are behind other polygons.
	for ( unsigned int i = 0; i < planes.size(); ++i )
	{
		const MPlane& curPlane = planes[ i ];
		for ( unsigned int j = 0; j < _polygons.size(); ++j )
		{
			GrPolygon::EPLANERELATION planeRel = _polygons[ j ]->CalcPlaneRelation( curPlane, epsilon );
			if ( planeRel == GrPolygon::EPR_BACK || planeRel == GrPolygon::EPR_SPLIT )
				return false;
		}
	}

	// no polygons were behind any other polygons so return true.
	return true;
}

//----------------------------------------------------------
void
GrPolygonGroup::GenMeshes( std::vector< GrMeshInst* >& meshes, const tstring& name, bool genVertexColors,
						   bool vboEnable, unsigned int maxVertexCount ) const
{
	// if we're under the desired vertex count limit, simply generate
	// meshes.
	if ( GetVertexCount() <= maxVertexCount )
	{
		GenMeshesInternal( meshes, name, genVertexColors, vboEnable );
		return;
	}

	// build a lis of all groups.
	std::list< GrPolygonGroup > groups;
	groups.push_back( GrPolygonGroup() );
	groups.push_back( GrPolygonGroup() );

	// split the polygon group into the two elements currently in the
	// list.
	Classify( groups.front(), groups.back(), CalcSplitPlane() );

	// now repeatedly split until all meshes are under the desired
	// limit.
	bool needWork = true;
	std::list< GrPolygonGroup >::iterator iter = groups.begin();
	do
	{
		// split the current group.
		if ( iter->GetVertexCount() > maxVertexCount )
		{
			// allocate two new groups.
			groups.push_back( GrPolygonGroup() );
			GrPolygonGroup& front = groups.back();
			groups.push_back( GrPolygonGroup() );
			GrPolygonGroup& back = groups.back();

			// split the current group into the two new groups.
			int result = iter->Classify( front, back, iter->CalcSplitPlane() );
			if ( result != 0 )
			{
				// if the group wasn't actually split, eliminate the
				// data that we split it's contents into.
				groups.pop_back();
				groups.pop_back();

				// advance to the next item.
				++iter;
			}
			else
			{
				// remove the current item from the list.
				iter = groups.erase( iter );
			}
		}
		else
		{
			// if we didn't need to split the current group, simply
			// advance to the next one.
			++iter;
		}
	}
	while( iter != groups.end() );

	// now tell all groups to generate meshes.
	for ( iter = groups.begin(); iter != groups.end(); ++iter )
		iter->GenMeshesInternal( meshes, name, genVertexColors, vboEnable );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonGroup::GenMeshesInternal( std::vector< GrMeshInst* >& meshes, const tstring& name, bool genVertexColors,
								   bool vboEnable ) const
{
	// group polygons by format/AO texture
	// for each format group
	//  create mesh
	//  group polygons by material
	//  for each material group
	//   for each polygon in material group
	//    triangulate
	//    add triangles to mesh
	//  create mesh instance
	//  set instance mesh
	//  set instance materials
	std::map< __int64, std::vector< const GrPolygon* > > formatGroups;
	PolygonVec::const_iterator polyIter = _polygons.begin();
	const PolygonVec::const_iterator polyEnd = _polygons.end();
	for ( ; polyIter != polyEnd; ++polyIter )
	{
		// get the current polygon.
		const GrPolygon* curPolygon = *polyIter;

		// skip the current polygon if it has no material.
		if ( curPolygon->GetMaterial() == 0 )
			continue;

		// get the polygon's vertex flags so that we can generate a
		// hash value.
		unsigned int flags = curPolygon->GetVertexFlags();
		GrUberTexture* uberTexture = curPolygon->GetUberTexture();

		// calculate a hash value.
		__int64 hash = HashMem64( &flags, sizeof( unsigned int ) );
		hash = HashMem64( &uberTexture, sizeof( GrUberTexture* ), hash );

		// add the polygon.
		std::vector< const GrPolygon* >& group = formatGroups[ hash ];
		group.push_back( curPolygon );
	}

	// ranges.
	UFastArray< GrMesh::SRange > rangeVec;
	UFastArray< URef< GrMaterial > > rangeMaterials;

	// vertex components.
	UFastArray< SVec3 > verts;
	UFastArray< SVec3 > normals;
	UFastArray< SVec3 > binormals;
	UFastArray< SVec3 > tangents;
	UFastArray< unsigned long > colors;
	UFastArray< SVec2 > uvs;
	UFastArray< unsigned int > skinBoneIds;
	UFastArray< SVec3 > skinWeights;

	// go through each different format group.
	PolyMatCmp polyMatCmp;
	UFastArray< unsigned short > indices;
	std::map< __int64, std::vector< const GrPolygon* > >::iterator iter = formatGroups.begin();
	const std::map< __int64, std::vector< const GrPolygon* > >::iterator end = formatGroups.end();
	for ( ; iter != end; ++iter )
	{
		// skip if there is nothing in the current format group.
		std::vector< const GrPolygon* >& curGroup = iter->second;
		if ( curGroup.size() == 0 )
			continue;

		// sort by material.
		std::sort( curGroup.begin(), curGroup.end(), polyMatCmp );

		// initialize to the first polygon's material.
		GrMaterial* curMat = curGroup[ 0 ]->GetMaterial();
		unsigned int flags = curGroup[ 0 ]->GetVertexFlags();
		GrUberTexture* curUberTex = curGroup[ 0 ]->GetUberTexture();

		// make sure the material isn't null!  All polygons are required to have a
		// valid material.
		B_ASSERT( curMat != 0 );

#ifdef _DEBUG
		std::set< GrMaterial* > usedMaterials;
#endif

		// for each polygon, triangulate.
		const unsigned int kMaxVertexCount = 30000;
		std::vector< const GrPolygon* >::iterator polyIter = curGroup.begin();
		const std::vector< const GrPolygon* >::iterator polyEnd = curGroup.end();
		do
		{
			// mesh specific stuff.
			unsigned int rangeStart = 0;
			unsigned int vertexCount = 0;

			// clear ranges from the previous mesh.
			rangeVec.Clear();
			rangeMaterials.Clear();

			// clear vertices from previous mesh.
			verts.Clear();
			normals.Clear();
			tangents.Clear();
			binormals.Clear();
			colors.Clear();
			uvs.Clear();
			skinBoneIds.Clear();
			skinWeights.Clear();

			// go through every polygon in the current vertex format group, as long
			// as we don't contain too many vertices.
			for ( ; polyIter != polyEnd && vertexCount < kMaxVertexCount; ++polyIter )
			{
				// create a new range if necessary.
				if ( ( *polyIter )->GetMaterial() != curMat )
				{
					// add the new range.
					GrMesh::SRange newRange;
					newRange.name = curMat->GetName();
					newRange.start = rangeStart;
					newRange.count = vertexCount - rangeStart;
					rangeVec.Push( newRange );

#ifdef _DEBUG
					B_ASSERT( usedMaterials.find( curMat ) == usedMaterials.end() );
					usedMaterials.insert( curMat );
#endif

					// add the material.
					rangeMaterials.Push( curMat );

					// end of previous range, beginning of new range!
					rangeStart = vertexCount;
					curMat = ( *polyIter )->GetMaterial();

					// make sure the next material isn't null!  All polygons are required
					// to have a valid material.
					B_ASSERT( curMat != 0 );
				}

				// allocate space for the new triangles.
				unsigned int polyTriCount = ( *polyIter )->GetTriangleCount();
				SPolygonTriangle *triangles = 0;
				SPolygonTriangleSkin* skinTriangles = 0;
				if ( _skinned )
				{
					// allocate data for polygons and skinning data.
					unsigned int totalVertexDataSize = sizeof( SPolygonTriangle ) + sizeof( SPolygonTriangleSkin );
					triangles = ( SPolygonTriangle* )TempAlloc( polyTriCount * totalVertexDataSize );
					skinTriangles = ( SPolygonTriangleSkin* )( triangles + polyTriCount );
				}
				else
				{
					// allocate data for polygons (don't worry about skinning info).
					triangles = ( SPolygonTriangle* )TempAlloc( polyTriCount * sizeof( SPolygonTriangle ) );
				}

				// triangulate the current polygon.
				( *polyIter )->Triangulate( triangles, skinTriangles );

				// add the triangles to various vertex component lists.
				for ( unsigned int tri = 0; tri < polyTriCount; ++tri )
				{
					// add the triangle to various vertex component lists.
					verts.Push( triangles[ tri ].v0.position );
					verts.Push( triangles[ tri ].v1.position );
					verts.Push( triangles[ tri ].v2.position );
					if ( flags & GrPolygon::kNormal )
					{
						normals.Push( triangles[ tri ].v0.normal.Normalized() );
						normals.Push( triangles[ tri ].v1.normal.Normalized() );
						normals.Push( triangles[ tri ].v2.normal.Normalized() );
					}
					if ( flags & GrPolygon::kTangent )
					{
						tangents.Push( triangles[ tri ].v0.tangent.Normalized() );
						tangents.Push( triangles[ tri ].v1.tangent.Normalized() );
						tangents.Push( triangles[ tri ].v2.tangent.Normalized() );
					}
					if ( flags & GrPolygon::kBiNormal )
					{
						binormals.Push( triangles[ tri ].v0.binormal.Normalized() );
						binormals.Push( triangles[ tri ].v1.binormal.Normalized() );
						binormals.Push( triangles[ tri ].v2.binormal.Normalized() );
					}
					if ( flags & GrPolygon::kColor )
					{
						colors.Push( *( ( unsigned long* )triangles[ tri ].v0.color ) );
						colors.Push( *( ( unsigned long* )triangles[ tri ].v1.color ) );
						colors.Push( *( ( unsigned long* )triangles[ tri ].v2.color ) );
					}
					if ( flags & GrPolygon::kUV )
					{
						uvs.Push( triangles[ tri ].v0.uv );
						uvs.Push( triangles[ tri ].v1.uv );
						uvs.Push( triangles[ tri ].v2.uv );
					}
					if ( _skinned )
					{
						skinBoneIds.Push( skinTriangles[ tri ].s0.boneIds );
						skinBoneIds.Push( skinTriangles[ tri ].s1.boneIds );
						skinBoneIds.Push( skinTriangles[ tri ].s2.boneIds );
						skinWeights.Push( skinTriangles[ tri ].s0.weights );
						skinWeights.Push( skinTriangles[ tri ].s1.weights );
						skinWeights.Push( skinTriangles[ tri ].s2.weights );
					}
				}

				// increment the vertex count.
				vertexCount += 3 * polyTriCount;

				// free up our scratch memory.
				TempFree( triangles );
			}

			// add the last range.
			GrMesh::SRange newRange;
			newRange.name = curMat->GetName();
			newRange.start = rangeStart;
			newRange.count = vertexCount - rangeStart;
			rangeVec.Push( newRange );

			// add the last material.
			rangeMaterials.Push( curMat );

			// sanity check.
			B_ASSERT( rangeMaterials.GetElemCount() == rangeVec.GetElemCount() );

			// reserve space for the indices.
			indices.Clear();
			indices.Resize( vertexCount, 0 );
#if !INDEXIFY_RANGES_SEPARATE
			// now indexify the mesh data.
			vertexCount = IndexifyMesh( indices,
				verts.GetElemCount() ? verts.GetPtr() : 0,
				normals.GetElemCount() ? normals.GetPtr() : 0,
				tangents.GetElemCount() ? tangents.GetPtr() : 0,
				binormals.GetElemCount() ? binormals.GetPtr() : 0,
				colors.GetElemCount() ? colors.GetPtr() : 0,
				uvs.GetElemCount() ? uvs.GetPtr() : 0,
				uniqueUVs.GetElemCount() ? uniqueUVs.GetPtr() : 0,
				skinBoneIds.GetElemCount() ? skinBoneIds.GetPtr() : 0,
				skinWeights.GetElemCount() ? skinWeights.GetPtr() : 0,
				0, 0, vertexCount );
#else
			vertexCount = 0;
#endif
			// indexify each range.
			for ( unsigned int i = 0; i < rangeVec.GetElemCount(); ++i )
			{
				// get the current range of the mesh.
				GrMesh::SRange& curRange = rangeVec[ i ];
#if INDEXIFY_RANGES_SEPARATE
				// indexify the range.  Note that we indexify each range
				// separately so that they do not share any vertices.  This
				// acts as both a vertex-cache optimization, as well as a way
				// to allow manipulation of vertices in each range without
				// disrupting the vertices in other ranges.
				vertexCount += IndexifyMesh( indices,
					verts.GetElemCount() ? verts.GetPtr() : 0,
					normals.GetElemCount() ? normals.GetPtr() : 0,
					tangents.GetElemCount() ? tangents.GetPtr() : 0,
					binormals.GetElemCount() ? binormals.GetPtr() : 0,
					colors.GetElemCount() ? colors.GetPtr() : 0,
					uvs.GetElemCount() ? uvs.GetPtr() : 0,
					skinBoneIds.GetElemCount() ? skinBoneIds.GetPtr() : 0,
					skinWeights.GetElemCount() ? skinWeights.GetPtr() : 0,
					curRange.start, vertexCount, curRange.count );
#endif
				// go through each vertex in the range and check for min/max
				// index.
				curRange.minIndex = 0xFFFF;
				curRange.maxIndex = 0;

				// go through all indices in the range and calculate the min/max.
				unsigned int idx = curRange.start;
				for ( unsigned int j = 0; j < curRange.count; ++j, ++idx )
				{
					curRange.minIndex = Min( indices[ idx ], ( unsigned short )curRange.minIndex );
					curRange.maxIndex = Max( indices[ idx ], ( unsigned short )curRange.maxIndex );
					B_ASSERT( curRange.maxIndex < vertexCount );
				}
			}

			unsigned int curMeshCount = ( unsigned int )meshes.size();
			tstring curMeshName = name;
			curMeshName += "_";
			curMeshName << curMeshCount;

			B_ASSERT( vertexCount > 0 );

			// create an indexed mesh.
			URef< GrMesh > newMesh = new GrMesh(
				curMeshName,
				( verts.GetElemCount() > 0 ) ? verts.GetPtr() : 0,
				( normals.GetElemCount() > 0 ) ? normals.GetPtr() : 0,
				( binormals.GetElemCount() > 0 ) ? binormals.GetPtr() : 0,
				( tangents.GetElemCount() > 0 ) ? tangents.GetPtr() : 0,
				( colors.GetElemCount() > 0 ) ? colors.GetPtr() : 0,
				( uvs.GetElemCount() > 0 ) ? uvs.GetPtr() : 0,
				vertexCount,
				( indices.GetElemCount() > 0 ) ? indices.GetPtr() : 0,
				( unsigned short )indices.GetElemCount(),
				( rangeVec.GetElemCount() > 0 ) ? rangeVec.GetPtr() : 0,
				( unsigned int )( rangeVec.GetElemCount() ),
				curUberTex,
				vboEnable );

			// now create skinning info if needed.
			if ( _skinned )
			{
				// there should be one skin vertex for every vertex.
				B_ASSERT( skinWeights.GetElemCount() == verts.GetElemCount() );
				B_ASSERT( skinBoneIds.GetElemCount() == verts.GetElemCount() );

				// create the skin and assign it to the mesh.
				GrSkin* skin = new GrSkin( curMeshName + "_skin", newMesh->Clone(),
					skinWeights.GetElemCount() ? skinWeights.GetPtr() : 0,
					skinBoneIds.GetElemCount() ? skinBoneIds.GetPtr() : 0,
					_boneNameArray, _invBindMatrices, _boneCount );
				newMesh->SetSkin( skin );
			}

			// create the new mesh instance with an identity transform and add
			// it to our list of meshes.
			GrMeshInst* meshInst = new GrMeshInst( newMesh, rangeMaterials.GetPtr(), MMat4x4() );
			meshes.push_back( meshInst );

		} while ( polyIter != polyEnd );
	}
}

//----------------------------------------------------------
unsigned int
GrPolygonGroup::IndexifyMesh( UFastArray< unsigned short >& indices, SVec3* positions, SVec3* normals,
							  SVec3* biNormals, SVec3* tangents, unsigned long* colors, SVec2* texCoords,
							  unsigned int* skinBoneIds, SVec3* skinWeights, unsigned int startVertex,
							  unsigned int dstVertex, unsigned int vertexCount ) const
{
#if 1
	B_ASSERT( dstVertex <= startVertex );

	// build a list of all duplicate vertices.  Initialize indices to -1.
	int *duplicates = new int[ vertexCount ];
	for ( unsigned int i = 0; i < vertexCount; ++i )
		duplicates[ i ] = -1;

	// for each vertex, find duplicates.
	unsigned int endVertex = startVertex + vertexCount;
	for ( unsigned int i = startVertex; i < endVertex; ++i )
	{
		// if current vertex is a duplicate of something else, then ignore it.
		if ( duplicates[ i - startVertex ] != -1 )
			continue;

		// check all remaining verts to see if it's a duplicate.
		for ( unsigned int j = i + 1; j < endVertex; ++j )
		{
			// if the current vertex is a duplicate of another vert, then skip it.
			if ( duplicates[ j - startVertex ] != -1 )
				continue;

			// check to see if it is equal to the current vertex.
			if ( !IsPositionEqual( positions[ i ], positions[ j ] ) )
				continue;

			if ( normals && !IsNormalEqual( normals[ i ], normals[ j ] ) )
				continue;

			if ( biNormals && !IsTangentEqual( biNormals[ i ], biNormals[ j ] ) )
				continue;

			if ( tangents && !IsTangentEqual( tangents[ i ], tangents[ j ] ) )
				continue;

			if ( colors && colors[ i ] != colors[ j ] )
				continue;

			if ( texCoords && !IsUVEqual( texCoords[ i ], texCoords[ j ] ) )
				continue;

			if ( skinBoneIds && skinBoneIds[ i ] != skinBoneIds[ j ] )
				continue;

			if ( skinWeights && skinWeights[ i ] != skinWeights[ j ] )
				continue;

			// if we get here, then all things are equal!
			duplicates[ j - startVertex ] = i;
		}
	}

	// create a remapping array that can be used to map a vertex's old index to it's
	// new index.
	unsigned int dstAdj = startVertex - dstVertex;
	unsigned int adjustCount = 0;
	int* curDuplicate = duplicates;
	for ( unsigned int i = startVertex; i < endVertex; ++i, ++curDuplicate )
	{
		if ( *curDuplicate != -1 )
		{
			// remove the current vertex.
			++adjustCount;
			B_ASSERT( *curDuplicate < ( int )i );
			indices[ i ] = indices[ *curDuplicate ];
			B_ASSERT( indices[ i ] < endVertex );
		}
		else
		{
			indices[ i ] = ( unsigned short )( i - adjustCount - dstAdj );
			B_ASSERT( indices[ i ] < endVertex );
		}
	}

	// adjustCount now contains the number of duplicates.  _vertexCount - adjustCount
	// is now the total number of vertices used.
	unsigned int newVertexCount = vertexCount - adjustCount;
	B_ASSERT( newVertexCount > 0 );

	// copy unique vertices to the new arrays.
	unsigned int lastVertex = dstVertex + newVertexCount;
	for ( unsigned int i = startVertex; i < endVertex; ++i )
	{
		if ( duplicates[ i - startVertex ] == -1 )
		{
			B_ASSERT( indices[ i ] < ( unsigned short )lastVertex );
			positions[ indices[ i ] ] = positions[ i ];
			if ( normals )
				normals[ indices[ i ] ] = normals[ i ];
			if ( tangents )
				tangents[ indices[ i ] ] = tangents[ i ];
			if ( biNormals )
				biNormals[ indices[ i ] ] = biNormals[ i ];
			if ( colors )
				colors[ indices[ i ] ] = colors[ i ];
			if ( texCoords )
				texCoords[ indices[ i ] ] = texCoords[ i ];
			if ( skinBoneIds )
				skinBoneIds[ indices[ i ] ] = skinBoneIds[ i ];
			if ( skinWeights )
				skinWeights[ indices[ i ] ] = skinWeights[ i ];
		}
	}

	// free temporary memory.
	delete[] duplicates;

	// return the new vertex count.
	return newVertexCount;
#else
#pragma message ( "Performance warning... mesh indexing is disabled." )

	// lets be a jackass and not really index anything.
	for ( unsigned int i = 0; i < vertexCount; ++i )
		indices.push_back( i );
	return vertexCount;
#endif
}

//----------------------------------------------------------
void
GrPolygonGroup::Copy( const GrPolygonGroup& group )
{
	_aaBox = group._aaBox;
	_vertexCount = group._vertexCount;
	_aaBoxDirty = group._aaBoxDirty;
	_skinned = group._skinned;

	delete[] _boneNameArray;
	delete[] _invBindMatrices;
	if ( _skinned )
	{
		_boneCount = group._boneCount;
		_boneNameArray = new tstring[ _boneCount ];
		_invBindMatrices = new MMat4x4[ _boneCount ];
		for ( unsigned int i = 0; i < _boneCount; ++i )
		{
			_boneNameArray[ i ] = group._boneNameArray[ i ];
			_invBindMatrices[ i ] = group._invBindMatrices[ i ];
		}
	}
	else
	{
		_boneCount = 0;
		_boneNameArray = 0;
		_invBindMatrices = 0;
	}

	_polygons.reserve( group._polygons.size() );
	unsigned int polyCount = group.GetPolygonCount();
	for ( unsigned int i = 0; i < polyCount; ++i )
		_polygons.push_back( new GrPolygon( *group._polygons[ i ] ) );
}

//----------------------------------------------------------
void
GrPolygonGroup::EnsureAABox() const
{
	if ( !_aaBoxDirty )
		return;

	// clear the dirty flag and calculate the bounding box.
	_aaBoxDirty = false;

	// simply set the AA box to the origin if we have no polygons.
	if ( _polygons.size() == 0 )
	{
		_aaBox = MAABox();
		return;
	}

	// for each polygon
	//  for each vertex
	//   check min and max
	// pos = avg min max
	SVec3 maxExts( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	SVec3 minExts( FLT_MAX, FLT_MAX, FLT_MAX );
	PolygonVec::const_iterator iter = _polygons.begin();
	const PolygonVec::const_iterator end = _polygons.end();
	for ( ; iter != end; ++iter )
	{
		// take the average normal and average position.
		unsigned int vertexCount = ( *iter )->GetVertexCount();
		for ( unsigned int i = 0; i < vertexCount; ++i )
		{
			maxExts.x = Max( maxExts.x, ( *iter )->GetVertex( i ).position.x );
			maxExts.y = Max( maxExts.y, ( *iter )->GetVertex( i ).position.y );
			maxExts.z = Max( maxExts.z, ( *iter )->GetVertex( i ).position.z );
			minExts.x = Min( minExts.x, ( *iter )->GetVertex( i ).position.x );
			minExts.y = Min( minExts.y, ( *iter )->GetVertex( i ).position.y );
			minExts.z = Min( minExts.z, ( *iter )->GetVertex( i ).position.z );
		}
	}

	// figure out AA box info.
	_aaBox.SetCenter( 0.5f * ( maxExts + minExts ) );
	_aaBox.SetHalfExts(
		MVec3(
			   0.5f * ( maxExts.x - minExts.x ),
			   0.5f * ( maxExts.y - minExts.y ),
			   0.5f * ( maxExts.z - minExts.z ) 
			 )
		);
}

//----------------------------------------------------------
bool
GrPolygonGroup::GenPolygonTSBasis( MVec3& tangentOut, MVec3& binormalOut, const GrPolygon& polygon )
{
	// find the first vertex we can use to calculate the TSB.
	unsigned int polyVertexCount = polygon.GetVertexCount();
	unsigned int startIdx = 0;
	for ( ; startIdx < polyVertexCount; ++startIdx )
	{
		// find a vertex we can actually start from.
		const SVec3& v0 = polygon.GetVertex( startIdx ).position;
		const SVec3& v1 = polygon.GetVertex( ( startIdx + 1 ) % polyVertexCount ).position;
		const SVec3& v2 = polygon.GetVertex( ( startIdx + 2 ) % polyVertexCount ).position;

		// take the cross product.
		MVec3 normal = ( v2 - v1 ).Cross( v1 - v0 );
		if ( normal.Mag() > MVec3::kNormalizeEpsilon )
		{
			// calculate the tangents.
			const SVec2& tc0 = polygon.GetVertex( startIdx ).uv;
			const SVec2& tc1 = polygon.GetVertex( ( startIdx + 1 ) % polyVertexCount ).uv;
			const SVec2& tc2 = polygon.GetVertex( ( startIdx + 2 ) % polyVertexCount ).uv;
			SVec3 tangentOutTemp;
			SVec3 binormalOutTemp;
			GrCalcTangents( tangentOutTemp, binormalOutTemp, v0, v1, v2, tc0, tc1, tc2, true );
			tangentOut = tangentOutTemp;
			binormalOut = binormalOutTemp;

			// all done.
			break;
		}
	}

	// return success if we found a triangle in the polygon that we can use.
	return startIdx != polyVertexCount;
}

//----------------------------------------------------------
MPlane
GrPolygonGroup::CalcSplitPlane() const
{
	// calculate the centroid of all polygons.
	MVec3 centroid;
	unsigned int polyCount = ( unsigned int )_polygons.size();
	for ( unsigned int i = 0; i < polyCount; ++i )
	{
		const GrPolygon* curPoly = _polygons[ i ];
		unsigned int vertCount = curPoly->GetVertexCount();
		for ( unsigned int j = 0; j < vertCount; ++j )
			centroid += curPoly->GetVertex( j ).position;
	}
	centroid *= 1.0f / ( float )GetVertexCount();

	// initialize the split plane to split along Z.
	const MAABox& aaBox = GetAABox();
	MPlane plane = MPlane( MVec3( 0.0f, 0.0f, 1.0f ), centroid );

	// determine if X and Y are longer than Z.  If so, split by one
	// of them.
	const MVec3& halfExts = aaBox.GetHalfExts();
	if ( halfExts.GetX() > halfExts.GetY() )
	{
		if ( halfExts.GetX() > halfExts.GetZ() )
		{
			// X.
			plane = MPlane( MVec3( 1.0f, 0.0f, 0.0f ), centroid );
		}
	}
	else
	{
		if ( halfExts.GetY() > halfExts.GetZ() )
		{
			// Y.
			plane = MPlane( MVec3( 0.0f, 1.0f, 0.0f ), centroid );
		}
	}

	// return the calculate split plane.
	return plane;
}
/*
//----------------------------------------------------------
void
GrPolygonGroup::CheckAABox( const GrPolygon& polygon )
{
	float halfWidth = _aaBox.width * 0.5f;
	float halfHeight = _aaBox.height * 0.5f;
	float halfDepth = _aaBox.depth * 0.5f;
	SVec3 maxExts = SVec3( _aaBox.pos.x + halfWidth, _aaBox.pos.y + halfHeight, _aaBox.pos.z + halfDepth );
	SVec3 minExts = SVec3( _aaBox.pos.x - halfWidth, _aaBox.pos.y - halfHeight, _aaBox.pos.z - halfDepth );
	unsigned int vertexCount = polygon.GetVertexCount();
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		maxExts.x = Max( maxExts.x, polygon.GetVertex( i ).position.x );
		maxExts.y = Max( maxExts.y, polygon.GetVertex( i ).position.y );
		maxExts.z = Max( maxExts.z, polygon.GetVertex( i ).position.z );
		minExts.x = Min( minExts.x, polygon.GetVertex( i ).position.x );
		minExts.y = Min( minExts.y, polygon.GetVertex( i ).position.y );
		minExts.z = Min( minExts.z, polygon.GetVertex( i ).position.z );
	}

	_aaBox.pos = 0.5f * maxExts + 0.5f * minExts;
	_aaBox.width = maxExts.x - minExts.x;
	_aaBox.height = maxExts.y - minExts.y;
	_aaBox.depth = maxExts.z - minExts.z;
}
*/
//----------------------------------------------------------
int
PolygonCmp( const SSimplePolygon* left, const SSimplePolygon* right )
{
	// return -1 if left has less height than right.
	if ( left->texelHeight < right->texelHeight )
		return -1;
	else if ( left->texelHeight > right->texelHeight )
		return 1;

	// don't care if they're equal.
	return 0;
}

//----------------------------------------------------------
bool
PlaneSortCmp( const std::pair< float, MPlane >& left, const std::pair< float, MPlane >& right )
{
	const float kClassArea = 300.0f;

	// smallest class to largest class, with largest to smallest
	// in each class.
//	int leftClass = ( int )Floor( left.first / kClassArea );
//	int rightClass = ( int )Floor( right.first / kClassArea );

	// largest to smallest with in each class.
//	if ( leftClass == rightClass )
//		return left.first > right.first;

	// smallest class first.
//	return leftClass < rightClass;
	return left.first > right.first;
}

//----------------------------------------------------------
unsigned __int64
CalcEdgeHash( const SVec3& v0, const SVec3& v1 )
{
	// hash the data as if we're hashing a string.
	__int64 hash = 0;
	hash = HashMem64( ( const char* )&v0, sizeof( SVec3 ) );
	hash = HashMem64( ( const char* )&v1, sizeof( SVec3 ), hash );
	return hash;
}

//----------------------------------------------------------
inline bool
IsPositionEqual( const SVec3& lhs, const SVec3& rhs )
{
#if USE_APPROX_COMPARE
	return ( TO_FIXED_10( lhs.x - rhs.x ) == 0 &&
			 TO_FIXED_10( lhs.y - rhs.y ) == 0 &&
			 TO_FIXED_10( lhs.z - rhs.z ) == 0 );
#else
	return ( lhs == rhs );
#endif
}

//----------------------------------------------------------
inline bool
IsNormalEqual( const SVec3& lhs, const SVec3& rhs )
{
#if USE_APPROX_COMPARE
	return ( TO_FIXED_8( lhs.x - rhs.x ) == 0 &&
			 TO_FIXED_8( lhs.y - rhs.y ) == 0 &&
			 TO_FIXED_8( lhs.z - rhs.z ) == 0 );
#else
	return ( lhs == rhs );
#endif	
}

//----------------------------------------------------------
inline bool
IsTangentEqual( const SVec3& lhs, const SVec3& rhs )
{
#if USE_APPROX_COMPARE
	return ( TO_FIXED_8( lhs.x - rhs.x ) == 0 &&
			 TO_FIXED_8( lhs.y - rhs.y ) == 0 &&
			 TO_FIXED_8( lhs.z - rhs.z ) == 0 );
#else
	return ( lhs == rhs );
#endif	
}

//----------------------------------------------------------
inline bool
IsUVEqual( const SVec2& lhs, const SVec2& rhs )
{
#if USE_APPROX_COMPARE
	return ( TO_FIXED_18( lhs.x - rhs.x ) == 0 &&
			 TO_FIXED_18( lhs.y - rhs.y ) == 0 );
#else
	return ( lhs == rhs );
#endif
}
