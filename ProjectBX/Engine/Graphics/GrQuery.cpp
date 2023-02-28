//----------------------------------------------------------
// File:		GrQuery.cpp
// Author:		Kevin Bray
// Created:		09-11-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrQuery.h"

// project includes.
#include "GLSubsys.h"


static unsigned int GenQueryHandle()
{
	unsigned int queryHandle;
	bglGenQueries( 1, &queryHandle );
	return queryHandle;
}

const unsigned int kQueryTargetMap[ GrQuery::EQT_COUNT ] =
{
	GL_SAMPLES_PASSED_ARB,
};

bool
GrQuery::_queryActive[ GrQuery::EQT_COUNT ] = 
{
	false,
};

//**********************************************************
// class GrQuery
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrQuery::GrQuery( EQUERYTYPE queryType )
: _handle( GenQueryHandle() )
, _target( kQueryTargetMap[ queryType ] )
, _queryType( queryType )
, _thisQueryActive( false )
{
	
}

//----------------------------------------------------------
GrQuery::~GrQuery()
{
	B_ASSERT( !_queryActive );
	bglDeleteQueries( 1, &_handle );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrQuery::GetQueryCounterBits() const
{
	int bitCount = 0;
	bglGetQueryiv( _target, GL_QUERY_COUNTER_BITS_ARB, &bitCount );
	CHECK_GL();
	return bitCount;
}

//----------------------------------------------------------
bool
GrQuery::IsResultAvailable() const
{
	B_ASSERT( _handle != 0 );
	int available = 0;
	bglGetQueryObjectiv( _handle, GL_QUERY_RESULT_AVAILABLE, &available );
	CHECK_GL();
	return ( available != 0 );
}

//----------------------------------------------------------
__int64
GrQuery::GetQueryResult() const
{
	B_ASSERT( _handle != 0 );
	int result = 0;
	bglGetQueryObjectiv( _handle, GL_QUERY_RESULT, &result );
	CHECK_GL();
	return result;
}

//----------------------------------------------------------
void
GrQuery::BeginQuery()
{
	B_ASSERT( _handle != 0 );
	B_ASSERT( !_queryActive[ _queryType ] );
	_queryActive[ _queryType ] = true;
	_thisQueryActive = true;

	// begin the query for the current target.
	bglBeginQuery( _target, _handle );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrQuery::EndQuery()
{
	B_ASSERT( _handle != 0 );
	B_ASSERT( _queryActive[ _queryType ] );
	B_ASSERT( _thisQueryActive );
	_queryActive[ _queryType ] = false;
	_thisQueryActive = false;

	// end the query for the current target.
	bglEndQuery( _target );
	CHECK_GL();
}

//----------------------------------------------------------
void
GrQuery::PreContextReset()
{
	B_ASSERT( _handle != 0 );
	B_ASSERT( !_queryActive[ _queryType ] );

	// free the current handle.
	bglDeleteQueries( 1, &_handle );
	_handle = 0;
}

//----------------------------------------------------------
void
GrQuery::PostContextReset()
{
	B_ASSERT( _handle == 0 );
	B_ASSERT( !_queryActive[ _queryType ] );

	// create a new handle.
	_handle = GenQueryHandle();
}
