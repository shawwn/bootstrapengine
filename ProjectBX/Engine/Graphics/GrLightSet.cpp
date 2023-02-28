//----------------------------------------------------------
// File:		GrLightSet.cpp
// Author:		Kevin Bray
// Created:		12-02-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrLightSet.h"

// project includes.
#include "GrLight.h"


//**********************************************************
// class GrLightSet
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrLightSet::GrLightSet()
{

}

//----------------------------------------------------------
GrLightSet::~GrLightSet()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrLightSet::GetLightCount() const
{
	return ( unsigned int )_lights.size();
}

//----------------------------------------------------------
URef< GrLight >
GrLightSet::GetLight( unsigned int idx ) const
{
	return _lights[ idx ];
}

//----------------------------------------------------------
void
GrLightSet::AddLight( URef< GrLight > light )
{
	// make sure the light hasn't already been added.
	unsigned int lightCount = GetLightCount();
	for ( unsigned int i = 0; i < lightCount; ++i )
	{
		if ( _lights[ i ] == light )
			return;
	}

	// add the light to our list and return it's new index.
	_lights.push_back( light );
}

//----------------------------------------------------------
URef< GrLight >
GrLightSet::FindLight( const char* name, int cmpLen ) const
{
	// find the index of the desired light.
	unsigned int idx = FindLightIdx( name, cmpLen );

	// return null if the light wasn't found.
	if ( idx == ~0 )
		return 0;

	// return the light.
	return _lights[ idx ];
}

//----------------------------------------------------------
bool
GrLightSet::RemoveLight( const char* name )
{
	// try to remove the light.
	unsigned int idx = FindLightIdx( name );

	// simply return false if we couldn't find the light.
	if ( idx == ~0 )
		return false;

	// remove the light and return true.
	_lights.erase( _lights.begin() + idx );
	return true;
}

//----------------------------------------------------------
bool
GrLightSet::RemoveLight( GrLight* light )
{
	// try to remove the light.
	unsigned int idx = FindLightIdx( light->GetId().GetPathString().c_str() );

	// simply return false if we couldn't find the light.
	if ( idx == ~0 )
		return false;

	// ensure that the pointers are the same.  If the light manager is
	// used, then it should be impossible for lights to have the same
	// name since they're prefixed with scene and model information.
	B_ASSERT( _lights[ idx ] == light );

	// remove the light and return true.
	_lights.erase( _lights.begin() + idx );
	return true;
}

//----------------------------------------------------------
void
GrLightSet::Clear()
{
	_lights.clear();
}

//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrLightSet::FindLightIdx( const char* name, int cmpLen ) const
{
	// try to find the light.
	unsigned int lightCount = GetLightCount();
	if ( cmpLen < 0 )
	{
		for ( unsigned int i = 0; i < lightCount; ++i )
		{
			if ( _stricmp( _lights[ i ]->GetId().GetPathString().c_str(), name ) == 0 )
				return i;
		}
	}
	else
	{
		for ( unsigned int i = 0; i < lightCount; ++i )
		{
			if ( _strnicmp( _lights[ i ]->GetId().GetPathString().c_str(), name, cmpLen ) == 0 )
				return i;
		}
	}

	// not found.
	return ~0;
}
