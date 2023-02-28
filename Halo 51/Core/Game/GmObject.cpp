//----------------------------------------------------------
// File:        GmObject.cpp
// Author:      Shawn Presser
// Created:     12-20-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmObject.h"

// physics headers.
#include "PhSubsys.h"

//----------------------------------------------------------
GmObject::SInfo::SInfo()
: density( 1.0f )
{
}

//----------------------------------------------------------
GmObject::GmObject( const tstring& name, URef< GrModel > model, NxActor* actor )
: GmEntity( name, model )
, _actor( actor )
{
    B_ASSERT( actor );
}

//----------------------------------------------------------
GmObject::~GmObject()
{
    // release the physics actor.
    gPhScene->releaseActor( *_actor );
}

//----------------------------------------------------------
void
GmObject::Update()
{
    if ( _model )
    {
        // move the entity's graphical representation to match its physical representation.
        float mat[ 16 ];
        _actor->getGlobalPose().getRowMajor44( mat );
        _model->SetWorld( MMat4x4( mat ) );
        _model->Update();
    }
}
