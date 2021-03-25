//----------------------------------------------------------
// File:		URand.cpp
// Author:		Kevin Bray
// Created:		11-19-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "URand.h"

#define IA					16807
#define IM					2147483647
#define AM					( 1.0 / IM )
#define IQ					127773
#define IR					2836
#define ARBITRARY_MASK		123459876

//**********************************************************
// class URand
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
URand::URand( int seed )
: _seed( seed )
{
}

//----------------------------------------------------------
URand::~URand()
{
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
URand::SetSeed( int seed )
{
	_seed = seed;
}

//----------------------------------------------------------
unsigned int
URand::RandInt()
{
	// xor the seed with the arbitrary mask so that it's rarely 0.
	_seed ^= ARBITRARY_MASK;

	// compute _seed = ( IA * _seed ) % IM without overflow using
	// Schrage's method.
	int k = _seed / IQ;
	_seed = IA * ( _seed - k * IQ ) - ( IR * k );
	if ( _seed < 0 )
		_seed += IM;

	// store the result and xor the seed with the arbitrary mask.
	unsigned int result = ( unsigned int )_seed;
	_seed ^= ARBITRARY_MASK;

	// return our result.
	return result;
}

//----------------------------------------------------------
float
URand::RandFloat()
{
	// move to the range [0..1] and drop bits by going to float.
	return float( AM * double( RandInt() ) );
}
