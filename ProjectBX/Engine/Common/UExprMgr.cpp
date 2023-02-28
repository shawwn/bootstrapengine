//----------------------------------------------------------
// File:		UExprMgr.cpp
// Author:		Kevin Bray
// Created:		05-26-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UExprMgr.h"

// project includes.
#include "UExprTerm.h"
#include "UExpr.h"
#include "UExprNumber.h"
#include "UExprArray.h"
#include "UExprArrayRef.h"
#include "UExprCallback.h"
#include "BscLexer.h"

// singleton pointer.
UExprMgr* gUExprMgr = 0;

//**********************************************************
// class UExprMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UExprMgr::UExprMgr()
{
	B_ASSERT( gUExprMgr == 0 );
	gUExprMgr = this;
}

//----------------------------------------------------------
UExprMgr::~UExprMgr()
{

}


//==========================================================
// public method
//==========================================================

//----------------------------------------------------------
URef< UExprTerm >
UExprMgr::ParseExpr( const tstring& text, tstring& errors )
{
	// tokenize the text.
	BscLexer lexer;
	if ( !lexer.Lex( text.c_str() ) )
	{
		// get the error list.
		const BscLexer::ErrorVector& lexErrors = lexer.GetErrorVec();
		BscLexer::ErrorVector::const_iterator iter = lexErrors.begin();
		for ( ; iter != lexErrors.end(); ++iter )
		{
			errors += "[Lexer Error] ";
			errors += ( *iter );
			errors += '\n';
		}

		// return null to indicate failure.
		return 0;
	}

	// verify that we have an expression.
	if ( !BscIsExpr( lexer.GetTokenStream(), _arrayTable, _varTable ) )
	{
		// store the error and return null to indicate failure.
		errors = "Unable to parse expression!";
		return 0;
	}

	// parse the expression.
	URef< UExprTerm > expr;
	BscParseExpr( &expr, lexer.GetTokenStream(), _arrayTable, _varTable );

	// simply return the expression.  Note that we don't need to 
	// store it because the parser has to create everything using
	// the CreateX() functions in this class.  That means that
	// everything is registered at that point.
	return expr;
}

//----------------------------------------------------------
URef< UExpr >
UExprMgr::CreateExpression()
{
	// create a new expression, register it, and return it.
	UExpr* expr = new UExpr;
	_expressions.push_back( expr );
	return expr;
}

//----------------------------------------------------------
URef< UExprNumber >
UExprMgr::CreateNumber( float value )
{
	UExprNumber* number = new UExprNumber( value );
	_expressions.push_back( number );
	return number;
}

//----------------------------------------------------------
URef< UExprArray >
UExprMgr::CreateArray( const tstring& name, float indexScale, bool smooth, bool clamp )
{
	UExprArray* array = new UExprArray( name, indexScale, smooth, clamp );
	_arrays.push_back( array );
	return array;
}

//----------------------------------------------------------
URef< UExprArrayRef >
UExprMgr::CreateArrayRef( URef< UExprArray > array, URef< UExprTerm > index )
{
	UExprArrayRef* arrayRef = new UExprArrayRef( array, index );
	_expressions.push_back( arrayRef );
	return arrayRef;
}

//----------------------------------------------------------
URef< UExprCallback >
UExprMgr::CreateScriptCallback( const tstring& name, float ( *callback )() )
{
	// return null if a callback with the given name has already been created.
	if ( _varTable.FindVarCallback( name ) )
		return 0;

	// create a new callback, register it, and return it.
	UExprCallback* var = new UExprCallback( callback );
	_varTable.RegisterVar( name, var );
	return var;
}

//----------------------------------------------------------
URef< UExprCallback >
UExprMgr::FindScriptCallback( const tstring& name ) const
{
	return _varTable.FindVarCallback( name );
}

//----------------------------------------------------------
URef< UExprArray >
UExprMgr::CreateGlobalArray( const tstring& name, float indexScale, bool smooth, bool clamp )
{
	// return null if an array with the given name has already been created.
	if ( _arrayTable.FindArray( name ) )
		return 0;

	// create a new array, register it, and return it.
	UExprArray* arr = new UExprArray( name, indexScale, smooth, clamp );
	_arrayTable.AddArray( name, arr );
	return arr;
}

//----------------------------------------------------------
URef< UExprArray >
UExprMgr::FindGlobalArray( const tstring& name ) const
{
	return _arrayTable.FindArray( name );
}
