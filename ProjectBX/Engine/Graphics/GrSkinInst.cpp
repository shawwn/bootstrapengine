//----------------------------------------------------------
// File:		GrSkinInst.cpp
// Author:		Kevin Bray
// Created:		06-21-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSkinInst.h"

// project includes.
#include "GrSkin.h"
#include "GrMesh.h"
#include "GrMeshInst.h"
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "GrTypes.h"
#include "GrConfig.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"

#define VecMinMax( curMin, curMax, vec )	\
	curMin.x = Min( curMin.x, vec.x );		\
	curMin.y = Min( curMin.y, vec.y );		\
	curMin.z = Min( curMin.z, vec.z );		\
	curMax.x = Max( curMax.x, vec.x );		\
	curMax.y = Max( curMax.y, vec.y );		\
	curMax.z = Max( curMax.z, vec.z )

//**********************************************************
// class GrSkinInst
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSkinInst::GrSkinInst( GrMeshInst* targetMeshInst )
: _skin( 0 )
, _targetMeshInst( targetMeshInst )
, _targetMesh( 0 )
, _animatedBoneMatrices( 0 )
, _bonePalette( 0 )
, _boneRotPalette( 0 )
{
	// ensure that a valid mesh was passed in!
	B_ASSERT( targetMeshInst != 0 )
	B_ASSERT( targetMeshInst->GetMesh() != 0 )

	// store the target mesh and skin data.
	_targetMesh = targetMeshInst->GetMesh();
	_skin = _targetMesh->GetSkin();

	// ensure that the mesh passed in is skinned!
	B_ASSERT( _skin != 0 );

	// ensure that the skinning information is valid.
	B_ASSERT( _skin->GetBindMesh() != 0 );

	// invert all of the modelToBone matrices as they're currently set to
	// their bind pose (bone->model... we want model->bone).
	unsigned int boneCount = _targetMesh->GetSkin()->GetBoneCount();
	_animatedBoneMatrices = new MMat4x4[ boneCount ];
	_bonePalette = new MMat4x4[ boneCount ];
	_boneRotPalette = new MMat3x3[ boneCount ];

	for ( unsigned int i = 0; i < boneCount; ++i )
		_targetMesh->GetSkin()->GetInvBindMatrix( i ).Inverse( _animatedBoneMatrices[ i ] );
}

//----------------------------------------------------------
GrSkinInst::~GrSkinInst()
{
	delete[] _animatedBoneMatrices;
	delete[] _bonePalette;
	delete[] _boneRotPalette;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrSkinInst::Pick( GrMesh::TriHitArray& triHits, const MRay& ray ) const
{
	_targetMesh->Pick( triHits, ray );
}

//----------------------------------------------------------
unsigned int
GrSkinInst::GetBoneCount() const
{
	B_ASSERT( _skin != 0 )
	return _skin->GetBoneCount();
}

//----------------------------------------------------------
const tstring&
GrSkinInst::GetBoneName( unsigned int id ) const
{
	B_ASSERT( _skin != 0 )
	return _skin->GetBoneName( id );
}

//----------------------------------------------------------
unsigned int
GrSkinInst::GetBoneId( const char* name ) const
{
	B_ASSERT( _skin != 0 )
	return _skin->GetBoneId( name );
}

//----------------------------------------------------------
void
GrSkinInst::SetBoneMatrix( unsigned int boneId, const MMat4x4& matrix )
{
	B_ASSERT( _skin != 0 );
	B_ASSERT( boneId < _skin->GetBoneCount() );

	// mark bones as dirty.  If we really want, we can keep a per
	// bone dirty flag, but this generally won't do jack since all
	// of the bones will most likely get changed and set once per
	// frame before we perform the skinning anyways.
	_dirty = true;

	// set the matrix.
	_animatedBoneMatrices[ boneId ] = matrix;
}

//----------------------------------------------------------
void
GrSkinInst::ApplySkin()
{
	B_ASSERT( _skin != 0 );
	B_ASSERT( _targetMesh != 0 );

	// update our bone matrices if needed.
	if ( _dirty )
	{
		// update our bone matrices by concatenating the inverse
		// bind pose matrix and the animated bone matrices.  This
		// has the effect of moving vertices into bone space, then
		// animating them back into model space.
		unsigned int boneCount = _skin->GetBoneCount();
		for ( unsigned int i = 0; i < boneCount; ++i )
		{
			// calculate the matrix for animating the vertex
			// position.
			_bonePalette[ i ] = _animatedBoneMatrices[ i ] * _skin->GetInvBindMatrix( i );

			// calculate the matrix for rotating the tangent space
			// basis.
			_bonePalette[ i ].GetRotate( _boneRotPalette[ i ] );
			_boneRotPalette[ i ].Inverse();
			_boneRotPalette[ i ].Transpose();
		}

		// we're no longer dirty, so clear the dirty flag.
		_dirty = false;
	}

	// min/max of skinned vertices
	SVec3 posMin( FLT_MAX, FLT_MAX, FLT_MAX );
	SVec3 posMax(-FLT_MAX,-FLT_MAX,-FLT_MAX );

	// get source vertex data.
	SVec3* srcVerts = _skin->GetBindMesh()->GetVertexData()->GetVertices();
	SVec3* srcTSB[3] =
	{
		_skin->GetBindMesh()->GetVertexData()->GetTangents(),
		_skin->GetBindMesh()->GetVertexData()->GetBiNormals(),
		_skin->GetBindMesh()->GetVertexData()->GetNormals(),
	};

	// get target vertex data.
	SVec3* dstVerts = _targetMesh->GetVertexData()->GetVertices();
	SVec3* dstTSB[3] =
	{
		_targetMesh->GetVertexData()->GetTangents(),
		_targetMesh->GetVertexData()->GetBiNormals(),
		_targetMesh->GetVertexData()->GetNormals(),
	};

	// get vertex bone ids/weights.
	const SVec3* weights = _skin->GetVertexWeights();
	const unsigned int* boneIds = _skin->GetVertexBoneIds();

	// skin.
	float invMag = 0;
	const float* matrixData = 0;
	unsigned int vertexCount = _skin->GetBindMesh()->GetVertexData()->GetVertexCount();
	for ( unsigned int i = 0; i < vertexCount; ++i )
	{
		unsigned int bones[3] = {
			( boneIds[ i ] & 0xFF ),
			( ( boneIds[ i ] >> 8 ) & 0xFF ),
			( ( boneIds[ i ] >> 16 ) & 0xFF )
		};

		SVec3 curVert;

		// calculate positional bone influences.
		// bone 0.
		matrixData = _bonePalette[ bones[0] ].GetDataStart();
		curVert.x = weights->x *
			( matrixData[0]*srcVerts->x + matrixData[1]*srcVerts->y + matrixData[2]*srcVerts->z + matrixData[3] );
		curVert.y = weights->x *
			( matrixData[4]*srcVerts->x + matrixData[5]*srcVerts->y + matrixData[6]*srcVerts->z + matrixData[7] );
		curVert.z = weights->x *
			( matrixData[8]*srcVerts->x + matrixData[9]*srcVerts->y + matrixData[10]*srcVerts->z + matrixData[11] );

		// bone 1.
		matrixData = _bonePalette[ bones[1] ].GetDataStart();
		curVert.x += weights->y *
			( matrixData[0]*srcVerts->x + matrixData[1]*srcVerts->y + matrixData[2]*srcVerts->z + matrixData[3] );
		curVert.y += weights->y *
			( matrixData[4]*srcVerts->x + matrixData[5]*srcVerts->y + matrixData[6]*srcVerts->z + matrixData[7] );
		curVert.z += weights->y *
			( matrixData[8]*srcVerts->x + matrixData[9]*srcVerts->y + matrixData[10]*srcVerts->z + matrixData[11] );

		// bone 2.
		matrixData = _bonePalette[ bones[2] ].GetDataStart();
		curVert.x += weights->z *
			( matrixData[0]*srcVerts->x + matrixData[1]*srcVerts->y + matrixData[2]*srcVerts->z + matrixData[3] );
		curVert.y += weights->z *
			( matrixData[4]*srcVerts->x + matrixData[5]*srcVerts->y + matrixData[6]*srcVerts->z + matrixData[7] );
		curVert.z += weights->z *
			( matrixData[8]*srcVerts->x + matrixData[9]*srcVerts->y + matrixData[10]*srcVerts->z + matrixData[11] );

		// take the min/max for each vertex to calculate a new AA box.
		VecMinMax( posMin, posMax, curVert );

		// copy to the destination vertex.
		*dstVerts = curVert;

		// calculate tangent space basis bone influences.
		for ( unsigned int j = 0; j < 3; ++j )
		{
			SVec3 curTSBVec;

			// bone 0.
			matrixData = _boneRotPalette[ bones[0] ].GetDataStart();
			curTSBVec.x = weights->x * 
				( matrixData[0]*srcTSB[j]->x + matrixData[1]*srcTSB[j]->y + matrixData[2]*srcTSB[j]->z );
			curTSBVec.y = weights->x * 
				( matrixData[3]*srcTSB[j]->x + matrixData[4]*srcTSB[j]->y + matrixData[5]*srcTSB[j]->z );
			curTSBVec.z = weights->x * 
				( matrixData[6]*srcTSB[j]->x + matrixData[7]*srcTSB[j]->y + matrixData[8]*srcTSB[j]->z );

			// bone 1.
			matrixData = _boneRotPalette[ bones[1] ].GetDataStart();
			curTSBVec.x += weights->y * 
				( matrixData[0]*srcTSB[j]->x + matrixData[1]*srcTSB[j]->y + matrixData[2]*srcTSB[j]->z );
			curTSBVec.y += weights->y * 
				( matrixData[3]*srcTSB[j]->x + matrixData[4]*srcTSB[j]->y + matrixData[5]*srcTSB[j]->z );
			curTSBVec.z += weights->y * 
				( matrixData[6]*srcTSB[j]->x + matrixData[7]*srcTSB[j]->y + matrixData[8]*srcTSB[j]->z );

			// bone 2.
			matrixData = _boneRotPalette[ bones[2] ].GetDataStart();
			curTSBVec.x += weights->z * 
				( matrixData[0]*srcTSB[j]->x + matrixData[1]*srcTSB[j]->y + matrixData[2]*srcTSB[j]->z );
			curTSBVec.y += weights->z * 
				( matrixData[3]*srcTSB[j]->x + matrixData[4]*srcTSB[j]->y + matrixData[5]*srcTSB[j]->z );
			curTSBVec.z += weights->z * 
				( matrixData[6]*srcTSB[j]->x + matrixData[7]*srcTSB[j]->y + matrixData[8]*srcTSB[j]->z );

			// normalize.
			invMag = InvSqrtFast( curTSBVec.x*curTSBVec.x + curTSBVec.y*curTSBVec.y + curTSBVec.z*curTSBVec.z );
			curTSBVec.x *= invMag;
			curTSBVec.y *= invMag;
			curTSBVec.z *= invMag;

			// copy to the destination TSB vector.
			*( dstTSB[j] ) = curTSBVec;

			// next TSB vector.
			++dstTSB[j];
			++srcTSB[j];
		}

		// next vertex data.
		++weights;
		++srcVerts;
		++dstVerts;
	}

	// flag mesh components as dirty.
	_targetMesh->GetVertexData()->MarkAsDirty( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TANGENT_MASK | GR_ATTRIB_NORMAL_MASK | GR_ATTRIB_BINORMAL_MASK );
	_targetMesh->VertexDataChanged();
}
