//----------------------------------------------------------
// File:        GrPolygonUberPatchGroup.h
// Author:      Kevin Bray
// Created:     12-05-08
//
// Purpose:     To represent uber-texture data for a polygon group.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"

// forward declarations.
class GrPolygon;
class GrPolygonGroup;
class GrPolygonUberPatch;
class MMat4x4;
struct SPolygonVertex;
struct SVec2;

//**********************************************************
// class GrPolygonUberPatchGroup
//**********************************************************
class GrPolygonUberPatchGroup
{
public:
    // builds calculates UV's for the group specified and builds
    // uber-texture patches.
    GrPolygonUberPatchGroup( GrPolygonGroup* group, float texelsPerMeter );
    ~GrPolygonUberPatchGroup();

    // returns the number of texels for each meter in the world.
    // This is based on the plane which a polygon is projected onto
    // and UV mapped.
    float                   GetTexelsPerMeter() const           {   return _texelsPerMeter;             }

    // returns the total number of patches in the current group.
    unsigned int            GetPatchCount() const               {   return _patches.GetElemCount();     }
    GrPolygonUberPatch*     GetPatch( unsigned int idx ) const;

private:
    enum EAXIS
    {
        EA_NX = 0,
        EA_PX,
        EA_NY,
        EA_PY,
        EA_NZ,
        EA_PZ,
    };

    typedef UFastArray< GrPolygonUberPatch* > UberPatchArray;

    // builds ubertexture patches from the polygon group specified.
    void                    BuildPatches( GrPolygonGroup* group, float texelsPerMeter );
    void                    BuildUberPatch( GrPolygonGroup* group, unsigned int* patchPolygons,
                                            unsigned int patchPolyCount, const SVec2& uvPos,
                                            const SVec2& uvSize, EAXIS axis, float texelsPerMeter );
    EAXIS                   CalcDomAxis( const GrPolygon& polygon );
    void                    CalcUVs( GrPolygonGroup* group, unsigned int polyIdx, EAXIS axis,
                                     float uvScale ) const;

    // patches.
    UberPatchArray          _patches;

    // number of texels per meter.
    float                   _texelsPerMeter;
    unsigned int            _uberTextureCount;
};
