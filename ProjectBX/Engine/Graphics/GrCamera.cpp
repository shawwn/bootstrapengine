//----------------------------------------------------------
// File:		GrCamera.cpp
// Author:		Kevin Bray
// Created:		10-28-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrCamera.h"

// project includes.
#include "GLSubsys.h"
#include "MVec4.h"
#include "MRay.h"

// std c++ includes.
#include <math.h>

//**********************************************************
// class GrCamera
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrCamera::GrCamera()
: _proj( DegToRad( 90.0f ), 1000.0f, 1.0f, MPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -1.0f ) ) )
, _farCull( 1000.0f )
, _dirty( true )
{

}

//----------------------------------------------------------
GrCamera::GrCamera( const MVec3& pos, const MMat3x3& rot, float fov, float farClip, float aspectRatio )
: _proj( fov, farClip, aspectRatio, MPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -1.0f ) ) )
, _pos( pos )
, _rot( rot )
, _farCull( farClip )
, _dirty( true )
{

}

//----------------------------------------------------------
GrCamera::GrCamera( const MVec3& pos, const MMat3x3& rot, const GrProjection& proj )
: _proj( proj )
, _pos( pos )
, _rot( rot )
, _farCull( proj.GetFarDist() )
, _dirty( true )
{
	
}

//----------------------------------------------------------
GrCamera::GrCamera( const MVec3& pos, const MMat3x3& rot, float farCull, float fov, float farClip, float aspectRatio )
: _proj( fov, farClip, aspectRatio, MPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -1.0f ) ) )
, _pos( pos )
, _rot( rot )
, _farCull( farCull )
, _dirty( true )
{
	
}

//----------------------------------------------------------
GrCamera::GrCamera( const MVec3& pos, float width, float height, float farClip )
: _proj( -width/2.0f, width/2.0f, -height/2.0f, height/2.0f, farClip )
, _pos( pos )
, _farCull( farClip )
, _dirty( true )
{

}

//----------------------------------------------------------
GrCamera::GrCamera( const MVec3& pos, float width, float height, float farCull, float farClip )
: _proj( -width/2.0f, width/2.0f, -height/2.0f, height/2.0f, farClip )
, _pos( pos )
, _farCull( farCull )
, _dirty( true )
{

}

//----------------------------------------------------------
GrCamera::~GrCamera()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrCamera::SetPos( const MVec3& pos )
{
	_dirty = true;
	_pos = pos;
}

//----------------------------------------------------------
void
GrCamera::SetRot( const MMat3x3& rot )
{
	_dirty = true;
	_rot = rot;
}

//----------------------------------------------------------
void
GrCamera::LookAt( const MVec3& pos, const MVec3& target )
{
	B_ASSERT( pos != target );

	// mark as dirty.
	_dirty = true;

	// store the position.
	_pos = pos;

	// compute the new z basis.
	Look( pos - target );
}

void 
GrCamera::Look( const MVec3& taintedDir )
{
	MVec3 worldUp( 0.0f, 1.0f, 0.0f );

	// mark as dirty.
	_dirty = true;

	// build the rotation matrix.
	MVec3 dir = taintedDir.Normalized();

	MVec3 right;
	// compute the new x basis.
	if( Abs( dir.GetY() ) > 0.999f )
	{
		// compensate for trying to look directly up or down.
		right = MVec3(1, 0, 0);
	}
	else
	{
		right = dir.Cross( worldUp );
		right.Normalize();
	}

	// compute the new y basis.
	MVec3 up = right.Cross( dir );

	// set the basis vectors.
	_rot.SetCol( 0, right );
	_rot.SetCol( 1, up );
	_rot.SetCol( 2, -dir );
}

//----------------------------------------------------------
void
GrCamera::SetFarCull( float farCull )
{
	_dirty = true;
	_farCull = farCull;
}

//----------------------------------------------------------
void
GrCamera::SetProj( const GrProjection& proj )
{
	_dirty = true;
	_proj = proj;
}

//----------------------------------------------------------
void
GrCamera::SetProjFOV( float fov )
{
	_dirty = true;
	_proj.SetFOV( fov );
}

//----------------------------------------------------------
void
GrCamera::SetProjNearClip( const MPlane& nearPlane )
{
	_dirty = true;
	_proj.SetNearClip( nearPlane );
}

//----------------------------------------------------------
void
GrCamera::SetProjFarClip( float farDist )
{
	_dirty = true;
	_proj.SetFarDist( farDist );
}

//----------------------------------------------------------
void
GrCamera::SetProjAspectRatio( float ratio )
{
	_dirty = true;
	_proj.SetAspect( ratio );
}

//----------------------------------------------------------
void
GrCamera::BuildRay( MRay& ray, float x, float y )
{
	float px = 0.0f;
	float py = 0.0f;
	float ax = 0.0f;
	float ay = 0.0f;

	// build ray offsets from the projection.
	_proj.BuildRayOffsets( px, py, ax, ay, x, y );

	// calculate and set the ray's starting position.
	MVec3 start = GetPos();
	start += px * _rot.GetCol( 0 );
	start += py * _rot.GetCol( 1 );
	ray.SetStart( start );

	// calculate the ray's direction.
	MVec3 lookDir = GetLookDir();
	lookDir += ax * _rot.GetCol( 0 );
	lookDir += ay * _rot.GetCol( 1 );
	ray.SetDir( lookDir );
}

//----------------------------------------------------------
GrCamera
GrCamera::BuildReflectedCamera( const MPlane& plane ) const
{
	// build needed matrices.
	MMat4x4 reflectMat = plane.BuildReflectMatrix();
	MVec3 cameraPos = reflectMat.TransformCoord( _pos );

	// get the reflection rotation.
	MMat3x3 reflectRot;
	reflectMat.GetRotate( reflectRot );

	// calculate the camera rotation and build the plane transform.
	MMat3x3 cameraRot = reflectRot * _rot;

	// build the new camera.
	GrCamera newCam;
	newCam.SetPos( cameraPos );	// reflect the camera's position.
	newCam.SetRot( cameraRot );	// adjust the camera's rotation.
	newCam.SetFarCull( _farCull );
	newCam.SetProjFOV( _proj.GetFOV() );
	newCam.SetProjFarClip( _proj.GetFarDist() );
	newCam.SetProjAspectRatio( _proj.GetAspect() );

	// transform the plane into the reflected camera's space.  We will use
	// this plane as the near clip plane.
	MPlane camSpacePlane = plane;
	camSpacePlane.TransformFast( newCam.GetViewMatrix(), newCam.GetNormalMatrix() );
	newCam.SetProjNearClip( camSpacePlane );

	// return the reflected camera.
	return newCam;
}

//----------------------------------------------------------
MMat4x4             
GrCamera::BuildWorldMatrix() const
{
    return MMat4x4( GetRot(), GetPos() );
}

//----------------------------------------------------------
const MMat4x4&
GrCamera::GetViewMatrix() const
{
	if ( _dirty )
		UpdateMatrices();
	return _viewMatrix;
}

//----------------------------------------------------------
const MMat4x4&
GrCamera::GetInvViewMatrix() const
{
	if ( _dirty )
		UpdateMatrices();
	return _invViewMatrix;
}

//----------------------------------------------------------
const MMat4x4&
GrCamera::GetViewProjMatrix() const
{
	if ( _dirty )
		UpdateMatrices();
	return _viewProjMatrix;
}

//----------------------------------------------------------
const MMat3x3&
GrCamera::GetNormalMatrix() const
{
	if ( _dirty )
		UpdateMatrices();
	return _normalRot;
}

//----------------------------------------------------------
const GrFrustum&
GrCamera::GetFrustum() const
{
	if ( _dirty )
		UpdateMatrices();
	return _frustum;
}

//----------------------------------------------------------
void
GrCamera::AddUserFrustumPlane( const MPlane& plane )
{
	if ( _dirty )
		UpdateMatrices();
	_frustum.AddCullPlane( plane );
}


//----------------------------------------------------------
GrCamera&			
GrCamera::operator = ( const GrCamera& rhs )
{
	SetPos( rhs.GetPos() );
	SetRot( rhs.GetRot() );
	SetProj( rhs.GetProj() );
	return *this;
}

//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrCamera::UpdateMatrices() const
{
	B_ASSERT( _dirty );
//	if ( !_dirty )
//		return;

	// mark the matrices as up to date.
	_dirty = false;

	// create the projection matrix.
	const MMat4x4& projMatrix = _proj.GetMatrix();

	MMat3x3 transViewRot = _rot.Transposed();

	// create the view matrix.
	MVec3 invCamPos( -transViewRot.RotatePoint( _pos ) );
	_viewMatrix = MMat4x4( transViewRot, invCamPos );

	// adjoint transpose.
	_normalRot = transViewRot;

	// create the view-proj matrix.
	_viewProjMatrix = projMatrix * _viewMatrix;
	bool valid = _viewMatrix.Inverse( _invViewMatrix );
	B_ASSERT( valid );

	bool reflection = false;

	// check to see if the view matrix is a reflection.
//	if ( ( _viewMatrix( 0, 0 ) * _viewMatrix( 1, 1 ) * _viewMatrix( 2, 2 ) ) < 0.0f )
//		reflection = true;

	// check to see if the view matrix is a reflection.
	MVec4 row;
	MVec3 xAxis;
	MVec3 yAxis;
	MVec3 zAxis;
	_viewMatrix.GetRow( row, 0 );
	row.MakeVec3( xAxis );
	_viewMatrix.GetRow( row, 1 );
	row.MakeVec3( yAxis );
	_viewMatrix.GetRow( row, 2 );
	row.MakeVec3( zAxis );

	// do we have a reflection?
	if ( xAxis.Cross( yAxis ).Dot( zAxis ) < 0.0f )
		reflection = true;

	// build the frustum ( note that we switch left and right if the current view
	// matrix is a reflection ).
	float right = reflection ? -_proj.GetAspect() : _proj.GetAspect();
	float left = reflection ? _proj.GetAspect() : -_proj.GetAspect();
	float top = 1.0f;
	float bottom = -1.0f;

	// adjust the frustum based on the width/height of the view area.
	left *= -_proj.GetLeft();
	right *= _proj.GetRight();
	bottom *= -_proj.GetBottom();
	top *= _proj.GetTop();

	float fov = _proj.GetFOV();

	// determine the near plane distance of visual projection (not z-depth projection).
	if ( fov > 0.0f )
	{
		float tan = Tan( 0.5f * fov );
		float nearDist = 1 / tan;

		// build the near plane's description.
		MVec3 bottomLeft( left, bottom, -nearDist );
		MVec3 topLeft( left, top, -nearDist );
		MVec3 topRight( right, top, -nearDist );
		MVec3 bottomRight( right, bottom, -nearDist );

		_invViewMatrix.TransformCoordFast( bottomLeft );
		_invViewMatrix.TransformCoordFast( topLeft );
		_invViewMatrix.TransformCoordFast( topRight );
		_invViewMatrix.TransformCoordFast( bottomRight );

		MVec3 lookDir = GetLookDir();

		// update the frustum.
		_frustum = GrFrustum( _pos, bottomLeft, topLeft, topRight, bottomRight, lookDir, 0.0f, _farCull );
	}
	else
	{
		float halfWidth = 0.5f * _proj.GetWidth();
		float halfHeight = 0.5f * _proj.GetHeight();

		// build the frustum planes in view space.
		MPlane left( MVec3( 1.0f, 0.0f, 0.0f ), MVec3( -halfWidth, 0.0f, 0.0f ) );
		MPlane right( MVec3( -1.0f, 0.0f, 0.0f ), MVec3( halfWidth, 0.0f, 0.0f ) );
		MPlane top( MVec3( 0.0f, -1.0f, 0.0f ), MVec3( 0.0f, halfHeight, 0.0f ) );
		MPlane bottom( MVec3( 0.0f, 1.0f, 0.0f ), MVec3( 0.0f, -halfHeight, 0.0f ) );
		MPlane nearPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, 0.0f ) );
		MPlane farPlane( MVec3( 0.0f, 0.0f, 1.0f ), MVec3( 0.0f, 0.0f, -_farCull ) );

		// TODO: create the inverse transpose of this rotation.  This assumes uniform scale.
		MMat3x3 invTrans;
		_invViewMatrix.GetRotate( invTrans );

		// get the look dir.
		MVec3 lookDir = GetLookDir();

		// move the frustum planes into world space.
		left.TransformFast( _invViewMatrix, invTrans );
		right.TransformFast( _invViewMatrix, invTrans );
		top.TransformFast( _invViewMatrix, invTrans );
		bottom.TransformFast( _invViewMatrix, invTrans );
		nearPlane.TransformFast( _invViewMatrix, invTrans );
		farPlane.TransformFast( _invViewMatrix, invTrans );

		// calculate the frustum corners.
		MVec3 nearBottomLeft( -halfWidth, -halfHeight, 0.0f );
		MVec3 nearTopLeft( -halfWidth, halfHeight, 0.0f );
		MVec3 nearTopRight( halfWidth, halfHeight, 0.0f );
		MVec3 nearBottomRight( halfWidth, -halfHeight, 0.0f );

		MVec3 farBottomLeft( -halfWidth, -halfHeight, -_farCull );
		MVec3 farTopLeft( -halfWidth, halfHeight, -_farCull );
		MVec3 farTopRight( halfWidth, halfHeight, -_farCull );
		MVec3 farBottomRight( halfWidth, -halfHeight, -_farCull );

		_invViewMatrix.TransformCoordFast( nearBottomLeft );
		_invViewMatrix.TransformCoordFast( nearTopLeft );
		_invViewMatrix.TransformCoordFast( nearTopRight );
		_invViewMatrix.TransformCoordFast( nearBottomRight );
		_invViewMatrix.TransformCoordFast( farBottomLeft );
		_invViewMatrix.TransformCoordFast( farTopLeft );
		_invViewMatrix.TransformCoordFast( farTopRight );
		_invViewMatrix.TransformCoordFast( farBottomRight );

		// update the frustum.
		_frustum = GrFrustum( _pos, lookDir, nearBottomLeft, nearTopLeft, nearTopRight,
			nearBottomRight, farBottomLeft, farTopLeft, farTopRight, farBottomRight, 0.0f, _farCull );
	}
}
