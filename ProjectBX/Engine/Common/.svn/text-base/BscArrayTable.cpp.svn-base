//----------------------------------------------------------
// File:		BscArrayTable.cpp
// Author:		Kevin Bray
// Created:		07-03-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "BscArrayTable.h"

// project includes.
#include "UExprArray.h"

//**********************************************************
// class BscArrayTable
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
BscArrayTable::BscArrayTable()
{

}

//----------------------------------------------------------
BscArrayTable::BscArrayTable( const ArrayMap& predefinedArrays )
: _arrays( predefinedArrays )
{

}

//----------------------------------------------------------
BscArrayTable::~BscArrayTable()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
BscArrayTable::AddArray( const tstring& name, URef< UExprArray > array )
{
	B_ASSERT( array != 0 );
	_arrays[ name ] = array;
}

//----------------------------------------------------------
URef< UExprArray >
BscArrayTable::FindArray( const tstring& name ) const
{
	// find the array.
	ArrayMap::const_iterator iter = _arrays.find( name );
	if ( iter == _arrays.end() )
		return 0;
	return iter->second;
}
