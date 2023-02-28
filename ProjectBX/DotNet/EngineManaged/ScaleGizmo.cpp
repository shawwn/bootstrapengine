//----------------------------------------------------------
// File:		ScaleGizmo.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "ScaleGizmo.h"

// project headers.
#include "SceneEntity.h"

// engine headers.
#include "MRay.h"
#include "MLineSegment.h"
#include "GrColor.h"
#include "GrRenderUtil.h"

// .NET namespaces.
using namespace System::Runtime::InteropServices;

//**********************************************************
// ref class ScaleGizmo
//**********************************************************

//----------------------------------------------------------
Bootstrap::ScaleGizmo::ScaleGizmo( float size )
	: _target( nullptr )
	, _size( size )
	, _scaleMode( ScaleMode::None )
{
	_highlightAxes = gcnew array< bool >( 3 );
	_axes = gcnew array< Vector3 >( 3 );
	_axes[ 0 ] = Vector3( _size, 0.0f, 0.0f );
	_axes[ 1 ] = Vector3( 0.0f, _size, 0.0f );
	_axes[ 2 ] = Vector3( 0.0f, 0.0f, _size );
}

//----------------------------------------------------------
Bootstrap::ScaleGizmo::~ScaleGizmo()
{
	this->!ScaleGizmo();
}

//----------------------------------------------------------
Bootstrap::ScaleGizmo::!ScaleGizmo()
{
	_target = nullptr;
	_axes = nullptr;
}

//----------------------------------------------------------
void 
Bootstrap::ScaleGizmo::SetEntity( SceneEntity^ target )
{
	_target = target;
}

//----------------------------------------------------------
bool 
Bootstrap::ScaleGizmo::MouseDown( Ray ray, int mouseX, int mouseY )
{
	// simply return false if we don't have a target.
	if ( !_target )
		return false;

	// check to see if we're in range with the gizmo.
	float rayDistToCenter = ray.Dist( _target->Position );
	if ( rayDistToCenter > _size )
		return false;

	// check to see if we should be in "scale all axes" mode.
	if ( rayDistToCenter < 0.25 * _size )
	{
		// store the mouse position.
		_startX = mouseX;
		_startY = mouseY;
		_scaleMode = ScaleMode::XYZ;
		return true;
	}

	// figure out which axis we're dragging along.
	float minDist = FLT_MAX;
	float rayMu = 0.0f;
	float segMu = 0.0f;
	MVec3 pos = _target->Position.ToMVec3();
	for ( int i = 0; i < 3; ++i )
	{
		float curDist = ray.Dist( MLineSegment( pos, pos + _axes[ i ].ToMVec3() ), rayMu, segMu );
		if ( curDist < minDist )
		{
			minDist = curDist;
			_scaleMode = ( ScaleMode )i;
		}
	}

	// store the mouse position.
	_startX = mouseX;
	_startY = mouseY;

	// return true to indicate that we're now in a dragging state.
	return true;
}

//----------------------------------------------------------
void 
Bootstrap::ScaleGizmo::MouseMove( Ray ray, int mouseX, int mouseY, bool active )
{
	if ( !_target )
		return;

	// calculate the deltas.
	int dx = mouseX - _startX;
	int dy = _startY - mouseY;
	float scaleDelta = 0.05f * ( float )( dx + dy );

	if( active )
	{
		Vector3 scale = _target->Scale;

		// scale the object based on the scale delta.
		if ( _scaleMode == ScaleMode::X )
			scale = scale + Vector3( scaleDelta, 0.0f, 0.0f );
		else if ( _scaleMode == ScaleMode::Y )
			scale = scale + Vector3( 0.0f, scaleDelta, 0.0f );
		else if ( _scaleMode == ScaleMode::Z )
			scale = scale + Vector3( 0.0f, 0.0f, scaleDelta );
		else if ( _scaleMode == ScaleMode::XYZ )
			scale = scale + Vector3( scaleDelta, scaleDelta, scaleDelta );

		const float kMinScale = 0.1f;

		// clamp the scale.
		if( scale.X < kMinScale ) 
			scale.X = kMinScale;
		if( scale.Y < kMinScale ) 
			scale.Y = kMinScale;
		if( scale.Z < kMinScale ) 
			scale.Z = kMinScale;
		
		// apply the scale.
		_target->Scale = scale;
		
		// update the mouse location.
		_startX = mouseX;
		_startY = mouseY;
	}
	else
	{
		// clear the highlight flags.
		for( int i = 0; i < 3; ++i )
			_highlightAxes[ i ] = false;

		// check to see if we're in range with the gizmo.
		float rayDistToCenter = ray.Dist( _target->Position );
		if ( rayDistToCenter > _size )
			return;

		// compute the closest axis.
		int closestAxis = 0;
		float minDist = FLT_MAX;
		float rayMu = 0.0f;
		float segMu = 0.0f;
		MVec3 pos = _target->Position.ToMVec3();
		for ( int i = 0; i < 3; ++i )
		{
			float curDist = ray.Dist( MLineSegment( pos, pos + _axes[ i ].ToMVec3() ), rayMu, segMu );
			if ( curDist < minDist )
			{
				minDist = curDist;
				closestAxis = i;
			}
		}

		// highlight the closest axis.
		_highlightAxes[ closestAxis ] = true;
	}
}

//----------------------------------------------------------
void 
Bootstrap::ScaleGizmo::MouseUp( Ray ray, int mouseX, int mouseY )
{
	if ( !_target )
		return;

	// all done.  
	_scaleMode = ScaleMode::None;

    // notify the target that we've finished moving it.
    _target->CommitTransform();
}

//----------------------------------------------------------
void 
Bootstrap::ScaleGizmo::Render()
{
	if ( !_target )
		return;

	float h = _size;
	MVec3 pos = _target->Position.ToMVec3();
	MVec3 axes[ 3 ] = {
		MVec3( 1, 0, 0 ),	// X,
		MVec3( 0, 1, 0 ),	// Y,
		MVec3( 0, 0, 1 )	// Z,
	};

	GrColor xColor( 1.0f, 0.0f, 0.0f, 1.0f );
	GrColor yColor( 0.0f, 1.0f, 0.0f, 1.0f );
	GrColor zColor( 0.0f, 0.0f, 1.0f, 1.0f );
	GrColor selColor( 1.0f, 1.0f, 0.0f, 1.0f );

	// draw lines.
	MVec3 xPos = pos + MVec3( _size, 0, 0 );
	MVec3 yPos = pos + MVec3( 0, _size, 0 );
	MVec3 zPos = pos + MVec3( 0, 0, _size );
	gGrRenderUtil->DrawColoredLine( pos, xPos, _highlightAxes[0] ? selColor : xColor );
	gGrRenderUtil->DrawColoredLine( pos, yPos, _highlightAxes[1] ? selColor : yColor );
	gGrRenderUtil->DrawColoredLine( pos, zPos, _highlightAxes[2] ? selColor : zColor );

	// draw boxes.
	gGrRenderUtil->DrawColoredCube( xPos, 0.25f * _size, xColor );
	gGrRenderUtil->DrawColoredCube( yPos, 0.25f * _size, yColor );
	gGrRenderUtil->DrawColoredCube( zPos, 0.25f * _size, zColor );
}
