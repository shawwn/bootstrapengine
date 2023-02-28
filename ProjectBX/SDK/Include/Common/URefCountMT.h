//----------------------------------------------------------
// File:		URefCountMT.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		Maintain a ref-count for smart pointers.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class URefCount
//**********************************************************
class URefCountMT
{
public:
	URefCountMT() : _ref( 0 ) { }
	~URefCountMT() { }

	int		GetRef()	const	{	return _ref;					}
	int		IncRef()	const	{	return LockIncrement( &_ref );	}
	int		DecRef()	const	{	return LockDecrement( &_ref );	}

private:
	mutable volatile int	_ref;
};
