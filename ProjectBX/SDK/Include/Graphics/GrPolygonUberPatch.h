//----------------------------------------------------------
// File:        GrPolygonUberPatch.h
// Author:      Kevin Bray
// Created:     12-05-08
//
// Purpose:     To represent a patch of uberized polygons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTypes.h"
#include "MPlane.h"
#include "MAABox.h"

// forward declarations.
class GrPolygonGroup;
struct SVec2;

//**********************************************************
// class GrPolygonUberPatch
//**********************************************************
class GrPolygonUberPatch
{
public:
    // constructs a patch out of the polygons specified.
    GrPolygonUberPatch( GrPolygonGroup* polygons, unsigned int* polyIndices, unsigned int polyCount,
                        const MPlane& worldPlane, const MMat4x4& uvTileMatrix, const SVec2& uvPos,
                        const SVec2& uvSize );
    ~GrPolygonUberPatch();

    // bounding box.
    const MAABox&       GetAABox() const            {   return _aaBox;          }

    // gets the size of the UV data in texture space.
    const SVec2&        GetUVSizeInTiles() const    {   return _uvSize;         }

    // returns the matrix that goes from world space to texture
    // space.
    const MMat4x4&      GetLocalToUVMatrix() const  {   return _uvTileMatrix;   }

    // returns the surface plane for the set of polygons represented
    // by the patch.
    const MPlane&       GetSurfacePlane() const     {   return _surfacePlane;   }

    // adjusts the patch's UVs so they're relative to the correct
    // tile.
    void                AssignToUberTexture( unsigned int uberTexIndex, unsigned int tileX, unsigned int tileY );

    // provides access to polygons in the current patch.
    GrPolygonGroup*     GetPolygonGroup() const;
    unsigned int        GetPolygonCount() const;
    unsigned int        GetPolygonIndex( unsigned int i ) const;

    // returns the ubertexture info.
    unsigned int        GetUberTextureIndex() const {   return _uberTexIndex;   }
    unsigned int        GetUberTileX() const        {   return _tileX;          }
    unsigned int        GetUberTileY() const        {   return _tileY;          }
    unsigned int        GetUberTileWidth() const    {   return _tileWidth;      }
    unsigned int        GetUberTileHeight() const   {   return _tileHeight;     }

private:
    void                TranslateUVs( const SVec2& uvPos );
    void                AdjustPolygonUVs( const SVec2& uvPos );
    void                CalcSurfaceInfo();

    // polygon group we represent, along with the indices of the
    // polygons actually in the group.
    GrPolygonGroup*     _polygons;
    unsigned int*       _polyIndices;
    unsigned int        _polyCount;

    // information about the best surface.  We use this as the world
    // space distance in our texture LOD attenuation function.  This
    // controls how fast texture-tile LOD's fall off.
    MPlane              _surfacePlane;

    // bounding volume of the patch in world space.
    MAABox              _aaBox;

    // matrix to go from space to UV space.
    MMat4x4             _uvTileMatrix;

    // size of the UVs in texture space.
    SVec2               _uvSize;

    // an arbitrary number that can be set to indicate what
    // ubertexture the patch resides in.
    unsigned int        _uberTexIndex;

    // tile width and height.
    unsigned int        _tileX;
    unsigned int        _tileY;
    unsigned int        _tileWidth;
    unsigned int        _tileHeight;
};
