//----------------------------------------------------------
// File:        GrMeshCombiner.h
// Author:      Kevin Bray
// Created:     12-13-06
//
// Purpose:     To combine meshes into a single mesh for batching.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrMesh.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrMeshInst;
class GrMaterial;
class GrUberTexture;

//**********************************************************
// class GrMeshCombiner
//**********************************************************
class GrMeshCombiner
{
public:
    GrMeshCombiner( unsigned int maxIndexCount = 65536, unsigned int maxVertexCount = 32767 );
    ~GrMeshCombiner();

    // returns true if the mesh can be added.
    bool                CanAddMesh( const GrMeshInst* meshInst );

    // adds a mesh instance... returns the first range that the mesh being 
    // added will reside.  Basically, the ranges in the instance passed in
    // are all preserved; they're simply added to a single mesh.  Note that
    // the transform in the mesh instance is ignored (and lost).
    unsigned int        AddMesh( const GrMeshInst* meshInst );

    // returns the number of ranges in all of the meshes that have been
    // added to the meshes so far.
    unsigned int        GetRangeCount() const       {   return ( unsigned int )_ranges.size();      }

    // returns the number of vertices in all of the meshes that have been
    // added to the combiner.
    unsigned int        GetVertexCount() const      {   return _vertexCount;                        }

    // builds the final mesh.
    GrMeshInst*         BuildFinalMesh( const tstring& name );

private:
    typedef std::vector< const GrMeshInst* > MeshInstVec;
    typedef std::vector< GrMesh::SRange > RangeVec;
    typedef std::vector< URef< GrMaterial > > MaterialVec;

    RangeVec            _ranges;
    MaterialVec         _rangeMaterials;
    MeshInstVec         _meshInsts;
    GrUberTexture*      _uberTexture;
    unsigned int        _vertexCount;
    unsigned int        _indexCount;
    unsigned int        _maxVertexCount;
    unsigned int        _maxIndexCount;
};
