//----------------------------------------------------------
// File:		MLineSegment.h
// Author:		Kevin Bray
// Created:		06-28-06
//
// Purpose:		To represent a line segment in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

//**********************************************************
// class MLineSegment
//**********************************************************
class MLineSegment
{
public:
	MLineSegment();
	MLineSegment( const MVec3& pt0, const MVec3& pt1 );
	~MLineSegment();

	// reset.
	void			Reset( const MVec3& pt0, const MVec3& pt1 );

	// finds the distance to a point.
	float			Dist( const MVec3& pt );

	float			GetLength()	const	{	return _pt0.Dist( _pt1 );	}

	const MVec3&	GetPt0() const		{	return _pt0;				}
	const MVec3&	GetPt1() const		{	return _pt1;				}

private:
	MVec3			_pt0;
	MVec3			_pt1;
};
