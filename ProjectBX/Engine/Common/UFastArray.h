//----------------------------------------------------------
// File:		UFastArray.h
// Author:		Kevin Bray
// Created:		08-05-05
//
// Purpose:		To act like a std::vector, only faster.  Avoids
//				dynamic memory allocation if small enough.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <algorithm>

//**********************************************************
// class UFastArray
//**********************************************************
template < class _Type, unsigned int kCacheSize = 10, unsigned int kGrowSize = 10 >
class UFastArray
{
public:
	UFastArray( unsigned int initSize = 0 );
	UFastArray( const UFastArray& array );
	~UFastArray();

	inline unsigned int	GetElemCount() const						{	return _elemCount;	}
	inline _Type&		operator [] ( unsigned int i )				{	B_ASSERT( i < _elemCount ); return _ptr[ i ];	}
	inline const _Type&	operator [] ( unsigned int i ) const		{	B_ASSERT( i < _elemCount ); return _ptr[ i ];	}
	inline _Type&		First()										{	B_ASSERT( _elemCount > 0 ); return _ptr[ 0 ];			}
	inline const _Type&	First()	const								{	B_ASSERT( _elemCount > 0 ); return _ptr[ 0 ];			}
	inline _Type&		Last()										{	B_ASSERT( _elemCount > 0 ); return _ptr[ _elemCount ];	}
	inline const _Type&	Last() const								{	B_ASSERT( _elemCount > 0 ); return _ptr[ _elemCount ];	}

	inline void			Insert( unsigned int before, const _Type& elem );
	inline void			Erase( unsigned int index );
	inline void			Push( const _Type& elem );
	inline _Type&		Push();
	inline _Type& 		Pop();

	// faster version of Erase, moves last elem into index instead of
	// shifting whole array
	inline void 		SwapErase(unsigned int index);

	// ensures that storage for the number of objects specified is allocated.
	void				Reserve( unsigned int size );
	unsigned int		GetReserved() const							{	return _elemCount;								}

	// resizes the array to the size specified.
	void				Resize( unsigned int size, const _Type& value );

	template< class _Cmp >
	void				Sort( _Cmp cmpLess )						{	std::sort( _ptr, _ptr+_elemCount, cmpLess );	}
	void				Sort()										{	std::sort( _ptr, _ptr+_elemCount );				}

	template< class _Cmp >
	void				SortRange( _Cmp cmpLess, unsigned int start, unsigned  count )
	{ B_ASSERT( start + count <= _elemCount ); std::sort( _ptr + start, _ptr + start + count, cmpLess );	}

	void				SortRange( unsigned int start, unsigned  count )
	{ B_ASSERT( start + count <= _elemCount ); std::sort( _ptr + start, _ptr + start + count );				}

	inline UFastArray&	operator = ( const UFastArray& array );

	// Note!  Clear DOES NOT actually destroy the objects or replace them with
	// default constructed objects.  This is for performance.
	inline void			Clear()										{	_elemCount = 0;		}

	_Type*				GetPtr() const								{	return _ptr;		}

private:
	void				EnsureSize( unsigned int size );
	void				Assign( const UFastArray& array );

	_Type*				_ptr;
	_Type				_cache[ kCacheSize ];
	unsigned int		_memSize;
	unsigned int		_elemCount;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
UFastArray< _Type, kCacheSize, kGrowSize >::UFastArray( unsigned int initSize )
: _ptr( _cache )
, _memSize( 0 )
, _elemCount( 0 )
{
	EnsureSize( initSize );
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
UFastArray< _Type, kCacheSize, kGrowSize >::UFastArray( const UFastArray& array )
: _ptr( _cache )
, _memSize( 0 )
, _elemCount( 0 )
{
	Assign( array );
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
UFastArray< _Type, kCacheSize, kGrowSize >::~UFastArray()
{
	if ( _memSize )
		delete[] _ptr;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Insert( unsigned int before, const _Type& elem )
{
	B_ASSERT( before <= _elemCount );

	EnsureSize( _elemCount + 1 );
	unsigned int i = _elemCount;
	while ( i-- > before )
	{
		_ptr[ i + 1 ] = _ptr[ i ];
	}
	_ptr[ before ] = elem;
	++_elemCount;
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Erase( unsigned int index )
{
	B_ASSERT( index < _elemCount );

	--_elemCount;
	while ( index++ < _elemCount )
	{
		_ptr[ index - 1 ] = _ptr[ index ];
	}
	_ptr[ index - 1 ] = _Type();
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::SwapErase( unsigned int index )
{
	B_ASSERT( index < _elemCount );

	--_elemCount;
	if ( index < _elemCount )
		_ptr[ index  ] = _ptr[ _elemCount ];
	_ptr[ _elemCount ] = _Type();
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Push( const _Type& elem )
{
	Push() = elem;
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
inline _Type&
UFastArray< _Type, kCacheSize, kGrowSize >::Push()
{
	EnsureSize( _elemCount + 1 );
	return _ptr[ _elemCount++ ];
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
_Type&
UFastArray< _Type, kCacheSize, kGrowSize >::Pop()
{
	B_ASSERT( _elemCount != 0 );
	return _ptr[ --_elemCount ];
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Reserve( unsigned int size )
{
	EnsureSize( size );
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Resize( unsigned int size, const _Type& value )
{
	EnsureSize( size );
	for ( unsigned int i = _elemCount; i < size; ++i )
		_ptr[ i ] = value;
	_elemCount = size;
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
UFastArray< _Type, kCacheSize, kGrowSize >&
UFastArray< _Type, kCacheSize, kGrowSize >::operator = ( const UFastArray< _Type, kCacheSize, kGrowSize >& array )
{
	Assign( array );
	return *this;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::EnsureSize( unsigned int size )
{
	if ( _memSize )
	{
		// we've already allocated memory, so see if we need to allocate more.
		if ( size > _memSize )
		{
			// allocate more memory and copy old data.
			_memSize = size + ( kGrowSize - ( size % kGrowSize ) );
			_Type* ptr = new _Type[ _memSize ];
			unsigned int i = _elemCount;
			while ( i-- > 0 )
				ptr[ i ] = _ptr[ i ];

			// delete the old memory and set to the new memory.
			delete[] _ptr;
			_ptr = ptr;
		}
	}
	else if ( size > kCacheSize )
	{
		// we haven't allocated memory, but need to.  Allocate memory and copy
		// old data.
		_memSize = size + ( kGrowSize - ( size % kGrowSize ) );
		_Type* ptr = new _Type[ _memSize ];
		unsigned int i = _elemCount;
		while ( i-- > 0 )
			ptr[ i ] = _ptr[ i ];

		// set to the newly allocated memory.
		_ptr = ptr;
	}
}

//----------------------------------------------------------
template < class _Type, unsigned int kCacheSize, unsigned int kGrowSize >
void
UFastArray< _Type, kCacheSize, kGrowSize >::Assign( const UFastArray< _Type, kCacheSize, kGrowSize >& array )
{
	unsigned int elemCount = array.GetElemCount();
	EnsureSize( elemCount );
	for ( unsigned int i = 0; i < elemCount; ++i )
		_ptr[ i ] = array._ptr[ i ];
	_elemCount = elemCount;
}
