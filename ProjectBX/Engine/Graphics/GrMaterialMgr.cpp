//----------------------------------------------------------
// File:        GrMaterialMgr.h
// Author:      Kevin Bray
// Created:     03-28-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMaterialMgr.h"

// project includes.
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrShaderMgr.h"
#include "RRsrcMgr.h"
#include "UExprNumber.h"
#include "UExprMgr.h"
#include "GrTextureMgr.h"
#include "GrTextureBase.h"
#include "GrParse.h"

// std c++ includes.
#include <set>

// material manager singleton pointer.
GrMaterialMgr* gGrMaterialMgr = 0;

extern const char* kDefaultLightVP;
extern const char* kDefaultLightFP;
extern const char* kDefaultAmbientVP;
extern const char* kDefaultAmbientFP;

//**********************************************************
// class GrMaterialMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMaterialMgr::GrMaterialMgr()
: _quality( EGQ_LOW )
{
    // make sure the shader manager has been created.
    B_ASSERT( gGrShaderMgr != 0 );

    B_ASSERT( gGrMaterialMgr == 0 );
    gGrMaterialMgr = this;

    // create the default material pass.
    SMaterialPassInfo passInfo;
    passInfo.shader = gGrShaderMgr->GetShader( GR_SHADER_DEFAULT_LIGHT, kDefaultLightVP, kDefaultLightFP );
    passInfo.ambientShader = gGrShaderMgr->GetShader( GR_SHADER_DEFAULT_AMBIENT, kDefaultAmbientVP, kDefaultAmbientFP );
    passInfo.vertexColorMod = GrColor( 0.0f );
    passInfo.vertexColorAdd = GrColor( 0.0f );
    passInfo.alphaTestRef = gUExprMgr->CreateNumber( 0.0f );
    passInfo.passEnable = gUExprMgr->CreateNumber( 1.0f );
    passInfo.receiveShadow = false;
    passInfo.enableState = GR_DEPTHTEST | GR_CULLFACE;
    passInfo.writeMask = GR_RGBA | GR_DEPTH;
    passInfo.castShadow = false;
    passInfo.slopeBias = 0.0f;
    passInfo.constBias = 0.0f;
    passInfo.ambBlendStates.srcColorBlendFactor = EBM_ONE;
    passInfo.ambBlendStates.srcAlphaBlendFactor = EBM_ONE;
    passInfo.ambBlendStates.dstColorBlendFactor = EBM_ZERO;
    passInfo.ambBlendStates.dstAlphaBlendFactor = EBM_ZERO;
    passInfo.lightBlendStates.srcColorBlendFactor = EBM_ONE;
    passInfo.lightBlendStates.srcAlphaBlendFactor = EBM_ONE;
    passInfo.lightBlendStates.dstColorBlendFactor = EBM_ONE;
    passInfo.lightBlendStates.dstAlphaBlendFactor = EBM_ONE;
    passInfo.alphaTestMode = EAM_LESSEQUAL;
    passInfo.cullMode = ECM_BACK;

    URef< GrTextureBase > white = gGrTextureMgr->FindTexture( GR_SYSTEX_WHITE );
    URef< GrTextureBase > black = gGrTextureMgr->FindTexture( GR_SYSTEX_BLACK );
    URef< GrTextureBase > normal = gGrTextureMgr->FindTexture( GR_SYSTEX_NORMAL );

    // override the default for the bump and specular stage.
    passInfo.textures.SetStageTexColorMod( ES_BUMP, GrColor( 0.006f ) );
    passInfo.textures.SetStageTexColorAdd( ES_BUMP, GrColor( -0.003f ) );
    passInfo.textures.SetStageTexColorMod( ES_SPECULAR, GrColor( 2.0f ) );
    passInfo.textures.SetStageTexColorAdd( ES_SPECULAR, GrColor( 0.0f ) );

    // set the default textures.
    passInfo.textures.SetStageTexture( ES_BUMP, white );
    passInfo.textures.SetStageTexture( ES_NORMAL, normal );
    passInfo.textures.SetStageTexture( ES_DIFFUSE, white );
    passInfo.textures.SetStageTexture( ES_SPECULAR, white );
    passInfo.textures.SetStageTexture( ES_EMISSIVE, black );
    passInfo.textures.SetStageTexture( ES_AMBIENT, white );

    GrMaterialPass** passes = new GrMaterialPass*[ 1 ];
    passes[ 0 ] = new GrMaterialPass( 0, passInfo );

    // now create the default material.
    GrMaterial::SInfo matInfo;
    matInfo.passCount = 1;
    matInfo.passes = passes;
    matInfo.postProcessPassCount = 0;
    matInfo.postProcessPasses = 0;
    matInfo.sortType = GrMaterial::EST_OPAQUE;
    matInfo.castShadow = true;
    matInfo.receiveShadow = true;
    matInfo.stipple = false;
    _defaultMaterial = new GrMaterial( "SystemDefault", matInfo );

    // store the default material.
    _materialMap[ _defaultMaterial->GetName() ] = _defaultMaterial;

    // register parsers.
    B_ASSERT( gRRsrcMgr );
    gRRsrcMgr->RegisterScriptParser( GR_KW_MATERIAL, ParseMaterial );
}

//----------------------------------------------------------
GrMaterialMgr::~GrMaterialMgr()
{
    B_ASSERT( gGrMaterialMgr != 0 );
    gGrMaterialMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrMaterial >
GrMaterialMgr::SetMaterial( const tstring& name, const GrMaterial::SInfo& info )
{
    // check to see if the material is already being used.
    MaterialMap::iterator iter = _materialMap.find( name );
    if ( iter != _materialMap.end() )
    {
        // rebuild the material.
        iter->second->Rebuild( info );
        return iter->second;
    }

    // simply create a new material.
    GrMaterial* material = new GrMaterial( name, info );
//  _materialMap.insert( std::pair< UPath, URef< GrMaterial > >( name, material ) );
    _materialMap[ name ] = material;
    _materials.push_back( material );

    // return the material.
    return material;
}

//----------------------------------------------------------
URef< GrMaterial >
GrMaterialMgr::GetMaterial( const UPath& name, tstring& error )
{
    // check to see if the material is already being used.
    MaterialMap::iterator iter = _materialMap.find( name );
    if ( iter != _materialMap.end() )
        return iter->second;

    // if we didn't find it, determine what script we need to load.
    tstring scriptName;
    GetScriptName( scriptName, name.GetPathString() );

    // load the script needed.
    if ( !gRRsrcMgr->LoadScript( scriptName, "material", error ) )
        return 0;

    // try to find the material now that the script was loaded.
    iter = _materialMap.find( name );
    if ( iter != _materialMap.end() )
    {
/*
        // return a clone of the material if it has render-textures
        // associated with it.

        // CHANGE ME!!!
        if ( iter->second->HasDeformers() )
        {
            URef< GrMaterial > clone = iter->second->Clone();
            std::pair< UPath, URef< GrMaterial > >( name, clone );
            _materials.push_back( clone );
            return clone;
        }
*/
        // if there are no deformers associated with the material,
        // simply return the material as is.
        return iter->second;
    }

    // return an error since the material was not found.
    error = "Material " + name.GetPathString() + " not found.";
    return 0;
}

//----------------------------------------------------------
URef< GrMaterial >
GrMaterialMgr::GetDefaultMaterial()
{
    return _defaultMaterial;
}

//----------------------------------------------------------
bool
GrMaterialMgr::ReloadMaterial( const tstring& materialName, tstring& errors )
{
    // try to load the script.
    tstring scriptName;
    GetScriptName( scriptName, materialName );
    return gRRsrcMgr->LoadScript( scriptName, "material", errors );
}

//----------------------------------------------------------
void
GrMaterialMgr::ReloadAll()
{
    // get a list of all of the scripts we need to reload.
    tstring scriptName;
    std::set< tstring > scripts;
    MaterialVec::iterator iter = _materials.begin();
    const MaterialVec::iterator end = _materials.end();
    for ( ; iter != end; ++iter )
    {
        GetScriptName( scriptName, ( *iter )->GetName() );
        if ( scriptName.length() > 0 )
            scripts.insert( scriptName );
    }

    // go through the scripts one by one and reload.
    tstring errors;
    std::set< tstring >::iterator scriptIter = scripts.begin();
    const std::set< tstring >::iterator scriptEnd = scripts.end();
    for ( ; scriptIter != scriptEnd; ++scriptIter )
    {
        gRRsrcMgr->LoadScript( *scriptIter, "material", errors );
    }
}

//----------------------------------------------------------
void
GrMaterialMgr::UnloadAll()
{
    _materialMap.clear();
    _materials.clear();
    _materialMap[ _defaultMaterial->GetName() ] = _defaultMaterial;
}

//----------------------------------------------------------
void
GrMaterialMgr::Update()
{
    MaterialVec::iterator iter = _materials.begin();
    const MaterialVec::iterator end = _materials.end();
    for ( ; iter != end; ++iter )
    {
        ( *iter )->Update();
    }
}

//----------------------------------------------------------
unsigned int
GrMaterialMgr::GetMaterialCount() const
{
    return ( unsigned int )_materials.size();
}

//----------------------------------------------------------
URef< GrMaterial >
GrMaterialMgr::GetMaterialByIdx( unsigned int index )
{
    return _materials[ index ];
}

//----------------------------------------------------------
void
GrMaterialMgr::PreContextReset()
{
/*
    MaterialVec::iterator iter = _materials.begin();
    const MaterialVec::iterator end = _materials.end();
    for ( ; iter != end; ++iter )
    {
        ( *iter )->PreContextReset();
    }
*/
}

//----------------------------------------------------------
void
GrMaterialMgr::PostContextReset()
{
/*
    // notify materials of a context reset.
    MaterialVec::iterator iter = _materials.begin();
    const MaterialVec::iterator end = _materials.end();
    for ( ; iter != end; ++iter )
    {
        ( *iter )->PostContextReset();
    }
*/
    // reload all materials.
//  ReloadAll();
}

//----------------------------------------------------------
void
GrMaterialMgr::GetScriptName( tstring& scriptName, const tstring& materialName )
{
    // if we didn't find it, determine what script we need to load.
    size_t scriptPathEnd = materialName.find_last_of( '/' );
    if ( scriptPathEnd != tstring::npos )
    {
        scriptName = materialName;
        scriptName = scriptName.substr( 0, scriptPathEnd );
        scriptPathEnd = scriptName.find_last_of( '/' );
        if ( scriptPathEnd != tstring::npos )
            scriptName = scriptName.substr( 0, scriptPathEnd + 1 );
    }
    else
        scriptName = "";

    scriptName += GR_MATERIAL_FILENAME;
}

const char* kDefaultLightVP = "\
//----------------------------------------------------------\n\
// File:        blinn.vsh\n\
// Author:      Kevin Bray\n\
// Created:     02-07-06\n\
// Copyright © 2004 Bootstrap Studios.  All rights reserved.\n\
//----------------------------------------------------------\n\
\n\
invariant gl_Position;\n\
\n\
//==========================================================\n\
// vertex attributes.\n\
//==========================================================\n\
attribute vec3 a_Position;\n\
attribute vec3 a_Tangent;\n\
attribute vec3 a_Binormal;\n\
attribute vec3 a_Normal;\n\
attribute vec2 a_TexCoord;\n\
\n\
//==========================================================\n\
// vertex outputs.\n\
//==========================================================\n\
varying vec3 v_LocalEyeVec;\n\
varying vec3 v_LocalLightVec;\n\
varying vec4 v_ProjTexCoord;\n\
varying vec4 v_ShadowTexCoord;\n\
varying vec2 v_TexCoord;\n\
\n\
//==========================================================\n\
// uniforms.\n\
//==========================================================\n\
uniform mat4 u_ModelViewProjectionMatrix;\n\
uniform vec4 u_ViewPos;             // X,Y,Z,undef\n\
uniform vec4 u_LightPos;            // X,Y,Z,undef\n\
uniform vec4 u_LightDirOverride;    // X,Y,Z,[0=override;1=no override]\n\
uniform mat4 u_LightProjMatrix;\n\
uniform mat4 u_ShadowMatrix;\n\
uniform mat4x2 u_TextureMatrix;\n\
\n\
//==========================================================\n\
// main.\n\
//==========================================================\n\
void main()\n\
{\n\
    gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );\n\
\n\
    // calculate the eye vector.\n\
    vec3 localEyeVec = u_ViewPos.xyz - a_Position.xyz;\n\
\n\
    // calculate the light vector.\n\
    vec3 localLightVec = u_LightPos.xyz - a_Position.xyz;\n\
    localLightVec = u_LightDirOverride.w * localLightVec + u_LightDirOverride.xyz;\n\
\n\
    // transform the eye vector into tangent space.\n\
    v_LocalEyeVec.x = dot( localEyeVec, a_Tangent.xyz );\n\
    v_LocalEyeVec.y = dot( localEyeVec, a_Binormal.xyz );\n\
    v_LocalEyeVec.z = dot( localEyeVec, a_Normal.xyz );\n\
\n\
    // transform the light vector into tangent space.\n\
    v_LocalLightVec.x = dot( localLightVec, a_Tangent.xyz );\n\
    v_LocalLightVec.y = dot( localLightVec, a_Binormal.xyz );\n\
    v_LocalLightVec.z = dot( localLightVec, a_Normal.xyz );\n\
\n\
    // evaluate the projected texture texgens.\n\
    v_ProjTexCoord = u_LightProjMatrix * vec4( a_Position.xyz, 1.0 );\n\
\n\
    // evaluate the shadow texture texgens.\n\
    v_ShadowTexCoord = u_ShadowMatrix * vec4( a_Position.xyz, 1.0 );\n\
\n\
    // calculate the bump map lookup.\n\
    v_TexCoord.xy = u_TextureMatrix * vec4( a_TexCoord, 0.0, 1.0 );\n\
}\n\
";

const char* kDefaultLightFP = "\
//----------------------------------------------------------\n\
// File:        blinn.fsh\n\
// Author:      Kevin Bray\n\
// Created:     02-07-06\n\
// Copyright © 2004 Bootstrap Studios.  All rights reserved.\n\
//----------------------------------------------------------\n\
\n\
// constants.\n\
#if B_QUALITY == 0\n\
const int kShadowSampleCount = 32;\n\
#elif B_QUALITY == 1\n\
const int kShadowSampleCount = 16;\n\
#elif B_QUALITY == 2\n\
const int kShadowSampleCount = 8;\n\
#endif\n\
\n\
#if B_QUALITY < 3\n\
const int kShadowSampleCountOver2 = kShadowSampleCount/2;\n\
const half kShadowInvSampleCount = 1.0 / half( kShadowSampleCount );\n\
const vec2 kShadowJitterTexScale = vec2( 0.03125, 0.03125 );            // 1 / 32\n\
//const vec3 kShadowJitterTexScale = vec3( 0.0625, 0.0625, 0.0 );       // 1 / 16\n\
//const vec3 kShadowJitterTexScale = vec3( 0.125, 0.125, 0.0 );         // 1 / 8\n\
#endif\n\
\n\
//----------------------------------------------------------\n\
// vertex inputs\n\
varying vec3 v_LocalEyeVec;\n\
varying vec3 v_LocalLightVec;\n\
varying vec4 v_ProjTexCoord;\n\
varying vec4 v_ShadowTexCoord;\n\
varying vec2 v_TexCoord;\n\
\n\
//----------------------------------------------------------\n\
// parameters\n\
uniform vec4 u_LightColor;          // R,G,B,undef\n\
uniform vec4 u_LightRange;          // range, range, range, 1/range\n\
uniform vec4 u_LightAtten;          // squared atten, linear atten, 0, constant atten\n\
uniform vec4 u_LightWidth;          // width, width, 0, 0\n\
uniform vec4 u_LightHaze;           // haze, 1.0-haze, 0, 0\n\
uniform vec4 u_UberTexCacheMiss;\n\
uniform vec4 u_BumpMod;\n\
uniform vec4 u_BumpAdd;\n\
uniform vec4 u_DiffuseMod;\n\
uniform vec4 u_DiffuseAdd;\n\
uniform vec4 u_SpecularMod;\n\
uniform vec4 u_SpecularAdd;\n\
uniform vec4 u_ShadowNear;\n\
uniform vec4 u_FogStart;\n\
uniform vec4 u_FogRange;\n\
uniform vec4 u_FogColor;\n\
\n\
//----------------------------------------------------------\n\
// samplers\n\
uniform samplerCube s_NormalizeTexCube;\n\
uniform samplerCube s_CubeFaceX;\n\
uniform samplerCube s_CubeFaceY;\n\
uniform sampler3D s_NoiseTableTex3D;\n\
uniform sampler2D s_LightProjTex;\n\
uniform sampler2DShadow s_LightShadowPage;\n\
uniform samplerCube s_LightShadowVSM;\n\
uniform sampler2D s_BumpTex;\n\
uniform sampler2D s_NormalTex;\n\
uniform sampler2D s_DiffuseTex;\n\
uniform sampler2D s_SpecularTex;\n\
\n\
//----------------------------------------------------------\n\
// main\n\
void main()\n\
{\n\
    //----------------------------------------------------\n\
    // normalize incoming vectors.\n\
    //----------------------------------------------------\n\
    // normalize the eye vector using a texture lookup.\n\
    half3 localEyeVec = 2.0 * textureCube( s_NormalizeTexCube, v_LocalEyeVec ).xyz - 1.0;\n\
\n\
    // normalize the light vector with math and calculate both it's length, and it's\n\
    // squared length.\n\
    half localLightDistSqr = dot( v_LocalLightVec, v_LocalLightVec );\n\
    half invLightDist = inversesqrt( localLightDistSqr );\n\
    half localLightDist = invLightDist * localLightDistSqr;\n\
    half3 localLightVec = invLightDist * v_LocalLightVec;\n\
\n\
    // calculate the half angle and normalize with math.\n\
    half3 localHalfVec = normalize( localLightVec + localEyeVec );\n\
\n\
    //----------------------------------------------------\n\
    // calculate attenuated light color as a combination of the light color and the\n\
    // projected texture lookup.  Then scale by the attenuation terms.\n\
    //----------------------------------------------------\n\
    half3 light = u_LightColor.rgb;\n\
    light *= half3( texture2DProj( s_LightProjTex, v_ProjTexCoord ).rgb );\n\
    light = v_ProjTexCoord.z <= 0.0 ? half3( 0.0, 0.0, 0.0 ) : light;\n\
\n\
    // calculate attenuation and scale the light color accordingly.\n\
    half linAtten = clamp( -localLightDist * u_LightRange.w + 1.0, 0.0, 1.0 );  // 1-(d/r)\n\
    light *= dot( vec4( linAtten * linAtten, linAtten, linAtten, 1.0 ), u_LightAtten );\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the shadow texture coordinate.\n\
    //----------------------------------------------------\n\
    vec3 axisX = textureCube( s_CubeFaceX, v_ShadowTexCoord.xyz ).xyz;\n\
    vec3 axisY = textureCube( s_CubeFaceY, v_ShadowTexCoord.xyz ).xyz;\n\
    vec3 maxAxis = abs( v_ShadowTexCoord.xyz );\n\
    half invMA = 1.0 / max( maxAxis.x, max( maxAxis.y, maxAxis.z ) );\n\
\n\
    // calculate cube face X & Y.\n\
    vec3 shadowTexCoord;\n\
    shadowTexCoord.x = dot( axisX, v_ShadowTexCoord.xyz );\n\
    shadowTexCoord.y = dot( axisY, v_ShadowTexCoord.xyz );\n\
    shadowTexCoord.z = 0.5 * u_ShadowNear.z;    // ( z - 0.5 * n ) / z = ( z / z - 0.5 * n / z )\n\
    shadowTexCoord *= invMA;\n\
\n\
    // adjust Z to account for the near plane.\n\
    shadowTexCoord.z += 1.0;\n\
    shadowTexCoord.z *= 0.5 * v_ShadowTexCoord.w + 0.5;\n\
\n\
    // dereference the proper region in the virtual shadow table.\n\
    vec4 shadowMulAdd = textureCube( s_LightShadowVSM, v_ShadowTexCoord.xyz );\n\
    shadowTexCoord.xy = shadowMulAdd.xy * shadowTexCoord.xy + shadowMulAdd.zw;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the shadow.\n\
    //----------------------------------------------------\n\
#if B_QUALITY < 3\n\
    half3 jitterSTQ;\n\
    jitterSTQ.xy = kShadowJitterTexScale.xy * gl_FragCoord.xy;\n\
    jitterSTQ.z = 0.0;\n\
    half center = 0.0;\n\
    vec3 shadowCoord = vec3( 0.0, 0.0, shadowTexCoord.z );\n\
    for ( int i = 0; i < kShadowSampleCountOver2; ++i )\n\
    {\n\
        half4 jitter = shadowMulAdd.xyxy * invMA * ( 2.0 * texture3D( s_NoiseTableTex3D, jitterSTQ ) - 1.0 );\n\
        jitterSTQ.z += kShadowInvSampleCount;\n\
        shadowCoord.xy = u_LightWidth.xx * jitter.xy + shadowTexCoord.xy;\n\
        center += shadow2D( s_LightShadowPage, shadowCoord.xyz ).x;\n\
        shadowCoord.xy = u_LightWidth.xx * jitter.zw + shadowTexCoord.xy;\n\
        center += shadow2D( s_LightShadowPage, shadowCoord.xyz ).x;\n\
    }\n\
\n\
    // apply the shadow mask.\n\
    light *= center * kShadowInvSampleCount;\n\
#else\n\
    light *= shadow2D( s_LightShadowPage, shadowTexCoord ).x;\n\
#endif\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the ubertexture UVs.\n\
    //----------------------------------------------------\n\
    half2 texCoord = v_TexCoord.xy;\n\
\n\
    //----------------------------------------------------\n\
    // lookup the bump and normal values.\n\
    //----------------------------------------------------\n\
    half bump = texture2D( s_BumpTex, texCoord ).r;\n\
    bump = u_BumpMod.x * bump + u_BumpAdd.x;\n\
    half2 uvOffset = bump * localEyeVec.xy;\n\
\n\
    half3 normal = half3( 2.0 ) * texture2D( s_NormalTex, texCoord + uvOffset ).xwz - half3( 1.0 );\n\
    normal = normalize( normal );\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the diffuse component.\n\
    //----------------------------------------------------\n\
    half4 diffuse = texture2D( s_DiffuseTex, texCoord + uvOffset );\n\
    diffuse = u_DiffuseMod * diffuse + u_DiffuseAdd;\n\
    diffuse.rgb *= clamp( dot( localLightVec, normal ), 0.0, 1.0 );\n\
    diffuse.rgb *= light;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the specular component.\n\
    //----------------------------------------------------\n\
    half specularDP = clamp( dot( localHalfVec, normal ), 0.0, 1.0 );\n\
    specularDP = clamp( 4.0 * specularDP - 3.0, 0.0, 1.0 );\n\
    specularDP = pow( specularDP, 20.0 );\n\
\n\
    half3 specular = texture2D( s_SpecularTex, texCoord + uvOffset ).rgb;\n\
    specular = u_SpecularMod.rgb * specular + u_SpecularAdd.rgb;\n\
    specular *= specularDP;\n\
    specular *= light;\n\
    diffuse.rgb += specular;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate light haze.\n\
    //----------------------------------------------------\n\
    diffuse.rgb = mix( diffuse.rgb, light.rgb, u_LightHaze.xxx );\n\
\n\
    //----------------------------------------------------\n\
    // Calculate fog.\n\
    //----------------------------------------------------\n\
//  diffuse.rgb *= 1.0 - clamp( ( length( v_LocalEyeVec ) - u_FogStart.x ) / u_FogRange.x, 0.0, 1.0 );\n\
\n\
    gl_FragColor = diffuse;\n\
}\n\
";

const char* kDefaultAmbientVP = "\
//----------------------------------------------------------\n\
// File:        Ambient.vsh\n\
// Author:      Kevin Bray\n\
// Created:     02-07-06\n\
// Copyright © 2004 Bootstrap Studios.  All rights reserved.\n\
//----------------------------------------------------------\n\
\n\
invariant gl_Position;\n\
\n\
//==========================================================\n\
// vertex attributes.\n\
//==========================================================\n\
attribute vec3 a_Position;\n\
attribute vec3 a_Tangent;\n\
attribute vec3 a_Binormal;\n\
attribute vec3 a_Normal;\n\
attribute vec2 a_TexCoord;\n\
\n\
//----------------------------------------------------------\n\
// vertex outputs\n\
varying vec3 v_LocalEyeVec;\n\
varying vec2 v_TexCoord;\n\
\n\
//----------------------------------------------------------\n\
// uniforms\n\
uniform mat4 u_ModelViewProjectionMatrix;\n\
uniform vec4 u_ViewPos;             // X,Y,Z,undef\n\
uniform mat4x2 u_TextureMatrix;\n\
\n\
//----------------------------------------------------------\n\
// main\n\
void main()\n\
{\n\
    gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );\n\
\n\
    // calculate the eye vector.\n\
    vec3 localEyeVec = u_ViewPos.xyz - a_Position.xyz;\n\
\n\
    // transform the eye vector into tangent space.\n\
    v_LocalEyeVec.x = dot( localEyeVec, a_Tangent.xyz );\n\
    v_LocalEyeVec.y = dot( localEyeVec, a_Binormal.xyz );\n\
    v_LocalEyeVec.z = dot( localEyeVec, a_Normal.xyz );\n\
\n\
    // calculate the bump map lookup.\n\
    v_TexCoord.xy = u_TextureMatrix * vec4( a_TexCoord, 0.0, 1.0 );\n\
}\n\
";

const char* kDefaultAmbientFP = "\
//----------------------------------------------------------\n\
// File:        Ambient.fsh\n\
// Author:      Kevin Bray\n\
// Created:     02-07-06\n\
// Copyright © 2004 Bootstrap Studios.  All rights reserved.\n\
//----------------------------------------------------------\n\
\n\
//----------------------------------------------------------\n\
// vertex inputs\n\
varying vec3 v_LocalEyeVec;\n\
varying vec2 v_TexCoord;\n\
\n\
//----------------------------------------------------------\n\
// uniforms\n\
uniform vec4 u_BumpMod;\n\
uniform vec4 u_BumpAdd;\n\
uniform vec4 u_DiffuseMod;\n\
uniform vec4 u_DiffuseAdd;\n\
uniform vec4 u_AmbientMod;\n\
uniform vec4 u_AmbientAdd;\n\
uniform vec4 u_EmissiveMod;\n\
uniform vec4 u_EmissiveAdd;\n\
uniform vec4 u_AmbientColor;\n\
uniform vec4 u_UberTexCacheMiss;\n\
uniform vec4 u_FogStart;\n\
uniform vec4 u_FogRange;\n\
uniform vec4 u_FogColor;\n\
\n\
//----------------------------------------------------------\n\
// samplers\n\
uniform samplerCube s_NormalizeTexCube;\n\
uniform sampler2D s_StippleTex;\n\
uniform sampler2D s_BumpTex;\n\
uniform sampler2D s_DiffuseTex;\n\
uniform sampler2D s_AmbientTex;\n\
uniform sampler2D s_EmissiveTex;\n\
\n\
//----------------------------------------------------------\n\
// main\n\
void main()\n\
{\n\
    //----------------------------------------------------\n\
    // normalize incoming vectors.\n\
    //----------------------------------------------------\n\
    // normalize the eye vector using a texture lookup.\n\
    half3 localEyeVec = 2.0 * textureCube( s_NormalizeTexCube, v_LocalEyeVec ).xyz - 1.0;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the ubertexture UVs.\n\
    //----------------------------------------------------\n\
    half2 texCoord = v_TexCoord.xy;\n\
\n\
    //----------------------------------------------------\n\
    // lookup the bump and normal values.\n\
    //----------------------------------------------------\n\
    half bump = texture2D( s_BumpTex, texCoord ).r;\n\
    bump = half( u_BumpMod.x ) * bump + half( u_BumpAdd.x );\n\
    vec2 uvOffset = bump * localEyeVec.xy;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the diffuse component.\n\
    //----------------------------------------------------\n\
    half4 diffuse = texture2D( s_DiffuseTex, texCoord + uvOffset );\n\
    diffuse = u_DiffuseMod * diffuse + u_DiffuseAdd;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the ambience.\n\
    //----------------------------------------------------\n\
    half3 ambient = texture2D( s_AmbientTex, texCoord + uvOffset ).rgb;\n\
    ambient = u_AmbientMod.rgb * ambient + u_AmbientAdd.rgb;\n\
    ambient *= u_AmbientColor.rgb;\n\
    diffuse.rgb *= ambient;\n\
\n\
    //----------------------------------------------------\n\
    // Calculate the emissive component.\n\
    //----------------------------------------------------\n\
    half4 emissive = texture2D( s_EmissiveTex, texCoord + uvOffset );\n\
    diffuse.rgb += u_EmissiveMod.rgb * emissive.rgb + u_EmissiveAdd.rgb;\n\
\n\
    // lookup the stipple mask.\n\
    diffuse.a *= texture2D( s_StippleTex, 0.5 * gl_FragCoord.xy ).a;\n\
\n\
    // output the result.\n\
    gl_FragColor = diffuse;\n\
}\n\
";
