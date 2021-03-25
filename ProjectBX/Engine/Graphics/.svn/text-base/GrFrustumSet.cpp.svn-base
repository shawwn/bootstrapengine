//----------------------------------------------------------
// File:		GrFrustumSet.cpp
// Author:		Kevin Bray
// Created:		09-23-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrFrustumSet.h"

// project includes.
#include "GrFrustum.h"

/*
//**********************************************************
// class GrFrustumSet
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrFrustumSet::GrFrustumSet( const GrFrustum& baseFrustum )
{
	_frustums.push_back( new GrFrustum( baseFrustum ) );
}

//----------------------------------------------------------
GrFrustumSet::GrFrustumSet( const GrFrustum& baseFrustum, const GrPortalSet& portals, float tolerance )
{
	GenFrustumsFromPortals( baseFrustum, portals, tolerance );
}

//----------------------------------------------------------
GrFrustumSet::GrFrustumSet( const GrFrustumSet& baseFrustums, const GrPortalSet& portals, float tolerance )
{
	// for each frustum...
	const unsigned int frustumCount = ( unsigned int )baseFrustums._frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
	{
		// get the current frustum.
		GrFrustum* curFrustum = baseFrustums._frustums[ i ];
		GenFrustumsFromPortals( *curFrustum, portals, tolerance );
	}
}

//----------------------------------------------------------
GrFrustumSet::GrFrustumSet( const GrFrustumSet& baseFrustums, const std::vector< GrPortalSet* >& portalSets, float tolerance )
{
	// for each frustum...
	const unsigned int frustumCount = ( unsigned int )baseFrustums._frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
	{
		// get the current frustum.
		GrFrustum* curFrustum = baseFrustums._frustums[ i ];
		GenFrustumsFromPortalSets( *curFrustum, portalSets, tolerance );
	}
}

//----------------------------------------------------------
GrFrustumSet::~GrFrustumSet()
{
	// free up current frustums.
	const unsigned int frustumCount = ( unsigned int )_frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
		delete _frustums[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrFrustumSet::AddPortals( const GrPortalSet& portals, float tolerance )
{
	// for each frustum...
	const unsigned int frustumCount = ( unsigned int )_frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
	{
		// get the current frustum.
		GrFrustum* curFrustum = _frustums[ 0 ];

		// generate new frustums from the current frustum and the portals.
		GenFrustumsFromPortals( *curFrustum, portals, tolerance );

		// remove the current frustum.
		_frustums.erase( _frustums.begin() );
	}
}

//----------------------------------------------------------
void
GrFrustumSet::AddPortalSets( const std::vector< GrPortalSet* >& portalSets, float tolerance )
{
	// for each frustum...
	const unsigned int frustumCount = ( unsigned int )_frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
	{
		// get the current frustum.
		GrFrustum* curFrustum = _frustums[ 0 ];

		// generate new frustums from the current frustum and the portals.
		GenFrustumsFromPortalSets( *curFrustum, portalSets, tolerance );

		// remove the current frustum.
		_frustums.erase( _frustums.begin() );
	}
}

//----------------------------------------------------------
bool
GrFrustumSet::IsPolygonVisible( const GrPolygon& polygon ) const
{
	// if the polygon is visible to any of the frustums, then the polygon is
	// considered visible.
	const unsigned int frustumCount = ( unsigned int )_frustums.size();
	for ( unsigned int i = 0; i < frustumCount; ++i )
	{
		if ( _frustums[ i ]->IsPolygonInside( polygon ) )
			return true;
	}

	// return false since the polygon wasn't visible to any of the frustums.
	return false;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrFrustumSet::GenFrustumsFromPortals( const GrFrustum& baseFrustum, const GrPortalSet& portals, float tolerance )
{
	// for each portal...
	const unsigned int portalCount = portals.GetPortalCount();
	for ( unsigned int j = 0; j < portalCount; ++j )
	{
		// if the current portal is visible, then we need to create a new
		// frustum for it.
		if ( portals.IsPortalVisible( baseFrustum, j ) )
		{
			// create a new frustum and add portal clipping planes to it.
			GrFrustum* newFrustum = new GrFrustum( baseFrustum );
			portals.AddPortalToFrustum( *newFrustum, j );
			_frustums.push_back( newFrustum );
		}
	}
}

//----------------------------------------------------------
void
GrFrustumSet::GenFrustumsFromPortalSets( const GrFrustum& baseFrustum, const std::vector< GrPortalSet* >& portalSets, float tolerance )
{
	// for each portal set...
	const unsigned int portalSetCount = ( unsigned int )portalSets.size();
	for ( unsigned int i = 0; i < portalSetCount; ++i )
		GenFrustumsFromPortals( baseFrustum, *portalSets[ i ], tolerance );
}
*/
