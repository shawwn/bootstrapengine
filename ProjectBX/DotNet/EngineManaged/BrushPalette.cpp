//----------------------------------------------------------
// File:        BrushPalette.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "BrushPalette.h"

// project includes.
#include "Utils.h"

// edlib includes.
#include "EdBrushPaletteMgr.h"
#include "EdBrushPalette.h"
#include "EdBrush.h"
#include "EdSave.h"

// managed includes.
#include "Brush.h"

// .NET namespaces.
using namespace System;

//**********************************************************
// ref class BrushPalette
//**********************************************************

//----------------------------------------------------------
Bootstrap::BrushPalette::BrushPalette( EdBrushPalette* palette )
: _palette( palette )
{
    // increment the brush palette's reference count.
    _palette->IncRef();
}

//----------------------------------------------------------
Bootstrap::BrushPalette::BrushPalette( String^ name )
{
    tstring errors;
    _palette = gEdBrushPaletteMgr->GetBrushPalette( StrToTStr( name ), errors );

    // increment the brush palette's reference count.
    _palette->IncRef();
}

//----------------------------------------------------------
Bootstrap::BrushPalette::~BrushPalette()
{
    // forward to the finalizer.
    this->!BrushPalette();
}

//----------------------------------------------------------
Bootstrap::BrushPalette::!BrushPalette()
{
    // decrement the brush palette's reference count.
    _palette->DecRef();
}

//----------------------------------------------------------
Bootstrap::BrushPalette^ 
Bootstrap::BrushPalette::Create( String^ name )
{
    // check to see if this palette is already being used.
    tstring error;
    URef< EdBrushPalette > palette = gEdBrushPaletteMgr->GetBrushPalette( StrToTStr( name ), error );

    // return a pointer to it.
    if( palette )
        return gcnew BrushPalette( palette );

    // otherwise, create a new palette.
    return gcnew BrushPalette( gEdBrushPaletteMgr->SetBrushPalette( StrToTStr( name ), EdBrushPalette::BrushVec() ) );
}

//----------------------------------------------------------
void 
Bootstrap::BrushPalette::Remove( System::String^ name )
{
    // remove the palette.
    gEdBrushPaletteMgr->RemoveBrushPalette( StrToTStr( name ) );
}

//----------------------------------------------------------
Bootstrap::BrushPalette^ 
Bootstrap::BrushPalette::TryLoad( System::String^ name )
{
    // try to load the brush palette.
    tstring error;
    URef< EdBrushPalette > palette = gEdBrushPaletteMgr->GetBrushPalette( StrToTStr( name ), error );

    // if the palette could not be loaded, return null.
    if( !palette )
        return nullptr;

    // otherwise, return a pointer to the loaded palette.
    return gcnew BrushPalette( palette );
}

//----------------------------------------------------------
array< String^ >^ 
Bootstrap::BrushPalette::GetBrushPaletteNameList()
{
    // iterate through each brush palette and build our managed array.
    unsigned int count = gEdBrushPaletteMgr->GetBrushPaletteCount();
    array< String^ >^ newArray = gcnew array< String^ >( count );

    // populate the list of brush palettes.
    for ( unsigned int i = 0; i < count; ++i )
    {
        URef< EdBrushPalette > palette = gEdBrushPaletteMgr->GetBrushPaletteByIdx( i ); 
        newArray->SetValue( TStrToStr( palette->GetName() ), ( int )i );
    }

    // return the new array.
    return newArray;
}

//----------------------------------------------------------
void 
Bootstrap::BrushPalette::SaveBrushPalettes()
{
    EdSave save;
    save.SaveBrushPaletteData();
}

//----------------------------------------------------------
String^ 
Bootstrap::BrushPalette::Name::get()
{
    return TStrToStr( _palette->GetName() );
}

//----------------------------------------------------------
unsigned int 
Bootstrap::BrushPalette::GetBrushCount()
{
    return _palette->GetBrushCount();
}

//----------------------------------------------------------
Bootstrap::Brush^ 
Bootstrap::BrushPalette::GetBrush( unsigned int idx )
{
    URef< EdBrush > brush = _palette->GetBrush( idx );
    if( brush )
        return gcnew Brush( brush );
    else
        return nullptr;
}

//----------------------------------------------------------
array< Bootstrap::Brush^ >^ 
Bootstrap::BrushPalette::GetBrushes()
{
    // iterate through each brush and build our managed array.
    unsigned int count = GetBrushCount();
    array< Brush^ >^ newArray = gcnew array< Brush^ >( count );

    // populate the list of brushes.
    for ( unsigned int i = 0; i < count; ++i )
    {
        newArray->SetValue( GetBrush( i ), ( int )i );
    }

    // return the new array.
    return newArray;
}

//----------------------------------------------------------
bool 
Bootstrap::BrushPalette::AddBrush( Brush^ brush )
{
    return _palette->AddBrush( brush->_brush );
}

//----------------------------------------------------------
void 
Bootstrap::BrushPalette::RemoveBrush( unsigned int idx )
{
    _palette->RemoveBrush( idx );
}

//----------------------------------------------------------
void 
Bootstrap::BrushPalette::RemoveBrush( System::String^ name )
{
    _palette->RemoveBrush( StrToTStr( name ) );
}