//----------------------------------------------------------
// File:		URef.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		To track reference counts.  Deletes when necessary.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"

//**********************************************************
// class URefPtr
//**********************************************************
template< typename _T >
class URef
{
public:
	URef();
	URef( _T *ptr );
	URef( const URef& ref );
	template< typename _T2 > URef( const URef<_T2>& ref ) : _ptr( ref._ptr )
	{
		if ( _ptr )
		{
			_ptr->IncRef();
		}
	}
	template< typename _T2 > friend class URef;

	~URef();

	URef&		operator = ( const URef& ref );
	operator _T * () const;
	_T *		operator -> () const;
	const _T&	operator * () const { return ( *_ptr ); }
	_T&			operator * () {	return ( *_ptr ); }

private:
	_T *		_ptr;
};

//----------------------------------------------------------
template< typename _T >
URef< _T >::URef()
: _ptr( 0 )
{

}

//----------------------------------------------------------
template< typename _T >
URef< _T >::URef( _T *ptr )
: _ptr( ptr )
{
	if ( ptr )
	{
		ptr->IncRef();
	}
}

//----------------------------------------------------------
template< typename _T >
URef< _T >::URef( const URef& ref )
: _ptr( ref._ptr )
{
	if ( _ptr )
	{
		_ptr->IncRef();
	}
}

//----------------------------------------------------------
template< typename _T >
URef< _T >::~URef()
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
URef< _T >&
URef< _T >::operator = ( const URef< _T >& ref )
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
URef< _T >::operator _T * () const
{
	return _ptr;
}

//----------------------------------------------------------
template< typename _T >
_T *
URef< _T >::operator -> () const
{
	return _ptr;
}
