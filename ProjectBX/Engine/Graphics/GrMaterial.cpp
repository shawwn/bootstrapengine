//----------------------------------------------------------
// File:		GrMaterial.cpp
// Author:		Kevin Bray
// Created:		03-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMaterial.h"

// project includes.
#include "GLSubsys.h"
#include "UReader.h"
#include "GrMaterialPass.h"
#include "MVec2.h"
#include "GrTexture.h"
#include "GrTextureCube.h"
#include "GrDeformer.h"
#include "GrMesh.h"
#include "GrMaterialMgr.h"

//**********************************************************
// class GrMaterial
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMaterial::GrMaterial( const tstring& name, const SInfo& matInfo )
: _name( name )
, _deformers( matInfo.deformers )
, _passes( matInfo.passes )
, _passCount( matInfo.passCount )
, _postProcessPassCount( matInfo.postProcessPassCount )
, _postProcessPasses( matInfo.postProcessPasses )
, _sortType( matInfo.sortType )
, _castShadow( matInfo.castShadow )
, _receiveShadow( matInfo.receiveShadow )
, _stipple( matInfo.stipple )
{
	UPath temp( name );

	// get the scene/package name.
	_shortName = temp.GetPathString();
	size_t pos = _shortName.find_first_of( '/' );
	if ( pos != tstring::npos )
		_shortName = _shortName.substr( pos + 1 );		

	// get the end of the scene/package name.
	pos = _shortName.find_first_of( '/' );
	tstring package = _shortName.substr( 0, pos );

	// now get the material name.
	pos = _shortName.find_last_of( '/' );
	if ( pos != tstring::npos && pos < ( _shortName.length() - 1 ) )
		_shortName = package + "\n" + _shortName.substr( pos + 1 );
}

//----------------------------------------------------------
GrMaterial::~GrMaterial()
{
	// free resources.
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMaterial::Rebuild( const SInfo& matInfo )
{
	Clean();
	_deformers = matInfo.deformers;
	_passes = matInfo.passes;
	_passCount = matInfo.passCount;
	_postProcessPasses = matInfo.postProcessPasses;
	_postProcessPassCount = matInfo.postProcessPassCount;
	_sortType = matInfo.sortType;
	_castShadow = matInfo.castShadow;
	_receiveShadow = matInfo.receiveShadow;
	_stipple = matInfo.stipple;
}

//----------------------------------------------------------
void
GrMaterial::AddPass( SMaterialPassInfo& passInfo )
{
	// create the new pass.
	GrMaterialPass* newPass = new GrMaterialPass( _passCount, passInfo );

	// allocate a new pass array.
	GrMaterialPass** newPassArray = new GrMaterialPass* [ _passCount+1 ];
	memcpy( newPassArray, _passes, _passCount * sizeof( GrMaterialPass* ) );
	delete[] _passes;

	// add the new pass and store the array.
	newPassArray[ _passCount++ ] = newPass;
	_passes = newPassArray;
}

//----------------------------------------------------------
void
GrMaterial::AddPostProcessPass( SMaterialPassInfo& passInfo )
{
	// create the new pass.
	GrMaterialPass* newPass = new GrMaterialPass( _postProcessPassCount, passInfo );

	// allocate a new pass array.
	GrMaterialPass** newPassArray = new GrMaterialPass* [ _postProcessPassCount+1 ];
	memcpy( newPassArray, _postProcessPasses, _postProcessPassCount * sizeof( GrMaterialPass* ) );
	delete[] _postProcessPasses;

	// add the new pass and store the array.
	newPassArray[ _postProcessPassCount++ ] = newPass;
	_postProcessPasses = newPassArray;
}

//----------------------------------------------------------
void
GrMaterial::DeletePass( unsigned int passIdx )
{
	B_ASSERT( passIdx < _passCount );

	delete _passes[ passIdx ];
	memmove( _passes + passIdx, _passes + passIdx + 1,
		( _passCount - passIdx - 1 ) * sizeof( GrMaterialPass* ) );
	--_passCount;
}

//----------------------------------------------------------
void
GrMaterial::DeletePostProcessPass( unsigned int passIdx )
{
	B_ASSERT( passIdx < _postProcessPassCount );

	delete _postProcessPasses[ passIdx ];
	memmove( _postProcessPasses + passIdx, _postProcessPasses + passIdx + 1,
		( _postProcessPassCount - passIdx - 1 ) * sizeof( GrMaterialPass* ) );
	--_postProcessPassCount;
}

//----------------------------------------------------------
URef< GrMaterial >
GrMaterial::Clone() const
{
	return Clone( _name );
}

//----------------------------------------------------------
URef< GrMaterial >
GrMaterial::Clone( const tstring& name ) const
{
	// fill out a material info structure.
	SInfo matInfo;
	for ( unsigned int i = 0; i < _deformers.GetElemCount(); ++i )
		matInfo.deformers.Push( _deformers[ i ]->Clone() );

	matInfo.passCount = _passCount;
	matInfo.passes = new GrMaterialPass*[ _passCount ];
	for ( unsigned int i = 0; i < _passCount; ++i )
		matInfo.passes[ i ] = _passes[ i ]->Clone();

	matInfo.postProcessPassCount = _postProcessPassCount;
	matInfo.postProcessPasses = new GrMaterialPass*[ _postProcessPassCount ];
	for ( unsigned int i = 0; i < _postProcessPassCount; ++i )
		matInfo.postProcessPasses[ i ] = _postProcessPasses[ i ]->Clone();

	matInfo.sortType = _sortType;
	matInfo.castShadow = _castShadow;
	matInfo.receiveShadow = _receiveShadow;
	matInfo.stipple = _stipple;

	// return a new material.
	return gGrMaterialMgr->SetMaterial( name, matInfo );
}

//----------------------------------------------------------
GrMaterialPass*
GrMaterial::GetPass( unsigned int pass )
{
	B_ASSERT( pass < _passCount );
	return _passes[ pass ];
}

//----------------------------------------------------------
const GrMaterialPass*
GrMaterial::GetPass( unsigned int pass ) const
{
	B_ASSERT( pass < _passCount );
	return _passes[ pass ];
}

//----------------------------------------------------------
GrTextureStage::GenTextureFlag
GrMaterial::GetGenTextureFlags() const
{
	// combine all of the gentexture flags in every pass.
	unsigned int flags = 0;
	for ( unsigned int pass = 0; pass < _passCount; ++pass )
		for ( unsigned int stage = 0; stage < ES_COUNT; ++stage )
			flags |= _passes[ pass ]->GetStage( ( ESTAGE )stage ).GetGenTextureFlag();

	for ( unsigned int pass = 0; pass < _postProcessPassCount; ++pass )
		for ( unsigned int stage = 0; stage < ES_COUNT; ++stage )
			flags |= _postProcessPasses[ pass ]->GetStage( ( ESTAGE )stage ).GetGenTextureFlag();

	// return the flags.
	return flags;
}

//----------------------------------------------------------
GrMaterialPass*
GrMaterial::GetPostProcessPass( unsigned int pass )
{
	B_ASSERT( pass < _postProcessPassCount );
	return _postProcessPasses[ pass ];
}

//----------------------------------------------------------
const GrMaterialPass*
GrMaterial::GetPostProcessPass( unsigned int pass ) const
{
	B_ASSERT( pass < _postProcessPassCount );
	return _postProcessPasses[ pass ];
}

//----------------------------------------------------------
bool
GrMaterial::AlphaTest() const
{
	// return false if any passes are non-alpha test.
	for ( unsigned int pass = 0; pass < _passCount; ++pass )
		if ( !_passes[ pass ]->AlphaTest() )
			return false;
	return true;
}

//----------------------------------------------------------
bool
GrMaterial::CastShadow() const
{
	// return false if the material overrides the pass behavior.
	if ( !_castShadow )
		return false;

	// return false if any passes are non-alpha test.
	for ( unsigned int pass = 0; pass < _passCount; ++pass )
		if ( _passes[ pass ]->CastShadow() )
			return true;
	return false;
}

//----------------------------------------------------------
bool
GrMaterial::ReceiveShadow() const
{
	// return false if the material overrides the pass behavior.
	if ( !_receiveShadow )
		return false;

	// return false if any passes are non-alpha test.
	for ( unsigned int pass = 0; pass < _passCount; ++pass )
		if ( _passes[ pass ]->ReceiveShadow() )
			return true;
	return false;
}

//----------------------------------------------------------
void
GrMaterial::Update()
{
	// update the passes.
	for ( unsigned int pass = 0; pass < _passCount; ++pass )
		_passes[ pass ]->Update();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMaterial::Clean()
{
	for ( unsigned int i = 0; i < _passCount; ++i )
		delete _passes[ i ];

	for ( unsigned int i = 0; i < _postProcessPassCount; ++i )
		delete _postProcessPasses[ i ];

	for ( unsigned int i = 0; i < _deformers.GetElemCount(); ++i )
		delete _deformers[ i ];

	_deformers.Clear();

	delete[] _passes;
	_passes = 0;

	delete[] _postProcessPasses;
	_postProcessPasses = 0;
}
