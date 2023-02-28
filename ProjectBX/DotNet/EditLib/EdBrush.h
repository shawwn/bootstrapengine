//----------------------------------------------------------
// File:        EdBrush.h
// Author:      Kevin Bray
// Created:     05-17-08
//
// Purpose:     To provide an shading setup for painting to
//              ubertextures.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrSoftwareRasterizer.h"
#include "GrConfig.h"
#include "URef.h"

// forward declarations.
class EdBrushImage;

//**********************************************************
// class EdBrush
//**********************************************************
class EdBrush : public URefCount
{
public:
    typedef GrSoftwareRasterizer::VertexProgram VertexProgram;
    typedef GrSoftwareRasterizer::FragmentProgram FragmentProgram;

    struct SInfo
    {
        VertexProgram vertexProgram;
        FragmentProgram fragmentProgram;
        URef< EdBrushImage > images[ ES_COUNT+1 ];
        URef< EdBrushImage > mask;
    };

    // ctor & dtor
    EdBrush( const tstring& name, const SInfo& info );
    virtual ~EdBrush();

    // returns the name of the brush.
    const tstring&          GetName() const                 {   return _name;               }
    const tstring&          GetShortName() const            {   return _shortName;          }

    // rebuilds the brush.
    void                    Rebuild( const SInfo& info );

    // gets/sets the mask texture.
    void                    SetMask( URef< EdBrushImage > image );
    URef< EdBrushImage >    GetMask() const;

    // gets/sets images on the brush.  Note that stage == ES_COUNT is
    // the user texture.
    void                    SetStageSource( ESTAGE stage, URef< EdBrushImage > image );
    URef< EdBrushImage >    GetStageSource( ESTAGE stage ) const;

    // stage source pre-cache/evict.
    void                    PreCacheStageSources();
    void                    EvictStageSources();

    // return the vertex/fragment programs.
    VertexProgram           GetVertexProgram() const        {   return _vertexProgram;      }
    FragmentProgram         GetFragmentProgram() const      {   return _fragmentProgram;    }

private:
    tstring                 _name;
    tstring                 _shortName;             // after the final '/'.
    URef< EdBrushImage >    _images[ ES_COUNT+1 ];
    URef< EdBrushImage >    _mask;
    VertexProgram           _vertexProgram;
    FragmentProgram         _fragmentProgram;
};
