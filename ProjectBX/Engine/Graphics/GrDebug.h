//----------------------------------------------------------
// File:		GrDebug.h
// Author:		Kevin Bray
// Created:		08-20-05
//
// Purpose:		To provide simple debugging utilities.  This file
//				contains functionality that is not dependent on any
//				graphics functionality.  That is, this stuff can be
//				used without including any files from the graphics
//				project.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
struct SVec3;

//**********************************************************
// functions prototypes.
//**********************************************************

//==========================================================
// Image saving functions.

// saves a TGA image.  If the file already exists, it will be replaced.
extern void GrSaveTGA( const char* filePath, const void* data, unsigned int width,
					   unsigned int height, unsigned int bytesPerPel );
extern void GrSaveOBJ( const char* filePath, const char* modelName, const SVec3* positions,
					   unsigned int positionCount, const unsigned short* indices,
					   unsigned int indexCount );
