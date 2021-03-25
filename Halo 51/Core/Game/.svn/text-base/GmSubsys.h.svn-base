//----------------------------------------------------------
// File:		GmSubsys.h
// Author:		Shawn Presser
// Created:		12-20-08
//
// Purpose:		To manage the game's singletons and the physics
//				singletons.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UPath.h"
#include "Graphics/GrCamera.h"

// project headers.
#include "PhSubsys.h"

//**********************************************************
// class GmSubsys
//**********************************************************
class GmSubsys
{
public:
	GmSubsys();
	~GmSubsys();

	bool				LoadScene( const UPath& scenePath );

	void				Update( unsigned int dt );

private:
	void				Clear();

	NxActorContainer	_staticActors;
};
extern GmSubsys*		gGmSubsys;
