//----------------------------------------------------------
// File:		UObjectPool.h
// Author:		Kevin Bray
// Created:		08-22-08
//
// Purpose:		To provide a pool of objects that can be allocated
//				one at a time and freed as a group.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UObjectPool
//**********************************************************
template < typename _T >
class UObjectPool
{
public:
	UObjectPool( unsigned int maxSize );
	~UObjectPool();

	// returns the number of items that can be allocated.
	unsigned int	GetMaxSize() const					{	return _maxSize;		}

	// allocates an object from the pool.
	_T*				Alloc( unsigned int count );

	// returns the number of elements that have been allocated.
	unsigned int	GetElemCount() const				{	return _elemCount;		}
	_T&				operator[] ( unsigned int idx );
	const _T&		operator[] ( unsigned int idx ) const;

	// clears all items.
	void			Clear();

private:
	_T*				_pool;
	unsigned int	_elemCount;
	unsigned int	_maxSize;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < typename _T >
UObjectPool< _T >::UObjectPool( unsigned int maxSize )
: _pool( 0 )
, _elemCount( 0 )
, _maxSize( maxSize )
{
	_pool = new _T[ maxSize ];
}

//----------------------------------------------------------
template < typename _T >
UObjectPool< _T >::~UObjectPool()
{
	delete[] _pool;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < typename _T >
_T*
UObjectPool< _T >::Alloc( unsigned int count )
{
	// make sure there is enough room in the pool.
	B_ASSERT( ( _elemCount + count ) < _maxSize );

	// get a pointer to the first free item and advance the
	// element count.
	_T* cur = _pool + _elemCount;
	_elemCount += count;

	// return the first item.
	return cur;
}

//----------------------------------------------------------
template < typename _T >
_T&
UObjectPool< _T >::operator[] ( unsigned int idx )
{
	B_ASSERT( idx < _elemCount );
	return _pool[ idx ];
}

//----------------------------------------------------------
template < typename _T >
const _T&
UObjectPool< _T >::operator[] ( unsigned int idx ) const
{
	B_ASSERT( idx < _elemCount );
	return _pool[ idx ];
}

//----------------------------------------------------------
template < typename _T >
void
UObjectPool< _T >::Clear()
{
	_elemCount = 0;
}
