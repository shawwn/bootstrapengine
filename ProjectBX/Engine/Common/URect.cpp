//----------------------------------------------------------
// File:		URect.h
// Author:		Kevin Bray
// Created:		03-27-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "URect.h"

//**********************************************************
// class URect
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
URect::URect( unsigned int width, unsigned int height )
: _x( 0 )
, _y( 0 )
, _width( width )
, _height( height )
{

}

//----------------------------------------------------------
URect::URect( unsigned int x, unsigned int y, unsigned int width, unsigned int height )
: _x( x )
, _y( y )
, _width( width )
, _height( height )
{

}
