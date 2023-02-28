//----------------------------------------------------------
// File:        ULRUList.h
// Author:      Kevin Bray
// Created:     11-24-2008
//
// Purpose:     To manage an LRU list.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

//**********************************************************
// class ULRUList
//**********************************************************
template< class _T >
class ULRUList
{
public:
    ULRUList( unsigned int size );
    ~ULRUList();

    // adds an item to the list and returns a handle to it.
    void*       Add( const _T& value );
    void*       Remove( void* key );
    const _T&   Get( void* key ) const;
    void        Set( void* key, const _T& value );

    // marks an item as used.
    void        MarkUsed( void* key );

    // returns the key of the least-recently used item.
    void*       GetLRUItem() const;
    void*       GetMRUItem() const;

    // item traversal.  Returns null if the item is the newest or the
    // oldest.
    void*       GetNewer( void* key ) const;
    void*       GetOlder( void* key ) const;

private:
    struct SAgeItem
    {
        _T value;
        SAgeItem* older;
        SAgeItem* newer;
    };

    // items.
    SAgeItem*       _newest;
    SAgeItem*       _oldest;

    // free list.
    SAgeItem*       _free;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < class _T >
ULRUList< _T >::ULRUList( unsigned int size )
: _newest( 0 )
, _oldest( 0 )
, _free( 0 )
{
    // allocate the free list.
    _free = new SAgeItem[ size ];

    // set the pointers in the free list.
    _free[ size - 1 ].newer = 0;
    for ( unsigned int i = 1; i < size; ++i )
        _free[ i - 1 ].newer = _free + i;
}

//----------------------------------------------------------
template < class _T >
ULRUList< _T >::~ULRUList()
{
    // move everything to the free list.
    while ( _oldest )
        Remove( _oldest );

    // take the smallest pointer value in the free list and free it.
    SAgeItem* start = _free;
    while ( _free != 0 )
    {
        // take the smaller starting address.
        if ( _free < start )
            start = _free;

        // next.
        _free = _free->newer;
    }

    // free the memory allocated for the items.
    delete[] start;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::Add( const _T& value )
{
    // if the list is full, remove the oldest item.
    if ( !_free )
        Remove( _oldest );

    // set the item to the front of the list.
    _free->older = _newest;
    if ( _newest )
        _newest->newer = _free;
    else
        _oldest = _free;

    // update the pointer to the newest item.
    _newest = _free;

    // update the free pointer.
    _free = _free->newer;

    // set the newer pointer on the newest item to null and store the
    // value.
    _newest->newer = 0;
    _newest->value = value;

    // return the item we just added.
    return _newest;
}

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::Remove( void* key )
{
    // get the item from the key.
    SAgeItem* item = ( SAgeItem* )key;

    // get the newer item so that we can return it's handle.
    void* newerItem = item->newer;

    // remove the item from the list.
    if ( item->older )
        item->older->newer = item->newer;
    if ( item->newer )
        item->newer->older = item->older;

    // update the _newest and _oldest pointers if necessary.
    if ( _newest == item )
        _newest = item->older;
    if ( _oldest == item )
        _oldest = item->newer;

    // place the item in the free list.
    item->newer = _free;
    _free = item;

    // return the new item than the item we just removed.
    return newerItem;
}

//----------------------------------------------------------
template < class _T >
const _T&
ULRUList< _T >::Get( void* key ) const
{
    // make sure the key passed in is valid.
    B_ASSERT( key != 0 );

    // return the value associated with the key.
    return ( ( SAgeItem* )key )->value;
}

//----------------------------------------------------------
template < class _T >
void
ULRUList< _T >::Set( void* key, const _T& value )
{
    // make sure the key passed in is valid.
    B_ASSERT( key != 0 );

    // set the value.
    ( ( SAgeItem* )key )->value = value;
}

//----------------------------------------------------------
template < class _T >
void
ULRUList< _T >::MarkUsed( void* key )
{
    // make sure the key passed in is valid.
    B_ASSERT( key != 0 );

    // get the current item.
    SAgeItem* item = ( SAgeItem* )key;
    if ( _newest == item )
        return;

    // remove the item from the list.
    if ( item->newer )
        item->newer->older = item->older;
    if ( item->older )
        item->older->newer = item->newer;
    else
        _oldest = item->newer;

    // set the item to the front of the list.
    item->older = _newest;
    item->newer = 0;
    _newest->newer = item;
    _newest = item;
}

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::GetLRUItem() const
{
    return ( void* )_oldest;
}

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::GetMRUItem() const
{
    return ( void* )_newest;
}

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::GetNewer( void* key ) const
{
    return ( ( SAgeItem* )key )->newer;
}

//----------------------------------------------------------
template < class _T >
void*
ULRUList< _T >::GetOlder( void* key ) const
{
    return ( ( SAgeItem* )key )->older;
}
