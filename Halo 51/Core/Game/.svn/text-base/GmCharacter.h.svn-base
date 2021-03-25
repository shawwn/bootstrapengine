//----------------------------------------------------------
// File:		GmCharacter.h
// Author:		Shawn Presser
// Created:		12-20-08
//
// Purpose:		To represent a game character.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "GmEntity.h"

// Bootstrap Engine headers.
#include "Common/MMat3x3.h"

// project headers.
#include "GmInputController.h"

// forward declarations.
class NxController;

class GmCharacter : public GmEntity, public IControllerListener
{
	friend class GmEntityMgr;

	// this is used to contain a set of character-specific controller inputs.
	struct SInputs
	{
		SInputs( GmInputController* controller );

		// moves the character by an offset.
		GmInput*		vecMove;

		// sets the character's rotation.
		GmInput*		vecRot;
	};

protected:
	GmCharacter( const tstring& name, URef< GrModel > model, GmInputController* inputController );
	~GmCharacter();

public:
	// this is used to describe how an entity should be constructed.
	struct SInfo
	{
		SInfo();

		// the path to the entity's graphics model.
		UPath modelFilePath;

		// the character's position.
		MVec3 position;

		// the character's rotation.
		MMat3x3 rotation;

		// the character's horizontal radius.
		float radius;

		// the character's height.
		float height;

		// the character's speed.
		float speed;

		// the character's intelligence.
		GmInputController* inputController;
	};

	// returns the character's position.
	MVec3				GetPos() const;

	// gets or sets the character's rotation.
	const MMat3x3&		GetRot() const					{	return _rot;	}
	void				SetRot( const MMat3x3& rot )	{	_rot = rot;		}

	// gets or sets the character's speed.
	float				GetSpeed() const				{	return _speed;	}
	void				SetSpeed( float speed );

	// sets where the character receives its input.
	void				SetInputController( GmInputController* controller );

	// moves the character by a displacement vector.
	void				Move( const MVec3& displacement );

	// moves the character's model to the location of the character's physics representation.
	virtual void		Update();

protected:
	// called when the input controller's inputs have been modified.
	virtual void		OnInputsChanged( GmInputController* controller );

	// called when the input controller is being deleted.
	virtual void		OnControllerShutdown( GmInputController* controller );

private:
	MMat3x3				_rot;
	NxController*		_physicsController;
	GmInputController*	_inputController;
	SInputs*			_inputs;
	float				_speed;
};
