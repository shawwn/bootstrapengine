//----------------------------------------------------------
// File:		UConstRef.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		To track reference counts for a const pointer.  Deletes when necessary.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once
#include "URefCount.h"
#include "URef.h"

//**********************************************************
// class UConstRef
//**********************************************************
template< typename _T >
class UConstRef
{
public:
	UConstRef();
	UConstRef( const _T *ptr );
	UConstRef( const URef< _T >& ref );
	UConstRef( const UConstRef& ref );
	~UConstRef();

	UConstRef&		operator = ( const UConstRef& ref );
	operator const _T * () const;
	const _T *	operator -> () const;
	const _T&	operator * () const { return ( *_ptr ); }

private:
	const _T *	_ptr;
};

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::UConstRef()
: _ptr( 0 )
{

}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::UConstRef( const _T *ptr )
: _ptr( ptr )
{
	if ( ptr )
	{
		ptr->IncRef();
	}
}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::UConstRef( const URef< _T >& ref )
: _ptr( ( _T * )ref )
{
	if ( _ptr )
	{
		_ptr->IncRef();
	}
}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::UConstRef( const UConstRef& ref )
: _ptr( ref._ptr )
{
	if ( _ptr )
	{
		_ptr->IncRef();
	}
}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::~UConstRef()
{
	if ( _ptr )
	{
		if ( _ptr->DecRef() == 0 )
		{
			delete _ptr;
		}
	}
}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >&
UConstRef< _T >::operator = ( const UConstRef< _T >& ref )
{
	if ( _ptr == ref._ptr )
		return *this;

	if ( _ptr )
	{
		if ( _ptr->DecRef() == 0 )
		{
			delete _ptr;
		}
	}
	_ptr = ref._ptr;
	if ( _ptr )
	{
		_ptr->IncRef();
	}

	return *this;
}

//----------------------------------------------------------
template< typename _T >
UConstRef< _T >::operator const _T * () const
{
	return _ptr;
}

//----------------------------------------------------------
template< typename _T >
const _T *
UConstRef< _T >::operator -> () const
{
	return _ptr;
}
