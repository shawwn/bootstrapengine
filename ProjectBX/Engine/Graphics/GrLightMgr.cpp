//----------------------------------------------------------
// File:		GrLightMgr.cpp
// Author:		Kevin Bray
// Created:		03-12-06
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrLightMgr.h"

// project includes.
#include "GrLight.h"
#include "RRsrcMgr.h"
#include "GrParse.h"

// std c++ includes.
#include <set>

// initialize the singleton pointer to null.
GrLightMgr* gGrLightMgr = 0;

//**********************************************************
// class GrLightMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrLightMgr::GrLightMgr()
{
	B_ASSERT( gGrLightMgr == 0 );
	gGrLightMgr = this;

	// register parsers.
	B_ASSERT( gRRsrcMgr );
	gRRsrcMgr->RegisterScriptParser( GR_KW_LIGHT, ParseLight );
}

//----------------------------------------------------------
GrLightMgr::~GrLightMgr()
{
	CollectGarbage();

	B_ASSERT( gGrLightMgr != 0 );
	gGrLightMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrLightMgr::SetLight( const UPath& id, const SLightInfo& lightInfo )
{
	LightMap::iterator iter = _lightMap.find( id );
	if ( iter != _lightMap.end() )
	{
		iter->second->Rebuild( lightInfo );
		return;
	}
	GrLight* light = new GrLight( id, lightInfo );
	AddLight( light );
}

//----------------------------------------------------------
URef< GrLight >
GrLightMgr::GetLight( const UPath& id, tstring& error )
{
	// check to see if the light is already being used.
	LightMap::iterator iter = _lightMap.find( id );
	if ( iter != _lightMap.end() )
		return iter->second;

	// if we didn't find it, determine what script we need to load.
	tstring scriptName;
	GetScriptName( scriptName, id.GetPathString() );

	// load the script needed.
	if ( !gRRsrcMgr->LoadScript( scriptName, "light", error ) )
		return 0;

	// try to find the light now that the script was loaded.
	iter = _lightMap.find( id );
	if ( iter != _lightMap.end() )
		return iter->second;

	// return an error since the light was not found.
	error = "Light " + id.GetPathString() + " not found.";
	return 0;
}

//----------------------------------------------------------
URef< GrLight >
GrLightMgr::FindLight( const UPath& id )
{
	// check to see if the light is already being used.
	LightMap::iterator iter = _lightMap.find( id );
	if ( iter != _lightMap.end() )
		return iter->second;

	// return null to indicate that the light doesn't exist.
	return 0;
}

//----------------------------------------------------------
URef< GrLight >	
GrLightMgr::CreateLight( const UPath& id, const SLightInfo& lightInfo )
{
	LightMap::iterator iter = _lightMap.find( id );
	if ( iter != _lightMap.end() )
		return 0;

	GrLight* light = new GrLight( id, lightInfo );
	AddLight( light );
	return light;
}

//----------------------------------------------------------
URef< GrLight >
GrLightMgr::GetOrCloneLight( const UPath& newLightId, const UPath& fromLight, tstring& error )
{
	// if a light with the new id already exists, simply return it.
	URef< GrLight > curLight = FindLight( newLightId );
	if ( curLight != 0 )
		return curLight;

	// try to get the light we need to clone.
	URef< GrLight > light = GetLight( fromLight, error );
	if ( !light )
		return 0;

	// clone the light and add it to our list of lights.
	URef< GrLight > newLight = light->Clone( newLightId );
	AddLight( newLight );

	// return the new light.
	return newLight;
}

//----------------------------------------------------------
URef< GrLight >
GrLightMgr::GetOrCloneLight( const UPath& newLightId, const GrLight* fromLight )
{
	// make sure a valid light to clone was passed in.
	B_ASSERT( fromLight != 0 );

	// if a light with the new id already exists, simply return it.
	URef< GrLight > light = FindLight( newLightId );
	if ( light != 0 )
		return light;

	// clone the light and add it to our list of lights.
	URef< GrLight > newLight = fromLight->Clone( newLightId );
	AddLight( newLight );

	// return the new light.
	return newLight;
}

//----------------------------------------------------------
unsigned int
GrLightMgr::GetLightCount()
{
	return ( unsigned int )_lightVec.size();
}

//----------------------------------------------------------
URef< GrLight >
GrLightMgr::GetLightByIdx( unsigned int idx )
{
	B_ASSERT( idx < GetLightCount() );
	return _lightVec[ idx ];
}

//----------------------------------------------------------
bool
GrLightMgr::ReloadLight( const tstring& name, tstring& error )
{
	tstring scriptName;
	GetScriptName( scriptName, name );
	return gRRsrcMgr->LoadScript( scriptName, "light", error );
}

//----------------------------------------------------------
void
GrLightMgr::ReloadAll()
{
	// get a list of all of the scripts we need to reload.
	tstring scriptName;
	std::set< tstring > scripts;
	LightVec::iterator iter = _lightVec.begin();
	const LightVec::iterator end = _lightVec.end();
	for ( ; iter != end; ++iter )
	{
		GetScriptName( scriptName, ( *iter )->GetId().GetPathString() );
		scripts.insert( scriptName );
	}

	// go through the scripts one by one and reload.
	tstring errors;
	std::set< tstring >::iterator scriptIter = scripts.begin();
	const std::set< tstring >::iterator scriptEnd = scripts.end();
	for ( ; scriptIter != scriptEnd; ++scriptIter )
	{
		gRRsrcMgr->LoadScript( *scriptIter, "light", errors );
	}
}

//----------------------------------------------------------
void
GrLightMgr::Update()
{
	// go through each light and update it.
	LightVec::iterator iter = _lightVec.begin();
	const LightVec::iterator end = _lightVec.end();
	for ( ; iter != end; ++iter )
	{
		( *iter )->Update();
	}
}

//----------------------------------------------------------
void
GrLightMgr::CollectGarbage()
{
	LightVec::iterator iter = _lightVec.begin();
	while ( iter != _lightVec.end() )
	{
		if ( ( *iter )->GetRef() == 1 )
		{
			// find the light in the name-to-light map and remove it.
			LightMap::iterator mapIter = _lightMap.find( ( *iter )->GetId() );
			B_ASSERT( mapIter != _lightMap.end() );
			_lightMap.erase( mapIter );

			// remove the light from the vector.
			iter = _lightVec.erase( iter );
		}
		else
			++iter;
	}

	// clear out our lists.
	_lightVec.clear();
	_lightMap.clear();
}

//----------------------------------------------------------
void
GrLightMgr::GetScriptName( tstring& scriptName, const tstring& lightName )
{
	// if we didn't find it, determine what script we need to load.
	scriptName = lightName;
	size_t scriptPathEnd = scriptName.find_last_of( '/' );
	if ( scriptPathEnd != tstring::npos )
	{
		scriptName = scriptName.substr( 0, scriptPathEnd );
	}

	scriptName += "/"GR_LIGHTS_FILENAME;
}


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
void
GrLightMgr::AddLight( GrLight* light )
{
	_lightMap[ light->GetId() ] = light;
	_lightVec.push_back( light );
}
