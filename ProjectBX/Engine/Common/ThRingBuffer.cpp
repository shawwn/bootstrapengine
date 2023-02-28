//----------------------------------------------------------
// File:		ThRingBuffer.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "ThRingBuffer.h"

//**********************************************************
// class ThRingBuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThRingBuffer::ThRingBuffer( unsigned int regionSize, unsigned int regionCount, unsigned int alignment )
: _curReadRegion( 0 )
, _curWriteRegion( 0 )
, _regionSize( regionSize )
, _regionCount( regionCount )
, _readSignal( false, false )
, _writeSignal( true, false )
{
	_buffer = ( unsigned char* )AlignedAlloc( alignment, regionCount * regionSize );
}

//----------------------------------------------------------
ThRingBuffer::~ThRingBuffer()
{
	AlignedFree( _buffer );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
ThRingBuffer::GetCurrentUsage()
{
	// calculate the total number of regions in use.
	_guard.Enter();
	unsigned int count = _curWriteRegion - _curReadRegion + 1;
	_guard.Leave();
	return count;
}

//----------------------------------------------------------
void*
ThRingBuffer::GetWriteRegion( bool wait )
{
	// wait for a write region to become available.
	if ( wait )
		_writeSignal.Wait();

	// calculate the region's starting address.
	unsigned int localRegion = ( _curWriteRegion % _regionCount );
	unsigned int localOffset = localRegion * _regionSize;

	// return the starting address to indicate success.
	return _buffer + localOffset;
}

//----------------------------------------------------------
const void*
ThRingBuffer::GetReadRegion( bool wait )
{
	// wait for data to become available.
	if ( wait )
		_readSignal.Wait();

	// calculate the region's starting address.
	unsigned int localRegion = ( _curReadRegion % _regionCount );
	unsigned int localOffset = localRegion * _regionSize;

	// return the starting address to indicate success.
	return _buffer + localOffset;
}

//----------------------------------------------------------
void*
ThRingBuffer::NextWriteRegion( bool wait )
{
	// wait if necessary.
	if ( wait )
		_writeSignal.Wait();

	// enter the region.
	_guard.Enter();

	// determine if there is room to advance.
	void* returnValue = 0;
	unsigned int usage = _curWriteRegion - _curReadRegion;
	if ( usage < _regionCount )
	{
		// if the read signal is lowered, raise it.
		if ( usage == 0 )
			_readSignal.Raise();

		// increment the current write region and the usage
		// count.
		_curWriteRegion += 1;
		usage += 1;

		// calculate the region's starting address.
		unsigned int localRegion = ( _curReadRegion % _regionCount );
		unsigned int localOffset = localRegion * _regionSize;

		// if we're in the last writable region, lower the
		// write signal.  Otherwise, return the start of the
		// current region.
		if ( usage == _regionCount )
			_writeSignal.Lower();
		else
			returnValue = _buffer + localOffset;
	}

	_guard.Leave();

	// wait if need be.
	if ( wait && returnValue == 0 )
		returnValue = GetWriteRegion( true );

	// return the buffer.
	return returnValue;
}

//----------------------------------------------------------
const void*
ThRingBuffer::NextReadRegion( bool wait )
{
	_guard.Enter();

	// determine if we can allow the read region to advance.
	const void* returnValue = 0;
	if ( _curReadRegion < _curWriteRegion )
	{
		// advance the read region.
		_curReadRegion += 1;

		// calculate the region's starting address.
		unsigned int localRegion = ( _curReadRegion % _regionCount );
		unsigned int localOffset = localRegion * _regionSize;

		// check to see if we need to raise the write signal.
		unsigned int usage = _curWriteRegion - _curReadRegion;
		if ( usage < _regionCount )
			_writeSignal.Raise();

		// check to see if we need to lower the read signal.
		if ( usage == 0 )
			_readSignal.Lower();
		else
			returnValue = _buffer + localOffset;
	}

	_guard.Leave();

	// wait if necessary.
	if ( wait && returnValue == 0 )
		returnValue = GetReadRegion( true );

	// return the buffer.
	return returnValue;
}
