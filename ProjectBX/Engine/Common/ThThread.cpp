//----------------------------------------------------------
// File:		ThThread.cpp
// Author:		Kevin Bray
// Created:		09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "ThThread.h"


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThThread::ThThread()
: _thread( 0 )
, _param( 0 )
, _started( false )
, _ended( false )
{

}

//----------------------------------------------------------
ThThread::~ThThread()
{
	// wait for the thread to terminate.
	if ( _thread != 0 )
	{
		// wait for the thread to terminate.
		DWORD result = WaitForSingleObject( ( HANDLE )_thread, 1000 );

		// make sure the thread terminated successfully!  If the return
		// value is WAIT_TIMEOUT, then either the thread is taking too
		// long to shut down, or the thread was simply not directed to
		// terminate.
		B_ASSERT( result == WAIT_OBJECT_0 );

		// close the thread's handle.
		CloseHandle( ( HANDLE )_thread );
	}
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
ThThread::IsCurrent()
{
	return ( GetCurrentThread() == ( HANDLE )_thread );
}

