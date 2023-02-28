//----------------------------------------------------------
// File:        URefArray.h
// Author:      Kevin Bray
// Created:     09-19-04
//
// Purpose:     To manage an array of reference counted objects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"
#include "URef.h"

//**********************************************************
// class URefArray
//**********************************************************
template < typename _T >
class URefArray
{
public:
    URefArray();
    ~URefArray();

    // collect garbage.
    void                        CollectGarbage();

    // add another item.
    void                        AddItem( URef< _T > ref );

    // item traversal.
    unsigned int                GetElemCount() const                    {   return _refs.GetElemCount();    }
    URef< _T >                  operator [] ( unsigned int idx ) const  {   return _refs[ idx ];            }

private:
    UFastArray< URef< _T > >    _refs;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < typename _T >
URefArray< _T >::URefArray()
{

}

//----------------------------------------------------------
template < typename _T >
URefArray< _T >::~URefArray()
{
    // make sure all resources have been freed.
#ifdef _DEBUG
    for ( unsigned int i = 0; i < _refs.GetElemCount(); ++i )
    {
        // we should be the only thing holding on to any references
        // at this point.
        B_ASSERT( _refs[ i ]->GetRef() == 1 );
    }
#endif
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < typename _T >
void
URefArray< _T >::CollectGarbage()
{
    unsigned int i = _refs.GetElemCount();
    while ( i-- > 0 )
    {
        if ( _refs[ i ]->GetRef() )
            _refs.Erase( i );
    }
}

//----------------------------------------------------------
template < typename _T >
void
URefArray< _T >::AddItem( URef< _T > ref )
{
    // add the item.
    B_ASSERT( ref != 0 );
    _refs.Push() = ref;
}
