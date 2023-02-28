//----------------------------------------------------------
// File:        GrAnimPlayer.h
// Author:      Kevin Bray
// Created:     04-08-06
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrAnimPlayer.h"

// project includes.
#include "GrAnim.h"


//**********************************************************
// class GrAnimPlayer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrAnimPlayer::GrAnimPlayer( GrModel* target )
: _cache( target )
, _curAnim( 0 )
, _prevAnim( 0 )
, _curAnimId( ~0 )
, _prevAnimId( ~0 )
, _curFadeTime( 0.0f )
, _fadeDuration( 0.0f )
, _state( ES_STOPPED )
{

}

//----------------------------------------------------------
GrAnimPlayer::~GrAnimPlayer()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrAnimPlayer*
GrAnimPlayer::Clone( GrModel* newTarget ) const
{
    // create a new animation player and clone the animation trees.
    GrAnimPlayer* newAnimPlayer = new GrAnimPlayer( newTarget );
    for ( unsigned int i = 0; i < _anims.GetElemCount(); ++i )
    {
        newAnimPlayer->_anims.Push( _anims[ i ]->Clone() );
        newAnimPlayer->_animNames.Push( _animNames[ i ] );
    }

    // return the new animation player.
    return newAnimPlayer;
}

//----------------------------------------------------------
unsigned int
GrAnimPlayer::AddAnim( const tstring& name, GrAnim* animRoot )
{
    B_ASSERT( animRoot != 0 );
    B_ASSERT( _anims.GetElemCount() == _animNames.GetElemCount() );
    _anims.Push( animRoot );
    _animNames.Push( name );
    animRoot->SetPlayer( this );
    return ( _anims.GetElemCount() - 1 );
}

//----------------------------------------------------------
unsigned int
GrAnimPlayer::GetAnimId( const char* name )
{
    for ( unsigned int i = 0; i < _animNames.GetElemCount(); ++i )
    {
        if ( strcmp( name, _animNames[ i ].c_str() ) == 0 )
            return i;
    }
    return ~0;
}

//----------------------------------------------------------
GrAnim*
GrAnimPlayer::GetAnim( unsigned int id )
{
    B_ASSERT( id < _anims.GetElemCount() );
    return _anims[ id ];
}

//----------------------------------------------------------
void
GrAnimPlayer::SetActiveAnim( unsigned int id, float fadeDuration )
{
    // modify state.
    _prevAnim = _curAnim;
    _prevAnimId = _curAnimId;
    _curAnim = GetAnim( id );
    _curAnimId = id;

    // set fade state.
    _fadeDuration = fadeDuration;
    _curFadeTime = 0.0f;
}

//----------------------------------------------------------
void
GrAnimPlayer::Play()
{
    _state = ES_PLAYING;
}

//----------------------------------------------------------
void
GrAnimPlayer::Pause()
{
    _state = ES_PAUSED;
}

//----------------------------------------------------------
void
GrAnimPlayer::Stop()
{
    _state = ES_STOPPED;
    _curAnim->SeekTo( 0.0f );
    if ( _prevAnim && _curFadeTime < _fadeDuration )
        _prevAnim->SeekTo( 0.0f );
}

//----------------------------------------------------------
void
GrAnimPlayer::SeekCurTo( float time )
{
    if ( _curAnim )
        _curAnim->SeekTo( time );
}

//----------------------------------------------------------
void
GrAnimPlayer::ApplyPose( float timeAdvance )
{
    if ( _curAnim )
    {
        float blendWeight = 1.0f;
        if ( _fadeDuration > 0.01 )
            blendWeight = Min( _curFadeTime / _fadeDuration, 1.0f );

        // set the blend weight on the current animation.
        _curAnim->ApplyPose( timeAdvance, blendWeight );

        // set the blend weight on the previous animation.
        if ( _prevAnim )
            _prevAnim->ApplyPose( timeAdvance, 1.0f - blendWeight );

        // apply animation data 
        _cache.ApplyToTarget();
    }
}
