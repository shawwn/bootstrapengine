//----------------------------------------------------------
// File:		UExprMgr.h
// Author:		Kevin Bray
// Created:		05-26-08
//
// Purpose:		A term to evaluate in an expression.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "BscExprLib.h"
#include "BscVarTable.h"
#include "BscArrayTable.h"

// std c++ includes
#include <vector>
#include <map>

// forward declarations.
class UExprTerm;
class UExprArray;

//**********************************************************
// class UExprMgr
//**********************************************************
class UExprMgr
{
public:
	UExprMgr();
	~UExprMgr();

	// attempts to parse an expression.
	URef< UExprTerm >		ParseExpr( const tstring& text, tstring& errors );

	// creates a new expression.
	URef< UExpr >			CreateExpression();

	// simple types.
	URef< UExprNumber >		CreateNumber( float value );

	// creates an array.
	URef< UExprArray >		CreateArray( const tstring& name, float indexScale, bool smooth, bool clamp );

	// simple types.
	URef< UExprArrayRef >	CreateArrayRef( URef< UExprArray > array, URef< UExprTerm > index );

	// expression callbacks.
	URef< UExprCallback >	CreateScriptCallback( const tstring& name, float ( *callback )() );
	URef< UExprCallback >	FindScriptCallback( const tstring& name ) const;
	const BscVarTable&		GetGlobalVarTable() const		{	return _varTable;		}

	// global arrays.
	URef< UExprArray >		CreateGlobalArray( const tstring& name, float indexScale, bool smooth, bool clamp );
	URef< UExprArray >		FindGlobalArray( const tstring& name ) const;
	const BscArrayTable&	GetGlobalArrayTable() const		{	return _arrayTable;		}

	// perform garbage collection.
	void					CollectGarbage();

private:
	typedef std::map< tstring, URef< UExprArray > > ArrayMap;
	typedef std::vector< URef< UExprTerm > > ExprVec;
	typedef std::vector< URef< UExprArray > > ArrayVec;

	// global variable table.
	BscArrayTable			_arrayTable;
	BscVarTable				_varTable;
	ExprVec					_expressions;
	ArrayVec				_arrays;
};
extern UExprMgr* gUExprMgr;