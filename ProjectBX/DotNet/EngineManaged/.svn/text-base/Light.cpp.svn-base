//----------------------------------------------------------
// File:		Light.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Light.h"

// project headers.
#include "Utils.h"
#include "Engine.h"
#include "Scene.h"
#include "Color.h"

// engine headers.
#include "UExprMgr.h"
#include "MMat4x4.h"
#include "UExprNumber.h"
#include "GrLight.h"
#include "GrLightMgr.h"
#include "GrTextureMgr.h"
#include "GrScene.h"
#include "GrModel.h"
#include "GrConfig.h"

// .NET namespaces.
using namespace System;

//**********************************************************
// ref class Light
//**********************************************************

//----------------------------------------------------------
static Bootstrap::Vector3 
MakeLightRot( GrLight* light )
{
    Bootstrap::Vector3 rotate;
	float x = 0.0f, y = 0.0f, z = 0.0f;
	light->GetWorldTransform().GetRotate().ToEulerYXZ( y, x, z );
	rotate.X = RadToDeg( x );
	rotate.Y = RadToDeg( y );
	rotate.Z = RadToDeg( z );
	return rotate;
}

//----------------------------------------------------------
Bootstrap::Light::Light( System::String^ id, System::String^ localName )
: SceneEntity( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 1.0f, 1.0f, 1.0f ), false )
{
	// build the light's info.
	SLightInfo info;
	info.name = StrToTStr( localName );
	info.transform = MMat4x4();
	//info.color = GrColor( (float)_random.NextDouble(), (float)_random.NextDouble(), (float)_random.NextDouble(), 1 );
	info.color = GrColor( 1.0f, 1.0f, 1.0f, 1.0f );
	info.intensity = gUExprMgr->CreateNumber( 1 );
	info.range = gUExprMgr->CreateNumber( 100 );
	info.sqrAtten = 1.0f;
	info.linAtten = 0.0f;
	info.constAtten = 0.0f;
	info.haziness = 0.0f;
	info.width = 0.02f;
	info.mode = ELM_ADD;
	info.parallel = false;
	info.castShadow = true;

	URef< GrLight > light = gGrLightMgr->CreateLight( StrToTStr( id ), info );

	Acquire( light );
}

//----------------------------------------------------------
Bootstrap::Light::Light( GrLight* light )
: SceneEntity( light->GetPos(), MakeLightRot( light ), light->GetWorldTransform().GetScale(), false )
{
	Acquire( light );
}

//----------------------------------------------------------
Bootstrap::Light^ 
Bootstrap::Light::Create( System::String^ localName )
{
	// generate the ID.
	System::String^ id = Engine::Get()->Scene->Name + "/" + localName;
	return gcnew Light( id, localName );
}

//----------------------------------------------------------
Bootstrap::Light::~Light()
{
	// call the finalizer.
	this->!Light();
}

//----------------------------------------------------------
Bootstrap::Light::!Light()
{
	// decrement the light's reference count.
	_light->DecRef();
}

//----------------------------------------------------------
String^ 
Bootstrap::Light::ID::get()
{
	return TStrToStr( _light->GetId().GetPathString() );
}

//----------------------------------------------------------
String^ 
Bootstrap::Light::Name::get()
{
	return TStrToStr( _light->GetName() );
}

//----------------------------------------------------------
void 
Bootstrap::Light::Name::set( String^ name )
{
	_light->SetName( StrToTStr( name ) );
}

//----------------------------------------------------------
Bootstrap::Color 
Bootstrap::Light::Color::get()
{
    return Bootstrap::Color( _light->GetColor() ); 
}

//----------------------------------------------------------
void 
Bootstrap::Light::Color::set( Bootstrap::Color col )
{
    _light->SetColor( col );
}

//----------------------------------------------------------
float 
Bootstrap::Light::Intensity::get()
{
    return _light->GetCachedIntensity();
}

//----------------------------------------------------------
void 
Bootstrap::Light::Intensity::set( float val )
{
    _light->SetIntensity( val );
}

//----------------------------------------------------------
float 
Bootstrap::Light::Range::get()
{
    return _light->GetCachedRange();
}

//----------------------------------------------------------
void 
Bootstrap::Light::Range::set( float val )
{
    _light->SetRange( val );
}

//----------------------------------------------------------
float 
Bootstrap::Light::Softness::get()
{
    return _light->GetWidth();
}

//----------------------------------------------------------
void 
Bootstrap::Light::Softness::set( float val )
{
    _light->SetWidth( val );
}

//----------------------------------------------------------
bool 
Bootstrap::Light::CastShadow::get()
{
    return _light->CastShadow();
}

//----------------------------------------------------------
void 
Bootstrap::Light::CastShadow::set( bool val )
{
    _light->SetCastShadow( val );
}

//----------------------------------------------------------
bool 
Bootstrap::Light::Parallel::get()
{
    return _light->IsParallel();
}

//----------------------------------------------------------
void 
Bootstrap::Light::Parallel::set( bool val )
{
    _light->SetParallel( val );
}

//----------------------------------------------------------
bool 
Bootstrap::Light::Global::get()
{
    return _light->GetGlobal();
}

//----------------------------------------------------------
void 
Bootstrap::Light::Global::set( bool val )
{
    _light->SetGlobal( val );
}

//----------------------------------------------------------
String^ 
Bootstrap::Light::Mask::get()
{
	// TODO: change this so that lights always have a mask texture, even
	// if it's white.  there should be a flag that indicates whether or
	// not a light's mask frustum should be used for culling.
	GrTexture* texture = _light->GetProjTexture();
	return TStrToStr( texture->GetName().GetPathString() );
}

//----------------------------------------------------------
void 
Bootstrap::Light::Mask::set( String^ name )
{
	// get the name.
	if ( name == nullptr )
	{
		_light->SetProjTexture( 0 );
		return;
	}

	// try to load the texture.
	tstring texName = StrToTStr( name );
	URef< GrTextureBase > textureBase = gGrTextureMgr->GetTextureFile( texName,
		GrTextureBase::EF_GUESS, 0 );
	if ( textureBase != 0 )
	{
		// make sure the texture is 2D.
		if ( textureBase->GetType() != GrTextureBase::ETT_2D )
			return;

		// assign the texture to the light.
		_light->SetProjTexture( ( GrTexture* )( GrTextureBase* )textureBase );
	}
}

//----------------------------------------------------------
float 
Bootstrap::Light::ProjSpreadAngle::get()
{
	return RadToDeg( _light->GetProjection().GetFOV() );
}

//----------------------------------------------------------
void 
Bootstrap::Light::ProjSpreadAngle::set( float value )
{
	// set the FOV.
	GrProjection proj = _light->GetProjection();
	proj.SetFOV( DegToRad( value ) );
	_light->SetProjection( proj );
}

//----------------------------------------------------------
float 
Bootstrap::Light::ProjWidth::get()
{
	return _light->GetProjection().GetWidth();
}

//----------------------------------------------------------
void 
Bootstrap::Light::ProjWidth::set( float value )
{
	// set the FOV.
	GrProjection proj = _light->GetProjection();
	proj.SetLeft( -0.5f * value );
	proj.SetRight( 0.5f * value );
	_light->SetProjection( proj );
}

//----------------------------------------------------------
float 
Bootstrap::Light::ProjHeight::get()
{
	return _light->GetProjection().GetHeight();
}

//----------------------------------------------------------
void 
Bootstrap::Light::ProjHeight::set( float value )
{
	// set the FOV.
	GrProjection proj = _light->GetProjection();
	proj.SetBottom( -0.5f * value );
	proj.SetTop( 0.5f * value );
	_light->SetProjection( proj );
}

//----------------------------------------------------------
float 
Bootstrap::Light::AttenConst::get()
{
	return _light->GetConstAtten();
}

//----------------------------------------------------------
void 
Bootstrap::Light::AttenConst::set( float value )
{
    _light->SetConstAtten( value );
}

//----------------------------------------------------------
float 
Bootstrap::Light::AttenLinear::get()
{
	return _light->GetLinAtten();
}

//----------------------------------------------------------
void 
Bootstrap::Light::AttenLinear::set( float value )
{
    _light->SetLinAtten( value );
}

//----------------------------------------------------------
float 
Bootstrap::Light::AttenQuadratic::get()
{
	return _light->GetSqrAtten();
}

//----------------------------------------------------------
void 
Bootstrap::Light::AttenQuadratic::set( float value )
{
    _light->SetSqrAtten( value );
}

//----------------------------------------------------------
Bootstrap::Light^
Bootstrap::Light::Clone( System::String^ instName )
{
    // try to clone the light.
	GrLight* clone = _light->Clone( StrToTStr( instName ) ) ;

    // if the clone was successful, return a reference to the cloned light.
    if ( clone )
        return gcnew Light( clone );

    // otherwise, return null.
    return nullptr;
}

//----------------------------------------------------------
void 
Bootstrap::Light::ApplyTransform( Matrix mat )
{
	// get the parent's transform.
	MMat4x4 inverseParentTransform;
	if( !_light->GetParentTransform().Inverse( inverseParentTransform ) )
	{
		// TODO: assert
		return; 
	}

	// convert the world-space light transform into the local-space transform.
	MMat4x4 localTransform = inverseParentTransform * mat.ToMMat4x4();
	_light->SetLocalTransform( localTransform );
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Light::GetTransform()
{
	return Matrix( _light->GetWorldTransform() );
}

//----------------------------------------------------------
void 
Bootstrap::Light::Acquire( GrLight* light )
{
	_light = light;
	_light->IncRef();
}
