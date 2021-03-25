//----------------------------------------------------------
// File:		GmInputController.h
// Author:		Shawn Presser
// Created:		12-20-08
//
// Purpose:		To use inputs to drive entities.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/MVec3.h"

// std c++ headers.
#include <map>
#include <set>

// forward declarations.
class GmInputController;

//**********************************************************
// enum EINPUTTYPE
//**********************************************************
enum EINPUTTYPE
{
	EIT_STRING,
	EIT_VECTOR,
	EIT_NUMBER,
	EIT_BOOL,
	EIT_MAX,
	EIT_FORCE_DWORD = 0x7FFFFFFF
};

//**********************************************************
// class GmInput
//**********************************************************
class GmInput
{
public:
	GmInput( GmInputController* controller, EINPUTTYPE type );

	// accessors.
	EINPUTTYPE			GetType() const			{	return _type;		}
	bool				IsDirty() const			{	return _dirty;		}
	const tstring&		GetString();
	const MVec3&		GetVector();
	float				GetNumber();
	bool				GetBool();

	// modifiers.
	void				SetValue( const tstring& val );
	void				SetValue( const MVec3& val );
	void				SetValue( float val );
	void				SetValue( bool val );

	// returns false if the input is null or if it isn't dirty, otherwise true.
	static bool			IsDirty( GmInput* input );

private:
	tstring				_strVal;
	MVec3				_vecVal;
	float				_numVal;
	EINPUTTYPE			_type;
	GmInputController*	_controller;
	bool				_boolVal;
	bool				_dirty;
};

//**********************************************************
// class IControllerListener
//**********************************************************
class IControllerListener
{
public:
	// called when the controller's inputs have been modified.
	virtual void		OnInputsChanged( GmInputController* controller )=0;

	// called when the controller is being deleted.
	virtual void		OnControllerShutdown( GmInputController* controller )=0;
};

//**********************************************************
// class GmInputController
//**********************************************************
class GmInputController
{
	friend class GmInput;
public:
	GmInputController();
	~GmInputController();

	// input management.
	GmInput*			GetInput( const tstring& name, EINPUTTYPE type );
	GmInput*			FindInput( const tstring& name, EINPUTTYPE type );

	// listener management.
	void				AddListener( IControllerListener* listener );
	void				RemoveListener( IControllerListener* listener );

	// updates the controller.  Call this once each frame.
	void				Update();

	// internal use only.
	void				MarkAsDirty()						{	_dirty = true;		}

private:
	typedef std::map< tstring, GmInput* >		InputMap;
	typedef std::set< IControllerListener* >	ListenerContainer;

	InputMap			_inputs;
	ListenerContainer	_listeners;
	bool				_dirty;
};

