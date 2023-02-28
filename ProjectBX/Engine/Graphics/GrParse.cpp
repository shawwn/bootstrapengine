//----------------------------------------------------------
// File:        GrParse.cpp
// Author:      Kevin Bray
// Created:     08-07-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// lib header.
#include "GrParse.h"

// project includes.
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrTexture.h"
#include "GrTextureCube.h"
#include "GrLight.h"
#include "BscLexer.h"
#include "BscExprLib.h"
#include "UExprNumber.h"
#include "UExpr.h"
#include "GrImage.h"
#include "RRsrcMgr.h"
#include "GrTextureMgr.h"
#include "GrColor.h"
#include "GrMaterialMgr.h"
#include "GrScene.h"
#include "UFastArray.h"
#include "GrProjection.h"
#include "MMat3x3.h"
#include "MMat4x4.h"
#include "GrShaderMgr.h"
#include "GrLightMgr.h"
#include "GrDeformWave.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "GrBSPTree.h"
#include "GrModelMgr.h"
#include "GrModel.h"
#include "GrModelNode.h"
#include "GrMeshInst.h"
#include "GrSoftwareTextureMgr.h"
#include "UExprMgr.h"

// std c++ includes.
#include <vector>

// inline helper function.
static bool IsTok( const SBscToken* tok, const char* str )  {   return ( _stricmp( tok->text.c_str(), str ) == 0 ); }

struct SShaderUserParam
{
    tstring name;
    URef< UExprTerm > values[ 4 ];
};

struct SShaderUserSampler
{
    tstring name;
    GrTextureStage stage;
};

struct SModelNodeMeshRangeInfo
{
    SModelNodeMeshRangeInfo() : rangeIdx( 0 ) { }

    int rangeIdx;
    UPath materialId;
};

// model node material reference.
struct SModelNodeMeshInfo
{
    SModelNodeMeshInfo() : meshInstIdx( 0 ) { }

    int meshInstIdx;
    std::vector< SModelNodeMeshRangeInfo > meshRangeInfoVec;
};

struct SModelNodeInfo
{
    SModelNodeInfo() : xform( 0 ) { }

    SModelNodeInfo( const SModelNodeInfo& info )
        : xform( 0 )
    {
        if ( info.xform )
            xform = new MMat4x4( *info.xform );
        nodeName = info.nodeName;
        modelNodeMeshInfoVec = info.modelNodeMeshInfoVec;
    }
    ~SModelNodeInfo() { delete xform; }

    SModelNodeInfo& operator = ( const SModelNodeInfo& info )
    {
        if ( info.xform )
        {
            if ( !xform )
                xform = new MMat4x4( *info.xform );
            else
                *xform = *info.xform;
        }
        nodeName = info.nodeName;
        modelNodeMeshInfoVec = info.modelNodeMeshInfoVec;
        return *this;
    }

    // node name.
    tstring nodeName;
    MMat4x4* xform;
    std::vector< SModelNodeMeshInfo > modelNodeMeshInfoVec;
};

struct SModelLightInfo
{
    tstring lightName;
    tstring parentName;
};

struct SModelInfo
{
    SModelInfo() : clone( true ), skybox( false ) { }

    UPath path;
    UPath instanceName;
    UPath parentInstanceName;
    MMat4x4 xform;
    std::vector< SModelNodeInfo > nodeInfoVec;
    std::vector< SModelLightInfo > lightInfoVec;
    bool clone;
    bool skybox;
};

struct STerrainInfo
{
    STerrainInfo() : found( false ) { }
    UPath path;
    bool found;
    UPath instanceName;
};


/*
struct STextureCubeData
{
    tstring name;
    URef< GrTextureCube > texCube;
};
*/
//**********************************************************
// GrParse forward declarations.
//**********************************************************

//==========================================================
// global functions.
//==========================================================
bool                ParseMaterial( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
bool                ParseLight( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
bool                ParseSceneDef( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );


//==========================================================
// static functions.
//==========================================================
static bool                     ParseModel( SModelInfo& info, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                            const BscVarTable& varTable, tstring& errors );
static bool                     ParseModelNodeInfo( SModelNodeInfo& nodeInfo, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                                    const BscVarTable& varTable, tstring& errors );
static bool                     ParseModelNodeMeshInfo( SModelNodeMeshInfo& meshInfo, const SBscToken*& stream,
                                                       const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
static bool                     ParseModelNodeMeshRangeInfo( SModelNodeMeshRangeInfo& rangeInfo, const SBscToken*& stream,
                                                             const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
static GrMaterialPass*          ParseMaterialPass( const SBscToken*& stream, const BscArrayTable& arrayTable,
                                                   const BscVarTable& varTable, unsigned int curPassCount, tstring& errors );
static bool                     ParseBlendFactor( EGRBLENDMODE& glFactor, const SBscToken*& stream, tstring& errors );
static UPath                    ParseImageRef( const SBscToken*& stream );
static bool                     ParseTextureStage( GrTextureStage& stage, const SBscToken*& stream, GrTextureBase::EFORMAT defTextureFormat,
                                                   const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
static URef< GrTextureBase >    ParseTextureRef( const SBscToken*& stream, GrTextureBase::EFORMAT defFormat, tstring& errors );
static bool                     ParseGenTextureFlags( GrTextureStage& stage, const SBscToken*& stream );
static URef< GrTextureBase >    ParseTextureCubeRef( const SBscToken*& stream, tstring& errors );
static URef< UExprTerm >        ParseExpr( const SBscToken*& stream, const BscArrayTable& arrayTable,
                                           const BscVarTable& varTable, tstring& errors );
static bool                     ParseInteger( int& value, const SBscToken*& stream, tstring& errors );
static bool                     ParseVec3( MVec3& value, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                           const BscVarTable& varTable, tstring& errors );
static bool                     ParseColor( GrColor& value, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                           const BscVarTable& varTable, tstring& errors );
static bool                     ParseVec4Expr( URef< UExprTerm >* values, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                           const BscVarTable& varTable, tstring& errors );
static bool                     ParseTextureFlags( const SBscToken*& stream, GrTextureBase::EFORMAT& format,
                                                  unsigned int& flags, bool& cache, tstring& errors );

static bool                     ParseTerrain( STerrainInfo& info, const SBscToken*& stream, const BscArrayTable& arrayTable,
                                            const BscVarTable& varTable, tstring& errors );

//**********************************************************
// GrParse implementations.
//**********************************************************

//==========================================================
// global functions.
//==========================================================

//----------------------------------------------------------
bool
ParseMaterial( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // make sure the engine is up and running ( or at least what we need to run correctly )
    B_ASSERT( gGrMaterialMgr != 0 );

    typedef UFastArray< SShaderUserParam, 32, 32 > ParamVec;
    typedef UFastArray< SShaderUserSampler, 32, 32 > SamplerVec;

    ParamVec userParams;
    SamplerVec userSamplers;

    GrMaterial::SInfo matInfo;
    matInfo.passes = new GrMaterialPass* [ 8 ]; // up to 8 passes.
    matInfo.passCount = 0;
    matInfo.postProcessPasses = new GrMaterialPass* [ 8 ];  // up to 8 post process passes.
    matInfo.postProcessPassCount = 0;
    matInfo.sortType = GrMaterial::EST_OPAQUE;
    matInfo.castShadow = true;
    matInfo.receiveShadow = true;
    matInfo.stipple = false;

    // parse the name.
    const tstring& name = curTok->text;
    ++curTok;

    // opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // read until the end of the material.
    while ( !IsTok( curTok, "}" ) )
    {
        // check for flags.
        if ( IsTok( curTok, "nocastshadow" ) )
            matInfo.castShadow = false;
        else if ( IsTok( curTok, "noreceiveshadow" ) )
            matInfo.receiveShadow = false;
        else if ( IsTok( curTok, "stipple" ) )
            matInfo.stipple = true;
        else if ( IsTok( curTok, "sort" ) )
        {
            ++curTok;
            if ( IsTok( curTok, "none" ) )
                matInfo.sortType = GrMaterial::EST_OPAQUE;
            else if ( IsTok( curTok, "decal" ) )
                matInfo.sortType = GrMaterial::EST_DECAL;
            else if ( IsTok( curTok, "alpha" ) )
                matInfo.sortType = GrMaterial::EST_ALPHA;
        }
        else if ( IsTok( curTok, "deform" ) )
        {
            // advance past the current token.
            ++curTok;

            // create the proper type of deformer.
            GrDeformer* deformer = 0;
            if ( IsTok( curTok, GrDeformWave::StaticGetName() ) )
            {
                deformer = new GrDeformWave();
                ++curTok;
                matInfo.deformers.Push( deformer );
                continue;
            }
            else
            {
                // don't know what kind of deformer this is.
                errors = "Unknown deformer type specified.";
                return false;
            }
        }/*
        else if ( IsTok( curTok, "postrendershader" ) )
        {
            ++curTok;

            if ( matInfo.postRenderShader != 0 )
            {
                errors = "Shader already specified.";
                return 0;
            }

            // load the shader.
            matInfo.postRenderShader = gGrShaderMgr->GetShader( curTok->text );
        }
        else if ( IsTok( curTok, "postrenderparam" ) )
        {
            ++curTok;

            SShaderUserParam param;
            param.name = curTok->text.c_str();
            ++curTok;

            if ( !ParseVec4Expr( param.values, curTok, arrayTable, varTable, errors ) )
                return 0;

            userParams.Push( param );
            continue;
        }
        else if ( IsTok( curTok, "postrendersampler" ) )
        {
            ++curTok;

            SShaderUserSampler sampler;
            sampler.name = curTok->text.c_str();
            ++curTok;

            ParseTextureStage( sampler.stage, curTok, arrayTable, varTable, errors );

            userSamplers.Push( sampler );
            continue;
        }*/
        else if ( IsTok( curTok, "postprocess" ) )
        {
            ++curTok;

            // parse the pass and move on to the next keyword.
            GrMaterialPass* newPass = ParseMaterialPass( curTok, arrayTable, varTable, matInfo.passCount, errors );
            if ( !newPass )
                return false;

            matInfo.postProcessPasses[ matInfo.postProcessPassCount++ ] = newPass;

            // no need to advance past any token.
            continue;
        }
        else if ( IsTok( curTok, "{" ) )
        {
            // parse the pass and move on to the next keyword.
            GrMaterialPass* newPass = ParseMaterialPass( curTok, arrayTable, varTable, matInfo.passCount, errors );
            if ( !newPass )
                return false;

            matInfo.passes[ matInfo.passCount++ ] = newPass;

            // no need to advance past any token.
            continue;
        }
        else
        {
            // no clue what this is.
            errors = "Unknown expression in material.";
            return false;
        }

        // advance past the current token.
        ++curTok;
    }

    // advance past the closing brace.
    ++curTok;

    // register the material with the material manager.
    GrMaterial* material = gGrMaterialMgr->SetMaterial( name, matInfo );

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
ParseLight( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the name.
    const tstring& id = curTok->text;
    ++curTok;

    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // vars to store information about light.
    SLightInfo lightInfo;
    MVec3 bvCenter;
    float bvRadius = 0.0f;
    MVec3 position;
    MMat3x3 rotation;

    // set the default name to be the light info.
    lightInfo.name = id;

    // read until we hit the end of the light.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "name" ) )
        {
            ++curTok;
            lightInfo.name = curTok->text;
            ++curTok;
        }
        else if ( IsTok( curTok, "parallel" ) )
        {
            ++curTok;
            lightInfo.parallel = true;
        }
        else if ( IsTok( curTok, "global" ) )
        {
            ++curTok;
            lightInfo.global = true;
        }
        else if ( IsTok( curTok, "position" ) )
        {
            ++curTok;
            if ( !ParseVec3( position, curTok, arrayTable, varTable, errors ) )
                return false;
        }
        else if ( IsTok( curTok, "color" ) )
        {
            ++curTok;
            if ( !ParseColor( lightInfo.color, curTok, arrayTable, varTable, errors ) )
                return false;
        }
        else if ( IsTok( curTok, "atten" ) )
        {
            MVec3 atten( 1.0f, 0.0f, 0.0f );
            ++curTok;
            if ( !ParseVec3( atten, curTok, arrayTable, varTable, errors ) )
                return false;

            lightInfo.sqrAtten = atten.GetX();
            lightInfo.linAtten = atten.GetY();
            lightInfo.constAtten = atten.GetZ();
        }
        else if ( IsTok( curTok, "haziness" ) )
        {
            ++curTok;
            URef< UExprTerm > temp = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( temp == 0 )
                return false;
            lightInfo.haziness = Clamp( temp->GetValue(), 0.0f, 1.0f );
        }
        else if ( IsTok( curTok, "spottexture" ) )
        {
            ++curTok;
            if ( IsTok( curTok, "_spotLinear" ) )
            {
                GrTextureBase* tex = gGrTextureMgr->FindTexture( GR_SYSTEX_SPOT_LINEAR );
                B_ASSERT( tex && tex->GetType() == GrTextureBase::ETT_2D );
                if ( tex && tex->GetType() == GrTextureBase::ETT_2D )
                    lightInfo.projTex = ( GrTexture* )tex;
                ++curTok;
            }
            else if ( IsTok( curTok, "_spotQuadratic" ) )
            {
                GrTextureBase* tex = gGrTextureMgr->FindTexture( GR_SYSTEX_SPOT_QUADRATIC );
                B_ASSERT( tex && tex->GetType() == GrTextureBase::ETT_2D );
                if ( tex && tex->GetType() == GrTextureBase::ETT_2D )
                    lightInfo.projTex = ( GrTexture* )tex;
                ++curTok;
            }
            else if ( IsTok( curTok, "_spotConstant" ) )
            {
                GrTextureBase* tex = gGrTextureMgr->FindTexture( GR_SYSTEX_SPOT_CONSTANT );
                B_ASSERT( tex && tex->GetType() == GrTextureBase::ETT_2D );
                if ( tex && tex->GetType() == GrTextureBase::ETT_2D )
                    lightInfo.projTex = ( GrTexture* )tex;
                ++curTok;
            }
            else
            {
                GrTextureBase* tex = ParseTextureRef( curTok, GrTextureBase::EF_GUESS, errors );
                if ( !tex || tex->GetType() != GrTextureBase::ETT_2D )
                    return false;
                lightInfo.projTex = ( GrTexture* )tex;
            }
        }
        else if ( IsTok( curTok, "intensity" ) )
        {
            ++curTok;
            lightInfo.intensity = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( lightInfo.intensity == 0 )
                return false;
        }
        else if ( IsTok( curTok, "range" ) )
        {
            ++curTok;
            lightInfo.range = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( lightInfo.range == 0 )
                return false;
            lightInfo.projection.SetFarDist( Max( lightInfo.range->GetValue(), 1.0f ) );
        }
        else if ( IsTok( curTok, "width" ) )
        {
            ++curTok;
            URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( expr == 0 )
                return false;
            lightInfo.width = expr->GetValue();
        }
        else if ( IsTok( curTok, "nocastshadow" ) )
        {
            ++curTok;
            lightInfo.castShadow = false;
        }
        else if ( IsTok( curTok, "projection" ) )
        {
            ++curTok;
            if ( !IsTok( curTok, "{" ) )
            {
                errors = "Expected opening brace.";
                return false;
            }
            ++curTok;

            while ( !IsTok( curTok, "}" ) )
            {
                if ( IsTok( curTok, "fov" ) )
                {
                    ++curTok;
                    URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
                    if ( !expr )
                        return false;
                    lightInfo.projection.SetFOV( DegToRad( expr->GetValue() ) );
                }
                else if ( IsTok( curTok, "width" ) )
                {
                    ++curTok;
                    URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
                    if ( !expr )
                        return false;
                    lightInfo.projection.SetLeft( -0.5f * expr->GetValue() );
                    lightInfo.projection.SetRight( 0.5f * expr->GetValue() );
                }
                else if ( IsTok( curTok, "height" ) )
                {
                    ++curTok;
                    URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
                    if ( !expr )
                        return false;
                    lightInfo.projection.SetBottom( -0.5f * expr->GetValue() );
                    lightInfo.projection.SetTop( 0.5f * expr->GetValue() );
                }
                else if ( IsTok( curTok, "aspect" ) )
                {
                    ++curTok;
                    URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
                    if ( !expr )
                        return false;
                    float aspect = expr->GetValue();
                    if ( aspect < 0.1 )
                        aspect = 1.0f;
                    lightInfo.projection.SetAspect( aspect );
                }
                else if ( IsTok( curTok, "rotationmatrix" ) )
                {
                    ++curTok;

                    MVec3 right;
                    if ( !ParseVec3( right, curTok, arrayTable, varTable, errors ) )
                        return false;
                    if ( !IsTok( curTok, "," ) )
                    {
                        errors = "Missing comma!";
                        return false;
                    }
                    ++curTok;

                    MVec3 up;
                    if ( !ParseVec3( up, curTok, arrayTable, varTable, errors ) )
                        return false;
                    if ( !IsTok( curTok, "," ) )
                    {
                        errors = "Missing comma!";
                        return false;
                    }
                    ++curTok;

                    MVec3 dir;
                    if ( !ParseVec3( dir, curTok, arrayTable, varTable, errors ) )
                        return false;

                    // combine for final results.
                    rotation.SetRow( 0, right );
                    rotation.SetRow( 1, up );
                    rotation.SetRow( 2, dir );
                }
                else if ( IsTok( curTok, "rotation" ) )
                {
                    ++curTok;

                    MVec3 angles;
                    if ( !ParseVec3( angles, curTok, arrayTable, varTable, errors ) )
                        return false;

                    // roll it, then change it's pitch, then change it's yaw.
                    rotation.FromEulerYXZ( DegToRad( angles.GetX() ), DegToRad( angles.GetY() ), DegToRad( angles.GetZ() ) );
                }
                else if ( IsTok( curTok, "near" ) )
                {
                    ++curTok;
                    URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
                    if ( !expr )
                        return false;
                    float nearDist = Max( expr->GetValue(), 0.05f );
                    lightInfo.projection.SetNearClip( MPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -nearDist ) ) );
                }
                else
                {
                    errors = "Unrecognized expression in light projection.";
                    return false;
                }
            }
            // advance past the closing brace.
            ++curTok;
        }
        else
        {
            errors = "Unrecognized expression in light definition.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    if ( !lightInfo.range )
        lightInfo.range = gUExprMgr->CreateNumber( 100.0f );

    if ( !lightInfo.intensity )
        lightInfo.intensity = gUExprMgr->CreateNumber( 1.0f );

    if ( bvRadius <= 0.0f )
        bvRadius = lightInfo.range->GetValue();

    lightInfo.transform = MMat4x4( rotation, position );
    lightInfo.mode = ELM_ADD;

    // Create a new light and register it with the scene.
    gGrLightMgr->SetLight( id, lightInfo );

    // success!
    return true;
}

//----------------------------------------------------------
bool
ParseSceneDef( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // make sure the engine is actually up and running in a valid state.
    B_ASSERT( gGrScene != 0 );

    // get the current script name (that's our script).  Note
    // that things we do will cause other scripts to be loaded.
    UPath scriptFileName = gRRsrcMgr->GetRecentScriptName();

    // parse the name.
    const tstring& name = curTok->text;
    ++curTok;

    // now lets parse info from the scene definition file.
    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // vector to hold models that need to be instantiated.
    std::vector< SModelInfo > modelInfoVec;

    // a place to store information about the scene.
    GrColor ambience( 1.0f );
    tstring bspPath;
    tstring modelPath;
    bool zUp = false;
    STerrainInfo terrainInfo;

    // scan through tokens until we hit the end of the scene definition.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "bsppath" ) )
        {
            // advance past the keyword.
            ++curTok;

            // store the path and advance past it.
            bspPath = curTok->text;
            ++curTok;
        }
        if ( IsTok( curTok, "modelpath" ) )
        {
            // WARNING: modelpath is depricated.  It should be removed in the future.
            // advance past the keyword.
            ++curTok;

            // store the path and advance past it.
            modelPath = curTok->text;
            ++curTok;
        }
        if ( IsTok( curTok, "model" ) )
        {
            // advance past the keyword.
            ++curTok;

            // now instantiate the model.
            SModelInfo modelInfo;
            if ( !ParseModel( modelInfo, curTok, arrayTable, varTable, errors ) )
                return false;

            // add the new model info to our list.
            modelInfoVec.push_back( modelInfo );
        }
        else if ( IsTok( curTok, "globalambience" ) )
        {
            // advance past the keyword.
            ++curTok;

            // parse the color.  Note that ParseColor automatically
            // advances past the color for us.
            if ( !ParseColor( ambience, curTok, arrayTable, varTable, errors ) )
                return false;
        }
        else if ( IsTok( curTok, "upaxis" ) )
        {
            // advance past the keyword.
            ++curTok;

            // get the axis.
            if ( IsTok( curTok, "z" ) )
            {
                zUp = true;
            }
            else if ( IsTok( curTok, "y" ) )
            {
                zUp = false;
            }
            else
            {
                errors = "Invalid up axis specified!  Only Y or Z are valid options.";
                return false;
            }
            ++curTok;
        }
        else if ( IsTok( curTok, "terrain" ) )
        {
            // advance past the keyword.
            ++curTok;

            if ( !ParseTerrain( terrainInfo, curTok, arrayTable, varTable, errors ) )
                return false;
            terrainInfo.found = true;

        }
        else
        {
            errors = "Unrecognized expression in scene definition.";
            return false;
        }

    }

    // advance past the closing brace.
    ++curTok;

    // now we should load the BSP.
    URef< RFile > file = gRFileMgr->GetFile( bspPath, RFileMgr::kFlagRead );
    if ( !file )
    {
        errors = "Invalid BSP path \"";
        errors += bspPath;
        errors += "\" specified in scene definition.";
        return false;
    }

    // get the data from the file.
    unsigned int bspSize = ( unsigned int )file->GetSize();
    const void* bspBuffer = file->GetData();

    // now try to read in the BSP file.  Note that we let the reader
    // delete bspBuffer for us.
    UReader bspReader( bspBuffer, bspSize, false );
    GrBSPTree* bspTree = new GrBSPTree( bspPath, bspReader );

    // clear the file data.
    file->Purge();

    // set the scene's data.
    gGrScene->Reset( scriptFileName, name, bspTree, zUp );

    // now we should load the default model (if one was specified).
    if ( modelPath.length() > 0 )
    {
        URef< GrModel > model = gGrModelMgr->GetModel( modelPath, GR_SCENE_MODEL_INST_NAME, false, false );
        if ( !model )
        {
            errors = "Invalid model path \"";
            errors += modelPath;
            errors += "\" specified in scene definition.";
            return false;
        }
        gGrScene->GetRootModel()->AddChild( model );
    }

    // add in models.
    unsigned int modelCount = ( unsigned int )modelInfoVec.size();
    for ( unsigned int i = 0; i < modelCount; ++i )
    {
        // get the current model info.
        SModelInfo& curModelInfo = modelInfoVec[ i ];

        // try to get an instance model.
        URef< GrModel > curModel = gGrModelMgr->GetModel( curModelInfo.path, curModelInfo.instanceName, curModelInfo.clone, false );
        if ( !curModel )
        {
            PrintF( "Warning: model \"%s\" not found!\n", curModelInfo.path.GetPathString().c_str() );
            continue;
        }

        // set the model's local transform.
        curModel->SetLocal( curModelInfo.xform );

        // set all of the current model's instance data.
        std::vector< SModelNodeInfo >::iterator iter = curModelInfo.nodeInfoVec.begin();
        const std::vector< SModelNodeInfo >::iterator end = curModelInfo.nodeInfoVec.end();
        for ( ; iter != end; ++iter )
        {
            // get the current node info structure.
            SModelNodeInfo& curModelNodeInfo = *iter;

            // get the root node.
            GrModelNode* root = curModel->GetRootNode();
            B_ASSERT( root != 0 );

            // try to find the node with the current name so that we can
            // operate on it.
            GrModelNode* node = root->FindNode( curModelNodeInfo.nodeName.c_str(), true );
            if ( iter->xform )
                node->SetLocal( *curModelNodeInfo.xform );

            // apply materials to any meshes.
            std::vector< SModelNodeMeshInfo >::iterator meshIter = curModelNodeInfo.modelNodeMeshInfoVec.begin();
            const std::vector< SModelNodeMeshInfo >::iterator meshEnd = curModelNodeInfo.modelNodeMeshInfoVec.end();
            for ( ; meshIter != meshEnd; ++meshIter )
            {
                // get the current mesh index.
                unsigned int unsignedMeshIdx = 0;
                int meshIdx = meshIter->meshInstIdx;
                if ( meshIdx < 0 )
                    unsignedMeshIdx = ( unsigned int )( meshIdx + node->GetMeshInstCount() );
                else
                    unsignedMeshIdx = ( unsigned int )meshIdx;

                // is the mesh index valid?
                if ( unsignedMeshIdx > node->GetMeshInstCount() )
                {
                    PrintF( "Warning: invalid mesh index %d specified!", meshIdx );
                    continue;
                }

                // now we should get the actual mesh.
                GrMeshInst* meshInst = node->GetMeshInst( unsignedMeshIdx );

                // iterate through each range.
                std::vector< SModelNodeMeshRangeInfo >::iterator rangeIter = meshIter->meshRangeInfoVec.begin();
                const std::vector< SModelNodeMeshRangeInfo >::iterator rangeEnd = meshIter->meshRangeInfoVec.end();
                for ( ; rangeIter != rangeEnd; ++rangeIter )
                {
                    // now do stuff on the current range.
                    unsigned int unsignedRangeIdx = 0;
                    int rangeIdx = rangeIter->rangeIdx;
                    if ( rangeIdx < 0 )
                        unsignedRangeIdx = ( unsigned int )( rangeIdx + meshInst->GetRangeCount() );
                    else
                        unsignedRangeIdx = ( unsigned int )rangeIdx;

                    // is the range index valid?
                    if ( unsignedRangeIdx > meshInst->GetRangeCount() )
                    {
                        PrintF( "Warning: invalid range index %d specified!", rangeIdx );
                        continue;
                    }

                    // should we set the material?
                    if ( rangeIter->materialId.GetPathString().length() > 0 )
                    {
                        // try to get the material.
                        URef< GrMaterial > material = gGrMaterialMgr->GetMaterial( rangeIter->materialId, errors );
                        if ( material != 0 )
                        {
                            // set the material.
                            meshInst->SetMaterial( unsignedRangeIdx, material );
                        }
                        else
                        {
                            PrintF( "Warning: invalid material \"%s\" specified!", rangeIter->materialId.GetPathString().c_str() );
                            continue;
                        }
                    }
                }
            }
        }

        // add lights to the model instance.
        std::vector< SModelLightInfo >::iterator lightIter = curModelInfo.lightInfoVec.begin();
        const std::vector< SModelLightInfo >::iterator lightEnd = curModelInfo.lightInfoVec.end();
        for ( ; lightIter != lightEnd; ++lightIter )
        {
            // try to get the light.
            URef< GrLight > light = gGrLightMgr->GetLight( lightIter->lightName, errors );
            if ( !light )
                continue;

            // if a parent was specified, try to find it's node.
            GrModelNode* parentNode = 0;
            if ( lightIter->parentName.length() > 0 )
                parentNode = curModel->GetRootNode()->FindNode( lightIter->parentName.c_str(), true );

            // add the light to the model.
            curModel->AddLight( light, parentNode );
        }

        GrModel* root = gGrScene->GetRootModel();
        if ( curModelInfo.skybox )
            root = gGrScene->GetSkyboxModel();

        // add the model to the scene.
        if ( curModelInfo.parentInstanceName.GetPathString().length() > 0 )
        {
            // search for the parent.
            URef< GrModel > parent = root->FindModelByInstName( curModelInfo.parentInstanceName );
            if ( !parent )
            {
                PrintF( "Warning: invalid parent model specified!\n" );
            }
            else
            {
                parent->AddChild( curModel );
            }
        }
        else
            root->AddChild( curModel );
    }

    gGrScene->SetGlobalAmbience( ambience );
    gGrScene->Cache();

    // return true to indicate success.
    return true;
}


//==========================================================
// static functions.
//==========================================================

//----------------------------------------------------------
bool
ParseModel( SModelInfo& info, const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the instance name.
    info.instanceName = curTok->text;
    ++curTok;

    info.clone = true;
    info.xform = MMat4x4();

    // now lets parse info from the scene definition file.
    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // a place to store information about the scene.
    GrColor ambience( 1.0f );
    tstring bspPath;
    tstring modelPath;
    bool zUp = false;

    // scan through tokens until we hit the end of the model definition.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "position" ) )
        {
            // advance past the current token.
            ++curTok;

            // now parse in the vector.
            MVec3 position;
            if ( !ParseVec3( position, curTok, arrayTable, varTable, errors ) )
                return false;

            // set the position in the model's transform.
            info.xform.SetTranslate( position );
        }
        else if ( IsTok( curTok, "rotation" ) )
        {
            // advance past the current token.
            ++curTok;

            // now parse in the rotation as Y, X, Z
            MVec3 yxzRot;
            if ( !ParseVec3( yxzRot, curTok, arrayTable, varTable, errors ) )
                return false;

            // now set the rotation in the model's transform.
            MMat3x3 newRot( DegToRad( yxzRot.GetX() ), DegToRad( yxzRot.GetY() ), DegToRad( yxzRot.GetZ() ) );
            info.xform.SetRotate( newRot );
        }
        else if ( IsTok( curTok, "scale" ) )
        {
            // advance past the current token.
            ++curTok;

            // now parse in the scale.
            URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( !expr )
                return false;

            // now set the scale in the model's transform.
            float scale = Max( expr->GetValue(), 0.05f );
            info.xform.SetScale( MVec3( scale, scale, scale ) );
        }
        else if ( IsTok( curTok, "path" ) )
        {
            // advance past the current token.
            ++curTok;

            // now store the path.
            info.path = curTok->text;
            ++curTok;
        }
        else if ( IsTok( curTok, "parent" ) )
        {
            // advance past the current token.
            ++curTok;

            // now store the name of the parent.
            info.parentInstanceName = curTok->text;
        }
        else if ( IsTok( curTok, "noclone" ) )
        {
            // advance past the current token.
            ++curTok;

            // set the clone flag to false.
            info.clone = false;
        }
        else if ( IsTok( curTok, "skybox" ) )
        {
            // advance past the current token.
            ++curTok;

            // set the skybox flag to true.
            info.skybox = true;
        }
        else if ( IsTok( curTok, "node" ) )
        {
            // advance past the current token.
            ++curTok;

            // fill out the material struct.
            SModelNodeInfo nodeInfo;
            if ( !ParseModelNodeInfo( nodeInfo, curTok, arrayTable, varTable, errors ) )
            {
                B_ASSERT( false );
                return false;
            }

            // add the material to our list.
            info.nodeInfoVec.push_back( nodeInfo );
        }
        else if ( IsTok( curTok, "light" ) )
        {
            // advance past the current token.
            ++curTok;

            // get the light's name.
            SModelLightInfo lightInfo;
            lightInfo.lightName = curTok->text;
            ++curTok;

            // get the light's parent if necessary.
            if ( curTok->tokenId == BL_COMMA )
            {
                // advance past the comma.
                ++curTok;

                // get the parent.
                lightInfo.parentName = curTok->text;
                ++curTok;
            }

            // add the light to our list.
            info.lightInfoVec.push_back( lightInfo );
        }
        else
        {
            errors = "Unrecognized expression in model declaration.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
ParseModelNodeInfo( SModelNodeInfo& info, const SBscToken*& curTok, const BscArrayTable& arrayTable,
                    const BscVarTable& varTable, tstring& errors )
{
    // parse the node name.
    info.nodeName = curTok->text;
    ++curTok;

    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // scan through tokens until we hit the end of the model material specification.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "mesh" ) )
        {
            // advance past the current token.
            ++curTok;

            // parse the node material ref.
            SModelNodeMeshInfo meshInfo;
            if ( !ParseModelNodeMeshInfo( meshInfo, curTok, arrayTable, varTable, errors ) )
                return false;

            // add the material reference.
            info.modelNodeMeshInfoVec.push_back( meshInfo );
        }
        else if ( IsTok( curTok, "position" ) )
        {
            // advance past the current token.
            ++curTok;

            // create the transform if it doesn't already exist.
            if ( !info.xform )
                info.xform = new MMat4x4();

            // parse in the position.
            MVec3 position;
            if ( !ParseVec3( position, curTok, arrayTable, varTable, errors ) )
                return false;

            // set the transform's position.
            info.xform->SetTranslate( position );
        }
        else if ( IsTok( curTok, "rotation" ) )
        {
            // advance past the current token.
            ++curTok;

            // create the transform if it doesn't already exist.
            if ( !info.xform )
                info.xform = new MMat4x4();

            // parse in the rotation.  Note that the XYZ components of the vector
            // are actually yaw, pitch, and roll in Y-up. (X=Y Y=X Z=Z)
            MVec3 angles;
            if ( !ParseVec3( angles, curTok, arrayTable, varTable, errors ) )
                return false;

            // create the rotation and set it to our transform.
            MMat3x3 rot( DegToRad( angles.GetX() ), DegToRad( angles.GetY() ), DegToRad( angles.GetZ() ) );
            info.xform->SetRotate( rot );
        }
        else if ( IsTok( curTok, "scale" ) )
        {
            // advance past the current token.
            ++curTok;

            // create the transform if it doesn't already exist.
            if ( !info.xform )
                info.xform = new MMat4x4();

            // now parse in the scale.
            URef< UExprTerm > expr = ParseExpr( curTok, arrayTable, varTable, errors );
            if ( !expr )
                return false;

            // now set the scale in the model's transform.
            float scale = Max( expr->GetValue(), 0.05f );
            info.xform->SetScale( MVec3( scale, scale, scale ) );
        }
        else
        {
            // unrecognized expression!
            errors = "Unrecognized expression in model material declaration.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
ParseModelNodeMeshInfo( SModelNodeMeshInfo& meshInfo, const SBscToken*& curTok,
                        const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the mesh index.
    int value = 0;
    if ( !ParseInteger( value, curTok, errors ) )
        return false;

    // store the mesh index.  If the value is negative, it's relative
    // to the last range index (-1 = last, -2 = second to last, etc).
    meshInfo.meshInstIdx = value;

    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // scan through tokens until we hit the end of the model material specification.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "range" ) )
        {
            // advance past the current token.
            ++curTok;

            // parse the range info.
            SModelNodeMeshRangeInfo rangeInfo;
            if ( !ParseModelNodeMeshRangeInfo( rangeInfo, curTok, arrayTable, varTable, errors ) )
                return false;

            // add the range info to our mesh info.
            meshInfo.meshRangeInfoVec.push_back( rangeInfo );
        }
        else
        {
            errors = "Unrecognized expression in mesh declaration.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
ParseModelNodeMeshRangeInfo( SModelNodeMeshRangeInfo& rangeRef, const SBscToken*& curTok,
                             const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the range index.
    if ( !ParseInteger( rangeRef.rangeIdx, curTok, errors ) )
        return false;

    // advance past the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // scan through tokens until we hit the end of the model material specification.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "materialid" ) )
        {
            // advance past the current token.
            ++curTok;

            // store the material's name.
            rangeRef.materialId = curTok->text;
            ++curTok;
        }
        else
        {
            errors = "Unrecognized expression in range declaration.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
GrMaterialPass*
ParseMaterialPass( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, unsigned int curPassCount,
                  tstring& errors )
{
    // make sure the engine is up and running ( or at least we need here ).
    B_ASSERT( gGrMaterialMgr != 0 );

    typedef UFastArray< SShaderUserParam, 32, 32 > ParamVec;
    typedef UFastArray< SShaderUserSampler, 32, 32 > SamplerVec;

    ParamVec userParams;
    SamplerVec userSamplers;

    SMaterialPassInfo matPass;
    matPass.shader = gGrShaderMgr->FindShader( GR_SHADER_DEFAULT_LIGHT );
    matPass.ambientShader = gGrShaderMgr->FindShader( GR_SHADER_DEFAULT_AMBIENT );
    matPass.ambBlendStates.srcAlphaBlendFactor = EBM_ONE;
    matPass.ambBlendStates.dstAlphaBlendFactor = EBM_ZERO;
    matPass.ambBlendStates.srcColorBlendFactor = EBM_ONE;
    matPass.ambBlendStates.dstColorBlendFactor = EBM_ZERO;
    matPass.lightBlendStates.srcAlphaBlendFactor = EBM_ZERO;
    matPass.lightBlendStates.dstAlphaBlendFactor = EBM_ONE;
    matPass.lightBlendStates.srcColorBlendFactor = EBM_ONE;
    matPass.lightBlendStates.dstColorBlendFactor = EBM_ONE;
    matPass.cullMode = ECM_BACK;
    matPass.alphaTestMode = EAM_LESSEQUAL;
    matPass.alphaTestRef = gUExprMgr->CreateNumber( 0.5f );
    matPass.passEnable = gUExprMgr->CreateNumber( 1.0f );
    matPass.receiveShadow = true;
    matPass.enableState = GR_DEPTHTEST | GR_CULLFACE | GR_BLEND;
    matPass.writeMask = GR_RGBA | GR_DEPTH;
    matPass.castShadow = true;
    matPass.vertexColorMod = GrColor( 0.0f, 0.0f, 0.0f, 0.0f );
    matPass.vertexColorAdd = GrColor( 1.0f, 1.0f, 1.0f, 1.0f );
    matPass.minLightResponse = GrColor( 0.0f, 0.0f, 0.0f, 0.0f );
    matPass.maxLightResponse = GrColor( 4.0f, 4.0f, 4.0f, 4.0f );
    matPass.slopeBias = 0.0f;
    matPass.constBias = 0.0f;

    URef< GrTextureBase > white = gGrTextureMgr->FindTexture( GR_SYSTEX_WHITE );
    URef< GrTextureBase > gray = gGrTextureMgr->FindTexture( GR_SYSTEX_GRAY );
    URef< GrTextureBase > black = gGrTextureMgr->FindTexture( GR_SYSTEX_BLACK );
    URef< GrTextureBase > normal = gGrTextureMgr->FindTexture( GR_SYSTEX_NORMAL );

    URef< UExprTerm > sScaleExpr;
    URef< UExprTerm > tScaleExpr;
    URef< UExprTerm > sTranslateExpr;
    URef< UExprTerm > tTranslateExpr;
    URef< UExprTerm > stRotateExpr;

    // override the default for the bump and specular stage.
    GrTextureStage stages[ ES_COUNT ];
    
    stages[ ES_AMBIENT  ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_AMBIENT  ].SetTexColorMod(   GR_PARSE_AMBIENT_MOD_DEF        );
    stages[ ES_AMBIENT  ].SetTexColorAdd(   GR_PARSE_AMBIENT_ADD_DEF        );
    stages[ ES_EMISSIVE ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_EMISSIVE ].SetTexColorMod(   GR_PARSE_EMISSIVE_MOD_DEF       );
    stages[ ES_EMISSIVE ].SetTexColorAdd(   GR_PARSE_EMISSIVE_ADD_DEF       );
    stages[ ES_BUMP     ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_BUMP     ].SetTexColorMod(   GR_PARSE_BUMP_MOD_DEF           );
    stages[ ES_BUMP     ].SetTexColorAdd(   GR_PARSE_BUMP_ADD_DEF           );
    stages[ ES_NORMAL   ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_DIFFUSE  ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_DIFFUSE  ].SetTexColorMod(   GR_PARSE_DIFFUSE_MOD_DEF        );
    stages[ ES_DIFFUSE  ].SetTexColorAdd(   GR_PARSE_DIFFUSE_ADD_DEF        );
    stages[ ES_SPECULAR ].SetFilterMode(    GrTextureStage::EFM_BILINEAR    );
    stages[ ES_SPECULAR ].SetTexColorMod(   GR_PARSE_SPECULAR_MOD_DEF       );
    stages[ ES_SPECULAR ].SetTexColorAdd(   GR_PARSE_SPECULAR_ADD_DEF       );

    // set the default textures.
    stages[ ES_BUMP     ].SetTexture( gray );
    stages[ ES_NORMAL   ].SetTexture( normal );
    stages[ ES_DIFFUSE  ].SetTexture( white );
    stages[ ES_SPECULAR ].SetTexture( black );
    stages[ ES_EMISSIVE ].SetTexture( black );
    stages[ ES_AMBIENT  ].SetTexture( white );

    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return 0;
    }
    ++curTok;

    while ( !IsTok( curTok, "}" ) )
    {
        // flags.
        if ( IsTok( curTok, "noreceiveshadow" ) )
            matPass.receiveShadow = false;
        else if ( IsTok( curTok, "nocastshadow" ) )
            matPass.castShadow = false;
        else if ( IsTok( curTok, "alphatest" ) )
            matPass.enableState |= GR_ALPHATEST;
        else if ( IsTok( curTok, "depthbias" ) )
            matPass.enableState |= GR_DEPTHBIAS;
        else if ( IsTok( curTok, "nodepthtest" ) )
            matPass.enableState &= ~GR_DEPTHTEST;
        else if ( IsTok( curTok, "nocull" ) )
            matPass.enableState &= ~GR_CULLFACE;
        else if ( IsTok( curTok, "nodepthwrite" ) )
            matPass.writeMask &= ~GR_DEPTH;
        // ambient blend states.
        else if ( IsTok( curTok, "ambientsrcblend" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.ambBlendStates.srcColorBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "ambientdstblend" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.ambBlendStates.dstColorBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "ambientsrcblendalpha" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.ambBlendStates.srcAlphaBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "ambientdstblendalpha" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.ambBlendStates.dstAlphaBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        // light blend states.
        else if ( IsTok( curTok, "lightsrcblend" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.lightBlendStates.srcColorBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "lightdstblend" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.lightBlendStates.dstColorBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "lightsrcblendalpha" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.lightBlendStates.srcAlphaBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "lightdstblendalpha" ) )
        {
            // get the blend factor.
            if ( !ParseBlendFactor( matPass.lightBlendStates.dstAlphaBlendFactor, ++curTok, errors ) )
                return false;
            continue;
        }
        else if ( IsTok( curTok, "cull" ) )
        {
            ++curTok;
            if ( IsTok( curTok, "front" ) )
                matPass.cullMode = ECM_FRONT;
            else if ( IsTok( curTok, "back" ) )
                matPass.cullMode = ECM_BACK;
            else
            {
                errors = "Invalid cull face specified.";
                return 0;
            }
        }
        else if ( IsTok( curTok, "alphatestref" ) )
        {
            ++curTok;
            // parse the expression with my overly verbose lib.  This needs changed
            // in the future.
            if ( !BscIsExpr( curTok, arrayTable, varTable ) )
            {
                errors = "Expected expression for AlphaTestRef.";
                return 0;
            }
            URef< UExprTerm > expr;
            curTok += BscParseExpr( &expr, curTok, arrayTable, varTable );
            matPass.alphaTestRef = expr;
            continue;
        }
        else if ( IsTok( curTok, "slopeDepthBias" ) )
        {
            ++curTok;
            // parse the expression with my overly verbose lib.  This needs changed
            // in the future.
            if ( !BscIsExpr( curTok, arrayTable, varTable ) )
            {
                errors = "Expected expression for SlopeDepthBias.";
                return 0;
            }
            URef< UExprTerm > expr;
            curTok += BscParseExpr( &expr, curTok, arrayTable, varTable );
            matPass.slopeBias = expr->GetValue();
            continue;
        }
        else if ( IsTok( curTok, "constDepthBias" ) )
        {
            ++curTok;
            // parse the expression with my overly verbose lib.  This needs changed
            // in the future.
            if ( !BscIsExpr( curTok, arrayTable, varTable ) )
            {
                errors = "Expected expression for ConstDepthBias.";
                return 0;
            }
            URef< UExprTerm > expr;
            curTok += BscParseExpr( &expr, curTok, arrayTable, varTable );
            matPass.constBias = expr->GetValue();
            continue;
        }
        else if ( IsTok( curTok, "passenable" ) )
        {
            ++curTok;
            // parse the expression with my overly verbose lib.  This needs changed
            // in the future.
            if ( !BscIsExpr( curTok, arrayTable, varTable ) )
            {
                errors = "Expected expression for PassEnable.";
                return 0;
            }
            URef< UExprTerm > expr;
            curTok += BscParseExpr( &expr, curTok, arrayTable, varTable );
            matPass.passEnable = expr;
            continue;
        }
        else if ( IsTok( curTok, "vertexColorMod" ) )
        {
            ++curTok;
            if ( !ParseColor( matPass.vertexColorMod, curTok, arrayTable, varTable, errors ) )
                return 0;
            continue;
        }
        else if ( IsTok( curTok, "vertexColorAdd" ) )
        {
            ++curTok;
            if ( !ParseColor( matPass.vertexColorAdd, curTok, arrayTable, varTable, errors ) )
                return 0;
            continue;
        }
        else if ( IsTok( curTok, "minLightResponse" ) )
        {
            ++curTok;
            if ( !ParseColor( matPass.minLightResponse, curTok, arrayTable, varTable, errors ) )
                return 0;
            continue;
        }
        else if ( IsTok( curTok, "maxLightResponse" ) )
        {
            ++curTok;
            if ( !ParseColor( matPass.maxLightResponse, curTok, arrayTable, varTable, errors ) )
                return 0;
            continue;
        }
        else if ( IsTok( curTok, "shader" ) )
        {
            ++curTok;

            // load the shader.
            matPass.shader = gGrShaderMgr->GetShader( curTok->text );
            if ( matPass.shader == 0 )
            {
                errors = "Unable to find shader specified!";
                return 0;
            }
        }
        else if ( IsTok( curTok, "ambientshader" ) )
        {
            ++curTok;

            // load the shader.
            matPass.ambientShader = gGrShaderMgr->GetShader( curTok->text );
            if ( matPass.ambientShader == 0 )
            {
                errors = "Unable to find shader specified!";
                return 0;
            }
        }
        // texture stages
        else if ( IsTok( curTok, "bumpmap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_BUMP ], curTok ) )
                continue;

            URef< GrTextureBase > bumpMap = ParseTextureRef( curTok, GrTextureBase::EF_I8, errors );
            if ( bumpMap != 0 )
                stages[ ES_BUMP ].SetTexture( bumpMap );
            continue;
        }
        else if ( IsTok( curTok, "bump" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_BUMP ], curTok, GrTextureBase::EF_I8, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "normalmap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_NORMAL ], curTok ) )
                continue;

            URef< GrTextureBase > normalMap = ParseTextureRef( curTok, GrTextureBase::EF_NORMALMAP, errors );
            if ( normalMap != 0 )
                stages[ ES_NORMAL ].SetTexture( normalMap );
            continue;
        }
        else if ( IsTok( curTok, "normal" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_NORMAL ], curTok, GrTextureBase::EF_NORMALMAP, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "diffusemap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_DIFFUSE ], curTok ) )
                continue;

            URef< GrTextureBase > diffuseMap = ParseTextureRef( curTok, GrTextureBase::EF_GUESS, errors );
            if ( diffuseMap != 0 )
                stages[ ES_DIFFUSE ].SetTexture( diffuseMap );
            continue;
        }
        else if ( IsTok( curTok, "diffuse" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_DIFFUSE ], curTok, GrTextureBase::EF_GUESS, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "specularmap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_SPECULAR ], curTok ) )
                continue;

            URef< GrTextureBase > specularMap = ParseTextureRef( curTok, GrTextureBase::EF_GUESS, errors );
            if ( specularMap != 0 )
                stages[ ES_SPECULAR ].SetTexture( specularMap );
            continue;
        }
        else if ( IsTok( curTok, "specular" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_SPECULAR ], curTok, GrTextureBase::EF_GUESS, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "ambientmap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_AMBIENT ], curTok ) )
                continue;

            URef< GrTextureBase > ambientMap = ParseTextureRef( curTok, GrTextureBase::EF_GUESS, errors );
            if ( ambientMap != 0 )
                stages[ ES_AMBIENT ].SetTexture( ambientMap );
            continue;
        }
        else if ( IsTok( curTok, "ambient" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_AMBIENT ], curTok, GrTextureBase::EF_GUESS, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "emissivemap" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stages[ ES_EMISSIVE ], curTok ) )
                continue;

            URef< GrTextureBase > emissiveMap = ParseTextureRef( curTok, GrTextureBase::EF_GUESS, errors );
            if ( emissiveMap != 0 )
                stages[ ES_EMISSIVE ].SetTexture( emissiveMap );
            continue;
        }
        else if ( IsTok( curTok, "emissive" ) )
        {
            ++curTok;
            ParseTextureStage( stages[ ES_EMISSIVE ], curTok, GrTextureBase::EF_GUESS, arrayTable, varTable, errors );
            continue;
        }
        // uv scale, rotate, translate
        else if ( IsTok( curTok, "scaleS" ) )
        {
            ++curTok;
            sScaleExpr = ParseExpr( curTok, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "scaleT" ) )
        {
            ++curTok;
            tScaleExpr = ParseExpr( curTok, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "translateS" ) )
        {
            ++curTok;
            sTranslateExpr = ParseExpr( curTok, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "translateT" ) )
        {
            ++curTok;
            tTranslateExpr = ParseExpr( curTok, arrayTable, varTable, errors );
            continue;
        }
        else if ( IsTok( curTok, "rotate" ) )
        {
            ++curTok;
            stRotateExpr = ParseExpr( curTok, arrayTable, varTable, errors );
        }
        // params
        else if ( IsTok( curTok, "userparam" ) )
        {
            ++curTok;

            SShaderUserParam param;
            param.name = curTok->text.c_str();
            ++curTok;

            if ( !ParseVec4Expr( param.values, curTok, arrayTable, varTable, errors ) )
                return 0;

            userParams.Push( param );
            continue;
        }
        else if ( IsTok( curTok, "usersampler" ) )
        {
            ++curTok;

            SShaderUserSampler sampler;
            sampler.name = curTok->text.c_str();
            ++curTok;

            ParseTextureStage( sampler.stage, curTok, GrTextureBase::EF_GUESS, arrayTable, varTable, errors );

            userSamplers.Push( sampler );
            continue;
        }
        else
        {
            errors = "Unrecognized material pass command!";
            return 0;
        }

        // next token.
        ++curTok;
    }

    // advance past the closing brace.
    ++curTok;

    if ( matPass.shader == 0 )
        errors = "Material pass does not specify a shader!";

    // apply the stages to the material pass.
    for ( unsigned int i = 0; i < ES_COUNT; ++i )
        matPass.textures.SetStage( ( ESTAGE )i, stages[ i ] );

    // create the new pass.
    GrMaterialPass* newPass = new GrMaterialPass( curPassCount, matPass );
    newPass->SetTextureTranslateS( sTranslateExpr );
    newPass->SetTextureTranslateT( tTranslateExpr );
    newPass->SetTextureScaleS( sScaleExpr );
    newPass->SetTextureScaleT( tScaleExpr );
    newPass->SetTextureRotate( stRotateExpr );

    // add user params.
    for ( unsigned int i = 0; i < ( unsigned int )( userParams.GetElemCount() ); ++i )
        newPass->GetUserParams().AddUserParam( userParams[ i ].name.c_str(), userParams[ i ].values );

    // add user samplers.
    for ( unsigned int i = 0; i < ( unsigned int )( userSamplers.GetElemCount() ); ++i )
        newPass->GetUserParams().AddUserSampler( userSamplers[ i ].name.c_str(), userSamplers[ i ].stage );

    // return the pass.
    return newPass;
}

//----------------------------------------------------------
bool
ParseBlendFactor( EGRBLENDMODE& glFactor, const SBscToken*& curTok, tstring& errors )
{
    // one and zero.
    if ( IsTok( curTok, "gl_one" ) )
        glFactor = EBM_ONE;
    else if ( IsTok( curTok, "gl_zero" ) )
        glFactor = EBM_ZERO;
    // src factors.
    else if ( IsTok( curTok, "gl_src_alpha" ) )
        glFactor = EBM_SRC_ALPHA;
    else if ( IsTok( curTok, "gl_one_minus_src_alpha" ) )
        glFactor = EBM_ONE_MINUS_SRC_ALPHA;
    else if ( IsTok( curTok, "gl_src_color" ) )
        glFactor = EBM_SRC_COLOR;
    else if ( IsTok( curTok, "gl_one_minus_src_color" ) )
        glFactor = EBM_ONE_MINUS_SRC_COLOR;
    // dest factors.
    else if ( IsTok( curTok, "gl_dst_alpha" ) )
        glFactor = EBM_DST_ALPHA;
    else if ( IsTok( curTok, "gl_one_minus_dst_alpha" ) )
        glFactor = EBM_ONE_MINUS_DST_ALPHA;
    else if ( IsTok( curTok, "gl_dst_color" ) )
        glFactor = EBM_DST_COLOR;
    else if ( IsTok( curTok, "gl_one_minus_dst_color" ) )
        glFactor = EBM_ONE_MINUS_DST_COLOR;
    else
    {
        errors = "Invalid blend factor specified.";
        return false;
    }

    // advance to next token and return.
    ++curTok;
    return true;
}

//----------------------------------------------------------
UPath
ParseImageRef( const SBscToken*& curTok )
{
    // return a path to the image.
    const SBscToken* refTok = curTok++;
    return refTok->text;
}

//----------------------------------------------------------
bool
ParseTextureStage( GrTextureStage& stage, const SBscToken*& curTok, GrTextureBase::EFORMAT defTextureFormat,
                   const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse texture stage information.  This includes filtering and addressing modes.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Expected {";
        return false;
    }
    ++curTok;

    URef< GrTextureBase > texture;
    GrTextureStage::EFILTERMODE filterMode = GrTextureStage::EFM_BILINEAR;
    GrTextureStage::EADDRESSMODE addressModeS = GrTextureStage::EAM_REPEAT;
    GrTextureStage::EADDRESSMODE addressModeT = GrTextureStage::EAM_REPEAT;
    bool mipmapEnable = true;
    while ( !IsTok( curTok, "}" ) )
    {
        // filter mode
        if ( IsTok( curTok, "map" ) )
        {
            ++curTok;
            if ( ParseGenTextureFlags( stage, curTok ) )
                continue;

            // if no special texgen modes were specified, simply try to find a texture.
            URef< GrTextureBase > texture = ParseTextureRef( curTok, defTextureFormat, errors );
            if ( texture != 0 )
                stage.SetTexture( texture );

            continue;
        }
        else if ( IsTok( curTok, "colorMod" ) )
        {
            ++curTok;
            GrColor color;
            if ( !ParseColor( color, curTok, arrayTable, varTable, errors ) )
                return 0;

            stage.SetTexColorMod( color );
            continue;
        }
        else if ( IsTok( curTok, "colorAdd" ) )
        {
            ++curTok;
            GrColor color;
            if ( !ParseColor( color, curTok, arrayTable, varTable, errors ) )
                return 0;

            stage.SetTexColorAdd( color );
            continue;
        }
        else if ( IsTok( curTok, "nomipmap" ) )
            mipmapEnable = false;
        else if ( IsTok( curTok, "mipmap" ) )
            mipmapEnable = true;
        else if ( IsTok( curTok, "anisotropic" ) )
            filterMode = GrTextureStage::EFM_ANISOTROPIC;
        else if ( IsTok( curTok, "trilinear" ) )
            filterMode = GrTextureStage::EFM_TRILINEAR;
        else if ( IsTok( curTok, "bilinear" ) )
            filterMode = GrTextureStage::EFM_BILINEAR;
        else if ( IsTok( curTok, "nearest" ) )
            filterMode = GrTextureStage::EFM_NEAREST;
        // addressing mode
        else if ( IsTok( curTok, "clampS" ) )
            addressModeS = GrTextureStage::EAM_CLAMP;
        else if ( IsTok( curTok, "repeatS" ) )
            addressModeS = GrTextureStage::EAM_REPEAT;
        else if ( IsTok( curTok, "clampT" ) )
            addressModeT = GrTextureStage::EAM_CLAMP;
        else if ( IsTok( curTok, "repeatT" ) )
            addressModeT = GrTextureStage::EAM_REPEAT;
        else
        {
            errors = "Unrecognized expression in texture stage.";
            return false;
        }
        ++curTok;
    }

    // advance past the closing brace.
    ++curTok;

    // fill out the texture stage.
    stage.SetAddressMode( addressModeS, addressModeT );
    stage.SetFilterMode( filterMode );
    stage.SetMipMapEnable( mipmapEnable );

    // success!
    return true;
}

//----------------------------------------------------------
URef< GrTextureBase >
ParseTextureRef( const SBscToken*& curTok, GrTextureBase::EFORMAT defFormat, tstring& errors )
{
    // syntax: data/demo/textures/mytex.tga [ rgba32, nocompress, nomipmap ]
    UPath imagePath = ParseImageRef( curTok );

    // check for any flags.
    GrTextureBase::EFORMAT format = defFormat;
    unsigned int flags = 0;
    bool cache = true;
    if ( IsTok( curTok, "[ " ) )
    {
        if ( !ParseTextureFlags( curTok, format, flags, cache, errors ) )
            return 0;
    }

    // try to create the texture.
    URef< GrTextureBase > texture = gGrTextureMgr->GetTextureFile( imagePath, format, flags, false, cache );
    if ( !texture )
    {
        static char buf[ 1024 ];
        _snprintf_s( buf, sizeof( buf ), sizeof( buf )-1, "Unable to load texture '%s'.", imagePath.GetPathString().c_str() );
        return 0;
    }
    return texture;
}

//----------------------------------------------------------
bool
ParseGenTextureFlags( GrTextureStage& stage, const SBscToken*& stream )
{
    // was a special texgen mode specified?
    if ( IsTok( stream, "_reflection" ) )
    {
        stage.SetGenTextureFlag( GrTextureStage::kGenReflection );
        ++stream;
        return true;
    }
    else if ( IsTok( stream, "_currentDepth" ) )
    {
        stage.SetGenTextureFlag( GrTextureStage::kGenCurDepth );
        ++stream;
        return true;
    }
    else if ( IsTok( stream, "_currentRender" ) )
    {
        stage.SetGenTextureFlag( GrTextureStage::kGenCurRender );
        ++stream;
        return true;
    }
    else if ( IsTok( stream, "_envCube" ) )
    {
        stage.SetGenTextureFlag( GrTextureStage::kGenEnvCube );
        ++stream;
        return true;
    }
    else if ( IsTok( stream, "_envDepthCube" ) )
    {
        stage.SetGenTextureFlag( GrTextureStage::kGenEnvDepthCube );
        ++stream;
        return true;
    }

    // not a valid flag.
    return false;
}

//----------------------------------------------------------
URef< GrTextureBase >
ParseTextureCubeRef( const SBscToken*& curTok, tstring& errors )
{
    UPath images[ 6 ];
    images[ 0 ] = ParseImageRef( curTok );

    for ( unsigned int i = 1; i < 6; ++i )
    {
        if ( !IsTok( curTok, "," ) && i != 6 )
        {
            errors = "Expected comma between image references.";
            return 0;
        }
        ++curTok;

        // parse the next image reference.
        images[ i ] = ParseImageRef( curTok );
    }

    // check for any flags.
    GrTextureBase::EFORMAT format = GrTextureBase::EF_RGBA32;
    unsigned int flags = 0;
    bool cache = true;
    if ( IsTok( curTok, "[ " ) )
    {
        if ( !ParseTextureFlags( curTok, format, flags, cache, errors ) )
            return 0;
    }

    const UPath* imageNamePtrs[] = {
        images + 0,
        images + 1,
        images + 2,
        images + 3,
        images + 4,
        images + 5,
    };

    // create the cubemap.
    return gGrTextureMgr->GetTextureCubeFiles( imageNamePtrs, format, flags, cache );
}

//----------------------------------------------------------
URef< UExprTerm >
ParseExpr( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // set the current term.
    // parse the expression with my overly verbose lib.  This needs changed
    // in the future.
    if ( !BscIsExpr( curTok, arrayTable, varTable ) )
    {
        errors = "Error parsing expression.";
        return 0;
    }
    URef< UExprTerm > expr;
    curTok += BscParseExpr( &expr, curTok, arrayTable, varTable );
    return expr;
}

//----------------------------------------------------------
bool
ParseInteger( int& value, const SBscToken*& curTok, tstring& errors )
{
    // handle the optional sign.
    int sign = 1;
    if ( curTok->tokenId == BL_MINUS )
    {
        sign = -1;
        ++curTok;
    }
    else if ( curTok->tokenId == BL_PLUS )
        ++curTok;

    // make sure we have a number at this point!
    if ( curTok->tokenId != BL_NUMBER )
    {
        errors = "Expecting number, but found \"";
        errors += curTok->text;
        errors += "\".";
        return false;
    }

    // convert from a string to a number.
    value = atoi( curTok->text.c_str() );

    // advance past the number.
    ++curTok;

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool
ParseVec3( MVec3& value, const SBscToken*& curTok, const BscArrayTable& arrayTable,
           const BscVarTable& varTable, tstring& errors )
{
    // parse the x term.
    URef< UExprTerm > xExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( xExpr == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the y term.
    URef< UExprTerm > yExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( yExpr == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the z term.
    URef< UExprTerm > zExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( zExpr == 0 )
        return false;

    // set the position.
    value.SetX( xExpr->GetValue() );
    value.SetY( yExpr->GetValue() );
    value.SetZ( zExpr->GetValue() );

    // success.
    return true;
}

//----------------------------------------------------------
bool
ParseColor( GrColor& value, const SBscToken*& curTok, const BscArrayTable& arrayTable,
            const BscVarTable& varTable, tstring& errors )
{
    // parse the x term.
    URef< UExprTerm > rExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( rExpr == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        // we have a single element color.
        value.SetR( rExpr->GetValue() );
        value.SetG( rExpr->GetValue() );
        value.SetB( rExpr->GetValue() );
        value.SetA( rExpr->GetValue() );
        return true;
    }
    ++curTok;

    // parse the y term.
    URef< UExprTerm > gExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( gExpr == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the z term.
    URef< UExprTerm > bExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( bExpr == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the z term.
    URef< UExprTerm > aExpr = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( aExpr == 0 )
        return false;

    // set the position.
    value.SetR( rExpr->GetValue() );
    value.SetG( gExpr->GetValue() );
    value.SetB( bExpr->GetValue() );
    value.SetA( aExpr->GetValue() );

    // success.
    return true;
}

//----------------------------------------------------------
bool
ParseVec4Expr( URef< UExprTerm >* values, const SBscToken*& curTok, const BscArrayTable& arrayTable,
            const BscVarTable& varTable, tstring& errors )
{
    // parse the x term.
    values[ 0 ] = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( values[ 0 ] == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the y term.
    values[ 1 ] = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( values[ 1 ] == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the z term.
    values[ 2 ] = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( values[ 2 ] == 0 )
        return false;

    if ( !IsTok( curTok, "," ) )
    {
        errors = "Missing comma.";
        return false;
    }
    ++curTok;

    // parse the z term.
    values[ 3 ] = ParseExpr( curTok, arrayTable, varTable, errors );
    if ( values[ 3 ] == 0 )
        return false;

    // success.
    return true;
}

//----------------------------------------------------------
bool
ParseTextureFlags( const SBscToken*& curTok, GrTextureBase::EFORMAT& format, unsigned int& flags, bool& cache, tstring& errors )
{
    cache = true;
    if ( !IsTok( curTok, "[ " ) )
    {
        errors = "Expected [ ";
        return false;
    }
    ++curTok;

    do
    {
        // flags
        if ( IsTok( curTok, "nocompress" ) )
            flags |= GrTextureBase::kNoCompress;
        else if ( IsTok( curTok, "nomipmap" ) )
            flags |= GrTextureBase::kNoMipMap;
        else if ( IsTok( curTok, "border" ) )
            flags |= GrTextureBase::kBorder;
        else if ( IsTok( curTok, "nocache" ) )
            cache = false;

        // format
        else if ( IsTok( curTok, "normalmap" ) )
            format = GrTextureBase::EF_NORMALMAP;
        else if ( IsTok( curTok, "hidefnormalmap" ) )
            format = GrTextureBase::EF_HIGHDEFNORMALMAP;
        else if ( IsTok( curTok, "rgb15_a1" ) )
            format = GrTextureBase::EF_RGB15_A1;
        else if ( IsTok( curTok, "rgb24" ) )
            format = GrTextureBase::EF_RGB24;
        else if ( IsTok( curTok, "rgba32" ) )
            format = GrTextureBase::EF_RGBA32;
        else if ( IsTok( curTok, "rgb48f" ) )
            format = GrTextureBase::EF_RGB48F;
        else if ( IsTok( curTok, "rgba64f" ) )
            format = GrTextureBase::EF_RGBA64F;
        else if ( IsTok( curTok, "alpha8" ) )
            format = GrTextureBase::EF_A8;
        else if ( IsTok( curTok, "l8" ) )
            format = GrTextureBase::EF_L8;
        else if ( IsTok( curTok, "la16" ) )
            format = GrTextureBase::EF_LA16;
        else if ( IsTok( curTok, "la32f" ) )
            format = GrTextureBase::EF_LA32F;
        else if ( IsTok( curTok, "i8" ) )
            format = GrTextureBase::EF_I8;
        else if ( IsTok( curTok, "guess" ) )
            format = GrTextureBase::EF_GUESS;
        else
        {
            errors = "Unrecognized texture flag.";
            return 0;
        }
        ++curTok;

        // make sure we have a comma or a ' ]'.
        if ( IsTok( curTok, "," ) )
            ++curTok;
        else if ( IsTok( curTok, " ]" ) )
        {
            // pass the closing bracket.
            ++curTok;
            break;
        }
        else
        {
            errors = "Expected comma or closing square brace.";
            return true;
        }
    }
    while( true );

    // success!
    return true;
}

//----------------------------------------------------------
bool
ParseTerrain( STerrainInfo& info, const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the instance name.
    info.instanceName = curTok->text;
    ++curTok;

    // now lets parse info from the scene definition file.
    // parse the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // scan through tokens until we hit the end of the model definition.
    while ( !IsTok( curTok, "}" ) )
    {
        if ( IsTok( curTok, "path" ) )
        {
            // advance past the current token.
            ++curTok;

            // now store the path.
            info.path = curTok->text;
            ++curTok;
        }
        else
        {
            errors = "Unrecognized expression in terrain declaration.";
            return false;
        }
    }

    // advance past the closing brace.
    ++curTok;

    // return true to indicate success.
    return true;
}
