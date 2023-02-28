//----------------------------------------------------------
// File:        Material.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Material.h"

// project includes.
#include "Utils.h"
#include "MaterialPass.h"

// engine includes.
#include "GrMaterial.h"
#include "GrMaterialMgr.h"
#include "GrMaterialPass.h"

// editlib includes.
#include "EdUberTexBuilder.h"
#include "GrScene.h"

//**********************************************************
// ref class Material
//**********************************************************

//----------------------------------------------------------
Bootstrap::Material::Material( GrMaterial* material )
: _material( material )
{
    // increment the reference count.
    _material->IncRef();
}

//----------------------------------------------------------
Bootstrap::Material::Material( System::String^ name )
{
    // get the material.
    tstring errors;
    tstring matName = StrToTStr( name );
    _material = gGrMaterialMgr->GetMaterial( matName, errors );
    _material->IncRef();
}

//----------------------------------------------------------
Bootstrap::Material::~Material()
{
    this->!Material();
}

//----------------------------------------------------------
Bootstrap::Material::!Material()
{
    // decrement the reference count.
    _material->DecRef();

    if( _passes != nullptr )
    {
        // make sure all passes get disposed.
        for ( int i = 0; i < _passes->Length; ++i )
            _passes[ i ]->~MaterialPass();
    }

    if( _postPasses != nullptr )
    {
        for ( int i = 0; i < _postPasses->Length; ++i )
            _postPasses[ i ]->~MaterialPass();
    }
}

//----------------------------------------------------------
array< System::String^ >^ 
Bootstrap::Material::GetLoadedMaterialNames()
{
    // get the material count.
    unsigned int materialCount = gGrMaterialMgr->GetMaterialCount();
    array< System::String^ >^ materials = gcnew array< System::String^ >( materialCount );

    // get names from all of the materials.
    for ( unsigned int i = 0; i < materialCount; ++i )
    {
        URef< GrMaterial > material = gGrMaterialMgr->GetMaterialByIdx( i );
        materials[ i ] = TStrToStr( material->GetName() );
    }

    // return the list of materials.
    return materials;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Material::Name::get()
{
    return TStrToStr( _material->GetName() );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Material::ShortName::get()
{
    return TStrToStr( _material->GetShortName() );
}

//----------------------------------------------------------
bool 
Bootstrap::Material::Stipple::get()
{
    return _material->GetStipple();
}

//----------------------------------------------------------
void 
Bootstrap::Material::Stipple::set( bool value )
{
    _material->SetStipple( value );
}

//----------------------------------------------------------
int 
Bootstrap::Material::PassCount::get()
{
    CachePasses();
    return ( int )_passes->Length;
}

//----------------------------------------------------------
Bootstrap::MaterialPass^ 
Bootstrap::Material::GetPass( int passIdx )
{
    CachePasses();
    return _passes[ passIdx ];
}

//----------------------------------------------------------
void 
Bootstrap::Material::AddPass()
{
    CachePasses();

    // fill out a new pass info structure.
    SMaterialPassInfo passInfo;

    // create a new material pass by cloning the last pass.
    GrMaterialPass* basePass = 0;
    if ( _material->GetPassCount() > 0 )
    {
        basePass = _material->GetPass( _material->GetPassCount() - 1 );
        passInfo.shader = basePass->GetShader();
        passInfo.ambientShader = basePass->GetAmbientShader();
        passInfo.textures = basePass->GetTextureSet();
        passInfo.vertexColorMod = basePass->GetVertexColorMod();
        passInfo.vertexColorAdd = basePass->GetVertexColorAdd();
        passInfo.minLightResponse = basePass->GetMinLightResponse();
        passInfo.maxLightResponse = basePass->GetMaxLightResponse();
        passInfo.alphaTestMode = basePass->GetAlphaTestMode();
        passInfo.alphaTestRef = basePass->GetAlphaTestRef();
        passInfo.passEnable = basePass->GetPassEnable();
        passInfo.slopeBias = basePass->GetSlopeDepthBias();
        passInfo.constBias = basePass->GetConstDepthBias();
        passInfo.ambBlendStates = basePass->GetAmbientBlendStates();
        passInfo.lightBlendStates = basePass->GetLightBlendStates();
        passInfo.enableState = basePass->GetStates();
        passInfo.writeMask = basePass->GetWriteMask();
        passInfo.cullMode = basePass->GetCullMode();
        passInfo.receiveShadow = basePass->ReceiveShadow();
        passInfo.castShadow = basePass->CastShadow();
    }

    // now create the new pass.
    _material->AddPass( passInfo );
    array< MaterialPass^ >^ newPassArray = gcnew array< MaterialPass^ >
    {
        gcnew MaterialPass( _material->GetPass( _material->GetPassCount() - 1 ) )
    };

    // add the pass to our array of passes.
    _passes->Resize( newPassArray, _passes->Length + 1 );
}

//----------------------------------------------------------
int 
Bootstrap::Material::PostPassCount::get()
{
    CachePostPasses();
    return _postPasses->Length;
}

//----------------------------------------------------------
Bootstrap::MaterialPass^ 
Bootstrap::Material::GetPostPass( int passIdx )
{
    CachePostPasses();
    return _postPasses[ passIdx ];
}

//----------------------------------------------------------
void 
Bootstrap::Material::AddPostPass()
{
    CachePostPasses();

    // create a new material pass by cloning the last pass.
    GrMaterialPass* newPass = _material->GetPostProcessPass( _material->GetPostProcessPassCount() - 1 )->Clone(); 
    array< MaterialPass^ >^ newPassArray = gcnew array< MaterialPass^ >
    {
        gcnew MaterialPass( newPass )
    };
    
    // add the pass to our array of passes.
    _postPasses->Resize( newPassArray, _postPasses->Length + 1 );
}

//----------------------------------------------------------
void 
Bootstrap::Material::CachePasses()
{
    if( _passes == nullptr )
    {
        // build pass wrappers.
        _passes = gcnew array< MaterialPass^ >( _material->GetPassCount() );
        for ( int i = 0; i < _passes->Length; ++i )
        {
            _passes[ i ] = gcnew MaterialPass( _material->GetPass( i ) );
        }
    }
}

//----------------------------------------------------------
void 
Bootstrap::Material::CachePostPasses()
{
    if( _postPasses == nullptr )
    {
        // build post process pass wrappers.
        _postPasses = gcnew array< MaterialPass^ >( _material->GetPostProcessPassCount() );
        for ( int i = 0; i < _postPasses->Length; ++i )
        {
            _postPasses[ i ] = gcnew MaterialPass( _material->GetPostProcessPass( i ) );
        }
    }
}
