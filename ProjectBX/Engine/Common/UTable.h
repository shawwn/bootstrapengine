//----------------------------------------------------------
// File:		UTable.h
// Author:		Kevin Bray
// Created:		01-21-06
//
// Purpose:		To allow for fast identification of free slots in
//				an array.  Note that this cannot be used with anything
//				that has a programmer supplied constructor.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"

//**********************************************************
// class UTable
//**********************************************************
template < typename _T, const unsigned int kGrowSize = 16 >
class UTable
{
public:
	UTable( unsigned int initSize = kGrowSize );
	~UTable();

	// item insertion and removal.
	unsigned int	Insert( const _T& item );
	void			Remove( unsigned int id );

	_T&				operator [] ( unsigned int id );
	const _T&		operator [] ( unsigned int id ) const;

private:
#ifdef _DEBUG
	struct Item
	{
		bool			isUsed;
		union
		{
			int				nextFree;
			_T				item;
		};
	};
#else
	union Item
	{
		int				nextFree;
		_T				item;
	};
#endif

	Item*				_items;
	unsigned int		_arraySize;
	unsigned int		_firstFree;
};

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
UTable< _T, kGrowSize >::UTable( unsigned int initSize )
: _items( 0 )
, _arraySize( initSize )
, _firstFree( 0 )
{
	_items = new Item[ initSize ];
	for ( unsigned int i = 0; i < initSize; ++i )
	{
		_items[ i ].nextFree = i+1;
	}
}

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
UTable< _T, kGrowSize >::~UTable()
{
	delete[] _items;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
unsigned int
UTable< _T, kGrowSize >::Insert( const _T& item )
{
	// expand.
	if ( _firstFree == _arraySize )
	{
		unsigned int sizeNeeded = _arraySize + kGrowSize;
		Item* newMem = new Item[ sizeNeeded ];

		// copy old items.
		for ( unsigned int i = 0; i < _arraySize; ++i )
			newMem[ i ] = _items[ i ];

		// link new items.
		for ( unsigned int i = _arraySize; i < sizeNeeded; ++i )
		{
			newMem[ i ].nextFree = i+1;
#ifdef _DEBUG
			newMem[ i ].isUsed = false;
#endif
		}

		// set the internal pointers.
		_items = newMem;
		_arraySize = sizeNeeded;
	}

	// allocate our items.
	unsigned int newId = _firstFree;
	_firstFree = _items[ newId ].nextFree;
	_items[ newId ].item = item;
#ifdef _DEBUG
	_items[ newId ].isUsed = true;
#endif
	return newId;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
void
UTable< _T, kGrowSize >::Remove( unsigned int id )
{
	B_ASSERT( id < _arraySize );
#ifdef _DEBUG
	B_ASSERT( _items[ id ].isUsed );
	_items[ id ].isUsed = false;
#endif
	_items[ id ].nextFree = _firstFree;
	_firstFree = id;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
_T&
UTable< _T, kGrowSize >::operator [] ( unsigned int id )
{
	B_ASSERT( id < _arraySize );
#ifdef _DEBUG
	B_ASSERT( _items[ id ].isUsed );
#endif
	return _items[ id ].item;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kGrowSize >
const _T&
UTable< _T, kGrowSize >::operator [] ( unsigned int id ) const
{
	B_ASSERT( id < _arraySize );
#ifdef _DEBUG
	B_ASSERT( _items[ id ].isUsed );
#endif
	return _items[ id ];
}
