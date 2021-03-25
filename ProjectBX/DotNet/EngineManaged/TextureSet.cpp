//---------------------------------------------------------------------------------------------
// File:		TextureSet.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//---------------------------------------------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "TextureSet.h"

// project includes.
#include "TextureStage.h"
#include "Utils.h"
#include "Expr.h"

// engine includes.
#include "GrMaterialPass.h"

// .NET namespaces.
using namespace System;

//**********************************************************
// ref class TextureSet
//**********************************************************

//----------------------------------------------------------
Bootstrap::TextureSet::TextureSet( GrMaterialPass* materialPass )
: _pass( materialPass )
{
	_stages = gcnew array< TextureStage^ >( ( int )Stage::Count ) {
		gcnew TextureStage( _pass, ES_BUMP ),
		gcnew TextureStage( _pass, ES_NORMAL ),
		gcnew TextureStage( _pass, ES_DIFFUSE ),
		gcnew TextureStage( _pass, ES_SPECULAR ),
		gcnew TextureStage( _pass, ES_EMISSIVE ),
		gcnew TextureStage( _pass, ES_AMBIENT ),
	};
}

//----------------------------------------------------------
Bootstrap::TextureSet::~TextureSet()
{
	this->!TextureSet();
}

//----------------------------------------------------------
Bootstrap::TextureSet::!TextureSet()
{
	// make sure the texture stage is all cleaned up.
	for ( unsigned int i = 0; i < ( int )Stage::Count; ++i )
		_stages[ i ]->~TextureStage();
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::ScaleS::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetScaleS() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::ScaleS::set( Expr^ value )
{
	_pass->SetTextureScaleS( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::ScaleT::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetScaleT() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::ScaleT::set( Expr^ value )
{
	_pass->SetTextureScaleT( value->_exprTerm );		
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::Rotate::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetRotate() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::Rotate::set( Expr^ value )
{
	_pass->SetTextureRotate( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::RotateCenterS::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetRotateCenterS() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::RotateCenterS::set( Expr^ value )
{
	_pass->SetTextureRotateCenterS( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::RotateCenterT::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetRotateCenterT() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::RotateCenterT::set( Expr^ value )
{
	_pass->SetTextureRotateCenterT( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::TranslateS::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetTranslateS() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::TranslateS::set( Expr^ value )
{
	_pass->SetTextureTranslateS( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::Expr^ 
Bootstrap::TextureSet::TranslateT::get()
{
	return gcnew Expr( _pass->GetTextureSet().GetTranslateT() );
}

//----------------------------------------------------------
void 
Bootstrap::TextureSet::TranslateT::set( Expr^ value )
{
	_pass->SetTextureTranslateT( value->_exprTerm );
}

//----------------------------------------------------------
Bootstrap::TextureStage^ 
Bootstrap::TextureSet::GetTextureStage( Stage stage )
{
	return _stages[ ( int )stage ];
}
