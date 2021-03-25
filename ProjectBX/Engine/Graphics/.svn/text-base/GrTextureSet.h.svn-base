//----------------------------------------------------------
// File:		GrTextureSet.h
// Author:		Kevin Bray
// Created:		08-10-07
//
// Purpose:		To manage the texture stages in a material pass.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTextureStage.h"
#include "GrConfig.h"
#include "UExprTerm.h"

// forward declarations.
class GrTexture;

//**********************************************************
// class GrTextureSet
//**********************************************************
class GrTextureSet
{
public:
	GrTextureSet();
	GrTextureSet( const GrTextureSet& textureSet );
	~GrTextureSet();

	void						SetScaleS( URef< UExprTerm > scale );
	void						SetScaleT( URef< UExprTerm > scale );
	void						SetRotate( URef< UExprTerm > rotate );
	void						SetRotateCenterS( URef< UExprTerm > s );
	void						SetRotateCenterT( URef< UExprTerm > t );
	void						SetTranslateS( URef< UExprTerm > translate );
	void						SetTranslateT( URef< UExprTerm > translate );

	URef< UExprTerm >			GetRotate() const				{	return _rotateExpr;				}
	URef< UExprTerm >			GetRotateCenterS() const		{	return _stRotateCenterExpr[ 0 ];}
	URef< UExprTerm >			GetRotateCenterT() const		{	return _stRotateCenterExpr[ 1 ];}
	URef< UExprTerm >			GetTranslateS() const			{	return _stTranslateExpr[ 0 ];	}
	URef< UExprTerm >			GetTranslateT() const			{	return _stTranslateExpr[ 1 ];	}
	URef< UExprTerm >			GetScaleS() const				{	return _stScaleExpr[ 0 ];		}
	URef< UExprTerm >			GetScaleT() const				{	return _stScaleExpr[ 1 ];		}

	const GrTextureStage&		GetStage( ESTAGE stage ) const	{	return _stages[ stage ];		}
	void						SetStage( ESTAGE stage, const GrTextureStage& texStage );

	void						SetStageTexture( ESTAGE stage, URef< GrTextureBase > texture );
	void						SetStageAddressMode( ESTAGE stage, GrTextureStage::EADDRESSMODE s, GrTextureStage::EADDRESSMODE t );
	void						SetStageFilterMode( ESTAGE stage, GrTextureStage::EFILTERMODE filterMode );
	void						SetStageMaxAnisotropy( ESTAGE stage, float maxAnisotropy );
	void						SetStageMipMapEnable( ESTAGE stage, bool mipMapEnable );
	void						SetStageGenTextureFlag( ESTAGE stage, GrTextureStage::GenTextureFlag flags );
	void						SetStageTexColorMod( ESTAGE stage, const GrColor& color );
	void						SetStageTexColorAdd( ESTAGE stage, const GrColor& color );

	void						Update() const;

	void						ApplyGlobalState( GrShader* shader ) const;
	void						ApplyStageState( ESTAGE stage, unsigned int glTextureUnit, GrShader* shader, GrShader::EPARAM colorReg0, GrShader::EPARAM colorReg1 ) const;
	void						ApplyStageState( ESTAGE stage, unsigned int glTextureUnit ) const;

	GrTextureSet&				operator = ( const GrTextureSet& textureSet );

private:
	void						Init();
	void						Assign( const GrTextureSet& textureSet );

	GrTextureStage				_stages[ ES_COUNT ];

	URef< UExprTerm >			_rotateExpr;
	URef< UExprTerm >			_stRotateCenterExpr[ 2 ];
	URef< UExprTerm >			_stScaleExpr[ 2 ];
	URef< UExprTerm >			_stTranslateExpr[ 2 ];

	mutable float				_matrix[ 8 ];
};
