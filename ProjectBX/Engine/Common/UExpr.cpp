//----------------------------------------------------------
// File:		UExpr.cpp
// Author:		Kevin Bray
// Created:		07-05-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

#include "UExpr.h"

//**********************************************************
// class UExpr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UExpr::UExpr()
: _dynamic( false )
{

}

//----------------------------------------------------------
UExpr::~UExpr()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UExpr::AddTerm( EMATHOP mathOp, URef< UExprTerm > term )
{
	B_ASSERT( term != 0 );
	_terms.push_back( STerm( mathOp, term ) );
	_dynamic = _dynamic || term->IsDynamic();
}

//----------------------------------------------------------
URef< UExprTerm >
UExpr::GetTerm( unsigned int idx ) const
{
	return _terms[ idx ].term;
}

//----------------------------------------------------------
UExpr::EMATHOP
UExpr::GetTermMathOp( unsigned int idx ) const
{
	return _terms[ idx ].mathOp;
}

//----------------------------------------------------------
float
UExpr::GetValue() const
{
	// shouldn't have been able to be parsed!  This shouldn't be possible.
	B_ASSERT( _terms.size() > 0 );

	struct SStackEntry
	{
		float value;
		EMATHOP mathOp;
	};

	// if there is only one term, return early.
	if ( _terms.size() == 1 )
	{
		return _terms[ 0 ].term->GetValue();
	}

	// Note that we reduce in order of precedence.
	// precedence: * / %, + -, < > <= >= == !=, && ||
	unsigned int stackSize = ( unsigned int )_terms.size();
	SStackEntry* valueStack0 = ( SStackEntry* )alloca( stackSize * sizeof( SStackEntry ) * 2 );
	SStackEntry* valueStack1 = valueStack0 + stackSize;

	// go through all of the terms.
	unsigned int stackTop0 = 0;
	TermVec::const_iterator iter = _terms.begin();
	const TermVec::const_iterator end = _terms.end();
	
	// initialize the first entry in the stack.
	valueStack0[ 0 ].value = iter->term->GetValue();
	valueStack0[ 0 ].mathOp = EO_ADD;

	// advance past the first term in the array.
	++iter;

	// go through the remaining terms in the array.
	for ( ; iter != end; ++iter )
	{
		if ( iter->mathOp == EO_MULTIPLY )
		{
			valueStack0[ stackTop0 ].value *= iter->term->GetValue();
		}
		else if ( iter->mathOp == EO_DIVIDE )
		{
			valueStack0[ stackTop0 ].value /= iter->term->GetValue();
		}
		else if ( iter->mathOp == EO_MODULUS )
		{
			// do the division and take the fractional part.
			float temp = ( valueStack0[ stackTop0 ].value / iter->term->GetValue() );
			valueStack0[ stackTop0 ].value = temp - Floor( temp );
		}
		else
		{
			// push onto the stack.
			++stackTop0;
			valueStack0[ stackTop0 ].value = iter->term->GetValue();
			valueStack0[ stackTop0 ].mathOp = iter->mathOp;
		}
	}

	// shouldn't be possible to overwrite.
	B_ASSERT( valueStack0[ 0 ].mathOp == EO_ADD );

	// are we done?  This means we hit all multiplication, division, or modulus.
	// If this is the case, then there is no need to reduce further.  We can
	// simply return early.
	if ( stackTop0 == 0 )
		return valueStack0[ 0 ].value;

	// now reduce addition/subtraction.
	// initialize the first element in valueStack1.
	valueStack1[ 0 ].value = valueStack0[ 0 ].value;
	valueStack1[ 0 ].mathOp = EO_ADD;
	unsigned int stackTop1 = 0;
	for ( unsigned int entryIdx = 1; entryIdx <= stackTop0; ++entryIdx )
	{
		if ( valueStack0[ entryIdx ].mathOp == EO_ADD )
		{
			valueStack1[ stackTop1 ].value += valueStack0[ entryIdx ].value;
		}
		else if ( valueStack0[ entryIdx ].mathOp == EO_SUBTRACT )
		{
			valueStack1[ stackTop1 ].value -= valueStack0[ entryIdx ].value;
		}
		else
		{
			// push the new entry onto valueStack1.  At this point, should be
			// either a comparison or a logical operation.
			valueStack1[ ++stackTop1 ] = valueStack0[ entryIdx ];
		}
	}

	// are we done?  This means we hit all addition or subtraction.  If this
	// is the case, then there is no need to reduce further.
	if ( stackTop1 == 0 )
		return valueStack1[ 0 ].value;

	// now reduce comparisons!
	// initialize the first element in valueStack0.
	valueStack0[ 0 ].value = valueStack1[ 0 ].value;
	valueStack0[ 0 ].mathOp = EO_ADD;
	stackTop0 = 0;
	for ( unsigned int entryIdx = 1; entryIdx <= stackTop1; ++entryIdx )
	{
		if ( valueStack1[ entryIdx ].mathOp == EO_CMPLESS )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value < valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else if ( valueStack1[ entryIdx ].mathOp == EO_CMPGREATER )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value > valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else if ( valueStack1[ entryIdx ].mathOp == EO_CMPLESSEQUAL )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value <= valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else if ( valueStack1[ entryIdx ].mathOp == EO_CMPGREATEREQUAL )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value >= valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else if ( valueStack1[ entryIdx ].mathOp == EO_CMPNOTEQUAL )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value != valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else if ( valueStack1[ entryIdx ].mathOp == EO_CMPEQUAL )
		{
			valueStack0[ stackTop0 ].value = ( valueStack0[ stackTop0 ].value == valueStack1[ entryIdx ].value ? 1.0f : 0.0f );
		}
		else
		{
			// push the new entry onto valueStack0.  At this point, should be
			// a logical operation.
			valueStack0[ ++stackTop0 ] = valueStack1[ entryIdx ];
		}
	}

	// are we done?  This means we hit all comparisons.  If this is the case,
	// then there is no need to reduce further.
	if ( stackTop0 == 0 )
		return valueStack0[ 0 ].value;

	// now reduce logical operations!
	// initialize the first element in valueStack1.
	float value = valueStack0[ 0 ].value;
	for ( unsigned int entryIdx = 1; entryIdx <= stackTop0; ++entryIdx )
	{
		if ( valueStack0[ entryIdx ].mathOp == EO_CMPAND )
		{
			value = ( value != 0.0f && valueStack0[ entryIdx ].value != 0.0f ? 1.0f : 0.0f );
		}
		else if ( valueStack0[ entryIdx ].mathOp == EO_CMPOR )
		{
			value = ( value != 0.0f || valueStack0[ entryIdx ].value != 0.0f ? 1.0f : 0.0f );
		}
		else
		{
			// shouldn't be possible!  At this point, all operations except
			// logical operations should have been reduced!  If we get here,
			// then an operation was added, and is not supported correctly,
			// or there is an error in the existing logic somewhere.
			B_ASSERT( false );
		}
	}

	// all done!
	return value;
}
