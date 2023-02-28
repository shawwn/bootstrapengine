//----------------------------------------------------------
// File:        ULRUHash.h
// Author:      Kevin Bray
// Created:     11-22-2008
//
// Purpose:     To cache tiles in system memory.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "ULRUList.h"

//**********************************************************
// class ULRUHash
//**********************************************************
template< typename _Key, class _T, typename _KeyHash = unsigned int >
class ULRUHash
{
public:
    ULRUHash( unsigned int sizePow2 );
    ~ULRUHash();

    // returns the total number of items that can be contained in the
    // hash table.
    unsigned int            GetSize() const         {   return _size;       }

    // returns the total number of items currently in the hash table.
    unsigned int            GetElemCount() const    {   return _count;      }

    // adds an item and returns it's handle.  'removed' is set if an
    // item needs to be removed to make room for the current item.
    // Otherwise, it's value is unchanged.
    void*                   Insert( _KeyHash keyHash, const _Key& key, const _T& item );
    void*                   Insert( _T& removed, _KeyHash keyHash, const _Key& key, const _T& item );

    // removes an item from the hash table.
    void*                   Remove( void* item );

    // removes all items from the hash table.
    void                    Clear();

    // finds items in the hash table with the keyHash specified.
    void*                   FindFirst( _KeyHash keyHash ) const;
    void*                   FindNext( void* handle, _KeyHash keyHash ) const;

    // finds the first occurance of a specific item in the cache.
    void*                   Find( _KeyHash keyHash, const _Key& key ) const;

    // marks an item as used.
    void                    MarkUsed( void* handle );

    // returns the key associated with a handle.
    const _Key&             GetKey( void* handle ) const;

    // returns the key hash associated with a handle.
    _KeyHash                GetKeyHash( void* handle ) const;

    // returns the item associated with a handle.
    _T&                     GetValue( void* handle );
    const _T&               GetValue( void* handle ) const;
    void                    SetValue( void* handle, const _T& item );

    // returns the oldest item in the hash.
    void*                   GetOldest() const;
    void*                   GetNewest() const;

    void*                   GetOlder( void* handle ) const;
    void*                   GetNewer( void* handle ) const;

private:
    // linked-list item found in each bin.
    struct SBinItem
    {
        _T item;
        _Key key;
        _KeyHash keyHash;
        void* age;
        SBinItem* next;
    };

    // hash table member variables.
    SBinItem**              _table;
    ULRUList< SBinItem* >   _lruList;
    unsigned int            _count;
    unsigned int            _size;
    unsigned int            _mask;

    // free list.
    SBinItem*               _linkFree;
    SBinItem*               _linkMem;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
ULRUHash< _Key, _T, _KeyHash >::ULRUHash( unsigned int size )
: _table( new SBinItem* [ 2 * size ] )
, _lruList( size )
, _count( 0 )
, _size( size )
, _mask( 2 * size - 1 )
{
    // verify that the size is a power of two.
    B_ASSERT( IsPow2( size ) );

    // clear the table.
    MemSet( _table, 0, 2 * size * sizeof( SBinItem* ) );

    // allocate links for the bins.
    _linkFree = _linkMem = new SBinItem[ size ];
    _linkFree[ size - 1 ].next = 0;
    for ( unsigned int i = 1; i < size; ++i )
        _linkFree[ i - 1 ].next = _linkFree + i;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
ULRUHash< _Key, _T, _KeyHash >::~ULRUHash()
{
    // free memory.
    delete[] _linkMem;
    delete[] _table;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::Insert( _KeyHash keyHash, const _Key& key, const _T& item )
{
    // make sure there is enough room in the table for the item.
    if ( _count == _size )
    {
        // get the oldest item and remove it.
        SBinItem* removeItem = _lruList.Get( _lruList.GetLRUItem() );
        Remove( removeItem );
    }

    // determine what bin the item resides.
    unsigned int bin = keyHash & _mask;

    // get a link to insert into the table.
    SBinItem* newLink = _linkFree;
    _linkFree = _linkFree->next;

    // fill out the new link.
    newLink->item = item;
    newLink->key = key;
    newLink->keyHash = keyHash;
    newLink->age = _lruList.Add( newLink );
    newLink->next = _table[ bin ];

    // add the link to the table.
    _table[ bin ] = newLink;

    // increment the count.
    ++_count;

    // return the handle.
    return newLink;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::Insert( _T& removed, _KeyHash keyHash, const _Key& key, const _T& item )
{
    // make sure there is enough room in the table for the item.
    if ( _count == _size )
    {
        // get the oldest item and remove it.
        SBinItem* removeItem = _lruList.Get( _lruList.GetLRUItem() );
        removed = removeItem->item;
        Remove( removeItem );
    }

    // determine what bin the item resides.
    unsigned int bin = keyHash & _mask;

    // get a link to insert into the table.
    SBinItem* newLink = _linkFree;
    _linkFree = _linkFree->next;

    // fill out the new link.
    newLink->item = item;
    newLink->key = key;
    newLink->keyHash = keyHash;
    newLink->age = _lruList.Add( newLink );
    newLink->next = _table[ bin ];

    // add the link to the table.
    _table[ bin ] = newLink;

    // increment the count.
    ++_count;

    // return the handle.
    return newLink;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::Remove( void* handle )
{
    // make sure the handle passed in is valid.
    B_ASSERT( handle != 0 );

    // get the current item we're working on.
    SBinItem* item = ( SBinItem* )handle;
    
    // find the bin that contains the item.
    unsigned int bin = item->keyHash & _mask;

    // remove the item in the bin.
    SBinItem** cur = _table + bin;
    while ( *cur != 0 )
    {
        // check to see if we found the item we need to remove.
        if ( *cur == item )
        {
            // remove the item from the list.
            *cur = item->next;
            break;
        }

        // get a pointer to the next item.
        cur = &( ( *cur )->next );
    }

    // remove the item from the LRU list.
    void* newer = _lruList.Remove( item->age );
    if ( newer )
        newer = _lruList.Get( newer );

    // add the item back into the free list.
    item->next = _linkFree;
    _linkFree = item;

    // decrement the item count.
    --_count;

    // return the newer item.
    return newer;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void
ULRUHash< _Key, _T, _KeyHash >::Clear()
{
    // clear the LRU hash the slow way.  If we need to optimize this
    // further, we can.
    for ( unsigned int i = 0; i <= _mask; ++i )
    {
        while ( _table[ i ] )
            Remove( _table[ i ] );
    }
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::FindFirst( _KeyHash keyHash ) const
{
    // get the bin that the keyHash is placed in.
    unsigned int bin = keyHash & _mask;

    // check to see if there are any items in the bin specified.
    SBinItem* item = _table[ bin ];
    if ( item == 0 )
        return 0;

    // check to see if the first item in the bin is the item we want.
    if ( item->keyHash == keyHash )
        return item;

    // return the first item in the list that has the keyHash specified
    // or null if the item can't be found.
    return FindNext( item, keyHash );
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::FindNext( void* handle, _KeyHash keyHash ) const
{
    // get the item we're working on.
    SBinItem* item = ( SBinItem* )handle;
    item = item->next;
    while ( item != 0 )
    {
        // check to see if the current item is the one we want.
        if ( item->keyHash == keyHash )
            return item;

        // get the next item.
        item = item->next;
    }

    // if we got here, then the item could not be found.  Therefor,
    // simply return null.
    return 0;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::Find( _KeyHash keyHash, const _Key& key ) const
{
    // get the first item with the keyHash specified.
    void* item = FindFirst( keyHash );
    while ( item != 0 )
    {
        // if the current item matches the value requested, return
        // it's handle.
        if ( ( ( SBinItem* )item )->key == key )
            return item;

        // get the next item.
        item = FindNext( item, keyHash );
    }

    // if we got here, then the value specified could not be found.
    // Therefore, simply return null.
    return 0;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void
ULRUHash< _Key, _T, _KeyHash >::MarkUsed( void* handle )
{
    // mark the age entry as used.
    _lruList.MarkUsed( ( ( SBinItem* )handle )->age );
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
const _Key&
ULRUHash< _Key, _T, _KeyHash >::GetKey( void* handle ) const
{
    // get the bin-item and return it's key.
    return ( ( SBinItem* )handle )->key;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
_KeyHash
ULRUHash< _Key, _T, _KeyHash >::GetKeyHash( void* handle ) const
{
    // get the bin-item and return it's key.
    return ( ( SBinItem* )handle )->keyHash;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
_T&
ULRUHash< _Key, _T, _KeyHash >::GetValue( void* handle )
{
    // get the bin-item and return the item it represents.
    return ( ( SBinItem* )handle )->item;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
const _T&
ULRUHash< _Key, _T, _KeyHash >::GetValue( void* handle ) const
{
    // get the bin-item and return the item it represents.
    return ( ( SBinItem* )handle )->item;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void
ULRUHash< _Key, _T, _KeyHash >::SetValue( void* handle, const _T& item )
{
    // get the bin-item and return the item it represents.
    ( ( SBinItem* )handle )->item = item;
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::GetOldest() const
{
    if ( _count == 0 )
        return 0;

    // get the oldest item.
    return _lruList.Get( _lruList.GetLRUItem() );
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::GetNewest() const
{
    if ( _count == 0 )
        return 0;

    // get the newest item.
    return _lruList.Get( _lruList.GetMRUItem() );
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::GetOlder( void* item ) const
{
    // try to get an older item from the LRU list.
    void* older = _lruList.GetOlder( ( ( SBinItem* )item )->age );
    if ( !older )
        return 0;

    // return the older item if possible.
    return _lruList.Get( older );
}

//----------------------------------------------------------
template < typename _Key, class _T, typename _KeyHash >
void*
ULRUHash< _Key, _T, _KeyHash >::GetNewer( void* item ) const
{
    // try to get an older item from the LRU list.
    void* newer = _lruList.GetNewer( ( ( SBinItem* )item )->age );
    if ( !newer )
        return 0;

    // return the older item if possible.
    return _lruList.Get( newer );
}
