//----------------------------------------------------------
// File:		URefCount.h
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
class URefCount
{
public:
	URefCount() : _ref( 0 ) { }
	~URefCount() { }

	int		GetRef()	const	{	return _ref;	}
	int		IncRef()	const	{	return ++_ref;	}
	int		DecRef()	const	{	return --_ref;	}

private:
	mutable int		_ref;
};
