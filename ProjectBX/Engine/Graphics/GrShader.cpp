//----------------------------------------------------------
// File:        GrShader.cpp
// Author:      Kevin Bray
// Created:     02-04-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrShader.h"

// project includes.
#include "GrShaderMgr.h"
#include "GLSubsys.h"
#include "GrUtil.h"

// std C++ includes.
#include <stdarg.h>
#include <stdlib.h>

#define GLSL_VERSION_STRINGIZE2( x )    #x
#define GLSL_VERSION_STRINGIZE( x )     GLSL_VERSION_STRINGIZE2( x )
#define GLSL_VERSION_STRING             "#version "GLSL_VERSION_STRINGIZE( GR_GLSL_SHADER_VERSION )"\n"

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

//**********************************************************
// class GrShader
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrShader::GrShader( const UPath& name, const char* vSource, const char* fSource )
: _name( name )
, _vSource( vSource )
, _fSource( fSource )
, _glShader( 0 )
, _quality( EGQ_COUNT )
{
    MemSet( _glShaders, 0, sizeof( _glShaders ) );
    MemSet( _glVertShaders, 0, sizeof( _glVertShaders ) );
    MemSet( _glFragShaders, 0, sizeof( _glFragShaders ) );
    MemSet( _needValidate, 1, sizeof( _needValidate ) );
    MemSet( _samplersUsed, 0, sizeof( _samplersUsed ) );
    Build();
}

//----------------------------------------------------------
GrShader::~GrShader()
{
    Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrShader::Rebuild( const tstring& vSource, const tstring& fSource )
{
    // set the source and build the shader.
    _vSource = vSource;
    _fSource = fSource;
    Build();

    // check to see if any errors occured.
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        // return false to indicate an error if necessary.
        if ( !_glShaders[ i ] )
            return false;
    }

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
GrShader::IsGLSamplerUsed( unsigned int texUnit )
{
    // check all user samplers for the GL sampler passed in.
    B_ASSERT( texUnit < 16 );

    // check all quality levels for the given sampler.
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
        if ( IsGLSamplerUsed( ( EGRQUALITY )i, texUnit ) )
            return true;

    // return false if the sampler isn't used by any quality levels.
    return false;
}

//----------------------------------------------------------
bool
GrShader::IsGLSamplerUsed( EGRQUALITY quality, unsigned int texUnit )
{
    // check all user samplers for the GL sampler passed in.
    B_ASSERT( texUnit < 16 );

    // determine whether or not the sampler is used.
    unsigned int bit = ( 1 << texUnit );
    return ( _samplersUsed[ quality ] & bit ) != 0;
}

//----------------------------------------------------------
bool
GrShader::BindUserSampler( const char* name, unsigned int texUnit )
{
    CHECK_GL();
    B_ASSERT( texUnit < 16 );

    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
        SetSamplerUsed( ( EGRQUALITY )i, texUnit );

    // check to see if we're reassigning a sampler.
    unsigned int s = 0;
    for ( ; s < kMaxUserSamplers; ++s )
    {
        // are we reassigning a sampler?
        if ( strcmp( _userSamplers[ s ].name.c_str(), name ) == 0 )
        {
            // performance warning: this is not a cheap thing to do.
            // This should only be done when a sampler changes!
            _userSamplers[ s ].texUnit = texUnit;
            break;
        }
    }

    // if we're not reassigning a sampler, check for a free one.
    if ( s == kMaxUserSamplers )
    {
        for ( s = 0; s < kMaxUserSamplers; ++s )
        {
            if ( _userSamplers[ s ].name.length() == 0 )
            {
                // store the user sampler information.
                _userSamplers[ s ].name = name;
                _userSamplers[ s ].texUnit = texUnit;
                break;
            }
        }
    }

    // make sure that a user sampler slot was available.  If not,
    // we should return false to indicate this case to the caller.
    if ( s == kMaxUserSamplers )
        return false;

    SetUserSamplerUniform( name, texUnit );

    // success!
    return true;
}

//----------------------------------------------------------
void
GrShader::Validate()
{
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
        Validate( ( EGRQUALITY )i );
}

//----------------------------------------------------------
unsigned int
GrShader::GetAttribIndex( const char* name )
{
    return bglGetAttribLocation( _glShader, name );
}

//----------------------------------------------------------
unsigned int
GrShader::GetAttribIndex( EGRQUALITY quality, const char* name )
{
    return bglGetAttribLocation( _glShaders[ quality ], name );
}

//----------------------------------------------------------
unsigned int
GrShader::GenUserParamHandle()
{
    // look for the last free handle.
    for ( unsigned int i = kMaxUserParams-1; i >= 0; --i )
    {
        if ( _userParams[ i ].name.length() == 0 )
            return i;
    }

    // no free handles!
    return ~0;
}

//----------------------------------------------------------
bool
GrShader::InitUserParam( const char* name, unsigned int handle )
{
    B_ASSERT( handle < kMaxUserParams );
    B_ASSERT( strlen( name ) > 0 );

    // check to see if the name is already used!
    for ( unsigned int i = 0; i < kMaxUserParams; ++i )
    {
        // parameter has already been bound!
        if ( strcmp( _userParams[ i ].name.c_str(), name ) == 0 )
        {
            PrintF( "Warning: parameter %s in shader %s has already been bound to a handle; ignoring initialize call.\n",
                name, _name.GetPathString().c_str() );
            return false;
        }
    }

    if ( _userParams[ handle ].name.length() > 0 )
    {
        PrintF( "Warning: Attempt to bind parameter %s in shader %s to a handle already being used by parameter %s; ignoring initialize call.\n",
            name, _name.GetPathString().c_str(), _userParams[ handle ].name.c_str() );
        return false;
    }

    // add a new user param.
    SUserParam* param = _userParams + handle;
    param->name = name;

    bool found = GetUserParamInfo( param );
    if ( !found )
        PrintF( "Warning: Parameter %s in shader %s was not found.\n", name, _name.GetPathString().c_str() );

    // success!
    return true;
}

//----------------------------------------------------------
unsigned int
GrShader::GetUserParamHandle( const char* name )
{
    // check to see if the name is already used!
    for ( unsigned int i = 0; i < kMaxUserParams; ++i )
    {
        // parameter has already been bound, so return it's id!
        if ( strcmp( _userParams[ i ].name.c_str(), name ) == 0 )
            return i;
    }

    // no param, so return ~0.
    return ~0;
}

//----------------------------------------------------------
void
GrShader::SetUserParam4fv( unsigned int handle, const float* data )
{
    // ensure the GL is okay.
    CHECK_GL();

    // the program must be bound before any of it's parameters can be updated!
    B_ASSERT( handle < kMaxUserParams );
    B_ASSERT( GrGetShader() == _glShader );
    B_ASSERT( strlen( _userParams[ handle ].name.c_str() ) > 0 );   // was the param ever initialized?

    // ignore the call if the parameter is not supported by the current shader configuration.
    if ( _userParams[ handle ].UniformXfvARB[ _quality ] == 0 )
        return;

    // update the parameter's value.
    _userParams[ handle ].UniformXfvARB[ _quality ]( _userParams[ handle ].handles[ _quality ], 1, data );

    // if there is an error here, check to ensure you're calling the right function!
    // the parameter's type must match the function called!
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::SetEngineParam4fv( EPARAM param, const float* data )
{
    // ensure the GL is okay.
    CHECK_GL();

    // if the param doesn't exist, simply return.
    if ( _engineParamHandles[ _quality ][ param ] == -1 )
        return;

    // the program must be bound before any of it's parameters can be updated!
    B_ASSERT( GrGetShader() == _glShader );

    // update the parameter's value.
    bglUniform4fv( _engineParamHandles[ _quality ][ param ], 1, data );

    // if there is an error here, check to ensure you're calling the right function!
    // the parameter's type must match the function called!
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::SetEngineParam4x2fv( EPARAM param, const float* data )
{
    // ensure the GL is okay.
    CHECK_GL();

    // if the param doesn't exist, simply return.
    if ( _engineParamHandles[ _quality ][ param ] == -1 )
        return;

    // the program must be bound before any of it's parameters can be updated!
    B_ASSERT( GrGetShader() == _glShader );

    // update the parameter's value.
    bglUniformMatrix4x2fv( _engineParamHandles[ _quality ][ param ], 1, GL_TRUE, data );

    // if there is an error here, check to ensure you're calling the right function!
    // the parameter's type must match the function called!
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::SetEngineParam4x3fv( EPARAM param, const float* data )
{
    // ensure the GL is okay.
    CHECK_GL();

    // if the param doesn't exist, simply return.
    if ( _engineParamHandles[ _quality ][ param ] == -1 )
        return;

    // the program must be bound before any of it's parameters can be updated!
    B_ASSERT( GrGetShader() == _glShader );

    // update the parameter's value.
    bglUniformMatrix4x3fv( _engineParamHandles[ _quality ][ param ], 1, GL_TRUE, data );

    // ensure the GL is okay.
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::SetEngineParam4x4fv( EPARAM param, const float* data )
{
    // ensure the GL is okay.
    CHECK_GL();

    // if the param doesn't exist, simply return.
    if ( _engineParamHandles[ _quality ][ param ] == -1 )
        return;

    // the program must be bound before any of it's parameters can be updated!
    B_ASSERT( GrGetShader() == _glShader );

    // update the parameter's value.
    bglUniformMatrix4fv( _engineParamHandles[ _quality ][ param ], 1, GL_TRUE, data );

    // ensure the GL is okay.
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::Bind( EGRQUALITY quality )
{
    if ( _needValidate[ quality ] )
        Validate( quality );

    _glShader = _glShaders[ quality ];
    _quality = quality;
    GrBindShader( _glShader );
}

//----------------------------------------------------------
void
GrShader::PreContextReset()
{
    // destroy the shader objects.
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        bglDeleteProgram( _glShaders[ i ] );
        bglDeleteShader( _glVertShaders[ i ] );
        bglDeleteShader( _glFragShaders[ i ] );
    }
}

//----------------------------------------------------------
void
GrShader::PostContextReset()
{
    // rebuild the shader using the current source.
    Build();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrShader::Build()
{
    CHECK_GL();

    // free all shaders.
    FreeShaders();

    // build a shader for each quality level.
    tstring errors;
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        // build the program.
        if ( !BuildProgram( _vSource.c_str(), _fSource.c_str(), errors, ( EGRQUALITY )i ) )
        {
            // if there was an error building the shader, simply fall
            // back to the error shader.
            BuildProgram( kErrorShaderVS, kErrorShaderFS, errors, ( EGRQUALITY )i );
        }

        // setup vertex shader params.
        InitEngineParam( ( EGRQUALITY )i, EP_MODEL_VIEW_PROJECTION_MATRIX,  GR_MODEL_VIEW_PROJECTION_MATRIX_NAME,   GL_FLOAT_MAT4 );
        InitEngineParam( ( EGRQUALITY )i, EP_MODEL_VIEW_MATRIX,         GR_MODEL_VIEW_MATRIX_NAME,          GL_FLOAT_MAT4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LOCAL_VIEW_POSITION,       GR_LOCAL_VIEW_POSITION_NAME,        GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LOCAL_LIGHT_POSITION,      GR_LOCAL_LIGHT_POSITION_NAME,       GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LOCAL_LIGHT_DIR_OVERRIDE,  GR_LOCAL_LIGHT_DIR_OVERRIDE_NAME,   GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_PROJECTION_MATRIX,   GR_LIGHT_PROJECTION_MATRIX_NAME,    GL_FLOAT_MAT4 );
        InitEngineParam( ( EGRQUALITY )i, EP_SHADOW_PROJECTION_MATRIX,  GR_SHADOW_PROJECTION_MATRIX_NAME,   GL_FLOAT_MAT4 );
        InitEngineParam( ( EGRQUALITY )i, EP_TEXTURE_MATRIX,            GR_TEXTURE_MATRIX_NAME,             GL_FLOAT_MAT4x2 );
        InitEngineParam( ( EGRQUALITY )i, EP_CUR_RENDER_TEXTURE_MATRIX, GR_CUR_RENDER_TEXTURE_MATRIX_NAME,  GL_FLOAT_MAT4x2 );
        InitEngineParam( ( EGRQUALITY )i, EP_ENV_TEXGEN_TEXTURE_MATRIX, GR_ENV_TEXGEN_TEXTURE_MATRIX_NAME,  GL_FLOAT_MAT4x3 );
        InitEngineParam( ( EGRQUALITY )i, EP_VERTEX_COLOR_MOD,          GR_VERTEX_COLOR_MOD_NAME,           GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_VERTEX_COLOR_ADD,          GR_VERTEX_COLOR_ADD_NAME,           GL_FLOAT_VEC4 );

        // setup fragment shader params.
        InitEngineParam( ( EGRQUALITY )i, EP_SHADOW_NEAR_CLIP,          GR_SHADOW_NEAR_CLIP_NAME,           GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_COLOR,               GR_LIGHT_COLOR_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_RANGE,               GR_LIGHT_RANGE_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_ATTEN,               GR_LIGHT_ATTEN_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_AMBIENT_COLOR,             GR_AMBIENT_COLOR_NAME,              GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_BUMP_MOD,                  GR_BUMP_MOD_NAME,                   GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_BUMP_ADD,                  GR_BUMP_ADD_NAME,                   GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_DIFFUSE_MOD,               GR_DIFFUSE_MOD_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_DIFFUSE_ADD,               GR_DIFFUSE_ADD_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_SPECULAR_MOD,              GR_SPECULAR_MOD_NAME,               GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_SPECULAR_ADD,              GR_SPECULAR_ADD_NAME,               GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_EMISSIVE_MOD,              GR_EMISSIVE_MOD_NAME,               GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_EMISSIVE_ADD,              GR_EMISSIVE_ADD_NAME,               GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_AMBIENT_MOD,               GR_AMBIENT_MOD_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_AMBIENT_ADD,               GR_AMBIENT_ADD_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_WIDTH,               GR_LIGHT_WIDTH_NAME,                GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_LIGHT_HAZE,                GR_LIGHT_HAZE_NAME,                 GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_FOG_START,                 GR_FOG_START_NAME,                  GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_FOG_RANGE,                 GR_FOG_RANGE_NAME,                  GL_FLOAT_VEC4 );
        InitEngineParam( ( EGRQUALITY )i, EP_FOG_COLOR,                 GR_FOG_COLOR_NAME,                  GL_FLOAT_VEC4 );

        // initialize samplers.
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_CUBE_NORMALIZE_NAME, GR_TEX_CUBE_NORMALIZE,              GL_SAMPLER_CUBE );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_SHADOW_VSM_CUBE_NAME,GR_TEX_SHADOW_VSM_CUBE,             GL_SAMPLER_CUBE );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_CUBE_FACE_X_NAME,    GR_TEX_CUBE_FACE_X,                 GL_SAMPLER_CUBE );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_CUBE_FACE_Y_NAME,    GR_TEX_CUBE_FACE_Y,                 GL_SAMPLER_CUBE );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_JITTER_NAME,         GR_TEX_JITTER,                      GL_SAMPLER_3D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_SHADOW_NAME,         GR_TEX_SHADOW,                      GL_SAMPLER_2D_SHADOW );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_PROJECTION_NAME,     GR_TEX_PROJECTION,                  GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_ATLAS_NAME,          GR_TEX_ATLAS,                       GL_SAMPLER_2D );

        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_BUMP_NAME,           GR_TEX_BUMP,                        GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_NORMAL_NAME,         GR_TEX_NORMAL,                      GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_DIFFUSE_NAME,        GR_TEX_DIFFUSE,                     GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_SPECULAR_NAME,       GR_TEX_SPECULAR,                    GL_SAMPLER_2D );

//      InitEngineSampler( ( EGRQUALITY )i, GR_TEX_REFLECTION_NAME,     GR_TEX_REFLECTION,                  GL_SAMPLER_2D );
//      InitEngineSampler( ( EGRQUALITY )i, GR_TEX_ENVMAP_NAME,         GR_TEX_ENVMAP,                      GL_SAMPLER_CUBE );

        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_CUR_RENDER_NAME,     GR_TEX_CUR_RENDER,                  GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_CUR_DEPTH_NAME,      GR_TEX_CUR_DEPTH,                   GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_STIPPLE_NAME,        GR_TEX_STIPPLE,                     GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_AMBIENT_NAME,        GR_TEX_AMBIENT,                     GL_SAMPLER_2D );
        InitEngineSampler( ( EGRQUALITY )i, GR_TEX_EMISSIVE_NAME,       GR_TEX_EMISSIVE,                    GL_SAMPLER_2D );

        CHECK_GL();
    }

    // re-initialize user parameters.
    for ( unsigned int i = 0; i < kMaxUserParams; ++i )
    {
        if ( _userParams[ i ].name.length() > 0 )
            GetUserParamInfo( _userParams + i );
    }

    // re-initialize user samplers.
    for ( unsigned int i = 0; i < kMaxUserSamplers; ++i )
    {
        if ( _userSamplers[ i ].name.length() > 0 )
            SetUserSamplerUniform( _userSamplers[ i ].name.c_str(), _userSamplers[ i ].texUnit );
    }

    // we need to re-validate the shader.
    MemSet( _needValidate, 1, sizeof( _needValidate ) );
}

//----------------------------------------------------------
bool
GrShader::BuildProgram( const char* vsText, const char* fsText, tstring& errorMsg, EGRQUALITY quality )
{
    // This is a string that gets prepended to each shader.
    char qualityLevel[10];
    const char* strTable[] = 
    {
        GLSL_VERSION_STRING,
        "#define B_QUALITY ",
        qualityLevel,
        "\n"
#if GR_OPTION_FRAGMENT_HALF
        "#if !defined __GLSL_CG_DATA_TYPES\n"
        "#define __GLSL_CG_DATA_TYPES\n"
#endif
        "#define half float\n"
        "#define half4 vec4\n"
        "#define half3 vec3\n"
        "#define half2 vec2\n"
#if GR_OPTION_FRAGMENT_HALF
        "#endif\n"
#endif
        "#extension GL_ARB_texture_rectangle : enable\n",
        "#line 1\n",
        0,
    };
    static const unsigned int kIndexCount = sizeof( strTable ) / sizeof( const char* );
    static const unsigned int kSrcIndex = kIndexCount - 1;

    // set the quality level.
    _itoa_s( quality, qualityLevel, 10, 10 );

    // compile the vertex shader.
    strTable[ kSrcIndex ] = vsText;
    unsigned int vertexShader = bglCreateShader( GL_VERTEX_SHADER );
    if ( !CompileShader( vertexShader, kIndexCount, strTable, errorMsg ) )
    {
        // delete the vertex shader and print an error.
        bglDeleteShader( vertexShader );
        PrintF( "Error compiling vertex shader...\n%s", errorMsg.c_str() );
        return 0;
    }

    // compile the fragment shader.
    strTable[ kSrcIndex ] = fsText;
    unsigned int fragmentShader = bglCreateShader( GL_FRAGMENT_SHADER );
    if ( !CompileShader( fragmentShader, kIndexCount, strTable, errorMsg ) )
    {
        bglDeleteShader( vertexShader );
        bglDeleteShader( fragmentShader );
        PrintF( "Error compiling fragment shader...\n%s", errorMsg.c_str() );
        return 0;
    }

    // create a program and setup shaders.
    unsigned int program = bglCreateProgram();
    bglAttachShader( program, vertexShader );
    bglAttachShader( program, fragmentShader );

    // bind attribute locations.
    bglBindAttribLocation( program,     GR_ATTRIB_POSITION_INDEX,       GR_ATTRIB_POSITION_NAME );
    bglBindAttribLocation( program,     GR_ATTRIB_TANGENT_INDEX,        GR_ATTRIB_TANGENT_NAME  );
    bglBindAttribLocation( program,     GR_ATTRIB_BINORMAL_INDEX,       GR_ATTRIB_BINORMAL_NAME );
    bglBindAttribLocation( program,     GR_ATTRIB_NORMAL_INDEX,         GR_ATTRIB_NORMAL_NAME   );
    bglBindAttribLocation( program,     GR_ATTRIB_TEXCOORD_INDEX,       GR_ATTRIB_TEXCOORD_NAME );
    bglBindAttribLocation( program,     GR_ATTRIB_COLOR_INDEX,          GR_ATTRIB_COLOR_NAME    );

    // link the program.
    int status = GL_TRUE;
    bglLinkProgram( program );
    bglGetProgramiv( program, GL_LINK_STATUS, &status );

    // get the programs info log and check for errors.
    tstring infoLog;
    GetProgramInfoLog( program, infoLog );
    if ( status == GL_FALSE )
    {
        bglDeleteShader( vertexShader );
        bglDeleteShader( fragmentShader );
        bglDeleteProgram( program );
        PostError( "Error linking shader...\n%s", infoLog.c_str() );
    }
    else if ( quality == EGQ_LOW && infoLog.length() > 2 )
        PrintF( "Shader %s compiled successfully.  Info:\n%s", _name.GetPathString().c_str(), infoLog.c_str() );

    // store the program.
    _glShaders[ quality ] = program;
    _glVertShaders[ quality ] = vertexShader;
    _glFragShaders[ quality ] = fragmentShader;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
void
GrShader::Clean()
{
    FreeShaders();
    for ( unsigned int i = 0; i < kMaxUserParams; ++i )
    {
        _userParams[ i ].name = "";
        for ( unsigned int j = 0; j < EGQ_COUNT; ++j )
            _userParams[ i ].UniformXfvARB[ j ] = 0;
    }

    for ( unsigned int i = 0; i < kMaxUserSamplers; ++i )
    {
        _userSamplers[ i ].name = "";
        _userSamplers[ i ].texUnit = 0;
    }
}

//----------------------------------------------------------
void
GrShader::FreeShaders()
{
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        if ( _glShaders[ i ] )
        {
            // free the current shader.
            bglDeleteProgram( _glShaders[ i ] );
            bglDeleteShader( _glVertShaders[ i ] );
            bglDeleteShader( _glFragShaders[ i ] );

            // clear the current shader handles.
            _glShaders[ i ] = 0;
            _glVertShaders[ i ] = 0;
            _glFragShaders[ i ] = 0;
        }
    }
}

//----------------------------------------------------------
bool
GrShader::CompileShader( unsigned int shader, unsigned int strCount, const char** srcStrs, tstring& errorMsg )
{
    int status = GL_TRUE;
    bglShaderSource( shader, strCount, srcStrs, NULL );
    bglCompileShader( shader );
    bglGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    if ( status == GL_FALSE )
    {
        GetShaderInfoLog( shader, errorMsg );
        return false;
    }
    return true;
}

//----------------------------------------------------------
void
GrShader::GetShaderInfoLog( unsigned int shader, tstring& str )
{
    GLsizei length = 0;
    bglGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
    char* infoLog = new char[ length + 1 ]; 
    bglGetShaderInfoLog( shader, length+1, &length, infoLog );
    str = infoLog;
    delete[] infoLog;
}

void
GrShader::GetProgramInfoLog( unsigned int program, tstring& str )
{
    GLsizei length = 0;
    bglGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
    char* infoLog = new char[ length + 1 ]; 
    bglGetProgramInfoLog( program, length+1, &length, infoLog );
    str = infoLog;
    delete[] infoLog;
}

//----------------------------------------------------------
void
GrShader::InitEngineParam( EGRQUALITY quality, EPARAM target, const char* name, unsigned int glType )
{
    // make sure the params passed in are valid!
    B_ASSERT( quality < EGQ_COUNT && target < EP_COUNT && name != 0 );

    // get the uniform's location.
    _engineParamHandles[ quality ][ target ] = bglGetUniformLocation( _glShaders[ quality ], name );
    if ( _engineParamHandles[ quality ][ target ] != -1 )
    {
        // check the param's type!
        GLint uniformSize = 0;
        GLenum type = 0;
        size_t uniformNameLen = strlen( name ) + 1;
        char* uniformName = ( char* )alloca( uniformNameLen );
        bglGetActiveUniform( _glShaders[ quality ], _engineParamHandles[ quality ][ target ], ( GLsizei )uniformNameLen,
            NULL, &uniformSize, &type, uniformName );
        B_ASSERT( uniformSize == 1 );
        B_ASSERT( type == glType );
        if ( type != glType )
            PostError( "Invalid engine parameter type specified for uniform %s!", name );

        if ( uniformSize != 1 )
            PostError( "Invalid uniform size encountered for uniform %s!", name );
    }

    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::InitEngineSampler( EGRQUALITY quality, const char* name, unsigned int texUnit, unsigned int glType )
{
    // make sure the params passed in are valid!
    B_ASSERT( quality < EGQ_COUNT && name != 0 );

    // get the uniform's location.
    int samplerLoc = bglGetUniformLocation( _glShaders[ quality ], name );
    if ( samplerLoc != -1 )
    {
        // mark the sampler as used.
        SetSamplerUsed( quality, texUnit );

        // check the sampler's type!
        GLint uniformSize = 0;
        GLenum type = 0;
        size_t uniformNameLen = strlen( name ) + 1;
        char* uniformName = ( char* )alloca( uniformNameLen );
        bglGetActiveUniform( _glShaders[ quality ], samplerLoc, ( GLsizei )uniformNameLen,
            NULL, &uniformSize, &type, uniformName );
        B_ASSERT( uniformSize == 1 );
        if ( type != glType )
        {
            // special case to allow a shadow sampler to be used with a 2D texture.
            if ( glType != GL_SAMPLER_2D && type != GL_SAMPLER_2D_SHADOW_ARB &&
                 glType != GL_SAMPLER_2D_RECT_ARB && type != GL_SAMPLER_2D_RECT_SHADOW_ARB )
                PostError( "Invalid engine sampler type specified for uniform %s!", name );
        }

        if ( uniformSize != 1 )
            PostError( "Invalid uniform size encountered for uniform %s!", name );

        // set the sampler's value.
        // get the previous shader.
        unsigned int prevShader = GrGetShader();

        // bind the sampler for the programs at each quality level.
        GrBindShader( _glShaders[ quality ] );
        bglUniform1i( samplerLoc, texUnit );

        // restore the previous shader.
        GrBindShader( prevShader );
        CHECK_GL();
    }
}

//----------------------------------------------------------
void
GrShader::PostError( const char* fmt, ... )
{
    // print the formatted string.
    va_list args;
    va_start( args, fmt );
#ifdef _MSC_VER
    unsigned int len = _vscprintf( fmt, args ) + 1;
    char* buf = new char[ len ];
    vsprintf_s( buf, len, fmt, args );
#else
    //// more portable version, should work on linux with glibc >= 2.1
    //char *buf = ( char * )alloca( 1 );
    //int bufSize = vsnprintf( buf, 1, fmt, args );
    ////vsnprintf leaves marker in undefined state, so reset it
    //va_end( args );
    //va_start( args, fmt );
    //buf = new char[ bufSize + 1 ];
    //vsnprintf( buf, bufSize+1, fmt, args );
    //va_end( args );
    tstring temp;
    VSPrintF( temp, fmt, args );
    const char *buf = temp.c_str();
#endif
    va_end( args );

    // add the shader name, etc.
    tstring errorMsg = "Error in shader '";
    errorMsg += _name.GetPathString();
    errorMsg += "'...\n\n";
    errorMsg += buf;

    // clean up after ourselves.
    delete buf;

    PrintF( errorMsg.c_str() );

    // post the error.
    B_ERROR( errorMsg.c_str() );
}

//----------------------------------------------------------
bool
GrShader::GetUserParamInfo( SUserParam* param )
{
    B_ASSERT( param != 0 );

    bool found = false;
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        // get information about the param.
        param->handles[ i ] = bglGetUniformLocation( _glShaders[ i ], param->name.c_str() );
        if ( param->handles[ i ] != ~0 )
        {
            found = true;
            static char nothing[1] = {0};
            int size = 0;
            GLenum type = 0;
            bglGetActiveUniform( _glShaders[ i ], param->handles[ i ], 0, 0, &size, &type,
                nothing );
            if ( size > 1 )
            {
                PrintF( "Warning: Array specified as shader parameter in shader '%s'; only array element 0 is accessible.\n",
                    _name.GetPathString().c_str() );
            }

            // find the function we need to upload data to the shader with.
            switch ( type )
            {
            case GL_FLOAT:
                param->UniformXfvARB[ i ] = bglUniform1fv;
                break;
            case GL_FLOAT_VEC2_ARB:
                param->UniformXfvARB[ i ] = bglUniform2fv;
                break;
            case GL_FLOAT_VEC3_ARB:
                param->UniformXfvARB[ i ] = bglUniform3fv;
                break;
            case GL_FLOAT_VEC4:
                param->UniformXfvARB[ i ] = bglUniform4fv;
                break;
            };
        }
    }
    return found;
}

//----------------------------------------------------------
void
GrShader::SetUserSamplerUniform( const char* name, unsigned int texUnit )
{
    // get the previous shader.
    unsigned int prevShader = GrGetShader();
    CHECK_GL();

    // bind the sampler for the programs at each quality level.
    for ( unsigned int i = 0; i < EGQ_COUNT; ++i )
    {
        int paramLoc = bglGetUniformLocation( _glShaders[ i ], name );
        CHECK_GL();
        if ( paramLoc == -1 )
            continue;
        GrBindShader( _glShaders[ i ] );
        bglUniform1i( paramLoc, texUnit );
        CHECK_GL();
    }

    // restore the previous shader.
    GrBindShader( prevShader );
    CHECK_GL();
}

//----------------------------------------------------------
void
GrShader::SetSamplerUsed( EGRQUALITY quality, unsigned int texUnit )
{
    _samplersUsed[ quality ] |= ( 1 << texUnit );
}

//----------------------------------------------------------
void
GrShader::Validate( EGRQUALITY quality )
{
    // simply return if we don't need to be validated.
    int status = GL_TRUE;
    if ( !_needValidate[ quality ] )
        return;

    // validate the program object.
    bglValidateProgram( _glShaders[ quality ] );
    bglGetProgramiv( _glShaders[ quality ], GL_VALIDATE_STATUS, &status );
    if ( status == GL_FALSE )
    {
        tstring errorMsg;
        GetProgramInfoLog( _glShaders[ quality ], errorMsg );
        PostError( "Error validating shader...\n%s", errorMsg.c_str() );
    }
    else
    {
        // clear the validation flag.
        _needValidate[ quality ] = false;
    }
}
