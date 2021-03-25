//----------------------------------------------------------
// File:		EdExprCompiler.cpp
// Author:		Kevin Bray
// Created:		07-28-07
//
// Purpose:		To present a box for viewing material stages.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdExprCompiler.h"

// project includes.
#include "EdConfig.h"
#include "UExprTerm.h"
#include "UExprMgr.h"
#include "UExpr.h"
#include "UExprNumber.h"
#include "UExprCallback.h"
#include "UExprArrayRef.h"
#include "UExprArray.h"
#include "BscVarTable.h"
#include "BscArrayTable.h"
#include "BscLexer.h"
#include "BscExprLib.h"
#include "RRsrcMgr.h"

#pragma warning ( disable : 4996 )

//**********************************************************
// class EdExprCompiler
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdExprCompiler::EdExprCompiler()
{

}

//----------------------------------------------------------
EdExprCompiler::~EdExprCompiler()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< UExprTerm >
EdExprCompiler::Compile( const tstring& script, tstring& errors )
{
	// make sure the resource manager is available since it owns
	// script callbacks.
	B_ASSERT( gRRsrcMgr != 0 );

	// get the text from the text box.
	BscArrayTable arrayTable( gUExprMgr->GetGlobalArrayTable() );

	// create a lexer and Lex.  TODO: Post errors!
	BscLexer lexer;
	if ( !lexer.Lex( script.c_str() ) )
	{
		errors = "Unable to parse expression.";
		return 0;
	}

	// get the script global variable table from the resource
	// manager.
	const BscVarTable& varTable = gUExprMgr->GetGlobalVarTable();

	// get the token stream so that we can begin parsing the
	// expression.
	const SBscToken* tokens = lexer.GetTokenStream();
	if ( !BscIsExpr( tokens, arrayTable, varTable ) )
	{
		errors = "Error in expression.";
		return 0;
	}

	// parse the expression.
	URef< UExprTerm > expr;
	BscParseExpr( &expr, tokens, arrayTable, varTable );
	if ( !expr )
	{
		// this shouldn't happen!  If we got here, it means that
		// BscIsExpr() returned true, but then the expression
		// couldn't be parsed.
		B_ASSERT( false );
		errors = "Unknown parse error.";
		return 0;
	}

	// return the expression.
	return expr;
}

//----------------------------------------------------------
void
EdExprCompiler::Decompile( tstring& script, URef< UExprTerm > term )
{
	// make sure the resource manager is available since it owns
	// script callbacks.
	B_ASSERT( gRRsrcMgr != 0 );

	// decompile the script (expr -> text).
	tstring temp;
	UExprTerm::ETYPE type = term->GetType();
	if ( type == UExprTerm::ET_NUMBER )
	{
		// simply output the number.
		char buffer[ 50 ];
		sprintf( buffer, "%.3f", term->GetValue() );
		script += buffer;
	}
	else if ( type == UExprTerm::ET_CALLBACK )
	{
		if ( GetCallbackName( temp, ( UExprCallback* )( UExprTerm* )term ) )
			script += temp;
	}
	else if ( type == UExprTerm::ET_ARRAYREF )
	{
		// get the array reference.
		UExprArrayRef* ref = ( UExprArrayRef* )( UExprTerm* )term;
		script += ref->GetArray()->GetName();

		// recurse the index.
		script += "[ ";
		Decompile( script, ref->GetIndex() );
		script += " ]";
	}
	else if ( type == UExprTerm::ET_EXPR )
	{
		// get the expression.
		UExpr* expr = ( UExpr* )( UExprTerm* )term;

		// write the opening parenthesis.
		script += "( ";

		// if we have an expression, simply add the operation between
		// elements.
		unsigned int termCount = expr->GetTermCount();
		for ( unsigned int i = 0; i < termCount; ++i )
		{
			// write out the operation if necessary.
			if ( i > 0 )
			{
				UExpr::EMATHOP op = expr->GetTermMathOp( i );
				switch( op )
				{
				case UExpr::EO_ADD:
					script += " + ";
					break;

				case UExpr::EO_SUBTRACT:
					script += " - ";
					break;

				case UExpr::EO_MULTIPLY:
					script += " * ";
					break;

				case UExpr::EO_DIVIDE:
					script += " / ";
					break;

				case UExpr::EO_MODULUS:
					script += " % ";
					break;

				case UExpr::EO_CMPLESS:
					script += " < ";
					break;

				case UExpr::EO_CMPGREATER:
					script += " > ";
					break;

				case UExpr::EO_CMPLESSEQUAL:
					script += " <= ";
					break;

				case UExpr::EO_CMPGREATEREQUAL:
					script += " >= ";
					break;

				case UExpr::EO_CMPNOTEQUAL:
					script += " != ";
					break;

				case UExpr::EO_CMPEQUAL:
					script += " == ";
					break;

				case UExpr::EO_CMPAND:
					script += " && ";
					break;

				case UExpr::EO_CMPOR:
					script += " || ";
					break;

				default:
					script += " + ";
					B_ASSERT( false );	// this should be impossible!
				}
			}

			// recurse on the current term.
			Decompile( script, expr->GetTerm( i ) );
		}

		// write the closing parenthesis.
		script += " )";
	}
}

//----------------------------------------------------------
EdExprCompiler::EEXPRCLASS
EdExprCompiler::Classify( float& A, float& C, URef< UExprTerm > expr )
{
	// make sure the resource manager is available since it owns
	// script callbacks.
	B_ASSERT( gRRsrcMgr != 0 );

	// temporary string.
	tstring temp;

	// identify the expression type.
	if ( !expr->IsDynamic() )
	{
		// do we have a constant?
		A = 0.0f;
		C = expr->GetValue();
		return EC_CONSTANT;
	}
	else if ( expr->GetType() == UExprTerm::ET_CALLBACK )
	{
		A = 1.0f;
		C = 0.0f;
		if ( !GetCallbackName( temp, ( UExprCallback* )( UExprTerm* )expr ) )
			return EC_OTHER;

		// if we're not looking at the time callback, then we can't
		// be sure of what's going on in this expression.
		if ( stricmp( temp.c_str(), ED_SCRIPTCALLBACK_TIME ) != 0 )
			return EC_OTHER;

		// if we got here, then we've got a linear ramp:
		// (1.0 * _time + 0.0).
		return EC_LINEAR;
	}
	else if ( expr->GetType() == UExprTerm::ET_ARRAYREF )
	{
		// get the array class.
		UExprArrayRef* arrayRef = ( UExprArrayRef* )( UExprTerm* )expr;
		EEXPRCLASS arrayClass = GetArrayClass( arrayRef );

		// is the array something we recognize?  If not, simply return
		// EC_OTHER.
		if ( arrayClass == EC_OTHER )
		{
			A = 1.0f;
			C = 0.0f;
			return EC_OTHER;
		}

		// get the array index.
		URef< UExprTerm > index = arrayRef->GetIndex();

		// recurse on the array's index.
		EEXPRCLASS indexClass = Classify( A, C, index );

		// note that we only deal with linear or constant indices.
		if ( indexClass != EC_CONSTANT && indexClass != EC_LINEAR )
		{
			A = 1.0f;
			C = 0.0f;
			return EC_OTHER;
		}

		// return the type of array we have as a lookup.
		return arrayClass;
	}
	else if ( expr->GetType() == UExprTerm::ET_EXPR )
	{
		// look for a constant and a callback.
		UExpr* complexExpr = ( UExpr* )( UExprTerm* )expr;

		// now try to classify the expression.
		return ClassifyExpr( A, C, complexExpr );
	}

	return EC_OTHER;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
EdExprCompiler::EEXPRCLASS
EdExprCompiler::ClassifyExpr( float& A, float& C, UExpr* expr )
{
	// initialize the A and C terms to defaults for an error condition.
	A = 0.0f;
	C = 1.0f;

	// we'll be modifying these until we return successfully.  At which
	// point, we update A and C.
	float curC = 0.0f;
	float curA = 0.0f;

	// determine the 
	unsigned int termCount = expr->GetTermCount();
	B_ASSERT( termCount > 0 );

	// is there only one term?  If so, call Classify() on that
	// term.
	if ( termCount == 1 )
		return Classify( A, C, expr->GetTerm( 0 ) );

	// allocate the stack.
	unsigned int stackTop = 0;
	SStackEntry* stack = ( SStackEntry* )alloca( termCount * sizeof( SStackEntry ) );

	// initialize the first stack entry.
	if ( !SetStackEntry( stack, 0, expr->GetTerm( 0 ), UExpr::EO_ADD ) )
		return EC_OTHER;

	// reduce multiplications.
	for ( unsigned int i = 1; i < termCount; ++i )
	{
		// get the current term and it's math op.
		UExprTerm* curTerm = expr->GetTerm( i );
		UExpr::EMATHOP curOp = expr->GetTermMathOp( i );

		// check the current operation.
		if ( curOp == UExpr::EO_MULTIPLY )
		{
			// are we the time callback?
			if ( curTerm->GetType() == UExprTerm::ET_CALLBACK )
			{
				// if we couldn't get the name, simply fail.
				if ( !IsTime( ( UExprCallback* )curTerm ) )
					return EC_OTHER;

				// increment the power.
				stack[ stackTop ].power += 1;

				// note that we only deal with linear equations!
				if ( stack[ stackTop ].power > 1 )
					return EC_OTHER;
			}
			else if ( !curTerm->IsDynamic() )
			{
				// if the current term is simply a number, multiply
				// against the top of the stack.
				stack[ stackTop ].value *= curTerm->GetValue();
			}
			else
				return EC_OTHER;
		}
		else
		{
			// push onto the stack.
			++stackTop;

			// push onto the stack.
			if ( !SetStackEntry( stack, stackTop, curTerm, curOp ) )
				return EC_OTHER;
		}
	}

	// now we should be left with a stack full of additions.
	++stackTop;
	while ( stackTop-- > 0 )
	{
		// accumulate terms.
		SStackEntry* curEntry = stack + stackTop;
		if ( curEntry->power > 0 )
			curA += stack[ stackTop ].value;
		else
			curC += stack[ stackTop ].value;
	}

	// set the output values.
	A = curA;
	C = curC;

	// all done!
	if ( Abs( A ) < 0.00001f )
		return EC_CONSTANT;

	// if A is 0, simply return EC_CONSTANT.
	return EC_LINEAR;
}

//----------------------------------------------------------
bool
EdExprCompiler::SetStackEntry( SStackEntry* stack, unsigned int idx, UExprTerm* term, UExpr::EMATHOP op )
{
	// make sure the operation is supported.
	if ( op != UExpr::EO_ADD && op != UExpr::EO_SUBTRACT )
		return false;

	// note that we convert subtraction to addition by negating
	// the value.
	stack[ idx ].mathOp = UExpr::EO_ADD;

	// store the term.
	if ( term->GetType() == UExprTerm::ET_CALLBACK )
	{
		if ( IsTime( ( UExprCallback* )term ) )
		{
			stack[ idx ].value = 1.0f;
			stack[ idx ].power = 1;
		}
	}
	else if ( !term->IsDynamic() )
	{
		stack[ idx ].value = term->GetValue();
		stack[ idx ].power = 0;
	}
	else
	{
		// unsupported term!
		return false;
	}

	// do we need to negate the value?
	if ( op == UExpr::EO_SUBTRACT )
		stack[ idx ].value = -stack[ idx ].value;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
EdExprCompiler::EEXPRCLASS
EdExprCompiler::GetArrayClass( UExprArrayRef* arrayRef )
{
	// get the array name.
	const tstring& arrayName = arrayRef->GetArray()->GetName();

	// cosine wave?
	if ( stricmp( arrayName.c_str(), ED_SCRIPTARRAY_SINE ) == 0 )
		return EC_SINEWAVE;

	// sawtooth wave?
	if ( stricmp( arrayName.c_str(), ED_SCRIPTARRAY_SAWTOOTH ) == 0 )
		return EC_SAWTOOTHWAVE;

	// square wave?
	if ( stricmp( arrayName.c_str(), ED_SCRIPTARRAY_SQUARE ) == 0 )
		return EC_SQUAREWAVE;

	// unknown.
	return EC_OTHER;
}

//----------------------------------------------------------
bool
EdExprCompiler::GetCallbackName( tstring& name, UExprCallback* callback )
{
	// return the callback's name.  Return true on success, false on failure.
	const BscVarTable& varTable = gUExprMgr->GetGlobalVarTable();
	return varTable.GetVarCallbackName( name, callback );
}

//----------------------------------------------------------
bool
EdExprCompiler::IsTime( UExprCallback* callback )
{
	// if we couldn't get the name, simply fail.
	tstring temp;
	if ( !GetCallbackName( temp, callback ) )
		return false;

	// make sure that we have the time here!
	return stricmp( temp.c_str(), ED_SCRIPTCALLBACK_TIME ) == 0;
}
