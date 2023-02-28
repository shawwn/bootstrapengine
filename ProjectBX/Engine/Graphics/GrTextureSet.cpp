//----------------------------------------------------------
// File:        GrTextureSet.cpp
// Author:      Kevin Bray
// Created:     08-10-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTextureSet.h"

// project includes.
#include "GrTexture.h"
#include "GrTextureMgr.h"

//**********************************************************
// class GrTextureSet
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureSet::GrTextureSet()
{
    Init();
}

//----------------------------------------------------------
GrTextureSet::GrTextureSet( const GrTextureSet& textureSet )
{
    Init();
    Assign( textureSet );
}

//----------------------------------------------------------
GrTextureSet::~GrTextureSet()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTextureSet::SetScaleS( URef< UExprTerm > scale )
{
    _stScaleExpr[ 0 ] = scale;
}

//----------------------------------------------------------
void
GrTextureSet::SetScaleT( URef< UExprTerm > scale )
{
    _stScaleExpr[ 1 ] = scale;
}

//----------------------------------------------------------
void
GrTextureSet::SetRotate( URef< UExprTerm > rotate )
{
    _rotateExpr = rotate;
}

//----------------------------------------------------------
void
GrTextureSet::SetRotateCenterS( URef< UExprTerm > s )
{
    _stRotateCenterExpr[ 0 ] = s;
}

//----------------------------------------------------------
void
GrTextureSet::SetRotateCenterT( URef< UExprTerm > t )
{
    _stRotateCenterExpr[ 1 ] = t;
}

//----------------------------------------------------------
void
GrTextureSet::SetTranslateS( URef< UExprTerm > translate )
{
    _stTranslateExpr[ 0 ] = translate;
}

//----------------------------------------------------------
void
GrTextureSet::SetTranslateT( URef< UExprTerm > translate )
{
    _stTranslateExpr[ 1 ] = translate;
}

//----------------------------------------------------------
void
GrTextureSet::SetStage( ESTAGE stage, const GrTextureStage& texStage )
{
    _stages[ stage ] = texStage;
}

//----------------------------------------------------------
void
GrTextureSet::SetStageTexture( ESTAGE stage, URef< GrTextureBase > texture )
{
    B_ASSERT( texture != 0 );
    _stages[ stage ].SetTexture( texture );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageAddressMode( ESTAGE stage, GrTextureStage::EADDRESSMODE s, GrTextureStage::EADDRESSMODE t )
{
    _stages[ stage ].SetAddressMode( s, t );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageFilterMode( ESTAGE stage, GrTextureStage::EFILTERMODE filterMode )
{
    _stages[ stage ].SetFilterMode( filterMode );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageMaxAnisotropy( ESTAGE stage, float maxAnisotropy )
{
    _stages[ stage ].SetMaxAnisotropy( maxAnisotropy );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageMipMapEnable( ESTAGE stage, bool mipMapEnable )
{
    _stages[ stage ].SetMipMapEnable( mipMapEnable );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageGenTextureFlag( ESTAGE stage, GrTextureStage::GenTextureFlag flags )
{
    _stages[ stage ].SetGenTextureFlag( flags );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageTexColorMod( ESTAGE stage, const GrColor& color )
{
    _stages[ stage ].SetTexColorMod( color );
}

//----------------------------------------------------------
void
GrTextureSet::SetStageTexColorAdd( ESTAGE stage, const GrColor& color )
{
    _stages[ stage ].SetTexColorAdd( color );
}

//----------------------------------------------------------
void
GrTextureSet::Update() const
{
    float rotate = 0.0f;
    float rotateCenterS = 0.0f;
    float rotateCenterT = 0.0f;
    float scaleS = 1.0f;
    float scaleT = 1.0f;
    float translateS = 0.0f;
    float translateT = 0.0f;

    if ( _rotateExpr )
        rotate = DegToRad( _rotateExpr->GetValue() );

    if ( _stRotateCenterExpr[ 0 ] )
        rotateCenterS = _stRotateCenterExpr[ 0 ]->GetValue();

    if ( _stRotateCenterExpr[ 1 ] )
        rotateCenterT = _stRotateCenterExpr[ 1 ]->GetValue();

    if ( _stScaleExpr[ 0 ] )
        scaleS = _stScaleExpr[ 0 ]->GetValue();

    if ( _stScaleExpr[ 1 ] )
        scaleT = _stScaleExpr[ 1 ]->GetValue();

    if ( _stTranslateExpr[ 0 ] )
        translateS = _stTranslateExpr[ 0 ]->GetValue();

    if ( _stTranslateExpr[ 1 ] )
        translateT = _stTranslateExpr[ 1 ]->GetValue();

    float cosTheta = Cos( rotate );
    float sinTheta = Sin( rotate );

    // create the matrix.
    // first row.
    _matrix[ 0 ] = cosTheta * scaleS;
    _matrix[ 1 ] = -sinTheta * scaleS;
    _matrix[ 2 ] = 0.0f;
    _matrix[ 3 ] = translateS;
    
    // second row.
    _matrix[ 4 ] = sinTheta * scaleT;
    _matrix[ 5 ] = cosTheta * scaleT;
    _matrix[ 6 ] = 0.0f;
    _matrix[ 7 ] = translateT;

    // adjust the translation to account for the rotation center.
    _matrix[ 3 ] -= cosTheta * rotateCenterS + -sinTheta * rotateCenterT;
    _matrix[ 7 ] -= sinTheta * rotateCenterS + cosTheta * rotateCenterT;
    _matrix[ 3 ] += rotateCenterS;
    _matrix[ 7 ] += rotateCenterT;
}

//----------------------------------------------------------
void
GrTextureSet::ApplyGlobalState( GrShader* shader ) const
{
    // upload the texture matrix to the approprate uniforms.
    shader->SetEngineParam4x2fv( GrShader::EP_TEXTURE_MATRIX, _matrix );
}

//----------------------------------------------------------
void
GrTextureSet::ApplyStageState( ESTAGE stage, unsigned int glTextureUnit, GrShader* shader, GrShader::EPARAM colorReg0, GrShader::EPARAM colorReg1 ) const
{
    _stages[ stage ].ApplyShaderState( shader, colorReg0, colorReg1 );
    _stages[ stage ].BindTexture( glTextureUnit );
}

//----------------------------------------------------------
void
GrTextureSet::ApplyStageState( ESTAGE stage, unsigned int glTextureUnit ) const
{
    _stages[ stage ].BindTexture( glTextureUnit );
}

//----------------------------------------------------------
GrTextureSet&
GrTextureSet::operator = ( const GrTextureSet& textureSet )
{
    Assign( textureSet );
    return *this;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTextureSet::Init()
{
    // clear the texture matrix.
    MemSet( _matrix, 0, sizeof( _matrix ) );
    _matrix[ 0 ] = 1.0f;
    _matrix[ 5 ] = 1.0f;
}

//----------------------------------------------------------
void
GrTextureSet::Assign( const GrTextureSet& textureSet )
{
    for ( unsigned int i = 0; i < ES_COUNT; ++i )
        _stages[ i ] = textureSet._stages[ i ];

    _rotateExpr = textureSet._rotateExpr;
    _stScaleExpr[ 0 ] = textureSet._stScaleExpr[ 0 ];
    _stScaleExpr[ 1 ] = textureSet._stScaleExpr[ 1 ];
    _stTranslateExpr[ 0 ] = textureSet._stTranslateExpr[ 0 ];
    _stTranslateExpr[ 0 ] = textureSet._stTranslateExpr[ 0 ];

    for ( unsigned int i = 0; i < 8; ++i )
        _matrix[ i ] = textureSet._matrix[ i ];
}
