//----------------------------------------------------------
// File:		Brush.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Brush.h"

// project includes.
#include "Utils.h"
#include "UberTextureRasterizer.h"

// edlib includes.
#include "EdUberTexRasterizer.h"
#include "EdBrushMgr.h"
#include "EdBrush.h"
#include "EdBrushImage.h"
#include "EdSave.h"

// engine includes.
#include "GrTypes.h"
#include "GrImage.h"
#include "GrPolygon.h"

using namespace System;

//**********************************************************
// ref class Brush
//**********************************************************

//----------------------------------------------------------
Bootstrap::Brush::Brush( EdBrush* brush )
: _brush( brush )
{
	B_ASSERT( brush );

	// increment the brush's reference count.
	_brush->IncRef();
}

//----------------------------------------------------------
Bootstrap::Brush::Brush( String^ name )
{
	tstring errors;
	_brush = gEdBrushMgr->GetBrush( StrToTStr( name ), errors );
	
	// increment the brush's reference count.
	_brush->IncRef();
}

//----------------------------------------------------------
Bootstrap::Brush::~Brush()
{
	// decrement the brush's reference count.
	this->!Brush();
}

//----------------------------------------------------------
Bootstrap::Brush::!Brush()
{
	// decrement the brush's reference count.
	_brush->DecRef();
}

//----------------------------------------------------------
Bootstrap::Brush^ 
Bootstrap::Brush::Create( String^ name, String^ category )
{
	// set the brush.
	String^ fullName = category + "/" + name;
	EdBrush::SInfo info;
	info.vertexProgram = 0;
	info.fragmentProgram = 0;

	// return a pointer to the new brush.
	URef< EdBrush > brush = gEdBrushMgr->SetBrush( StrToTStr( fullName ), info );
	return gcnew Brush( brush );
}

//----------------------------------------------------------
array< String^ >^ 
Bootstrap::Brush::GetBrushNameList()
{
	// iterate through each brush and build our managed array.
	unsigned int count = gEdBrushMgr->GetBrushCount();
	array< String^ >^ newArray = gcnew array< String^ >( count );

	// populate the list of brushes.
	for ( unsigned int i = 0; i < count; ++i )
	{
		URef< EdBrush > brush = gEdBrushMgr->GetBrushByIdx( i ); 
		newArray->SetValue( TStrToStr( brush->GetName() ), ( int )i );
	}

	// return the new array.
	return newArray;
}

//----------------------------------------------------------
void 
Bootstrap::Brush::SaveBrushes()
{
	EdSave save;
	save.SaveBrushData();
}

//----------------------------------------------------------
String^ 
Bootstrap::Brush::Name::get()
{
	return TStrToStr( _brush->GetName() );
}

//----------------------------------------------------------
String^ 
Bootstrap::Brush::ShortName::get()
{
	return TStrToStr( _brush->GetShortName() );
}

//----------------------------------------------------------
void 
Bootstrap::Brush::SetImage( Stage stage, String^ fileName )
{
	UPath filePath( StrToTStr( fileName ) );
	if( filePath.GetPathString().length() > 0 )
	{
		// set the image to the brush.
		URef< EdBrushImage > brushImage = gEdBrushMgr->GetBrushImage( filePath );
		_brush->SetStageSource( GetEngineStage( stage ), brushImage );
	}
	else
	{
		// clear the brush image.
		_brush->SetStageSource( GetEngineStage( stage ), 0 );
	}
}

//----------------------------------------------------------
String^ 
Bootstrap::Brush::GetImageName( Stage stage )
{
	// get the image's name from the brush.
	URef< EdBrushImage > brushImage = _brush->GetStageSource( GetEngineStage( stage ) );
	if ( !brushImage )
		return gcnew String( "" );
	return TStrToStr( brushImage->GetFilePath().GetPathString() );		
}

//----------------------------------------------------------
bool 
Bootstrap::Brush::HasImage( Stage stage )
{
	return (_brush->GetStageSource( GetEngineStage( stage ) ) != 0);
}

//----------------------------------------------------------
void 
Bootstrap::Brush::SetMask( System::String^ fileName )
{
	UPath filePath( StrToTStr( fileName ) );
	if( filePath.GetPathString().length() > 0 )
	{
		// set the image to the brush mask.
		URef< EdBrushImage > brushImage = gEdBrushMgr->GetBrushImage( filePath );
		_brush->SetMask( brushImage );
	}
	else
	{
		// clear the brush image.
		_brush->SetMask( 0 );
	}
}

//----------------------------------------------------------
System::String^	
Bootstrap::Brush::GetMaskName()
{
	// get the mask's name from the brush.
	URef< EdBrushImage > brushMask = _brush->GetMask();
	if ( !brushMask )
		return gcnew String( "" );
	return TStrToStr( brushMask->GetFilePath().GetPathString() );		
}

//----------------------------------------------------------
bool 
Bootstrap::Brush::HasMask()
{
	return (_brush->GetMask() != 0);
}

//----------------------------------------------------------
void 
Bootstrap::Brush::Apply( EdUberTexRasterizer* rasterizer )
{
	rasterizer->SetBrush( _brush );
}

//----------------------------------------------------------
Bootstrap::VertexProgram^ 
Bootstrap::Brush::VertexProgram::get() 
{
	return gcnew Bootstrap::VertexProgram( static_cast< GrSoftwareRasterizer::VertexProgram >( _brush->GetVertexProgram() ) );
}

//----------------------------------------------------------
Bootstrap::FragmentProgram^ 
Bootstrap::Brush::FragmentProgram::get()
{
	return gcnew Bootstrap::FragmentProgram( static_cast< GrSoftwareRasterizer::FragmentProgram >( _brush->GetFragmentProgram() ) );
}

//----------------------------------------------------------
ESTAGE 
Bootstrap::Brush::GetEngineStage( Stage stage )
{
	if ( stage == Stage::Bump )
		return ES_BUMP;
	else if ( stage == Stage::Normal )
		return ES_NORMAL;
	else if ( stage == Stage::Diffuse )
		return ES_DIFFUSE;
	else if ( stage == Stage::Specular )
		return ES_SPECULAR;
	else if ( stage == Stage::Emissive )
		return ES_EMISSIVE;
	else if ( stage == Stage::Ambient )
		return ES_AMBIENT;		
	return ES_COUNT;
}
