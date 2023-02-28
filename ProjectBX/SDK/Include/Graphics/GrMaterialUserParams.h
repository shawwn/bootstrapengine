//----------------------------------------------------------
// File:        GrMaterialUserParams.h
// Author:      Kevin Bray
// Created:     01-04-07
//
// Purpose:     To manage user parameters in materials.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UExprTerm.h"
#include "GrTextureStage.h"
#include "GrTextureBase.h"
#include "GrShader.h"

//**********************************************************
// class GrMaterialUserParams
//**********************************************************
class GrMaterialUserParams
{
public:
    // constants.
    static const unsigned int kMaxUserParams = 128;
    static const unsigned int kMaxUserSamplers = 8;

    GrMaterialUserParams();
    ~GrMaterialUserParams();

    // copies from an existing object.
    void                    Copy( const GrMaterialUserParams* params );

    // rebuilds the user params.
    void                    SetShader( URef< GrShader > shader );
    URef< GrShader >        GetShader() const;

    // adds user parameters.
    unsigned int            AddUserParam( const char* paramName, URef< UExprTerm >* values );
    unsigned int            GetUserParamHandle( const char* paramName );
    void                    SetUserParam( unsigned int handle, URef< UExprTerm >* values );
    void                    SetUserParam( unsigned int handle, float* values );

    // adds user samplers.
    unsigned int            AddUserSampler( const char* samplerName, const GrTextureStage& texture );
    unsigned int            GetUserSamplerHandle( const char* samplerName );
    void                    SetUserSampler( unsigned int handle, const GrTextureStage& texture );

    // updates dynamic params.
    void                    Update();

    // applies user params/samplers.
    void                    ApplyUserParams() const;
    void                    ApplyUserSamplers() const;

private:
    // param flags.
    static const unsigned int kUsed = 0x01;
    static const unsigned int kDynamic = 0x02;

    // param structure.
    struct SUserParam
    {
        SUserParam() : handle( ~0 ) { }

        tstring                 name;
        unsigned int            flags;
        unsigned int            handle;
        float                   values[ 4 ];
        URef< UExprTerm >       valueExprs[ 4 ];
    };

    // sampler structure.
    struct SUserSampler
    {
        SUserSampler() : texUnit( ~0 ) { }

        tstring                 name;
        unsigned int            texUnit;
        GrTextureStage          texture;
    };

    // binds all of the params to the currently set shader.
    void                    BindParamsToShader();

    // the shader these params go to.
    URef< GrShader >        _shader;

    // user param array.
    SUserParam              _userParams[ kMaxUserParams ];
    unsigned int            _userParamCount;

    // user sampler array.
    SUserSampler            _userSamplers[ kMaxUserSamplers ];
    unsigned int            _userSamplerCount;
};
