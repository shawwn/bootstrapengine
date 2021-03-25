//----------------------------------------------------------
// File:		GrPortal.h
// Author:		Kevin Bray
// Created:		09-12-06
//
// Purpose:		To manage a portal.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "MVec3.h"
#include "MPlane.h"

// forward declarations.
class GrPolygonGroup;
class MPlane;
class UReader;
class UWriter;

//**********************************************************
// class GrPortal
//**********************************************************
class GrPortal
{
public:
	typedef UFastArray< MVec3, 4, 4 > VertexArray;

	GrPortal() { }
	GrPortal( UReader& reader );
	GrPortal( const MPlane& plane, const VertexArray& polygon );
	~GrPortal() { }

	// returns true if the portal is coplanar to the plane passed in.
	bool				IsCoplanar( const MPlane& plane, float epsilon ) const;

	// returns true if the current portal is degenerate.
	bool				IsDegenerate() const;

	// splits the portal by the plane passed in.
	int					Split( GrPortal& front, GrPortal& back, const MPlane& splitPlane, const MPlane& orgPlane, float epsilon ) const;

	// reverses the geometry winding order.
	void				ReverseWinding();

	// returns the polygon represented by this portal.
//	const GrPolygon&	GetPolygon() const								{	return _polygon;				}

	unsigned int		GetVertexCount() const;
	const MVec3&		GetVertex( unsigned int idx ) const;

	// checks winding and reverses if necessary.
//	void				CheckWinding( const GrPolygonGroup& group, float epsilon );

	// returns true if the portals overlap.
//	bool				IsOverlapping( const GrPortal& portal, float epsilon ) const;

	void				Save( UWriter& writer );
	void				Load( UReader& reader );

private:
	VertexArray			_polygon;
};
