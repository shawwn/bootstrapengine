//----------------------------------------------------------
// File:        GrMaterialPass.cpp
// Author:      Kevin Bray
// Created:     03-20-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMaterialPass.h"

// project includes.
#include "GrMaterial.h"
#include "GrCamera.h"
#include "GrMeshInst.h"
#include "GLSubsys.h"
#include "GrMaterialMgr.h"
#include "BscLexer.h"
#include "UExprTerm.h"
#include "UExprMgr.h"
#include "UExprNumber.h"
#include "GrTextureCube.h"
#include "GrLight.h"
#include "GrScene.h"
#include "GrUtil.h"
#include "GrFramebuffer.h"
#include "GrShaderMgr.h"

static const unsigned int kStateMask = ( GR_ALPHATEST | GR_DEPTHTEST | GR_DEPTHBIAS | GR_CULLFACE | GR_BLEND );

SMaterialPassBlendStates::SMaterialPassBlendStates()
: srcColorBlendFactor( EBM_ONE )
, dstColorBlendFactor( EBM_ZERO )
, srcAlphaBlendFactor( EBM_ONE )
, dstAlphaBlendFactor( EBM_ZERO )
{

}

SMaterialPassBlendStates::~SMaterialPassBlendStates() { }


SMaterialPassInfo::SMaterialPassInfo()
: vertexColorMod( 1.0f )
, vertexColorAdd( 0.0f )
, minLightResponse( 0.0f )
, maxLightResponse( 100.0f )
, cullMode( ECM_BACK )
, alphaTestMode( EAM_LESSEQUAL )
, alphaTestRef( 0 )
, enableState( GR_DEPTHTEST | GR_CULLFACE )
, passEnable( 0 )
, slopeBias( 0.0f )
, constBias( 0.0f )
, writeMask( GR_RGBA | GR_DEPTH )
, receiveShadow( true )
, castShadow( true )
{
    shader = gGrShaderMgr->FindShader( GR_SHADER_DEFAULT_LIGHT );
    ambientShader = gGrShaderMgr->FindShader( GR_SHADER_DEFAULT_AMBIENT );
    alphaTestRef = gUExprMgr->CreateNumber( 1.0f );
    passEnable = gUExprMgr->CreateNumber( 1.0f );
}

SMaterialPassInfo::~SMaterialPassInfo() { }


// overdraw test.
//#define _WORLD_OVERDRAW_TEST

//**********************************************************
// class GrMaterialPass
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMaterialPass::GrMaterialPass( unsigned int passNumber, const SMaterialPassInfo& passInfo )
: _name( "" )
, _userParams()
, _cullMode( ECM_BACK )
, _alphaTestMode( EAM_LESSEQUAL )
//, _userParamCount( 0 )
, _enableState( 0 )
, _cachedAlphaTestRef( 0.0f )
, _slopeDepthBias( 0.0f )
, _constDepthBias( 0.0f )
, _receiveShadow( true )
, _castShadow( true )
, _firstPass( false )
, _writeMask( GR_DEPTH | GR_RGBA )
{
    // initialize the pass.
    Init( passNumber, passInfo );
}

//----------------------------------------------------------
GrMaterialPass::~GrMaterialPass()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMaterialPass::Init( unsigned int passNumber, const SMaterialPassInfo& passInfo )
{
    char buf[ 12 ];
    MemSet( buf, 0, sizeof( buf ) );

    // initialize the pass.
    _name = _itoa_s( passNumber, buf, 12, 10 );

    // create a new user param block.
    _userParams = GrMaterialUserParams();
    _userParams.SetShader( passInfo.shader );

    _ambBlendStates = passInfo.ambBlendStates;
    _lightBlendStates = passInfo.lightBlendStates;
    _cullMode = passInfo.cullMode;
    _alphaTestMode = passInfo.alphaTestMode;
    _alphaTestRef = ( passInfo.alphaTestRef != 0 ) ? passInfo.alphaTestRef : gUExprMgr->CreateNumber( 0.5f );
    _receiveShadow = passInfo.receiveShadow;
    _castShadow = passInfo.castShadow;
    _firstPass = ( passNumber == 0 );
    _shader = passInfo.shader;
    _ambientShader = passInfo.ambientShader;
    _passEnable = passInfo.passEnable;

    _textures = passInfo.textures;

    _vertexColorMod = passInfo.vertexColorMod;
    _vertexColorAdd = passInfo.vertexColorAdd;

    _minLightResponse = passInfo.minLightResponse;
    _maxLightResponse = passInfo.maxLightResponse;

    _slopeDepthBias = passInfo.slopeBias;
    _constDepthBias = passInfo.constBias;

    _writeMask = passInfo.writeMask;

    // determine what states we need to enable/disable for rendering.
    _enableState = passInfo.enableState & kStateMask;
    _disableState = ~passInfo.enableState & kStateMask;

    // make sure we don't have any overlapping states.  This shouldn't be
    // possible given the intended logic.
    B_ASSERT( ( _enableState & _disableState ) == 0 );
}

//----------------------------------------------------------
GrMaterialPass*
GrMaterialPass::Clone() const
{
    // it just so happens we can use the copy constructor.  It is recommended that
    // this function is used instead of the default copy constructor since there is
    // no way to make the default copy constructor private.
    GrMaterialPass* newMatPass = new GrMaterialPass( *this );

    // return the new material pass.
    return newMatPass;
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageTexture( ESTAGE stage, URef< GrTextureBase > texture )
{
    _textures.SetStageTexture( stage, texture );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageAddressMode( ESTAGE stage, GrTextureStage::EADDRESSMODE s, GrTextureStage::EADDRESSMODE t )
{
    _textures.SetStageAddressMode( stage, s, t );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageFilterMode( ESTAGE stage, GrTextureStage::EFILTERMODE filterMode )
{
    _textures.SetStageFilterMode( stage, filterMode );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageMaxAnisotropy( ESTAGE stage, float maxAnisotropy )
{
    _textures.SetStageMaxAnisotropy( stage, maxAnisotropy );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageMipMapEnable( ESTAGE stage, bool mipMapEnable )
{
    _textures.SetStageMipMapEnable( stage, mipMapEnable );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageGenTextureFlag( ESTAGE stage, GrTextureStage::GenTextureFlag flags )
{
    _textures.SetStageGenTextureFlag( stage, flags );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageTexColorMod( ESTAGE stage, const GrColor& color )
{
    _textures.SetStageTexColorMod( stage, color );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStageTexColorAdd( ESTAGE stage, const GrColor& color )
{
    _textures.SetStageTexColorAdd( stage, color );
}

//----------------------------------------------------------
void
GrMaterialPass::SetStates( unsigned int states )
{
    _enableState = states;
    _enableState &= kStateMask;
    _disableState = ~states;
    _disableState &= kStateMask;
}

//----------------------------------------------------------
void
GrMaterialPass::Update()
{
    // update parameters.
    _userParams.Update();

    // update the textures.
    _textures.Update();

    // update the alpha test reference value.
    _cachedAlphaTestRef = _alphaTestRef->GetValue();
    _cachedPassEnable = _passEnable->GetValue() > 0.5f;
}

//----------------------------------------------------------
void
GrMaterialPass::ApplyAmbientBlendStates( unsigned int stateMask ) const
{
    ApplyBlendStates( _ambBlendStates, stateMask );
}

//----------------------------------------------------------
void
GrMaterialPass::ApplyLightBlendStates( unsigned int stateMask ) const
{
    ApplyBlendStates( _lightBlendStates, stateMask );
}

//----------------------------------------------------------
void
GrMaterialPass::ApplyStates( unsigned int stateMask, unsigned int writeMask ) const
{
    // calculate valid render states.
    unsigned int enableState = _enableState & stateMask;
    unsigned int disableState = _disableState & stateMask;
    unsigned int writeState = _writeMask & writeMask;

    // enable/disable states.
    GrEnable( enableState );
    GrDisable( disableState );

    // if we're handling alpha test...
    if ( ( enableState & GR_ALPHATEST ) != 0 )
        GrSetAlphaTestMode( EAM_GREATER, _cachedAlphaTestRef );

    if ( ( enableState & GR_DEPTHBIAS ) != 0 )
        GrSetDepthBias( _slopeDepthBias, _constDepthBias );

    // if we're handling depth test...  do we care about this?
//  if ( ( enableState & GR_DEPTHTEST ) != 0 )
//  {
//  }

    // if we're handling culling...
    if ( ( enableState & GR_CULLFACE ) != 0 )
        GrSetCullMode( _cullMode );

    // enable write masks.
    GrSetWriteEnable( writeState );
}

//----------------------------------------------------------
void
GrMaterialPass::ApplyVertexParams( GrShader* shaderOverride ) const
{
    // upload vertex color params.
    if ( !shaderOverride )
        shaderOverride = _shader;
    shaderOverride->SetEngineParam4fv( GrShader::EP_VERTEX_COLOR_MOD, _vertexColorMod );
    shaderOverride->SetEngineParam4fv( GrShader::EP_VERTEX_COLOR_ADD, _vertexColorAdd );
}

//----------------------------------------------------------
void
GrMaterialPass::ApplyShader( EGRQUALITY quality ) const
{
    _shader->Bind( quality );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMaterialPass::ApplyBlendStates( const SMaterialPassBlendStates& blendState, unsigned int stateMask ) const
{
    // setup blending.
    unsigned int enableState = _enableState & stateMask;
    if ( ( enableState & GR_BLEND ) != 0 )
    {
        GrSetBlendMode( blendState.srcColorBlendFactor, blendState.dstColorBlendFactor,
                        blendState.srcAlphaBlendFactor, blendState.dstAlphaBlendFactor );
    }
}
