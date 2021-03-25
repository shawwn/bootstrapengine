//----------------------------------------------------------
// File:		GrTextureStage.cpp
// Author:		Kevin Bray
// Created:		11-12-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTextureStage.h"

// project includes.
#include "GrTextureBase.h"
#include "UExprTerm.h"
#include "GLSubsys.h"
#include "GrTextureMgr.h"

// _filterMap[ _texturingQuality ][ _filterQual ][ min=0, mag=1 ]
static const GLenum _filterMap[ EGQ_COUNT ][ GrTextureStage::EFM_COUNT ][ 2 ] =
{
	// ultra texture quality.
	{
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// anisotropic
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// trilinear
		{	GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,	},	// bilinear
		{	GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,	},	// nearest
	},

	// high texture quality.
	{
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// anisotropic
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// trilinear
		{	GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,	},	// bilinear
		{	GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,	},	// nearest
	},

	// medium texture quality.
	{
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// anisotropic
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// trilinear
		{	GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,	},	// bilinear
		{	GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,	},	// nearest
	},

	// low texture quality.
	{
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// anisotropic
		{	GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,		},	// trilinear
		{	GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR,	},	// bilinear
		{	GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,	},	// nearest
	},
};

static const float _anisotropy[ 4 ] =
{
	16.0f,		// ultra
	4.0f,		// high
	1.0f,		// medium
	1.0f,		// low
};

static const int _mipBias[ 4 ] =
{
	0,			// ultra
	0,			// high
	0,			// medium
	0,			// low
};

static const unsigned int _addressModeMap[ 3 ] =
{
	GL_REPEAT,
	GL_CLAMP,
//	GL_MIRROR_CLAMP_TO_EDGE_EXT,
};

//**********************************************************
// class GrTextureStage
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTextureStage::GrTextureStage()
: _glMinFilter( GL_LINEAR )
, _glMagFilter( GL_LINEAR )
, _glAddressS( GL_REPEAT )
, _glAddressT( GL_REPEAT )
, _addressS( EAM_REPEAT )
, _addressT( EAM_REPEAT )
, _genTextureFlag( 0 )
, _texColorMod( 1.0, 1.0, 1.0, 1.0 )
, _texColorAdd( 0.0, 0.0, 0.0, 0.0 )
, _maxAnisotropy( 1.0f )
, _filterMode( EFM_BILINEAR )
, _mipMapEnable( true )
, _glModesDirty( true )
{

}

//----------------------------------------------------------
GrTextureStage::GrTextureStage( const GrTextureStage& stage )
{
	Assign( stage );
}

//----------------------------------------------------------
GrTextureStage::~GrTextureStage()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTextureStage::SetTexture( URef< GrTextureBase > texture )
{
	B_ASSERT( texture != 0 );
	_texture = texture;
	_glModesDirty = true;	// mipmapping may not be supported on this texture!
}

//----------------------------------------------------------
void
GrTextureStage::SetAddressMode( EADDRESSMODE s, EADDRESSMODE t )
{
	// figure out the filtering modes.
	_glAddressS = _addressModeMap[ s ];
	_glAddressT = _addressModeMap[ t ];
	_addressS = s;
	_addressT = t;
}

//----------------------------------------------------------
void
GrTextureStage::SetFilterMode( EFILTERMODE filterMode )
{
	_glModesDirty = true;
	_filterMode = filterMode;
}

//----------------------------------------------------------
void
GrTextureStage::SetMaxAnisotropy( float maxAnisotropy )
{
	_maxAnisotropy = maxAnisotropy;
}

//----------------------------------------------------------
void
GrTextureStage::SetMipMapEnable( bool mipMapEnable )
{
	_glModesDirty = true;
	_mipMapEnable = mipMapEnable;
}

//----------------------------------------------------------
void
GrTextureStage::SetGenTextureFlag( GenTextureFlag flags )
{
	B_ASSERT( IsPow2( flags ) || flags == 0 );
	_genTextureFlag = flags;
}

//----------------------------------------------------------
void
GrTextureStage::SetTexColorMod( const GrColor& color )
{
	_texColorMod = color;
}

//----------------------------------------------------------
void
GrTextureStage::SetTexColorAdd( const GrColor& color )
{
	_texColorAdd = color;
}

//----------------------------------------------------------
URef< GrTextureBase >
GrTextureStage::GetTexture() const
{
	return _texture;
}

//----------------------------------------------------------
void
GrTextureStage::Update() const
{

}

//----------------------------------------------------------
void
GrTextureStage::ApplyShaderState( GrShader* shader, GrShader::EPARAM colorReg0, GrShader::EPARAM colorReg1 ) const
{
	// make sure the shader passed in is valid.
	B_ASSERT( shader != 0 );

	// upload the color factors to the appropriate uniforms.
	shader->SetEngineParam4fv( colorReg0, _texColorMod );
	shader->SetEngineParam4fv( colorReg1, _texColorAdd );
}

//----------------------------------------------------------
void
GrTextureStage::BindTexture( unsigned int texUnit ) const
{
	B_ASSERT( texUnit < 16 );

	// no texture?  Then there is nothing to do.
	if ( !_texture )
		return;

	// get the texturing quality.
	EGRQUALITY texQual = gGrTextureMgr->GetTexturingQuality();

	// grab the filter modes.
	_glMagFilter = _filterMap[ texQual ][ _filterMode ][ 1 ];
	if ( _mipMapEnable && !( _texture->GetFlags() & GrTextureBase::kNoMipMap ) )
		_glMinFilter = _filterMap[ texQual ][ _filterMode ][ 0 ];
	else
		_glMinFilter = _glMagFilter;

	// get the mip bias (if any).
	int mipBias = _mipBias[ texQual ];

	// bind the texture.
	_texture->Bind( texUnit, _glMinFilter, _glMagFilter, _glAddressS, _glAddressT, _anisotropy[ texQual ], mipBias );
}

//----------------------------------------------------------
GrTextureStage&
GrTextureStage::operator = ( const GrTextureStage& stage )
{
	Assign( stage );
	return *this;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTextureStage::Assign( const GrTextureStage& stage )
{
	_texture = stage._texture;
	_glMinFilter = stage._glMinFilter;
	_glMagFilter = stage._glMagFilter;
	_glAddressS = stage._glAddressS;
	_glAddressT = stage._glAddressT;
	_genTextureFlag = stage._genTextureFlag;
	_texColorMod = stage._texColorMod;
	_texColorAdd = stage._texColorAdd;
	_maxAnisotropy = stage._maxAnisotropy;
	_addressS = stage._addressS;
	_addressT = stage._addressT;
	_filterMode = stage._filterMode;
	_mipMapEnable = stage._mipMapEnable;
	_glModesDirty = stage._glModesDirty;
}
