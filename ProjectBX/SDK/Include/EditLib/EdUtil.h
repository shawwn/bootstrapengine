//----------------------------------------------------------
// File:		EdUtil.h
// Author:		Shawn Presser
// Created:		12-23-08
//
// Purpose:		To expose useful editlib-related utility functions
//				referenced from multiple places.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// forward declarations.
struct SVec3;

//**********************************************************
// EdUtils
//**********************************************************

extern UPath			EdFixupTexturePath( const UPath& outputPath, const UPath& filePath );

// returns whether the three points are colinear.
extern bool				EdIsTriDegenerate( const SVec3& p0, const SVec3& p1, const SVec3& p2, float epsilon );
