//----------------------------------------------------------
// File:        GrSkin.h
// Author:      Kevin Bray
// Created:     06-21-06
//
// Purpose:     To manage skin data for a mesh.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"

// forward declarations.
class GrMesh;
class MMat4x4;
class MMat3x3;
class UReader;
class UWriter;
struct SVec3;

//**********************************************************
// class GrSkin
//**********************************************************
class GrSkin : public URefCount
{
public:
    GrSkin( UReader& reader );
    GrSkin( const tstring& name, URef< GrMesh > bindMesh, const SVec3* vertexWeights, const unsigned int* vertexBoneIds,
        const tstring* boneNames, const MMat4x4* invBindMatrices, unsigned int boneCount );
    ~GrSkin();

    const tstring&      GetName() const                 {   return _name;           }

    // clones the skin in it's entirety.
    GrSkin*             Clone( const tstring& name );

    // clones a range of the skin.  This should be used when a range of a mesh
    // is cloned.
    GrSkin*             CloneRange( const tstring& name, URef< GrMesh > bindMesh, unsigned int vertexStart, unsigned int vertexCount );

    // bone id/names.
    unsigned int        GetBoneCount() const            {   return _boneCount;      }
    unsigned int        GetBoneId( const char* name ) const;
    const tstring&      GetBoneName( unsigned int id ) const;

    const MMat4x4&      GetInvBindMatrix( unsigned int id ) const;

    URef< GrMesh >      GetBindMesh() const;
    const SVec3*        GetVertexWeights() const        {   return _vertexWeights;  }
    const unsigned int* GetVertexBoneIds() const        {   return _vertexBoneIds;  }

    // replaces the vertex skinning data.  There must be one entry for every vertex
    // in the mesh we're skinning.
    void                ReplaceVertexSkinData( SVec3* newWeights, unsigned int* newBoneIds );

    // remaps vertex skinning data using the remapping passed in.
    void                RemapVertexSkinData( unsigned short* remapping, unsigned int newCount, unsigned int prevCount );

    // saving and loading.
    void                Save( UWriter& writer ) const;
    void                Load( UReader& reader );

private:
    // skin name.
    tstring             _name;

    // bind pose data.
    URef< GrMesh >      _bindMesh;

    // vertex data.
    SVec3*              _vertexWeights;
    unsigned int*       _vertexBoneIds;

    // bone information.
    unsigned int        _boneCount;
    tstring*            _boneNames;
    MMat4x4*            _invBindMatrices;
};
