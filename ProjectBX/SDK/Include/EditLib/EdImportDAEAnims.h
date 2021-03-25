//----------------------------------------------------------
// File:		EImportDAEAnims.h
// Author:		Kevin Bray
// Created:		01-18-07
//
// Purpose:		To import collada animations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrKFAnim.h"
#include "UPath.h"

// forward declarations.
class FCDocument;


//**********************************************************
// class EdImportDAEAnims
//**********************************************************
class EdImportDAEAnims
{
public:
	EdImportDAEAnims();
	~EdImportDAEAnims();

	GrKFAnim*		ImportAnim( FCDocument* doc, const UPath& outputPath );

private:

};
