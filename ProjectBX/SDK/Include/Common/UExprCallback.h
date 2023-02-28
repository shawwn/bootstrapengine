//----------------------------------------------------------
// File:		UExprTerm.h
// Author:		Kevin Bray
// Created:		07-05-05
//
// Purpose:		A term to evaluate in an expression.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UExprTerm.h"

//**********************************************************
// class UExprCallback
//**********************************************************
class UExprCallback : public UExprTerm
{
public:
	typedef float ( *Callback )();
private:
	friend class UExprMgr;
	UExprCallback( Callback callback ) : _callback( callback ) { B_ASSERT( callback != 0 );	}

public:
	~UExprCallback() { }

	float			GetValue() const		{	return _callback();	}
	bool			IsDynamic() const		{	return true;		}
	ETYPE			GetType() const			{	return ET_CALLBACK;	}

	Callback		GetCallback() const		{	return _callback;	}

private:
	Callback		_callback;
};


