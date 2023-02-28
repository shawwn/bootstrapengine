//----------------------------------------------------------
// File:        PhSubsys.h
// Author:      Shawn Presser
// Created:     12-15-08
//
// Purpose:     To manage game physics.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UFastArray.h"

// PhysX headers.
#include "NxPhysics.h"
#include "NxControllerManager.h"
#include "NxController.h"

// project headers.
#include "PhAllocator.h"

// std c++ headers.
#include <list>

// global typedefs.
typedef UFastArray< NxActor* >      NxActorContainer;

// forward declarations.
class MMat4x4;
class MMat3x3;
class MVec3;
class GrModel;
class GrColor;
class NxTriangleMesh;

//**********************************************************
// enum EPHGROUP
//**********************************************************
enum EPHGROUP
{
    PH_GROUP_NON_COLLIDABLE,
    PH_GROUP_COLLIDABLE_NON_PUSHABLE,
    PH_GROUP_COLLIDABLE_PUSHABLE,
};
#define PH_GROUP_COLLIDABLE ( 1 << PH_GROUP_COLLIDABLE_NON_PUSHABLE ) | ( 1 << PH_GROUP_COLLIDABLE_PUSHABLE )


//**********************************************************
// class PhSubsys
//**********************************************************
class PhSubsys
{
public:
    // ctor & dtor.
    PhSubsys();
    ~PhSubsys();

    // creates a cube actor.
    NxActor*                    CreateCube( const MMat4x4& world, float size, const NxBodyDesc& bodyDesc, float density, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE, const MVec3* initialVelocity=0 );

    // creates an actor that represents a static model.
    bool                        CreateStaticModelActors( NxActorContainer& results, GrModel* model );

    // creates an actor that represents a dynamic, rigid model.
    NxActor*                    CreateConvexDynamicModelActor( GrModel* model, const NxBodyDesc& bodyDesc, float density, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE );

    // creates a capsule character controller.
    NxController*               CreateCharController( const MVec3& pos, float radius, float height, void* userData, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE );

    // moves a character controller.
    NxU32                       MoveChar( NxController* controller, const MVec3& displacement, unsigned int collisionGroups = PH_GROUP_COLLIDABLE );

    // returns the position of a character controller.
    MVec3                       GetCharPosition( NxController* controller );

    // releases any unreferenced triangle meshes.
    void                        CollectGarbage();

    // converts an MVec3 into an NxVec3, or vice-versa.
    static NxVec3               ConvertVec3( const MVec3& vec );
    static MVec3                ConvertVec3( const NxVec3& vec );

    // converts an MMat3x3 into an NxMat33.
    static NxMat33              ConvertMat3x3( const MMat3x3& rot );

    // converts an MMat4x4 into an NxMat34.
    static NxMat34              ConvertMat4x4( const MMat4x4& mat );

    // converts an NxU32 into a GrColor.
    static GrColor              ConvertColor( NxU32 col, float alpha = 1.0f );

private:
    typedef std::list< NxTriangleMesh* >    TriangleMeshContainer;

    TriangleMeshContainer       _triangleMeshes;
};
extern PhSubsys*                gPhSubsys;
extern PhAllocator*             gPhAllocator;
extern NxScene*                 gPhScene;
extern NxPhysicsSDK*            gPhSDK;
extern NxControllerManager*     gPhCharMgr;
extern NxCookingInterface*      gPhCooking;
