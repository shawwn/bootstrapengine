//----------------------------------------------------------
// File:		EdBrushPalette.cpp
// Author:		Kevin Bray
// Created:		06-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBrushPalette.h"

// project includes.
#include "EdBrush.h"


//**********************************************************
// class EdBrushPalette
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBrushPalette::EdBrushPalette( const tstring& name, const BrushVec& brushVec )
: _name( name )
{
	_brushes = brushVec;
}

//----------------------------------------------------------
EdBrushPalette::~EdBrushPalette()
{

}


//==========================================================
// public
//==========================================================

//----------------------------------------------------------
void
EdBrushPalette::Rebuild( const BrushVec& brushVec )
{
	_brushes = brushVec;
}

//----------------------------------------------------------
unsigned int
EdBrushPalette::GetBrushCount()
{
	return ( unsigned int )_brushes.size();
}

//----------------------------------------------------------
URef< EdBrush >
EdBrushPalette::GetBrush( unsigned int idx )
{
	B_ASSERT( idx < _brushes.size() );
	return _brushes[ idx ];
}

//----------------------------------------------------------
bool
EdBrushPalette::AddBrush( URef< EdBrush > brush )
{
	// make sure the brush doesn't already exist in the palette.
	unsigned int brushCount = ( unsigned int )_brushes.size();
	for ( unsigned int i = 0; i < brushCount; ++i )
		if ( _brushes[ i ] == brush )
			return false;

	// add the brush to the brush list.
	_brushes.push_back( brush );

	// return true to indicate that the brush was in fact added.
	return true;
}

//----------------------------------------------------------
void
EdBrushPalette::RemoveBrush( unsigned int idx )
{
	B_ASSERT( idx < _brushes.size() );
	_brushes.erase( _brushes.begin() + idx );
}

//----------------------------------------------------------
bool
EdBrushPalette::RemoveBrush( const tstring& name )
{
	int i = 0;
	for( BrushVec::iterator it = _brushes.begin(); it != _brushes.end(); ++it, ++i )
	{
		URef< EdBrush > brush = *it;
		if( brush->GetName() == name )
		{
			RemoveBrush( i );
			return true;
		}
	}

	return false;
}