//----------------------------------------------------------
// File:		BscVarTable.cpp
// Author:		Kevin Bray
// Created:		07-03-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "BscVarTable.h"

// project includes.
#include "UExprCallback.h"

//**********************************************************
// class BscVarTable
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
BscVarTable::BscVarTable()
{

}

//----------------------------------------------------------
BscVarTable::~BscVarTable()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
BscVarTable::RegisterVar( const tstring& varName, URef< UExprCallback > callbackTerm )
{
	B_ASSERT( callbackTerm != 0 );
	_varCallbacks[ varName ] = callbackTerm;
}

//----------------------------------------------------------
URef< UExprCallback >
BscVarTable::FindVarCallback( const tstring& name ) const
{
	// find the callback.
	VarMap::const_iterator iter = _varCallbacks.find( name );
	if ( iter == _varCallbacks.end() )
		return 0;
	return iter->second;
}

//----------------------------------------------------------
bool
BscVarTable::GetVarCallbackName( tstring& name, UExprCallback* callback ) const
{
	// search through the callbacks.
	VarMap::const_iterator iter = _varCallbacks.begin();
	VarMap::const_iterator end = _varCallbacks.end();
	for ( ; iter != end; ++iter )
	{
		// return the name if the callback functions are the same.
		if ( iter->second->GetCallback() == callback->GetCallback() )
		{
			name = iter->first;
			return true;
		}
	}

	// return false to indicate failure.
	return false;
}
