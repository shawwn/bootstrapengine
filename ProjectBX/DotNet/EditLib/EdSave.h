//----------------------------------------------------------
// File:		ECmdSave.h
// Author:		Kevin Bray
// Created:		02-19-07
//
// Purpose:		To provide console commands for saving data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrTextureBase.h"
#include "GrUtil.h"

// std c++ includes.
#include <vector>

// forward declarations.
class GrModel;
class GrModelNode;
class GrMaterialPass;
class MMat4x4;
class UExprTerm;
class GrColor;
class GrTextureStage;
class GrTerrain;

//**********************************************************
// class EdSave
//**********************************************************
class EdSave
{
public:
	EdSave();
	~EdSave();

	bool			SaveSceneData();
	bool			SaveLightData();
	bool			SaveMaterialData();
	bool			SaveBrushData();
	bool			SaveBrushPaletteData();

private:
	void			SaveMaterialPass( tstring& data, GrMaterialPass* pass, bool postProcess );
	void			SaveMaterialStage( tstring& data, const tstring& stageName, const GrTextureStage& stage,
									   const GrColor& defColorMod, const GrColor& defColorAdd, GrTextureBase::EFORMAT defFormat,
									   const tstring& defTexture );
	void			SaveTextureRef( tstring& data, const GrTextureStage& stage, GrTextureBase::EFORMAT defFormat );
	void			SaveTextureFlag( tstring& data, unsigned int& curFlags, unsigned int flag, const tstring& flagName );
	void			SaveBlendFactor( tstring& data, EGRBLENDMODE mode );
	void			SaveModelChildren( tstring& data, GrModel* root, bool skybox );
	void			SaveModelNodeInfo( tstring& data, GrModelNode* modelRoot, GrModelNode* protoRoot, bool skipLine );
	void			SaveModelLightInfo( tstring& data, GrModel* curChild );
	void			EnsureNodeInfo( tstring& data, GrModelNode* node, bool& written, bool skipLine );
	void			EnsureMeshInfo( tstring& data, unsigned int meshIdx, bool& written, bool skipLine );
	void			EnsureRangeInfo( tstring& data, unsigned int rangeIdx, bool& written, bool skipLine );
	void			WriteXForm( tstring& data, const MMat4x4& xform, const char* indent, bool writeScale = true );
	void			WriteExpr( tstring& data, URef< UExprTerm > expr );
	void			ConditionalWriteExpr( tstring& data, URef< UExprTerm > expr, float defaultValue, const tstring& name );
	void			WriteColor( tstring& data, const GrColor& color );
	bool			ApproxEqual( const MMat4x4& left, const MMat4x4& right );

    void            WritePath( tstring& data, const char* pathName, const UPath& path, const char* indent );
};
