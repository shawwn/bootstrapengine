//----------------------------------------------------------
// File:		GrCamera.h
// Author:		Kevin Bray
// Created:		10-28-04
//
// Purpose:		To provide a camera in a 3D scene.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrFrustum.h"
#include "MVec3.h"
#include "MMat3x3.h"
#include "MMat4x4.h"
#include "GrProjection.h"

class MRay;

//**********************************************************
// class GrCamera
//**********************************************************
class GrCamera
{
public:
	GrCamera();
	GrCamera( const MVec3& pos, const MMat3x3& rot, float fov, float farClip, float aspectRatio );
	GrCamera( const MVec3& pos, const MMat3x3& rot, float farCull, float fov, float farClip, float aspectRatio );
	GrCamera( const MVec3& pos, const MMat3x3& rot, const GrProjection& proj );
  	GrCamera( const MVec3& pos, float width, float height, float farClip );
  	GrCamera( const MVec3& pos, float width, float height, float farCull, float farClip );
	~GrCamera();

	// set position, lookdir, etc.
	void				SetPos( const MVec3& pos );
	void				SetRot( const MMat3x3& rot );
	void				Look( const MVec3& direction );
	void				LookAt( const MVec3& pos, const MVec3& target );
	void				SetFarCull( float farCull );

	void				SetProj( const GrProjection& proj );
	void				SetProjFOV( float fov );			// horizontal FOV in radians.
	void				SetProjNearClip( const MPlane& nearPlane );
	void				SetProjFarClip( float farDist );
	void				SetProjAspectRatio( float ratio );	// width/height.

	// build a ray in world space given a viewport coordinate.
	void				BuildRay( MRay& ray, float x, float y );

	// reflects the camera across the plane passed in.
	GrCamera			BuildReflectedCamera( const MPlane& plane ) const;

	// camera info accessors.
	const MVec3&		GetPos() const				{	return _pos;				}
	const MMat3x3&		GetRot() const				{	return _rot;				}
	const GrProjection&	GetProj() const				{	return _proj;				}

	// camera matrices.
    MMat4x4             BuildWorldMatrix() const;
	const MMat4x4&		GetViewMatrix() const;		//{	return _viewMatrix;			}
	const MMat4x4&		GetInvViewMatrix() const;	//{	return _invViewMatrix;		}
	const MMat4x4&		GetProjMatrix() const		{	return _proj.GetMatrix();	}
	const MMat4x4&		GetViewProjMatrix() const;	//{	return _viewProjMatrix;		}
	const MMat3x3&		GetNormalMatrix() const;	//{	return _normalRot;			}
	const GrFrustum&	GetFrustum() const;			//{	return _frustum;			}

	// returns the current direction the camera looking.
	MVec3				GetSideDir() const			{	return  _rot.GetCol( 0 );	}
	MVec3				GetUpDir() const			{	return  _rot.GetCol( 1 );	}
	MVec3				GetLookDir() const			{	return -_rot.GetCol( 2 );	}

	// adds a frustum plane to the current frustum.  Note that user frustum
	// planes are cleared every time the camera is changed.
	void				AddUserFrustumPlane( const MPlane& plane );

	GrCamera&			operator = ( const GrCamera& rhs );

private:
	// builds the view, proj, and viewProj matrices.
	void				UpdateMatrices() const;

	GrProjection		_proj;
	MVec3				_pos;
	MMat3x3				_rot;
	float				_farCull;			// far culling distance.
	mutable MMat4x4		_viewProjMatrix;
	mutable MMat4x4		_viewMatrix;
	mutable MMat4x4		_invViewMatrix;
	mutable MMat3x3		_normalRot;
	mutable GrFrustum	_frustum;
	mutable bool		_dirty;
};
