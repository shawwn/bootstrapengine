//----------------------------------------------------------
// File:		GrAnim.h
// Author:		Kevin Bray
// Created:		04-04-06
//
// Purpose:		To contain animation data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"

// forward declarations.
class GrAnimXFormCache;
class GrAnimPlayer;

//**********************************************************
// class GrAnim
//**********************************************************
class GrAnim
{
public:
	GrAnim( const tstring& name, float blendWeight = 1.0f, float speed = 1.0f );
	virtual ~GrAnim();

	virtual GrAnim*		Clone() const;

	const tstring&		GetName() const					{	return _name;			}

	// this is set when an animation is placed into a blend tree.
	void				SetPlayer( GrAnimPlayer* player );

	// note that this object takes ownership over animations passed in.
	void				AddChild( GrAnim* child );
	unsigned int		GetChildCount() const;
	GrAnim*				GetChild( unsigned int idx );
	void				RemoveChild( GrAnim* child );

	// mutators.
	void				SetBlendWeight( float weight )	{	_blendWeight = weight;	}
	void				SetSpeed( float speed )			{	_speed = speed;			}

	// accessors.
	float				GetBlendWeight() const			{	return _blendWeight;	}
	float				GetSpeed() const				{	return _speed;			}

	// sets the current time in animation playback.
	void				SeekTo( float time );

	// returns the total play time of the animation.
	float				GetDuration();

	// updates the target tree with animated transforms.
	void				ApplyPose( float timeAdvance, float parentBlendWeight = 1.0f );

protected:
	void				SetName( const tstring& name )	{	_name = name;			}
	void				CloneChildren( GrAnim* parent ) const;

	virtual void		OnApplyPose( float timeAdvance, float blendWeight ) { }
	virtual void		OnPlayerChange( GrAnimPlayer* player ) { }
	virtual void		OnSeekTo( float time ) { }
	virtual float		OnGetDuration() { return 0.0f; }

private:
	typedef UFastArray< GrAnim*, 16, 16 > AnimArray;

	GrAnim( const GrAnim& ) { B_ASSERT( false ); }
	GrAnim&				operator = ( const GrAnim& ) { B_ASSERT( false ); return *this; }

	GrAnimPlayer*		_player;
	GrAnim*				_parent;
	AnimArray			_children;
	float				_blendWeight;
	float				_speed;
	tstring				_name;
};
