//----------------------------------------------------------
// File:		GrPolygonBSPTree.cpp
// Author:		Kevin Bray
// Created:		09-30-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonBSPTree.h"

// project includes.
#include "GrPolygonBSPSplit.h"
#include "GrPolygonBSPArea.h"
#include "GrFrustum.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrTextureBase.h"

//**********************************************************
// class GrPolygonBSPTree
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonBSPTree::GrPolygonBSPTree()
: _root( 0 )
, _leaves( 0 )
, _frameId( 1 )
, _cached( false )
{

}

//----------------------------------------------------------
GrPolygonBSPTree::GrPolygonBSPTree( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
								    GrPolygonUberUVMapper& mapper, bool calcPVS, bool uberTexture,
									float uberTexelsPerMeter )
: _root( 0 )
, _leaves( 0 )
, _frameId( 1 )
, _cached( false )
{
	Build( hullPolygons, propPolygons, mapper, calcPVS, uberTexture, uberTexelsPerMeter );
}

//----------------------------------------------------------
GrPolygonBSPTree::GrPolygonBSPTree( UReader& reader )
: _root( 0 )
, _leaves( 0 )
, _frameId( 1 )
, _cached( false )
{
	Load( reader );
}

//----------------------------------------------------------
GrPolygonBSPTree::~GrPolygonBSPTree()
{
	Clean();
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
bool
GrPolygonBSPTree::GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist )
{
	return _root->GetIntersect( hitPos, ray, maxDist );
}

//----------------------------------------------------------
void
GrPolygonBSPTree::Build( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
						 GrPolygonUberUVMapper& mapper, bool calcPVS, bool uberTexture,
						 float uberTexelsPerMeter )
{
	// cleanup the current tree.
	Clean();

	// create the new BSP node tree.
	if ( hullPolygons.GetPolygonCount() > 0 )
	{
		_root = new GrPolygonBSPSplit( hullPolygons, propPolygons, mapper, calcPVS, uberTexture, uberTexelsPerMeter );

		// fill our array of leaves.
		BuildLeafVector();

		// tell all leaves to compress their visibility data.
		const unsigned int leafCount = ( unsigned int )_leaves.size();
		for ( unsigned int i = 0; i < leafCount; ++i )
		{
			if ( calcPVS )
				_leaves[ i ]->CompressPVSData( _leaves );
			else
				_leaves[ i ]->CalcPVSAllVisibleIndices( leafCount );
		}
	}
	else
	{
		_root = new GrPolygonBSPSplit();
	}
}

//----------------------------------------------------------
void
GrPolygonBSPTree::GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const
{
	_root->GetUberTexData( dataArray );
}

//----------------------------------------------------------
void
GrPolygonBSPTree::Load( UReader& reader )
{
	// cleanup the current tree.
	Clean();

	// load the BSP node tree.
	_root = new GrPolygonBSPSplit( reader );

	// fill our array of leaves.
	BuildLeafVector();
}

//----------------------------------------------------------
void
GrPolygonBSPTree::Save( UWriter& writer )
{
	_root->Save( writer );
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
void
GrPolygonBSPTree::BuildLeafVector()
{
	// get all of the leaves.
	_root->GetLeaves( _leaves );
}

//----------------------------------------------------------
void
GrPolygonBSPTree::Clean()
{
	_leaves.clear();
	delete _root;
	_root = 0;
}
