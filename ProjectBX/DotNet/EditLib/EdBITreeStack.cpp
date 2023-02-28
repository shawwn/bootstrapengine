//----------------------------------------------------------
// File:		EdBITreeStack.cpp
// Author:		Kevin Bray
// Created:		08-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBITreeStack.h"


//**********************************************************
// class EdBITreeStack
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBITreeStack::EdBITreeStack( unsigned int depth )
: _stack( 0 )
, _depth( depth )
, _top( 0 )
{
	// allocate the stack.
	_stack = ( SEntry* )AlignedAlloc( 16, depth * sizeof( SEntry ) );
}

//----------------------------------------------------------
EdBITreeStack::~EdBITreeStack()
{
	AlignedFree( _stack );
}
	

//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
EdBITreeStack::SEntry&
EdBITreeStack::Push()
{
	B_ASSERT( _top < _depth );

	// get the new entry.
	SEntry& curTop = _stack[ _top++ ];
	return curTop;
}

//----------------------------------------------------------
const EdBITreeStack::SEntry&
EdBITreeStack::Pop()
{
	return _stack[ --_top ];
}

//----------------------------------------------------------
const EdBITreeStack::SEntry&
EdBITreeStack::Top()
{
	B_ASSERT( _top > 0 );
	return _stack[ _top - 1 ];
}
