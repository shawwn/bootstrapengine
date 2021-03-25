//----------------------------------------------------------
// File:		GrMeshInst.h
// Author:		Kevin Bray
// Created:		10-04-04
//
// Purpose:		Contains mesh instance related info.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "MMat4x4.h"
#include "MMat3x3.h"
#include "URef.h"
#include "GrMesh.h"
#include "GrTextureMgr.h"
#include "MPlane.h"
#include "UFastArray.h"
#include "MOBox.h"

class GrMaterial;
class UReader;
class UWriter;
class GrScene;
class GrPolygonGroup;
class GrSkinInst;
class GrStreamingIB;
class MRay;

//**********************************************************
// class GrMeshInst
//**********************************************************
class GrMeshInst
{
public:
	// matInfo is an array of materials... one is required for each range in the mesh.
	GrMeshInst( UReader& reader, const UFastArray< URef< GrMesh > >& meshArray );
	GrMeshInst( UReader& reader, URef< GrMesh > mesh );
	GrMeshInst( URef< GrMesh > mesh, const URef< GrMaterial >* matInfo, const MMat4x4& transform );
	~GrMeshInst();

	// clones a mesh instance.  If deep is true, then the mesh and materials are also cloned.
	// Generally, this should be false.
	GrMeshInst*				Clone( bool deep );

	// caches data for rendering.
	void					Cache();
	void					Evict();

	// assignment for the mesh instance data.
	void					SetMesh( GrMesh* mesh, const URef< GrMaterial >* matInfo );
	void					SetMaterial( unsigned int range, URef< GrMaterial > material );
	void					SetTransform( const MMat4x4& transform );

	// accessors
	unsigned int			GetRangeCount() const			{	return _mesh->GetRangeCount();			}
	GrMesh*					GetMesh() const					{	return _mesh;							}
	GrMaterial*	 			GetMaterial( unsigned int range ) const;
	const MMat4x4&			GetTransform() const			{	return _transform;						}
	const MMat4x4&			GetInvTransform() const			{	return _invTransform;					}
	const MMat3x3&			GetInvTransposeRot() const		{	return _rotInvTranspose;				}

	// spatial information.
	const MSphere&			GetBoundingSphere() const;
	const MOBox&			GetOBox() const					{	return _bounds.orientedBox;				}
	const MSphere&			GetRangeBoundingSphere( unsigned int range ) const;
	const MOBox&			GetRangeOBox( unsigned int range ) const;
	void					GetRangePlane( MPlane& plane, MVec3& pointOnPlane, unsigned int range ) const;

	// skinning instance data.
	GrSkinInst*				GetSkinInst() const				{	return _skinInst;						}

	// internal use only.
//	void					ChangeEnvTexCube( URef< GrTextureBase > texture ) const;
//	void					ChangeReflection( unsigned int range, URef< GrTextureBase > texture ) const;
//	URef< GrTextureBase >	GetEnvTexCube() const;
//	URef< GrTextureBase >	GetReflection( unsigned int range ) const;

	// saving and loading.
	void					Save( UWriter& writer ) const;
	void					Load( UReader& reader, const UFastArray< URef< GrMesh > >& meshArray );

	// updates deformers and skin (if there are any).
	void					Update( unsigned int frameTime );

	// picks against the mesh.
	bool					Pick( float& dist, const MRay& ray ) const;
	bool					Pick( GrMesh::STriCollision& triHit, const MRay& ray ) const;

	// render.
	void					RenderRange( unsigned int range, unsigned int vertexComponents, unsigned int frameTime );

	// note that this is only usable if the transform is identity.
	unsigned int			BatchRange( GrStreamingIB& streamingIB, unsigned int range, unsigned int frameTime );

private:
	void					SaveMaterial( UWriter& writer, unsigned int range ) const;
	void					LoadMaterial( UReader& reader, unsigned int range );
	void					UpdateBounds();
	void					UpdateRangeGenTextures( unsigned int range );
	void					UpdateGenTextures();

	struct SBoundsInfo
	{
		MOBox orientedBox;
		MSphere sphere;
	};

	URef< GrMesh >			_mesh;				// mesh data.
	GrSkinInst*				_skinInst;			// skin instance data.
	URef< GrMaterial >*		_materials;			// material data used by shaders.
//	mutable URef< GrTextureBase >	_envTexCube;	// this is simply data tagged to an instance
//	mutable URef< GrTextureBase >*	_reflections;	// by the scene for use in the material.
	URef< GrTextureBase >	_radianceMap;
	URef< GrMesh >*			_deformedMeshes;	// there is optionally one deformed mesh per range.
	MMat4x4					_transform;
	MMat4x4					_invTransform;
	MMat3x3					_rotInvTranspose;
	SBoundsInfo				_bounds;
	SBoundsInfo*			_rangeBounds;
//	MOBox					_orientedBoxBounds;
//	MSphere					_sphereBounds;
	unsigned int			_lastRenderTime;
};
