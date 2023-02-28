//----------------------------------------------------------
// File:		GrModelSkinController.h
// Author:		Kevin Bray
//
// Purpose:		To manage a model consisting of multiple meshes.
//
// Created:		05-26-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"

// forward declarations.
class GrSkinInst;
class GrModelNode;

//**********************************************************
// class GrModelSkinController
//**********************************************************
class GrModelSkinController
{
public:
	GrModelSkinController();
	GrModelSkinController( GrSkinInst* skinInst, GrModelNode* root );
	~GrModelSkinController();

	// this is for array initialization.
	void			Init( GrSkinInst* skinInst, GrModelNode* root );

	// update the skin instance we're driving.
	void			Update();

private:
	typedef UFastArray< GrModelNode* > NodeArray;

	void			BuildBoneMap( GrModelNode* root );

	// mapping of bones to nodes.
	NodeArray		_boneMapping;
	GrSkinInst*		_skinInst;
};
