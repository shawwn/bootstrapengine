//----------------------------------------------------------
// File:		EdImportDAEScene.h
// Author:		Kevin Bray
// Created:		11-28-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdImportDAEScene.h"

// project includes.
//#include "EConsole.h"
#include "EdImportDAEGeometries.h"
#include "EdImportDAEMaterials.h"
#include "GrPolygonNode.h"
#include "GrPolygonMesh.h"
#include "GrPolygonGroup.h"
#include "GrMaterialMgr.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrLightMgr.h"
#include "GrLight.h"
#include "GrKFAnimData.h"
#include "GrKFAnim.h"

// FCollada includes.
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4996 )
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDSceneNodeTools.h"
#include "FCDocument/FCDEntityInstance.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDMaterialInstance.h"
#include "FCDocument/FCDLight.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDSkinController.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometrySource.h"
#include "FMath/FMQuaternion.h"
#include "FUtils/FUDAEEnum.h"
#pragma warning ( default : 4996 )
#pragma warning ( default : 4267 )

struct EdImportDAEScene::SAnimNode
{
	FCDSceneNode* node;
	FMMatrix44 invBindMatrix;
};


//----------------------------------------------------------
MMat4x4
MakeMat4x4( const FMMatrix44& matrix )
{
#if 1
	return MMat4x4( matrix[ 0 ][ 0 ], matrix[ 1 ][ 0 ], matrix[ 2 ][ 0 ], matrix[ 3 ][ 0 ],
					matrix[ 0 ][ 1 ], matrix[ 1 ][ 1 ], matrix[ 2 ][ 1 ], matrix[ 3 ][ 1 ],
					matrix[ 0 ][ 2 ], matrix[ 1 ][ 2 ], matrix[ 2 ][ 2 ], matrix[ 3 ][ 2 ],
					matrix[ 0 ][ 3 ], matrix[ 1 ][ 3 ], matrix[ 2 ][ 3 ], matrix[ 3 ][ 3 ] );
#else
	return MMat4x4( matrix[ 0 ][ 0 ], matrix[ 0 ][ 1 ], matrix[ 0 ][ 2 ], matrix[ 0 ][ 3 ],
					matrix[ 1 ][ 0 ], matrix[ 1 ][ 1 ], matrix[ 1 ][ 2 ], matrix[ 1 ][ 3 ],
					matrix[ 2 ][ 0 ], matrix[ 2 ][ 1 ], matrix[ 2 ][ 2 ], matrix[ 2 ][ 3 ],
					matrix[ 3 ][ 0 ], matrix[ 3 ][ 1 ], matrix[ 3 ][ 2 ], matrix[ 3 ][ 3 ] );
#endif
}

//----------------------------------------------------------
MVec3
MakeVec3( const FMVector3& vec )
{
	return MVec3( vec.x, vec.y, vec.z );
}

//----------------------------------------------------------
unsigned int
MakeFixed22_10( float value )
{
	unsigned int fixed = ( ( unsigned int )Floor( value ) ) << 10;
	fixed |= ( unsigned int )( Fract( value ) * 1024.0f );
	return fixed;
}

//----------------------------------------------------------
bool JointWeightPairCmp( const FCDJointWeightPair& left, const FCDJointWeightPair& right )
{
	return left.weight > right.weight;
}


//**********************************************************
// class EdImportDAEScene
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportDAEScene::EdImportDAEScene()
{

}

//----------------------------------------------------------
EdImportDAEScene::~EdImportDAEScene()
{
	// delete all of the lights.
	LightVec::iterator iter = _lights.begin();
	const LightVec::iterator end = _lights.end();
	for ( ; iter != end; ++iter )
		delete ( *iter );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrPolygonNode*
EdImportDAEScene::ImportScene( GrKFAnim** anim, FCDocument* doc, const UPath& outputPath, const EdImportDAEGeometries& geometryImporter,
							  const EdImportDAEMaterials& materialImporter, const tstring& animName, bool overwriteLights )
{
	// get the list of scenes.
	FCDVisualSceneNodeLibrary* sceneLib = doc->GetVisualSceneLibrary();

	// go through all of the visual scenes and import.  Note that we'll combine them
	// under one root node.  Only the first will be visible.
	GrPolygonNode* root = new GrPolygonNode( "__GlobalRoot", MMat4x4() );
	unsigned int sceneCount = ( unsigned int )sceneLib->GetEntityCount();
	for ( unsigned int i = 0; i < sceneCount; ++i )
	{
		// get the current visual scene.
		FCDSceneNode* curSceneNode = sceneLib->GetEntity( i );
		_root = curSceneNode;

		// try to import the current scene.
		GrPolygonNode* sceneRoot = ImportNode( curSceneNode, outputPath, geometryImporter, materialImporter );
		root->AddChild( sceneRoot );
	}

	// write out all lights.
	WriteLights( outputPath, overwriteLights );

	if ( anim )
		*anim = ImportAnimation( animName );

	// return the scene.
	return root;
}

//----------------------------------------------------------
GrPolygonNode*
EdImportDAEScene::ImportRawGeometry( FCDocument* doc, const EdImportDAEGeometries& geometryImporter )
{
	// get the list of scenes.
	FCDVisualSceneNodeLibrary* sceneLib = doc->GetVisualSceneLibrary();

	// go through all of the visual scenes and import.  Note that we'll combine them
	// under one root node.  Only the first will be visible.
	GrPolygonNode* root = new GrPolygonNode( "__GlobalRoot", MMat4x4() );
	unsigned int sceneCount = ( unsigned int )sceneLib->GetEntityCount();
	for ( unsigned int i = 0; i < sceneCount; ++i )
	{
		// get the current visual scene.
		FCDSceneNode* curSceneNode = sceneLib->GetEntity( i );
		_root = curSceneNode;

		// try to import the current scene.
		GrPolygonNode* sceneRoot = ImportRawNode( curSceneNode, geometryImporter );
		root->AddChild( sceneRoot );
	}

	// return the scene.
	return root;
}

//----------------------------------------------------------
unsigned int
EdImportDAEScene::GetLightCount() const
{
	return ( unsigned int )_lights.size();
}

//----------------------------------------------------------
EdImportDAEScene::SLight*
EdImportDAEScene::GetLight( unsigned int idx ) const
{
	B_ASSERT( idx < GetLightCount() );
	return _lights[ idx ];
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
GrKFAnim*
EdImportDAEScene::ImportAnimation( const tstring& animName ) const
{
	// return NULL if nothing is animated.
	//if ( _animatedNodes.size() == 0 )
	//	return 0;

	// decomposed animation keys.
	GrKFAnimData::RotKeyArray rotateKeys;
	GrKFAnimData::ScaleKeyArray scaleKeys;
	GrKFAnimData::TranslateKeyArray translateKeys;

	// keys to fill out.
	GrKFAnimData::SScaleKeyFrame scaleKey;
	GrKFAnimData::SRotKeyFrame rotateKey;
	GrKFAnimData::STranslateKeyFrame translateKey;

	// array holding all of the animation data.
	UFastArray< URef< GrKFAnimData > > animDataArray;

	// get bind matrices.
//		FMatrix44 localXForm = ( *iter )->CalculateLocalTransform();
//		FMatrix44 worldXForm = ( *iter )->CalculateWorldTransform();

	// record the world bind matrix.
	FMMatrix44List invBindMatrices;
	NodeVector::const_iterator iter = _animatedNodes.begin();
	const NodeVector::const_iterator end = _animatedNodes.end();
	for ( ; iter != end; ++iter )
		invBindMatrices.push_back( ( *iter ).invBindMatrix );

	// sample animation data for each node.
	unsigned int bindIdx = 0;
	iter = _animatedNodes.begin();
	for ( ; iter != end; ++iter, ++bindIdx )
	{
		// clear the translated data.
		rotateKeys.Clear();
		scaleKeys.Clear();
		translateKeys.Clear();

		// sample animation data from collada.
		FCDSceneNodeTools::ClearSampledAnimation();
		FCDSceneNodeTools::GenerateSampledAnimation( ( *iter ).node );
		const FloatList& keys = FCDSceneNodeTools::GetSampledAnimationKeys();
		const FMMatrix44List& values = FCDSceneNodeTools::GetSampledAnimationMatrices();

		B_ASSERT( keys.size() == values.size() );

		// skip the current node if it has no animations.
		if ( keys.size() <= 1 )
			continue;

		// get the current bind matrix.
		FMMatrix44 invBindMatrix = invBindMatrices[ bindIdx ];

		// translate collada's animation data.
		for ( unsigned int i = 0; i < ( unsigned int )keys.size(); ++i )
		{
			// get the current time/xform.
			float time = keys[ i ];
			FMMatrix44 xform = values[ i ];

			// transform the xform from world space to offset space.
			xform = invBindMatrix * xform;

			// decompose the current matrix so that we can create our
			// keys.
			float invert = 0.0f;
			FMVector3 scale;
			FMVector3 eulerRot;
			FMVector3 translate;
			xform.Decompose( scale, eulerRot, translate, invert );

			// create a rotation quaternion.
			FMQuaternion rotate( FMQuaternion::EulerRotationQuaternion( eulerRot.x, eulerRot.y, eulerRot.z ) );

			// create our own scale key.
			scaleKey.time = MakeFixed22_10( time );
			scaleKey.scaleXYZ = MakeVec3( scale );
			scaleKey.inTan = scaleKey.scaleXYZ;
			scaleKey.outTan = scaleKey.scaleXYZ;
			scaleKeys.Push( scaleKey );

			// create our own rotate key.
			rotateKey.time = MakeFixed22_10( time );
			rotateKey.rotate = MQuat( rotate.x, rotate.y, rotate.z, rotate.w );
			rotateKeys.Push( rotateKey );

			// create our own translate key.
			translateKey.time = MakeFixed22_10( time );
			translateKey.translate = MakeVec3( translate );
			translateKey.inTan = translateKey.translate;
			translateKey.outTan = translateKey.translate;
			translateKeys.Push( translateKey );
		}

		// create a new KF data sequence.
		GrKFAnimData* targetKFData = new GrKFAnimData( ( *iter ).node->GetName().c_str(), keys.back(), rotateKeys,
			scaleKeys, translateKeys );
		animDataArray.Push( targetKFData );
	}

	// if there isn't any animation, return null.
	if ( animDataArray.GetElemCount() == 0 )
		return 0;

	// create a new animation from the anim data array.
	return new GrKFAnim( animName, animDataArray );
}


//----------------------------------------------------------
GrPolygonNode*
EdImportDAEScene::ImportNode( FCDSceneNode* node, const UPath& outputPath, const EdImportDAEGeometries& geometryImporter,
							 const EdImportDAEMaterials& materialImporter )
{
	// make sure the node passed in is valid!
	B_ASSERT( node != 0 );

	// check to see if the node is animated, and if so, store it in our animated
	// node list.
//	if ( node->IsTarget() )
	SAnimNode animNode = {
		node,
		node->CalculateLocalTransform().Inverted(),
	};
	_animatedNodes.push_back( animNode );

	// get transform information from the node.
	GrPolygonNode* xformNode = new GrPolygonNode( node->GetName().c_str(), MakeMat4x4( node->CalculateLocalTransform() ) );

	// import instances.
	unsigned int instanceCount = ( unsigned int )node->GetInstanceCount();
	for ( unsigned int i = 0; i < instanceCount; ++i )
	{
		// get the current instance.
		FCDEntityInstance* curInstance = node->GetInstance( i );
		FCDEntityInstance::Type type = curInstance->GetType();

		// note that we only care about geometry instances.
		if ( type == FCDEntityInstance::GEOMETRY || type == FCDEntityInstance::CONTROLLER )
		{
			// TODO: Change this to support mesh instances!!!  Currently, this will
			// simply duplicate meshes, which is a waste for dynamic geometry.
			GrPolygonMesh* mesh = ImportMesh( ( FCDGeometryInstance* )curInstance, geometryImporter,
				materialImporter );
			if ( mesh == 0 )
				continue;

			// add the mesh to the current node.
			xformNode->AddMesh( mesh );
		}
		else if ( type == FCDEntityInstance::SIMPLE )
		{
			// get the type of the entity and deal with it accordingly.
			FCDEntity::Type type = curInstance->GetEntityType();
			if ( type == FCDEntity::LIGHT )
				ImportLight( outputPath, curInstance );
		}
		else
		{
			type = type;
		}
	}

	// now recurse children and add to the current polygon node.
	unsigned int childCount = ( unsigned int )node->GetChildrenCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		// get the current child.
		FCDSceneNode* curChild = node->GetChild( i );
		B_ASSERT( curChild );

		// now import the child and add it to the polygon node.
		GrPolygonNode* newNode = ImportNode( curChild, outputPath, geometryImporter, materialImporter );
		if ( newNode )
			xformNode->AddChild( newNode );
	}

	// return the node we've created.
	return xformNode;
}

//----------------------------------------------------------
GrPolygonNode*
EdImportDAEScene::ImportRawNode( FCDSceneNode* node, const EdImportDAEGeometries& geometryImporter )
{
	// make sure the node passed in is valid!
	B_ASSERT( node != 0 );

	// check to see if the node is animated, and if so, store it in our animated
	// node list.
	SAnimNode animNode = {
		node,
		node->CalculateLocalTransform().Inverted(),
	};
	_animatedNodes.push_back( animNode );

	// get transform information from the node.
	GrPolygonNode* xformNode = new GrPolygonNode( node->GetName().c_str(), MakeMat4x4( node->CalculateLocalTransform() ) );

	// import instances.
	unsigned int instanceCount = ( unsigned int )node->GetInstanceCount();
	for ( unsigned int i = 0; i < instanceCount; ++i )
	{
		// get the current instance.
		FCDEntityInstance* curInstance = node->GetInstance( i );
		FCDEntityInstance::Type type = curInstance->GetType();

		// note that we only care about geometry instances.
		if ( type == FCDEntityInstance::GEOMETRY || type == FCDEntityInstance::CONTROLLER )
		{
			// TODO: Change this to support mesh instances!!!  Currently, this will
			// simply duplicate meshes, which is a waste for dynamic geometry.
			GrPolygonMesh* mesh = ImportRawMesh( ( FCDGeometryInstance* )curInstance, geometryImporter );
			if ( mesh == 0 )
				continue;

			// add the mesh to the current node.
			xformNode->AddMesh( mesh );
		}
	}

	// now recurse children and add to the current polygon node.
	unsigned int childCount = ( unsigned int )node->GetChildrenCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		// get the current child.
		FCDSceneNode* curChild = node->GetChild( i );
		B_ASSERT( curChild );

		// now import the child and add it to the polygon node.
		GrPolygonNode* newNode = ImportRawNode( curChild, geometryImporter );
		if ( newNode )
			xformNode->AddChild( newNode );
	}

	// return the node we've created.
	return xformNode;
}

//----------------------------------------------------------
GrPolygonMesh*
EdImportDAEScene::ImportMesh( FCDGeometryInstance* instance, const EdImportDAEGeometries& geometryImporter,
							 const EdImportDAEMaterials& materialImporter )
{
	typedef std::list< std::vector< unsigned int > > PolygonIndexList;

	// make sure we're dealing with a geometry object.
	B_ASSERT( instance->GetType() == FCDEntityInstance::GEOMETRY || instance->GetType() == FCDEntityInstance::CONTROLLER );

	// get the entity we're instancing and make sure it's a geometry object.
	FCDEntity* entity = instance->GetEntity();
	FCDSkinController* skin = 0;
	if ( entity->GetType() == FCDEntity::CONTROLLER )
	{
		// get the geometric target and the skin data.
		FCDController* controller = ( FCDController* )entity;
		entity = controller->GetBaseGeometry();
		skin = controller->GetSkinController();

		FMMatrix44 bindXForm = skin->GetBindShapeTransform();
		bindXForm = bindXForm;
	}

	// make sure we have a geometry.
	B_ASSERT( entity != 0 && entity->GetType() == FCDEntity::GEOMETRY );

	// get the DAE id and get the data we've extracted from the geometries earlier.
	const tstring& id = entity->GetDaeId().c_str();
	EdImportDAEGeometries::SGeometry* geometry = geometryImporter.FindGeometry( id );

	// if we couldn't find the geometry, it means it wasn't a mesh.
	if ( geometry == 0 )
		return 0;

	// create a new geometry group to hold all of the polygons in each range.
	std::vector< SPolygonSkinVertex > skinInfo;
	GrPolygonGroup newGroup;
	unsigned int rangeCount = ( unsigned int )geometry->ranges.size();
	for ( unsigned int i = 0; i < rangeCount; ++i )
	{
		// get the current range.
		EdImportDAEGeometries::SGeometryRange* curRange = geometry->ranges[ i ];
		PolygonIndexList& curIndices = curRange->positionIndices;

		// import skinning information if necessary.
		if ( skin != 0 )
		{
			// make sure there aren't too many joints
			B_ASSERT( skin->GetJointCount() <= 255 );

			const FMMatrix44& bindMatrix = skin->GetBindShapeTransform();

			// reduce the number of influences on each vertex to the maximum
			// supported.
			skin->ReduceInfluences( 3 );

			// go through all polygons in the group.
			PolygonIndexList::iterator iter = curIndices.begin();
			GrPolygonGroup& curPolygonGroup = curRange->polygons;
			unsigned int polyCount = curPolygonGroup.GetPolygonCount();
			B_ASSERT( curIndices.size() == polyCount );
			for ( unsigned int j = 0; j < polyCount; ++j, ++iter )
			{
				// get the current polygon.
				const GrPolygon& curPoly = curPolygonGroup.GetPolygon( j );

				// add skinning info for the current polygon.
				unsigned int boneIds = 0;
				unsigned int vertexCount = curPoly.GetVertexCount();
				B_ASSERT( vertexCount == iter->size() );
				B_ASSERT( vertexCount < 16 );
				for ( unsigned int k = 0; k < vertexCount; ++k )
				{
					// add the new vertex.
					skinInfo.resize( skinInfo.size() + 1 );
					SPolygonSkinVertex& curSkinVertex = skinInfo.back();

					// record the skin info for each vertex.
					curSkinVertex.boneIds = 0;
					FCDSkinControllerVertex* vertexSkinInfo = skin->GetVertexInfluence( ( *iter )[ k ] );

					// read in the skin info.
					size_t influenceCount = vertexSkinInfo->GetPairCount();
					for ( size_t w = 0; w < influenceCount; ++w )
					{
						// get the current joint-weighting info.
						FCDJointWeightPair* cur = vertexSkinInfo->GetPair( w );

						// make sure the joint is valid.
						B_ASSERT( cur->jointIndex <= 255 );

						// get the weight/bone ID.
						curSkinVertex.weights[ ( unsigned int )w ] = cur->weight;
						curSkinVertex.boneIds |= ( cur->jointIndex << ( w*8 ) );
					}

					// determine how much weighting was dropped by losing any
					// extra influences.
					float error = 0.0f;
					for ( size_t w = influenceCount; w < 3; ++w )
						curSkinVertex.weights[ ( unsigned int )w ] = 0.0f;
				}
			}

			// build the name array of joints and bind matrices.
			unsigned int jointCount = ( unsigned int )skin->GetJointCount();

//			const StringList& skinJointNames = skin->GetJointIds();
//			const FMMatrix44List& skinBindPoses = skin->GetBindPoses();

			tstring* jointNames = new tstring[ jointCount ];
			FMMatrix44 bindShapeXForm = skin->GetBindShapeTransform();
			MMat4x4* invBindMatrices = new MMat4x4[ jointCount ];
			for ( unsigned int j = 0; j < jointCount; ++j )
			{
				// record joint information.
				FCDSkinControllerJoint* curJoint = skin->GetJoint( j );
				FCDEntity* entity = _root->FindDaeId( curJoint->GetId() );

				// make sure an entity was found so that we can have a proper name!
				B_ASSERT( entity != 0 );
				if ( entity )
					jointNames[ j ] = entity->GetName().c_str();
				else
					jointNames[ j ] = curJoint->GetId().c_str();

				// store the inverse bind matrix.
				invBindMatrices[ j ] = MakeMat4x4( curJoint->GetBindPoseInverse() * bindShapeXForm );
			}

			// set the skin information.
			curPolygonGroup.SetSkinningInfo( &skinInfo.front(), jointNames, invBindMatrices, jointCount );
		}

		// get the material instance that binds to the current semantic.
		FCDMaterialInstance* materialInstance = instance->FindMaterialInstance( curRange->materialSemantic.c_str() );
		if ( materialInstance == 0 )
		{
			// if the semantic doesn't have a material bound to it, we need to complain.
			EdPrintF( "Warning: an invalid material semantic was found on object %s in one of it's ranges; range skipped.\n",
				id.c_str() );
			continue;
		}

		// get the material we resolve to.
		FCDMaterial* material = materialInstance->GetMaterial();
		B_ASSERT( material );

		// use the old material to find out what engine material we imported to.
		tstring name;
		URef< GrMaterial > engineMaterial;
		if ( !materialImporter.GetImportedMaterialName( name, material ) )
		{
			// if we couldn't find the material, complain.
			EdPrintF( "Warning: a material instance specifies an invalid material on object %s in one of it's ranges; the default material was assigned instead.\n",
				id.c_str() );
			engineMaterial = gGrMaterialMgr->GetDefaultMaterial();
		}
		else
		{
			// get the actual material.
			tstring error;
			engineMaterial = gGrMaterialMgr->GetMaterial( name, error );
			if ( engineMaterial == 0 )
			{
				// if we couldn't find the material, complain.
				EdPrintF( "Warning: an error occured loading material %s.\nError:\n%s\n", name.c_str(), error.c_str() );
				continue;
			}
		}

		// now copy over all geometry in the current range, setting each polygon's
		// material.
		unsigned int rangePolygonCount = curRange->polygons.GetPolygonCount();
		unsigned int basePolygonIdx = newGroup.GetPolygonCount();

		// add the range polygons to our group and then set materials.
		newGroup.AddGroup( curRange->polygons );
		for ( unsigned int j = 0; j < rangePolygonCount; ++j, ++basePolygonIdx )
			newGroup.SetPolygonMaterial( basePolygonIdx, engineMaterial );
	}

	// determine a name for the instance.
	FCDSceneNode* parentNode = instance->GetParent();
	B_ASSERT( parentNode != 0 );

	// create the new polygon mesh and return it.
	GrPolygonMesh* newMesh = new GrPolygonMesh( parentNode->GetDaeId().c_str(), newGroup );
	return newMesh;
}

//----------------------------------------------------------
GrPolygonMesh*
EdImportDAEScene::ImportRawMesh( FCDGeometryInstance* instance, const EdImportDAEGeometries& geometryImporter )
{
	typedef std::list< std::vector< unsigned int > > PolygonIndexList;

	// make sure we're dealing with a geometry object.
	B_ASSERT( instance->GetType() == FCDEntityInstance::GEOMETRY || instance->GetType() == FCDEntityInstance::CONTROLLER );

	// get the entity we're instancing and make sure it's a geometry object.
	FCDEntity* entity = instance->GetEntity();
	FCDSkinController* skin = 0;
	if ( entity->GetType() == FCDEntity::CONTROLLER )
	{
		// get the geometric target and the skin data.
		FCDController* controller = ( FCDController* )entity;
		entity = controller->GetBaseGeometry();
		skin = controller->GetSkinController();

		FMMatrix44 bindXForm = skin->GetBindShapeTransform();
		bindXForm = bindXForm;
	}

	// make sure we have a geometry.
	B_ASSERT( entity != 0 && entity->GetType() == FCDEntity::GEOMETRY );

	// get the DAE id and get the data we've extracted from the geometries earlier.
	const tstring& id = entity->GetDaeId().c_str();
	EdImportDAEGeometries::SGeometry* geometry = geometryImporter.FindGeometry( id );

	// if we couldn't find the geometry, it means it wasn't a mesh.
	if ( geometry == 0 )
		return 0;

	// create a new geometry group to hold all of the polygons in each range.
	std::vector< SPolygonSkinVertex > skinInfo;
	GrPolygonGroup newGroup;
	unsigned int rangeCount = ( unsigned int )geometry->ranges.size();
	for ( unsigned int i = 0; i < rangeCount; ++i )
	{
		// get the current range.
		EdImportDAEGeometries::SGeometryRange* curRange = geometry->ranges[ i ];
		PolygonIndexList& curIndices = curRange->positionIndices;

		// import skinning information if necessary.
		if ( skin != 0 )
		{
			// make sure there aren't too many joints
			B_ASSERT( skin->GetJointCount() <= 255 );

			const FMMatrix44& bindMatrix = skin->GetBindShapeTransform();

			// reduce the number of influences on each vertex to the maximum
			// supported.
			skin->ReduceInfluences( 3 );

			// go through all polygons in the group.
			PolygonIndexList::iterator iter = curIndices.begin();
			GrPolygonGroup& curPolygonGroup = curRange->polygons;
			unsigned int polyCount = curPolygonGroup.GetPolygonCount();
			B_ASSERT( curIndices.size() == polyCount );
			for ( unsigned int j = 0; j < polyCount; ++j, ++iter )
			{
				// get the current polygon.
				const GrPolygon& curPoly = curPolygonGroup.GetPolygon( j );

				// add skinning info for the current polygon.
				unsigned int boneIds = 0;
				unsigned int vertexCount = curPoly.GetVertexCount();
				B_ASSERT( vertexCount == iter->size() );
				B_ASSERT( vertexCount < 16 );
				for ( unsigned int k = 0; k < vertexCount; ++k )
				{
					// add the new vertex.
					skinInfo.resize( skinInfo.size() + 1 );
					SPolygonSkinVertex& curSkinVertex = skinInfo.back();

					// record the skin info for each vertex.
					curSkinVertex.boneIds = 0;
					FCDSkinControllerVertex* vertexSkinInfo = skin->GetVertexInfluence( ( *iter )[ k ] );

					// read in the skin info.
					size_t influenceCount = vertexSkinInfo->GetPairCount();
					for ( size_t w = 0; w < influenceCount; ++w )
					{
						// get the current joint-weighting info.
						FCDJointWeightPair* cur = vertexSkinInfo->GetPair( w );

						// make sure the joint is valid.
						B_ASSERT( cur->jointIndex <= 255 );

						// get the weight/bone ID.
						curSkinVertex.weights[ ( unsigned int )w ] = cur->weight;
						curSkinVertex.boneIds |= ( cur->jointIndex << ( w*8 ) );
					}

					// determine how much weighting was dropped by losing any
					// extra influences.
					float error = 0.0f;
					for ( size_t w = influenceCount; w < 3; ++w )
						curSkinVertex.weights[ ( unsigned int )w ] = 0.0f;
				}
			}

			// build the name array of joints and bind matrices.
			unsigned int jointCount = ( unsigned int )skin->GetJointCount();

//			const StringList& skinJointNames = skin->GetJointIds();
//			const FMMatrix44List& skinBindPoses = skin->GetBindPoses();

			tstring* jointNames = new tstring[ jointCount ];
			FMMatrix44 bindShapeXForm = skin->GetBindShapeTransform();
			MMat4x4* invBindMatrices = new MMat4x4[ jointCount ];
			for ( unsigned int j = 0; j < jointCount; ++j )
			{
				// record joint information.
				FCDSkinControllerJoint* curJoint = skin->GetJoint( j );
				FCDEntity* entity = _root->FindDaeId( curJoint->GetId() );

				// make sure an entity was found so that we can have a proper name!
				B_ASSERT( entity != 0 );
				if ( entity )
					jointNames[ j ] = entity->GetName().c_str();
				else
					jointNames[ j ] = curJoint->GetId().c_str();

				// store the inverse bind matrix.
				invBindMatrices[ j ] = MakeMat4x4( curJoint->GetBindPoseInverse() * bindShapeXForm );
			}

			// set the skin information.
			curPolygonGroup.SetSkinningInfo( &skinInfo.front(), jointNames, invBindMatrices, jointCount );
		}

		// now copy over all geometry in the current range, setting each polygon's
		// material.
		unsigned int rangePolygonCount = curRange->polygons.GetPolygonCount();
		unsigned int basePolygonIdx = newGroup.GetPolygonCount();

		// add the range polygons to our group and then set materials.
		newGroup.AddGroup( curRange->polygons );
	}

	// determine a name for the instance.
	FCDSceneNode* parentNode = instance->GetParent();
	B_ASSERT( parentNode != 0 );

	// create the new polygon mesh and return it.
	GrPolygonMesh* newMesh = new GrPolygonMesh( parentNode->GetDaeId().c_str(), newGroup );
	return newMesh;
}

//----------------------------------------------------------
void
EdImportDAEScene::ImportLight( const UPath& outputPath, FCDEntityInstance* instance )
{
	// keep track of the parent.
	FCDSceneNode* parent = instance->GetParent();

	// get a new light and fill it out.
	_lights.push_back( new SLight );
	SLight* newLight = _lights.back();

	// store the name of the parent.
	newLight->parentName = parent->GetName();
	newLight->newName = ( outputPath + newLight->parentName ).GetPathString();
	newLight->lightInst = instance;
}

//----------------------------------------------------------
void
EdImportDAEScene::WriteLights( const UPath& outputPath, bool overwriteExisting )
{
	tstring lightFileText;
	UPath lightFilePath = outputPath + GR_LIGHTS_FILENAME;

	// read the text in the existing material file.
	URef< RFile > file = gRFileMgr->GetFile( lightFilePath, RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );

	// set the overwriteExisting flag to true if the file doesn't exist.
	if ( !file || overwriteExisting )
	{
		lightFileText =
			"//----------------------------------------------------------\n"
			"// File:		"GR_LIGHTS_FILENAME"\n"
			"// Author:		DAE Importer (: Kevin Bray's proxy :)\n"
			"// Copyright © 2005 Bootstrap Studios\n"
			"//----------------------------------------------------------\n\n";
	}
	else
	{
		// read the text in the existing lights file.
		lightFileText = ( char* )file->GetData();

		// free the file's data.
		file->Purge();
	}

	// go through all of our lights and write them out to the target
	// lights file.
	LightVec::iterator iter = _lights.begin();
	const LightVec::iterator end = _lights.end();
	for ( ; iter != end; ++iter )
	{
		// get the current light.
		SLight* curLight = ( *iter );
		B_ASSERT( curLight != 0 );

		// check to see if a light with the current name already exists.
		if ( !overwriteExisting )
		{
			tstring errors;
			if ( gGrLightMgr->GetLight( curLight->newName, errors ) )
				continue;
		}

		// get the light instance data.
		FCDEntityInstance* lightInst = curLight->lightInst;
		B_ASSERT( lightInst != 0 );
		B_ASSERT( lightInst->GetEntityType() == FCDEntity::LIGHT );
		FCDLight* lightData = ( FCDLight* )lightInst->GetEntity();
		B_ASSERT( lightData != 0 );

		// now write out the light's data.
		FCDSceneNode* parentNode = lightInst->GetParent();

		// get the world matrix.
//		FMMatrix44 worldMatrix = parentNode->CalculateWorldTransform();

		// decompose into translation and rotation.
//		FMVector3 scale;
//		FMVector3 rotation;
//		FMVector3 translation;
//		float inverted = 1.0f;
//		worldMatrix.Decompose( scale, rotation, translation, inverted );

		// get the light type.
		FCDLight::LightType lightType = lightData->GetLightType();

		// write information about the light.
		lightFileText += curLight->newName;
		lightFileText += "\n{\n";

		// store the name.
		lightFileText += "\tName\t\t\t";
		lightFileText += curLight->newName;
		lightFileText += "\n";

		// store the position.
		lightFileText += "\tPosition\t\t";
		lightFileText << 0.0f; //translation.x;
		lightFileText += ", ";
		lightFileText << 0.0f; //translation.y;
		lightFileText += ", ";
		lightFileText << 0.0f; //translation.z;
		lightFileText += "\n";

		// store the color.
		FMVector3 color = lightData->GetColor();
		lightFileText += "\tColor\t\t\t";
		lightFileText << color.x;
		lightFileText += ", ";
		lightFileText << color.y;
		lightFileText += ", ";
		lightFileText << color.z;
		lightFileText += ", 1.0f\n";

		// write out the intensity.
		float intensity = lightData->GetIntensity();
		lightFileText += "\tIntensity\t\t";
		lightFileText << 1.0f; //intensity;
		lightFileText += "\n";

		// write out the attenuation.
		float linearAtten = lightData->GetLinearAttenuationFactor();
		float quadraticAtten = lightData->GetQuadraticAttenuationFactor();
		float constAtten = lightData->GetConstantAttenuationFactor();

		// calculate and write out a good range.
		float range = 0.0f;
		if ( linearAtten > 0.01f )
			range = ( 64.0f * intensity ) / linearAtten;
		else if ( quadraticAtten > 0.01f )
			range = ( 16.0f * intensity ) / quadraticAtten;
		else if ( constAtten > 0.01f )
			range = 100000.0f;

		// write out our range.
		lightFileText += "\tRange\t\t\t";
		lightFileText << range;
		lightFileText += "\n";

		// write out our attenuation.
		lightFileText += "\tAtten\t\t\t";
		if ( linearAtten > 0.01f || quadraticAtten > 0.01f )
			lightFileText += "1.000000, 0.000000, 0.000000\n";
		else
			lightFileText += "0.000000, 0.000000, 1.000000\n";

		// write out the width.
		lightFileText += "\tWidth\t\t\t0.02\n";

		// write out the haziness.
		lightFileText += "\tHaziness\t\t0.0\n";

		// determine what kind of light we need to write out.
		if ( lightType == FCDLight::SPOT )
		{
			// select a falloff exponent texture.
			float falloffExponent = lightData->GetFallOffExponent();
			if ( falloffExponent >= 1.7f )
				lightFileText += "\tSpotTexture\t\t_spotQuadratic\n";
			else if ( falloffExponent >= 0.7f )
				lightFileText += "\tSpotTexture\t\t_spotLinear\n";

			// get the total falloff angle and set the projection.
			float falloffAngle = lightData->GetFallOffAngle();
			lightFileText += "\tProjection\n\t{\n";
			lightFileText += "\t\tFOV\t\t\t\t";
			lightFileText << falloffAngle;
			lightFileText += "\n";

			// write out the aspect.  This functionality once existed in FCollada,
			// but was dropped.
			float aspect = 1.0f;
			lightFileText += "\t\tAspect\t\t\t";
			lightFileText << aspect;
			lightFileText += "\n";

			// write out the near plane.
			lightFileText += "\t\tNear\t\t\t1.0\n";

#if 0
			float xRot = RadToDeg( rotation.x );
			float yRot = RadToDeg( rotation.y );
			float zRot = RadToDeg( rotation.z );

			// write out the rotation.
			lightFileText += "\t\tRotation\t\t";
			lightFileText << RadToDeg( xRot );
			lightFileText += ", ";
			lightFileText << RadToDeg( yRot );
			lightFileText += ", ";
			lightFileText << RadToDeg( zRot );
			lightFileText += "\n";
#else
			// write out the rotation.
			lightFileText += "\t\tRotationMatrix\t\t";
			lightFileText << 1.0f; //worldMatrix[ 0 ][ 0 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 1 ][ 0 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 2 ][ 0 ];
			lightFileText += ",";

			lightFileText << 0.0f; //worldMatrix[ 0 ][ 1 ];
			lightFileText += ",";
			lightFileText << 1.0f; //worldMatrix[ 1 ][ 1 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 2 ][ 1 ];
			lightFileText += ",";

			lightFileText << 0.0f; //worldMatrix[ 0 ][ 2 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 1 ][ 2 ];
			lightFileText += ",";
			lightFileText << 1.0f; //worldMatrix[ 2 ][ 2 ];
			lightFileText += "\n";
#endif
			// end the projection block.
			lightFileText += "\t}\n";
		}
		else if ( lightType == FCDLight::DIRECTIONAL )
		{
			lightFileText += "\tParallel\n";

			// begin the projection block.
			lightFileText += "\tProjection\n\t{\n";

			// write out the near plane.
			lightFileText += "\t\tNear\t\t\t1.0\n";

#if 0
			// write out the rotation.
			lightFileText += "\t\tRotation\t\t";
			lightFileText << RadToDeg( rotation.x );
			lightFileText += ", ";
			lightFileText << RadToDeg( rotation.y );
			lightFileText += ", ";
			lightFileText << RadToDeg( rotation.z );
			lightFileText += "\n";
#else
			// write out the rotation.
			lightFileText += "\t\tRotationMatrix\t";
			lightFileText << 1.0f; //worldMatrix[ 0 ][ 0 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 1 ][ 0 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 2 ][ 0 ];
			lightFileText += ",";

			lightFileText << 0.0f; //worldMatrix[ 0 ][ 1 ];
			lightFileText += ",";
			lightFileText << 1.0f; //worldMatrix[ 1 ][ 1 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 2 ][ 1 ];
			lightFileText += ",";

			lightFileText << 0.0f; //worldMatrix[ 0 ][ 2 ];
			lightFileText += ",";
			lightFileText << 0.0f; //worldMatrix[ 1 ][ 2 ];
			lightFileText += ",";
			lightFileText << 1.0f; //worldMatrix[ 2 ][ 2 ];
			lightFileText += "\n";
#endif
			// end the projection block.
			lightFileText += "\t}\n";
		}

		// end the current light.
		lightFileText += "}\n\n";
	}

	// now save out the lights file.
	file->WriteData( 0, lightFileText.c_str(), ( unsigned int )lightFileText.length() );
}
