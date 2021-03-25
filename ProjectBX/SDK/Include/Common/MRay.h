//----------------------------------------------------------
// File:		MRay.h
// Author:		Kevin Bray
// Created:		07-17-06
//
// Purpose:		To contain a ray.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

// forward declarations.
class MMat4x4;
class MMat3x3;
class MSphere;
class MAABox;
class MOBox;
class MPlane;
class MLineSegment;

//**********************************************************
// class MRay
//**********************************************************
class MRay
{
public:
	MRay();
	MRay( const MVec3& start, const MVec3& dir );
	~MRay();

	void			SetStart( const MVec3& start )	{	_start = start;		}
	void			SetDir( const MVec3& dir );
	void			SetDirFast( const MVec3& dir );

	const MVec3&	GetStart() const				{	return _start;		}
	const MVec3&	GetDir() const					{	return _dir;		}

	// returns true if the ray intersects with the box provided.
	bool			Intersect( const MAABox& box, float maxDist = FLT_MAX ) const;
	bool			Intersect( MVec3& hitLoc, const MAABox& box, float maxDist = FLT_MAX ) const;
	bool			Intersect( const MOBox& box ) const;
	bool			Intersect( MVec3& coord, const MOBox& box ) const;
	bool			Intersect( const MSphere& sphere ) const;

	// transforms the ray.
	MRay			Transform( const MMat4x4& mat, const MMat3x3& normalRot ) const;

	// returns true if the ray intersects with the plane.  It also returns where
	// the ray intersects with the plane.
	bool			Intersect( MVec3& hitPos, const MPlane& plane, float maxDist ) const;

	// returns the distance from the ray to the triangle as well as the barycentric
	// coordinates of the intersection if a intersection occurs.  If no intersection
	// occurs, then the function returns -1.0.
	bool			Intersect( MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2,
							   bool twoSided = false ) const;
	bool			Intersect( float& dist, MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2, bool twoSided = false, float maxDist = FLT_MAX ) const;
	bool			Intersect( MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2,
							   const MVec3& faceNormal, bool twoSided = false ) const;

	// distance from the ray to a point.
	float			Dist( const MVec3& point ) const;

	// distance between two rays.
//	float			Dist( const MRay& ray, float epsilon ) const;

	// distance between a ray and a line segment.  This function returns -1.0f if
	// the line segment is just a point.
	float			Dist( const MLineSegment& segment, float epsilon ) const;
	float			Dist( const MLineSegment& segment, float& rayMu, float& segMu, float epsilon ) const;

private:
	MVec3			_start;
	MVec3			_dir;
};
