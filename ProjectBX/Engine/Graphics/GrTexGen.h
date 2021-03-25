//----------------------------------------------------------
// File:		GrTexGen.h
// Author:		Kevin Bray
// Created:		10-03-05
//
// Purpose:		To render procedurally generated textures.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "GrTextureStage.h"

class GrRenderTarget;
class GrRenderbuffer;
class GrTextureBase;
class GrCamera;
class MPlane;
class GrMeshInst;
class GrMaterial;

//**********************************************************
// class GrTexGen
//**********************************************************
class GrTexGen
{
public:
	GrTexGen( unsigned int startLOD );
	~GrTexGen();

	// build a reflection for the mesh instance passed in.
	void					AddReflection( const GrMeshInst* meshInst, int range );
	void					RemoveReflection( const GrMeshInst* meshInst, int range );

	// build a portal for the mesh instance passed in.
	void					AddPortal( const GrMeshInst* meshInst, int range );
	void					RemovePortal( const GrMeshInst* meshInst, int range );

	// returns true if we're currently rendering a reflection.
	bool					InReflectGen() const		{	return _inReflectGen;		}

	// generates all registered textures.
	void					GenTextures( const GrCamera* camera );

private:
	struct SRenderTexture
	{
		URef< GrRenderTarget > reflection;
		const GrMeshInst* meshInst;
		int range;
	};
	typedef UFastArray< SRenderTexture, 16, 16 > RenderTextureList;

	// generates all reflections for visible objects.
	void					GenReflections( const GrCamera* camera );
	void					GenPortals( const GrCamera* camera );
	void					ApplyTexture( GrMaterial* material, GrTextureStage::GenTextureFlag flag, URef< GrTextureBase > texture );
	void					Add( RenderTextureList& rtList, const GrMeshInst* meshInst, int range );
	void					Remove( RenderTextureList& rtList, const GrMeshInst* meshInst, int range );
	URef< GrRenderTarget >	BuildRenderTarget();

	unsigned int			_lod;
	RenderTextureList		_reflections;
	RenderTextureList		_portals;
	URef< GrTextureBase >	_white;
	bool					_inReflectGen;
};
extern GrTexGen* gGrTexGen;
