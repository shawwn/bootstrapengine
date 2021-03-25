//----------------------------------------------------------
// File:		GrRenderInteraction.h
// Author:		Kevin Bray
// Created:		11-14-05
//
// Purpose:		To manage interaction rendering state.
//
// Copyright © 2004 Bootstrap Games
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"

// forward declarations.
class GrCamera;
class GrGLProg;
class GrGLProg;
class GrLight;
class GrMaterialPass;
class GrMesh;

// NOTE: Once BeginRender() is called, rendering may only occur
// through this class until EndRender() is called.

//**********************************************************
// class GrRenderInteraction
//**********************************************************
class GrRenderInteraction
{
public:
	GrRenderInteraction();
	~GrRenderInteraction();

	enum ERENDERMODE
	{
		ERM_DEPTH,
		ERM_AMBIENT,
		ERM_LIGHT,
	};

	enum ETRANSFORMMODE
	{
		ETM_MODEL,		// model space geometry.
		ETM_WORLD,		// world space geometry.
	};

	// must be called before any rendering occurs.
	void				BeginRender();
	void				EndRender();

	// rendering commands.
	void				SetRenderMode( ERENDERMODE mode );
	void				SetTransformMode( ETRANSFORMMODE mode );
	void				SetCamera( const GrCamera* camera );
	void				SetLight( const GrLight* light );
	void				SetMaterialPass( GrMaterialPass* materialPass );
	void				SetTransform( const MMat4x4& modelToWorld );
	void				Render( const GrMesh* mesh, unsigned int range );

private:
	GrCamera*			_camera;
	GrGLProg*			_vertexProg;
	GrGLProg*			_fragmentProg;
	GrLight*			_light;
	GrMaterialPass*		_pass;
	MMat4x4				_modelToWorld;
	MMat4x4				_modelToView;
	MMat4x4				_viewToProj;
	ERENDERMODE			_renderMode;
	ETRANSFORMMODE		_transformMode;
	bool				_inRender;
};
extern GrRenderInteraction* gGrRenderInteraction;
