//----------------------------------------------------------
// File:        GrCoplanarPolygonGroup.h
// Author:      Kevin Bray
// Created:     09-13-06
//
// Purpose:     To represent a group of coplanar polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrCoplanarPolygonGroup.h"

//**********************************************************
// class GrCoplanarPolygonGroup
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrCoplanarPolygonGroup::GrCoplanarPolygonGroup( const MPlane& plane )
: _plane( plane )
{

}

//----------------------------------------------------------
GrCoplanarPolygonGroup::~GrCoplanarPolygonGroup()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
GrCoplanarPolygonGroup::AddPolygonIdx( unsigned int polygon )
{
    // add to the total surface area and then add the polygon.
    _polygons.Push( polygon );
}

//----------------------------------------------------------
unsigned int
GrCoplanarPolygonGroup::GetPolygonIdx( unsigned int idx ) const
{
    B_ASSERT( idx < _polygons.GetElemCount() );
    return _polygons[ idx ];
}
