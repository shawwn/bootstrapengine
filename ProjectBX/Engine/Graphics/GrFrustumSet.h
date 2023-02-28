//----------------------------------------------------------
// File:		GrFrustumSet.h
// Author:		Kevin Bray
// Created:		09-23-06
//
// Purpose:		To manage a set of frustums.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include <vector>

// forward declarations.
class GrPortalSet;
class GrFrustum;
class GrPolygon;
/*
//**********************************************************
// class GrFrustumSet
//**********************************************************
class GrFrustumSet
{
public:
	// creates a bunch of frustums based on the frustum(s) and the portals passed in.
	GrFrustumSet( const GrFrustum& baseFrustum );
	GrFrustumSet( const GrFrustum& baseFrustum, const GrPortalSet& portals, float tolerance );
	GrFrustumSet( const GrFrustumSet& baseFrustums, const GrPortalSet& portals, float tolerance );
	GrFrustumSet( const GrFrustumSet& baseFrustums, const std::vector< GrPortalSet* >& portals, float tolerance );
	~GrFrustumSet();

	// returns the number of frustums.
	unsigned int	GetFrustumCount() const		{	return ( unsigned int )_frustums.size();	}

	// modifies current frustums using the portals passed in.
	void			AddPortals( const GrPortalSet& portals, float tolerance );
	void			AddPortalSets( const std::vector< GrPortalSet* >& portalSets, float tolerance );

	// returns true if a polygon is visible to any of the frustums.
	bool			IsPolygonVisible( const GrPolygon& polygon ) const;

private:
	typedef std::vector< GrFrustum* > FrustumArray;

	void			GenFrustumsFromPortals( const GrFrustum& baseFrustum, const GrPortalSet& portals, float tolerance );
	void			GenFrustumsFromPortalSets( const GrFrustum& baseFrustum, const std::vector< GrPortalSet* >& portalSets, float tolerance );

	FrustumArray	_frustums;

};
*/
