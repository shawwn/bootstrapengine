//----------------------------------------------------------
// File:		GrAnim.cpp
// Author:		Kevin Bray
// Created:		04-04-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrAnim.h"

//**********************************************************
// class GrAnim
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrAnim::GrAnim( const tstring& name, float blendWeight, float speed )
: _player( 0 )
, _blendWeight( blendWeight )
, _speed( speed )
, _parent( 0 )
, _name( name )
{

}

//----------------------------------------------------------
GrAnim::~GrAnim()
{
	// delete children.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		delete[] _children[ i ];
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrAnim*
GrAnim::Clone() const
{
	// create a new animation and clone it's children.
	GrAnim* anim = new GrAnim( _name, _blendWeight, _speed );
	CloneChildren( anim );

	return anim;
}

//----------------------------------------------------------
void
GrAnim::SetPlayer( GrAnimPlayer* player )
{
	// change the owning player if necessary.
	if ( player != _player )
	{
		OnPlayerChange( player );
		_player = player;

		// recurse.
		for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
			_children[ i ]->SetPlayer( player );
	}
}

//----------------------------------------------------------
void
GrAnim::AddChild( GrAnim* child )
{
	// add the animation to our child list.
	_children.Push( child );

	// change the parent.
	if ( child->_parent )
		child->_parent->RemoveChild( child );
	child->_parent = this;

	// set the player.
	child->SetPlayer( _player );
}

//----------------------------------------------------------
unsigned int
GrAnim::GetChildCount() const
{
	return _children.GetElemCount();
}

//----------------------------------------------------------
GrAnim*
GrAnim::GetChild( unsigned int idx )
{
	B_ASSERT( idx < _children.GetElemCount() );
	return _children[ idx ];
}

//----------------------------------------------------------
void
GrAnim::RemoveChild( GrAnim* child )
{
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
	{
		if ( _children[ i ] == child )
			_children.Erase( i );
	}
}

//----------------------------------------------------------
void
GrAnim::SeekTo( float time )
{
	OnSeekTo( time );

	// recurse.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->OnSeekTo( time );
}

//----------------------------------------------------------
float
GrAnim::GetDuration()
{
	if ( _speed < 0.001f )
		return 0.0f;

	float invSpeed = 1.0f / _speed;

	// since we don't have any animation data, simply return 0.0f for our
	// duration.
	float duration = OnGetDuration();

	// get the maximum duration of children.
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		duration = Max( duration, invSpeed * _children[ i ]->GetDuration() );

	// return our duration.
	return duration;
}

//----------------------------------------------------------
void
GrAnim::ApplyPose( float timeAdvance, float parentBlendWeight )
{
	B_ASSERT( _player != 0 );

	// no need to do anything if we have no influence over the result.
	if ( _blendWeight < 0.001f )
		return;

	// update the children.
	float actualTimeAdvance = _speed * timeAdvance;
	float actualBlendWeight = parentBlendWeight * _blendWeight;
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		_children[ i ]->ApplyPose( actualTimeAdvance, actualBlendWeight );

	// update our animation.
	OnApplyPose( actualTimeAdvance, actualBlendWeight );
}


//==========================================================
// protected methods
//==========================================================

//----------------------------------------------------------
void
GrAnim::CloneChildren( GrAnim* parent ) const
{
	for ( unsigned int i = 0; i < _children.GetElemCount(); ++i )
		parent->AddChild( _children[ i ]->Clone() );
}
