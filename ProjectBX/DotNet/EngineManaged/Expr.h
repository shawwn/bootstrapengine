//----------------------------------------------------------
// File:		Expr.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To evaluate mathematical expressions.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "UExprTerm.h"

namespace Bootstrap
{
    //**********************************************************
    // ref class Expr
    //**********************************************************
	public ref class Expr
	{
	internal:
		Expr( UExprTerm* exprTerm );

        // native expression term.
		UExprTerm*		                    _exprTerm;

	public:
		Expr( System::String^ exprScript );
		Expr( float value );
		~Expr();
		!Expr();

		// returns true if the expression is valid.
		property bool                       Valid
		{
			bool                                get();
		}

        // returns the value of the expression.
		property float                      Value
		{
			float                               get()               {   return _exprTerm->GetValue();   }
		}

		property System::String^            Errors
		{
			System::String^                     get()			    {	return _errors;		            }
		}

		System::String^                     Decompile();

	private:
		System::String^	                    _errors;
	};
}
