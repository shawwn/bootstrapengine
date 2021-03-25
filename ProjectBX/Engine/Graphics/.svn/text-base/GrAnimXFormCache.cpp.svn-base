//----------------------------------------------------------
// File:		GrAnimXFormCache.h
// Author:		Kevin Bray
// Created:		04-08-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrAnimXFormCache.h"

// project includes.
#include "GrModelNode.h"
#include "GrModel.h"


//**********************************************************
// class GrAnimXFormCache
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrAnimXFormCache::GrAnimXFormCache( GrModel* modelTarget )
: _modelTarget( modelTarget )
, _nodeTargets( 0 )
, _xformCache( 0 )
, _bindPose( 0 )
, _xformCount( 0 )
{
	BuildArrays();
}

//----------------------------------------------------------
GrAnimXFormCache::~GrAnimXFormCache()
{
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrAnimXFormCache::SetTarget( GrModel* modelTarget )
{
	Clean();
	_modelTarget = modelTarget;
	BuildArrays();
}

//----------------------------------------------------------
unsigned int
GrAnimXFormCache::GetXFormIdx( const tstring& name )
{
	for ( unsigned int i = 0; i < _xformCount; ++i )
	{
		if ( _nodeTargets[ i ]->GetName() == name )
			return i;
	}

	// not found.
	return ~0;
}

//----------------------------------------------------------
void
GrAnimXFormCache::AccumXForm( unsigned int idx, const GrAnimXForm& xform )
{
	B_ASSERT( idx < _xformCount );
	_xformCache[ idx ].Accumulate( xform );
}

//----------------------------------------------------------
void
GrAnimXFormCache::ApplyToTarget()
{
	MMat4x4 mat;
	for ( unsigned int i = 0; i < _xformCount; ++i )
	{
		_xformCache[ i ].BuildMatrix( mat );
		_nodeTargets[ i ]->SetLocal( _bindPose[ i ] * mat );
		_xformCache[ i ].SetToIdentity();
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrAnimXFormCache::BuildArrays()
{
	// count out the nodes.
	GrModelNode* root = _modelTarget->GetRootNode();
	unsigned int nodeCount = CountNodes( root );

	// build the arrays.
	_xformCount = 0;
	_nodeTargets = new GrModelNode*[ nodeCount ];
	RecordTargets( root );

	// allocate the transform cache (initialized to identity).
	_xformCache = new GrAnimXForm[ nodeCount ];
	_bindPose = new MMat4x4[ nodeCount ];

	for ( unsigned int i = 0; i < nodeCount; ++i )
		_bindPose[ i ] = _nodeTargets[ i ]->GetLocal();
}

//----------------------------------------------------------
unsigned int
GrAnimXFormCache::CountNodes( GrModelNode* tree )
{
	unsigned int count = 1;
	for ( unsigned int i = 0; i < tree->GetChildCount(); ++i )
		count += CountNodes( tree->GetChild( i ) );
	return count;
}

//----------------------------------------------------------
void
GrAnimXFormCache::RecordTargets( GrModelNode* tree )
{
	// record the current target and recurse.
	_nodeTargets[ _xformCount++ ] = tree;
	for ( unsigned int i = 0; i < tree->GetChildCount(); ++i )
		RecordTargets( tree->GetChild( i ) );
}

//----------------------------------------------------------
void
GrAnimXFormCache::Clean()
{
	delete[] _nodeTargets;
	delete[] _xformCache;
	delete[] _bindPose;
	_nodeTargets = 0;
	_xformCache = 0;
	_bindPose = 0;
}
