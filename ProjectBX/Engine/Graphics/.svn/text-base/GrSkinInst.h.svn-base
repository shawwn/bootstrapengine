//----------------------------------------------------------
// File:		GrSkinInst.h
// Author:		Kevin Bray
// Created:		06-21-06
//
// Purpose:		To manage skin data for a mesh.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"
#include "MAABox.h"
#include "GrMesh.h"

// forward declarations.
struct SVec3;
class GrMesh;
class GrMeshInst;
class MMat4x4;
class MMat3x3;
class GrSkin;

//**********************************************************
// class GrSkin
//**********************************************************
class GrSkinInst
{
public:
	// the skin takes over the pointers passed in.
	GrSkinInst( GrMeshInst* targetMeshInst );
	~GrSkinInst();

	// returns the skin.
	GrSkin*			GetSkin() const				{	return _skin;			}

	// picks against the animated mesh.
	void			Pick( GrMesh::TriHitArray& triHits, const MRay& ray ) const;

	// bone management.
	unsigned int	GetBoneCount() const;
	const tstring&	GetBoneName( unsigned int id ) const;
	unsigned int	GetBoneId( const char* name ) const;
	void			SetBoneMatrix( unsigned int boneId, const MMat4x4& matrix );

	// skin the mesh.
	void			ApplySkin();

private:

	// the mesh we're skinning.
	GrMeshInst*		_targetMeshInst;
	URef< GrMesh >	_targetMesh;
	GrSkin*			_skin;

	// animated bone matrices and the matrix palette.
	MMat4x4*		_animatedBoneMatrices;
	MMat4x4*		_bonePalette;
	MMat3x3*		_boneRotPalette;

	// this is set if the bone palettes need updating.
	bool			_dirty;
};
