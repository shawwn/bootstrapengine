//----------------------------------------------------------
// File:		GrModelMgr.h
// Author:		Kevin Bray
// Created:		01-31-06
//
// Purpose:		To manage models.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"
#include "URef.h"

// std c++ includes.
#include <map>

// forward declarations.
class GrModel;

//**********************************************************
// class GrModelMgr
//**********************************************************
class GrModelMgr
{
public:
	GrModelMgr();
	~GrModelMgr();

	// this should be called once per frame.
	void			Update( unsigned int timeDelta );

	// gets a model or it's clone.
	URef< GrModel >	GetModel( const UPath& filePath, const UPath& instanceName, bool clone, bool cloneLights );
	void			AddModel( URef< GrModel > model );

private:
	struct SModelEntry
	{
		URef< GrModel > model;
		unsigned int lastUsed;
	};
	typedef std::map< UPath, SModelEntry > ModelMap;

	void			CollectGarbage();

	ModelMap		_models;
	unsigned int	_curTime;
	unsigned int	_lastCleanTime;
};
extern GrModelMgr* gGrModelMgr;
