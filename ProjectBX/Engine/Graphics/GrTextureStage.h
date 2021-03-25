//----------------------------------------------------------
// File:		GrTextureStage.h
// Author:		Kevin Bray
// Created:		11-12-05
//
// Purpose:		To manage parameters to a texture stage.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"
#include "GrColor.h"
#include "GrShader.h"

// forward declarations.
class GrTextureBase;
class UExprTerm;

//**********************************************************
// class GrTextureStage
//**********************************************************
class GrTextureStage
{
public:
	// gen-texture flags.  These are bit flags so that the renderer can
	// quickly check for procedurally generated textures.
	typedef unsigned int GenTextureFlag;
	static const GenTextureFlag kGenEnvCube = 0x01;
	static const GenTextureFlag kGenEnvDepthCube = 0x02;
	static const GenTextureFlag kGenCurRender = 0x04;
	static const GenTextureFlag kGenCurDepth = 0x08;
	static const GenTextureFlag kGenReflection = 0x10;

	enum EFILTERMODE
	{
		EFM_ANISOTROPIC,
		EFM_TRILINEAR,
		EFM_BILINEAR,
		EFM_NEAREST,
		EFM_COUNT,
	};

	enum EADDRESSMODE
	{
		EAM_REPEAT,
		EAM_CLAMP,
//		EAM_MIRROR,
	};

	GrTextureStage();
	GrTextureStage( const GrTextureStage& stage );
	~GrTextureStage();

	void					SetTexture( URef< GrTextureBase > texture );
	void					SetAddressMode( EADDRESSMODE s, EADDRESSMODE t );
	void					SetFilterMode( EFILTERMODE filterMode );
	void					SetMaxAnisotropy( float maxAnisotropy );
	void					SetMipMapEnable( bool mipMapEnable );
	void					SetGenTextureFlag( GenTextureFlag flags );

	EADDRESSMODE			GetAddressModeS() const			{	return _addressS;				}
	EADDRESSMODE			GetAddressModeT() const			{	return _addressT;				}
	float					GetMaxAnisotropy() const		{	return _maxAnisotropy;			}
	bool					GetMipMapEnable() const			{	return _mipMapEnable;			}
	GenTextureFlag			GetGenTextureFlag() const		{	return _genTextureFlag;			}

	void					SetTexColorMod( const GrColor& color );
	void					SetTexColorAdd( const GrColor& color );

	const GrColor&			GetTexColorMod() const			{	return _texColorMod;			}
	const GrColor&			GetTexColorAdd() const			{	return _texColorAdd;			}

	EFILTERMODE				GetFilterMode() const			{	return _filterMode;				}

	URef< GrTextureBase >	GetTexture() const;

	// updates the texture matrix.  Should be called once per frame.
	void					Update() const;

	// applies the stage state to the GL.
	void					ApplyShaderState( GrShader* shader, GrShader::EPARAM colorReg0, GrShader::EPARAM colorReg1 ) const;

	// binds the stage's texture to the sampler specified.
	void					BindTexture( unsigned int glTextureUnit ) const;

	// assignment operator.
	GrTextureStage&			operator = ( const GrTextureStage& stage );

private:
	void					Assign( const GrTextureStage& stage );

	URef< GrTextureBase >	_texture;
	mutable unsigned int	_glMinFilter;
	mutable unsigned int	_glMagFilter;
	unsigned int			_glAddressS;
	unsigned int			_glAddressT;
	GenTextureFlag			_genTextureFlag;
	GrColor					_texColorMod;
	GrColor					_texColorAdd;
	float					_maxAnisotropy;
	EADDRESSMODE			_addressS;
	EADDRESSMODE			_addressT;
	EFILTERMODE				_filterMode;
	bool					_mipMapEnable;
	mutable bool			_glModesDirty;
};
