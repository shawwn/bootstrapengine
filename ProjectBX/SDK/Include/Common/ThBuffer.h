//----------------------------------------------------------
// File:		ThBuffer.h
// Author:		Kevin Bray
// Created:		09-01-08
//
// Purpose:		To provide an interface for shared data between threads.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ThSection.h"
#include "ThSignal.h"

//**********************************************************
// class ThBuffer
//**********************************************************
class ThBuffer
{
public:
	ThBuffer( unsigned int size, unsigned int alignment = 16 );
	~ThBuffer();

	// returns the size of the buffer, in bytes.
	unsigned int		GetSize() const					{	return _size;			}

	// aquires access to the data.  No one can access this data
	// except for whoever acquired this buffer.  All other accesses
	// will block or return NULL.
	void*				Acquire( bool wait = true );
	const void*			Acquire( bool wait = true ) const;
	void				Release() const;

	// optional signal to indicate that can be used to indicate
	// whether or not this buffer has been used.
	ThSignal&			GetSignal()						{	return _signal;			}

	// get the current write offset.
	unsigned int		GetWriteOffset() const			{	return _writeOffset;	}
	void				AdvanceWriteOffset( unsigned int amount );

private:
	ThBuffer( const ThBuffer& ) : _signal( false, false ) { }
	ThBuffer& operator = ( const ThBuffer& ) { return *this; }

	unsigned int		_size;
	void*				_buffer;
	unsigned int		_writeOffset;
	ThSection			_writeOffsetSection;
	mutable ThSection	_section;
	mutable ThSignal	_signal;
};
