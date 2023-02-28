//----------------------------------------------------------
// File:        GrMesh.h
// Author:      Kevin Bray
// Created:     10-04-04
//
// Purpose:     Contains mesh data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTypes.h"
#include "MPlane.h"
#include "MSphere.h"
#include "MAABox.h"
#include "URef.h"
#include "UFastArray.h"

// std c++ includes.
#include <string>
#include <list>
#include <vector>

// forward declarations.
class UReader;
class UWriter;
class MMat4x4;
class MMat3x3;
class GrPolygonGroup;
class GrMeshVB;
class GrMeshIB;
class GrSkin;
class MRay;
class GrMeshUVData;
class GrUberTexture;

//**********************************************************
// class GrMesh
//**********************************************************
class GrMesh : public URefCount
{
public:
    struct SRange
    {
        SRange() : name( "" ), start( 0 ), count( 0 ), minIndex( 0 ), maxIndex( 0 ), aaBox( MVec3(), MVec3() ) { }

        tstring                 name;
        unsigned int            start;      // starting vertex or index.
        unsigned int            count;      // number of vertices or indices to process.
        unsigned int            minIndex;   // only used for indexed meshes.
        unsigned int            maxIndex;
        MSphere                 bounds;
        MAABox                  aaBox;
    };

    // triangle collision data.
    struct STriCollision
    {
        STriCollision() : triIdx( 0 ), u( 0.0f ), v( 0.0f ), w( 0.0f ) { }

        unsigned int triIdx;
        MVec3 hitLoc;
        MVec3 hitNormal;
        float u;
        float v;
        float w;
    };
    typedef UFastArray< STriCollision > TriHitArray;

    // triangle info.
    struct STriInfo
    {
        unsigned int vertIdx0;
        unsigned int vertIdx1;
        unsigned int vertIdx2;
        unsigned int range;
    };

    GrMesh( UReader& reader );
    GrMesh( UReader& reader, const UFastArray< URef< GrSkin > >& skins );

    // pass in arrays to each component.
    // non-indexed mesh.
    GrMesh( const tstring& name,
            const SVec3* verts,
            const SVec3* normals,
            const SVec3* binormals,
            const SVec3* tangents,
            const unsigned long* colors,
            const SVec2* uvs,
            unsigned long vertexCount,
            const SRange* ranges,
            unsigned int rangeCount,
            URef< GrUberTexture > uberTexture,
            unsigned int vbFlags = 0 );

    // indexed or non-indexed mesh.
    GrMesh( const tstring& name,
            const SVec3* verts,
            const SVec3* normals,
            const SVec3* binormals,
            const SVec3* tangents,
            const unsigned long* colors,
            const SVec2* uvs,
            unsigned long vertexCount,
            unsigned short* indices,
            unsigned int indexCount,
            const SRange* ranges,
            unsigned int rangeCount,
            URef< GrUberTexture > uberTexture,
            unsigned int vbFlags = 0,
            unsigned int ibFlags = 0, 
            unsigned int maxVertexCount = 0, 
            unsigned int maxIndexCount = 0 );

    // indexed or non-indexed mesh, uninitialized vertex data.
    GrMesh( const tstring& name,
            unsigned int vertexComponents,
            unsigned int vertexCount,
            unsigned int indexCount,
            const SRange* ranges,
            unsigned int rangeCount,
            URef< GrUberTexture > uberTexture,
            unsigned int vbFlags = 0,
            unsigned int ibFlags = 0 );

    // ctor.
    virtual ~GrMesh();

    const tstring&          GetName() const                 {   return _name;               }

    // creates a "deep" clone of a mesh.
    GrMesh*                 Clone();
    GrMesh*                 CloneRange( unsigned int range );

    // caches the data for rendering.
    void                    Cache();
    void                    Evict();

    // returns true if the mesh is indexed or not.
    bool                    IsIndexed() const               {   return _indexBuffer != 0;   }

    // picks the closest triangle from the mesh and returns the ID of triangle,
    // the model space position where the hit occured, as well as the bary-centric
    // coordinates of the triangle intersection.  Returns ~0 if no intersection
    // was found.
    void                    Pick( TriHitArray& triHits, const MRay& ray ) const;
    void                    PickRange( TriHitArray& triHits, unsigned int range, const MRay& ray ) const;

    // triangle info.
    unsigned int            GetTriCount() const;
    void                    GetTriInfo( STriInfo& triInfo, unsigned int triIdx ) const;

    // vertex info.
    URef< GrMeshVB >        GetVertexData() const;
    URef< GrMeshIB >        GetIndexData() const;

    // ubertexture.
    URef< GrUberTexture >   GetUberTexture() const;

    // call to notify the mesh that it's vertex/index data has changed.
    void                    VertexDataChanged();

    // returns the number of vertices in the mesh.
    unsigned int            GetVertexCount() const;
    unsigned int            GetIndexCount() const;

    // skin info.  Note that the mesh takes ownership over the skinning data
    // passed in.  Also note that once the skin is set, it cannot be changed.
    void                    SetSkin( URef< GrSkin > skin );
    GrSkin*                 GetSkin() const                 {   return _skin;               }

    // range info.
    unsigned int            GetRangeCount() const           {   return _rangeCount;         }
    const SRange&           GetRange( unsigned int range )  {   return _ranges[ range ];    }
    const MSphere&          GetRangeBoundingSphere( unsigned int range ) const;
    const MAABox&           GetRangeAABox( unsigned int range ) const;
    void                    GetRangePlane( MVec3& normal, MVec3& point, unsigned int range ) const;
    unsigned int            GetRangeVertexStart( unsigned int range ) const;
    unsigned int            GetRangeVertexCount( unsigned int range ) const;

    // general info.
    const MAABox&           GetAABox() const                {   return _aaBox;              }
    const MSphere&          GetBoundingSphere() const       {   return _boundingSphere;     }

    // optimizes mesh data for rendering.
    void                    Optimize();

    // process all vertices in a range.
    void                    BeginRender( unsigned int vertexComponents );   // must be called before rendering.
    void                    Render( unsigned int range, bool buildOccQuery );

    // returns the number of fragments/sample taps rendered the last time occlusion query was
    // enabled.
    unsigned int            GetVisibleFragmentCount( unsigned int range ) const;

    // debugging functions.
    void                    DrawNormals();
    void                    DrawTangents();
    void                    DrawBinormals();

    // saving and loading.
    void                    Save( UWriter& writer ) const;
    void                    Load( UReader& reader, const UFastArray< URef< GrSkin > >& skins );

    // for editor ...
    SRange&                 GetModifiableRange( unsigned int range )    {   return _ranges[ range ];    }

private:
    // used for cloning.
    GrMesh( const tstring& name,
            SVec3* verts,
            SVec3* normals,
            SVec3* binormals,
            SVec3* tangents,
            unsigned long* colors,
            SVec2* uvs,
            unsigned long vertexCount,
            unsigned short* indices,
            unsigned int indexCount,
            SRange* ranges,
            unsigned int rangeCount,
            URef< GrUberTexture > uberTexture,
            const MAABox& aaBox,
            const MSphere& boundingSphere,
            bool vbDynamic = false,
            bool ibDynamic = false );

    void                    SetSkin( URef< GrSkin > skin, unsigned int startIdx, unsigned int skinVertexCount );
    void                    CalcAABox();
    void                    CalcBoundingSphere();
    void                    CleanUp();

    // name of the mesh.
    tstring                 _name;

    // range info.
    unsigned int            _rangeCount;
    SRange*                 _ranges;

    // BV info.
    MAABox                  _aaBox;
    MSphere                 _boundingSphere;

    // vertex buffer offsets.
    URef< GrMeshVB >        _vertexBuffer;
    URef< GrMeshIB >        _indexBuffer;

    // geometry ubertexture.
    URef< GrUberTexture >   _uberTexture;

    // skin.
    URef< GrSkin >          _skin;
    unsigned int            _startSkinIdx;      // which part of the skin is relevant.
    unsigned int            _skinVertexCount;
};
