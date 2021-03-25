//----------------------------------------------------------
// File:		GrPolygonMesh.cpp
// Author:		Kevin Bray
// Created:		01-16-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonMesh.h"


//**********************************************************
// class GrPolygonMesh
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonMesh::GrPolygonMesh( const tstring& name )
: _name( name )
, _uberData( 0 )
{

}

//----------------------------------------------------------
GrPolygonMesh::GrPolygonMesh( const tstring& name, const GrPolygonGroup& polygons )
: _name( name )
, _polygons( polygons )
, _uberData( 0 )
{

}

//----------------------------------------------------------
GrPolygonMesh::~GrPolygonMesh()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonMesh::GenMeshes( std::vector< GrMeshInst* >& meshes )
{
	_polygons.GenMeshes( meshes, _name, false );
}
