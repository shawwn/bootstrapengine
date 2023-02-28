//----------------------------------------------------------
// File:		GrAnimPlayer.h
// Author:		Kevin Bray
// Created:		04-08-06
//
// Purpose:		To manage and play animations.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrAnimXFormCache.h"
#include "UFastArray.h"

// forward declarations.
class GrAnim;

//**********************************************************
// class GrAnimPlayer
//**********************************************************
class GrAnimPlayer
{
public:
	GrAnimPlayer( GrModel* target );
	~GrAnimPlayer();

	enum ESTATE
	{
		ES_STOPPED,
		ES_PLAYING,
		ES_PAUSED,
	};

	// clones the animation player.
	GrAnimPlayer*		Clone( GrModel* newTarget ) const;

	// returns the animation mix ID.
	unsigned int		AddAnim( const tstring& name, GrAnim* animRoot );
	unsigned int		GetAnimId( const char* name );
	GrAnim*				GetAnim( unsigned int id );

	// play, pause, etc.
	void				SetActiveAnim( unsigned int id, float fadeDuration = 0.0f );
	void				Play();
	void				Pause();
	void				Stop();

	ESTATE				GetState() const					{	return _state;				}

	unsigned int		GetCurAnimId() const				{	return _curAnimId;			}
	unsigned int		GetPrevAnimId()	const				{	return _prevAnimId;			}
	GrAnim*				GetCurAnim() const					{	return _curAnim;			}
	GrAnim*				GetPrevAnim() const					{	return _prevAnim;			}

	// operations possible on the currently set animation.
	void				SeekCurTo( float time );

	// update the current active mix(es).
	void				ApplyPose( float timeAdvance );

	// gets a reference to the transform cache.
	GrAnimXFormCache&	GetTransformCache()					{	return _cache;				}

private:
	typedef UFastArray< GrAnim*, 16, 16 > AnimArray;
	typedef UFastArray< tstring, 16, 16 > StringArray;

	GrAnimXFormCache	_cache;
	GrAnim*				_curAnim;
	GrAnim*				_prevAnim;
	unsigned int		_curAnimId;
	unsigned int		_prevAnimId;
	AnimArray			_anims;
	StringArray			_animNames;
	float				_curFadeTime;
	float				_fadeDuration;
//	float				_globalWeight;
//	float				_globalSpeed;
	ESTATE				_state;			// kStopped..kPaused.
};
