//----------------------------------------------------------
// File:		Expr.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Expr.h"

// engine headers.
#include "UExprMgr.h"
#include "UExprNumber.h"
#include "UExpr.h"

// editlib headers.
#include "EdExprCompiler.h"

// project headers.
#include "Utils.h"

// .NET namespaces.
using namespace System;

//**********************************************************
// ref class Expr
//**********************************************************

//----------------------------------------------------------
Bootstrap::Expr::Expr( UExprTerm* exprTerm )
	: _exprTerm( exprTerm )
{
	if ( _exprTerm )
		_exprTerm->IncRef();
}

//----------------------------------------------------------
Bootstrap::Expr::Expr( String^ exprScript )
	: _exprTerm( 0 )
{
	tstring errors;
	_exprTerm = gUExprMgr->ParseExpr( StrToTStr( exprScript ), errors );
	if ( _exprTerm )
		_exprTerm->IncRef();
	if ( errors.length() )
		_errors = TStrToStr( errors );
}

//----------------------------------------------------------
Bootstrap::Expr::Expr( float value )
	: _exprTerm( 0 )
{
	_exprTerm = gUExprMgr->CreateNumber( value );
	_exprTerm->IncRef();
}

//----------------------------------------------------------
Bootstrap::Expr::~Expr()
{
	this->!Expr();
}

//----------------------------------------------------------
Bootstrap::Expr::!Expr()
{
	if ( _exprTerm )
		_exprTerm->DecRef();
}

//----------------------------------------------------------
bool 
Bootstrap::Expr::Valid::get()
{
	return ( _exprTerm != 0 );
}

//----------------------------------------------------------
String^ 
Bootstrap::Expr::Decompile()
{
	// simply return an empty string if there is nothing to decompile.
	if ( !_exprTerm )
		return "";

	// decompile the script and return it's source text.
	tstring source;
	EdExprCompiler exprCompiler;
	exprCompiler.Decompile( source, _exprTerm );

	// return the source.
	return TStrToStr( source );
}
