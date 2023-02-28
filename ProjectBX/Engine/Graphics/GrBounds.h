//----------------------------------------------------------
// File:		GrBounds.h
// Author:		Kevin Bray
// Created:		12-10-06
//
// Purpose:		To provide a generic bounds type.  Currently, this
//				can only be used to represent a frustum or a sphere.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MSphere;
class GrFrustum;

//**********************************************************
// class GrBounds
//**********************************************************
class GrBounds
{
public:
	GrBounds( const GrBounds& bounds );
	GrBounds( const MSphere& sphere );
	GrBounds( const GrFrustum& frustum );
	~GrBounds();

	// performs an intersection test between two bounds.
	bool			Intersects( const GrBounds& bounds );
	bool			Intersects( const MSphere& bounds );
	bool			Intersects( const GrFrustum& bounds );

	// assignment.
	GrBounds&		operator = ( const GrBounds& bounds );
	GrBounds&		operator = ( const MSphere& sphere );
	GrBounds&		operator = ( const GrFrustum& frustum );

private:
	void			Destroy();

	// bounds.
	MSphere*		_sphere;
	GrFrustum*		_frustum;

	// memory passed to the placement new operator.  This allows the
	// bounds type to change without having to reallocate memory.
	void*			_mem;
};
