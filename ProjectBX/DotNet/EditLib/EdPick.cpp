//----------------------------------------------------------
// File:		EdPick.cpp
// Author:		Kevin Bray
// Created:		04-06-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdPick.h"

// project includes.
#include "GrScene.h"
#include "GrLight.h"
#include "GrModel.h"
#include "MRay.h"

//**********************************************************
// class EdPick
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdPick::EdPick( float lightHitDist )
: _lightHitDist( lightHitDist )
, _hitType( kPickNone )
, _hitLight( 0 )
, _hitModel( 0 )
, _hitModelNode( 0 )
, _hitMeshInst( 0 )
{

}

//----------------------------------------------------------
EdPick::~EdPick()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdPick::Pick( const MRay& ray, Flag flags, float maxDist )
{
	Reset();

	// iterate through all of the lights.
	if ( flags & kPickLights )
	{
		// check lights for a hit.
		float bestHit = maxDist;
		unsigned int lightCount = gGrScene->GetLightCount();
		for ( unsigned int i = 0; i < lightCount; ++i )
		{
			// get the current light.
			GrLight* cur = gGrScene->GetLight( i );
			MVec3 curPos = cur->GetPos();

			// get the distance from the point to the ray.
			float dist = ray.Dist( curPos );
			if ( dist < _lightHitDist )
			{
				float distToViewer = ray.GetStart().Dist( curPos );
				if ( distToViewer < maxDist )
				{
					maxDist = distToViewer;
					_hitLight = cur;
				}
			}
		}

		// did we hit something?  If so, set our flags and return.
		if ( _hitLight )
		{
			_hitType = kPickLights;
			return;
		}
	}

	// pick against the scene and convert the hit result to our own
	// pick-flag value.
	GrScene::PickFlags hitResult = gGrScene->Pick( _hitModel, _hitModelNode, _hitMeshInst, _hitInfo, ray, (GrScene::PickFlags)flags );
    _hitType = (Flag)hitResult;
}

//----------------------------------------------------------
void
EdPick::Reset()
{
	_hitType = kPickNone;
	_hitLight = 0;
	_hitModel = 0;
	_hitMeshInst = 0;
}
