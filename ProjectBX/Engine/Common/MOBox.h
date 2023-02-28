//----------------------------------------------------------
// File:		MOBox.h
// Author:		Kevin Bray
// Created:		03-04-06
//
// Purpose:		To manage an oriented bounding box.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MAABox.h"
#include "MMat4x4.h"

//**********************************************************
// class MOBox
//**********************************************************
class MOBox
{
public:
	MOBox();
	MOBox( const MAABox& aaBox );
	MOBox( const MMat4x4& transform, const MAABox& aaBox );
	MOBox( const MMat4x4& transform, const MVec3& halfExts );
	~MOBox();

	void			SetTransform( const MMat4x4& transform )		{	_transform = transform;		}
	const MMat4x4&	GetTransform() const							{	return _transform;			}

	const MVec3&	GetHalfExts() const								{	return _halfExts;			}
	void			SetHalfExts( const MVec3& halfExts )			{	_halfExts = halfExts.Abs();	}

	// returns (8) world space corners.
	void			GetCorners( MVec3* corners ) const;

private:
	MMat4x4			_transform;
	MVec3			_halfExts;
};
