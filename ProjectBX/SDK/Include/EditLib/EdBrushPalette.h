//----------------------------------------------------------
// File:        EdBrushPalette.h
// Author:      Kevin Bray
// Created:     06-20-08
//
// Purpose:     To provide an interface for holding a set brushes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"
#include "URef.h"

// std c++ includes.
#include <vector>

// forward declarations.
class EdBrush;

//**********************************************************
// class EdBrushPalette
//**********************************************************
class EdBrushPalette : public URefCount
{
public:
    typedef std::vector< URef< EdBrush > > BrushVec;

    EdBrushPalette( const tstring& path, const BrushVec& brushVec );
    ~EdBrushPalette();

    // brush palette name.
    const tstring&      GetName() const     {   return _name;       }

    // rebuilds the palette.
    void                Rebuild( const BrushVec& brushVec );

    // brush management.
    unsigned int        GetBrushCount();
    URef< EdBrush >     GetBrush( unsigned int idx );

    // owned brushes.
    bool                AddBrush( URef< EdBrush > brush );
    void                RemoveBrush( unsigned int idx );
    bool                RemoveBrush( const tstring& name );
    
private:
    BrushVec            _brushes;
    tstring             _name;
};
