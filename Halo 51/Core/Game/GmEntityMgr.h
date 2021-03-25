//----------------------------------------------------------
// File:		GmEntityMgr.h
// Author:		Shawn Presser
// Created:		12-15-08
//
// Purpose:		To track and update game entities.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UPath.h"

// project headers.
#include "GmEntity.h"
#include "GmObject.h"
#include "GmCharacter.h"

// std c++ headers.
#include <list>

// forward declarations.
class GmAI;

//**********************************************************
// class GmEntityMgr
//**********************************************************
class GmEntityMgr
{
public:
	GmEntityMgr();
	~GmEntityMgr();

	// creates a game object.
	GmObject*			CreateDynamicObject( const tstring& name, const GmObject::SInfo& info );

	// creates a game character.
	GmCharacter*		CreateCharacter( const tstring& name, const GmCharacter::SInfo& info );

	// creates an AI-powered game character.
	GmAI*				CreateAICharacter( const tstring& name, const GmCharacter::SInfo& info );

	// removes all game entities.
	void				ClearEntities();

	// updates all game entities.
	void				Update( unsigned int dt );

	// debug rendering.
	void				DebugRender();

private:
	URef< GrModel >		GetModel( const UPath& path, const UPath& instanceName, const MMat4x4& transform );

	typedef std::list< GmEntity* >		EntityContainer;
	typedef std::list< GmAI* >			AIContainer;

	EntityContainer		_objects;
	EntityContainer		_characters;
	AIContainer			_brains;
};
extern GmEntityMgr*		gGmEntityMgr;
