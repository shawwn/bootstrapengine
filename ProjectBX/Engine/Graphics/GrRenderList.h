//----------------------------------------------------------
// File:        GrRenderList.h
// Author:      Kevin Bray
// Created:     03-15-06
//
// Purpose:     To contain and manage a list of objects to render.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"
#include "UFastArray.h"

class GrUberPatchSet;
class GrMeshInst;
class GrCamera;
class GrFrustum;
class MVec3;

//**********************************************************
// class GrRenderList
//**********************************************************
class GrRenderList
{
public:
    struct SItem
    {
        SItem() : meshInst( 0 ), range( 0 ), preTransformed( 0 ) { }
        SItem( GrMeshInst* meshInst, unsigned int range, bool preTransformed )
            : meshInst( meshInst ), range( range ), preTransformed( preTransformed ? 1 : 0 ) { }

        GrMeshInst* meshInst;
        unsigned int range;
        unsigned int preTransformed;        // either 0 or 1.
    };
    typedef UFastArray<SItem, 128, 128> ItemArray;
    typedef UFastArray<GrUberPatchSet*, 128, 128> UberPatchSetArray;
    typedef bool ( *ItemCompare )( const SItem& left, const SItem& right );

    GrRenderList();
    ~GrRenderList();

    // adds a mesh instance or a mesh instance range to the list.
    void                        AddMeshInstRange( GrMeshInst* meshInst, unsigned int range, bool preTransformed );
    void                        AddMeshInstRanges( GrMeshInst* meshInst, unsigned int startRange, unsigned int rangeCount, bool preTransformed );
    void                        AddMeshInst( GrMeshInst* meshInst, bool preTransformed );
    void                        Add( const SItem& item );

    // adds an ubertexture patch set.
    void                        AddUberPatchSet( GrUberPatchSet* patchSet );

    // sorts the objects by material various criteria.
    void                        Sort( ItemCompare compare ) const;
    void                        SortByState() const;
    void                        SortByStateAndFrontToBack( const GrCamera& camera ) const;

    // renderable accessors.
    unsigned int                GetItemCount() const                        {   return _items.GetElemCount();       }
    const SItem&                GetItem( unsigned int idx ) const           {   return _items[ idx ];               }
    const ItemArray&            GetItemArray() const                        {   return _items;                      }
    const SItem&                operator[] ( unsigned int idx ) const       {   return _items[ idx ];               }

    // uber-patch accessors.
    unsigned int                GetUberPatchSetCount() const                {   return _patches.GetElemCount();     }
    GrUberPatchSet*             GetUberPatchSet( unsigned int idx ) const   {   return _patches[ idx ];             }

    // other stuff.
    void                        GetVisibleItems( GrRenderList& visible, const GrFrustum& frustum ) const;

    // clears the list.
    void                        Clear();

private:
    mutable ItemArray           _items;
    mutable UberPatchSetArray   _patches;
};
