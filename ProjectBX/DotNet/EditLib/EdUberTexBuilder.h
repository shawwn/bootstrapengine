//----------------------------------------------------------
// File:		EdUberTexBuilder.h
// Author:		Kevin Bray
// Created:		12-20-07
//
// Purpose:		To build a thumbnail from a texture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "EdUberTexRasterizer.h"

// engine includes.
#include "GrTypes.h"
#include "UPath.h"

// forward declarations.
class GrMaterial;
class GrModel;
class GrMeshInst;
class GrTexture;
class GrImage;
class GrColor;
enum ESTAGE;

//**********************************************************
// class EdUberTexBuilder
//**********************************************************
class EdUberTexBuilder
{
public:
	EdUberTexBuilder();
	~EdUberTexBuilder();

	// converts a texture into an ubertexture.
	void					CreateUberTexture( const UPath& fileName, GrTexture* fillBump, GrTexture* fillDiffuse,
											   GrTexture* fillSpecular );

private:
	// contains UV information for a mesh.
	struct SMeshUVInfo
	{
		SVec2 min;
		SVec2 max;
	};
};
