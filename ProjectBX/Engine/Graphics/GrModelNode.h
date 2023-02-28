//----------------------------------------------------------
// File:        GrModelNode.h
// Author:      Kevin Bray
//
// Purpose:     To manage a node in a model.
//
// Created:     04-02-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "MMat4x4.h"
#include "GrMesh.h"

// std c++ includes.
#include <map>
#include <vector>

// forward declarations.
class GrModel;
class GrMeshInst;
class GrPolygonNode;
class UWriter;
class UReader;
class GrFrustum;
class GrLight;
class MSphere;
class GrRenderList;

//**********************************************************
// class GrModelNode
//**********************************************************
class GrModelNode
{
public:
    typedef UFastArray< GrMeshInst* > MeshInstArray;

    // note that if a mesh instance is passed in, the model node will
    // take ownership of it.
    GrModelNode( GrModel* owner, GrModelNode* parent, const MMat4x4& local, GrMeshInst* meshInst = 0 );
    GrModelNode( GrModel* owner, GrPolygonNode* fromTree );
    GrModelNode( GrModel* owner, UReader& reader, const UFastArray< URef< GrMesh > >& meshes,
                 unsigned short majorVer, unsigned short minorVer );
    ~GrModelNode();

    const tstring&      GetName() const                     {   return _name;       }

    GrModel*            GetOwner() const                    {   return _owner;      }

    // clones a node hierarchy.
    GrModelNode*        Clone( GrModel* newOwner, bool deep = false, GrModelNode* parent = 0 );

    // transform management.
    void                SetLocal( const MMat4x4& local );
    const MMat4x4&      GetLocal() const                    {   return _local;      }

    // child management.
    unsigned int        GetChildCount()                     {   return _children.GetElemCount();    }
    GrModelNode*        GetChild( unsigned int idx );
    GrModelNode*        FindNode( const char* name, bool ignoreCase );
    GrModelNode*        GetParent() const                   {   return _parent;                     }

    // mesh instance management.
    unsigned int        GetMeshInstCount() const            {   return _meshes.GetElemCount();      }
    GrMeshInst*         GetMeshInst( unsigned int idx ) const;

    // updates the transform.
    void                Update( const MMat4x4& parentMatrix );

    // calculates an AABB from geometry that includes the origin.
    void                CalcBounds( MAABox& bounds );

    // calculates the optimal AABB from geometry.
    bool                CalcBoundsExact( MAABox& bounds, bool boundsInit = false );

    // note: try to avoid these if possible.  They aren't horribly slow, but
    // they aren't free.
    const MMat4x4&      GetWorld() const;
    void                SetWorld( const MMat4x4& transform );

    // builds a list of all unique meshes in the hierarchy.
    void                BuildMeshList( UFastArray< URef< GrMesh > >& meshes );

    // optimize meshes on this node.
    void                Optimize();

    // caches or evicts child nodes.
    void                Cache();
    void                Evict();

    // load and save the node tree.
    void                Save( UWriter& writer ) const;

    // send to the renderer.
    void                GetRenderableObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters ) const;

private:
    GrModelNode( GrModel* owner, GrPolygonNode* fromTree, const std::map< tstring, std::vector< GrMeshInst* > >& meshes );

    void                InitFromPolyNode( GrModel* owner, GrPolygonNode* fromTree,
                                          const std::map< tstring, std::vector< GrMeshInst* > >& meshes );
    void                Load( GrModel* owner, UReader& reader, const UFastArray< URef< GrMesh > >& meshes,
                              unsigned short majorVer, unsigned short minorVer );

    typedef UFastArray< GrModelNode* > NodeArray;

    tstring             _name;
    GrModel*            _owner;
    GrModelNode*        _parent;
    NodeArray           _children;
    MeshInstArray       _meshes;
    MMat4x4             _local;
    MMat4x4             _world;
};
