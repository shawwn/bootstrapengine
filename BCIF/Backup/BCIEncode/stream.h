// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// std c includes.
#include <assert.h>

class Stream
{
public:
	Stream::Stream();
	Stream( void* data, unsigned int size, bool ownMem );
	~Stream();

	// seeks to a given location in the stream.
	void			SetOffset( unsigned int offset )	{	_offs = offset;		}
	unsigned int	GetOffset() const					{	return _offs;		}

	// returns the pointer to the buffer.
	void*			GetBufferPtr() const				{	return _data;		}
	unsigned int	GetSize() const						{	return _size;		}

	// reserve room for 'size' number of bytes.
	void			Reserve( unsigned int size )		{	EnsureMem( size );	}

	// writes a value to the current location and advances the pointer.
	template < typename _T >
	void WriteValue( const _T& value )
	{
		// make sure we can fit enough memory.
		EnsureMem( _offs + sizeof( _T ) );
		assert( _offs + sizeof( _T ) <= _size );

		// write the value.
		char* dst = ( char* )_data + _offs;
		_offs += sizeof( _T );
		*( ( _T* )dst ) = value;
	}

	// writes a value to the current location and advances the pointer.
	template < typename _T >
	void WriteValue( unsigned int offset, const _T& value )
	{
		// make sure we can fit enough memory.
		EnsureMem( offset + sizeof( _T ) );
		assert( offset + sizeof( _T ) <= _size );

		// write the value.
		char* dst = ( char* )_data + offset;
		*( ( _T* )dst ) = value;
	}

	// reads a value from the current location and advances the pointer.
	template < typename _T >
	const _T& ReadValue()
	{
		// make sure there is still memory left to read!
		assert( ( _offs + sizeof( _T ) ) <= _size );

		// read the value.
		char* src = ( char* )_data + _offs;
		_offs += sizeof( _T );
		return *( ( _T* )src );
	}

private:
	void			EnsureMem( unsigned int size );

	void*			_data;
	unsigned int	_size;
	unsigned int	_offs;
	bool			_ownMem;
};
