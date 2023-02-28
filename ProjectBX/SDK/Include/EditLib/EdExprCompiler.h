//----------------------------------------------------------
// File:		EdExprCompiler.h
// Author:		Kevin Bray
// Created:		07-28-07
//
// Purpose:		To provide a simple way to compile, decompile,
//				and classify expressions.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UExpr.h"

// forward declarations.
class UExprTerm;
class UExprCallback;
class UExprArrayRef;

//**********************************************************
// class EdExprCompiler
//**********************************************************
class EdExprCompiler
{
public:
	enum EEXPRCLASS
	{
		EC_CONSTANT,
		EC_LINEAR,
		EC_SINEWAVE,
		EC_SAWTOOTHWAVE,
		EC_SQUAREWAVE,
		EC_OTHER,
	};

	EdExprCompiler();
	~EdExprCompiler();

	// converts an expression to BScript.
	URef< UExprTerm >	Compile( const tstring& script, tstring& errors );
	void				Decompile( tstring& script, URef< UExprTerm > expr );

	// classifies an expression based on what it does.  Expressions
	// take the form of waveform[ A * _time + C ].  A and C are always
	// valid regardless of the expression (A = 1 and C = 0 for 'EC_OTHER'
	// expressions).
	EEXPRCLASS			Classify( float& A, float& C, URef< UExprTerm > expr );

private:
	// stack entry for expression evaluation.
	struct SStackEntry
	{
		int power;
		float value;
		UExpr::EMATHOP mathOp;
	};

	EEXPRCLASS			ClassifyExpr( float& A, float& C, UExpr* expr );
	bool				SetStackEntry( SStackEntry* stack, unsigned int idx, UExprTerm* term, UExpr::EMATHOP op );
	EEXPRCLASS			GetArrayClass( UExprArrayRef* arrayRef );
	bool				GetCallbackName( tstring& name, UExprCallback* callback );
	bool				IsTime( UExprCallback* callback );
};
