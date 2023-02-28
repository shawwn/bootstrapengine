//----------------------------------------------------------
// File:        ThArray.h
// Author:      Kevin Bray
// Created:     09-01-08
//
// Purpose:     To provide an interface for shared data between threads.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ThSection.h"
#include "ThSignal.h"
#include "ThBuffer.h"

//**********************************************************
// class ThArray
//**********************************************************
template< typename _T >
class ThArray
{
public:
    ThArray( unsigned int size, unsigned int alignment = 16 );
    ~ThArray() { }

    // returns the size of the buffer, in elements.
    unsigned int        GetSize() const                     {   return ( _buffer.GetSize() / sizeof( _T ) );    }

    // aquires access to the data.  No one can access this data
    // except for whoever acquired this buffer.  All other accesses
    // will block or return NULL.
    _T*                 Acquire( bool wait = true )         {   return ( _T* )_buffer.Acquire( wait );          }
    const _T*           Acquire( bool wait = true ) const   {   return ( const _T* )_buffer.Acquire( wait );    }
    void                Release() const                     {   return _buffer.Release();                       }

    // optional signal to indicate that can be used to indicate
    // whether or not this buffer has been used.
    ThSignal&           GetSignal()                         {   return _buffer.GetSignal();                     }
    void                RaiseSignal()                       {   _buffer.GetSignal().Raise();                    }
    void                LowerSignal()                       {   _buffer.GetSignal().Lower();                    }

private:
    ThArray( const ThArray& ) : _buffer( 1, 1 ) { }
    ThArray& operator = ( const ThArray& ) { return *this; }

    // buffer that we manage.
    ThBuffer            _buffer;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template< typename _T >
ThArray< _T >::ThArray( unsigned int size, unsigned int alignment )
: _buffer( size, alignment )
{

}