//----------------------------------------------------------
// File:		GrCoplanarPolygonGroup.h
// Author:		Kevin Bray
// Created:		09-13-06
//
// Purpose:		To represent a group of coplanar polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "MPlane.h"
#include "GrPolygon.h"

//**********************************************************
// class GrCoplanarPolygonGroup
//**********************************************************
class GrCoplanarPolygonGroup
{
public:
	GrCoplanarPolygonGroup( const MPlane& plane );
	~GrCoplanarPolygonGroup();

	// returns the plane that all of the geometry lies on.
	const MPlane&			GetPlane() const			{	return _plane;						}

	// adds the polygon to the group.  The polygon must be coplanar.
	void					AddPolygonIdx( unsigned int index );

	// polygon management.
	unsigned int			GetPolygonCount() const		{	return _polygons.GetElemCount();	}
	unsigned int			GetPolygonIdx( unsigned int idx ) const;

private:
	typedef UFastArray< unsigned int > PolygonArray;

	MPlane					_plane;
	PolygonArray			_polygons;
};
