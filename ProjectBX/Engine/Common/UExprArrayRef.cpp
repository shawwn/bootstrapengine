//----------------------------------------------------------
// File:        UExprArrayRef.cpp
// Author:      Kevin Bray
// Created:     07-05-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UExprArrayRef.h"

// project includes.
#include "UExprArray.h"

//**********************************************************
// class UExprArrayRef
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UExprArrayRef::UExprArrayRef( URef< UExprArray > array, URef< UExprTerm > index )
: _array( array )
, _index( index )
, _dynamic( false )
{
    B_ASSERT( array != 0 );
    B_ASSERT( index != 0 );

    if ( _index->IsDynamic() )
    {
        _dynamic = true;
    }
    else
    {
        _dynamic = _array->IsDynamic( _index->GetValue() );
    }
}

//----------------------------------------------------------
UExprArrayRef::~UExprArrayRef()
{

}


//==========================================================
// public methods
//==========================================================
/*
//----------------------------------------------------------
void
UExprArrayRef::SetArray( UConstRef< UExprArray > array )
{
    B_ASSERT( array != 0 );
    _array = array;
}

//----------------------------------------------------------
void
UExprArrayRef::SetIndex( UConstRef< UExprTerm > index )
{
    B_ASSERT( index != 0 );
    _index = index;
}
*/
//----------------------------------------------------------
float
UExprArrayRef::GetValue() const
{
    return _array->GetValue( _index->GetValue() );
}

//----------------------------------------------------------
bool
UExprArrayRef::IsDynamic() const
{
    return _index->IsDynamic() || _array->IsDynamic( _index->GetValue() );
}
