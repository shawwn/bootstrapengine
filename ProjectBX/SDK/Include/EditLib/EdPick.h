//----------------------------------------------------------
// File:		EdPick.h
// Author:		Kevin Bray
// Created:		04-06-08
//
// Purpose:		To pick objects in the scene.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrMeshInst.h"
#include "GrMesh.h"

// forward declarations.
class MRay;
class GrLight;
class GrModel;
class GrModelNode;
class GrMeshInst;

//**********************************************************
// class EdPick
//**********************************************************
class EdPick
{
public:
    // NOTE: this enumeration must match GrScene.h.
	typedef unsigned int Flag;
	static const Flag kPickNone		    = 0x00;
	static const Flag kPickBSP		    = 0x01;
	static const Flag kPickModels	    = 0x02;
	static const Flag kPickModelNodes	= 0x04;
	static const Flag kPickLights	    = 0x08;

	typedef GrMesh::STriCollision STriHit;

	EdPick( float lightHitDist );
	~EdPick();

	void			SetLightHitDist( float dist )			{	_lightHitDist = dist;	}
	float			GetLightHitDist() const					{	return _lightHitDist;	}

	// picks the scene.
	void			Pick( const MRay& ray, Flag flags, float maxDist );

	// returns the type of object that was hit.
	Flag			GetHitType()							{	return _hitType;		}

	// hit types.
	GrLight*		GetHitLight() const						{	return _hitLight;		}
	GrModel*		GetHitModel() const						{	return _hitModel;		}
	GrModelNode*	GetHitModelNode() const					{	return _hitModelNode;	}
	GrMeshInst*		GetHitMeshInst() const					{	return _hitMeshInst;	}

	// return the triangle hit information for intersection with models, BSP, or terrain.
	const STriHit&	GetTriHitInfo() const					{	return _hitInfo;		}

	// reset the picker.
	void			Reset();

private:
	Flag			_hitType;
	GrLight*		_hitLight;
	GrModel*		_hitModel;
    GrModelNode*    _hitModelNode;
	GrMeshInst*		_hitMeshInst;
	STriHit			_hitInfo;
	float			_lightHitDist;
};
