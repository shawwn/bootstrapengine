//----------------------------------------------------------
// File:		GrSceneTraverser.h
// Author:		Kevin Bray
// Created:		08-17-08
//
// Purpose:		To provide an interface for traversing scene data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MAABox;
class GrMeshInst;
class GrModel;
class GrBSPSplit;
class GrLight;

//**********************************************************
// class GrSceneTraverser
//**********************************************************
class GrSceneTraverser
{
public:
	virtual ~GrSceneTraverser() = 0 { }

	// these methods should return true if children should be
	// traversed.  This is called for BSP splits and models when
	// traversing the scene.  It is abstracted out to allow for
	// alteration of the BSP-tree and the model higherarchy.
	virtual bool			OnNode( const MAABox& box ) = 0;

	// this is called when there is a mesh instance range.
	virtual void			OnMeshInstRange( GrMeshInst* meshInst, unsigned int range ) = 0;

private:
};
