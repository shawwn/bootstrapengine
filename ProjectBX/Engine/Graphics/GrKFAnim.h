//----------------------------------------------------------
// File:		GrKFAnim.h
// Author:		Kevin Bray
// Created:		04-29-06
//
// Purpose:		To manage a set of keyframe interpolators associated with
//				named output targets.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "GrAnim.h"

// project includes.
#include "UFastArray.h"
#include "GrKFInterpolator.h"

// forward declarations.
class UWriter;
class UReader;

//**********************************************************
// class GrKFAnim
//**********************************************************
class GrKFAnim : public GrAnim
{
public:
	GrKFAnim( const tstring& name, const UFastArray< URef< GrKFAnimData > >& kfData, float blendWeight = 1.0f, float speed = 1.0f );
	GrKFAnim( UReader& reader );
	~GrKFAnim();

	void				SetClamp( bool clamp )		{	_clamp = clamp;		}
	bool				GetClamp() const			{	return _clamp;		}

	GrAnim*				Clone() const;

	// saves out animation data.
	void				Save( UWriter& writer );
	void				Load( UReader& reader );

private:
	typedef UFastArray< GrKFInterpolator > InterpolatorArray;

	GrKFAnim( const tstring& name, GrKFInterpolator* interpolators, unsigned int interpolatorCount, float blendWeight, float speed );
	GrKFAnim( const GrKFAnim& ) : GrAnim( "invalid" ) { B_ASSERT( false ); }
	GrKFAnim&			operator = ( const GrKFAnim& ) { B_ASSERT( false ); return *this; }

	void				CalcDuration();

	// implementation callbacks.
	void				OnApplyPose( float timeAdvance, float blendWeight );
	void				OnPlayerChange( GrAnimPlayer* player );
	void				OnSeekTo( float time );
	float				OnGetDuration();
	void				Clean();

	GrKFInterpolator*	_interpolators;
	unsigned int		_interpolatorCount;
	float				_duration;
	float				_time;
	bool				_clamp;
};
