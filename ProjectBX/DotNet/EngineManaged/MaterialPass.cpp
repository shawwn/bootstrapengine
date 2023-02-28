//----------------------------------------------------------
// File:		MaterialPass.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "MaterialPass.h"

// project includes.
#include "TextureSet.h"
#include "Utils.h"
#include "Expr.h"
#include "Color.h"

// engine includes.
#include "GrMaterialPass.h"
#include "GrShaderMgr.h"

//**********************************************************
// ref class MaterialPass
//**********************************************************

//----------------------------------------------------------
Bootstrap::MaterialPass::MaterialPass( GrMaterialPass* pass )
: _pass( pass )
{
	_textures = gcnew TextureSet( pass );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::~MaterialPass()
{
	this->!MaterialPass();
}

//----------------------------------------------------------
Bootstrap::MaterialPass::!MaterialPass()
{
	_textures->~TextureSet();
}

//----------------------------------------------------------
System::String^ 
Bootstrap::MaterialPass::Shader::get()
{
	return TStrToStr( _pass->GetShader()->GetName().GetPathString() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::Shader::set( System::String^ value )
{
	URef< GrShader > shader = gGrShaderMgr->GetShader( StrToTStr( value ) );
	if ( shader )
		_pass->SetShader( shader );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::MaterialPass::AmbientShader::get()
{
	return TStrToStr( _pass->GetAmbientShader()->GetName().GetPathString() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AmbientShader::set( System::String^ value )
{
	URef< GrShader > shader = gGrShaderMgr->GetShader( StrToTStr( value ) );
	if ( shader )
		_pass->SetAmbientShader( shader );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::MaterialPass::PassEnable::get()
{
	return gcnew Expr( _pass->GetPassEnable() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::PassEnable::set( Expr^ value )
{
	_pass->SetPassEnable( value->_exprTerm );
}

//----------------------------------------------------------
bool 
Bootstrap::MaterialPass::FirstPass::get()
{
	return _pass->IsFirstPass();
}

//----------------------------------------------------------
Bootstrap::MaterialPass::RenderState 
Bootstrap::MaterialPass::RenderStates::get()
{
	return ( RenderState )_pass->GetStates();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::RenderStates::set( RenderState value )
{
	_pass->SetStates( ( unsigned int )value );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::Face 
Bootstrap::MaterialPass::CullFace::get()
{
	return ( Face )_pass->GetCullMode();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::CullFace::set( Face face )
{
	_pass->SetCullMode( ( EGRCULLMODE )face );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::AlphaTest 
Bootstrap::MaterialPass::AlphaTestMode::get()
{
	return ( AlphaTest )_pass->GetAlphaTestMode();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AlphaTestMode::set( AlphaTest test )
{
	_pass->SetAlphaTestMode( ( EGRALPHATESTMODE )test );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::MaterialPass::AlphaTestRef::get()
{
	return gcnew Expr( _pass->GetAlphaTestRef() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AlphaTestRef::set( Expr^	value )
{
	_pass->SetAlphaTestRef( value->_exprTerm );
}

//----------------------------------------------------------
float 
Bootstrap::MaterialPass::ConstDepthBias::get()
{
	return _pass->GetConstDepthBias();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::ConstDepthBias::set( float value )
{
	_pass->SetConstDepthBias( value );
}

//----------------------------------------------------------
float 
Bootstrap::MaterialPass::SlopeDepthBias::get()
{
	return _pass->GetSlopeDepthBias();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::SlopeDepthBias::set( float value )
{
	_pass->SetSlopeDepthBias( value );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::WriteChannel 
Bootstrap::MaterialPass::WriteMask::get()
{
	return ( WriteChannel )_pass->GetWriteMask();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::WriteMask::set( WriteChannel value )
{
	_pass->SetWriteMask( ( unsigned int )value );
}

//----------------------------------------------------------
bool 
Bootstrap::MaterialPass::CastShadow::get()
{
	return _pass->CastShadow();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::CastShadow::set( bool value )
{
	_pass->SetCastShadow( value );
}

//----------------------------------------------------------
bool 
Bootstrap::MaterialPass::ReceiveShadow::get()
{
	return _pass->ReceiveShadow();
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::ReceiveShadow::set( bool value )
{
	_pass->SetReceiveShadow( value );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::MaterialPass::VertexColorMod::get()
{
	return Color( _pass->GetVertexColorMod() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::VertexColorMod::set( Color value )
{
	_pass->SetVertexColorMod( value.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::MaterialPass::VertexColorAdd::get()
{
	return Color( _pass->GetVertexColorAdd() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::VertexColorAdd::set( Color value )
{
	_pass->SetVertexColorAdd( value.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::MaterialPass::MinLightResponse::get()
{
	return Color( _pass->GetMinLightResponse() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::MinLightResponse::set( Color value )
{
	_pass->SetMinLightResponse( value.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::MaterialPass::MaxLightResponse::get()
{
	return Color( _pass->GetMaxLightResponse() );
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::MaxLightResponse::set( Color value )
{
	_pass->SetMaxLightResponse( value.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::AmbientSrcColorBlendFactor::get()
{
	return ( BlendFactor )_pass->GetAmbientBlendStates().srcColorBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AmbientSrcColorBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetAmbientBlendStates();
	states.srcColorBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetAmbientBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::AmbientDstColorBlendFactor::get()
{
	return ( BlendFactor )_pass->GetAmbientBlendStates().dstColorBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AmbientDstColorBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetAmbientBlendStates();
	states.dstColorBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetAmbientBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::AmbientSrcAlphaBlendFactor::get()
{
	return ( BlendFactor )_pass->GetAmbientBlendStates().srcAlphaBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AmbientSrcAlphaBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetAmbientBlendStates();
	states.srcAlphaBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetAmbientBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::AmbientDstAlphaBlendFactor::get()
{
	return ( BlendFactor )_pass->GetAmbientBlendStates().dstAlphaBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::AmbientDstAlphaBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetAmbientBlendStates();
	states.dstAlphaBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetAmbientBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::LightSrcColorBlendFactor::get()
{
	return ( BlendFactor )_pass->GetLightBlendStates().srcColorBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::LightSrcColorBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetLightBlendStates();
	states.srcColorBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetLightBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::LightDstColorBlendFactor::get()
{
	return ( BlendFactor )_pass->GetLightBlendStates().dstColorBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::LightDstColorBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetLightBlendStates();
	states.dstColorBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetLightBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::LightSrcAlphaBlendFactor::get()
{
	return ( BlendFactor )_pass->GetLightBlendStates().srcAlphaBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::LightSrcAlphaBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetLightBlendStates();
	states.srcAlphaBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetLightBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::MaterialPass::BlendFactor 
Bootstrap::MaterialPass::LightDstAlphaBlendFactor::get()
{
	return ( BlendFactor )_pass->GetLightBlendStates().dstAlphaBlendFactor;
}

//----------------------------------------------------------
void 
Bootstrap::MaterialPass::LightDstAlphaBlendFactor::set( BlendFactor value )
{
	SMaterialPassBlendStates states = _pass->GetLightBlendStates();
	states.dstAlphaBlendFactor = ( EGRBLENDMODE )value;
	_pass->SetLightBlendStates( states );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::DiffuseStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Diffuse );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::SpecularStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Specular );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::NormalStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Normal );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::BumpStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Bump );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::EmissiveStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Emissive );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::MaterialPass::AmbientStage::get()
{
	return _textures->GetTextureStage( TextureSet::Stage::Ambient );
}

//----------------------------------------------------------
Bootstrap::TextureSet^	
Bootstrap::MaterialPass::Textures::get()
{
	return _textures;
}