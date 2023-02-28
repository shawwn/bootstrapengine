//----------------------------------------------------------
// File:		GrAfterEffect.cpp
// Author:		Kevin Bray
// Created:		01-04-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrAfterEffect.h"

// project includes.
#include "GrShader.h"
#include "GrTextureBase.h"
#include "GLSubsys.h"


//**********************************************************
// class GrAfterEffect
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrAfterEffect::GrAfterEffect( const tstring& name, URef< GrShader > shader, const MVec3& position, float width, float queryWidth )
: _name( name )
, _shader( shader )
, _samplers( kMaxSamplerCount )
, _position( position )
, _width( width )
, _queryWidth( queryWidth )
{
	B_ASSERT( _shader != 0 );
}

//----------------------------------------------------------
GrAfterEffect::~GrAfterEffect()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
GrAfterEffect::InitParam( const char* name, const URef< UExpr >* values )
{
	// ensure that incoming values are valid.
	B_ASSERT( name != 0 );
	B_ASSERT( values != 0 );

	// determine a good unique ID for the param.
	unsigned int handle = _params.GetElemCount();

	// if the param doesn't exist, simply return ~0.
	if ( !_shader->InitUserParam( name, handle ) )
		return ~0;

	// initialize param data.
	SUserParam& param = _params.Push();
	param.name = name;
	for ( unsigned int i = 0; i < 4; ++i )
	{
		B_ASSERT( values[ i ] != 0 );
		param.expr[ i ] = values[ i ];
		param.cache[ i ] = 0.0f;
	}

	// the param exists, so return the handle.
	return handle;
}

//----------------------------------------------------------
unsigned int
GrAfterEffect::GetParamHandle( const char* name ) const
{
	// ensure that a valid name was passed in.
	B_ASSERT( name != 0 );

	// find the parameter with the name specified.
	const unsigned int paramCount = _params.GetElemCount();
	for ( unsigned int i = 0; i < paramCount; ++i )
	{
		if ( strcmp( _params[ i ].name.c_str(), name ) == 0 )
			return i;
	}

	// if the parameter wasn't found, simply return ~0.
	return ~0;
}

//----------------------------------------------------------
void
GrAfterEffect::SetParamExprs( unsigned int handle, const URef< UExpr >* values )
{
	// ensure that the parameters passed in are valid.
	B_ASSERT( handle < _params.GetElemCount() );
	B_ASSERT( values != 0 );

	// set the expressions.
	for ( unsigned int i = 0; i < 4; ++i )
	{
		B_ASSERT( values[ i ] != 0 );
		_params[ handle ].expr[ i ] = values[ i ];
	}
}

//----------------------------------------------------------
void
GrAfterEffect::GetParamExprs( unsigned int handle, URef< UExpr >* values )
{
	// ensure that the parameters passed in are valid.
	B_ASSERT( handle < _params.GetElemCount() );
	B_ASSERT( values != 0 );

	// get the expressions.
	for ( unsigned int i = 0; i < 4; ++i )
		values[ i ] = _params[ handle ].expr[ i ];
}

//----------------------------------------------------------
unsigned int
GrAfterEffect::InitSampler( const char* name, const GrTextureStage& textureStage )
{
	// ensure that the parameters passed in are valid.
	B_ASSERT( name != 0 );
	B_ASSERT( textureStage.GetTexture() != 0 );
	B_ASSERT( _samplers.GetElemCount() < kMaxSamplerCount );

	// add a new sampler.
	unsigned int handle = _samplers.GetElemCount();

	// try to bind the user sampler.  If we can't, then simply return ~0.
	if ( !_shader->BindUserSampler( name, handle ) )
		return ~0;

	// fill out the sampler data.
	SUserSampler& sampler = _samplers.Push();

	// set the sampler's name and texture stage data.
	sampler.name = name;
	sampler.stage = textureStage;

	// return the handle.
	return handle;
}

//----------------------------------------------------------
unsigned int
GrAfterEffect::GetSamplerHandle( const char* name ) const
{
	// ensure that a valid name was passed in.
	B_ASSERT( name != 0 );

	// find the sampler with the name specified.
	const unsigned int samplerCount = _samplers.GetElemCount();
	for ( unsigned int i = 0; i < samplerCount; ++i )
	{
		if ( strcmp( _samplers[ i ].name.c_str(), name ) == 0 )
			return i;
	}

	// if the sampler wasn't found, simply return ~0.
	return ~0;
}

//----------------------------------------------------------
const GrTextureStage&
GrAfterEffect::GetSamplerStage( unsigned int handle ) const
{
	// ensure that a valid handle was passed in.
	B_ASSERT( handle < _samplers.GetElemCount() );
	return _samplers[ handle ].stage;
}

//----------------------------------------------------------
void
GrAfterEffect::SetSamplerStage( unsigned int handle, const GrTextureStage& stage )
{
	// ensure that a valid handle was passed in.
	B_ASSERT( handle < _samplers.GetElemCount() );
	_samplers[ handle ].stage = stage;
}

//----------------------------------------------------------
void
GrAfterEffect::Update()
{
	// spin through parameters and update.
	const unsigned int paramCount = _params.GetElemCount();
	for ( unsigned int i = 0; i < paramCount; ++i )
	{
		// get the current parameter and then evaluate and cache it's expressions.
		SUserParam& curParam = _params[ i ];
		for ( unsigned int j = 0; j < 4; ++j )
		{
			B_ASSERT( curParam.expr[ j ] );
			curParam.cache[ j ] = curParam.expr[ j ]->GetValue();
		}
	}
}

//----------------------------------------------------------
void
GrAfterEffect::ApplyShaderParams()
{
	const unsigned int paramCount = _params.GetElemCount();
	for ( unsigned int i = 0; i < paramCount; ++i )
		_shader->SetUserParam4fv( i, _params[ i ].cache );
}

//----------------------------------------------------------
void
GrAfterEffect::ApplySamplerStates()
{
	const unsigned int samplerCount = _samplers.GetElemCount();
	for ( unsigned int i = 0; i < samplerCount; ++i )
		_samplers[ i ].stage.BindTexture( i );
}
