//----------------------------------------------------------
// File:        ThQueue.h
// Author:      Kevin Bray
// Created:     09-01-08
//
// Purpose:     To provide an interface for manipulating an ubertexture.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ThSection.h"
#include "ThSignal.h"

//**********************************************************
// class ThQueue
//**********************************************************
template< typename _T >
class ThQueue
{
public:
    ThQueue( unsigned int size );
    ~ThQueue();

    // this is a simple interface for adding and removing items from
    // the queue.  This isn't as efficient, but it's easier to use.
    bool                    Add( const _T& value );
    bool                    Remove( _T& value );

    // adds a message into the queue.
    _T*                     BeginAdd();
    void                    FinishAdd();

    // gets the oldest message in the queue.
    const _T*               BeginRemove();
    void                    FinishRemove();

    // returns the message signal.  It is raised when a message is
    // ready to be removed, and lowered when one is not.
    const ThSignal&         GetMessageSignal() const        {   return _signal;     }

private:
    ThSection               _addSection;
    ThSection               _remSection;
    ThSignal                _signal;
    _T*                     _ringBuf;
    volatile unsigned int   _start;
    volatile unsigned int   _end;
    volatile unsigned int   _size;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template< typename _T >
ThQueue< _T >::ThQueue( unsigned int size )
: _signal( false, false )
, _size( size )
, _start( 0 )
, _end( 0 )
{
    _ringBuf = new _T[ size ];
}

//----------------------------------------------------------
template< typename _T >
ThQueue< _T >::~ThQueue()
{
    delete[] _ringBuf;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
bool
ThQueue< _T >::Add( const _T& item )
{
    // try to begin adding the item.
    _T* dst = BeginAdd();
    if ( !dst )
        return false;

    // copy the value.
    *dst = item;

    // all done.
    FinishAdd();

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
template< typename _T >
bool
ThQueue< _T >::Remove( _T& item )
{
    // try to begin removing the item.
    _T* src = BeginRemove();
    if ( !src )
        return false;

    // copy the value.
    item = *src;

    // all done.
    FinishRemove();

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
template< typename _T >
_T*
ThQueue< _T >::BeginAdd()
{
    // enter the add section.
    _addSection.Enter();

    // verify that we have room in the queue to insert a message.
    unsigned int end = _end;
    if ( ( end - _start ) == _size )
    {
        // leave the critical section and return null.
        _addSection.Leave();
        return 0;
    }

    // return a new message slot.
    return _ringBuf + ( _start % _size );
}

//----------------------------------------------------------
template< typename _T >
void
ThQueue< _T >::FinishAdd()
{
    ++_start;
    _addSection.Leave();
    _signal.Raise();
}

//----------------------------------------------------------
template< typename _T >
const _T*
ThQueue< _T >::BeginRemove()
{
    _remSection.Enter();
    unsigned int start = _start;
    if ( _end == start )
    {
        // leave the critical section and return null.
        _remSection.Leave();
        return 0;
    }

    // return the message we're removing.
    return _ringBuf + ( _end % _size );
}

//----------------------------------------------------------
template< typename _T >
void
ThQueue< _T >::FinishRemove()
{
    // increment the _end counter.
    ++_end;

    // if there are no more messages available, lower the signal.
    unsigned int start = _start;
    if ( _end == start )
        _signal.Lower();

    // leave the remove section.
    _remSection.Leave();
}
