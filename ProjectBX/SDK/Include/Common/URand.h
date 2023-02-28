//----------------------------------------------------------
// File:		URand.h
// Author:		Kevin Bray
// Created:		11-19-06
//
// Purpose:		To provide a high performance, psuedo random number generator
//				that exceeds rand() in both speed and randomness.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class URand
//**********************************************************
class URand
{
public:
	URand( int seed );
	~URand();

	void			SetSeed( int seed );

	// get the next random number.
	unsigned int	RandInt();
	float			RandFloat();

private:
	int				_seed;
};
