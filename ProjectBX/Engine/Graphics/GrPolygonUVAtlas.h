//----------------------------------------------------------
// File:		GrPolygonUVAtlas.h
// Author:		Kevin Bray
// Created:		11-19-06
//
// Purpose:		To place a rectangle into a larger image.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URect.h"

//**********************************************************
// class GrPolygonUVAtlas
//**********************************************************
class GrPolygonUVAtlas
{
public:
	GrPolygonUVAtlas( const URect& rect );
	~GrPolygonUVAtlas();

	void					Clear();
	bool					Insert( URect& rect, unsigned int width, unsigned height );

private:
	URect					_rect;
	GrPolygonUVAtlas*		_child1;
	GrPolygonUVAtlas*		_child2;
	URect					_leafRect;
};
