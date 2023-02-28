//----------------------------------------------------------
// File:		GrMaterialMgr.h
// Author:		Kevin Bray
// Created:		03-28-05
//
// Purpose:		To contain parameters for a program and is intended
//				to contain a pass for a material.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"
#include "URef.h"
#include "GrConfig.h"
#include "GrMaterial.h"

// std c++ includes.
#include <vector>
#include <map>

//**********************************************************
// class GrMaterialMgr
//**********************************************************
class GrMaterialMgr
{
public:
	GrMaterialMgr();
	~GrMaterialMgr();

	// returns the quality.
	EGRQUALITY			GetShadingQuality() const					{	return _quality;		}
	void				SetShadingQuality( EGRQUALITY quality )		{	_quality = quality;		}

	// registers a material.  This is usually called when a material is specified in a script.
	URef< GrMaterial >	SetMaterial( const tstring& name, const GrMaterial::SInfo& info );
	URef< GrMaterial >	GetMaterial( const UPath& name, tstring& error );

	// returns the default material.  This should be used for errors only.  Do expect this
	// material to not change over time.
	URef< GrMaterial >	GetDefaultMaterial();

	// reloads the script a given material resides in.
	bool				ReloadMaterial( const tstring& name, tstring& error );
	void				ReloadAll();
	void				UnloadAll();

	// updates all materials.
	void				Update();

	unsigned int		GetMaterialCount() const;
	URef< GrMaterial >	GetMaterialByIdx( unsigned int index );

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

	// returns the name of the script a material should reside in.
	void				GetScriptName( tstring& scriptName, const tstring& materialName );

private:
	typedef std::vector< GrMaterial* >					MaterialVec;
	typedef std::map< UPath, URef< GrMaterial > >		MaterialMap;
//	typedef std::multimap< UPath, URef< GrMaterial > >	MaterialMap;

	MaterialMap			_materialMap;
	MaterialVec			_materials;
	URef< GrMaterial >	_defaultMaterial;
	EGRQUALITY			_quality;
};
extern GrMaterialMgr* gGrMaterialMgr;
