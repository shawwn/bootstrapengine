//----------------------------------------------------------
// File:		GrMeshInst.h
// Author:		Kevin Bray
// Created:		10-04-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMeshInst.h"

// project includes.
#include "MVec3.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrTextureMgr.h"
#include "GrImage.h"
#include "GrMaterial.h"
#include "GrMaterialMgr.h"
#include "GLSubsys.h"
#include "GrTextureCube.h"
#include "GrTexGen.h"
#include "GrUtil.h"
#include "GrDeformer.h"
#include "GrSkinInst.h"
#include "GrStreamingIB.h"
#include "GrMaterialPass.h"
#include "MPlane.h"
#include "MRay.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"


//**********************************************************
// class GrMeshInst
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMeshInst::GrMeshInst( UReader& reader, const UFastArray< URef< GrMesh > >& meshArray )
: _mesh( 0 )
, _skinInst( 0 )
, _deformedMeshes( 0 )
, _materials( 0 )
, _lastRenderTime( 0 )
{
	Load( reader, meshArray );
}

//----------------------------------------------------------
GrMeshInst::GrMeshInst( UReader& reader, URef< GrMesh > mesh )
: _mesh( 0 )
, _skinInst( 0 )
, _deformedMeshes( 0 )
, _materials( 0 )
, _lastRenderTime( 0 )
{
	// create an array and then load.
	UFastArray< URef< GrMesh > > meshArray;
	meshArray.Push( mesh );
	Load( reader, meshArray );
}

//----------------------------------------------------------
GrMeshInst::GrMeshInst( URef< GrMesh > mesh, const URef< GrMaterial >* materials, const MMat4x4& transform )
: _mesh( mesh )
, _skinInst( 0 )
, _deformedMeshes( 0 )
, _materials( 0 )
, _lastRenderTime( 0 )
{
	// make sure everything passed in is okay.
	B_ASSERT( mesh != 0 );
	B_ASSERT( materials != 0 );

	// copy the material pointers.
	unsigned int rangeCount = mesh->GetRangeCount();
	_materials = new URef< GrMaterial >[ rangeCount ];
	for ( unsigned int i = 0; i < rangeCount; ++i )
	{
		B_ASSERT( materials[ i ] != 0 );
		_materials[ i ] = materials[ i ];
	}

	// allocate space for bounding volumes.
	_rangeBounds = new SBoundsInfo[ rangeCount ];

	// deformer results.
	_deformedMeshes = new URef< GrMesh >[ rangeCount ];

	// allocate room for any reflection textures.
//	_reflections = new URef< GrTextureBase >[ rangeCount ];

	if ( mesh->GetSkin() != 0 )
		_skinInst = new GrSkinInst( this );

	UpdateBounds();
	UpdateGenTextures();

	// get the scale.
//	MVec3 scale;
//	_localTransform.GetScale( scale );

	// make a uniform scale for the bounding volume.
//	_scale = Max( Max( scale.GetX(), scale.GetY() ), scale.GetZ() );
}

//----------------------------------------------------------
GrMeshInst::~GrMeshInst()
{
//	if ( _envTexCube != 0 )
//		gGrTexGen->RemoveEnvMap( this );

//	for ( unsigned int i = 0; i < GetRangeCount(); ++i )
//	{
//		if ( _reflections[ i ] != 0 )
//			gGrTexGen->RemoveReflection( this, ( int )i );
//	}

	delete[] _deformedMeshes;
	delete[] _materials;
//	delete[] _reflections;
	delete[] _rangeBounds;
	delete _skinInst;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrMeshInst*
GrMeshInst::Clone( bool deep )
{
	// create a new mesh instance.
	GrMeshInst* newMeshInst = 0;

	// we need to clone a mesh if it has skinning data associated with it.
	if ( _mesh->GetSkin() != 0 || deep )
		newMeshInst = new GrMeshInst( _mesh->Clone(), _materials, _transform );
	else
		newMeshInst = new GrMeshInst( _mesh, _materials, _transform );

	// return the new mesh instance.
	return newMeshInst;
}

//----------------------------------------------------------
void
GrMeshInst::Cache()
{
	_mesh->Cache();
	for ( unsigned int i = 0; i < GetRangeCount(); ++i )
	{
		if ( _deformedMeshes[ i ] != 0 )
			_deformedMeshes[ i ]->Cache();
	}
}

//----------------------------------------------------------
void
GrMeshInst::Evict()
{
	_mesh->Evict();
	for ( unsigned int i = 0; i < GetRangeCount(); ++i )
	{
		if ( _deformedMeshes[ i ] != 0 )
			_deformedMeshes[ i ]->Evict();
	}
}

//----------------------------------------------------------
void
GrMeshInst::SetMesh( GrMesh *mesh, const URef< GrMaterial >* materials )
{
	// testme.
	B_ASSERT( false );

	// create skin instance data if the mesh is skinned.	
	delete _skinInst;
	_skinInst = 0;
	if ( mesh->GetSkin() != 0 )
		_skinInst = new GrSkinInst( this );

	// copy the material info and assign the new mesh.
	unsigned int rangeCount = mesh->GetRangeCount();
	delete[] _materials;
	_materials = new URef< GrMaterial >[ rangeCount ];
	for ( unsigned int i = 0; i < rangeCount; ++i )
	{
		_materials[ i ] = materials[ i ];
	}
	_mesh = mesh;

	// allocate room for reflections.
//	delete[] _reflections;
//	_reflections = new URef< GrTextureBase >[ rangeCount ];

	// update the bounding volumes.
	UpdateBounds();
}

//----------------------------------------------------------
void
GrMeshInst::SetMaterial( unsigned int range, URef< GrMaterial > material )
{
	B_ASSERT( range < GetRangeCount() && material != 0 );
	_materials[ range ] = material;
	if ( material->HasDeformers() )
		_deformedMeshes[ range ] = _mesh->CloneRange( range );
	else
		_deformedMeshes[ range ] = 0;

	UpdateRangeGenTextures( range );
}

//----------------------------------------------------------
void
GrMeshInst::SetTransform( const MMat4x4& transform )
{
	_transform = transform;
	bool valid = _transform.Inverse( _invTransform );
	B_ASSERT( valid );
	valid = _transform.GetRotate().InverseTransposed( _rotInvTranspose );
	B_ASSERT( valid );

	// update the bounding volumes.
	UpdateBounds();
}

//----------------------------------------------------------
GrMaterial*
GrMeshInst::GetMaterial( unsigned int range ) const
{
	B_ASSERT( range < GetRangeCount() );
	return _materials[ range ];
}

//----------------------------------------------------------
const MSphere&
GrMeshInst::GetBoundingSphere() const
{
	return _bounds.sphere;
}

//----------------------------------------------------------
const MSphere&
GrMeshInst::GetRangeBoundingSphere( unsigned int range ) const
{
	B_ASSERT( range < GetRangeCount() );

	// return a new sphere, transformed into world space.
	return _rangeBounds[ range ].sphere;
}

//----------------------------------------------------------
const MOBox&
GrMeshInst::GetRangeOBox( unsigned int range ) const
{
	B_ASSERT( range < GetRangeCount() );
	return _rangeBounds[ range ].orientedBox;
}

//----------------------------------------------------------
void
GrMeshInst::GetRangePlane( MPlane& plane, MVec3& pointOnPlane, unsigned int range ) const
{
	B_ASSERT( range < GetRangeCount() );

	// get the plane on the mesh.
	MVec3 normal;
	_mesh->GetRangePlane( normal, pointOnPlane, range );

	// build a plane and transform it in to world space.
	plane = MPlane( normal, pointOnPlane );
	plane.TransformFast( _transform, _rotInvTranspose );

	// transform the point on the plane into world space.
	_transform.TransformCoord( pointOnPlane );
}
/*
//----------------------------------------------------------
void
GrMeshInst::ChangeEnvTexCube( URef< GrTextureBase > texture ) const
{
	_envTexCube = texture;
}

//----------------------------------------------------------
void
GrMeshInst::ChangeReflection( unsigned int range, URef< GrTextureBase > texture ) const
{
	B_ASSERT( range < GetRangeCount() );
	_reflections[ range ] = texture;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrMeshInst::GetEnvTexCube() const
{
	return _envTexCube;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrMeshInst::GetReflection( unsigned int range ) const
{
	B_ASSERT( range < GetRangeCount() );
	return _reflections[ range ];
}
*/
//----------------------------------------------------------
void
GrMeshInst::Save( UWriter& writer ) const
{
	writer.WriteInt( 'IHSM' );

	// save the name of the mesh we're bound to.
	writer.WriteString( _mesh->GetName() );

	// save the range count.
	unsigned int rangeCount = GetRangeCount();
	writer.WriteInt( ( int )rangeCount );

	// save material data.
	for ( unsigned int i = 0; i < rangeCount; ++i )
	{
		// write the current material.
		SaveMaterial( writer, i );
	}

	// save transforms.
	_transform.Save( writer );
}

//----------------------------------------------------------
void
GrMeshInst::Load( UReader& reader, const UFastArray< URef< GrMesh > >& meshArray )
{
	// clean up.
	delete[] _materials;
//	delete[] _reflections;
	delete[] _deformedMeshes;
	delete _skinInst;
	_skinInst = 0;

	unsigned long tag = reader.ReadInt();
	B_ASSERT( tag == 'IHSM' );

	unsigned int meshListSize = meshArray.GetElemCount();

	// read the name of the mesh we're bound to.  Find the mesh
	// in the mesh list.
	tstring name = reader.ReadString();
	unsigned int i = 0;
	for ( ; i < meshListSize; ++i )
	{
		if ( meshArray[ i ]->GetName() == name )
		{
			_mesh = meshArray[ i ];
			break;
		}
	}

	// the mesh we're linked to wasn't found!
	B_ASSERT( i != meshListSize );
	if ( _mesh->GetSkin() != 0 )
		_skinInst = new GrSkinInst( this );

	// read the range count.
	unsigned int rangeCount = ( unsigned int )reader.ReadInt();
	_deformedMeshes = new URef< GrMesh >[ rangeCount ];

	// read material data.
	CHECK_GL();
	_materials = new URef< GrMaterial >[ rangeCount ];
	for ( unsigned int mat = 0; mat < rangeCount; ++mat )
	{
		// read in the current material.
		LoadMaterial( reader, mat );
	}
	CHECK_GL();

	// allocate space for bounds.
	_rangeBounds = new SBoundsInfo[ rangeCount ];

	// allocate space for reflections.
//	_reflections = new URef< GrTextureBase >[ rangeCount ];

	// read transforms.
	_transform.Load( reader );

	// initialize the OBB.
	UpdateBounds();
	UpdateGenTextures();
}

//----------------------------------------------------------
void
GrMeshInst::Update( unsigned int frameTime )
{
	// check to see if we need to perform an update.
	if ( frameTime > _lastRenderTime )
	{
		_lastRenderTime = frameTime;

		// update skin and bounds if necessary.
		if ( _skinInst )
		{
			_skinInst->ApplySkin();
			UpdateBounds();
		}

		// update ranges if necessary.
		unsigned int rangeCount = GetRangeCount();
		for ( unsigned int i = 0; i < rangeCount; ++i )
		{
			GrMesh* srcMesh = _mesh;
			unsigned int srcRange = i;
			const GrMaterial::DeformerArray& deformers = _materials[ i ]->GetDeformers();
			for ( unsigned int deformerIdx = 0; deformerIdx < deformers.GetElemCount(); ++deformerIdx )
			{
				deformers[ deformerIdx ]->Deform( _deformedMeshes[ i ], srcMesh, srcRange, frameTime );
				srcMesh = _deformedMeshes[ i ];
				srcRange = 0;
			}
		}
	}
}

//----------------------------------------------------------
bool
GrMeshInst::Pick( float& dist, const MRay& ray ) const
{
	// try to intersect with the mesh.
	GrMesh::STriCollision triHit;
	if ( !Pick( triHit, ray ) )
		return false;

	// calculate the distance and return.
	dist = ray.GetStart().Dist( triHit.hitLoc );

	// return true to indicate a hit was made.
	return true;
}

//----------------------------------------------------------
bool
GrMeshInst::Pick( GrMesh::STriCollision& triHit, const MRay& ray ) const
{
	// if we don't intersect with our obox, then we know we don't
	// intersect with the mesh.
	if ( !ray.Intersect( _bounds.orientedBox ) )
		return false;

	// transform the ray into local space.
	MRay localRay = ray.Transform( _invTransform, _rotInvTranspose.Transposed() );

	// now perform the pick operation.
	GrMesh::TriHitArray triHits;
	_mesh->Pick( triHits, localRay );

	// simply return if there were no hits.
	if ( triHits.GetElemCount() == 0 )
		return false;

	// go through the tri hits and take the closest one to the ray's origin.
	float minDistSqr = FLT_MAX;
	for ( unsigned int i = 0; i < triHits.GetElemCount(); ++i )
	{
		// calculate the distance between ray hits
		GrMesh::STriCollision& curTriHit = triHits[ i ];
		float curDistSqr = ray.GetStart().DistSqr( curTriHit.hitLoc );

		// take the minimum and store the tri-hit info.
		if ( curDistSqr < minDistSqr )
		{
			minDistSqr = curDistSqr;
			triHit = curTriHit;
		}
	}

	// move the hit location into world space.
	_transform.TransformCoordFast( triHit.hitLoc );
	_rotInvTranspose.RotatePointFast( triHit.hitNormal );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void
GrMeshInst::RenderRange( unsigned int range, unsigned int vertexComponents, unsigned int frameTime )
{
	// updates each range if needed.
	B_ASSERT( range < GetRangeCount() );

	// make sure we've been updated.
	Update( frameTime );

	// render the mesh.
	if ( _deformedMeshes[ range ] )
	{
		// draw the deformed mesh if one exists.
		_deformedMeshes[ range ]->BeginRender( vertexComponents );
		_deformedMeshes[ range ]->Render( 0, false );
	}
	else
	{
		// no deformer, so draw the original mesh.
		_mesh->BeginRender( vertexComponents );
		_mesh->Render( range, false );
	}
}

//----------------------------------------------------------
unsigned int
GrMeshInst::BatchRange( GrStreamingIB& streamingIB, unsigned int range, unsigned int frameTime )
{
	// updates each range if needed.
	B_ASSERT( range < GetRangeCount() );

	// make sure we've been updated.
	Update( frameTime );

	// copy the indices.
	const GrMesh::SRange& curRange = _mesh->GetRange( range );
	streamingIB.AddIndices( ( _mesh->GetIndexData()->GetIndices() + curRange.start ), curRange.count );

	// return the number of indices in the range we just batched.
	return curRange.count;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMeshInst::SaveMaterial( UWriter& writer, unsigned int range ) const
{
	// save the material set name and the material name.
	// TODO: remove the set name from the .BSP file!
	writer.WriteString( "" );	// NO LONGER USED.
	writer.WriteString( GetMaterial( range )->GetName() );
}

//----------------------------------------------------------
void
GrMeshInst::LoadMaterial( UReader& reader, unsigned int range )
{
	// HACK!  TODO:  ONLY STORE A SINGLE NAME IN THE BSP FILE IN THE
	// FORMAT REQUIRED BY THE RESOURCE SUBSYSTEM.
	reader.ReadString();

	// read in/bind the shader.
	tstring name = reader.ReadString();

	// get the material.
	tstring errors;
	URef< GrMaterial > material = gGrMaterialMgr->GetMaterial( name, errors );
	if ( material == 0 )
	{
		B_ERROR( "Unable to load material \'" + name + "\'!\nReason: " + errors );
	}

	// set the material.
	SetMaterial( range, material );
}

//----------------------------------------------------------
void
GrMeshInst::UpdateBounds()
{
	// update the bounding box.
	const MSphere& sphere = _mesh->GetBoundingSphere();
	MVec3 pos = sphere.GetCenter();

	// calculate the scale of the matrix.
	MVec3 scales;
	_transform.GetScaleSqr( scales );

	// determine the maximum scale.
	float maxScale = Sqrt( scales.Max() );

	// transform the center into world space.
	_transform.TransformCoordFast( pos );

	// return a new sphere, transformed into world space.
	_bounds.sphere.SetCenter( pos );
	_bounds.sphere.SetRadius( maxScale * sphere.GetRadius() );

	// update the bounding box's transform.
	_bounds.orientedBox = MOBox( _transform, _mesh->GetAABox() );

	// update the range bounds.
	for ( unsigned int i = 0; i < _mesh->GetRangeCount(); ++i )
	{
		float sphereRadius = _mesh->GetRangeBoundingSphere( i ).GetRadius();
		MVec3 sphereCenter = _mesh->GetRangeBoundingSphere( i ).GetCenter();
		_transform.TransformCoordFast( sphereCenter );

		_rangeBounds[ i ].sphere.SetCenter( sphereCenter );
		_rangeBounds[ i ].sphere.SetRadius( maxScale * sphereRadius );
		_rangeBounds[ i ].orientedBox = MOBox( _transform, _mesh->GetRangeAABox( i ) );
	}
}

//----------------------------------------------------------
void
GrMeshInst::UpdateRangeGenTextures( unsigned int range )
{
	// get the current material and all of it's gen texture flags.
	GrMaterial* curMaterial = _materials[ range ];
	GrTextureStage::GenTextureFlag genTextureFlags = curMaterial->GetGenTextureFlags();

	// add the reflection.
	if ( ( genTextureFlags & GrTextureStage::kGenReflection ) != 0 )
		gGrTexGen->AddReflection( this, range );
}

//----------------------------------------------------------
void
GrMeshInst::UpdateGenTextures()
{
	// iterate through materials checking for reflections and registering if needed.
	unsigned int rangeCount = _mesh->GetRangeCount();
	for ( unsigned int i = 0; i < rangeCount; ++i )
		UpdateRangeGenTextures( i );
}
