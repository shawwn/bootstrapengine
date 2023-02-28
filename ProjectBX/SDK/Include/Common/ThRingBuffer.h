//----------------------------------------------------------
// File:        ThRingBuffer.h
// Author:      Kevin Bray
// Created:     09-01-08
//
// Purpose:     To provide a ring-buffer interface.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class includes.
#include "ThPipe.h"

// project includes.
#include "ThSignal.h"
#include "ThSection.h"

//**********************************************************
// class ThRingBuffer
//**********************************************************
class ThRingBuffer : public ThPipe
{
public:
    ThRingBuffer( unsigned int regionSize, unsigned int regionCount, unsigned int alignment = 16 );
    ~ThRingBuffer();

    // region info.
    unsigned int    GetRegionSize() const           {   return _regionSize;     }
    unsigned int    GetRegionCount() const          {   return _regionCount;    }

    // returns how many regions are currently in use between the
    // read and write locations.
    unsigned int    GetCurrentUsage();

    // returns the current write region.
    void*           GetWriteRegion( bool wait = true );
    const void*     GetReadRegion( bool wait = true );

    // region access.
    void*           NextWriteRegion( bool wait = true );
    const void*     NextReadRegion( bool wait = true );

    // signal access.
    const ThSignal& GetReadSignal() const           {   return _readSignal;     }
    const ThSignal& GetWriteSignal() const          {   return _writeSignal;    }

private:
    // current state.
    unsigned int    _curReadRegion;
    unsigned int    _curWriteRegion;

    // region info.
    unsigned int    _regionSize;
    unsigned int    _regionCount;

    // variable safe-guard.
    ThSection       _guard;

    // signals.
    ThSignal        _readSignal;        // raised if NextReadRegion can be called.
    ThSignal        _writeSignal;       // raised if NextWriteRegion can be called.

    // memory.
    unsigned char*  _buffer;
};
