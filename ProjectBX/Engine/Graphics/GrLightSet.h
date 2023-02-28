//----------------------------------------------------------
// File:		GrLightSet.h
// Author:		Kevin Bray
// Created:		12-02-06
//
// Purpose:		A utility to help manage a set of lights.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>
#include <map>

// forward declarations.
class GrLight;

//**********************************************************
// class GrLightSet
//**********************************************************
class GrLightSet
{
public:
	GrLightSet();
	~GrLightSet();

	// can be used to iterate through lights.
	unsigned int		GetLightCount() const;
	URef< GrLight >		GetLight( unsigned int idx ) const;

	// adds a light to this group.
	void				AddLight( URef< GrLight > light );
	URef< GrLight >		FindLight( const char* name, int cmpLen = -1 ) const;
	bool				RemoveLight( const char* name );
	bool				RemoveLight( GrLight* light );

	// clears all lights.
	void				Clear();

private:
	typedef std::vector< URef< GrLight > > LightVec;

	unsigned int		FindLightIdx( const char* name, int cmpLen = -1 ) const;

	// vector of lights.
	LightVec			_lights;
};
