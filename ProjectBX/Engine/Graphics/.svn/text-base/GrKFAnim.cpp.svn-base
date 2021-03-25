//----------------------------------------------------------
// File:		GrKFAnim.h
// Author:		Kevin Bray
// Created:		04-29-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrKFAnim.h"

// project includes.
#include "GrKFAnimData.h"
#include "GrAnimPlayer.h"
#include "UReader.h"
#include "UWriter.h"


//**********************************************************
// class GrKFAnim
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrKFAnim::GrKFAnim( const tstring& name, const UFastArray< URef< GrKFAnimData > >& kfData, float blendWeight, float speed )
: GrAnim( name, blendWeight, speed )
, _interpolators( 0 )
, _interpolatorCount( kfData.GetElemCount() )
, _duration( 0.0f )
, _time( 0.0f )
, _clamp( false )
{
	// create interpolators for the keyframe animation data passed in.
	_interpolators = new GrKFInterpolator[ _interpolatorCount ];
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ] = GrKFInterpolator( kfData[ i ] );
	CalcDuration();
}

//----------------------------------------------------------
GrKFAnim::GrKFAnim( UReader& reader )
: GrAnim( "" )
, _interpolators( 0 )
, _interpolatorCount( 0 )
, _duration( 0.0f )
, _time( 0.0f )
, _clamp( false )
{
	Load( reader );
}

//----------------------------------------------------------
GrKFAnim::GrKFAnim( const tstring& name, GrKFInterpolator* interpolators, unsigned int interpolatorCount, float blendWeight, float speed )
: GrAnim( name, blendWeight, speed )
, _interpolators( 0 )
, _interpolatorCount( interpolatorCount )
, _duration( 0.0f )
, _time( 0.0f )
, _clamp( false )
{
	_interpolators = new GrKFInterpolator[ interpolatorCount ];
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ] = interpolators[ i ];
	CalcDuration();
}

//----------------------------------------------------------
GrKFAnim::~GrKFAnim()
{
	Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrAnim*
GrKFAnim::Clone() const
{
	GrAnim* anim = new GrKFAnim( GetName(), _interpolators, _interpolatorCount, GetBlendWeight(), GetSpeed() );
	CloneChildren( anim );
	return anim;
}

//----------------------------------------------------------
void
GrKFAnim::Save( UWriter& writer )
{
	// write out the header and version.
	writer.WriteUInt( *( ( unsigned int *)"KANM" ) );
	writer.WriteUInt( 0x00010000U );

	// write out the name.
	writer.WriteString( GetName() );

	// write out the duration and the clamp flag.
	writer.WriteFloat( _duration );
	writer.WriteInt( _clamp ? 1 : 0 );

	// write out the number of per-target animations to be saved.
	writer.WriteInt( _interpolatorCount );

	// save out the animation data for each target.
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ].GetKFData()->Save( writer );
}

void
GrKFAnim::Load( UReader& reader )
{
	Clean();

	// check the tag and version.
	unsigned int tag = reader.ReadUInt();
	unsigned int ver = reader.ReadUInt();
	B_ASSERT( tag == *( ( unsigned int *)"KANM" ) );
	B_ASSERT( ver == 0x00010000U );

	// read in the name.
	SetName( reader.ReadString() );

	// read in the duration and the clamp fag.
	_duration = reader.ReadFloat();
	_clamp = ( reader.ReadInt() != 0 );

	// read in the number of per-target animations saved in the file.
	_interpolatorCount = reader.ReadUInt();
	_interpolators = new GrKFInterpolator[ _interpolatorCount ];

	// read in per-target animation data.
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ].Init( new GrKFAnimData( reader ) );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrKFAnim::CalcDuration()
{
	_duration = 0.0f;
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_duration = Max( _interpolators[ i ].GetKFData()->GetDuration(), _duration );
}

//----------------------------------------------------------
void
GrKFAnim::OnApplyPose( float timeAdvance, float blendWeight )
{
	if ( _duration > 0 )
	{
		if ( _clamp )
		{
			_time = _duration;
		}
		else
		{
			_time = Fract( _time / _duration ) * _duration;
			_time += timeAdvance;
		}
	}

	// apply.
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ].ApplyPose( _time, blendWeight, _duration, _clamp );
}

//----------------------------------------------------------
void
GrKFAnim::OnPlayerChange( GrAnimPlayer* player )
{
	for ( unsigned int i = 0; i < _interpolatorCount; ++i )
		_interpolators[ i ].SetOutput( player != 0 ? &player->GetTransformCache() : 0 );
}

//----------------------------------------------------------
void
GrKFAnim::OnSeekTo( float time )
{
	_time = time;
}

//----------------------------------------------------------
float
GrKFAnim::OnGetDuration()
{
	return _duration;
}

//----------------------------------------------------------
void
GrKFAnim::Clean()
{
	delete[] _interpolators;
}
