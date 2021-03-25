// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#include "stream.h"

// std c includes.
#include <memory.h>

// ctor
Stream::Stream()
: _data( 0 )
, _size( 0 )
, _offs( 0 )
, _ownMem( true )
{

}

// ctor
Stream::Stream( void* data, unsigned int size, bool ownMem )
: _data( data )
, _size( size )
, _offs( 0 )
, _ownMem( ownMem )
{

}

// dtor
Stream::~Stream()
{
	if ( _ownMem )
		delete[] _data;
}

void Stream::EnsureMem( unsigned int size )
{
	const unsigned int kExtra = 1024;
	if ( size > _size )
	{
		// calculate the new size.
		unsigned int newSize = size + kExtra;

		// allocate new memory, copy contents, and free the old.
		void* newMem = new char[ newSize ];
		memcpy( newMem, _data, _size );
		delete[] _data;

		// store the new size.
		_data = newMem;
		_size = newSize;
	}
}
