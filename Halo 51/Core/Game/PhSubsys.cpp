//----------------------------------------------------------
// File:		PhSubsys.cpp
// Author:		Shawn Presser
// Created:		12-15-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "PhSubsys.h"

// Bootstrap Engine headers.
#include "Common/MMat4x4.h"
#include "Common/MMat3x3.h"
#include "Common/MVec3.h"
#include "Common/UFastArray.h"
#include "Graphics/GrModel.h"
#include "Graphics/GrMeshInst.h"
#include "Graphics/GrMesh.h"
#include "Graphics/GrMeshVB.h"
#include "Graphics/GrMeshIB.h"
#include "Graphics/GrColor.h"

// PhysX headers.
#include "NxCooking.h"
#include "NxCapsuleController.h"

// project headers.
#include "GmConfig.h"
#include "PhErrorStream.h"
#include "PhStream.h"

// constants.
#define PH_SKIN_WIDTH		0.2f	// meters.

// global variables.
PhSubsys*				gPhSubsys;
PhAllocator*			gPhAllocator;
NxScene*				gPhScene;
NxPhysicsSDK*			gPhSDK;
NxControllerManager*	gPhCharMgr;
NxCookingInterface*		gPhCooking;

// forward declarations.
const char*			getNxSDKCreateError( const NxSDKCreateError& errorCode );

//**********************************************************
// class PhTriggerReport
//**********************************************************
class PhTriggerReport : public NxUserTriggerReport
{
	virtual void onTrigger( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status );
} gPhTriggerReport;

//----------------------------------------------------------
void
PhTriggerReport::onTrigger( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status )
{
	NX_ASSERT( triggerShape.getFlag( NX_TRIGGER_ENABLE ) );
	if ( status & NX_TRIGGER_ON_ENTER )
	{
		NxActor& actor = otherShape.getActor();
		if ( actor.userData )
		{
			NxActor& triggerActor = triggerShape.getActor();
			if ( triggerActor.isDynamic() && triggerActor.userData != NULL )
			{
				//MyObject* Object = (MyObject*)triggerActor.userData;
				//Object->nbTouched++;
			}
		}
	}
	if( status & NX_TRIGGER_ON_LEAVE )
	{
		NxActor& actor = otherShape.getActor();
		if ( actor.userData )
		{
			NxActor& triggerActor = triggerShape.getActor();
			if ( triggerActor.isDynamic() && triggerActor.userData != NULL )
			{
				//MyObject* Object = (MyObject*)triggerActor.userData;
				//Object->nbTouched--;
			}
		}
	}
}

//**********************************************************
// class PhControllerHitReport
//**********************************************************
class PhControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit);
	virtual NxControllerAction  onControllerHit( const NxControllersHit& hit );

} gPhControllerHitReport;

//----------------------------------------------------------
NxControllerAction
PhControllerHitReport::onShapeHit( const NxControllerShapeHit& hit )
{
	if ( hit.shape )
	{
		NxCollisionGroup group = hit.shape->getGroup();
		if( group != PH_GROUP_COLLIDABLE_NON_PUSHABLE )
		{
			NxActor& actor = hit.shape->getActor();
			if ( actor.isDynamic() )
			{
				// we only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
				// useless stress on the solver. It would be possible to enable/disable vertical pushes on
				// particular objects, if the gameplay requires it.
				if ( hit.dir.y == 0.0f )
				{
					NxF32 coeff = 5.0f * actor.getMass() * hit.length;
					actor.addForceAtLocalPos( hit.dir*coeff, NxVec3( 0.0f, 0.0f, 0.0f ), NX_IMPULSE );
					//						actor.addForceAtPos(hit.dir*coeff, hit.controller->getPosition(), NX_IMPULSE);
					//						actor.addForceAtPos(hit.dir*coeff, hit.worldPos, NX_IMPULSE);
				}
			}
		}
	}

	return NX_ACTION_NONE;
}

//----------------------------------------------------------
NxControllerAction
PhControllerHitReport::onControllerHit( const NxControllersHit& hit )
{
	return NX_ACTION_NONE;
}

//**********************************************************
// class PhSubsys
//**********************************************************
//----------------------------------------------------------
PhSubsys::PhSubsys()
{
	B_ASSERT( !gPhSubsys && !gPhAllocator && !gPhScene && !gPhSDK );

	// create a physics allocator.
	gPhAllocator = new PhAllocator();

	// startup physics.
	NxPhysicsSDKDesc desc;
	desc.flags |= NX_SDKF_NO_HARDWARE; // don't use hardware acceleration, to prevent possible driver bugs.
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	PhErrorStream* errorStream = new PhErrorStream();
	gPhSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, gPhAllocator, errorStream, desc, &errorCode );
	if ( !gPhSDK )
	{
		HandleError( "Physics initialization failed.  (Wrong SDK DLL version?)  Error code %d - %s\n", errorCode, getNxSDKCreateError( errorCode ) );
		return;
	}

	// set the physics skin width to 1cm.
	gPhSDK->setParameter( NX_SKIN_WIDTH, 0.01f );

	// create a physics scene.
	NxSceneDesc sceneDesc;
	sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS; // only update active entities each frame, rather than every entity.
	sceneDesc.gravity = ConvertVec3( GAME_GRAVITY );
	sceneDesc.userTriggerReport = &gPhTriggerReport;
	gPhScene = gPhSDK->createScene( sceneDesc );
	if( !gPhScene ) 
	{
		HandleError( "Error: Unable to create a PhysX scene.\n" );
		return;
	}

	// set the default physics material.
	NxMaterial* defaultMaterial = gPhScene->getMaterialFromIndex( 0 );
	defaultMaterial->setRestitution( 0.0f );
	defaultMaterial->setStaticFriction( 0.5f );
	defaultMaterial->setDynamicFriction( 0.5f );

	// create the physics cooking interface.
	gPhCooking = NxGetCookingLib( NX_PHYSICS_SDK_VERSION );
	B_ASSERT( gPhCooking );
	bool success = gPhCooking->NxInitCooking( /*gPhAllocator*/ 0, errorStream );
	B_ASSERT( success );

	// initialize the character controller manager.
	gPhCharMgr = NxCreateControllerManager( gPhAllocator );
	B_ASSERT( gPhCharMgr );

	gPhSubsys = this;
}

//----------------------------------------------------------
PhSubsys::~PhSubsys()
{
	B_ASSERT( gPhSDK && gPhScene && gPhSubsys );

	// release all triangle meshes.
	for ( TriangleMeshContainer::iterator it = _triangleMeshes.begin(); it != _triangleMeshes.end(); ++it )
	{
		NxTriangleMesh* triMesh = *it;
		gPhSDK->releaseTriangleMesh( *triMesh );
	}
	_triangleMeshes.clear();

	// release the character controller manager.
	if ( gPhCharMgr )
	{
		NxReleaseControllerManager( gPhCharMgr );
		gPhCharMgr = 0;
	}

	// release the physics cooking interface.
	gPhCooking->NxCloseCooking();

	// release the physics scene.
	if ( gPhScene )
	{
		gPhSDK->releaseScene( *gPhScene );
		gPhScene = 0;
	}

	// release the physics SDK.
	if ( gPhSDK )
	{
		NxReleasePhysicsSDK( gPhSDK );
		gPhSDK = 0;
	}

	// release the physics allocator.
	if ( gPhAllocator )
	{
		delete gPhAllocator;
		gPhAllocator = 0;
	}

	gPhSubsys = 0;
}

//----------------------------------------------------------
NxActor*
PhSubsys::CreateCube( const MMat4x4& world, float size, const NxBodyDesc& bodyDesc, float density, EPHGROUP group, const MVec3* initialVelocity )
{
	B_ASSERT( gPhScene );

	NxBoxShapeDesc boxDesc;
	float half = 0.5f*size;
	boxDesc.dimensions = NxVec3( half, half, half );
	boxDesc.group = group;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &boxDesc );
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= density;
	actorDesc.globalPose.setRowMajor44( world );

	NxActor* result = gPhScene->createActor( actorDesc );
	return result;
}

//----------------------------------------------------------
bool
PhSubsys::CreateStaticModelActors( NxActorContainer& results, GrModel* model )
{
	// force the model to update its transforms.
	if ( !model->Update( true ) )
	{
		B_ASSERT( false );
		return false;
	}

	// gather all meshinsts within the model.
	UFastArray< GrMeshInst* > meshInsts;
	model->GetAllMeshInsts( meshInsts );

	for ( unsigned int i = 0; i < meshInsts.GetElemCount(); ++i )
	{
		GrMeshInst* meshInst = meshInsts[ i ];

		// get meshinst's mesh data.
		GrMesh* mesh = meshInst->GetMesh();
		URef< GrMeshVB > meshVB( mesh->GetVertexData() );
		URef< GrMeshIB > meshIB( mesh->GetIndexData() );
		UFastArray< SVec3 > vertices;

		// fill out PhysX descriptors.
		NxActorDesc actorDesc;
		NxTriangleMeshShapeDesc meshShapeDesc;
		NxTriangleMeshDesc meshDesc;
		meshDesc.numVertices				= meshVB->GetVertexCount();
		meshDesc.numTriangles				= meshIB->GetIndexCount()/3;
		meshDesc.pointStrideBytes			= sizeof( SVec3 );
		meshDesc.triangleStrideBytes		= 3*sizeof( unsigned short );
		meshDesc.triangles					= meshIB->GetIndices();
		meshDesc.flags						= NX_MF_16_BIT_INDICES;

		// get the meshinst's transform data.
		MMat4x4 transform( meshInst->GetTransform() );
		MVec3 scale( transform.GetScale() );

		// determine whether the meshinst is scaled.
		if ( !ApproxEqual( scale.GetX(), 1.0f ) ||
			 !ApproxEqual( scale.GetY(), 1.0f ) || 
			 !ApproxEqual( scale.GetZ(), 1.0f ) )
		{
			// since the meshinst is scaled, bake its scale into its vertices.
			MMat4x4 scaleXForm( MMat3x3(), MVec3( 0.0f, 0.0f, 0.0f ), scale );
			meshVB->PretransformVertices( vertices, scaleXForm );

			// reset the transform's scale.
			transform.SetScale( MVec3( 1.0f, 1.0f, 1.0f ) );

			// submit the baked vertices to PhysX.
			meshDesc.points = vertices.GetPtr();
		}
		else
		{
			// otherwise, just submit the mesh vertices without modifying them.
			meshDesc.points = meshVB->GetVertices();
		}

		// prepare the mesh for submission to PhysX.
		MemoryWriteBuffer writeBuffer;
		if ( !gPhCooking->NxCookTriangleMesh( meshDesc, writeBuffer ) )
		{
			HandleError( "Unable to cook a triangle mesh.\n" );
			return false;
		}
		// create the triangle mesh.
		NxTriangleMesh* triMesh = gPhSDK->createTriangleMesh( MemoryReadBuffer( writeBuffer.data ) );
		if ( !triMesh )
		{
			HandleError( "Unable to create a triangle mesh.\n" );
			return false;
		}

		// add the triangle mesh to our global list of triangle meshes.
		_triangleMeshes.push_back( triMesh );

		meshShapeDesc.meshData = triMesh;

		// move and rotate the mesh to match the meshinst.
		meshShapeDesc.localPose = ConvertMat4x4( transform );

		//
		// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
		// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
		//
		// TODO: don't leak memory.

		// flag the physics representation as "collidable, but not movable."
		meshShapeDesc.group = PH_GROUP_COLLIDABLE_NON_PUSHABLE;

		// create one actor per meshinst.
		// TODO: why can't there be multiple triangle mesh shapes for a single actor?
		actorDesc.shapes.pushBack( &meshShapeDesc);
		NxActor* actor = gPhScene->createActor( actorDesc );
		results.Push( actor );
	}

	return true;
}

//----------------------------------------------------------
NxActor*
PhSubsys::CreateConvexDynamicModelActor( GrModel* model, const NxBodyDesc& bodyDesc, float density, EPHGROUP group )
{
	// temporarily move the model to origin.
	MMat4x4 modelXForm( model->GetWorld() );
	model->SetWorld( MMat4x4() );

	// force the model to update its transforms.
	bool success = model->Update( true );
	B_ASSERT( success );

	// gather all meshinsts within the model.
	UFastArray< GrMeshInst* > meshInsts;
	model->GetAllMeshInsts( meshInsts );

	// fill out a PhysX actor descriptor.
	NxActorDesc actorDesc;
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= density;
	actorDesc.globalPose	= ConvertMat4x4( modelXForm );

	for ( unsigned int i = 0; i < meshInsts.GetElemCount(); ++i )
	{
		GrMeshInst* meshInst = meshInsts[ i ];

		// get meshinst's mesh data.
		GrMesh* mesh = meshInst->GetMesh();
		URef< GrMeshVB > meshVB( mesh->GetVertexData() );
		URef< GrMeshIB > meshIB( mesh->GetIndexData() );
		UFastArray< SVec3 > vertices;

		// fill out a PhysX convex hull descriptor.
		NxConvexMeshDesc convexDesc;
		convexDesc.numVertices			= meshVB->GetVertexCount();
		convexDesc.pointStrideBytes		= sizeof( SVec3 );
		convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

		// get the meshinst's transform data.
		MMat4x4 transform( meshInst->GetTransform() );
		MVec3 scale( transform.GetScale() );

		// determine whether the meshinst is scaled.
		if ( !ApproxEqual( scale.GetX(), 1.0f ) ||
			 !ApproxEqual( scale.GetY(), 1.0f ) || 
			 !ApproxEqual( scale.GetZ(), 1.0f ) )
		{
			// since the meshinst is scaled, bake its scale into its vertices.
			MMat4x4 scaleXForm( MMat3x3(), MVec3( 0.0f, 0.0f, 0.0f ), scale );
			meshVB->PretransformVertices( vertices, scaleXForm );

			// reset the transform's scale.
			transform.SetScale( MVec3( 1.0f, 1.0f, 1.0f ) );

			// submit the baked vertices to PhysX.
			convexDesc.points = vertices.GetPtr();
		}
		else
		{
			// otherwise, just submit the mesh vertices without modifying them.
			convexDesc.points = meshVB->GetVertices();
		}
	
		// cook the convex hull shape.
		MemoryWriteBuffer buf;
		if ( !gPhCooking->NxCookConvexMesh( convexDesc, buf ) )
		{
			HandleError( "Failed to cook a convex mesh.\n" );
			return 0;
		}

		// create the convex hull shape.
		NxConvexMesh* convexShape = gPhSDK->createConvexMesh( MemoryReadBuffer( buf.data ) );
		if ( !convexShape )
		{
			HandleError( "Failed to create a convex mesh.\n" );
			return 0;
		}

		// prepare to add the convex hull shape to the actor descriptor.
		NxConvexShapeDesc convexShapeDesc;
		convexShapeDesc.meshData = convexShape;
		convexShapeDesc.group = group;

		// move and rotate the convex hull to match the meshinst.
		convexShapeDesc.localPose = ConvertMat4x4( transform );

		// add the convex hull.
		actorDesc.shapes.pushBack( &convexShapeDesc );
	}

	// move the model back to its original orientation, and ensure that its transforms are updated.
	model->SetWorld( modelXForm );
	model->Update( true );

	// create and return the actor.
	NxActor* result = gPhScene->createActor( actorDesc );
	return result;
}

//----------------------------------------------------------
NxController*
PhSubsys::CreateCharController( const MVec3& pos, float radius, float height, void* userData, EPHGROUP group )
{
	// (Note: the below commented-out lines of code are from a PhysX SDK sample.  
	// I'm going to leave them in for now.)

	NxCapsuleControllerDesc desc;
	desc.position.x		= pos.GetX();
	desc.position.y		= pos.GetY();
	desc.position.z		= pos.GetZ();
	desc.radius			= radius;
	desc.height			= height;
	desc.upDirection	= NX_Y;
	desc.userData		= userData;
	//		desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
	desc.slopeLimit		= 0;
	desc.skinWidth		= PH_SKIN_WIDTH;
	desc.stepOffset		= 0.5;
	desc.stepOffset		= radius * 0.5f;
	//	desc.stepOffset	= 0.01f;
	//		desc.stepOffset		= 0;	// Fixes some issues
	//		desc.stepOffset		= 10;
	desc.callback		= &gPhControllerHitReport;
	NxController* result = gPhCharMgr->createController( gPhScene, desc );
	return result;
}

//----------------------------------------------------------
NxU32
PhSubsys::MoveChar( NxController* controller, const MVec3& displacement, unsigned int collisionGroups )
{
	const NxF32 sharpness = 1.0f;
	NxVec3 disp( displacement );
	NxU32 outCollisionFlags = 0;
	controller->move( disp, collisionGroups, 0.000001f, outCollisionFlags, sharpness );
	return outCollisionFlags;
}

//----------------------------------------------------------
MVec3
PhSubsys::GetCharPosition( NxController* controller )
{
	const NxExtendedVec3& pos = controller->getPosition();
	return MVec3( (float)pos[ 0 ], (float)pos[ 1 ], (float)pos[ 2 ] );
}

//----------------------------------------------------------
void
PhSubsys::CollectGarbage()
{
	for ( TriangleMeshContainer::iterator it = _triangleMeshes.begin(); it != _triangleMeshes.end(); )
	{
		NxTriangleMesh* triMesh = *it;
		if ( triMesh->getReferenceCount() == 0 )
		{
			// if the triangle mesh is unreferenced, then release it.
			gPhSDK->releaseTriangleMesh( *triMesh );
			it = _triangleMeshes.erase( it );
		}
		else
		{
			++it;
		}
	}
}

//----------------------------------------------------------
NxVec3
PhSubsys::ConvertVec3( const MVec3& vec )
{
	return NxVec3( vec.GetX(), vec.GetY(), vec.GetZ() );
}

//----------------------------------------------------------
MVec3
PhSubsys::ConvertVec3( const NxVec3& vec )
{
	return MVec3( vec.x, vec.y, vec.z );
}

//----------------------------------------------------------
NxMat33
PhSubsys::ConvertMat3x3( const MMat3x3& rot )
{
	return NxMat33( 
		ConvertVec3( rot.GetRow( 0 ) ), 
		ConvertVec3( rot.GetRow( 1 ) ),
		ConvertVec3( rot.GetRow( 2 ) ) );
}

//----------------------------------------------------------
NxMat34
PhSubsys::ConvertMat4x4( const MMat4x4& mat )
{
	// verify that the matrix doesn't contain projection.
	B_ASSERT(	ApproxEqual( mat( 3, 0 ), 0.0f ) &&
				ApproxEqual( mat( 3, 1 ), 0.0f ) &&
				ApproxEqual( mat( 3, 2 ), 0.0f ) &&
				ApproxEqual( mat( 3, 3 ), 1.0f ) );

	// build an equivalent PhysX matrix, but without any scale.
	NxMat34 result(
		ConvertMat3x3( mat.GetRotate() ),
		ConvertVec3( mat.GetTranslate() ) );

#if TRANSFORMS_USE_SCALE
	// now take scale into account.
	MVec3 scale( mat.GetScale() );
	for ( unsigned int axis = 0; axis < 3; ++axis )
		for ( unsigned int col = 0; col < 3; ++col )
			result.M( axis, col ) *= scale( col );
#endif

	// return the final PhysX matrix that represents our MMat4x4.
	return result;
}

//----------------------------------------------------------
GrColor
PhSubsys::ConvertColor( NxU32 col, float alpha )
{
	GrColor result;
	result.SetB( ( ( col       ) & 0xff ) / 255.0f );
	result.SetG( ( ( col >>  8 ) & 0xff ) / 255.0f );
	result.SetR( ( ( col >> 16 ) & 0xff ) / 255.0f );
	result.SetA( alpha );
	return result;
}

//----------------------------------------------------------
const char*
getNxSDKCreateError(const NxSDKCreateError& errorCode) 
{
	switch(errorCode) 
	{
	case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
	case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
	case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
	case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
	case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
	case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
	case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
	default: return "Unknown error";
	}
};
