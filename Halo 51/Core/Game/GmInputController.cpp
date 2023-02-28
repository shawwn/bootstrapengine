//----------------------------------------------------------
// File:		GmInputController.cpp
// Author:		Shawn Presser
// Created:		12-20-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmInputController.h"

//**********************************************************
// class GmInput
//**********************************************************

//----------------------------------------------------------
GmInput::GmInput( GmInputController* controller, EINPUTTYPE type )
: _type( type )
, _controller( controller )
, _vecVal( 0.0f, 0.0f, 0.0f )
, _numVal( 0.0f )
, _boolVal( false )
, _dirty( false )
{
}

//----------------------------------------------------------
const tstring&
GmInput::GetString()
{
	B_ASSERT( _type == EIT_STRING );
	_dirty = false;
	return _strVal;
}

//----------------------------------------------------------
const MVec3&
GmInput::GetVector()
{
	B_ASSERT( _type == EIT_VECTOR );
	_dirty = false;
	return _vecVal;
}

//----------------------------------------------------------
float
GmInput::GetNumber()
{
	B_ASSERT( _type == EIT_NUMBER );
	_dirty = false;
	return _numVal;
}

//----------------------------------------------------------
bool
GmInput::GetBool()
{
	B_ASSERT( _type == EIT_BOOL );
	_dirty = false;
	return _boolVal;
}

//----------------------------------------------------------
void
GmInput::SetValue( const tstring& val )
{
	B_ASSERT( _type == EIT_STRING );
	_strVal = val;
	_dirty = true;
	_controller->MarkAsDirty();
}

//----------------------------------------------------------
void
GmInput::SetValue( const MVec3& val )
{
	B_ASSERT( _type == EIT_VECTOR );
	_vecVal = val;
	_dirty = true;
	_controller->MarkAsDirty();
}

//----------------------------------------------------------
void
GmInput::SetValue( float val )
{
	B_ASSERT( _type == EIT_NUMBER );
	_numVal = val;
	_dirty = true;
	_controller->MarkAsDirty();
}

//----------------------------------------------------------
void
GmInput::SetValue( bool val )
{
	B_ASSERT( _type == EIT_BOOL );
	_boolVal = val;
	_dirty = true;
	_controller->MarkAsDirty();
}

//----------------------------------------------------------
bool
GmInput::IsDirty( GmInput* input )
{
	return input && input->_dirty;
}

//**********************************************************
// class GmInputController
//**********************************************************

//----------------------------------------------------------
GmInputController::GmInputController()
{
}

//----------------------------------------------------------
GmInputController::~GmInputController()
{
	// notify each listener that we're being deleted.
	for ( ListenerContainer::iterator it = _listeners.begin(); it != _listeners.end(); ++it )
	{
		IControllerListener* listener = *it;
		listener->OnControllerShutdown( this );
	}
}

//----------------------------------------------------------
GmInput*
GmInputController::GetInput( const tstring& name, EINPUTTYPE type )
{
	// if the input exists, return it.
	InputMap::iterator it = _inputs.find( name );
	if ( it != _inputs.end() )
	{
		GmInput* result = it->second;
		B_ASSERT( result->GetType() == type );
		return result;
	}

	// otherwise, add a new input.
	GmInput* input = new GmInput( this, type );
	bool success = _inputs.insert( std::make_pair( name, input ) ).second;

	// verify that the insertion succeeded.
	B_ASSERT( success );

	// return the new input.
	return input;
}

//----------------------------------------------------------
GmInput*
GmInputController::FindInput( const tstring& name, EINPUTTYPE type )
{
	// if the input exists, return it.
	InputMap::iterator it = _inputs.find( name );
	if ( it != _inputs.end() )
	{
		GmInput* result = it->second;
		B_ASSERT( result->GetType() == type );
		return result;
	}

	// otherwise, return null.
	return 0;
}

//----------------------------------------------------------
void
GmInputController::AddListener( IControllerListener* listener )
{
	// add the listener.
	B_ASSERT( listener );
	_listeners.insert( listener );
}

//----------------------------------------------------------
void
GmInputController::RemoveListener( IControllerListener* listener )
{
	// remove the listener, if present.
	ListenerContainer::iterator it = _listeners.find( listener );
	if ( it != _listeners.end() )
	{
		_listeners.erase( listener );
	}
}

//----------------------------------------------------------
void
GmInputController::Update()
{
	// if there is no work to be done, abort.
	if ( !_dirty )
		return;

	// notify each listener that inputs have changed.
	for ( ListenerContainer::iterator it = _listeners.begin(); it != _listeners.end(); ++it )
	{
		IControllerListener* listener = *it;
		listener->OnInputsChanged( this );
	}

	// clear the dirty flag.
	_dirty = false;
}
