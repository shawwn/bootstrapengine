//----------------------------------------------------------
// File:		GmEntity.cpp
// Author:		Shawn Presser
// Created:		12-15-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmEntity.h"

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Graphics/GrModel.h"

// physics headers.
#include "Game/PhSubsys.h"

//----------------------------------------------------------
GmEntity::GmEntity( const tstring& name, URef< GrModel > model )
: _name( name )
, _model( model )
{
}

//----------------------------------------------------------
GmEntity::~GmEntity()
{
	// remove the model from the scene.
	if ( _model && _model->GetParent() )
	{
		_model->GetParent()->RemoveChild( _model );
	}
}
