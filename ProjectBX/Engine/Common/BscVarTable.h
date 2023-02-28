//----------------------------------------------------------
// File:        BscVarTable.h
// Author:      Kevin Bray
// Created:     07-03-05
//
// Purpose:     A table that maps a variable name to a callback.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <map>

// forward declarations.
class UExprCallback;

//**********************************************************
// class BscArrayTable
//**********************************************************
class BscVarTable
{
public:
    BscVarTable();
    virtual ~BscVarTable();

    // when an array is added, ownership is assumed.  All added arrays
    // are deleted when this object is destroyed.
    void                        RegisterVar( const tstring& varName, URef< UExprCallback > callbackTerm );

    // returns a pointer to an array.  If the array was not found, then
    // the return value is 0.
    URef< UExprCallback >       FindVarCallback( const tstring& name ) const;
    bool                        GetVarCallbackName( tstring& name, UExprCallback* callback ) const;

private:
    typedef std::map< tstring, URef< UExprCallback > > VarMap;

    VarMap                      _varCallbacks;
};
