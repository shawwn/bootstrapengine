//----------------------------------------------------------
// File:		GrBounds.cpp
// Author:		Kevin Bray
// Created:		12-10-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrBounds.h"

// project includes.
#include "MSphere.h"
#include "GrFrustum.h"

const size_t kMaxMemSize = Max( sizeof( MSphere ), sizeof( GrFrustum ) );

//**********************************************************
// class GrBounds
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrBounds::GrBounds( const GrBounds& bounds )
: _mem( new char[ kMaxMemSize ] )
, _sphere( 0 )
, _frustum( 0 )
{
	if ( bounds._sphere )
		_sphere = new( _mem ) MSphere( *bounds._sphere );
	else
		_frustum = new( _mem ) GrFrustum( *bounds._frustum );
}

//----------------------------------------------------------
GrBounds::GrBounds( const MSphere& sphere )
: _mem( new char[ kMaxMemSize ] )
, _sphere( 0 )
, _frustum( 0 )
{
	// copy the sphere passed in.
	_sphere = new( _mem ) MSphere( sphere );
}

//----------------------------------------------------------
GrBounds::GrBounds( const GrFrustum& frustum )
: _mem( new char[ kMaxMemSize ] )
, _sphere( 0 )
, _frustum( 0 )
{
	// copy the frustum passed in.
	_frustum = new( _mem ) GrFrustum( frustum );
}

//----------------------------------------------------------
GrBounds::~GrBounds()
{
	// call the appropriate destructor.
	Destroy();

	// free our memory.
	delete[] ( char * )_mem;
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
bool
GrBounds::Intersects( const GrBounds& bounds )
{
	if ( _sphere )
	{
		// sphere-sphere.
		if ( bounds._sphere )
			return _sphere->Intersect( *bounds._sphere );

		// sphere-frustum
		return bounds._frustum->IsSphereInside( *_sphere );
	}

	// frustum-sphere...
	if ( bounds._sphere )
		return _frustum->IsSphereInside( *bounds._sphere );

	// frustum-frustum
	return _frustum->IsFrustumInside( *bounds._frustum );
}

//----------------------------------------------------------
bool
GrBounds::Intersects( const MSphere& bounds )
{
	// sphere-sphere.
	if ( _sphere )
		return _sphere->Intersect( bounds );

	// frustum-sphere.
	return _frustum->IsSphereInside( bounds );
}

//----------------------------------------------------------
bool
GrBounds::Intersects( const GrFrustum& bounds )
{
	// sphere-frustum.
	if ( _sphere )
		return bounds.IsSphereInside( *_sphere );

	// frustum-frustum.
	return _frustum->IsFrustumInside( bounds );
}

//----------------------------------------------------------
GrBounds&
GrBounds::operator = ( const GrBounds& bounds )
{
	// call the approprate destructor.
	Destroy();

	// assign.
	if ( bounds._sphere )
		_sphere = new( _mem ) MSphere( *bounds._sphere );
	else
		_frustum = new( _mem ) GrFrustum( *bounds._frustum );
	return *this;
}

//----------------------------------------------------------
GrBounds&
GrBounds::operator = ( const MSphere& sphere )
{
	// call the approprate destructor.
	Destroy();
	_sphere = new( _mem ) MSphere( sphere );
	return *this;
}

//----------------------------------------------------------
GrBounds&
GrBounds::operator = ( const GrFrustum& frustum )
{
	// call the approprate destructor.
	Destroy();
	_frustum = new( _mem ) GrFrustum( frustum );
	return *this;
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
void
GrBounds::Destroy()
{
	// call the appropriate destructor.
	if ( _sphere )
		_sphere->~MSphere();
	else
		_frustum->~GrFrustum();
}
