//----------------------------------------------------------
// File:		Render.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Render.h"

// engine headers.
#include "GLSubsys.h"
#include "GrUtil.h"

// project headers.
#include "Camera.h"
#include "Color.h"

//----------------------------------------------------------
void 
Bootstrap::Render::Points( array< Vector3 >^ points, Color color, int sizeInPixels, unsigned int offset, unsigned int count )
{
	// must call GrStateStart() first!
	B_ASSERT( GrStateStarted() );

	// get the viewport's dimensions.
	float w, h;
	{{
		int viewX, viewY, width, height;
		GrGetViewport( viewX, viewY, width, height );
		w = 0.5f*width;
		h = 0.5f*height;
	}}
	float invW = 1.0f/w;
	float invH = 1.0f/h;

	// setup.
	GrBindVB( 0 );
	GrBindIB( 0 );
	bglMatrixMode( GL_MODELVIEW );
	bglLoadIdentity();
	bglMatrixMode( GL_PROJECTION );
	bglLoadIdentity();
	bglBegin( GL_TRIANGLES );

	for( unsigned int i = offset; i < offset+count; ++i )
	{
		Vector3 pt = points[i];

		// calculate the upper-left corner of the quad (which is 
		// centered around the pixel closest to [pos]).
		float minX = invW*(Floor( w*pt.X ) - 0.5f*(sizeInPixels-1) + 0.5f);
		float minY = invH*(Floor( h*pt.Y ) - 0.5f*(sizeInPixels-1) + 0.5f);

		// calculate the lower-right corner.
		float maxX = minX + invW*sizeInPixels;
		float maxY = minY + invH*sizeInPixels;

		// draw.
		bglColor4f( color.R, color.G, color.B, color.A );

		bglVertex3f( minX, maxY, 0.0f );
		bglVertex3f( maxX, maxY, 0.0f );
		bglVertex3f( minX, minY, 0.0f );

		bglVertex3f( minX, minY, 0.0f );
		bglVertex3f( maxX, maxY, 0.0f );
		bglVertex3f( maxX, minY, 0.0f );
	}

	// teardown.
	bglEnd();
}