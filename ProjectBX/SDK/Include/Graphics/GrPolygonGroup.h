//----------------------------------------------------------
// File:		GrPolygonGroup.h
// Author:		Kevin Bray
// Created:		07-31-05
//
// Purpose:		To represent a group of polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTypes.h"
#include "MAABox.h"
#include "GrPolygon.h"

// std c++ includes.
#include <list>
#include <vector>

// forward declarations.
class GrPolygon;
class MPlane;
class GrMeshInst;
class GrTextureBase;
class MSphere;
class GrPolygonUVMapper;
//class GrCoplanarPolygonGroup;

//**********************************************************
// class GrPolygonGroup
//**********************************************************
class GrPolygonGroup
{
public:
	// note that PolygonVec must be an array of some sort.  This is
	// because we do a &_polygons.front() to get a pointer to the
	// array.  This is a blatant hack, but it gives a decent speed
	// increase when fixing t-junctions.
	typedef std::vector< GrPolygon* > PolygonVec;

	struct SEdge
	{
		unsigned int polygonIndices[ 2 ];
		unsigned int edges[ 2 ];
	};
	typedef std::vector< SEdge > EdgeVec;

	GrPolygonGroup();
	GrPolygonGroup( const PolygonVec& list );			// assumes ownership of polygons.
	GrPolygonGroup( const GrPolygonGroup& group );
	~GrPolygonGroup();

	// returns the total number of vertices in the group.
	unsigned int			GetVertexCount() const			{	return _vertexCount;							}

	// returns a volume that encompasses all polygons.
	MSphere					GetBoundingSphere() const;
	const MAABox&			GetAABox() const;

	// polygon iteration
	unsigned int			GetPolygonCount() const			{	return ( unsigned int )( _polygons.size() );	}
	const GrPolygon&		GetPolygon( unsigned int i ) const;
	void					SetPolygonMaterial( unsigned int i, URef< GrMaterial > material );
	void					SetPolygonUserData( unsigned int i, unsigned int userData );
	void					SetGroupUserData( unsigned int userData );

	// vertex component changes.
	void					SetPolygonUV( unsigned int polygonIdx, unsigned int vertexIdx, const SVec2& uv ) const;

	// sets vertex normals for polygons to the face normal.
	void					GenFaceNormals();

	// generates the tangent space basis.
	void					GenTSBasis();

	// UV coordinate generation.
	void					GenPlanarUVMap( const MPlane& projectionPlane );

	// Edge list generation.
	void					GenEdgeList( EdgeVec& edges );

	// ambient occlusion UV coordinate generation.
	void					GenAOMapping( float worldTexelSize, unsigned int textureSize );
	void					AddToUniqueUVMapper( GrPolygonUVMapper& mapper );

	// polygon management.
	void					AddPolygon( const GrPolygon& polygon );
	void					AddGroup( const GrPolygonGroup& group );
	void					AddGroup( const GrPolygonGroup& group, const MMat4x4& transform, const MMat3x3& normalRot );
	void					Clear();

	// returns true if the object is skinned.
	bool					IsSkinned() const				{	return _skinned;								}

	// adds skinning information.  There should be one entry for every polygon vertex
	// in the group.  This essentially does:
	//
	// for each polygon
	//  polygon->SetSkinningInfo( skinVertArray );
	//  skinVertArray += polygon->GetVertexCount();
	//
	// Note that this object takes ownership over the pointers passed in.
	void					SetSkinningInfo( SPolygonSkinVertex* skinVertArray, tstring* boneNames, MMat4x4* invBindMatrices, unsigned int count );

	// removes degenerate triangles and duplicate vertices.
	void					OptimizePolygons( float epsilon, float distEpsilonSqr );

	// welds vertices together.
	void					WeldVertices( float distEpsilonSqr );

	// builds a palette of planes to use for splitting from the geometry in the group.
//	void					BuildCoplanarGroups( std::vector< GrCoplanarPolygonGroup >& groups, float epsilon ) const;

	// checks the group passed in for t-junctions and fixes if needed.
	void					FixTJuncs( const GrPolygonGroup& group, float epsilon );

	// builds a palette of planes to use for splitting from the geometry in the group.
	void					GenPlaneList( std::vector< MPlane >& planes, float epsilon ) const;

	// splits the polygons into two groups using the plane passed in.
	int						Split( GrPolygonGroup& front, GrPolygonGroup& back, const MPlane& plane, float epsilon ) const;

	// classifies the polygons into two groups using the plane passed in and each
	// polygon's center of mass.
	int						Classify( GrPolygonGroup& front, GrPolygonGroup& back, const MPlane& plane ) const;

	// determine if we have a convex set of polygons.
	bool					IsConvexSet( float epsilon ) const;

	// converts into mesh(es).
	void					GenMeshes( std::vector< GrMeshInst* >& meshes, const tstring& name,
									   bool genVertexColors = false, bool vboEnable = true,
									   unsigned int maxVertexCount = 30000 ) const;

	GrPolygonGroup&			operator = ( const GrPolygonGroup& group )		{	Copy( group );	return *this;		}

private:

	// generates meshes out of the current set of polygons.
	void					GenMeshesInternal( std::vector< GrMeshInst* >& meshes, const tstring& name, bool genVertexColors = false,
											   bool vboEnable = true ) const;
	// indexifies mesh data in place and generates a list of indices.  It returns
	// the number of vertices remaining.
	unsigned int			IndexifyMesh( UFastArray< unsigned short >& indices, SVec3* positions, SVec3* normals,
										  SVec3* biNormals, SVec3* tangents, unsigned long* colors, SVec2* texCoords,
										  unsigned int* skinBoneIds, SVec3* skinWeights, unsigned int startVertex,
										  unsigned int dstVertex, unsigned int vertexCount ) const;

	void					Copy( const GrPolygonGroup& group );
	void					EnsureAABox() const;
	bool					GenPolygonTSBasis( MVec3& tangent, MVec3& binormal, const GrPolygon& polygon );
	void					OrthoNormalize( MVec3& vec, const MVec3& basis );
	MPlane					CalcSplitPlane() const;

	PolygonVec				_polygons;
	unsigned int			_vertexCount;
	mutable MAABox			_aaBox;
	mutable bool			_aaBoxDirty;

	// skinning info.
	unsigned int			_boneCount;
	tstring*				_boneNameArray;
	MMat4x4*				_invBindMatrices;

	// true if skinned.
	bool					_skinned;
};
