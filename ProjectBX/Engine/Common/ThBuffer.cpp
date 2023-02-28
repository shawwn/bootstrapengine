//----------------------------------------------------------
// File:		ThBuffer.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// file header.
#include "ThBuffer.h"

//**********************************************************
// class ThBuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThBuffer::ThBuffer( unsigned int size, unsigned int alignment )
: _size( size )
, _buffer( 0 )
, _signal( false, false )
{
	_buffer = AlignedAlloc( alignment, _size );
}

//----------------------------------------------------------
ThBuffer::~ThBuffer()
{
	AlignedFree( _buffer );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void*
ThBuffer::Acquire( bool wait )
{
	// if we're supposed to wait, simply try to enter the section.
	if ( wait )
	{
		_section.Enter();
		return _buffer;
	}

	// if we're not supposed to wait, try to enter the section.  If
	// we can't enter the section, return null.
	if ( !_section.TryEnter() )
		return 0;

	// return the buffer's pointer.
	return _buffer;
}

//----------------------------------------------------------
const void*
ThBuffer::Acquire( bool wait ) const
{
	// if we're supposed to wait, simply try to enter the section.
	if ( wait )
	{
		_section.Enter();
		return _buffer;
	}

	// if we're not supposed to wait, try to enter the section.  If
	// we can't enter the section, return null.
	if ( !_section.TryEnter() )
		return 0;

	// return the buffer's pointer.
	return _buffer;
}

//----------------------------------------------------------
void
ThBuffer::Release() const
{
	_section.Leave();
}

//----------------------------------------------------------
void
ThBuffer::AdvanceWriteOffset( unsigned int amount )
{
	_writeOffsetSection.Enter();
	_writeOffset += amount;
	_writeOffsetSection.Leave();
}
