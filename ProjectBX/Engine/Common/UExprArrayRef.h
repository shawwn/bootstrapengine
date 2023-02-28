//----------------------------------------------------------
// File:		UExprArrayRef.h
// Author:		Kevin Bray
// Created:		07-05-05
//
// Purpose:		To dereference an array.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UExprTerm.h"

// std c++ includes.
#include <vector>

// forward declarations.
class UExprArray;

//**********************************************************
// class UExprArrayRef
//**********************************************************
class UExprArrayRef : public UExprTerm
{
	friend class UExprMgr;

	UExprArrayRef( URef< UExprArray > array, URef< UExprTerm > index );
public:
	~UExprArrayRef();

	float				GetValue() const;
	bool				IsDynamic() const;
	ETYPE				GetType() const			{	return ET_ARRAYREF;	}

	URef< UExprArray >	GetArray() const		{	return _array;		}
	URef< UExprTerm >	GetIndex() const		{	return _index;		}

private:
	URef< UExprArray >	_array;
	URef< UExprTerm >	_index;
	bool				_dynamic;
};

