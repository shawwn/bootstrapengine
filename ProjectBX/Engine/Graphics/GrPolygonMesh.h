//----------------------------------------------------------
// File:        GrPolygonMesh.h
// Author:      Kevin Bray
// Created:     01-16-06
//
// Purpose:     To represent a polygonal mesh.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"
#include "GrPolygonGroup.h"

// forward declarations.
class GrPolygonGroupUberData;

//**********************************************************
// class GrPolygonMesh
//**********************************************************
class GrPolygonMesh : public URefCount
{
public:
    GrPolygonMesh( const tstring& name );
    GrPolygonMesh( const tstring& name, const GrPolygonGroup& polygons );
    ~GrPolygonMesh();

    // mesh name
    const tstring&          GetName() const             {   return _name;       }

    // polygon management.
    GrPolygonGroup&         GetPolygons()               {   return _polygons;   }
    const GrPolygonGroup&   GetPolygons() const         {   return _polygons;   }

    void                    GenMeshes( std::vector< GrMeshInst* >& meshes );

private:
    tstring                 _name;
    GrPolygonGroup          _polygons;
    GrPolygonGroupUberData* _uberData;
};
