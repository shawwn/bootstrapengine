//----------------------------------------------------------
// File:        GrPolygonUberUVMapper.h
// Author:      Kevin Bray
// Created:     12-11-08
//
// Purpose:     To atlas UV patch data into ubertextures.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// STL includes.
#include <vector>

// forward declarations.
class GrPolygonUberPatchGroup;
class GrPolygonUberPatch;
class GrPolygonUberMapping;
class GrUberTexture;
class MVec3;
class UPath;

//**********************************************************
// class GrPolygonUberUVMapper
//**********************************************************
class GrPolygonUberUVMapper
{
public:
    GrPolygonUberUVMapper();
    ~GrPolygonUberUVMapper();

    enum EPLANE
    {
        EP_XY,
        EP_XZ,
        EP_YZ,
    };

    // this maps groups from a very large planar mapping into an
    // ubertexture.  Note that for this to work, everything must be
    // mapped onto a single plane.
    void                    PlaceCoplanarGroups( std::vector< GrPolygonUberPatchGroup* > groups, EPLANE mappingPlane );

    // This maps arbitrary patches into an ubertexture, trying to
    // preserve spatial locality within the map based on spatial
    // locality in world space.
    void                    PlaceGroupsSlow( std::vector< GrPolygonUberPatchGroup* > groups, EPLANE movementPlane,
                                             float gridEntrySize );

    // This maps arbitrary patches into an ubertexture using a path
    // as a guide for packing ubertexture data.  Note that the lines
    // in the path must be sorted as desired.  The step size is how
    // far the eye position moves between packing checks.
    void                    PlaceGroupsByPath( std::vector< GrPolygonUberPatchGroup* > groups, const MVec3* path,
                                               int numLines, float stepSize );

    // returns the number of mappings.  Note that there is one
    // mapping per ubertexture.
    unsigned int            GetTexturesRequired() const;

    // this generates ubertextures out of all of the mapped geometry.
    void                    GenUberTextures( const UPath& basePath );

    // ubertexture enumeration functions.
    unsigned int            GetUberTextureCount() const;
    URef< GrUberTexture >   GetUberTexture( unsigned int idx ) const;

private:
    typedef std::vector< GrPolygonUberMapping* > MappingVec;
    typedef std::vector< URef< GrUberTexture > > UberTextureVec;

    MappingVec              _mappings;
    UberTextureVec          _uberTextures;
};
