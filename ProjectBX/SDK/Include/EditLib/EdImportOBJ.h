//----------------------------------------------------------
// File:		EdImportOBJ.h
// Author:		Kevin Bray
// Created:		08-15-08
//
// Purpose:		To import an OBJ file as a collection of polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class EdSimplePolygonGroup;
struct SVec3;

//**********************************************************
// class EdImportOBJ
//**********************************************************
class EdImportOBJ
{
public:
	EdImportOBJ();
	~EdImportOBJ();

	// loads an OBJ file from the disk.
	void			LoadOBJ( EdSimplePolygonGroup& group, const char* fileName );

private:
	void			ReadVec3( SVec3& result, char* start, char** strTokContext );
	void			ReadHint( SVec3& hint, char* buffer );
};
