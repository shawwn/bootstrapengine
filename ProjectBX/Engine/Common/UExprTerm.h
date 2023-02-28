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
#include "URefCount.h"

//**********************************************************
// class UExprTerm
//**********************************************************
class UExprTerm : public URefCount
{
public:
	enum ETYPE
	{
		ET_EXPR,
		ET_NUMBER,
		ET_CALLBACK,
		ET_ARRAYREF,
	};

	UExprTerm() { }
	virtual ~UExprTerm() { }

	virtual float	GetValue() const = 0;
	virtual bool	IsDynamic() const = 0;
	virtual ETYPE	GetType() const = 0;
};
