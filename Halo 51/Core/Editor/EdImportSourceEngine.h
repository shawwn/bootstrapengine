//----------------------------------------------------------
// File:		EdImportMD3.h
// Author:		Shawn Presser
// Created:		12-21-08
//
// Purpose:		To import Source Engine art assets.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UPath.h"

//**********************************************************
// class EdImportSourceEngine
//**********************************************************
class EdImportSourceEngine
{
public:
	EdImportSourceEngine();

	bool			ImportModel( const char* name, const char* targetPackageName, const char* path, float scale );
};
