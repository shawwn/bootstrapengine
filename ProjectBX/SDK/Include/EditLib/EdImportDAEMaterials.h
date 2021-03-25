//----------------------------------------------------------
// File:		EdImportDAEMaterials.h
// Author:		Kevin Bray
// Created:		11-24-06
//
// Purpose:		To implement importing of collada materials.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// std c++ includes.
#include <map>

// forward declarations.
class FCDocument;
class FCDMaterial;
class FCDTexture;
class FCDEffectStandard;
class FMVector4;
class GrColor;

namespace FUDaeTextureChannel { enum Channel; };

//**********************************************************
// EdImportDAEMaterials
//**********************************************************
class EdImportDAEMaterials
{
public:
	EdImportDAEMaterials();
	~EdImportDAEMaterials();

	bool				ImportMaterials( FCDocument* doc, const UPath& outputPath, bool findNormalMaps, bool overwriteExisting );
	bool				GetImportedMaterialName( tstring& name, FCDMaterial* material ) const;

private:
	typedef std::map< FCDMaterial*, tstring > MaterialMap;

	FCDEffectStandard*	FindProfile( FCDMaterial* material );
	FCDTexture*			GetTexture( FCDEffectStandard* profile, FUDaeTextureChannel::Channel channel );
	tstring				GetTextureFilePath( FCDTexture* texture );
	void				ProcessTexStage( tstring& materialTxt, const tstring& stageName, const tstring& mapName, FCDTexture* texture,
										 const FMVector4& colorMod, const FMVector4& colorAdd, const GrColor& defaultMod,
										 const GrColor& defaultAdd, const UPath& outputPath );
	bool				IsApproxEqual( const GrColor& color, const FMVector4& fmColor );
	void				CopyTexture( const UPath& dstFileName, const UPath& srcFileName );

	MaterialMap			_materialNames;
};
