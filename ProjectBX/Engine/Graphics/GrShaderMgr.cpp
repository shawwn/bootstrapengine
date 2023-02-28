//----------------------------------------------------------
// File:        GrGLProgMgr.h
// Author:      Kevin Bray
// Created:     04-13-04
//
// Purpose:     To manage OpenGL vertex/fragment programs.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrShaderMgr.h"

// project includes.
#include "GrShader.h"
#include "GrSubsys.h"
#include "GrUtil.h"
#include "RFileMgr.h"
#include "RFile.h"

// GL includes.
#include "GLSubsys.h"

GrShaderMgr* gGrShaderMgr = 0;

static const char kErrorShaderVS[] =
"attribute vec3 a_Position;\n"
"uniform mat4 u_ModelViewProjectionMatrix;\n"
"void main()\n"
"{\n"
"\tgl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );\n"
"}\n"
;

static const char kErrorShaderFS[] =
"void main()\n"
"{\n"
"\tgl_FragColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );\n"
"}\n"
;

static bool ReloadHandler( const tstring& command );

//**********************************************************
// class GrShaderMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrShaderMgr::GrShaderMgr()
: _errorShader( 0 )
{
    B_ASSERT( gGrShaderMgr == 0 );
    gGrShaderMgr = this;

    // register our console commands.
    gGrSubsys->AddCommandHandler( "vidReloadShaders", ReloadHandler );
}

//----------------------------------------------------------
GrShaderMgr::~GrShaderMgr()
{
    _shaders.clear();
    gGrShaderMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrShader >
GrShaderMgr::GetShader( const UPath& name )
{
    B_ASSERT( gRFileMgr != 0 );

    // get the path string.
    tstring strName = name.GetPathString();

    // find the shader or create it if it doesn't already exist.
    URef< GrShader > shader = 0;
    ShaderMap::iterator iter = _shaders.find( name );
    if ( iter == _shaders.end() )
    {
        // files.
        URef< RFile > vsFile;
        URef< RFile > fsFile;

        // load the shader and add it to the shader map.
        if ( LoadShaderFiles( vsFile, fsFile, name ) )
        {
            // build a new shader and cache it.
            shader = new GrShader( name, ( const char* )vsFile->GetData(), ( const char* )fsFile->GetData() );
            _shaders[ name ] = SShaderEntry( shader, true );
        }
        else
        {
            // the shader failed to load, so file a complaint with the bugfixer.
            B_ASSERT( !"A shader failed to load." );
        }
    }
    else
    {
        // the shader already exists.
        shader = iter->second.shader;
    }

    // return the shader.
    return shader;
}

//----------------------------------------------------------
URef< GrShader >
GrShaderMgr::GetShader( const UPath& name, const char* vertexShader, const char* fragmentShader )
{
    // find the shader or create it if it doesn't already exist.
    URef< GrShader > shader = 0;
    ShaderMap::iterator iter = _shaders.find( name );
    if ( iter == _shaders.end() )
    {
        // create the shader and add it to the shader map.
        shader = new GrShader( name, vertexShader, fragmentShader );
        _shaders[ name ] = SShaderEntry( shader, false );
    }
    else
    {
        // the shader already exists.
        shader = iter->second.shader;
    }

    // return the shader.
    return shader;
}

//----------------------------------------------------------
URef< GrShader >
GrShaderMgr::FindShader( const UPath& name )
{
    // find the shader or create it if it doesn't already exist.
    ShaderMap::iterator iter = _shaders.find( name );
    if ( iter == _shaders.end() )
        return 0;
    return iter->second.shader;
}

//----------------------------------------------------------
void
GrShaderMgr::ReloadShaders()
{
    URef< RFile > vsFile;
    URef< RFile > fsFile;
    ShaderMap::iterator iter = _shaders.begin();
    const ShaderMap::iterator end = _shaders.end();
    for ( ; iter != end; ++iter )
    {
        if ( iter->second.fromFile )
        {
            if ( LoadShaderFiles( vsFile, fsFile, iter->second.shader->GetName() ) )
                iter->second.shader->Rebuild( ( const char* )vsFile->GetData(), ( const char* )fsFile->GetData() );
        }
    }
}

//----------------------------------------------------------
void
GrShaderMgr::PreContextReset()
{
    ShaderMap::iterator iter = _shaders.begin();
    const ShaderMap::iterator end = _shaders.end();
    for ( ; iter != end; ++iter )
        iter->second.shader->PreContextReset();
}

//----------------------------------------------------------
void
GrShaderMgr::PostContextReset()
{
    // clear the currently bound program object.
    GrBindShader( 0 );

    // notify all shaders that the reset has completed.
    ShaderMap::iterator iter = _shaders.begin();
    const ShaderMap::iterator end = _shaders.end();
    for ( ; iter != end; ++iter )
        iter->second.shader->PostContextReset();
}

//----------------------------------------------------------
void
GrShaderMgr::BindErrorShader()
{
    _errorShader->Bind( EGQ_LOW );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
GrShaderMgr::LoadShaderFiles( URef< RFile >& vsFile, URef< RFile >& fsFile, const UPath& shaderName )
{
    // determine the vertex and fragment shader file paths.
    UPath vFilePath = GetFilePath( shaderName, ".vsh" );
    UPath fFilePath = GetFilePath( shaderName, ".fsh" );

    // load the vertex shader.
    vsFile = gRFileMgr->GetFile( vFilePath, RFileMgr::kFlagRead );
    if ( vsFile == 0 )
    {
        PrintF( "The vertex shader %s could not be loaded.  (Is the working directory valid?)", vFilePath.GetPathString().c_str() );
        return false;
    }

    // load the fragment shader.
    fsFile = gRFileMgr->GetFile( fFilePath, RFileMgr::kFlagRead );
    if ( fsFile == 0 )
    {
        PrintF( "The fragment shader %s could not be loaded.  (Is the working directory valid?)", fFilePath.GetPathString().c_str() );
        return false;
    }

    // reload the file from disk to make sure we get any changes to
    // the file's contents.
    vsFile->Reload();
    fsFile->Reload();

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
UPath
GrShaderMgr::GetFilePath( const UPath& shaderName, const tstring& ext )
{
    UPath path( GR_PATH_SHADERS );
    path += ( shaderName.GetPathString() + ext );
    return path;
}


//**********************************************************
// File Local Functions
//**********************************************************

//----------------------------------------------------------
bool ReloadHandler( const tstring& command )
{
    gGrShaderMgr->ReloadShaders();
    PrintF( "Shaders reloaded successfully." );
    return true;
}
