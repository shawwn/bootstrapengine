//----------------------------------------------------------
// File:		BscArrayTable.h
// Author:		Kevin Bray
// Created:		07-03-05
//
// Purpose:		A table that maps array names to the actual array.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"

// std c++ includes.
#include <map>

// forward declarations.
class UExprArray;

//**********************************************************
// class BscArrayTable
//**********************************************************
class BscArrayTable
{
public:
	typedef std::map< tstring, URef< UExprArray > > ArrayMap;

	BscArrayTable();
	BscArrayTable( const ArrayMap& predefinedArrays );
	virtual ~BscArrayTable();

	// add or find an array.
	void					AddArray( const tstring& name, URef< UExprArray > array );
	URef< UExprArray >		FindArray( const tstring& name ) const;

private:
	ArrayMap				_arrays;
};
