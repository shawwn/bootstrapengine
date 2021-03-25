//----------------------------------------------------------
// File:		Projection.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Projection.h"

// engine headers.
#include "GrProjection.h"

//**********************************************************
// value class Projection
//**********************************************************

//----------------------------------------------------------
Bootstrap::Projection::Projection( const GrProjection& projection )
	: _fov( projection.GetFOV() )
	, _nearClip( projection.GetNearClip() )
	, _farDist( projection.GetFarDist() )
	, _aspect( projection.GetAspect() )
	, _left( projection.GetLeft() )
	, _right( projection.GetRight() )
	, _bottom( projection.GetBottom() )
	, _top( projection.GetTop() )
	, _dirty( true )
{
}

//----------------------------------------------------------
GrProjection 
Bootstrap::Projection::ToGrProjection()
{
	if( IsOrtho )
		return GrProjection( Left, Right, Bottom, Top, FarDistance );
	else
		return GrProjection( FOV, FarDistance, AspectRatio, NearClip.ToMPlane() );
}

//----------------------------------------------------------
Bootstrap::Projection::Projection( float fov, float farDist, float aspect, Plane nearClip )
	: _fov( fov )
	, _nearClip( nearClip )
	, _farDist( farDist )
	, _aspect( aspect )
	, _left( -1.0f )
	, _right( 1.0f )
	, _bottom( -1.0f )
	, _top( 1.0f )
	, _dirty( true )
{
	if( fov < 0.0f )
		throw gcnew System::ArgumentException( "[fov] must be >= 0.0" );
}

//----------------------------------------------------------
Bootstrap::Projection::Projection(float left, float right, float bottom, float top, float farDist )
	: _fov( 0.0f )
	, _farDist( farDist )
	, _nearClip( Vector3( 0.0f, 0.0f, -1.0f ), Vector3( 0.0f, 0.0f, -1.0f ) )
	, _aspect( 1.0f )
	, _left( left )
	, _right( right )
	, _bottom( bottom )
	, _top( top )
	, _dirty( true )
{
}

//----------------------------------------------------------
void 
Bootstrap::Projection::BuildRayOffsets( [Out] float% px, [Out] float% py, [Out] float% ax, [Out] float% ay, float x, float y )
{
	// either adjust the starting position or adjust the angles.
	if ( IsOrtho )
	{
		// calculate interpolants.
		float tx = 0.5f * x + 0.5f;
		float ty = 0.5f * y + 0.5f;

		// calculate the starting position offset.
		px = LerpF( _left, _right, tx );
		py = LerpF( _bottom, _top, ty );
		ax = 0.0f;
		ay = 0.0f;
	}
	else
	{
		// calculate variables that reflect the current projection.
		float projNear = 1.0f / Tan( _fov * 0.5f );
		float xOffset = ( _right + _left ) / ( _right - _left );
		float yOffset = ( _top + _bottom ) / ( _top - _bottom );
		float xScale = 2.0f / ( _right - _left );
		float yScale = 2.0f / ( _top - _bottom );
		xScale /= _aspect;

		// calculate the new angle offsets.
		ax = ( x + xOffset ) / ( xScale * projNear );
		ay = ( y + yOffset ) / ( yScale * projNear );
		px = 0.0f;
		py = 0.0f;
	}
}

//----------------------------------------------------------
void 
Bootstrap::Projection::FOV::set( float fov )
{
	if( fov < 0.0f )
		throw gcnew System::ArgumentException( "[fov] must be >= 0.0" );

	_dirty = true;
	_fov = fov;
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Projection::Transform::get()
{
	// update the cached projection matrix if necessary.
	if ( _dirty )
	{
		// make sure the width and height is valid.
		if( (Abs( _right - _left ) <= 0.0001f)
		 || (Abs( _top - _bottom ) <= 0.0001f) )
		{
			throw gcnew System::InvalidProgramException( "A Projection has invalid dimensions." );
		}

		float xScale = 2.0f / ( _right - _left );
		float yScale = 2.0f / ( _top - _bottom );
		float xOffset = ( _right + _left ) / ( _right - _left );
		float yOffset = ( _top + _bottom ) / ( _top - _bottom );

		// calculate a projection matrix based on whether or not the
		// current configuration is orthographic or perspective.
		if ( !IsOrtho )
		{
			// calculate matrix components.
			float projNear = 1.0f / Tan( _fov * 0.5f );

			// now build the matrix.
			_mat.M11 = xScale * projNear / ( _aspect );
			_mat.M12 = 0.0f;
			_mat.M13 = xOffset;
			_mat.M14 = 0.0f;

			_mat.M21 = 0.0f;
			_mat.M22 = yScale * projNear;
			_mat.M23 = yOffset;
			_mat.M24 = 0.0f;

			_mat.M31 = 0.0f;
			_mat.M32 = 0.0f;
			_mat.M33 = -1.0f; //( projNear + _farDist ) / ( projNear - _farDist );
			_mat.M34 = -1.0f; //_farDist * projNear / ( projNear - _farDist );

			_mat.M41 = 0.0f;
			_mat.M42 = 0.0f;
			_mat.M43 = -1.0f;
			_mat.M44 = 0.0f;

			// incorperate the near clip plane into the matrix by applying a shear
			// on the Z axis.
			AddNearClip();
		}
		else
		{
			// make sure a valid far distance was passed in.
			if( _farDist <= 0.01f )
				throw gcnew System::InvalidProgramException( "A Projection contains an invalid FarDistance." );

			// now build the matrix.
			_mat.M11 = xScale;
			_mat.M12 = 0.0f;
			_mat.M13 = 0.0f;
			_mat.M14 = -xOffset;

			_mat.M21 = 0.0f;
			_mat.M22 = yScale;
			_mat.M23 = 0.0f;
			_mat.M24 = -yOffset;

			_mat.M31 = 0.0f;
			_mat.M32 = 0.0f;
			_mat.M33 = ( -1.0f / _farDist );
			_mat.M34 = 0.0f;

			_mat.M41 = 0.0f;
			_mat.M42 = 0.0f;
			_mat.M43 = 0.0f;
			_mat.M44 = 1.0f;
		}

		// clear the dirty flag.
		_dirty = false;
	}

	// return the cached matrix.
	return _mat;
}

//----------------------------------------------------------
void 
Bootstrap::Projection::AddNearClip()
{
	// Calculate the clip-space corner point opposite the clipping plane
	// as ( sgn( clipPlane.x ), sgn( clipPlane.y ), 1, 1 ) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix
	float qx = ( Sign( _nearClip.Normal.X ) + _mat.M13 ) / _mat.M11;
	float qy = ( Sign( _nearClip.Normal.Y ) + _mat.M23 ) / _mat.M22;
	float qz = -1.0f;
	float qw = ( 1.0f + _mat.M33 ) / _mat.M34;

	float scale = 2.0f / ( 
		( qx * _nearClip.Normal.X ) +
		( qy * _nearClip.Normal.Y ) +
		( qz * _nearClip.Normal.Z ) +
		( qw * _nearClip.D ) );

	Vector3 scaledNormal = scale * _nearClip.Normal;
	float scaledD = scale * _nearClip.D;

	// place the scaled plane into the third row of the projection matrix.
	_mat.M31 = scaledNormal.X;
	_mat.M32 = scaledNormal.Y;
	_mat.M33 = scaledNormal.Z + 1.0f;
	_mat.M34 = scaledD + 1.0f;
	/*
	// since we're not using a far clip plane, we don't need to adjust for use
	// with the far clip plane.  That means we can just shove this plane into
	// the projection matrix.
	const MVec3& clipNormal = _nearClip.GetNormal();
	_mat.M31 = clipNormal.GetX();
	_mat.M32 = clipNormal.GetY();
	_mat.M33 = clipNormal.GetZ();
	_mat.M34 = _nearClip.GetD();
	*/
}