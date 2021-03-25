//----------------------------------------------------------
// File:		ThThread.h
// Author:		Kevin Bray
// Created:		09-01-08
//
// Purpose:		To abstract out and simplify thread creation.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <process.h>

//**********************************************************
// class ThThread
//**********************************************************
class ThThread
{
public:
	ThThread();
	virtual ~ThThread();

	// thread startup.
	template< typename _T >
	void					Start( _T* owner, unsigned int ( _T::*proc )( void* ), void* param = 0,
								   unsigned int stackSize = 4096, bool suspended = false );

	// this returns true if called from the thread itself.
	bool					IsCurrent();

private:
	// generic function type.
	typedef unsigned int ( ThThread::*GenericFuncPtr )( void* );

	// the thread's main function.
	template< typename _T >
	static unsigned int	__stdcall	ThreadMain( void* param );

	// thread member function.
	uintptr_t				_thread;
	void*					_owner;
	GenericFuncPtr			_proc;
	void*					_param;
	volatile bool			_started;
	volatile bool			_ended;
};


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
void
ThThread::Start( _T* owner, unsigned int ( _T::*proc )( void* ), void* param,
				 unsigned int stackSize, bool suspended )
{
	// make sure a valid callback was passed in.
	B_ASSERT( proc != 0 );

	// update our internal state.
	_proc = ( GenericFuncPtr )proc;
	_owner = owner;
	_param = param;

	// now create the thread.
	unsigned int threadId = 0;
	unsigned int createFlags = suspended ? CREATE_SUSPENDED : 0;
	_thread = _beginthreadex( 0, stackSize, ThreadMain< _T >, param, createFlags, &threadId );

	// if the thread was created successfully, wait until it has
	// started execution before resuming.
	if ( _thread )
	{
		while ( !_started )
		{
			__asm pause;
		}
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
template< typename _T >
unsigned int
ThThread::ThreadMain( void* threadThis )
{
	// get the member function pointer.
	typedef unsigned int ( _T::*MemberFuncPtr )( void* );

	// get the thread.
	ThThread* thread = ( ThThread* )threadThis;

	// get the callback.
	MemberFuncPtr proc = ( MemberFuncPtr )thread->_proc;

	// get the callback's object.
	_T*	owner = ( _T* )( thread->_owner );

	// get the parameter to pass to the callback.
	void* param = thread->_param;

	// now call the callback.
	thread->_started = true;
	unsigned int retVal = ( owner->*proc )( param );
	thread->_ended = true;

	// return the value returned by the callback.
	return retVal;
}
