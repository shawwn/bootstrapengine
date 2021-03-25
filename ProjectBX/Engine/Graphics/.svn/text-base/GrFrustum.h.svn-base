//----------------------------------------------------------
// File:		GrFrustum.h
// Author:		Kevin Bray
// Created:		11-10-04
//
// Purpose:		To link sectors together.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MPlane.h"
#include "MVec3.h"
#include "UFastArray.h"
#include "MSphere.h"

// forward declarations.
struct SQuad;
struct SVec3;
class MAABox;
class MOBox;
class GrPolygon;
class MRay;

//**********************************************************
// class GrFrustum
//**********************************************************
class GrFrustum
{
public:
	typedef UFastArray< MPlane, 20, 6 > PlaneArray;

	enum ECULLPLANE
	{
		ECP_LEFT,
		ECP_RIGHT,
		ECP_TOP,
		ECP_BOTTOM,
		ECP_NEAR,
		ECP_FAR,
	};

	// creates an empty frustum.  This is so a frustum can be created and then defined later.
	// An empty frustum will have no area.
	GrFrustum() { }

	// creates a frustum with no area.
	GrFrustum( const MVec3& lookPoint, const MVec3& lookDir, const MVec3& nearBottomLeft, const MVec3& nearTopLeft,
		const MVec3& nearTopRight, const MVec3& nearBottomRight, const MVec3& farBottomLeft, const MVec3& farTopLeft,
		const MVec3& farTopRight, const MVec3& farBottomRight, float nearDist, float farDist );
	// creates a frustum from a point and a quad.
	GrFrustum( const MVec3& lookPoint, const MVec3& bottomLeft, const MVec3& topLeft, const MVec3& topRight,
		const MVec3& bottomRight, const MVec3& lookDir, float nearDist, float farDist );
	GrFrustum( const MVec3& lookPoint, const SQuad& quad, const MVec3& lookDir, float nearDist, float farDist );
	GrFrustum( const GrFrustum& frustum, const SQuad& quad, const MVec3& lookDir, float nearDist, float farDist );
	GrFrustum( const GrFrustum& frustum );
	~GrFrustum();

	void				AddCullPlane( const MPlane& plane );
	const PlaneArray&	GetCullPlanes() const									{	return _planes;				}
	const MVec3&		GetPos() const											{	return _pos;				}

	const MPlane&		GetCullPlane( ECULLPLANE plane ) const;

	// returns the 'best' cross section of the frustum.
	void				GetCrossSection( UFastArray< MVec3 >& hull, const MPlane& plane ) const;
	void				GetPerpCrossSection( UFastArray< MVec3 >& hull, float dist ) const;

	// builds a hull using the frustum corners.  This does NOT include any additional planes,
	// only the 6 original planes.  This returns 18 vertices in a triangle list format.
	void				BuildHull( SVec3* verts ) const;

	// the frustum is only a hull if it's position is on or is in the positive half-space of
	// all of the planes.
	bool				IsHull() const											{	return _isHull;				}

	// assignment operator.
	GrFrustum&			operator = ( const GrFrustum& frustum );

	// These determine how an object is relative to the frustum.  The IsXXXInside functions
	// return true if an object is fully or partially inside.
	bool				IsSphereInside( const MVec3& pt, float radius ) const;			// true if fully or partially inside.
	bool				IsSphereInside( const MSphere& sphere ) const	{	return IsSphereInside( sphere.GetCenter(), sphere.GetRadius() );	}
	bool				IsPointInside( const MVec3& pt ) const;
	bool				IsAABoxInside( const MAABox& box ) const;
	bool				IsOBoxInside( const MOBox& box ) const;
//	bool				IsPortalInside( const GrPortal& portal ) const;
	int					FindRelativeLoc( const MPlane& plane ) const;					// -1 = outside, 1 = inside, 0 = both.
	int					FindRelativeLoc( const MVec3& pt0, const MVec3& pt1 ) const;	// -1 = outside, 1 = inside, 0 = both.
	bool				IsFrustumInside( const GrFrustum& frustum ) const;				// true if fully or partially inside.
	bool				IsPolygonInside( const GrPolygon& polygon ) const;
	bool				IsTriangleInside( const SVec3& p0, const SVec3& p1, const SVec3& p2 ) const;
	bool				IsRayInside( const MRay& ray, float epsilon ) const;

	void				DebugDraw() const;

	// accessors.
	const MVec3&		GetFarBottomLeft() const								{	return _farBottomLeft;		}
	const MVec3&		GetFarTopLeft() const									{	return _farTopLeft;			}
	const MVec3&		GetFarTopRight() const									{	return _farTopRight;		}
	const MVec3&		GetFarBottomRight() const								{	return _farBottomRight;		}
	const MVec3&		GetNearBottomLeft() const								{	return _nearBottomLeft;		}
	const MVec3&		GetNearTopLeft() const									{	return _nearTopLeft;		}
	const MVec3&		GetNearTopRight() const									{	return _nearTopRight;		}
	const MVec3&		GetNearBottomRight() const								{	return _nearBottomRight;	}


private:
	MVec3				_pos;
	MVec3				_farBottomLeft;
	MVec3				_farTopLeft;
	MVec3				_farTopRight;
	MVec3				_farBottomRight;
	MVec3				_nearBottomLeft;
	MVec3				_nearTopLeft;
	MVec3				_nearTopRight;
	MVec3				_nearBottomRight;
	PlaneArray			_planes;
	bool				_isHull;

//	MPlane				_left;
//	MPlane				_right;
//	MPlane				_top;
//	MPlane				_bottom;
};
