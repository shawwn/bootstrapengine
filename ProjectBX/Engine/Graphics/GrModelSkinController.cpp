//----------------------------------------------------------
// File:		GrModelSkinController.cpp
// Author:		Kevin Bray
// Created:		05-26-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrModelSkinController.h"

// project includes.
#include "GrModelNode.h"
#include "GrSkinInst.h"

//**********************************************************
// class GrModelSkinController
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrModelSkinController::GrModelSkinController()
: _skinInst( 0 )
{

}

//----------------------------------------------------------
GrModelSkinController::GrModelSkinController( GrSkinInst* skinInst, GrModelNode* root )
: _skinInst( skinInst )
{
	// build the bone map.
	BuildBoneMap( root );
}

//----------------------------------------------------------
GrModelSkinController::~GrModelSkinController()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrModelSkinController::Init( GrSkinInst* skinInst, GrModelNode* root )
{
	// make sure we haven't already been initialized!
	B_ASSERT( _skinInst == 0 );

	// initialize.
	_skinInst = skinInst;
	BuildBoneMap( root );
}

//----------------------------------------------------------
void
GrModelSkinController::Update()
{
	// there should be one node for each bone!
	B_ASSERT( _skinInst != 0 );
	B_ASSERT( _boneMapping.GetElemCount() == _skinInst->GetBoneCount() );

	// drive the bones.
	GrModelNode* curBoneNode = 0;
	const unsigned int boneCount = _skinInst->GetBoneCount();
	for ( unsigned int i = 0; i < boneCount; ++i )
	{
		curBoneNode = _boneMapping[ i ];
		if ( curBoneNode )
			_skinInst->SetBoneMatrix( i, curBoneNode->GetWorld() );
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrModelSkinController::BuildBoneMap( GrModelNode* root )
{
	// find the node thats supposed to drive each bone.
	const unsigned int boneCount = _skinInst->GetBoneCount();
	for ( unsigned int i = 0; i < boneCount; ++i )
		_boneMapping.Push( root->FindNode( _skinInst->GetBoneName( i ).c_str(), true ) );

	// ensure that the bone mapping is the correct size.
	B_ASSERT( _boneMapping.GetElemCount() == boneCount );
}
