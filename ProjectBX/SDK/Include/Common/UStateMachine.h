//----------------------------------------------------------
// File:		UStateMachine.h
// Author:		Kevin Bray
// Created:		04-12-06
//
// Purpose:		To provide state driven event based callbacks.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UStateMachine
//**********************************************************
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
class UStateMachine
{
public:
	typedef bool ( _T::*Callback )( unsigned int prevState, unsigned int event, unsigned int newState );
	typedef void ( _T::*UpdateCallback )( unsigned int curState );

	UStateMachine( unsigned int initState );
	~UStateMachine();

	unsigned int	GetCurState() const			{	return _curState;		}

	// sends an event.
	void			SendEvent( _T* thisPtr, unsigned int event );

	// calls the current update callback if there is one.
	void			Update( _T* thisPtr );

	// the callback gets the state it's currently in, and the event that was fired.  If the callback
	// returns true, then the state can be entered.
	void			SetTransition( unsigned curState, unsigned int event, unsigned int toState, Callback callback );

	// adds a callback for update.
	void			SetStateUpdateCallback( unsigned int state, UpdateCallback callback );

	// this is called when a state is entered.
	void			SetStateEnterCallback( unsigned int state, Callback callback );
	void			SetStateLeaveCallback( unsigned int state, Callback callback );

private:
	unsigned int	_toState[ kMaxStates ][ kMaxEvents ];
	Callback		_transitions[ kMaxStates ][ kMaxEvents ];
	Callback		_onEnter[ kMaxStates ];
	Callback		_onLeave[ kMaxStates ];
	UpdateCallback	_onUpdate[ kMaxStates ];
	unsigned int	_curState;
};


//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
UStateMachine< _T, kMaxStates, kMaxEvents >::UStateMachine( unsigned int initState )
: _curState( initState )
{
	MemSet( _toState, 0xFF, sizeof( _toState ) );
	MemSet( _transitions, 0, sizeof( _transitions ) );
	MemSet( _onEnter, 0, sizeof( _onEnter ) );
	MemSet( _onLeave, 0, sizeof( _onLeave ) );
	MemSet( _onUpdate, 0, sizeof( _onUpdate ) );
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
UStateMachine< _T, kMaxStates, kMaxEvents >::~UStateMachine()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::SendEvent( _T* thisPtr, unsigned int event )
{
	B_ASSERT( event < kMaxEvents );
	unsigned int toState = _toState[ _curState ][ event ];
	if ( toState == ~0 )
		return;

	// check to see if we have a callback available.
	unsigned int prevState = _curState;
	if ( _transitions[ _curState ][ event ] != 0 )
	{
		// call the function.  do not change states if it returns false.
		if ( !( thisPtr->*_transitions[ _curState ][ event ] )( _curState, event, toState ) )
			return;
	}

	// transition.
	_curState = toState;
	if ( _curState != prevState )
	{
		if ( _onEnter[ _curState ] != 0 )
			( thisPtr->*_onEnter[ _curState ] )( prevState, event, _curState );
		if ( _onLeave[ _curState ] != 0 )
			( thisPtr->*_onLeave[ _curState ] )( prevState, event, _curState );
	}
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::Update( _T* thisPtr )
{
	if ( _onUpdate[ _curState ] )
		( thisPtr->*_onUpdate[ _curState ] )( _curState );
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::SetTransition( unsigned curState, unsigned int event, unsigned int toState, Callback callback )
{
	// add a transition.
	_toState[ curState ][ event ] = toState;
	_transitions[ curState ][ event ] = callback;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::SetStateUpdateCallback( unsigned int state, UpdateCallback callback )
{
	_onUpdate[ state ] = callback;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::SetStateEnterCallback( unsigned int state, Callback callback )
{
	_onEnter[ state ] = callback;
}

//----------------------------------------------------------
template < typename _T, const unsigned int kMaxStates, const unsigned int kMaxEvents >
void
UStateMachine< _T, kMaxStates, kMaxEvents >::SetStateLeaveCallback( unsigned int state, Callback callback )
{
	_onLeave[ state ] = callback;
}
