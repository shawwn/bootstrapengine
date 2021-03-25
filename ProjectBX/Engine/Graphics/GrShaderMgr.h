//----------------------------------------------------------
// File:		GrShaderMgr.h
// Author:		Kevin Bray
// Created:		02-04-06
//
// Purpose:		To manage GrShader objects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <map>

// forward declarations.
class GrShader;
class UPath;
class RFile;

//**********************************************************
// class GrShaderMgr
//**********************************************************
class GrShaderMgr
{
public:
	GrShaderMgr();
	~GrShaderMgr();

	// finds/builds a shader.  Note that this is the *name*, not the path.
	// this will be turned into: GR_PATH_SHARED"shaders/" + name + ".shd"
	URef< GrShader >	GetShader( const UPath& name );

	// returns a custom shader.  It will build the shader from the strings
	// passed in if the shader does not currently exist.
	URef< GrShader >	GetShader( const UPath& name, const char* vertexShader, const char* fragmentShader );

	// finds an already created shader.
	URef< GrShader >	FindShader( const UPath& name );

	// reloads all disk-based shaders.
	void				ReloadShaders();

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

	// makes the error shader the active shader.
	void				BindErrorShader();

private:
	struct SShaderEntry
	{
		SShaderEntry() : shader( 0 ), fromFile( false ) { }
		SShaderEntry( URef< GrShader > _shader, bool _fromFile ) : shader( _shader ), fromFile( _fromFile ) { }

		URef< GrShader > shader;
		bool fromFile;
	};

	typedef std::map< UPath, SShaderEntry > ShaderMap;

	bool				LoadShaderFiles( URef< RFile >& vsFile, URef< RFile >& fsFile, const UPath& shaderName );
	UPath				GetFilePath( const UPath& shaderName, const tstring& ext );

	ShaderMap			_shaders;
	URef< GrShader >	_errorShader;
};
extern GrShaderMgr* gGrShaderMgr;
