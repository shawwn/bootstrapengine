//----------------------------------------------------------
// File:        GrDeformWave.h
// Author:      Kevin Bray
// Created:     05-12-06
//
// Purpose:     To displace a mesh by a wave function.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class
#include "GrDeformer.h"

//**********************************************************
// class GrDeformWave
//**********************************************************
class GrDeformWave : public GrDeformer
{
public:
    static const unsigned int kWaveSourceCount = 1;

    GrDeformWave();
    ~GrDeformWave();

    // cloning support.
    GrDeformer*         Clone() const;

    // parameters.
    static const char*  StaticGetName()         {   return "wave";              }
    const char*         GetName() const         {   return StaticGetName();     }

    // deform the mesh.
    void                Deform( GrMesh* dst, GrMesh* src, unsigned int srcRangeIdx, unsigned int time );
};
