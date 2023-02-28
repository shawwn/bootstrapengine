//----------------------------------------------------------
// File:		GrBSPPortal.h
// Author:		Kevin Bray
// Created:		09-17-06
//
// Purpose:		To manage portal geometry for PVS generation.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "GrPolygon.h"
#include "GrPolygonBSPPortal.h"

// std c++ includes.
#include <map>

#define _CACHE_CULL_PLANES

// forward declarations.
class MPlane;
class GrMeshInst;
class GrMaterial;
class GrFrustum;
class GrPolygonBSPSplit;
class GrPolygonBSPArea;
class UReader;
class UWriter;

//**********************************************************
// class GrPolygonBSPPortalSet
//**********************************************************
class GrPolygonBSPPortalSet
{
public:
	GrPolygonBSPPortalSet();
	GrPolygonBSPPortalSet( GrPolygonBSPSplit* split, float scale );
	~GrPolygonBSPPortalSet();

	// subtracts a polygon from this geometry.  Returns true if there are still polygons
	// left after the subtraction.  False otherwise.
	bool					Subtract( const GrPolygon& polygon, float epsilon );

	// splits the portal polygons into two groups.
	int						Split( GrPolygonBSPPortalSet& front, GrPolygonBSPPortalSet& back, const MPlane& plane, float epsilon ) const;

	// reverses the winding of all portals in the set.
	void					ReverseWinding();

	// generate frustums from portal geometry.
	unsigned int			GetPortalCount() const;
	const GrPortal&			GetPortal( unsigned int idx ) const;
	void					GenCullPlaneSet( std::vector< MPlane >& planes, const GrPortal& fromPortal, unsigned int portalIdx, bool best, bool cache = true ) const;

	// returns a portal that is the convex hull of all portals.
	const GrPortal&			GetHullPortal() const;

	// area management.
	void					SetOwner( GrPolygonBSPArea* owner );
	void					SetNeighbor( GrPolygonBSPArea* neighbor );

	const MPlane&			GetPlane() const			{	return _plane;		}

	GrPolygonBSPSplit*		GetCreator() const			{	return _creator;	}
	GrPolygonBSPArea*		GetOwner() const			{	return _owner;		}
	GrPolygonBSPArea*		GetNeighbor() const			{	return _neighbor;	}

	// generates a renderable mesh from the portal geometry.  This is for debugging
	// purposes only.
	GrMeshInst* 			GenMesh( URef< GrMaterial > material );

private:
	typedef UFastArray< GrPortal, 1, 1 > PortalArray;

	// plane caching map.
	typedef std::map< unsigned int, std::vector< MPlane > > PlaneMap;

	bool					JoinPortals( unsigned int idx0, unsigned int idx1 );
	void					CreatePlanePolygon( GrPortal::VertexArray& polygon, const MPlane& plane, float scale );

	// array of all polygons.
	// portal caching.
#ifdef _CACHE_CULL_PLANES
	mutable const GrPortal*	_lastFromPortal;
	mutable PlaneMap		_cullPlaneCache;
#endif
	PortalArray				_portals;
	GrPolygonBSPSplit*		_creator;
	GrPolygonBSPArea*		_owner;
	GrPolygonBSPArea*		_neighbor;
	MPlane					_plane;
	mutable GrPortal*		_hullPortal;
	mutable bool			_hullDirty;
};
