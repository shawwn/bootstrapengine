//----------------------------------------------------------
// File:		GrPolygon.h
// Author:		Kevin Bray
// Created:		07-31-05
//
// Purpose:		To represent a convex 3D polygon.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTypes.h"
#include "MPlane.h"
#include "UFastArray.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrMaterial;
class MPlane;
class GrPolygon;
class GrUberTexture;

// polygon vertex.
struct SPolygonVertex
{
	void NormalizeComponents( unsigned int componentFlags );

	// set to a combination of the constants above.
	SVec3 position;
	SVec3 normal;
	SVec3 tangent;
	SVec3 binormal;
	unsigned char color[ 4 ];	// 0 = blue, 1 = green, 2 = red, 3 = alpha.
	SVec2 uv;

	SPolygonVertex operator + ( const SPolygonVertex& vertex );
};
SPolygonVertex operator * ( float scalar, const SPolygonVertex& vertex );

// polygon vertex skinning info.
struct SPolygonSkinVertex
{
	// note that there is currently support for only 3 bones.
	SVec3 weights;
	unsigned int boneIds;
};

// polygon triangle.
struct SPolygonTriangle
{
	SPolygonVertex v0;
	SPolygonVertex v1;
	SPolygonVertex v2;
};

// polygon triangle skinning info.
struct SPolygonTriangleSkin
{
	SPolygonSkinVertex s0;
	SPolygonSkinVertex s1;
	SPolygonSkinVertex s2;
};

//**********************************************************
// class GrPolygon
//**********************************************************
class GrPolygon
{
public:
	// valid flags.
	static const unsigned int kNormal = 0x01;
	static const unsigned int kTangent = 0x02;
	static const unsigned int kBiNormal = 0x04;
	static const unsigned int kColor = 0x08;
	static const unsigned int kUV = 0x10;
	static const unsigned int kUniqueUV = 0x20;

	enum EFLAGVALUE
	{
		EFV_EDGEPOLY = 1,		// the polygon is on the edge of a region.
	};

	enum EPLANERELATION
	{
		EPR_BACK,
		EPR_FRONT,
		EPR_SPLIT,
		EPR_COPLANAR,
	};

	// initializes completely.  Assumes ownership of pointers if arraySize is
	// not 0.
	GrPolygon();	// initializes as degenerate ( no verts ).
	GrPolygon( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount, GrMaterial* material );
	GrPolygon( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount, GrMaterial* material, const MPlane& plane );
	GrPolygon( const GrPolygon& polygon );
	GrPolygon( UReader& polygon );
	GrPolygon( unsigned int flags, const SPolygonVertex& v0, const SPolygonVertex& v1, const SPolygonVertex& v2, GrMaterial* material );

	// dtor.
	~GrPolygon();

	struct SRayIntersectInfo
	{
		unsigned int triIdx;
		SVec3 hitLoc;
		float hitDist;
		float u;
		float v;
	};

	// indicates what components of the vertices are being used ( kNormal, kTangent,
	// etc ).  Note that position is implied.
	unsigned int			GetVertexFlags() const							{	return _flags;				}
	void					SetVertexFlags( unsigned int flags )			{	_flags = flags;				}

	// sets vertices for the polygon.
	void					SetVertices( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount,
										 GrMaterial* material );
	void					SetVertices( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount,
										 GrMaterial* material, const MPlane& plane );

	// sets skinning info for the polygon.  There should be one entry for every
	// vertex in the polygon.
	void					SetSkinningInfo( SPolygonSkinVertex* skinVertArray );
	void					RemapBoneIndices( unsigned int* indices );
	bool					IsSkinned() const								{	return _skinned;			}

	// user flag management... can assign flags to various polygons.
	void					SetUserFlag( EFLAGVALUE flagValue )				{	_userFlags |= flagValue;				}
	void					ClearUserFlag( EFLAGVALUE flagValue )			{	_userFlags &= ~flagValue;				}
	unsigned int			GetUserFlags() const							{	return _userFlags;						}

	// user data get/set.
	void					SetUserData( unsigned int data )				{	_userData = data;						}
	unsigned int			GetUserData() const								{	return _userData;						}

	// vertex information.
	unsigned int			GetVertexCount() const							{	return _vertices.GetElemCount();		}
	const SPolygonVertex&	GetVertex( unsigned int i ) const			{	return _vertices[ i ];					}
	SPolygonVertex&			GetVertex( unsigned int i )						{	return _vertices[ i ];					}

	// material information.
	URef< GrMaterial >		GetMaterial() const;
	void					SetMaterial( URef< GrMaterial > material );

	// ubertexture information.
	URef< GrUberTexture >	GetUberTexture() const;
	void					SetUberTexture( URef< GrUberTexture > texture );

	// triangulates the polygon.  triangles must be large enough to hold all
	// of the triangles.  Queried the needed size by using GetTriangleCount().
	unsigned int			GetTriangleCount() const						{	return( GetVertexCount() - 2 );			}
	void					Triangulate( SPolygonTriangle* triangles, SPolygonTriangleSkin* triangleSkinArray = 0 ) const;
	void					Triangulate( SPolygonVertex* triangleList ) const;

	// returns the plane the verts lie on.
	const MPlane&			GetPlane() const								{	return _plane;							}
	const MVec3&			GetNormal() const								{	return _plane.GetNormal();				}

	// performs a fast intersection test on the polygon using the ray passed
	// in.  This is faster than iterating through triangles and using the
	// ray's test routine because it avoids a lot of SVec3->MVec3 implicit
	// conversions.
	bool					Intersect( SRayIntersectInfo& info, const SVec3& rayStart, const SVec3& rayDir, float maxDist ) const;
	float					IntersectFast( const SVec3& rayStart, const SVec3& rayDir, float maxDist ) const;

	// splits the polygon by a plane.  The return value is 1 if there is only
	// a front polygon, -1 if there is only a back polygon, and 0 if there is
	// both a front and back polygon.
	int						Split( GrPolygon& front, GrPolygon& back, const MPlane& plane, float planeDistEpsilon ) const;
	EPLANERELATION			CalcPlaneRelation( const MPlane& plane, float planeDistEpsilon ) const;
	bool					CheckTJunc( const GrPolygon& polygon, float epsilon );

	// checks to see if it is degenerate.
	bool					IsDegenerate( float epsilon ) const;
	bool					RemoveDuplicateVerts( float distEpsilonSqr );
	void					WeldVertices( const GrPolygon& polygon, float distEpsilonSqr );

	// reverses the winding order of the polygon.
	void					ReverseWinding();

	// checks to see if the polygon has an edge on the plane passed in.
	bool					IsEdgePolygon( const MPlane& plane, float planeDistEpsilon ) const;

	// checks to see if the polygon is coplanar to the plane passed in.
	bool					IsCoplanar( const MPlane& plane, float epsilon ) const;

	// returns line segments that lie on the plane passed in.
//	void					GetEdgeSegments();

	// returns true if the polygon passed in is a neighbor of this polygon.
	bool					IsNeighbor( const GrPolygon& polygon, float epsilon = 0.001f ) const;

	// transforms the polygon by the matrix specified and returns a new
	// polygon.
	GrPolygon				Transform( const MMat4x4& transform, const MMat3x3& normalRot ) const;
	void					TransformFast( const MMat4x4& transform, const MMat3x3& normalRot );

	// returns the area of the triangle.
	float					GetArea() const;

	// assignment operator.
	GrPolygon&				operator = ( const GrPolygon& polygon );

	// save/load
	void					Save( UWriter& writer ) const;
	void					Load( UReader& reader );

private:
	// intializes.
	void					Init( const SPolygonVertex* verts, unsigned int vertexCount );
	void					CalcPlane();

	// adds a vertex.
	void					AddVertex( const SPolygonVertex& vertex );

	// inserts a vertex to the polygon.  This function does not check to make
	// sure the polygon stays convex, although this behavior is required.
	void					InsertVertex( unsigned int pos, const SPolygonVertex& vertex );

	// removes a vertex from the polygon.
	void					RemoveVertex( unsigned int index );

	typedef UFastArray< SPolygonVertex, 4, 4 > VertexArray;
	typedef UFastArray< SPolygonSkinVertex, 4, 4 > SkinVertexArray;

	unsigned int			_flags;
	VertexArray				_vertices;		// vertices.
	SkinVertexArray			_skinVertArray;	// skinning info.
	URef< GrMaterial >		_material;		// polygon material.
	URef< GrUberTexture >	_uberTexture;	// polygon ubertexture.
	MPlane					_plane;			// plane the points lie on.
	unsigned int			_userFlags;		// can assign flags to the polygon.
	unsigned int			_userData;
	bool					_skinned;
};
