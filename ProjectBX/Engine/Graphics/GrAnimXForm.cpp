//----------------------------------------------------------
// File:		GrAnimXForm.h
// Author:		Kevin Bray
// Created:		04-08-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrAnimXForm.h"

// project includes.
#include "MMat4x4.h"
#include "MMat3x3.h"


//**********************************************************
// class GrAnimXForm
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrAnimXForm::GrAnimXForm()
: _scale( 0.0f, 0.0f, 0.0f )
{

}

//----------------------------------------------------------
GrAnimXForm::GrAnimXForm( const MQuat& rotate, const MVec3& translate, const MVec3& scale )
: _rotate( rotate )
, _translate( translate )
, _scale( scale )
{

}

//----------------------------------------------------------
GrAnimXForm::GrAnimXForm( const MMat4x4& xform )
: _rotate( xform.GetRotate() )
, _translate( xform.GetTranslate() )
, _scale( xform.GetScale() )
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrAnimXForm&
GrAnimXForm::operator = ( const MMat4x4& xform )
{
	_rotate = xform.GetRotate();
	_translate = xform.GetTranslate();
	_scale = xform.GetScale();
	return *this;
}

//----------------------------------------------------------
void
GrAnimXForm::SetRotate( const MQuat& rotate )
{
	_rotate = rotate;
}

//----------------------------------------------------------
void
GrAnimXForm::SetTranslate( const MVec3& translate )
{
	_translate = translate;
}

//----------------------------------------------------------
void
GrAnimXForm::SetScale( const MVec3& scale )
{
	_scale = scale;
}

//----------------------------------------------------------
void
GrAnimXForm::SetToIdentity()
{
	_rotate.SetX( 0.0f );
	_rotate.SetY( 0.0f );
	_rotate.SetZ( 0.0f );
	_rotate.SetW( 1.0f );
	_translate.SetX( 0.0f );
	_translate.SetY( 0.0f );
	_translate.SetZ( 0.0f );
	_scale.SetX( 0.0f );
	_scale.SetY( 0.0f );
	_scale.SetZ( 0.0f );
}

//----------------------------------------------------------
void
GrAnimXForm::BlendToFast( const GrAnimXForm& xform, float t )
{
	float oneMinusT = ( 1.0f - t );
	_rotate.SlerpToFast( xform._rotate, t );
	_translate = oneMinusT * _translate + t * xform._translate;
	_scale = oneMinusT * _scale + t * xform._scale;
}

//----------------------------------------------------------
void
GrAnimXForm::Accumulate( const GrAnimXForm& xform )
{
	_rotate = _rotate * xform._rotate;
	_translate = _translate + xform._translate;
	_scale += xform._scale;
}

//----------------------------------------------------------
void
GrAnimXForm::BuildMatrix( MMat4x4& mat )
{
	static const MVec3 kOne( 1.0f, 1.0f, 1.0f );
	_rotate.SetMatrixRot( mat );
	mat.SetTranslate( _translate );
	mat.SetScale( _scale + kOne );
}
