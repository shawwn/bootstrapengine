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
// class UExprNumber
//**********************************************************
class UExprNumber : public UExprTerm
{
	friend class UExprMgr;

	UExprNumber( float value ) : _value( value ) { }
public:
	~UExprNumber() { }

	float			GetValue() const		{	return _value;		}
	bool			IsDynamic() const		{	return false;		}
	ETYPE			GetType() const			{	return ET_NUMBER;	}

private:
	float			_value;
};
