//----------------------------------------------------------
// File:		GrAfterEffect.h
// Author:		Kevin Bray
// Created:		01-04-04
//
// Purpose:		To contain information about after effects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "MVec3.h"
#include "URef.h"
#include "UFastArray.h"
#include "UExpr.h"
#include "GrTextureStage.h"

class UExpr;
class GrShader;

//**********************************************************
// class GrAfterEffect
//**********************************************************
class GrAfterEffect
{
public:
	// units 0..7 are for use by the effect.  Units after that are reserved by
	// the system.
	static const unsigned int kMaxSamplerCount = 8;

	GrAfterEffect( const tstring& name, URef< GrShader > shader, const MVec3& position, float width, float queryWidth );
	~GrAfterEffect();

	void				SetPosition( const MVec3& position )		{	_position = position;		}
	void				SetWidth( float width )						{	_width = width;				}
	void				SetQueryWidth( float queryWidth )			{	_queryWidth = queryWidth;	}

	const MVec3&		GetPosition() const							{	return _position;			}
	float				GetWidth() const							{	return _width;				}
	float				GetQueryWidth() const						{	return _queryWidth;			}

	// parameter management.
	unsigned int		InitParam( const char* name, const URef< UExpr >* values );
	unsigned int		GetParamHandle( const char* name ) const;
	void				SetParamExprs( unsigned int handle, const URef< UExpr >* values );	// 4 exprs.
	void				GetParamExprs( unsigned int handle, URef< UExpr >* values );		// 4 exprs.

	// sampler management.
	unsigned int		InitSampler( const char* name, const GrTextureStage& textureStage );
	unsigned int		GetSamplerHandle( const char* name ) const;
	const GrTextureStage&	GetSamplerStage( unsigned int handle ) const;
	void				SetSamplerStage( unsigned int handle, const GrTextureStage& stage );

	// updates the effect.
	void				Update();

	// state management.
	void				ApplyShaderParams();
	void				ApplySamplerStates();

private:
	// vec4 parameter.
	struct SUserParam
	{
		SUserParam() { memset( cache, 0, sizeof( cache ) ); }
		SUserParam( const tstring& newName )
			: name( newName ) { memset( cache, 0, sizeof( cache ) ); }

		tstring	name;
		URef< UExpr > expr[ 4 ];
		float cache[ 4 ];
	};
	typedef UFastArray< SUserParam > ParamArray;

	// sampler parameter.
	struct SUserSampler
	{
		SUserSampler() { }

		tstring name;
		GrTextureStage stage;
	};
	typedef UFastArray< SUserSampler > SamplerArray;

	tstring				_name;
	URef< GrShader >	_shader;
	ParamArray			_params;
	SamplerArray		_samplers;
	MVec3				_position;		// world space position.
	float				_width;			// width in world units.
	float				_queryWidth;	// occlusion query width in world units.
};
