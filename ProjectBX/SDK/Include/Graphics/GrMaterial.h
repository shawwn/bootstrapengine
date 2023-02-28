//----------------------------------------------------------
// File:		GrMaterial.h
// Author:		Kevin Bray
// Created:		03-20-05
//
// Purpose:		To describe how an object should be rendered.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"
#include "RRsrc.h"
#include "UExprTerm.h"
#include "URefCount.h"
#include "UFastArray.h"
#include "GrShader.h"
#include "GrMaterialUserParams.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrCamera;
class GrLight;
class GrMeshInst;
class GrMaterialPass;
class GrDeformer;
class GrMesh;
class UExprTerm;
struct SMaterialPassInfo;

// optimal material usage:
// for each light
//  for each material
//   for each pass
//    BeginPass( pass, effect )
//    for each mesh instance
//     ShadePass()

//**********************************************************
// class GrMaterial
//**********************************************************
class GrMaterial : public URefCount
{
public:
	typedef UFastArray< GrDeformer* > DeformerArray;

	enum ESORTTYPE
	{
		EST_OPAQUE,	// no special sort required.
		EST_DECAL,	// after normal objects, before alpha.
		EST_ALPHA,	// after everything, sorted from back to front.
	};

	// this is used to describe how a material should be constructed.
	struct SInfo
	{
		// deformers.
		UFastArray< GrDeformer* > deformers;

		// passes.
		unsigned int passCount;
		GrMaterialPass** passes;

		// passes.
		unsigned int postProcessPassCount;
		GrMaterialPass** postProcessPasses;

		// sort priority.
		ESORTTYPE sortType;

		// material info.
		bool castShadow;
		bool receiveShadow;
		bool stipple;
	};

	GrMaterial( const tstring& name, const SInfo& matInfo );
	~GrMaterial();

	// rebuilds the material.
	void							Rebuild( const SInfo& matInfo );

	// adds a pass to the material.
	void							AddPass( SMaterialPassInfo& passInfo );
	void							AddPostProcessPass( SMaterialPassInfo& passInfo );

	void							DeletePass( unsigned int passIdx );
	void							DeletePostProcessPass( unsigned int passIdx );

	// get the material's name.
	const tstring&					GetName() const				{	return _name;			}
	const tstring&					GetShortName() const		{	return _shortName;		}

	// creates a clone of this material.
	URef< GrMaterial >				Clone() const;
	URef< GrMaterial >				Clone( const tstring& name ) const;

	// deformers.
	bool							HasDeformers() const		{	return _deformers.GetElemCount() > 0;	}
	const DeformerArray&			GetDeformers() const		{	return _deformers;		}

	// returns the number of passes needed.
	unsigned int					GetPassCount() const		{	return _passCount;		}

	// note that the passes are freed if a material is reloaded.  This means that the
	// caller should not keep pointers to passes for more than one frame.
	GrMaterialPass*					GetPass( unsigned int pass );
	const GrMaterialPass*			GetPass( unsigned int pass ) const;

	// returns any gentextures needed by the lighting passes.
	GrTextureStage::GenTextureFlag	GetGenTextureFlags() const;

	// post processing passes.
	unsigned int					GetPostProcessPassCount() const		{	return _postProcessPassCount;	}
	GrMaterialPass*					GetPostProcessPass( unsigned int pass );
	const GrMaterialPass*			GetPostProcessPass( unsigned int pass ) const;

	// this is only for distance sorting.  Note that objects are sorted by priority
	// first, and then by distance within their respective priority.
	ESORTTYPE						GetSortType() const				{	return _sortType;		}
	void							SetSortType( ESORTTYPE type )	{	_sortType = type;		}

	// flags.
	void							SetCastShadow( bool castShadow )		{	_castShadow = castShadow;		}
	void							SetReceiveShadow( bool receiveShadow )	{	_receiveShadow = receiveShadow;	}
	void							SetStipple( bool stipple )				{	_stipple = stipple;				}

	bool							GetCastShadow() const		{	return _castShadow;		}
	bool							GetReceiveShadow() const	{	return _receiveShadow;	}
	bool							GetStipple() const			{	return _stipple;		}

	// return true if all of the material's passes have the property.
	bool							AlphaTest() const;

	// return true if any of the material's passes have the property or if the material
	// has the property disabled.
	bool							CastShadow() const;
	bool							ReceiveShadow() const;

	// updates the material.  This should be called once per frame for every material.
	void							Update();

private:
	void							Clean();

	// name.
	tstring							_name;
	tstring							_shortName;					// after the final '/'.

	// deformers.
	DeformerArray					_deformers;

	// passes.
	unsigned int					_passCount;
	GrMaterialPass**				_passes;

	// post process passes.
	unsigned int					_postProcessPassCount;
	GrMaterialPass**				_postProcessPasses;

	// distance sort/priority sort information.
	ESORTTYPE						_sortType;

	// miscellaneous shader information.
	bool							_castShadow;
	bool							_receiveShadow;
	bool							_stipple;
};
