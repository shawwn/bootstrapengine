//----------------------------------------------------------
// File:		GrLight.cpp
// Author:		Kevin Bray
// Created:		10-28-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------s
#include "graphics_afx.h"

// class header.
#include "GrLight.h"

// project includes.
#include "MVec3.h"
#include "GrColor.h"
#include "GLSubsys.h"
#include "GrUtil.h"
#include "GrTextureBase.h"
#include "GrTextureMgr.h"
#include "UExprTerm.h"
#include "UExprMgr.h"
#include "UExprNumber.h"
#include "GrCamera.h"

static const unsigned int _lightModeStates[] =
{
	GL_FUNC_ADD,
	GL_FUNC_REVERSE_SUBTRACT,	// dest - src.
};

SLightInfo::SLightInfo()
: projection( -10.0f, 10.0f, -10.0f, 10.0f, 1000.0f )
, sqrAtten( 0.0f )
, linAtten( 0.0f )
, constAtten( 1.0f )
, haziness( 0.0f )
, width( 0.0f )
, mode( ELM_ADD )
, parallel( false )
, global( false )
, castShadow( true )
{

}

//**********************************************************
// class GrLight
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrLight::GrLight( const UPath& id, const SLightInfo& lightInfo )
: _id( id )
, _name( lightInfo.name )
, _localTransform( lightInfo.transform )
, _projection( lightInfo.projection )
, _position( lightInfo.transform.GetTranslate() )
, _direction( lightInfo.transform.GetRotate().GetCol( 2 ) )
, _projTex( lightInfo.projTex )
, _color( lightInfo.color )
, _cachedColor( lightInfo.color )
, _intensity( lightInfo.intensity )
, _cachedIntensity( 1.0f )
, _range( lightInfo.range )
, _cachedRange( 100.0f )
, _sqrAtten( lightInfo.sqrAtten )
, _linAtten( lightInfo.linAtten )
, _constAtten( lightInfo.constAtten )
, _haziness( lightInfo.haziness )
, _width( lightInfo.width )
, _shadowLOD( 0 )
, _castShadow( lightInfo.castShadow )
, _parallel( lightInfo.parallel )
, _global( lightInfo.global )
, _mode( lightInfo.mode )
{
	// the range and intensity must be set!
	B_ASSERT( _range != 0 );
	B_ASSERT( _intensity != 0 );

	// update the world transform.
	UpdateWorldTransform();
}
/*
//----------------------------------------------------------
GrLight::GrLight( const tstring& name, const MMat4x4& transform, const GrColor& color, URef< UExprTerm > intensity,
				 URef< UExprTerm > range, const MSphere& localBounds, float sqrAtten, float linAtten, float constAtten, float haziness,
				 float width, ELIGHTMODE mode, bool castShadow )
: _name( name )
, _transform( transform )
, _projection( 0.0f, 0.0f, 0.0f )
, _texgenMatrix( 0.0f, 0.0f, 0.0f, 0.5f,
				0.0f, 0.0f, 0.0f, 0.5f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f )
, _position( transform.GetTranslate() )
, _direction( transform.GetRotate().GetRow( 2 ) )
, _projTex( 0 )
, _color( color )
, _cachedColor( color )
, _intensity( intensity )
, _cachedIntensity( 1.0f )
, _range( range )
, _cachedRange( 100.0f )
, _localBounds( localBounds )
, _sqrAtten( sqrAtten )
, _linAtten( linAtten )
, _constAtten( constAtten )
, _haziness( haziness )
, _width( width )
, _shadowLOD( 0 )
, _castShadow( castShadow )
, _parallel( false )
, _mode( mode )
{
	B_ASSERT( range != 0 );
	B_ASSERT( intensity != 0 );
}

//----------------------------------------------------------
GrLight::GrLight( const tstring& name, const MMat4x4& transform, const GrProjection& projection, URef<GrTexture> projTex,
				  const GrColor& color, URef<UExprTerm> intensity, URef<UExprTerm> range, const MSphere& localBounds, float sqrAtten,
				  float linAtten, float constAtten, float haziness, float width, ELIGHTMODE mode, bool parallel, bool castShadow )
: _name( name )
, _transform( transform )
, _projection( projection )
, _position( transform.GetTranslate() )
, _direction( transform.GetRotate().GetRow( 2 ) )
, _projTex( projTex )
, _color( color )
, _cachedColor( color )
, _intensity( intensity )
, _cachedIntensity( 1.0f )
, _range( range )
, _cachedRange( 100.0f )
, _localBounds( localBounds )
, _sqrAtten( sqrAtten )
, _linAtten( linAtten )
, _constAtten( constAtten )
, _haziness( haziness )
, _width( width )
, _shadowLOD( 0 )
, _castShadow( castShadow )
, _parallel( parallel )
, _mode( mode )
{
	B_ASSERT( range != 0 );
	B_ASSERT( intensity != 0 );

	// build the texgen matrix.  This is an offset projection matrix
	// so after projection, coordinates come out in the range of 0..1
	// instead of -1..1.  This is necessary for projected textures to
	// work effectively.
	MakeTexgenMatrix();
}
*/
//----------------------------------------------------------
GrLight::~GrLight()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrLight::Rebuild( const SLightInfo& lightInfo )
{
	_localTransform = lightInfo.transform;
	_projection = lightInfo.projection;
	_position = lightInfo.transform.GetTranslate();
	_direction = lightInfo.transform.GetRotate().GetCol( 2 );
	_projTex = lightInfo.projTex;
	_color = lightInfo.color;
	_cachedColor = lightInfo.color;
	_intensity = lightInfo.intensity;
	_cachedIntensity = 1.0f;
	_range = lightInfo.range;
	_cachedRange = 100.0f;
	_sqrAtten = lightInfo.sqrAtten;
	_linAtten = lightInfo.linAtten;
	_constAtten = lightInfo.constAtten;
	_haziness = lightInfo.haziness;
	_width = lightInfo.width;
	_shadowLOD = 0;
	_castShadow = lightInfo.castShadow;
	_parallel = lightInfo.parallel;
	_global = lightInfo.global;
	_mode = lightInfo.mode;

	// update the world transform.
	UpdateWorldTransform();

	// the range and intensity must be set!
	B_ASSERT( _range != 0 );
	B_ASSERT( _intensity != 0 );
}

//----------------------------------------------------------
GrLight*
GrLight::Clone( const UPath& newId ) const
{
	GrLight* newLight = new GrLight( *this );
	newLight->_id = newId;
	return newLight;
}

//----------------------------------------------------------
void
GrLight::SetLocalTransform( const MMat4x4& transform )
{
	// store the new transform and update with the previous parent
	// matrix.
	_localTransform = transform;
	UpdateWorldTransform();
}

//----------------------------------------------------------
void
GrLight::SetParentTransform( const MMat4x4& transform )
{
	_parentTransform = transform;
	UpdateWorldTransform();
}

//----------------------------------------------------------
void
GrLight::SetProjection( const GrProjection& projection )
{
	_projection = projection;
	MakeTexgenMatrix();
}

//----------------------------------------------------------
void
GrLight::SetProjTexture( URef<GrTexture> projTexture )
{
	_projTex = projTexture;
	MakeTexgenMatrix();
}

//----------------------------------------------------------
void 
GrLight::SetIntensity( float intensity )
{
	SetIntensity( gUExprMgr->CreateNumber( intensity ) );
}

//----------------------------------------------------------
void 
GrLight::SetRange( float range )
{
	SetRange( gUExprMgr->CreateNumber( range ) );
}

//----------------------------------------------------------
void
GrLight::SetParallel( bool parallel )
{
	_parallel = parallel;
	MakeTexgenMatrix();
}

//----------------------------------------------------------
void
GrLight::Update()
{
	B_ASSERT( _range != 0 );
	B_ASSERT( _intensity != 0 );

	// retrieve animated values and cache.
	_cachedIntensity = _intensity->GetValue();
	_cachedColor = _cachedIntensity * _color;
	_cachedRange = _range->GetValue();
	_worldBounds.SetRadius( _cachedRange );
	_localBounds.SetRadius( _cachedRange );
	_projection.SetFarDist( _cachedRange );
}

//----------------------------------------------------------
void
GrLight::UpdateWorldTransform()
{
	// calculate the world transform.
	_worldTransform = _parentTransform * _localTransform;

	// calculate various params.
	_direction = _worldTransform.GetRotate().GetCol( 2 );
	_position = _worldTransform.GetTranslate();
	_localBounds.Transform( _worldBounds, _worldTransform );

	// make sure the texgen matrix gets updated.
	MakeTexgenMatrix();
}


//==========================================================
// protected methods
//==========================================================

//----------------------------------------------------------
void
GrLight::MakeTexgenMatrix()
{
	static const MMat4x4 kPerspectiveBias( 0.5f, 0.0f, 0.0f, 0.5f,
										   0.0f, 0.5f, 0.0f, 0.5f,
										   0.0f, 0.0f, 0.5f, 0.5f,
										   0.0f, 0.0f, 0.0f, 1.0f );

	static const MMat4x4 kParallelBias( 0.5f, 0.0f, 0.0f, 0.5f,
										0.0f, 0.5f, 0.0f, 0.5f,
										0.0f, 0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 0.0f, 1.0f );

	// simply return if the range is too small.
	if ( _cachedRange <= 0.01f )
		return;

	// calculate the light's view matrix.
	MMat3x3 lightViewRot = _worldTransform.GetRotate().Transposed();
	MVec3 lightViewPos = -lightViewRot.RotatePoint( _worldTransform.GetTranslate() );

	if ( _projTex )
	{
		if ( _projection.GetFOV() >= 0.01f )
			_texgenMatrix = kParallelBias * _projection.GetMatrix() * MMat4x4( lightViewRot, lightViewPos );
		else
			_texgenMatrix = kParallelBias * _projection.GetMatrix() * MMat4x4( lightViewRot, lightViewPos );
	}
	else
	{
		_texgenMatrix = MMat4x4( 0.0f, 0.0f, 0.0f, 0.5f,
								 0.0f, 0.0f, 0.0f, 0.5f,
								 0.0f, 0.0f, 0.0f, 0.5f,
								 0.0f, 0.0f, 0.0f, 1.0f );
	}

//	if ( _parallel )
//	{
//		_texgenMatrix( 0,3 ) = 0.5f;
//		_texgenMatrix( 1,3 ) = 0.5f;
//	}
//	else
//	{
//		_texgenMatrix( 0,2 ) = 0.5f;
//		_texgenMatrix( 1,2 ) = 0.5f;
//	}
}
