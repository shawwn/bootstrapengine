//----------------------------------------------------------
// File:		EdSave.cpp
// Author:		Kevin Bray
// Created:		02-19-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdSave.h"

// project includes.
#include "GrLight.h"
#include "GrLightMgr.h"
#include "GrScene.h"
#include "GrBSPTree.h"
#include "GrModelMgr.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrMeshInst.h"
#include "GrMaterial.h"
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "MVec3.h"
#include "GrMaterialMgr.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrDeformer.h"
#include "EdScriptModifier.h"
#include "EdExprCompiler.h"
#include "EdBrushMgr.h"
#include "EdBrush.h"
#include "EdBrushImage.h"
#include "EdBrushPaletteMgr.h"
#include "EdBrushPalette.h"
#include "GrTextureStage.h"
#include "GrParse.h"
#include "UWriter.h"
#include "RFileMgr.h"

// approximately equivalent floats.
static bool FloatApproxEqual( float lhs, float rhs )
{
	// are the two items passed in approximately equal?
	float dif = Abs( lhs - rhs );
	return dif < 0.001f;
}

//**********************************************************
// class EdSave
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdSave::EdSave()
{

}

//----------------------------------------------------------
EdSave::~EdSave()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdSave::SaveSceneData()
{
	// get the current scene definition file.
	UPath scriptPath = gGrScene->GetSceneDefPath();
	tstring sceneName = gGrScene->GetName();

	// select the scene in the script file.
	EdScriptModifier scriptModifier( scriptPath.GetPathString() );
	if ( !scriptModifier.SelectObj( sceneName ) )
		scriptModifier.AddObj( sceneName );

	// now write out the object's params.
	tstring objData;

	// write out the BSP tree (if there is one).
	GrBSPTree* bspTree = gGrScene->GetBSPTree();
	if ( bspTree )
	{
		objData += "\tBSPPath\t\t\t\t";
		objData += bspTree->GetFileName().GetPathString();
		objData += "\n";
	}

	// write out the global ambience.
	const GrColor& globalAmbience = gGrScene->GetGlobalAmbience();
	objData += "\tGlobalAmbience\t\t";
	objData << globalAmbience.GetR();
	objData += ", ";
	objData << globalAmbience.GetG();
	objData += ", ";
	objData << globalAmbience.GetB();
	objData += ", 1.0f\n";

	// write out the up axis.
	objData += "\tUpAxis\t\t\t\t";
	objData += gGrScene->GetZUp() ? "z\n" : "y\n";

	// write out any models.
	SaveModelChildren( objData, gGrScene->GetRootModel(), false );
	SaveModelChildren( objData, gGrScene->GetSkyboxModel(), false );

	// set the new object data.
	scriptModifier.Replace( objData );

	// save the data.
	scriptModifier.Save();

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool
EdSave::SaveLightData()
{
	// go through all lights and group them by script file.
	std::multimap< tstring, GrLight* > scriptLightMap;
	unsigned int lightCount = gGrLightMgr->GetLightCount();
	for ( unsigned int i = 0; i < lightCount; ++i )
	{
		// get the current light.
		GrLight* light = gGrLightMgr->GetLightByIdx( i );

		// get the script name and insert the light into the map.
		tstring scriptName;
		gGrLightMgr->GetScriptName( scriptName, light->GetId().GetPathString() );
		scriptLightMap.insert( std::pair< tstring, GrLight* >( scriptName, light ) );
	}

	// save all of the lights.
	EdScriptModifier* scriptModifier = 0;
	std::multimap< tstring, GrLight* >::iterator iter = scriptLightMap.begin();
	const std::multimap< tstring, GrLight* >::iterator end = scriptLightMap.end();
	for ( ; iter != end; ++iter )
	{
		// do we need to create a new script modifier?
		if ( !scriptModifier || iter->first != scriptModifier->GetFileName() )
		{
			// save the data in the current script modifier.
			if ( scriptModifier )
				scriptModifier->Save();

			delete scriptModifier;
			scriptModifier = new EdScriptModifier( iter->first );
		}

		// get the current light.
		GrLight* curLight = iter->second;

		// if the light is animated, skip it.
//		if ( curLight->GetIntensity()->IsDynamic() || curLight->GetRange()->IsDynamic() )
//			continue;

		// select or add the object.
		if ( !scriptModifier->SelectObj( curLight->GetId().GetPathString() ) )
		{
			// add the object.  Note that this automatically selects the
			// new object.
			scriptModifier->AddObj( curLight->GetId().GetPathString() );
		}

		// now write out the object's params.
		tstring objData;
		objData += "\tName\t\t\t";
		objData += curLight->GetName();
		objData += "\n";

		// write out the translation.
		MVec3 translate;
		curLight->GetLocalTransform().GetTranslate( translate );
		objData += "\tPosition\t\t";
		objData << translate.GetX();
		objData += ", ";
		objData << translate.GetY();
		objData += ", ";
		objData << translate.GetZ();
		objData += "\n";

		// store the color.
		const GrColor& curColor = curLight->GetColor();
		objData += "\tColor\t\t\t";
		objData << curColor.GetR();
		objData += ", ";
		objData << curColor.GetG();
		objData += ", ";
		objData << curColor.GetB();
		objData += ", ";
		objData << curColor.GetA();
		objData += "\n";

		// write out the intensity.
		objData += "\tIntensity\t\t";
		WriteExpr( objData, curLight->GetIntensity() );
		objData += "\n";

		// write out our range.
		objData += "\tRange\t\t\t";
		WriteExpr( objData, curLight->GetRange() );
		objData += "\n";

		// write out our attenuation.
		objData += "\tAtten\t\t\t";
		objData << curLight->GetSqrAtten();
		objData += ", ";
		objData << curLight->GetLinAtten();
		objData += ", ";
		objData << curLight->GetConstAtten();
		objData += "\n";

		// write out the width.
		objData += "\tWidth\t\t\t";
		objData << curLight->GetWidth();
		objData += "\n";

		// write out the haziness.
		objData += "\tHaziness\t\t";
		objData << curLight->GetHaziness();
		objData += "\n";

		// write out the spot texture (if there is one).
		URef< GrTexture > projTex = curLight->GetProjTexture();
		if ( projTex )
		{
			objData += "\tSpotTexture\t\t";
			objData += projTex->GetName().GetPathString();
			objData += "\n";
		}

		// write out the parallel flag if the light rays are parallel.
		if ( curLight->IsParallel() )
			objData += "\tParallel\n";

		// write out the global flag if the light is global.
		if ( curLight->GetGlobal() )
			objData += "\tGlobal\n";

		// write out the no cast shadow flag if the light shouldn't cast a shadow.
		if ( !curLight->CastShadow() )
			objData += "\tNoCastShadow\n";

		// write out the project info.
		const GrProjection& curProj = curLight->GetProjection();
		objData += "\tProjection\n\t{\n";

		// write the FOV.
		objData += "\t\tFOV\t\t\t\t";
		objData << RadToDeg( curProj.GetFOV() );
		objData += "\n";

		// write the aspect ratio.
		objData += "\t\tAspect\t\t\t";
		objData << curProj.GetAspect();
		objData += "\n";

		// write out the near plane.
		objData += "\t\tNear\t\t\t";
		objData << -curProj.GetNearClip().GetD();
		objData += "\n";

		// write out the width and height.
		objData += "\t\tWidth\t\t\t";
		objData << curProj.GetWidth();
		objData += "\n";
		objData += "\t\tHeight\t\t\t";
		objData << curProj.GetHeight();
		objData += "\n";

		// convert the rotation to Euler angles.
		float y = 0.0f;
		float x = 0.0f;
		float z = 0.0f;
		MMat3x3 rotate;
		curLight->GetLocalTransform().GetRotate( rotate );
		rotate.ToEulerYXZ( y, x, z );

		// write out the rotation.
		objData += "\t\tRotation\t\t";
		objData << RadToDeg( y );
		objData += ", ";
		objData << RadToDeg( x );
		objData += ", ";
		objData << RadToDeg( z );
		objData += "\n";

		// end of the projection block.
		objData += "\t}\n";

		// now replace the current light.
		scriptModifier->Replace( objData );
	}

	// save the data in the current script modifier.
	if ( scriptModifier )
		scriptModifier->Save();

	// free the current script modifier.
	delete scriptModifier;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool
EdSave::SaveMaterialData()
{
	// go through all materials and group them by script file.
	std::multimap< tstring, GrMaterial* > scriptMaterialMap;
	unsigned int materialCount = gGrMaterialMgr->GetMaterialCount();
	for ( unsigned int i = 0; i < materialCount; ++i )
	{
		// get the current light.
		GrMaterial* material = gGrMaterialMgr->GetMaterialByIdx( i );

		// get the script name and insert the light into the map.
		tstring scriptName;
		gGrMaterialMgr->GetScriptName( scriptName, material->GetName() );
		scriptMaterialMap.insert( std::pair< tstring, GrMaterial* >( scriptName, material ) );
	}

	// now save all of the materials.
	EdScriptModifier* scriptModifier = 0;
	std::multimap< tstring, GrMaterial* >::iterator iter = scriptMaterialMap.begin();
	const std::multimap< tstring, GrMaterial* >::iterator end = scriptMaterialMap.end();
	for ( ; iter != end; ++iter )
	{
		// do we need to create a new script modifier?
		if ( !scriptModifier || iter->first != scriptModifier->GetFileName() )
		{
			// save the data in the current script modifier.
			if ( scriptModifier )
				scriptModifier->Save();

			delete scriptModifier;
			scriptModifier = new EdScriptModifier( iter->first );
		}

		// get the current material.
		GrMaterial* curMaterial = iter->second;

		// select or add the object.
		if ( !scriptModifier->SelectObj( curMaterial->GetName() ) )
		{
			// add the object.  Note that this automatically selects the
			// new object.
			scriptModifier->AddObj( curMaterial->GetName() );
		}

		// now write out the object's params.
		tstring objData;

		// cast shadow?
		if ( !curMaterial->CastShadow() )
			objData += "\tNoCastShadow\n";

		// recieve shadow?
		if ( !curMaterial->ReceiveShadow() )
			objData += "\tNoReceiveShadow\n";

		// stipple?
		if ( curMaterial->GetStipple() )
			objData += "\tStipple\n";

		// sort?
//		if ( curMaterial->GetSortType() == GrMaterial::EST_OPAQUE )
//			objData += "\tSort\t\t\tNone
		if ( curMaterial->GetSortType() == GrMaterial::EST_DECAL )
			objData += "\tSort\t\t\t\t\tDecal\n";
		else if ( curMaterial->GetSortType() == GrMaterial::EST_ALPHA )
			objData += "\tSort\t\t\t\t\tAlpha\n";

		// any deformers?
		const GrMaterial::DeformerArray& deformers = curMaterial->GetDeformers();
		for ( unsigned int i = 0; i < deformers.GetElemCount(); ++i )
		{
			objData += "\tDeform\t\t\t\t\t";
			objData += deformers[ i ]->GetName();
			objData += "\n";
		}

		// write out passes.
		unsigned int passCount = curMaterial->GetPassCount();
		for ( unsigned int i = 0; i < passCount; ++i )
			SaveMaterialPass( objData, curMaterial->GetPass( i ), false );

		// write out post process passes.
		unsigned int postProcessPassCount = curMaterial->GetPostProcessPassCount();
		for ( unsigned int i = 0; i < postProcessPassCount; ++i )
			SaveMaterialPass( objData, curMaterial->GetPostProcessPass( i ), true );

		// now replace the current material.
		scriptModifier->Replace( objData );
	}

	// save the data in the current script modifier.
	if ( scriptModifier )
		scriptModifier->Save();

	// free the current script modifier.
	delete scriptModifier;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool
EdSave::SaveBrushData()
{
	// go through all brushes and group them by script file.
	std::multimap< tstring, EdBrush* > scriptBrushMap;
	unsigned int brushCount = gEdBrushMgr->GetBrushCount();
	for ( unsigned int i = 0; i < brushCount; ++i )
	{
		// get the current brush.
		EdBrush* brush = gEdBrushMgr->GetBrushByIdx( i );

		// get the script name and insert the brush into the map.
		tstring scriptName;
		gEdBrushMgr->GetScriptName( scriptName, brush->GetName() );
		scriptBrushMap.insert( std::pair< tstring, EdBrush* >( scriptName, brush ) );
	}

	// now save all of the brushes.
	EdScriptModifier* scriptModifier = 0;
	std::multimap< tstring, EdBrush* >::iterator iter = scriptBrushMap.begin();
	const std::multimap< tstring, EdBrush* >::iterator end = scriptBrushMap.end();
	for ( ; iter != end; ++iter )
	{
		// do we need to create a new script modifier?
		if ( !scriptModifier || iter->first != scriptModifier->GetFileName() )
		{
			// save the data in the current script modifier.
			if ( scriptModifier )
				scriptModifier->Save();

			delete scriptModifier;
			scriptModifier = new EdScriptModifier( iter->first );
		}

		// get the current material.
		EdBrush* curBrush = iter->second;

		// select or add the object.
		if ( !scriptModifier->SelectObj( curBrush->GetName() ) )
		{
			// add the object.  Note that this automatically selects the
			// new object.
			scriptModifier->AddObj( curBrush->GetName() );
		}

		// now write out the object's params.
		tstring objData;

		// save each stage.
		const char* stageLUT[] = {
			"\tBump\t\t\t",
			"\tNormal\t\t\t",
			"\tDiffuse\t\t\t",
			"\tSpecular\t\t",
			"\tEmissive\t\t",
			"\tAmbient\t\t\t",
			"\tUser\t\t\t",
		};
		for ( unsigned int i = 0; i <= ES_COUNT; ++i )
		{
			URef< EdBrushImage > image = curBrush->GetStageSource( ( ESTAGE )i );
			if ( image != 0 )
			{
				objData += stageLUT[ i ];
				objData += "\"" + image->GetFilePath().GetPathString() + "\"";
				objData += "\n";
			}
		}

		// save the mask.
		URef< EdBrushImage > mask = curBrush->GetMask();
		if ( mask )
		{
			objData += "\tMask\t\t\t";
			objData += "\"" + mask->GetFilePath().GetPathString() + "\"";
			objData += "\n";
		}

		// now replace the current brush.
		scriptModifier->Replace( objData );
	}

	// save the data in the current script modifier.
	if ( scriptModifier )
		scriptModifier->Save();

	// free the current script modifier.
	delete scriptModifier;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool
EdSave::SaveBrushPaletteData()
{
	// go through all brushes and group them by script file.
	std::multimap< tstring, EdBrushPalette* > scriptBrushPaletteMap;
	unsigned int brushPaletteCount = gEdBrushPaletteMgr->GetBrushPaletteCount();
	for ( unsigned int i = 0; i < brushPaletteCount; ++i )
	{
		// get the current brush.
		EdBrushPalette* brushPalette = gEdBrushPaletteMgr->GetBrushPaletteByIdx( i );

		// get the script name and insert the brush palette into the map.
		tstring scriptName;
		gEdBrushPaletteMgr->GetScriptName( scriptName, brushPalette->GetName() );
		scriptBrushPaletteMap.insert( std::pair< tstring, EdBrushPalette* >( scriptName, brushPalette ) );
	}

	// now save all of the brush palettes.
	EdScriptModifier* scriptModifier = 0;
	std::multimap< tstring, EdBrushPalette* >::iterator iter = scriptBrushPaletteMap.begin();
	const std::multimap< tstring, EdBrushPalette* >::iterator end = scriptBrushPaletteMap.end();
	for ( ; iter != end; ++iter )
	{
		// do we need to create a new script modifier?
		if ( !scriptModifier || iter->first != scriptModifier->GetFileName() )
		{
			// save the data in the current script modifier.
			if ( scriptModifier )
				scriptModifier->Save();

			delete scriptModifier;
			scriptModifier = new EdScriptModifier( iter->first );
		}

		// get the current brush palette.
		EdBrushPalette* curBrushPalette = iter->second;

		// select or add the object.
		if ( !scriptModifier->SelectObj( curBrushPalette->GetName() ) )
		{
			// add the object.  Note that this automatically selects the
			// new object.
			scriptModifier->AddObj( curBrushPalette->GetName() );
		}

		// save the brushes.
		tstring objData;
		unsigned int brushPaletteCount = curBrushPalette->GetBrushCount();
		for ( unsigned int i = 0; i < brushPaletteCount; ++i )
		{
			URef< EdBrush > brush = curBrushPalette->GetBrush( i );
			objData += "\t";
			objData += "\"" + brush->GetName() + "\"";
			objData += "\n";
		}

		// now replace the current brush.
		scriptModifier->Replace( objData );
	}

	// save the data in the current script modifier.
	if ( scriptModifier )
		scriptModifier->Save();

	// free the current script modifier.
	delete scriptModifier;

	// return true to indicate success.
	return true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdSave::SaveMaterialPass( tstring& data, GrMaterialPass* pass, bool postProcess )
{
	EdExprCompiler compiler;

	// is this post process?  If so, add the post process keyword.
	if ( postProcess )
		data += "\tPostProcess\n";

	// add the opening brace.
	data += "\t{\n";

	// write out the shader.
	GrShader* shader = pass->GetShader();
	if ( shader->GetName() != UPath( GR_SHADER_DEFAULT_LIGHT ) )
	{
		data += "\t\tShader\t\t\t\t\t";
		data += shader->GetName().GetPathString();
		data += "\n";
	}

	// write out the ambient shader.
	GrShader* ambShader = pass->GetAmbientShader();
	B_ASSERT( ambShader != 0 );
	if ( ambShader->GetName() != UPath( GR_SHADER_DEFAULT_AMBIENT ) )
	{
		data += "\t\tAmbientShader\t\t\t";
		data += ambShader->GetName().GetPathString();
		data += "\n";
	}

	// write out stages.
	if ( !pass->ReceiveShadow() )
		data += "\t\tNoReceiveShadow\n";
	if ( !pass->CastShadow() )
		data += "\t\tNoCastShadow\n";
	if ( pass->AlphaTest() )
		data += "\t\tAlphaTest\n";
	if ( !( pass->GetStates() & GR_DEPTHTEST ) )
		data += "\t\tNoDepthTest\n";
	if ( !( pass->GetWriteMask() & GR_DEPTH ) )
		data += "\t\tNoDepthWrite\n";
	if ( !( pass->GetStates() & GR_CULLFACE ) )
		data += "\t\tNoCull\n";
	if ( pass->GetStates() & GR_DEPTHBIAS )
		data += "\t\tDepthBias\n";

	// save the ambient blend mode.
	const SMaterialPassBlendStates& ambientBlendStates = pass->GetAmbientBlendStates();
	if ( ambientBlendStates.srcColorBlendFactor != EBM_ONE )
	{
		data += "\t\tAmbientSrcBlend\t\t\t";
		SaveBlendFactor( data, ambientBlendStates.srcColorBlendFactor );
	}
	if ( ambientBlendStates.dstColorBlendFactor != EBM_ZERO )
	{
		data += "\t\tAmbientDstBlend\t\t\t";
		SaveBlendFactor( data, ambientBlendStates.dstColorBlendFactor );
	}
	if ( ambientBlendStates.srcAlphaBlendFactor != EBM_ONE )
	{
		data += "\t\tAmbientSrcBlendAlpha\t";
		SaveBlendFactor( data, ambientBlendStates.srcColorBlendFactor );
	}
	if ( ambientBlendStates.dstAlphaBlendFactor != EBM_ZERO )
	{
		data += "\t\tAmbientDstBlendAlpha\t";
		SaveBlendFactor( data, ambientBlendStates.dstAlphaBlendFactor );
	}

	// save the light blend mode.
	const SMaterialPassBlendStates& lightBlendStates = pass->GetLightBlendStates();
	if ( lightBlendStates.srcColorBlendFactor != EBM_ONE )
	{
		data += "\t\tLightSrcBlend\t\t\t";
		SaveBlendFactor( data, lightBlendStates.srcColorBlendFactor );
	}
	if ( lightBlendStates.dstColorBlendFactor != EBM_ONE )
	{
		data += "\t\tLightDstBlend\t\t\t";
		SaveBlendFactor( data, lightBlendStates.dstColorBlendFactor );
	}
	if ( lightBlendStates.srcAlphaBlendFactor != EBM_ZERO )
	{
		data += "\t\tLightSrcBlendAlpha\t\t";
		SaveBlendFactor( data, lightBlendStates.srcColorBlendFactor );
	}
	if ( lightBlendStates.dstAlphaBlendFactor != EBM_ONE )
	{
		data += "\t\tLightDstBlendAlpha\t\t";
		SaveBlendFactor( data, lightBlendStates.dstAlphaBlendFactor );
	}

	// save the cull mode.
	if ( pass->GetCullMode() == ECM_FRONT )
		data += "\t\tCull\t\t\t\t\tfront\n";

	// save the alpha test reference value.
	ConditionalWriteExpr( data, pass->GetAlphaTestRef(), 0.5f, "\t\tAlphaTestRef\t\t\t" );

	// get the slope depth bias.
	if ( Abs( pass->GetSlopeDepthBias() ) > 0.0000001f )
	{
		data += "\t\tSlopeDepthBias\t\t\t";
		data << pass->GetSlopeDepthBias();
		data += "\n";
	}

	// get the const depth bias.
	if ( Abs( pass->GetConstDepthBias() ) > 0.0000001f )
	{
		data += "\t\tConstDepthBias\t\t\t";
		data << pass->GetConstDepthBias();
		data += "\n";
	}

	// write out the pass enable.
	ConditionalWriteExpr( data, pass->GetPassEnable(), 1.0f, "\t\tPassEnable\t\t\t" );

	// vertex color mod.
	const GrColor& vertexColorMod = pass->GetVertexColorMod();
	if ( !vertexColorMod.ApproxEqual( GrColor( 0.0f ) ) )
	{
		data += "\t\tVertexColorMod\t\t\t";
		WriteColor( data, vertexColorMod );
	}

	// vertex color add
	const GrColor& vertexColorAdd = pass->GetVertexColorAdd();
	if ( !vertexColorAdd.ApproxEqual( GrColor( 1.0f ) ) )
	{
		data += "\t\tVertexColorAdd\t\t\t";
		WriteColor( data, vertexColorAdd );
	}

	// min light response.
	const GrColor& minLightResponse = pass->GetMinLightResponse();
	if ( !minLightResponse.ApproxEqual( GrColor( 0.0f ) ) )
	{
		data += "\t\tMinLightResponse\t\t";
		WriteColor( data, minLightResponse );
	}

	// max light response.
	const GrColor& maxLightResponse = pass->GetMaxLightResponse();
	if ( !maxLightResponse.ApproxEqual( GrColor( 100.0f ) ) )
	{
		data += "\t\tMaxLightResponse\t\t";
		WriteColor( data, maxLightResponse );
	}

	// write out the uv animation.
	// save the scale S.
	ConditionalWriteExpr( data, pass->GetTextureSet().GetScaleS(), 1.0f, "\t\t\tScaleS\t\t\t\t\t" );

	// save the scale T.
	ConditionalWriteExpr( data, pass->GetTextureSet().GetScaleT(), 1.0f, "\t\t\tScaleT\t\t\t\t\t" );

	// save the translate S.
	ConditionalWriteExpr( data, pass->GetTextureSet().GetTranslateS(), 0.0f, "\t\t\tTranslateS\t\t\t\t" );

	// save the translate T.
	ConditionalWriteExpr( data, pass->GetTextureSet().GetTranslateT(), 0.0f, "\t\t\tTranslateT\t\t\t\t" );

	// save the rotational component.
	ConditionalWriteExpr( data, pass->GetTextureSet().GetRotate(), 0.0f, "\t\t\tRotate\t\t\t\t\t" );

	// save stages.
	SaveMaterialStage( data, "Ambient",		pass->GetStage( ES_AMBIENT ),	GrColor( GR_PARSE_AMBIENT_MOD_DEF ),	GrColor( GR_PARSE_AMBIENT_ADD_DEF ),	GrTextureBase::EF_GUESS,		GR_SYSTEX_WHITE );
	SaveMaterialStage( data, "Emissive",	pass->GetStage( ES_EMISSIVE ),	GrColor( GR_PARSE_EMISSIVE_MOD_DEF ),	GrColor( GR_PARSE_EMISSIVE_ADD_DEF ),	GrTextureBase::EF_GUESS,		GR_SYSTEX_BLACK );
	SaveMaterialStage( data, "Bump",		pass->GetStage( ES_BUMP ),		GrColor( GR_PARSE_BUMP_MOD_DEF ),		GrColor( GR_PARSE_BUMP_ADD_DEF ),		GrTextureBase::EF_I8,			GR_SYSTEX_GRAY );
	SaveMaterialStage( data, "Normal",		pass->GetStage( ES_NORMAL ),	GrColor( 1.0f ),						GrColor( 0.0f ),						GrTextureBase::EF_NORMALMAP,	GR_SYSTEX_NORMAL );
	SaveMaterialStage( data, "Diffuse",		pass->GetStage( ES_DIFFUSE ),	GrColor( GR_PARSE_DIFFUSE_MOD_DEF ),	GrColor( GR_PARSE_DIFFUSE_ADD_DEF ),	GrTextureBase::EF_GUESS,		GR_SYSTEX_WHITE );
	SaveMaterialStage( data, "Specular",	pass->GetStage( ES_SPECULAR ),	GrColor( GR_PARSE_SPECULAR_MOD_DEF ),	GrColor( GR_PARSE_SPECULAR_ADD_DEF ),	GrTextureBase::EF_GUESS,		GR_SYSTEX_BLACK );

	// save the closing brace.
	data += "\t}\n";
}

//----------------------------------------------------------
void
EdSave::SaveMaterialStage( tstring& data, const tstring& stageName, const GrTextureStage& stage,
							 const GrColor& defColorMod, const GrColor& defColorAdd, GrTextureBase::EFORMAT defFormat,
							 const tstring& defTexture )
{
	bool isDefTexture = ( stage.GetTexture()->GetName().GetPathString() == defTexture );

	// do we need to specify full stages or the simplified form?
	if ( !stage.GetTexColorMod().ApproxEqual( defColorMod ) ||
		 !stage.GetTexColorAdd().ApproxEqual( defColorAdd ) ||
		 stage.GetFilterMode() != GrTextureStage::EFM_BILINEAR ||
		 stage.GetAddressModeS() != GrTextureStage::EAM_REPEAT ||
		 stage.GetAddressModeT() != GrTextureStage::EAM_REPEAT )
	{
		// complex form.
		data += "\t\t";
		data += stageName;
		data += "\n\t\t{\n";

		// save the map if necessary.
		if ( !isDefTexture )
		{
			// save the map command.
			data += "\t\t\tMap\t\t\t\t\t\t";

			// write the texture map.
			SaveTextureRef( data, stage, defFormat );
			data += "\n";
		}

		// save the color mod.
		if ( !stage.GetTexColorMod().ApproxEqual( defColorMod ) )
		{
			data += "\t\t\tColorMod\t\t\t\t";
			WriteColor( data, stage.GetTexColorMod() );
		}

		// save the color add.
		if ( !stage.GetTexColorAdd().ApproxEqual( defColorAdd ) )
		{
			data += "\t\t\tColorAdd\t\t\t\t";
			WriteColor( data, stage.GetTexColorAdd() );
		}

		// save the filter info.
		if ( stage.GetFilterMode() != GrTextureStage::EFM_BILINEAR )
		{
			if ( stage.GetFilterMode() == GrTextureStage::EFM_NEAREST )
				data += "\t\t\tNearest\n";
			else if ( stage.GetFilterMode() == GrTextureStage::EFM_TRILINEAR )
				data += "\t\t\tTrilinear\n";
			else if ( stage.GetFilterMode() == GrTextureStage::EFM_ANISOTROPIC )
				data += "\t\t\tAnisotropic\n";
			else
			{
				// this should be impossible!  Was a new filter type added?
				B_ASSERT( false );
			}
		}

		// save the addressing mode S.
		if ( stage.GetAddressModeS() != GrTextureStage::EAM_REPEAT )
		{
			// make sure this is a valid mode!
			B_ASSERT( stage.GetAddressModeS() == GrTextureStage::EAM_CLAMP );
			data += "\t\t\tClampS\n";
		}

		// save the addressing mode T.
		if ( stage.GetAddressModeT() != GrTextureStage::EAM_REPEAT )
		{
			// make sure this is a valid mode!
			B_ASSERT( stage.GetAddressModeT() == GrTextureStage::EAM_CLAMP );
			data += "\t\t\tClampT\n";
		}

		// save the closing brace.
		data += "\t\t}\n";
	}
	else if ( !isDefTexture )
	{
		// simple form.
		data += "\t\t";
		data += stageName;
		data += "Map";

		// get the length of the stage name and add 3 (length of Map).
		size_t len = stageName.length() + 3;

		// determine how many tabs we have already exhausted.
		size_t tabCount = 6 - ( len >> 2 );
		for ( unsigned int i = 0; i < tabCount; ++i )
			data += "\t";

		// write the texture map.
		SaveTextureRef( data, stage, defFormat );
		data += "\n";
	}
}

//----------------------------------------------------------
void
EdSave::ConditionalWriteExpr( tstring& data, URef< UExprTerm > expr, float defaultValue, const tstring& name )
{
	// if no expression exists, simply return.
	if ( !expr )
		return;

	// always write if it's dynamic.
	if ( expr->IsDynamic() )
	{
		data += name;
		WriteExpr( data, expr );
	}
	else if ( !FloatApproxEqual( expr->GetValue(), defaultValue ) )
	{
		// write the expression since it doesn't match the default value.
		data += name;
		WriteExpr( data, expr );
	}
}

//----------------------------------------------------------
void
EdSave::SaveTextureRef( tstring& data, const GrTextureStage& stage, GrTextureBase::EFORMAT defFormat )
{
	// write the gen texture flag if necessary.
	GrTextureStage::GenTextureFlag flag = stage.GetGenTextureFlag();
	if ( flag != 0 )
	{
		// only one flag can be specified!
		B_ASSERT( IsPow2( flag ) );

		// env cube?
		if ( flag == GrTextureStage::kGenEnvCube )
			data += "_envCube";
		else if ( flag == GrTextureStage::kGenEnvDepthCube )
			data += "_envDepthCube";
		else if ( flag == GrTextureStage::kGenCurRender )
			data += "_currentRender";
		else if ( flag == GrTextureStage::kGenCurDepth )
			data += "_currentDepth";
		else if ( flag == GrTextureStage::kGenReflection )
			data += "_reflection";
		else
		{
			// unrecognized GenTextureFlag!
			B_ASSERT( false );
		}
	}
	else
	{
		// write the texture's name.  This can be a file or a system
		// texture.
		URef< GrTextureBase > tex = stage.GetTexture();
		data += tex->GetName().GetPathString();

		// get the texture's format.
		GrTextureBase::EFORMAT format = tex->GetFormat();

		// should we cache the image?
		bool cache = true;
		if ( !gGrTextureMgr->FindTexture( tex->GetName() ) )
			cache = false;

		// do we need to save any flags?
		unsigned int flags = tex->GetFlags();

		// are there any flags that need written?
		bool haveFlags = format != defFormat || flags || !cache;
		if ( haveFlags )
			data += " [ ";

		// save flags.
		if ( flags != 0 )
		{
			// save texture flags.
			SaveTextureFlag( data, flags, GrTextureBase::kNoCompress, "NoCompress" );
			SaveTextureFlag( data, flags, GrTextureBase::kNoMipMap, "NoMipmap" );
			SaveTextureFlag( data, flags, GrTextureBase::kBorder, "Border" );

			// flags get cleared as they're saved.  If we got here, they should have
			// all been saved!
			B_ASSERT( flags == 0 );
			if ( !cache || format != defFormat )
				data += ", ";
		}
		if ( !cache )
		{
			data += "NoCache";
			if ( format != defFormat )
				data += ", ";
		}

		// do we need to save any format flags?
		if ( format != defFormat )
		{
			if ( format == GrTextureBase::EF_NORMALMAP )
				data += "NormalMap";
			else if ( format == GrTextureBase::EF_HIGHDEFNORMALMAP )
				data += "HidefNormalMap";
			else if ( format == GrTextureBase::EF_RGB24 )
				data += "RGB24";
			else if ( format == GrTextureBase::EF_RGBA32 )
				data += "RGBA32";
			else if ( format == GrTextureBase::EF_RGB48F )
				data += "RGB48F";
			else if ( format == GrTextureBase::EF_RGBA64F )
				data += "RGBA64F";
			else if ( format == GrTextureBase::EF_A8 )
				data += "A8";
			else if ( format == GrTextureBase::EF_L8 )
				data += "L8";
			else if ( format == GrTextureBase::EF_LA16 )
				data += "LA16";
			else if ( format == GrTextureBase::EF_LA32F )
				data += "LA32F";
			else if ( format == GrTextureBase::EF_I8 )
				data += "I8";
			else if ( format == GrTextureBase::EF_GUESS )
				data += "Guess";
			else
			{
				// unknown format!
				B_ASSERT( false );
			}
		}

		// save the closing bracket if necessary.
		if ( haveFlags )
			data += " ]";
	}
}

//----------------------------------------------------------
void
EdSave::SaveTextureFlag( tstring& data, unsigned int& curFlags, unsigned int flag, const tstring& flagName )
{
	if ( curFlags & flag )
	{
		curFlags &= ~flag;
		data += flagName;
		if ( curFlags != 0 )
			data += ", ";
	}
}

//----------------------------------------------------------
void
EdSave::SaveBlendFactor( tstring& data, EGRBLENDMODE blendFactor )
{
	if ( blendFactor == EBM_ZERO )
		data += "gl_zero\n";
	else if ( blendFactor == EBM_ONE )
		data += "gl_one\n";
	else if ( blendFactor == EBM_SRC_COLOR )
		data += "gl_src_color\n";
	else if ( blendFactor == EBM_DST_COLOR )
		data += "gl_dst_color\n";
	else if ( blendFactor == EBM_ONE_MINUS_SRC_COLOR )
		data += "gl_one_minus_src_color\n";
	else if ( blendFactor == EBM_ONE_MINUS_DST_COLOR )
		data += "gl_one_minus_dst_color\n";
	else if ( blendFactor == EBM_SRC_ALPHA )
		data += "gl_src_alpha\n";
	else if ( blendFactor == EBM_DST_ALPHA )
		data += "gl_dst_alpha\n";
	else if ( blendFactor == EBM_ONE_MINUS_SRC_ALPHA )
		data += "gl_one_minus_src_alpha\n";
	else if ( blendFactor == EBM_ONE_MINUS_DST_ALPHA )
		data += "gl_one_minus_dst_alpha\n";
	else if ( blendFactor == EBM_SRC_ALPHA_SAT )
		data += "gl_src_alpha\n";
	else
	{
		// unrecognized blend mode!
		B_ASSERT( false );
		data += "gl_one\n";
	}
}

//----------------------------------------------------------
void
EdSave::SaveModelChildren( tstring& data, GrModel* root, bool skybox )
{
	// save each child and recurse.
	unsigned int childCount = root->GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		// get the current child and it's prototype.
		GrModel* curChild = root->GetChild( i );
		GrModel* prototype = gGrModelMgr->GetModel( curChild->GetFileName(), "", false, false );

		// now write out model info.
		data += "\n\tModel ";
		data += curChild->GetInstName().GetPathString();
		data += "\n\t{\n";

		// path.
		data += "\t\tPath\t\t\t\t";
		data += curChild->GetFileName().GetPathString();
		data += "\n";

		// write the transform.
		const MMat4x4& curXForm = curChild->GetLocal();
		WriteXForm( data, curXForm, "\t\t" );

		// flags.
		if ( skybox )
			data += "\t\tSkybox\n";

		// save instance specific node data.
		SaveModelNodeInfo( data, curChild->GetRootNode(), prototype->GetRootNode(), true );

		// save light data.
		SaveModelLightInfo( data, curChild );

		// end.
		data += "\t}\n";

		// recurse children.
		SaveModelChildren( data, curChild, skybox );
	}
}

//----------------------------------------------------------
void
EdSave::SaveModelNodeInfo( tstring& data, GrModelNode* modelRoot, GrModelNode* protoRoot, bool skipLine )
{
	bool nodeWritten = false;

	// is there a different transform?
	bool skipMeshLine = false;
	if ( !ApproxEqual( modelRoot->GetLocal(), protoRoot->GetLocal() ) )
	{
		EnsureNodeInfo( data, modelRoot, nodeWritten, skipLine );
		WriteXForm( data, modelRoot->GetLocal(), "\t\t\t" );
		skipMeshLine = true;
	}

	// compare mesh instances.
	B_ASSERT( modelRoot->GetMeshInstCount() == protoRoot->GetMeshInstCount() );
	unsigned int meshInstCount = Min( modelRoot->GetMeshInstCount(), protoRoot->GetMeshInstCount() );
	for ( unsigned int i = 0; i < meshInstCount; ++i )
	{
		bool meshWritten = false;

		// get the mesh instances.
		GrMeshInst* modelMeshInst = modelRoot->GetMeshInst( i );
		GrMeshInst* protoMeshInst = protoRoot->GetMeshInst( i );

		// compare ranges.
		B_ASSERT( modelMeshInst->GetRangeCount() == protoMeshInst->GetRangeCount() );
		unsigned int rangeCount = Min( modelMeshInst->GetRangeCount(), protoMeshInst->GetRangeCount() );
		for ( unsigned int j = 0; j < rangeCount; ++j )
		{
			bool rangeWritten = false;

			// compare range materials.
			URef< GrMaterial > modelMat = modelMeshInst->GetMaterial( j );
			URef< GrMaterial > protoMat = protoMeshInst->GetMaterial( j );
			if ( modelMat->GetName() != protoMat->GetName() )
			{
				EnsureNodeInfo( data, modelRoot, nodeWritten, skipLine );
				EnsureMeshInfo( data, i, meshWritten, ( i > 0 ) || skipMeshLine );
				EnsureRangeInfo( data, j, rangeWritten, j > 0 );

				// write out the material reference.
				data += "\t\t\t\t\tMaterialId\t\t\t";
				data += modelMat->GetName();
				data += "\n";
			}

			if ( rangeWritten )
				data += "\t\t\t\t}\n";
		}

		if ( meshWritten )
			data += "\t\t\t}\n";
	}

	if ( nodeWritten )
		data += "\t\t}\n";

	// we want to skip a line between nodes if necessary.
	skipLine = skipLine || nodeWritten;

	// recurse.
	B_ASSERT( modelRoot->GetChildCount() == protoRoot->GetChildCount() );
	unsigned int childCount = Min( modelRoot->GetChildCount(), protoRoot->GetChildCount() );
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		GrModelNode* modelChild = modelRoot->GetChild( i );
		GrModelNode* protoChild = protoRoot->GetChild( i );
		SaveModelNodeInfo( data, modelChild, protoChild, skipLine );
	}
}

//----------------------------------------------------------
void
EdSave::SaveModelLightInfo( tstring& data, GrModel* curChild )
{
	unsigned int lightCount = curChild->GetLightCount();
	for ( unsigned int i = 0; i < lightCount; ++i )
	{
		URef< GrLight > light = curChild->GetLight( i );
		GrModelNode* parent = curChild->GetLightParent( i );

		// save out the light's info.
		data += "\t\tlight\t\t\t\t";
		data += light->GetId().GetPathString();
		if ( parent != curChild->GetRootNode() )
		{
			data += ", ";
			data += parent->GetName();
		}
		data += "\n";
	}
}

//----------------------------------------------------------
void
EdSave::EnsureNodeInfo( tstring& data, GrModelNode* node, bool& written, bool skipLine )
{
	if ( !written )
	{
		written = true;
		if ( skipLine )
			data += "\t\t\n";
		data += "\t\tNode ";
		data += node->GetName();
		data += "\n\t\t{\n";
	}
}

//----------------------------------------------------------
void
EdSave::EnsureMeshInfo( tstring& data, unsigned int meshIdx, bool& written, bool skipLine )
{
	if ( !written )
	{
		written = true;
		if ( skipLine )
			data += "\t\t\t\n";
		data += "\t\t\tMesh ";
		data << meshIdx;
		data += "\n\t\t\t{\n";
	}
}

//----------------------------------------------------------
void
EdSave::EnsureRangeInfo( tstring& data, unsigned int rangeIdx, bool& written, bool skipLine )
{
	if ( !written )
	{
		written = true;
		if ( skipLine )
			data += "\t\t\t\t\n";
		data += "\t\t\t\tRange ";
		data << rangeIdx;
		data += "\n\t\t\t\t{\n";
	}
}

//----------------------------------------------------------
void
EdSave::WriteXForm( tstring& data, const MMat4x4& xform, const char* indent, bool writeScale )
{
	// decompose the current child's transform.
	MVec3 position;
	MVec3 scale;
	MMat3x3 rotation;
	xform.GetTranslate( position );
	xform.GetRotate( rotation );
	xform.GetScale( scale );

	// decompose the rotation.
	float y = 0.0f;
	float x = 0.0f;
	float z = 0.0f;
	rotation.ToEulerYXZ( y, x, z );

	// position.
	data += indent;
	data += "Position\t\t\t";
	data << position.GetX();
	data += ", ";
	data << position.GetY();
	data += ", ";
	data << position.GetZ();
	data += "\n";

	// rotation.
	data += indent;
	data += "Rotation\t\t\t";
	data << RadToDeg( y );
	data += ", ";
	data << RadToDeg( x );
	data += ", ";
	data << RadToDeg( z );
	data += "\n";

    if ( writeScale )
    {
        // scale.
        data += indent;
        data += "Scale\t\t\t\t";
        data << Abs( scale.AbsMax() );
        data += "\n";
    }
}

//----------------------------------------------------------
void
EdSave::WriteExpr( tstring& data, URef< UExprTerm > expr )
{
	EdExprCompiler compiler;

	// decompile the expression and write it out.
	tstring temp;
	compiler.Decompile( temp, expr );
	data += temp;
	data += "\n";
}

//----------------------------------------------------------
void
EdSave::WriteColor( tstring& data, const GrColor& color )
{
	data << color.GetR();
	data += ", ";
	data << color.GetG();
	data += ", ";
	data << color.GetB();
	data += ", ";
	data << color.GetA();
	data += "\n";
}

//----------------------------------------------------------
bool
EdSave::ApproxEqual( const MMat4x4& left, const MMat4x4& right )
{
	// compare each float in the matrix down to the 3rd decimal place.
	for ( unsigned int i = 0; i < 16; ++i )
		if ( !FloatApproxEqual( left[ i ], right[ i ] ) )
			return false;

	// return true to indicate approximately equal.
	return true;
}

//----------------------------------------------------------
void            
EdSave::WritePath( tstring& data, const char* pathName, const UPath& path, const char* indent )
{
    // write the indentation.
    data += indent;

    // write the path name.
    data += pathName;

    // write the path, in quotes.
    data += "\"";
    data += path.GetPathString();
    data += "\"\n";
}
