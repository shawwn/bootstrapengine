//----------------------------------------------------------
// File:		GmSubsys.cpp
// Author:		Shawn Presser
// Created:		12-20-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmSubsys.h"

// project headers.
#include "GmConfig.h"
#include "GmEntityMgr.h"

// Bootstrap Engine headers.
#include "Common/URef.h"
#include "Common/UExprMgr.h"
#include "Common/RFileMgr.h"
#include "Common/RRsrcMgr.h"
#include "Graphics/GrConfig.h"
#include "Graphics/GrScene.h"

// physics headers.
#include "PhSubsys.h"

// global variables.
GmSubsys*					gGmSubsys;

//----------------------------------------------------------
GmSubsys::GmSubsys()
{
	B_ASSERT( !gGmSubsys );

	// initialize managers.
	new UExprMgr();
	new RFileMgr( "C:\\bin\\game\\" );
	new RRsrcMgr();

	// initialize physics.
	new PhSubsys();

	// initialize game managers.
	new GmEntityMgr();

	gGmSubsys = this;
}

//----------------------------------------------------------
GmSubsys::~GmSubsys()
{
	B_ASSERT( gGmSubsys );

	// unload the current scene.
	Clear();

	// shutdown game managers.
	delete gGmEntityMgr;

	// shutdown physics.
	delete gPhSubsys;

	// shutdown managers.
	delete gRRsrcMgr;
	delete gRFileMgr;
	delete gUExprMgr;

	gGmSubsys = 0;
}

//----------------------------------------------------------
bool
GmSubsys::LoadScene( const UPath& scenePath )
{
	// unload the current scene.
	Clear();

	// load the new scene.
	tstring errors;
	if ( !gRRsrcMgr->LoadScript( scenePath + GR_SCENEDEF_FILENAME, GR_KW_SCENEDEF, errors ) )
	{
		B_ASSERT( false );
		return false;
	}

	// create a physics triangle mesh representation of the scene.
	if ( !gPhSubsys->CreateStaticModelActors( _staticActors, gGrScene->GetRootModel() ) )
	{
		B_ASSERT( false );
		Clear();
		return false;
	}

	return true;
}

//----------------------------------------------------------
void
GmSubsys::Update( unsigned int dt )
{
	const float t = dt / 1000.0f;

	// simulate a physics step (non blocking).
	gPhScene->simulate( t );

	// fetch physics simulation results.
	gPhScene->flushStream();
	gPhScene->fetchResults( NX_RIGID_BODY_FINISHED, true );

	NxReal maxTimestep;
	NxTimeStepMethod method;
	NxU32 maxIter;
	NxU32 numSubSteps;
	gPhScene->getTiming( maxTimestep, maxIter, method, &numSubSteps );
	if( numSubSteps )
		gPhCharMgr->updateControllers();

	// update the entities.
	gGmEntityMgr->Update( dt );
}

//----------------------------------------------------------
void
GmSubsys::Clear()
{
	// clear all game entities.
	gGmEntityMgr->ClearEntities();

	// remove all static actors from the physics scene.
	for ( unsigned int i = 0; i < _staticActors.GetElemCount(); ++i )
	{
		NxActor* actor = _staticActors[i];
		gPhScene->releaseActor( *actor );
	}
	_staticActors.Clear();

	// remove any unreferenced triangle meshes.
	gPhSubsys->CollectGarbage();

	// unload the current scene.
	gGrScene->Clear();
}
