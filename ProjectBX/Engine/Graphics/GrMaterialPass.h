//----------------------------------------------------------
// File:		GrMaterialPass.h
// Author:		Kevin Bray
// Created:		03-20-05
//
// Purpose:		To contain parameters for a program and is intended
//				to contain a pass for a material.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"
#include "MVec2.h"
#include "GrConfig.h"
#include "UExprTerm.h"
#include "GrTextureSet.h"
#include "GrMaterialUserParams.h"
#include "MMat4x4.h"
#include "GrColor.h"
#include "GrUtil.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrShader;
class GrTexture;
class GrTextureCube;
class GrCamera;
class GrMeshInst;
class GrMesh;
class UExprTerm;

struct SMaterialPassBlendStates
{
	SMaterialPassBlendStates();
	~SMaterialPassBlendStates();

	// states specific to various passes.
	EGRBLENDMODE srcColorBlendFactor;
	EGRBLENDMODE srcAlphaBlendFactor;
	EGRBLENDMODE dstColorBlendFactor;
	EGRBLENDMODE dstAlphaBlendFactor;

	bool operator == ( const SMaterialPassBlendStates& rhs ) const
	{
		return ( srcColorBlendFactor == rhs.srcColorBlendFactor &&
				 dstColorBlendFactor == rhs.dstColorBlendFactor &&
				 srcAlphaBlendFactor == rhs.srcAlphaBlendFactor &&
				 dstAlphaBlendFactor == rhs.dstAlphaBlendFactor );
	}
};

struct SMaterialPassInfo
{
	SMaterialPassInfo();
	~SMaterialPassInfo();

	// primary shader for the pass.
	URef< GrShader > shader;

	// this is used by the renderer when rendering ambience.  If
	// this is NULL, then the default ambient lighting model is
	// used.
	URef< GrShader > ambientShader;

	// stage parameters.  Note that if ubertextures are used,
	// stages are ignored.
	GrTextureSet textures;

	// vertex parameters.
	GrColor	vertexColorMod;
	GrColor	vertexColorAdd;

	// light response range.
	GrColor minLightResponse;
	GrColor maxLightResponse;

	// alpha test reference value.
	EGRALPHATESTMODE alphaTestMode;
	URef< UExprTerm > alphaTestRef;

	// pass enable.
	URef< UExprTerm > passEnable;

	// depth bias.
	float slopeBias;
	float constBias;

	// GL enums.
	SMaterialPassBlendStates	ambBlendStates;
	SMaterialPassBlendStates	lightBlendStates;

	// states.
	unsigned int enableState;
	unsigned int writeMask;

	// culling mode.
	EGRCULLMODE cullMode;

	// various flags.
	bool receiveShadow;
	bool castShadow;
};

//**********************************************************
// class GrMaterialPass
//**********************************************************
class GrMaterialPass
{
public:
	//==========================================================
	// initialization.
	//==========================================================
	GrMaterialPass( unsigned int passNumber, const SMaterialPassInfo& passInfo );
	~GrMaterialPass();

	// for array initialization or material rebuilding.
	void						Init( unsigned int passNumber, const SMaterialPassInfo& passInfo );

	const tstring&				GetName() const									{	return _name;							}

	// creates a clone of this material pass.
	GrMaterialPass*				Clone() const;

	void						SetShader( URef< GrShader > shader )			{	_shader = shader;						}
	void						SetAmbientShader( URef< GrShader > shader )		{	_ambientShader = shader;				}

	// texgen flags enable/disable.
//	void						EnableGenTextureFlags( unsigned int flags )		{	_genTextureFlags |= flags;				}
//	void						DisableGenTextureFlags( unsigned int flags )	{	_genTextureFlags &= ~flags;				}

	// does this material pass use alpha testing?
	bool						AlphaTest() const								{	return ( _enableState & GR_ALPHATEST ) != 0;	}
	bool						CastShadow() const								{	return _castShadow;						}
	bool						ReceiveShadow() const							{	return _receiveShadow;					}

	void						SetCastShadow( bool castShadow )				{	_castShadow = castShadow;				}
	void						SetReceiveShadow( bool receiveShadow )			{	_receiveShadow = receiveShadow;			}

	bool						IsEnabled() const								{	return _cachedPassEnable;				}
	bool						IsFirstPass() const								{	return _firstPass;						}

	// accessors.
	unsigned int				GetStates() const								{	return _enableState;					}
	unsigned int				GetWriteMask() const							{	return _writeMask;						}
	EGRCULLMODE					GetCullMode() const								{	return _cullMode;						}
	EGRALPHATESTMODE			GetAlphaTestMode() const						{	return _alphaTestMode;					}
	URef< UExprTerm >			GetAlphaTestRef() const							{	return _alphaTestRef;					}
	URef< UExprTerm >			GetPassEnable() const							{	return _passEnable;						}
	float						GetSlopeDepthBias() const						{	return _slopeDepthBias;					}
	float						GetConstDepthBias() const						{	return _constDepthBias;					}
	const GrColor&				GetVertexColorMod() const						{	return _vertexColorMod;					}
	const GrColor&				GetVertexColorAdd() const						{	return _vertexColorAdd;					}

	const SMaterialPassBlendStates&	GetAmbientBlendStates() const				{	return _ambBlendStates;					}
	const SMaterialPassBlendStates&	GetLightBlendStates() const					{	return _lightBlendStates;				}

	// mutators.
	void						SetStates( unsigned int states );
	void						SetWriteMask( unsigned int mask )				{	_writeMask = mask;						}
	void						SetCullMode( EGRCULLMODE mode )					{	_cullMode = mode;						}
	void						SetAlphaTestMode( EGRALPHATESTMODE mode )		{	_alphaTestMode = mode;					}
	void						SetAlphaTestRef( URef< UExprTerm > expr )		{	_alphaTestRef = expr;					}
	void						SetSlopeDepthBias( float bias )					{	_slopeDepthBias = bias;					}
	void						SetConstDepthBias( float bias )					{	_constDepthBias = bias;					}
	void						SetPassEnable( URef< UExprTerm > expr )			{	_passEnable = expr;						}
	void						SetVertexColorMod( const GrColor& color )		{	_vertexColorMod = color;				}
	void						SetVertexColorAdd( const GrColor& color )		{	_vertexColorAdd = color;				}

	void						SetAmbientBlendStates( const SMaterialPassBlendStates& states ) 	{	_ambBlendStates = states;		}
	void						SetLightBlendStates( const SMaterialPassBlendStates& states )		{	_lightBlendStates = states;		}

	//==========================================================
	// user parameters
	//==========================================================
	GrMaterialUserParams&		GetUserParams()									{	return _userParams;						}
	const GrMaterialUserParams&	GetUserParams() const							{	return _userParams;						}

	//==========================================================
	// texture stages
	//==========================================================
	const GrTextureStage&		GetStage( ESTAGE stage ) const					{	return _textures.GetStage( stage );		}
	void						SetStage( ESTAGE stage, GrTextureStage& s )		{	return _textures.SetStage( stage, s );	}

	const GrTextureSet&			GetTextureSet() const							{	return _textures;						}
	void						SetTextureSet( const GrTextureSet& textures )	{	_textures = textures;					}

	void						SetTextureScaleS( URef< UExprTerm > scale )			{	_textures.SetScaleS( scale );			}
	void						SetTextureScaleT( URef< UExprTerm > scale )			{	_textures.SetScaleT( scale );			}
	void						SetTextureRotate( URef< UExprTerm > rotate )		{	_textures.SetRotate( rotate );			}		
	void						SetTextureRotateCenterS( URef< UExprTerm > rotate )	{	_textures.SetRotateCenterS( rotate );	}		
	void						SetTextureRotateCenterT( URef< UExprTerm > rotate )	{	_textures.SetRotateCenterT( rotate );	}		
	void						SetTextureTranslateS( URef< UExprTerm > translate )	{	_textures.SetTranslateS( translate );	}
	void						SetTextureTranslateT( URef< UExprTerm > translate )	{	_textures.SetTranslateT( translate );	}

	void						SetStageTexture( ESTAGE stage, URef< GrTextureBase > texture );
	void						SetStageAddressMode( ESTAGE stage, GrTextureStage::EADDRESSMODE s, GrTextureStage::EADDRESSMODE t );
	void						SetStageFilterMode( ESTAGE stage, GrTextureStage::EFILTERMODE filterMode );
	void						SetStageMaxAnisotropy( ESTAGE stage, float maxAnisotropy );
	void						SetStageMipMapEnable( ESTAGE stage, bool mipMapEnable );
	void						SetStageGenTextureFlag( ESTAGE stage, GrTextureStage::GenTextureFlag flags );
	void						SetStageTexColorMod( ESTAGE stage, const GrColor& color );
	void						SetStageTexColorAdd( ESTAGE stage, const GrColor& color );

	//==========================================================
	// update
	//==========================================================
	// updates dynamic parameters and texture stages.
	void						Update();

	//==========================================================
	// parameter bindings
	//==========================================================

	// applies FF render states.
	void						ApplyAmbientBlendStates( unsigned int stateMask ) const;
	void						ApplyLightBlendStates( unsigned int stateMask ) const;
	void						ApplyStates( unsigned int stateMask, unsigned int writeMask ) const;

	// applies the vertex color modulate/add.
	void						ApplyVertexParams( GrShader* shaderOverride = 0 ) const;

	void						ApplyShader( EGRQUALITY quality ) const;

	GrShader*					GetShader()	const								{	return _shader;						}
	GrShader*					GetAmbientShader() const						{	return _ambientShader;				}

	//==========================================================
	// color response.
	//==========================================================
	const GrColor&				GetMinLightResponse() const						{	return _minLightResponse;			}
	const GrColor&				GetMaxLightResponse() const						{	return _maxLightResponse;			}
	void						SetMinLightResponse( const GrColor& color )		{	_minLightResponse = color;			}
	void						SetMaxLightResponse( const GrColor& color )		{	_maxLightResponse = color;			}

private:
	void						ApplyCommonStates( unsigned int stateMask, unsigned int writeMask ) const;
	void						ApplyBlendStates( const SMaterialPassBlendStates& blendState, unsigned int stateMask ) const;

	// param flags.
	static const unsigned int kUsed = 0x01;
	static const unsigned int kDynamic = 0x02;

	// pass name.
	tstring						_name;

	// ambient shader.  If this is null, then the renderer uses the default
	// ambient shader.
	URef< GrShader >			_ambientShader;

	// main shader of this pass.  This is the light interaction shader or
	// the post processing shader.
	URef< GrShader >			_shader;

	// user params.
	GrMaterialUserParams		_userParams;

	// textures.
	GrTextureSet				_textures;

	// global states.
	GrColor						_vertexColorMod;
	GrColor						_vertexColorAdd;

	// lighting response.
	GrColor						_minLightResponse;
	GrColor						_maxLightResponse;

	// lighting states.
	SMaterialPassBlendStates	_ambBlendStates;
	SMaterialPassBlendStates	_lightBlendStates;

	EGRCULLMODE					_cullMode;
	EGRALPHATESTMODE			_alphaTestMode;
	URef< UExprTerm >			_passEnable;
	URef< UExprTerm >			_alphaTestRef;
	float						_cachedAlphaTestRef;
	unsigned int				_enableState;
	unsigned int				_disableState;
	unsigned int				_writeMask;

	float						_slopeDepthBias;
	float						_constDepthBias;

	// textures that need to automatically be generated.
//	unsigned int				_genTextureFlags;

	// flags.
	bool						_receiveShadow;
	bool						_castShadow;
	bool						_firstPass;
	bool						_cachedPassEnable;
};
