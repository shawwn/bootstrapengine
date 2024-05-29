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
#include "PxPhysics.h"
#include "PxControllerManager.h"
#include "PxController.h"
#include "PxScene.h"
#include "PxCooking.h"
#include "foundation/PxSimpleTypes.h"
#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"
#include "foundation/PxMat44.h"

// project headers.
#include "PhAllocator.h"

using namespace physx;

// std c++ headers.
#include <list>

// global typedefs.
typedef UFastArray< PxActor* >      PxActorContainer;

// forward declarations.
class MMat4x4;
class MMat3x3;
class MVec3;
class GrModel;
class GrColor;
class PxTriangleMesh;

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
    PxActor*                    CreateCube( const MMat4x4& world, float size, float density, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE, const MVec3* initialVelocity=0, float angularDamping = 0.0 );

    // creates an actor that represents a static model.
    bool                        CreateStaticModelActors( PxActorContainer& results, GrModel* model );

    // creates an actor that represents a dynamic, rigid model.
    PxActor*                    CreateConvexDynamicModelActor( GrModel* model, PxReal angularDamping, const PxVec3& linearVelocity, float density, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE );

    // creates a capsule character controller.
    PxController*               CreateCharController( const MVec3& pos, float radius, float height, void* userData, EPHGROUP group = PH_GROUP_COLLIDABLE_PUSHABLE );

    // moves a character controller.
    PxU32                       MoveChar( PxController* controller, const MVec3& displacement, unsigned int collisionGroups = PH_GROUP_COLLIDABLE );

    // returns the position of a character controller.
    MVec3                       GetCharPosition( PxController* controller );

    // releases any unreferenced triangle meshes.
    void                        CollectGarbage();

    // converts an MVec3 into an PxVec3, or vice-versa.
    static PxVec3               ConvertVec3( const MVec3& vec );
    static MVec3                ConvertVec3( const PxVec3& vec );

    // converts an MMat3x3 into an PxMat33.
    static PxMat33              ConvertMat3x3( const MMat3x3& rot );

    // converts an MMat4x4 into an PxMat34.
    static PxMat44              ConvertMat4x4( const MMat4x4& mat );

    // converts an PxU32 into a GrColor.
    static GrColor              ConvertColor( PxU32 col, float alpha = 1.0f );

private:
    typedef std::list< PxTriangleMesh* >    TriangleMeshContainer;

    TriangleMeshContainer       _triangleMeshes;
};
extern PhSubsys*                gPhSubsys;
extern PhAllocator*             gPhAllocator;
extern PxScene*                 gPhScene;
extern PxPhysics*               gPhSDK;
extern PxControllerManager*     gPhCharMgr;
extern PxCooking*      gPhCooking;
