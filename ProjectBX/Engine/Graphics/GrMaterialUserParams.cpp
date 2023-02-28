//----------------------------------------------------------
// File:        GrMaterialUserParams.cpp
// Author:      Kevin Bray
// Created:     01-04-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMaterialUserParams.h"

// user samplers.
#define USER_SAMPLER_START          11
#define USER_SAMPLER_END            15

//**********************************************************
// class GrMaterialUserParams
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMaterialUserParams::GrMaterialUserParams()
: _shader( 0 )
, _userParamCount( 0 )
, _userSamplerCount( 0 )
{

}

//----------------------------------------------------------
GrMaterialUserParams::~GrMaterialUserParams()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMaterialUserParams::Copy( const GrMaterialUserParams* params )
{
    // get the shader.
    _shader = params->_shader;

    // remove all user parameters.
    _userParamCount = params->_userParamCount;
    _userSamplerCount = params->_userSamplerCount;

    // copy all user params.
    for ( unsigned int i = 0; i < _userParamCount; ++i )
        _userParams[ i ] = params->_userParams[ i ];

    // copy all user samplers.
    for ( unsigned int i = 0; i < _userSamplerCount; ++i )
        _userSamplers[ i ] = params->_userSamplers[ i ];

    // reset the shader.
    BindParamsToShader();
}

//----------------------------------------------------------
void
GrMaterialUserParams::SetShader( URef< GrShader > shader )
{
    // simply return if there isn't anything to do.
    if ( _shader == shader )
        return;

    // store the shader and bind all params to it.
    _shader = shader;
    BindParamsToShader();
}

//----------------------------------------------------------
URef< GrShader >
GrMaterialUserParams::GetShader() const
{
    return _shader;
}

//----------------------------------------------------------
unsigned int
GrMaterialUserParams::AddUserParam( const char* paramName, URef< UExprTerm >* values )
{
    // make sure a shader has been assigned.
    B_ASSERT( _shader != 0 );

    // if the parameter has already been added, simply set it's value.
    for ( unsigned int i = 0; i < _userParamCount; ++i )
    {
        if ( strcmp( _userParams[ i ].name.c_str(), paramName ) == 0 )
        {
            SetUserParam( i, values );
            return i;
        }
    }

    if ( _userParamCount == kMaxUserParams )
        return ~0;

    // get the handle from the shader.  Assign a new handle if none exists.
    unsigned int handle = ~0;
    if ( _shader )
    {
        handle = _shader->GetUserParamHandle( paramName );
        if ( handle == ~0 )
            handle = _shader->GenUserParamHandle();
    }

    _userParams[ _userParamCount ].name = paramName;
    _userParams[ _userParamCount ].handle = handle;
    _userParams[ _userParamCount ].flags = kUsed | kDynamic;
    _userParams[ _userParamCount ].valueExprs[ 0 ] = values[ 0 ];
    _userParams[ _userParamCount ].valueExprs[ 1 ] = values[ 1 ];
    _userParams[ _userParamCount ].valueExprs[ 2 ] = values[ 2 ];
    _userParams[ _userParamCount ].valueExprs[ 3 ] = values[ 3 ];

    // return the index the user param was added at and then increment
    // _userParamCount.
    return _userParamCount++;
}

//----------------------------------------------------------
unsigned int
GrMaterialUserParams::GetUserParamHandle( const char* paramName )
{
    for ( unsigned int i = 0; i < _userParamCount; ++i )
    {
        if ( strcmp( _userParams[ i ].name.c_str(), paramName ) == 0 )
            return i;
    }
    return ~0;
}

//----------------------------------------------------------
void
GrMaterialUserParams::SetUserParam( unsigned int handle, URef< UExprTerm >* values )
{
    // make sure that the handle passed in is valid.
    B_ASSERT( handle < _userParamCount );
    _userParams[ handle ].flags = kUsed | kDynamic;
    _userParams[ handle ].valueExprs[ 0 ] = values[ 0 ];
    _userParams[ handle ].valueExprs[ 1 ] = values[ 1 ];
    _userParams[ handle ].valueExprs[ 2 ] = values[ 2 ];
    _userParams[ handle ].valueExprs[ 3 ] = values[ 3 ];
}

//----------------------------------------------------------
void
GrMaterialUserParams::SetUserParam( unsigned int handle, float* values )
{
    B_ASSERT( handle < _userParamCount );
    _userParams[ handle ].flags = kUsed;
    _userParams[ handle ].values[ 0 ] = values[ 0 ];
    _userParams[ handle ].values[ 1 ] = values[ 1 ];
    _userParams[ handle ].values[ 2 ] = values[ 2 ];
    _userParams[ handle ].values[ 3 ] = values[ 3 ];
}

//----------------------------------------------------------
unsigned int
GrMaterialUserParams::AddUserSampler( const char* samplerName, const GrTextureStage& texture )
{
    // make sure a valid texture was passed in.
    B_ASSERT( samplerName != 0 );

    // if the sampler has already been added, simply set it's value.
    for ( unsigned int i = 0; i < _userSamplerCount; ++i )
    {
        if ( strcmp( _userSamplers[ i ].name.c_str(), samplerName ) == 0 )
        {
            SetUserSampler( i, texture );
            return i;
        }
    }

    // if too many samplers are set, return ~0.
    if ( _userSamplerCount == kMaxUserSamplers )
        return ~0;

    // determine what texture unit to use.
    unsigned int texUnit = USER_SAMPLER_START;
    for ( ; texUnit <= USER_SAMPLER_END; ++texUnit )
        if ( !_shader->IsGLSamplerUsed( texUnit ) )
            break;

    // fill out the user sampler info.
    _userSamplers[ _userSamplerCount ].name = samplerName;
    _userSamplers[ _userSamplerCount ].texUnit = texUnit;
    _userSamplers[ _userSamplerCount ].texture = texture;

    // bind the user sampler in the shader.
    if ( texUnit <= USER_SAMPLER_END )
        _shader->BindUserSampler( _userSamplers[ _userSamplerCount].name.c_str(), texUnit );

    // return the index the user param was added at and then increment
    // _userParamCount.
    return _userSamplerCount++; 
}

//----------------------------------------------------------
unsigned int
GrMaterialUserParams::GetUserSamplerHandle( const char* samplerName )
{
    // try to find the sampler and return it's handle.
    B_ASSERT( samplerName != 0 );
    for ( unsigned int i = 0; i < _userSamplerCount; ++i )
        if ( strcmp( _userSamplers[ i ].name.c_str(), samplerName ) == 0 )
            return i;

    // if we couldn't find the sampler, return ~0.
    return ~0;
}

//----------------------------------------------------------
void
GrMaterialUserParams::SetUserSampler( unsigned int handle, const GrTextureStage& texture )
{
    // make sure the params are valid.
    B_ASSERT( handle < _userSamplerCount );

    // set the user sampler's texture.
    _userSamplers[ handle ].texture = texture;
}

//----------------------------------------------------------
void
GrMaterialUserParams::Update()
{
    // go through all user params and update the shader.
    for ( unsigned int i = 0; i < _userParamCount; ++i )
    {
        // get the current param.
        SUserParam& param = _userParams[ i ];

        // update the param.
        if ( ( param.flags & kUsed ) != 0 && ( param.flags & kDynamic ) != 0 )
        {
            B_ASSERT( param.valueExprs[ 0 ] != 0 );
            B_ASSERT( param.valueExprs[ 1 ] != 0 );
            B_ASSERT( param.valueExprs[ 2 ] != 0 );
            B_ASSERT( param.valueExprs[ 3 ] != 0 );

            param.values[ 0 ] = param.valueExprs[ 0 ]->GetValue();
            param.values[ 1 ] = param.valueExprs[ 1 ]->GetValue();
            param.values[ 2 ] = param.valueExprs[ 2 ]->GetValue();
            param.values[ 3 ] = param.valueExprs[ 3 ]->GetValue();
        }
    }
}

//----------------------------------------------------------
void
GrMaterialUserParams::ApplyUserParams() const
{
    // simply return if no shader is attached.
    if ( !_shader )
        return;

    // go through all user params and update the shader.
    for ( unsigned int i = 0; i < _userParamCount; ++i )
    {
        // send the current param to the shader.
        if ( ( _userParams[ i ].flags & kUsed ) != 0 && _userParams[ i ].handle != ~0 )
            _shader->SetUserParam4fv( _userParams[ i ].handle, _userParams[ i ].values );
    }
}

//----------------------------------------------------------
void
GrMaterialUserParams::ApplyUserSamplers() const
{
    // simply return if no shader is attached.
    if ( !_shader )
        return;

    // go through all user samplers and bind them to the correct texture unit.
    for ( unsigned int i = 0; i < _userSamplerCount; ++i )
    {
        // send the current param to the shader.
        if ( _userSamplers[ i ].texUnit <= USER_SAMPLER_END )
            _userSamplers[ i ].texture.BindTexture( _userSamplers[ i ].texUnit );
    }
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMaterialUserParams::BindParamsToShader()
{
    // set the shader.
    if ( !_shader )
        return;

    // now get or register any user parameter handles from the shader.
    for ( unsigned int i = 0; i < _userParamCount; ++i )
    {
        // find a free handle for the user param.
        unsigned int handle = ~0;
        handle = _shader->GetUserParamHandle( _userParams[ i ].name.c_str() );
        if ( handle == ~0 )
            handle = _shader->GenUserParamHandle();

        // store the handle to the parameter.
        _userParams[ i ].handle = handle;
    }

    // allocate user samplers.
    for ( unsigned int i = 0; i < _userSamplerCount; ++i )
    {
        // find a free sampler.
        unsigned int texUnit = USER_SAMPLER_START;
        for ( ; texUnit <= USER_SAMPLER_END; ++texUnit )
            if ( !_shader->IsGLSamplerUsed( texUnit ) )
                break;

        // assign the texture unit.
        _userSamplers[ i ].texUnit = texUnit;
        if ( texUnit <= USER_SAMPLER_END )
            _shader->BindUserSampler( _userSamplers[ i ].name.c_str(), texUnit );
    }
}
