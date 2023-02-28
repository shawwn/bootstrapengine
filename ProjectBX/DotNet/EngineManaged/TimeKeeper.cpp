//----------------------------------------------------------
// File:		TimeKeeper.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "TimeKeeper.h"

// standard headers.
#include <windows.h>
#include <mmsystem.h>
#include <cassert>

//**********************************************************
// ref class TimeKeeper
//**********************************************************

//----------------------------------------------------------
Bootstrap::TimeKeeper::TimeKeeper()
: _speed( 65536 )
, _absTime( 0 )
, _sysTimeDelta( 0 )
, _gameTimeDelta( 0 )
, _paused( false )
{
	_absTime = timeGetTime();
}

//----------------------------------------------------------
Bootstrap::TimeKeeper::~TimeKeeper()
{
	this->!TimeKeeper();
}

//----------------------------------------------------------
Bootstrap::TimeKeeper::!TimeKeeper()
{
}

//----------------------------------------------------------
void 
Bootstrap::TimeKeeper::Update()
{
	// calculate the new system time delta.
	DWORD time = timeGetTime();
	_sysTimeDelta = time - _absTime;
	_absTime = time;

	// add in the speed multiplier.
	if ( !_paused )
	{
		unsigned __int64 gameTimeDelta = _sysTimeDelta * _speed;
		gameTimeDelta >>= 16;
		_gameTimeDelta = ( unsigned int )gameTimeDelta;
	}
	else
	{
		_gameTimeDelta = 0;
	}
}

//----------------------------------------------------------
void 
Bootstrap::TimeKeeper::SetGameTimeSpeed( float speed )
{
	_speed = ( unsigned int )( ( double )speed * 65536.0 );
}