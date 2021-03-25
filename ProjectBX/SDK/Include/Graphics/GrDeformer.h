//----------------------------------------------------------
// File:		GrDeformer.h
// Author:		Kevin Bray
// Created:		05-12-06
//
// Purpose:		An interface used to deform a mesh.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMesh;

//**********************************************************
// class GrDeformer
//**********************************************************
class GrDeformer
{
public:
	GrDeformer();
	virtual ~GrDeformer();

	virtual const char*	GetName() const = 0;

	// sets where the data to deform comes from.
	virtual GrDeformer*	Clone() const = 0;

	// perform the deform operation.  This happens once per frame before
	// rendering occurs.
	virtual void		Deform( GrMesh* dst, GrMesh* src, unsigned int srcRange, unsigned int time ) = 0;

private:
};
