//----------------------------------------------------------
// File:		MSphere.h
// Author:		Kevin Bray
// Created:		09-05-05
//
// Purpose:		To represent a sphere in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

// forward declarations.
class MPlane;
class UWriter;
class MMat4x4;

//**********************************************************
// class MSphere
//**********************************************************
class MSphere
{
public:
	MSphere();
	MSphere( MVec3 center, float radius );
	~MSphere();

	// returns true if the sphere passed in is inside this sphere or if it
	// intersects this sphere in anyway.
	bool			Intersect( const MSphere& sphere ) const;

	// returns true if the sphere passed in is inside this sphere.
	bool			IsInside( const MSphere& sphere ) const;

	void			SetCenter( const MVec3& center )	{	_center = center;	}
	void			SetRadius( float radius );

	const MVec3&	GetCenter() const					{	return _center;		}
	float			GetRadius() const					{	return _radius;		}

	int				GetSide( const MPlane& plane ) const;
	int				GetSection( const MPlane& plane, MVec3& center, float& radius ) const;

	void			Transform( MSphere& sphere, const MMat4x4& transform ) const;
	void			TransformFast( const MMat4x4& transform );

	void			Save( UWriter& writer ) const;
	void			Load( UReader& reader );

private:
	MVec3			_center;
	float			_radius;
};
