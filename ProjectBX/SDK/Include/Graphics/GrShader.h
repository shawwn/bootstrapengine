//----------------------------------------------------------
// File:		GrShader.h
// Author:		Kevin Bray
// Created:		02-04-06
//
// Purpose:		To load and manage a shader.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"
#include "UFastArray.h"
#include "URefCount.h"
#include "GLSubsys.h"
#include "UPath.h"

//**********************************************************
// class GrShader
//**********************************************************
class GrShader : public URefCount
{
public:
	static const unsigned int kMaxUserParams = 128;
	static const unsigned int kMaxUserSamplers = 32;

	// engine driven parameters.
	enum EPARAM
	{
		// vertex params.
		EP_MODEL_VIEW_PROJECTION_MATRIX,
		EP_MODEL_VIEW_MATRIX,
		EP_LOCAL_VIEW_POSITION,
		EP_LOCAL_LIGHT_POSITION,
		EP_LOCAL_LIGHT_DIR_OVERRIDE,	// { x, y, z, 0 = override, 1 = no override }
		EP_LIGHT_PROJECTION_MATRIX,		// 4x4 projection matrix.
		EP_SHADOW_PROJECTION_MATRIX,	// 4x4 projection matrix.
		EP_TEXTURE_MATRIX,				// 4x2 projection matrix.
		EP_CUR_RENDER_TEXTURE_MATRIX,
		EP_ENV_TEXGEN_TEXTURE_MATRIX,
		EP_VERTEX_COLOR_MOD,
		EP_VERTEX_COLOR_ADD,

		// fragment params.
		EP_SHADOW_NEAR_CLIP,
		EP_LIGHT_COLOR,
		EP_LIGHT_RANGE,					// { r, r, r, 1/r }
		EP_LIGHT_ATTEN,					// { a^2, a, 0, c }
		EP_AMBIENT_COLOR,
		EP_BUMP_MOD,
		EP_BUMP_ADD,
		EP_DIFFUSE_MOD,
		EP_DIFFUSE_ADD,
		EP_SPECULAR_MOD,
		EP_SPECULAR_ADD,
		EP_AMBIENT_MOD,
		EP_AMBIENT_ADD,
		EP_EMISSIVE_MOD,
		EP_EMISSIVE_ADD,
		EP_LIGHT_WIDTH,					// { w, w, -w, 0 }
		EP_LIGHT_HAZE,					// { h, 1-h, 0, 0 }
		EP_FOG_START,
		EP_FOG_RANGE,
		EP_FOG_COLOR,
		EP_COUNT,
	};

	GrShader( const UPath& name, const char* vSource, const char* fSource );
	~GrShader();

	const UPath&		GetName() const				{	return _name;		}

	// rebuilds the shader.  Returns true if the source compiled successfully.  On
	// failure, the error shader is used.
	bool				Rebuild( const tstring& vSource, const tstring& fSource );

	// returns true or false if a sampler is used by the shader.
	bool				IsGLSamplerUsed( unsigned int texUnit );
	bool				IsGLSamplerUsed( EGRQUALITY quality, unsigned int texUnit );

	// binds a shader sampler to the sampler passed in.  The sampler will perform
	// all texture fetches from that sampler.  Valid values are [0..15].  This
	// function will return false if no valid samplers are available.
	bool				BindUserSampler( const char* name, unsigned int texUnit );

	// this needs to be called after *all* user samplers have been bound to a
	// texture unit.
	void				Validate();

	// returns the index of the attribute specified.
	unsigned int		GetAttribIndex( const char* name );
	unsigned int		GetAttribIndex( EGRQUALITY quality, const char* name );

	// user params.
	unsigned int		GenUserParamHandle();
	bool				InitUserParam( const char* name, unsigned int handle );
	unsigned int		GetUserParamHandle( const char* name );
	void				SetUserParam4fv( unsigned int handle, const float* data );

	// renderer driven params.
	void				SetEngineParam4fv( EPARAM param, const float* data );
	void				SetEngineParam4x2fv( EPARAM paramStart, const float* data );
	void				SetEngineParam4x3fv( EPARAM paramStart, const float* data );
	void				SetEngineParam4x4fv( EPARAM paramStart, const float* data );

	// make the shader current.
	void				Bind( EGRQUALITY quality );

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

private:
	struct SUserParam
	{
		SUserParam()
		{
			for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
				UniformXfvARB[ i ] = 0;
		}

		tstring				name;
		unsigned int		handles[ EGQ_COUNT ];
		void				(*UniformXfvARB[ EGQ_COUNT ])( int location, GLsizei count, const float *value );
	};
	struct SUserSampler
	{
		tstring				name;
		unsigned int		texUnit;
	};

	void				Build();
	bool				BuildProgram( const char* vsText, const char* fsText, tstring& errorMsg, EGRQUALITY quality );
	void				Clean();
	void				FreeShaders();
	bool				CompileShader( unsigned int shader, unsigned int strCount, const char** srcStrs, tstring& errorMsg );
	void				GetShaderInfoLog( unsigned int shader, tstring& str );
	void				GetProgramInfoLog( unsigned int program, tstring& str );
	void				InitEngineParam( EGRQUALITY quality, EPARAM target, const char* name, unsigned int glType );
	void				InitEngineSampler( EGRQUALITY quality, const char* name, unsigned int texUnit, unsigned int glType );
	void				PostError( const char* fmt, ... );
	bool				GetUserParamInfo( SUserParam* param );
	void				SetUserSamplerUniform( const char* name, unsigned int texUnit );
	void				SetSamplerUsed( EGRQUALITY quality, unsigned int texUnit );
	void				Validate( EGRQUALITY quality );
	void				BuildErrorShader( EGRQUALITY quality );

	UPath				_name;
	tstring				_vSource;
	tstring				_fSource;
	unsigned int		_glShader;
	unsigned int		_glShaders[ EGQ_COUNT ];
	unsigned int		_glVertShaders[ EGQ_COUNT ];
	unsigned int		_glFragShaders[ EGQ_COUNT ];
	int					_engineParamHandles[ EGQ_COUNT ][ EP_COUNT ];
	SUserParam			_userParams[ kMaxUserParams ];
	SUserSampler		_userSamplers[ kMaxUserSamplers ];
	unsigned int		_samplersUsed[ EGQ_COUNT ];
	EGRQUALITY			_quality;
	bool				_needValidate[ EGQ_COUNT ];
};
