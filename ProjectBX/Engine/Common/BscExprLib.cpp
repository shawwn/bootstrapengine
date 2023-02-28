//----------------------------------------------------------
// File:		BscExpr.h
// Author:		Kevin Bray
// Created:		07-01-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// lib header.
#include "BscExprLib.h"

// project includes.
#include "BscLexer.h"
#include "UExpr.h"
#include "UExprCallback.h"
#include "UExprNumber.h"
#include "UExprArrayRef.h"
#include "UExprArray.h"
#include "UExprMgr.h"
#include "BscArrayTable.h"
#include "BscVarTable.h"

#define ARRAYKEYWORD		"table"

static unsigned int	ParseTerm( URef< UExprTerm >* term, const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable );
static bool			IsOp( const SBscToken* token );

//**********************************************************
// expression parser
//**********************************************************

//----------------------------------------------------------
bool
BscIsExpr( const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable, unsigned int* tokensReadOut )
{
	B_ASSERT( tokens != 0 );

	SafeOffset tokensRead( tokensReadOut );

	tokensRead = 0;
	bool isExpr = true;
	do
	{
		if ( tokens[ tokensRead ].tokenId == BL_LPAREN )
		{
			// advance past the left paren.
			++tokensRead;

			// check to see if the sub-expression is an expression.
			unsigned int subTokensRead = 0;
			if ( !BscIsExpr( tokens + tokensRead, arrayTable, varTable, &subTokensRead ) )
			{
				tokensRead += subTokensRead;
				return false;
			}
			tokensRead += subTokensRead;

			// make sure there is a closing parenthesis.
			if ( tokens[ tokensRead ].tokenId != BL_RPAREN )
				return false;
			++tokensRead;
		}
		else
		{
			unsigned int subTokensRead = 0;
			if ( BscIsNumber( tokens + tokensRead, &subTokensRead ) )
				tokensRead += subTokensRead;
			else if ( BscIsArrayRef( tokens + tokensRead, arrayTable, varTable, &subTokensRead ) )
				tokensRead += subTokensRead;
			else if ( BscIsCallback( tokens + tokensRead, varTable, &subTokensRead ) )
				tokensRead += subTokensRead;
			else
			{
				// don't know what this is!
				tokensRead += subTokensRead;
				return false;
			}
		}

		// check to see if there is another term.
		if ( IsOp( tokens + tokensRead ) )
		{
			// advance past the operation.
			++tokensRead;
		}
		else
		{
			// no need to continue on... done reading the expression.
			break;
		}
	} while ( true );

	// we have an expression.
	return true;
}

//----------------------------------------------------------
unsigned int
BscParseExpr( URef< UExprTerm >* exprOut, const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
	B_ASSERT( tokens != 0 );
	B_ASSERT( exprOut != 0 );

	// ensure that someone checked on whether or not we can parse this.
	B_ASSERT( BscIsExpr( tokens, arrayTable, varTable ) );
	unsigned int tokensRead = 0;

	// simply create an expression.  Note that for simple expressions, we
	// can eliminate the need for this layer and simply return a constant.
	URef< UExpr > expr = gUExprMgr->CreateExpression();

	// parse the first term.
	unsigned int termTokensRead = 0;
	URef< UExprTerm > term;
	tokensRead += ParseTerm( &term, tokens, arrayTable, varTable );

	// add the term to the expression.
	expr->AddTerm( UExpr::EO_ADD, term );

	// parse alternating operations and terms.
	while ( IsOp( tokens + tokensRead ) )
	{
		// record the operation.
		UExpr::EMATHOP mathOp = UExpr::EO_ADD;
		switch ( tokens[ tokensRead ].tokenId )
		{
		case BL_PLUS:
			mathOp = UExpr::EO_ADD;
			break;
		case BL_MINUS:
			mathOp = UExpr::EO_SUBTRACT;
			break;
		case BL_ASTERISK:
			mathOp = UExpr::EO_MULTIPLY;
			break;
		case BL_SLASH:
			mathOp = UExpr::EO_DIVIDE;
			break;
		case BL_PERCENT:
			mathOp = UExpr::EO_MODULUS;
			break;
		case BL_LESS:
			mathOp = UExpr::EO_CMPLESS;
			break;
		case BL_GREATER:
			mathOp = UExpr::EO_CMPGREATER;
			break;
		case BL_LESSEQUAL:
			mathOp = UExpr::EO_CMPLESSEQUAL;
			break;
		case BL_GREATEREQUAL:
			mathOp = UExpr::EO_CMPGREATEREQUAL;
			break;
		case BL_NOTEQUAL:
			mathOp = UExpr::EO_CMPNOTEQUAL;
			break;
		case BL_ISEQUAL:
			mathOp = UExpr::EO_CMPEQUAL;
			break;
		case BL_LOGICALAND:
			mathOp = UExpr::EO_CMPAND;
			break;
		case BL_LOGICALOR:
			mathOp = UExpr::EO_CMPOR;
			break;
		default:
			// This should never happen.
			B_ASSERT( false );
		};

		// advance past the operation.
		++tokensRead;

		// parse and add the term to the term array.
		tokensRead += ParseTerm( &term, tokens + tokensRead, arrayTable, varTable );
		expr->AddTerm( mathOp, term );
	}

	// is there only one term?
	if ( expr->GetTermCount() == 1 )
		*exprOut = expr->GetTerm( 0 );
	else
		*exprOut = expr;

	return tokensRead;
}

//----------------------------------------------------------
unsigned int
ParseTerm( URef< UExprTerm >* term, const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
	B_ASSERT( term != 0 );
	B_ASSERT( tokens != 0 );

	// initialize to zero.
	unsigned int tokensRead = 0;

	// parse the first term.
	if ( tokens[ 0 ].tokenId == BL_LPAREN )
	{
		// parse an expression.
		// advance past the left paren.
		++tokensRead;

		// create a new expression for the term.
		unsigned int termTokensRead = 0;
		URef< UExprTerm > expr = 0;
		tokensRead += BscParseExpr( &expr, tokens + tokensRead, arrayTable, varTable );
		*term = expr;

		// make sure we have a matching closing parenthesis.
		B_ASSERT( tokens[ tokensRead ].tokenId == BL_RPAREN );

		// advance past the right paren.
		++tokensRead;
	}
	else
	{
		// parse a number or a variable.
		unsigned int termTokensRead = 0;
		if ( BscIsNumber( tokens ) )
		{
			URef< UExprNumber > number;
			tokensRead += BscParseNumber( &number, tokens );
			*term = number;
		}
		else if ( BscIsArrayRef( tokens, arrayTable, varTable ) )
		{
			URef< UExprArrayRef > arrayRef;
			tokensRead += BscParseArrayRef( &arrayRef, tokens, arrayTable, varTable );
			*term = arrayRef;
		}
		else if ( BscIsCallback( tokens, varTable ) )
		{
			// cast from URef< UExprCallback > -> ( UExprCallback* ) -> URef< ExprTerm >
			URef< UExprCallback > exprCallback = 0;
			tokensRead += BscParseCallback( &exprCallback, tokens, varTable );
			*term = ( UExprCallback* )exprCallback;
		}
		else
		{
			// TODO: Add support for variables and array references.
			B_ASSERT( false );
		}
	}

	// return the number of tokens read!
	return tokensRead;
}

//----------------------------------------------------------
bool
IsOp( const SBscToken* token )
{
	B_ASSERT( token != 0 );
	unsigned int tokenId = token->tokenId;
	return ( tokenId == BL_PLUS || tokenId == BL_MINUS ||
			 tokenId == BL_ASTERISK || tokenId == BL_SLASH ||
			 tokenId == BL_PERCENT || tokenId == BL_LESS ||
			 tokenId == BL_GREATER || tokenId == BL_LESSEQUAL ||
			 tokenId == BL_GREATEREQUAL || tokenId == BL_NOTEQUAL ||
			 tokenId == BL_ISEQUAL || tokenId == BL_LOGICALAND ||
			 tokenId == BL_LOGICALOR );
}


//**********************************************************
// number parser
//**********************************************************

//----------------------------------------------------------
bool
BscIsNumber( const SBscToken* tokens, unsigned int* tokensReadOut )
{
	B_ASSERT( tokens != 0 );

	SafeOffset tokensRead( tokensReadOut );

	// check for a sign and a number.
	if ( tokens[ 0 ].tokenId == BL_PLUS || tokens[ 0 ].tokenId == BL_MINUS )
	{
		tokensRead = 2;
		return tokens[ 1 ].tokenId == BL_NUMBER;
	}
	tokensRead = 1;
	return ( tokens[ 0 ].tokenId == BL_NUMBER );
}

//----------------------------------------------------------
unsigned int
BscParseNumber( URef< UExprNumber >* numberOut, const SBscToken* tokens )
{
	B_ASSERT( tokens != 0 );
	B_ASSERT( BscIsNumber( tokens ) );

	// check for a sign.
	unsigned int tokensRead = 0;
	tstring text = tokens[ tokensRead ].text;
	if ( tokens[ tokensRead ].tokenId == BL_PLUS || tokens[ tokensRead ].tokenId == BL_MINUS )
	{
		++tokensRead;
		text += tokens[ tokensRead ].text;
	}

	// make sure the token we just processed is a number.
	B_ASSERT( tokens[ tokensRead ].tokenId == BL_NUMBER );

	// increment past the number part.
	++tokensRead;

	// convert from text to a floating point value.
	float value = ( float )atof( text.c_str() );
	*numberOut = gUExprMgr->CreateNumber( value );

	return tokensRead;
}


//**********************************************************
// array reference parser
//**********************************************************

//----------------------------------------------------------
bool
BscIsArrayRef( const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable, unsigned int* tokensReadOut )
{
	SafeOffset tokensRead( tokensReadOut );

	// check to make sure the first thing is an identifier.
	tokensRead = 1;
	if ( tokens[ 0 ].tokenId != BL_IDENTIFIER )
		return false;

	// array not found.
	if ( arrayTable.FindArray( tokens[ 0 ].text ) == 0 )
		return false;

	// make sure the identifier is followed by an left square brace.
	tokensRead = 2;
	if ( tokens[ 1 ].tokenId != BL_LSQUAREBRACE )
		return false;

	// make sure the left square brace is followed by an expression.
	unsigned int subTokensRead = 0;
	if ( !BscIsExpr( tokens + 2, arrayTable, varTable, &subTokensRead ) )
	{
		tokensRead += subTokensRead;
		return false;
	}
	tokensRead += subTokensRead;

	// make sure the expression is followed by a right square brace.
	if ( tokens[ tokensRead ].tokenId != BL_RSQUAREBRACE )
	{
		++tokensRead;
		return false;
	}
	++tokensRead;

	// we do have an array reference.
	return true;
}

//----------------------------------------------------------
unsigned int
BscParseArrayRef( URef< UExprArrayRef >* arrayRef, const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
	B_ASSERT( BscIsArrayRef( tokens, arrayTable, varTable ) );

	// get the expression.
	URef< UExprTerm > expr = 0;
	unsigned int tokensRead = BscParseExpr( &expr, tokens + 2, arrayTable, varTable );

	// include the name and square braces in our count.
	tokensRead += 3;

	// get the array name.
	tstring arrayName = tokens[ 0 ].text;

	// find the array, and create an array reference.
	URef< UExprArray > array = arrayTable.FindArray( arrayName );
	*arrayRef = gUExprMgr->CreateArrayRef( array, expr );

	return tokensRead;
}


//**********************************************************
// callback parser
//**********************************************************

//----------------------------------------------------------
bool
BscIsCallback( const SBscToken* tokens, const BscVarTable& varTable, unsigned int* tokensReadOut )
{
	SafeOffset tokensRead( tokensReadOut );

	tokensRead = 1;
	if ( tokens[ 0 ].tokenId != BL_IDENTIFIER )
		return false;

	if ( varTable.FindVarCallback( tokens[ 0 ].text ) == 0 )
		return false;

	// yes, this is a callback.
	return true;
}

//----------------------------------------------------------
unsigned int
BscParseCallback( URef< UExprCallback >* callback, const SBscToken* tokens, const BscVarTable& varTable )
{
	B_ASSERT( BscIsCallback( tokens, varTable ) );
	*callback = varTable.FindVarCallback( tokens[ 0 ].text );
	return 1;
}


//**********************************************************
// array parser
//**********************************************************

//----------------------------------------------------------
bool
BscIsArray( const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable, unsigned int* tokensReadOut )
{
	SafeOffset tokensRead( tokensReadOut );

	// check to see if we have an identifier, followed by an equal sign, which
	// is followed by another identifier, a number, or a string.
	tokensRead = 1;
	if ( _stricmp( tokens[ 0 ].text.c_str(), ARRAYKEYWORD ) != 0 )
		return false;

	tokensRead = 2;
	if ( tokens[ 1 ].tokenId != BL_IDENTIFIER )
		return false;

	bool clamp = false;
	bool smooth = false;
	bool scale = false;

	// check for array parameters.
	if ( tokens[ tokensRead ].tokenId == BL_LPAREN )
	{
		// advance past the left paren.
		++tokensRead;

		// parse in array parameters.
		do
		{
			if ( tokens[ tokensRead ].tokenId == BL_IDENTIFIER )
			{
				if ( _stricmp( tokens[ tokensRead ].text.c_str(), "smooth" ) == 0 )
				{
					if ( smooth )
						return false;
					++tokensRead;
					smooth = true;
				}
				else if ( _stricmp( tokens[ tokensRead ].text.c_str(), "clamp" ) == 0 )
				{
					if ( clamp )
						return false;
					++tokensRead;
					clamp = true;
				}
				else
				{
					return false;
				}
			}
			else if ( tokens[ tokensRead ].tokenId == BL_NUMBER )
			{
				// if the scale has already been specified, return false.
				if ( scale )
					return false;
				++tokensRead;
				scale = true;
			}
			else
			{
				break;
			}

			if ( tokens[ tokensRead ].tokenId != BL_COMMA )
			{
				// can only have a right paren.
				if ( tokens[ tokensRead ].tokenId == BL_RPAREN )
				{
					++tokensRead;
					break;
				}
				else
					return false;
			}
			// advance past the comma.
			++tokensRead;
		} while ( true );
	}

	// don't need an equals... seems... crappy.
//	if ( tokens[ tokensRead ].tokenId != BL_EQUALS )
//	{
//		return false;
//	}
//	++tokensRead;

	if ( tokens[ tokensRead ].tokenId != BL_LBRACE )
	{
		return false;
	}
	++tokensRead;

	do
	{
		unsigned int subTokensRead = 0;
		if ( !BscIsExpr( tokens + tokensRead, arrayTable, varTable, &subTokensRead ) )
		{
			tokensRead += subTokensRead;
			return false;
		}

		tokensRead += subTokensRead;

		if ( tokens[ tokensRead ].tokenId == BL_COMMA )
		{
			// advance past the comma.
			++tokensRead;

			// optional comma before the right brace.
			if ( tokens[ tokensRead ].tokenId == BL_RBRACE )
				break;
		}
		else
		{
			// not a comma, so break.
			break;
		}

		// check all subsequent paths making sure they're paths.
	} while ( true );

	if ( tokens[ tokensRead ].tokenId != BL_RBRACE )
	{
		++tokensRead;
		return false;
	}

	return true;
}

//----------------------------------------------------------
unsigned int
BscParseArray( UExprArray** arrayOut, tstring& name, const SBscToken* tokens, const BscArrayTable& arrayTable,
			   const BscVarTable& varTable )
{
	// make sure we have a valid array to parse.
	B_ASSERT( BscIsArray( tokens, arrayTable, varTable ) );

	name = tokens[ 1 ].text;

	float indexScale = 1.0f;
	bool smooth = false;
	bool clamp = false;

	// advance past the array keyword and the name.
	unsigned int tokensRead = 2;

	// check for array parameters.
	if ( tokens[ tokensRead ].tokenId == BL_LPAREN )
	{
		// advance past the left paren.
		++tokensRead;

		// parse in array parameters.
		do
		{
			if ( _stricmp( tokens[ tokensRead ].text.c_str(), "smooth" ) == 0 )
			{
				++tokensRead;
				smooth = true;
			}
			else if ( _stricmp( tokens[ tokensRead ].text.c_str(), "clamp" ) == 0 )
			{
				++tokensRead;
				clamp = true;
			}
			else if ( tokens[ tokensRead ].tokenId == BL_NUMBER )
			{
				++tokensRead;
				indexScale = ( float )atof( tokens[ tokensRead ].text.c_str() );
			}
			else
			{
				// this shouldn't be possible.  This would be caused by an error
				// in this loop's logic, or a problem with the IsArray function.
				B_ASSERT( false );
			}

			// check to see if we hit the comma or closing paren, and advance past
			// the token.
			if ( tokens[ tokensRead++ ].tokenId != BL_COMMA )
			{
				// must have hit the closing paren.
				break;
			}

		} while ( true );
	}

	URef< UExprArray > array = gUExprMgr->CreateArray( name, indexScale, smooth, clamp );

//	// advance past the equals sign and left brace.
//	tokensRead += 2;

	// advance past the left brace.
	tokensRead += 1;

	// parse in all expressions.
	unsigned int subTokensRead = 0;
	do
	{
		if ( tokens[ tokensRead ].tokenId == BL_RBRACE )
		{
			++tokensRead;
			break;
		}
		if ( BscIsExpr( tokens + tokensRead, arrayTable, varTable, &subTokensRead ) )
		{
			URef< UExprTerm > expr = 0;
			tokensRead += BscParseExpr( &expr, tokens + tokensRead, arrayTable, varTable );
			array->AddElement( expr );
		}
		if ( tokens[ tokensRead ].tokenId == BL_COMMA )
		{
			++tokensRead;
		}
	} while ( true );

	// return the array.
	*arrayOut = array;
	return tokensRead;
}
