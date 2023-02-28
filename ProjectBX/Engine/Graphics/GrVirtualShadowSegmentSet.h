//----------------------------------------------------------
// File:		GrVirtualShadowSegmentSet.h
// Author:		Kevin Bray
// Created:		05-05-07
//
// Purpose:		To manage a shadow region of a viewer camera.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTypes.h"
#include "GrFrustum.h"

// forward declarations.
class GrCamera;

//**********************************************************
// class GrVirtualShadowSegmentSet
//**********************************************************
class GrVirtualShadowSegmentSet
{
public:
	struct SAABox2D
	{
		SVec2 lowerLeft;
		SVec2 upperRight;
		int clipLogX;
		int clipLogY;
		float z;
	};

	GrVirtualShadowSegmentSet( const GrCamera& viewerCamera, unsigned int segmentCount, float firstSize );
	~GrVirtualShadowSegmentSet();

	// calculates AA boxes for all segments given the current camera.
	void				UpdateSegments( const GrCamera& faceCamera, float border, float distBias );

	// returns the number of segments.
	unsigned int		GetSegmentCount() const						{	return _segmentCount;	}

	// returns the segment information calculated the last time that
	// UpdateSegments was called.
	const SAABox2D&		GetSegmentAABox( unsigned int segment ) const;
	bool				IsSegmentVisible( unsigned int segment ) const;

private:
	struct SSegment
	{
		MVec3 corners[ 8 ];
		SAABox2D aaBox;
		float start;	// start along the eye vector.
		float end;		// end along the eye vector.
		bool visible;
	};

	// private members.
	bool				IsSegmentVisible( const SSegment& segment, const GrFrustum& frustum );

	// all managed segments.
	GrFrustum			_frustum;
	SSegment*			_segments;
	unsigned int		_segmentCount;
};
