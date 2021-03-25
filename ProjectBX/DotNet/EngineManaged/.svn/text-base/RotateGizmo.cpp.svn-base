//----------------------------------------------------------
// File:		RotateGizmo.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "RotateGizmo.h"

// project headers.
#include "SceneEntity.h"
#include "Ray.h"

// engine headres.
#include "MRay.h"
#include "MLineSegment.h"
#include "GrColor.h"
#include "GrUtil.h"
#include "GLSubsys.h"

// constants.
#define CIRCLE_SEGMENT_COUNT		32

// .NET namespaces.
using namespace System::Runtime::InteropServices;

//**********************************************************
// ref class RotateGizmo
//**********************************************************

//----------------------------------------------------------
Bootstrap::RotateGizmo::RotateGizmo( float size )
	: _target( nullptr )
	, _size( size )
	, _circles( 0 )
	, _startX( 0 )
	, _startY( 0 )
	, _rotateAxis( RotateAxis::None )
{
	_highlightAxes = gcnew array< bool >( 3 );

	// allocate the line segments and build circles.
	_circles = new MLineSegment[ 3 * CIRCLE_SEGMENT_COUNT ];
}

//----------------------------------------------------------
Bootstrap::RotateGizmo::~RotateGizmo()
{
	this->!RotateGizmo();
}

//----------------------------------------------------------
Bootstrap::RotateGizmo::!RotateGizmo()
{
	_target = nullptr;
	delete[] _circles;
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::SetEntity( SceneEntity^ target )
{
	_target = target;

	// make the rotation circles.
	if ( _target )
	{
		MakeCircle( GetCircle( RotateAxis::X ), _target->Position.ToMVec3(), 1, 2, 0.90f * _size );	// x-axis
		MakeCircle( GetCircle( RotateAxis::Y ), _target->Position.ToMVec3(), 0, 2, 0.95f * _size );	// y-axis
		MakeCircle( GetCircle( RotateAxis::Z ), _target->Position.ToMVec3(), 1, 0, 1.00f * _size );	// z-axis
	}
}

//----------------------------------------------------------
bool 
Bootstrap::RotateGizmo::MouseDown( Ray ray, int mouseX, int mouseY )
{
	// simply return false if we don't have a target.
	if ( !_target )
		return false;

	// check to see what circle we're closest to.
	MRay mray = ray.ToMRay();
	int minAxis = 0;
	float minDist = FLT_MAX;
	for ( int i = 0; i < 3; ++i )
	{
		float dist = GetDistToCircle( GetCircle( ( RotateAxis )i ), mray );
		if ( dist < minDist )
		{
			minDist = dist;
			minAxis = i;
		}
	}

	// are we within the area of tolerance?
	if ( minDist > _size )
		return false;

	// store the mouse location and set the rotation axis.
	_startX = mouseX;
	_startY = mouseY;
	_rotateAxis = ( RotateAxis )minAxis;

	// return true to indicate that we're now in a dragging state.
	return true;
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::MouseMove( Ray ray, int mouseX, int mouseY, bool active )
{
	if ( !_target )
		return;

	// determine the closest circle.
	MRay mray = ray.ToMRay();
	int minAxis = 0;
	float minDist = FLT_MAX;
	for ( int i = 0; i < 3; ++i )
	{
		float dist = GetDistToCircle( GetCircle( ( RotateAxis )i ), mray );
		if ( dist < minDist )
		{
			minDist = dist;
			minAxis = i;
		}
	}

	if( active )
	{
		if ( _rotateAxis == RotateAxis::None )
			return;

		// calculate the delta in rotation.
		int dx = mouseX - _startX;
		int dy = _startY - mouseY;
		float deltaRot = ( float )( dx + dy );

		// if we're messing with the Y axis, we don't need to consider
		// the orientation of the object in relation to us.  Otherwise,
		// we need to flip the travel direction based on the orientation
		// of the object.  What about the z-up case?
		if ( _rotateAxis == RotateAxis::X )
		{
			_target->Rotation = _target->Rotation + Vector3( deltaRot, 0.0f, 0.0f );
		}
		else if ( _rotateAxis == RotateAxis::Y )
		{
			_target->Rotation = _target->Rotation + Vector3( 0.0f, deltaRot, 0.0f );  
		}
		else if ( _rotateAxis == RotateAxis::Z )
		{
			_target->Rotation = _target->Rotation + Vector3( 0.0f, 0.0f, deltaRot );
		}

		_startX = mouseX;
		_startY = mouseY;
	}
	else 
	{
		// clear the highlight flags.
		for( int i = 0; i < 3; ++i )
			_highlightAxes[ i ] = false;

		// are we within the range of the gizmo?
		if ( minDist > _size )
			return;

		// then highlight the axis closest to the mouse.
		_highlightAxes[ minAxis ] = true;
	}
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::MouseUp( Ray ray, int mouseX, int mouseY )
{
	if ( !_target )
		return;

	// all done.  
	_rotateAxis = RotateAxis::None;

    // notify the target that we've finished rotating it.
    _target->CommitTransform();
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::Render()
{
	if ( !_target )
		return;

	// try to get the target transform.  If we can't, simply return.
	MVec3 position = _target->Position.ToMVec3();

	GrSetWriteEnable( GR_RGBA | GR_DEPTH );
	GrSetState( GR_DEPTHTEST | GR_CULLFACE );

	// render the circles.
	bglPushMatrix();

	// colors.
	GrColor xColor( 1.0f, 0.0f, 0.0f );
	GrColor yColor( 0.0f, 1.0f, 0.0f );
	GrColor zColor( 0.0f, 0.0f, 1.0f );
	GrColor selColor( 1.0f, 1.0f, 0.0f );

	// render the circles.
	RenderCircle( GetCircle( RotateAxis::X ), _highlightAxes[0] ? selColor : xColor );
	RenderCircle( GetCircle( RotateAxis::Y ), _highlightAxes[1] ? selColor : yColor );
	RenderCircle( GetCircle( RotateAxis::Z ), _highlightAxes[2] ? selColor : zColor );

	bglPopMatrix();
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::MakeCircle( MLineSegment* segments, const MVec3& translate, int c0,
							  int c1, float scale )
{
	// temporary vars.
	MVec3 pt0( 0.0f, 0.0f, 0.0f );
	MVec3 pt1( 0.0f, 0.0f, 0.0f );

	// create a circle of line segments.
	float theta = 0.0f;
	float step = TWO_PI / float( CIRCLE_SEGMENT_COUNT );
	for ( unsigned int i = 0; i < CIRCLE_SEGMENT_COUNT; ++i, theta += step )
	{
		pt0[ c0 ] = scale * Cos( theta );
		pt0[ c1 ] = scale * Sin( theta );
		pt1[ c0 ] = scale * Cos( theta + step );
		pt1[ c1 ] = scale * Sin( theta + step );
		segments[ i ].Reset( translate + pt0, translate + pt1 );
	}
}

//----------------------------------------------------------
void 
Bootstrap::RotateGizmo::RenderCircle( MLineSegment* segments, const GrColor& color )
{
	bglBegin( GL_LINES );
	{
		bglColor4f( color.GetR(), color.GetG(), color.GetB(), color.GetA() );
		for ( unsigned int i = 0; i < CIRCLE_SEGMENT_COUNT; ++i )
		{
			bglVertex3f( segments[ i ].GetPt0().GetX(), segments[ i ].GetPt0().GetY(), segments[ i ].GetPt0().GetZ() );
			bglVertex3f( segments[ i ].GetPt1().GetX(), segments[ i ].GetPt1().GetY(), segments[ i ].GetPt1().GetZ() );
		}
	}
	bglEnd();
}

//----------------------------------------------------------
float 
Bootstrap::RotateGizmo::GetDistToCircle( MLineSegment* segments, const MRay& ray )
{
	float segMu = 0.0f;
	float rayMu = 0.0f;
	float minDist = FLT_MAX;
	for ( unsigned int i = 0; i < CIRCLE_SEGMENT_COUNT; ++i )
	{
		float curDist = ray.Dist( segments[ i ], rayMu, segMu, 0.0001f );
		minDist = Min( curDist, minDist );			
	}
	return minDist;
}

//----------------------------------------------------------
MLineSegment* 
Bootstrap::RotateGizmo::GetCircle( RotateAxis axis )
{
	return _circles + ( int )axis * CIRCLE_SEGMENT_COUNT;
}