//----------------------------------------------------------
// File:        GmAI.cpp
// Author:      Shawn Presser
// Created:     12-21-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmAI.h"

// project headers.
#include "GmInputController.h"
#include "GmCharacter.h"
#include "GmConfig.h"

//----------------------------------------------------------
GmAI::GmAI( GmCharacter* character )
: _character( character )
, _controller( new GmInputController() )
, _vecMove( 0 )
, _vecRot( 0 )
, _destination( MVec3( 0.0f, 0.0f, 0.0f ) )
, _active( false )
{
    B_ASSERT( character );

    _vecMove = _controller->GetInput( "move", EIT_VECTOR );
    _vecRot = _controller->GetInput( "rotation", EIT_VECTOR );

    // possess the character.
    character->SetInputController( _controller );
}

//----------------------------------------------------------
GmAI::~GmAI()
{
    delete _controller;
}

//----------------------------------------------------------
void
GmAI::Update( unsigned int dt )
{
    float t = dt / 1000.0f;

    MVec3 charPos( _character->GetPos() );
    MVec3 dest( _destination );

    // ignore the Y-axis for now.
    charPos.SetY( 0.0f );
    dest.SetY( 0.0f );

    MVec3 displacement( 0.0f, 0.0f, 0.0f );
    if ( _active )
    {
        displacement = dest - charPos;
        displacement.Normalize();
        displacement *= t*_character->GetSpeed();

        float dist = charPos.Dist( dest );
        if ( dist < 0.25f )
        {
            _active = false;
        }
        else if ( displacement.Mag() > dist )
        {
            _active = false;
        }
    }

    // add gravity.
    displacement += t*GAME_GRAVITY;

    _vecMove->SetValue( displacement );
    _controller->Update();
}
