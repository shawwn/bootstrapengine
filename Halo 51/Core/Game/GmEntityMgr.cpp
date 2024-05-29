//----------------------------------------------------------
// File:        GmEntityMgr.cpp
// Author:      Shawn Presser
// Created:     12-15-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "GmEntityMgr.h"

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/URand.h"
#include "Graphics/GrModelMgr.h"
#include "Graphics/GrModel.h"
#include "Graphics/GrScene.h"
#include "Graphics/GrRenderUtil.h"

// physics headers.
#include "Game/PhSubsys.h"

// project headers.
#include "GmAI.h"

// global variables.
GmEntityMgr*            gGmEntityMgr;

// external variables.
extern URand*           gRand;

//----------------------------------------------------------
MVec3
GenRandomVector( float xRange, float yRange, float zRange )
{
    return MVec3(   xRange*( 2.0f*gRand->RandFloat() - 1.0f ),
                    yRange*( 2.0f*gRand->RandFloat() - 1.0f ),
                    zRange*( 2.0f*gRand->RandFloat() - 1.0f ) );
}

//----------------------------------------------------------
GmEntityMgr::GmEntityMgr()
{
    B_ASSERT( !gGmEntityMgr );

    gGmEntityMgr = this;
}

//----------------------------------------------------------
GmEntityMgr::~GmEntityMgr()
{
    B_ASSERT( gGmEntityMgr );

    // remove all entities.
    ClearEntities();

    gGmEntityMgr = 0;
}

//----------------------------------------------------------
GmObject*
GmEntityMgr::CreateDynamicObject( const tstring& name, const GmObject::SInfo& info )
{
    // load the model.
    URef< GrModel > model( GetModel( info.modelFilePath, name, info.transform ) );

    // create a physics representation for that entity.
    PxActor* actor = gPhSubsys->CreateConvexDynamicModelActor( model, info.angularDamping, info.linearVelocity, info.density, PH_GROUP_COLLIDABLE_PUSHABLE );
    if ( !actor )
    {
        HandleError( "Failed to create a physics representation for entity %s model %s\n", 
            name.c_str(), info.modelFilePath.GetPathString().c_str() );
        return 0;
    }

    // create a game object instance.
    GmObject* result = new GmObject( name, model, actor );

    // store the entity in the userdata of the physics actor.
    actor->userData = (void*)result;

    // add the game object to the global list of game objects.
    _objects.push_back( result );

    // return it.
    return result;
}

//----------------------------------------------------------
GmCharacter*
GmEntityMgr::CreateCharacter( const tstring& name, const GmCharacter::SInfo& info )
{
    // load the model.
    URef< GrModel > model( GetModel( info.modelFilePath, name, MMat4x4( info.position ) ) );

    // create a character instance.
    GmCharacter* character = new GmCharacter( name, model, info.inputController );

    // store the character's speed.
    character->SetSpeed( info.speed );

    // create a character controller, storing the character instance in its userdata.
    PxController* controller = gPhSubsys->CreateCharController( info.position, info.radius, info.height, character );
    if ( !controller )
    {
        // report the error.
        HandleError( "Failed to create a character controller for entity %s model %s\n",
            name.c_str(), info.modelFilePath.GetPathString().c_str() );

        // cleanup and return.
        delete character;
        return 0;
    }

    // store the controller in the character instance.
    character->_physicsController = controller;

    // add the character to the global list of characters.
    _characters.push_back( character );

    // return it.
    return character;
}

//----------------------------------------------------------
GmAI*
GmEntityMgr::CreateAICharacter( const tstring& name, const GmCharacter::SInfo& info )
{
    GmAI* ai = new GmAI( CreateCharacter( name, info ) );
    ai->SetDestination( GenRandomVector( 20.0f, 0.0f, 20.0f ) );
    _brains.push_back( ai );
    return ai;
}

//----------------------------------------------------------
void
GmEntityMgr::ClearEntities()
{
    // delete all game objects.
    for ( EntityContainer::iterator it = _objects.begin(); it != _objects.end(); ++it )
    {
        GmEntity* entity = *it;
        delete entity;
    }
    _objects.clear();

    // delete all AIs.
    for ( AIContainer::iterator it = _brains.begin(); it != _brains.end(); ++it )
    {
        GmAI* ai = *it;
        delete ai;
    }
    _brains.clear();

    // delete all game characters.
    for ( EntityContainer::iterator it = _characters.begin(); it != _characters.end(); ++it )
    {
        GmEntity* entity = *it;
        delete entity;
    }
    _characters.clear();
}

//----------------------------------------------------------
void
GmEntityMgr::Update( unsigned int dt )
{
    // get a list of actors that were active.
    PxU32 transformsCount = 0;
    PxActiveTransform* activeTransforms = gPhScene->getActiveTransforms( transformsCount );

    // update only those entities.
    if ( transformsCount )
    {
        B_ASSERT( activeTransforms );
        for( PxU32 i = 0; i < transformsCount; ++i )
        {
            // the user data of the actor holds the game entity pointer.
            GmEntity* entity = (GmEntity*)activeTransforms[i].userData;

            // update the game entity's visual representation to match its physics representation.
            if ( entity )
            {
                entity->Update();
            }
        }
    }

    // update each AI-controlled character.
    for ( AIContainer::iterator it = _brains.begin(); it != _brains.end(); ++it )
    {
        GmAI* ai = *it;
        ai->Update( dt );

        // if the AI has reached its last waypoint, generate a new waypoint.
        if ( !ai->IsActive() )
        {
            ai->SetDestination( GenRandomVector( 20.0f, 0.0f, 20.0f ) );
        }
    }
}

//----------------------------------------------------------
void
GmEntityMgr::DebugRender()
{
    for ( AIContainer::iterator it = _brains.begin(); it != _brains.end(); ++it )
    {
        GmAI* ai = *it;
        gGrRenderUtil->DrawColoredLine( ai->GetDestination(), ai->GetDestination() + MVec3( 0.0f, 2.0f, 0.0f ), GrColor( 0.10f, 0.15f, 0.85f, 0.90f ) );
    }
}

//----------------------------------------------------------
URef< GrModel >
GmEntityMgr::GetModel( const UPath& path, const UPath& instanceName, const MMat4x4& transform )
{
    // if there is no model, return null.
    if ( path.GetPathString().empty() )
        return 0;

    // instantiate the model.
    URef< GrModel > model = gGrModelMgr->GetModel( path, instanceName, true, true );
    if ( !model )
    {
        HandleError( "Failed to load model %s\n", path.GetPathString().c_str() );
        return 0;
    }

    // set its initial transform.
    model->SetWorld( transform );

    // ensure that the model is updated.
    model->Update();

    // add the model to the scene.
    gGrScene->GetRootModel()->AddChild( model );

    // return the model.
    return model;
}
