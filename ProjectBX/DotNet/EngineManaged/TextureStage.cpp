//----------------------------------------------------------
// File:        TextureStage.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "TextureStage.h"

// project includes.
#include "Utils.h"
#include "Color.h"

// engine includes.
#include "GrTextureMgr.h"
#include "GrMaterialPass.h"

// .NET namespaces.
using namespace System;

//----------------------------------------------------------
Bootstrap::TextureStage::TextureStage( GrMaterialPass* pass, ESTAGE stage )
: _pass( pass )
, _stage( stage )
{
    
}

//----------------------------------------------------------
Bootstrap::TextureStage::~TextureStage()
{
    this->!TextureStage();
}

//----------------------------------------------------------
Bootstrap::TextureStage::!TextureStage()
{

}

//----------------------------------------------------------
String^ 
Bootstrap::TextureStage::Texture::get()
{
    // get the stage from the material pass.
    return TStrToStr( _pass->GetStage( _stage ).GetTexture()->GetName().GetPathString() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::Texture::set( String^ value )
{
    // figure out the right format for the texture.
    GrTextureBase::EFORMAT format = GrTextureBase::EF_GUESS;
    if ( _stage == ES_NORMAL )
        format = GrTextureBase::EF_NORMALMAP;

    // load and assign the texture if possible.
    URef< GrTextureBase > texture = gGrTextureMgr->GetTextureFile( StrToTStr( value ),
        format, 0 );
    if ( texture )
        _pass->SetStageTexture( _stage, texture );
}

//----------------------------------------------------------
Bootstrap::TextureStage::AddressType 
Bootstrap::TextureStage::AddressModeS::get()
{
    return ( AddressType )_pass->GetStage( _stage ).GetAddressModeS();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::AddressModeS::set( TextureStage::AddressType mode )
{
    _pass->SetStageAddressMode( _stage, ( GrTextureStage::EADDRESSMODE )mode, _pass->GetStage( _stage ).GetAddressModeT() );
}

//----------------------------------------------------------
Bootstrap::TextureStage::AddressType 
Bootstrap::TextureStage::AddressModeT::get()
{
    return ( AddressType )_pass->GetStage( _stage ).GetAddressModeT();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::AddressModeT::set( TextureStage::AddressType mode )
{
    _pass->SetStageAddressMode( _stage, _pass->GetStage( _stage ).GetAddressModeS(), ( GrTextureStage::EADDRESSMODE )mode );
}

//----------------------------------------------------------
Bootstrap::TextureStage::FilterType 
Bootstrap::TextureStage::FilterMode::get()
{
    return ( FilterType )_pass->GetStage( _stage ).GetFilterMode();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::FilterMode::set( TextureStage::FilterType value )
{
    _pass->SetStageFilterMode( _stage, ( GrTextureStage::EFILTERMODE )value );
}

//----------------------------------------------------------
float 
Bootstrap::TextureStage::MaxAnisotropy::get()
{
    return _pass->GetStage( _stage ).GetMaxAnisotropy();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::MaxAnisotropy::set( float value )
{
    _pass->SetStageMaxAnisotropy( _stage, value );
}

//----------------------------------------------------------
bool 
Bootstrap::TextureStage::MipmapEnable::get()
{
    return _pass->GetStage( _stage ).GetMipMapEnable();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::MipmapEnable::set( bool value )
{
    _pass->SetStageMipMapEnable( _stage, value );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::TextureStage::TexColorMod::get()
{
    return Color( _pass->GetStage( _stage ).GetTexColorMod() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::TexColorMod::set( Color color )
{
    _pass->SetStageTexColorMod( _stage, color.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::TextureStage::TexColorAdd::get()
{
    return Color( _pass->GetStage( _stage ).GetTexColorAdd() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::TexColorAdd::set( Color color )
{
    _pass->SetStageTexColorAdd( _stage, color.ToGrColor() );
}

//----------------------------------------------------------
Bootstrap::TextureStage::GenTextureFlag 
Bootstrap::TextureStage::GenTextureFlags::get()
{
    return ( GenTextureFlag )_pass->GetStage( _stage ).GetGenTextureFlag();
}

//----------------------------------------------------------
void 
Bootstrap::TextureStage::GenTextureFlags::set( GenTextureFlag value )
{
    _pass->SetStageGenTextureFlag( _stage, ( GrTextureStage::GenTextureFlag )value );
}