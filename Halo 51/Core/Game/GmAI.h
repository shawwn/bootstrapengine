//----------------------------------------------------------
// File:		GmAI.h
// Author:		Shawn Presser
// Created:		12-21-08
//
// Purpose:		To drive a game character using an input controller.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/MVec3.h"

// forward declarations.
class GmInputController;
class GmInput;
class GmCharacter;

class GmAI
{
public:
	GmAI( GmCharacter* character );
	~GmAI();

	bool				IsActive() const					{	return _active;							}

	const MVec3&		GetDestination() const				{	return _destination;					}
	void				SetDestination( const MVec3& pt )	{	_destination = pt;	_active = true;		}

	void				Update( unsigned int dt );

private:
	MVec3				_destination;
	GmCharacter*		_character;
	GmInputController*	_controller;
	GmInput*			_vecMove;
	GmInput*			_vecRot;
	bool				_active;
};
