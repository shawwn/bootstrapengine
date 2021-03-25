//----------------------------------------------------------
// File:		GrKFAnimMgr.h
// Author:		Kevin Bray
// Created:		04-30-06
//
// Purpose:		To manage keyframe animations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// std c++ includes.
#include <map>

// forward declarations.
class GrKFAnim;

//**********************************************************
// class GrKFAnimMgr
//**********************************************************
class GrKFAnimMgr
{
public:
	GrKFAnimMgr();
	~GrKFAnimMgr();

	GrKFAnim*			GetKFAnim( const UPath& name );
	void				SaveKFAnim( GrKFAnim* anim );

private:
	typedef std::map< UPath, GrKFAnim* > AnimMap;

	AnimMap				_animMap;
};
extern GrKFAnimMgr* gGrKFAnimMgr;
