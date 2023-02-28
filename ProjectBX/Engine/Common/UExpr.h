//----------------------------------------------------------
// File:		UExpr.h
// Author:		Kevin Bray
// Created:		07-05-05
//
// Purpose:		A class evaluating mathematical expressions.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"
#include "UExprTerm.h"

// std c++ includes.
#include <vector>

//**********************************************************
// class UExpr
//**********************************************************
class UExpr : public UExprTerm
{
	friend class UExprMgr;

	UExpr();
public:
	enum EMATHOP {
		EO_ADD,
		EO_SUBTRACT,
		EO_MULTIPLY,
		EO_DIVIDE,
		EO_MODULUS,
		EO_CMPLESS,
		EO_CMPGREATER,
		EO_CMPLESSEQUAL,
		EO_CMPGREATEREQUAL,
		EO_CMPNOTEQUAL,
		EO_CMPEQUAL,
		EO_CMPAND,
		EO_CMPOR,
	};

	~UExpr();

	// adds terms to the 
	void				AddTerm( EMATHOP mathOp, URef< UExprTerm > term );

	unsigned int		GetTermCount() const	{	return ( unsigned int )_terms.size();	}
	URef< UExprTerm >	GetTerm( unsigned int idx ) const;
	EMATHOP				GetTermMathOp( unsigned int idx ) const;

	float				GetValue() const;
	bool				IsDynamic() const		{	return _dynamic;						}
	ETYPE				GetType() const			{	return ET_EXPR;							}

private:
	struct STerm
	{
		STerm( EMATHOP mathOp, URef< UExprTerm > term ) : term( term ), mathOp( mathOp ) { }
		~STerm() { }

		URef< UExprTerm > term;
		EMATHOP mathOp;
	};

	typedef std::vector< STerm > TermVec;

	TermVec				_terms;
	bool				_dynamic;
};
