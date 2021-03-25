//----------------------------------------------------------
// File:		ThSignal.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// file header.
#include "ThSignal.h"

// windows includes.
#include <windows.h>

//**********************************************************
// class ThSignal
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThSignal::ThSignal( bool raised, bool autoLower )
{
	// create the signal.
	_signalData = CreateEvent( NULL, autoLower ? FALSE : TRUE, raised ? TRUE : FALSE, NULL );
}

//----------------------------------------------------------
ThSignal::~ThSignal()
{
	CloseHandle( ( HANDLE )_signalData );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
ThSignal::Wait() const
{
	WaitForSingleObject( _signalData, INFINITE );
}

//----------------------------------------------------------
bool
ThSignal::IsRaised() const
{
	// check the event and return true if it has been signaled.
	DWORD result = WaitForSingleObject( _signalData, 0 );
	return ( result == WAIT_OBJECT_0 );
}

//----------------------------------------------------------
bool
ThSignal::IsLowered() const
{
	return !IsRaised();
}

//----------------------------------------------------------
void
ThSignal::Raise()
{
	SetEvent( _signalData );
}

//----------------------------------------------------------
void
ThSignal::Lower()
{
	ResetEvent( _signalData );
}
