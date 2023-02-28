//----------------------------------------------------------
// File:		EdImportDAEMaterials.cpp
// Author:		Kevin Bray
// Created:		11-24-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdImportDAEMaterials.h"

// engine includes.
#include "EdSettings.h"
#include "EdUtil.h"
#include "GrParse.h"
#include "GrColor.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrConfig.h"
#include "GrMaterialMgr.h"
#include "UWriter.h"

// FCollada includes.
#pragma warning ( disable : 4996 )
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDLibrary.h"
//#include "FCDocument/FCDMaterialLibrary.h"
#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDEffectProfile.h"
#include "FCDocument/FCDEffectStandard.h"
#include "FCDocument/FCDTexture.h"
#include "FCDocument/FCDImage.h"
#include "FUtils/FUDAEEnum.h"
#pragma warning ( default : 4996 )

// std c++ includes.
#include <set>


//**********************************************************
// EdImportDAEMaterials
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportDAEMaterials::EdImportDAEMaterials()
{

}

//----------------------------------------------------------
EdImportDAEMaterials::~EdImportDAEMaterials()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdImportDAEMaterials::ImportMaterials( FCDocument* doc, const UPath& outputPath, bool findNormalMaps, bool overwriteExisting )
{
	tstring materialTxt;

	// check to see if the file exists.
	UPath materialFileName = outputPath + GR_MATERIAL_FILENAME;

	// read the text in the existing material file.
	URef< RFile > file = gRFileMgr->GetFile( materialFileName, RFileMgr::kFlagReadWrite );

	// set the overwriteExisting flag to true if the file doesn't exist.
	if ( !file || overwriteExisting )
	{
		// material file output.
		materialTxt =
			"//----------------------------------------------------------\n"
			"// File:		"GR_MATERIAL_FILENAME"\n"
			"// Author:		BootEdit DAE Importer (: Kevin Bray's proxy :)\n"
			"// ";

		materialTxt += gEdSettings->GetCopyrightNotice();
		materialTxt += "\n"
			"//----------------------------------------------------------\n"
			;

		file = gRFileMgr->GetFile( materialFileName, RFileMgr::kFlagReadWrite | RFileMgr::kFlagCreateAlways );
	}
	else
	{
		// initialize the material data to what is currently in the file.
		materialTxt = ( char* )file->GetData();

		// free the file's data.
		file->Purge();
	}

	// dig through the document to import materials.
	FCDMaterialLibrary* materials = doc->GetMaterialLibrary();
	unsigned int matCount = ( unsigned int )materials->GetEntityCount();
	for ( unsigned int i = 0; i < matCount; ++i )
	{
		// get the current material.
		FCDMaterial* curMaterial = materials->GetEntity( i );
		FCDEffectStandard* profile = FindProfile( curMaterial );

		// if the profile wasn't found, skip this material.
		if ( !profile )
			continue;

		// get the texture buckets we care about.
		FCDTexture* ambientTex = GetTexture( profile, FUDaeTextureChannel::AMBIENT );
		FCDTexture* bumpTex = GetTexture( profile, FUDaeTextureChannel::BUMP );
		FCDTexture* diffuseTex = GetTexture( profile, FUDaeTextureChannel::DIFFUSE );
		FCDTexture* emissionTex = GetTexture( profile, FUDaeTextureChannel::EMISSION );
		FCDTexture* specularTex = GetTexture( profile, FUDaeTextureChannel::SPECULAR );
		if ( !specularTex )
		{
			if ( profile->GetTextureCount( FUDaeTextureChannel::DIFFUSE ) > 1 )
				specularTex = profile->GetTexture( FUDaeTextureChannel::DIFFUSE, 1 );
		}

		// note that we'll skip this material if we don't have a valid
		// diffuse map assigned.
		if ( !diffuseTex || diffuseTex->GetImage() == 0 )
		{
			EdPrintF( "Warning: material %s does not contain a diffuse texture; material skipped.\n",
				curMaterial->GetName().c_str() );
			continue;
		}

		// determine the name of the material using the diffuse texture's
		// name.
		tstring materialName = EdFixupTexturePath( outputPath, GetTextureFilePath( diffuseTex ) ).GetPathString();
		size_t pos = materialName.find_first_of( '.' );
		if ( pos != tstring::npos )
			materialName = materialName.substr( 0, pos );

		// store the material's name for future reference.
		_materialNames[ curMaterial ] = materialName;

		// if the current material exists, and we're not supposed to overwrite
		// existing materials, then ignore it.
		if ( !overwriteExisting )
		{
			// if the material already exists, skip it.
			tstring error;
			if ( gGrMaterialMgr->GetMaterial( materialName, error ) )
				continue;
		}

		// write the name of the material.
		materialTxt += "\n";
		materialTxt += materialName;

		// write out the shader.
		materialTxt += "\n{\n\t{\n";
		materialTxt += "\t\tShader\t\t\tblinn\n";

		// write out stages.
		if ( ambientTex )
		{
			ProcessTexStage( materialTxt, "Ambient", "AmbientMap\t\t", ambientTex, profile->GetAmbientColor(),
							 FMVector4( 0.0f, 0.0f, 0.0f, 0.0f ), GR_PARSE_AMBIENT_MOD_DEF, GR_PARSE_AMBIENT_ADD_DEF, outputPath );
		}
		if ( emissionTex )
		{
			ProcessTexStage( materialTxt, "Emissive", "EmissiveMap\t\t", emissionTex, profile->GetEmissionFactor() * profile->GetEmissionColor(),
							 FMVector4( 0.0f, 0.0f, 0.0f, 0.0f ), GR_PARSE_EMISSIVE_MOD_DEF, GR_PARSE_EMISSIVE_ADD_DEF, outputPath );
		}
		if ( bumpTex )
		{
			ProcessTexStage( materialTxt, "Bump", "BumpMap\t\t\t", bumpTex, FMVector4( 0.006f, 0.006f, 0.006f, 0.006f ),
							 FMVector4( -0.003f, -0.003f, -0.003f, -0.003f ), GR_PARSE_BUMP_MOD_DEF, GR_PARSE_BUMP_ADD_DEF, outputPath );

			// hack for normal maps.
			if ( findNormalMaps )
			{
				tstring srcFilePath = GetTextureFilePath( bumpTex );
				size_t extPos = srcFilePath.find_first_of( '.' );
				if ( extPos >= 1 )
				{
					tstring file = srcFilePath.substr( 0, extPos-1 );
					file += "N";
					file += srcFilePath.substr( extPos );
					srcFilePath = file;
				}

				tstring dstFilePath = EdFixupTexturePath( outputPath, srcFilePath ).GetPathString();

				// write out the normal map stage.
				materialTxt += "\t\tNormalMap\t\t";
				materialTxt += dstFilePath;
				materialTxt += " [normalmap]\n";

				// copy the file to the target location.
				CopyTexture( dstFilePath, srcFilePath );
			}
		}
		if ( diffuseTex )
		{
			ProcessTexStage( materialTxt, "Diffuse", "DiffuseMap\t\t", diffuseTex, profile->GetDiffuseColor(),
							 FMVector4( 0.0f, 0.0f, 0.0f, 0.0f ), GR_PARSE_DIFFUSE_MOD_DEF, GR_PARSE_DIFFUSE_ADD_DEF, outputPath );
		}
		if ( specularTex )
		{
			ProcessTexStage( materialTxt, "Specular", "SpecularMap\t\t", specularTex,
							2.0f * profile->GetSpecularFactor() * profile->GetSpecularColor(),
							FMVector4( 0.0f, 0.0f, 0.0f, 0.0f ), GR_PARSE_SPECULAR_MOD_DEF, GR_PARSE_SPECULAR_ADD_DEF, outputPath );
		}

		materialTxt += "\t}\n}\n";
	}

	// write out our material file.
	file->WriteData( 0, materialTxt.c_str(), ( unsigned int )materialTxt.length() );

	// success!
	return true;
}

//----------------------------------------------------------
bool
EdImportDAEMaterials::GetImportedMaterialName( tstring& name, FCDMaterial* material ) const
{
	// try to find the material passed in and if we can't, return false.
	MaterialMap::const_iterator iter = _materialNames.find( material );
	if ( iter == _materialNames.end() )
		return false;

	// set the name and return true to indicate success.
	name = iter->second;
	return true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
FCDEffectStandard*
EdImportDAEMaterials::FindProfile( FCDMaterial* material )
{
	// get the effect from the profile.
	FCDEffect* effect = material->GetEffect();

	// go through all of the profiles and find the COMMON one .
	for ( unsigned int i = 0; i < ( unsigned int )effect->GetProfileCount(); ++i )
	{
		// get the current profile and see if it's the right type.  If so,
		// return a pointer to it.
		FCDEffectProfile* curProfile = effect->GetProfile( 0 );
		if ( curProfile->GetType() == FUDaeProfileType::COMMON )
			return ( FCDEffectStandard* )curProfile;
	}

	// the profile was not found, so simply return NULL.
	return 0;
}

//----------------------------------------------------------
FCDTexture*
EdImportDAEMaterials::GetTexture( FCDEffectStandard* profile, FUDaeTextureChannel::Channel channel )
{
	// get the texture bucket and if it doesn't have any textures, return null.
	if ( profile->GetTextureCount( channel ) == 0 )
		return 0;

	// no textures.
	return profile->GetTexture( channel, 0 );
}

//----------------------------------------------------------
tstring
EdImportDAEMaterials::GetTextureFilePath( FCDTexture* texture )
{
	FCDImage* image = texture->GetImage();
	B_ASSERT( image != 0 );
	return image->GetFilename().c_str();
}

//----------------------------------------------------------
void
EdImportDAEMaterials::CopyTexture( const UPath& dstFilePath, const UPath& srcFilePath )
{
	// if a texture is missing from the destination path, copy it over.
	if ( !gRFileMgr->Exists( dstFilePath ) )
	{
		gRFileMgr->CopyFile( dstFilePath, srcFilePath );
	}
}

//----------------------------------------------------------
void
EdImportDAEMaterials::ProcessTexStage( tstring& materialTxt, const tstring& stageName, const tstring& mapName, FCDTexture* texture,
									  const FMVector4& colorMod, const FMVector4& colorAdd, const GrColor& defaultMod,
									  const GrColor& defaultAdd, const UPath& outputPath )
{
	UPath srcFilePath = GetTextureFilePath( texture );
	UPath dstFilePath = EdFixupTexturePath( outputPath, srcFilePath );

	// if a texture is missing from the destination path, copy it over.
	CopyTexture( dstFilePath, srcFilePath );

	// check to see if we can use the shortened form...
	if ( IsApproxEqual( defaultMod, colorMod ) && IsApproxEqual( defaultAdd, colorAdd ) )
	{
		materialTxt += "\t\t";
		materialTxt += mapName;
		materialTxt += dstFilePath.GetPathString();
		materialTxt += "\n";
	}
	else
	{
		materialTxt += "\t\t";
		materialTxt += stageName;
		materialTxt += "\n\t\t{\n\t\t\tMap\t\t\t\t";
		materialTxt += dstFilePath.GetPathString();
		materialTxt += "\n\t\t\tColorMod\t\t";
		materialTxt << colorMod.x;
		materialTxt += ", ";
		materialTxt << colorMod.y;
		materialTxt += ", ";
		materialTxt << colorMod.z;
		materialTxt += ", ";
		materialTxt << colorMod.w;
		materialTxt += "\n\t\t\tColorAdd\t\t";
		materialTxt << colorAdd.x;
		materialTxt += ", ";
		materialTxt << colorAdd.y;
		materialTxt += ", ";
		materialTxt << colorAdd.z;
		materialTxt += ", ";
		materialTxt << colorAdd.w;
		materialTxt += "\n\t\t}\n";
	}
}

//----------------------------------------------------------
bool
EdImportDAEMaterials::IsApproxEqual( const GrColor& color, const FMVector4& fmColor )
{
	GrColor temp( fmColor.x, fmColor.y, fmColor.z, fmColor.w );
	return temp.ApproxEqual( color );
}
