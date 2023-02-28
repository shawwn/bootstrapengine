//----------------------------------------------------------
// File:        GmCharacter.cpp
// Author:      Shawn Presser
// Created:     12-20-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmCharacter.h"

// physics headers.
#include "PhSubsys.h"

//----------------------------------------------------------
GmCharacter::SInputs::SInputs( GmInputController* controller )
: vecMove( controller->FindInput( "move", EIT_VECTOR ) )
, vecRot( controller->FindInput( "rotation", EIT_VECTOR ) )
{
}

//----------------------------------------------------------
GmCharacter::SInfo::SInfo()
: position( 0.0f, 0.0f, 0.0f )
, radius( 0.5f )
, height( 2.0f )
, speed( 1.0f )
, inputController( 0 )
{
}

//----------------------------------------------------------
GmCharacter::GmCharacter( const tstring& name, URef< GrModel > model, GmInputController* inputController )
: GmEntity( name, model )
, _physicsController( 0 )
, _inputController( 0 )
, _inputs( 0 )
{
    // acquire the incoming input controller.
    SetInputController( inputController );
}

//----------------------------------------------------------
GmCharacter::~GmCharacter()
{
    B_ASSERT( _physicsController );
    gPhCharMgr->releaseController( *_physicsController );

    // release our input controller.
    SetInputController( 0 );
}

//----------------------------------------------------------
MVec3
GmCharacter::GetPos() const
{
    return gPhSubsys->GetCharPosition( _physicsController );
}

//----------------------------------------------------------
void
GmCharacter::SetSpeed( float speed )
{
    _speed = Max( 0.0f, speed );
}

//----------------------------------------------------------
void
GmCharacter::SetInputController( GmInputController* controller )
{
    // free the controller inputs container 
    delete _inputs;
    _inputs = 0;

    // remove the old controller.
    if ( _inputController )
    {
        // remove ourselves from the controller's listener list.
        _inputController->RemoveListener( this );
        _inputController = 0;
    }

    if ( controller )
    {
        // store the new controller.
        _inputController = controller;

        // acquire its relevant inputs.
        _inputs = new SInputs( controller );

        // add ourselves to the new controller's listener list.
        _inputController->AddListener( this );
    }
}

//----------------------------------------------------------
void
GmCharacter::Move( const MVec3& displacement )
{
    gPhSubsys->MoveChar( _physicsController, displacement );
}

//----------------------------------------------------------
void
GmCharacter::Update()
{
    if ( _model )
    {
        // build the character's transform.
        MMat4x4 transform( _model->GetWorld() );
        transform.SetTranslate( gPhSubsys->GetCharPosition( _physicsController ) );
        transform.SetRotate( _rot );

        // upload it to the model.
        _model->SetWorld( transform );
        _model->Update();
    }
}

//----------------------------------------------------------
void
GmCharacter::OnInputsChanged( GmInputController* controller )
{
    bool update = false;

    if ( GmInput::IsDirty( _inputs->vecMove ) )
    {
        // move the character.
        Move( _inputs->vecMove->GetVector() );
        update = true;
    }

    if ( GmInput::IsDirty( _inputs->vecRot ) )
    {
        // set the character's rotation.
        MVec3 rot = _inputs->vecRot->GetVector();
        SetRot( MMat3x3( rot( 0 ), rot( 1 ), rot( 2 ) ) );
        update = true;
    }

    // if the character should update, then do so.
    if ( update )
    {
        Update();
    }
}

//----------------------------------------------------------
void
GmCharacter::OnControllerShutdown( GmInputController* controller )
{
    // free the controller inputs container 
    delete _inputs;
    _inputs = 0;

    // remove the old controller.
    if ( _inputController )
    {
        _inputController = 0;
    }
}
