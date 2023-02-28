//----------------------------------------------------------
// File:		GrConfig.h
// Author:		Kevin Bray
// Created:		10-28-04
//
// Purpose:		To define constants that are used throughout the
//				graphics engine.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "enums.h"

//==========================================================
// texture stages
//==========================================================
enum ESTAGE
{
	ES_BUMP,
	ES_NORMAL,
	ES_DIFFUSE,
	ES_SPECULAR,
	ES_EMISSIVE,
	ES_AMBIENT,
	ES_COUNT,
};

//==========================================================
// standard render types
//==========================================================
enum ERENDERTYPE
{
	ERT_NORMAL,
	ERT_DECAL,
	ERT_TRANSLUCENT,
	ERT_COUNT,
};

//==========================================================
// Rendering compile options
//==========================================================

// this works around some alpha test shadow bugs in older drivers.  This comes
// at a cost of performance.
#define GR_OPTION_ALPHATEST_COLOR_WRITE		0
#define GR_OPTION_FRAGMENT_DATATYPE_HALF	0

//==========================================================
// Uber texture compile options
//==========================================================

// New ubertexture constants.
#define GR_UBERTEX_SIZE_IN_TILES			2048
#define GR_UBERTEX_SIZE_IN_TEXELS			131072
#define GR_UBERTEX_TILE_SIZE				64
#define GR_UBERTEX_PATCH_BOUNDARY			512

// Old ubertexture constants.
#define GR_UBERTEX_MAX_SIZE					131072
#define GR_UBERTEX_MAX_SIZE_IN_TILES		2048
#define GR_UBERTEX_DCT_BLOCK_SIZE			8
#define GR_UBERTEX_DCT_QUALITY				90
#define GR_UBERTEX_BPP						4
#define GR_UBERTEX_PELS_PER_TILE			4096
#define GR_UBERTEX_MAX_BYTES_PER_TILE		16384
#define GR_UBERTEX_STAGE_COUNT				(ES_COUNT + 1)
#define GR_UBERTEX_MAX_PER_FRAME			1024
#define GR_UBERTEX_CACHE_BUFFERS			2

// number of color channels for ubertexture stages.
#define GR_UBERTEX_FILE_CHANNELS_BUMP		1
#define GR_UBERTEX_FILE_CHANNELS_NORMAL		2
#define GR_UBERTEX_FILE_CHANNELS_DIFFUSE	4
#define GR_UBERTEX_FILE_CHANNELS_SPECULAR	4
#define GR_UBERTEX_FILE_CHANNELS_AMBIENT	3
#define GR_UBERTEX_FILE_CHANNELS_EMISSIVE	3
#define GR_UBERTEX_FILE_CHANNELS_USER		1
#define GR_UBERTEX_FILE_TILESET_SIZE		73728

// This is the minimum lifetime of a tile (in frames) in the ubertexture cache.
#define GR_UBERTEX_CACHE_MIN_TTL			20

// THESE AREN'T USED.
// various sizes.
#define GR_UBERTEX_LUM_SIZE					1024
#define GR_UBERTEX_BGRA_COMPRESSED_SIZE		1024
#define GR_UBERTEX_BGR_COMPRESSED_SIZE		512

// the cache size should be a power of two.
#define GR_UBERTEX_CACHE_SLOT_COUNT			2048
#define GR_UBERTEX_CACHE_SLOT_MASK			(GR_UBERTEX_CACHE_SLOT_COUNT-1)
#define GR_UBERTEX_CACHE_SIZE				1024
#define GR_UBERTEX_MAX_LOADED				1024

//==========================================================
// Scene compile options
//==========================================================

// Epsilons: the defaults assume the world is in meters.
#define GR_BSP_PORTAL_EPSILON				0.03125f
#define GR_PVS_VISIBILITY_EPSILON			0.03125f
#define GR_BSP_TJUNC_EPSILON				0.03125f
#define GR_BSP_PLANE_EPSILON				0.03125f
#define GR_WORLD_GEOMETRY_SCALE				4096.0f
#define GR_BSP_GEOMETRY_SCALE				8192.0f
#define GR_GENERIC_DISTANCE_EPSILON			0.03125f
#define GR_SMALLEST_UNIT					0.0009765625f

#define GR_SCENE_MODEL_INST_NAME			"scene_model"

//==========================================================
// Graphics resource search paths
//==========================================================
#define GR_PATH_SYSTEM						"data/system/"
#define GR_PATH_SHADERS						"data/system/shaders/"
#define GR_PATH_SHADERS_POSTPROCESS			"postprocess/"

//==========================================================
// graphics resource file names
//==========================================================
#define GR_MATERIAL_FILENAME				"materials.mtr"
#define GR_LIGHTS_FILENAME					"lights.ldf"
#define GR_SCENEDEF_FILENAME				"scene.sdf"

//==========================================================
// standard quality enum
//==========================================================
//enum EGRQUALITY
//{
//	EGQ_ULTRA,
//	EGQ_HIGH,
//	EGQ_MEDIUM,
//	EGQ_LOW,
//	EGQ_COUNT,
//};

//==========================================================
// script resource-keywords.
//==========================================================
#define GR_KW_SCENEDEF						"scene"
#define GR_KW_MATERIAL						"material"
#define GR_KW_LIGHT							"light"

//==========================================================
// shader version number.
//==========================================================
#define GR_GLSL_SHADER_VERSION				120

//==========================================================
// engine driven shader attribute names
//==========================================================
#define GR_ATTRIB_POSITION_NAME				"a_Position"
#define GR_ATTRIB_TANGENT_NAME				"a_Tangent"
#define GR_ATTRIB_BINORMAL_NAME				"a_Binormal"
#define GR_ATTRIB_NORMAL_NAME				"a_Normal"
#define GR_ATTRIB_TEXCOORD_NAME				"a_TexCoord"
#define GR_ATTRIB_COLOR_NAME				"a_Color"

//==========================================================
// engine driven shader attributes.
//==========================================================
#define GR_ATTRIB_POSITION_INDEX			0
#define GR_ATTRIB_TANGENT_INDEX				1
#define GR_ATTRIB_BINORMAL_INDEX			2
#define GR_ATTRIB_NORMAL_INDEX				3
#define GR_ATTRIB_TEXCOORD_INDEX			4
#define GR_ATTRIB_COLOR_INDEX				5

//==========================================================
// engine driven shader attribute enable flags.
//==========================================================
#define GR_ATTRIB_POSITION_MASK				0x0001
#define GR_ATTRIB_TANGENT_MASK				0x0002
#define GR_ATTRIB_BINORMAL_MASK				0x0004
#define GR_ATTRIB_NORMAL_MASK				0x0008
#define GR_ATTRIB_TEXCOORD_MASK				0x0010
#define GR_ATTRIB_COLOR_MASK				0x0020
#define GR_ATTRIB_MASK_ALL					0x003F

//==========================================================
// engine driven shader param names
//==========================================================
#define GR_MODEL_VIEW_PROJECTION_MATRIX_NAME "u_ModelViewProjectionMatrix"
#define GR_MODEL_VIEW_MATRIX_NAME			"u_ModelViewMatrix"
#define GR_LOCAL_VIEW_POSITION_NAME			"u_ViewPos"
#define GR_LOCAL_LIGHT_POSITION_NAME		"u_LightPos"
#define GR_LOCAL_LIGHT_DIR_OVERRIDE_NAME	"u_LightDirOverride"
#define GR_LIGHT_PROJECTION_MATRIX_NAME		"u_LightProjMatrix"
#define GR_SHADOW_PROJECTION_MATRIX_NAME	"u_ShadowMatrix"
#define GR_TEXTURE_MATRIX_NAME				"u_TextureMatrix"
#define GR_CUR_RENDER_TEXTURE_MATRIX_NAME	"u_CurRenderTextureMatrix"
#define GR_ENV_TEXGEN_TEXTURE_MATRIX_NAME	"u_EnvTexGenTextureMatrix"
#define GR_VERTEX_COLOR_MOD_NAME			"u_VertexColorMod"
#define GR_VERTEX_COLOR_ADD_NAME			"u_VertexColorAdd"
#define GR_SHADOW_NEAR_CLIP_NAME			"u_ShadowNear"
#define GR_LIGHT_COLOR_NAME					"u_LightColor"
#define GR_LIGHT_RANGE_NAME					"u_LightRange"
#define GR_LIGHT_ATTEN_NAME					"u_LightAtten"
#define GR_AMBIENT_COLOR_NAME				"u_AmbientColor"
#define GR_BUMP_MOD_NAME					"u_BumpMod"
#define GR_BUMP_ADD_NAME					"u_BumpAdd"
#define GR_DIFFUSE_MOD_NAME					"u_DiffuseMod"
#define GR_DIFFUSE_ADD_NAME					"u_DiffuseAdd"
#define GR_SPECULAR_MOD_NAME				"u_SpecularMod"
#define GR_SPECULAR_ADD_NAME				"u_SpecularAdd"
#define GR_EMISSIVE_MOD_NAME				"u_EmissiveMod"
#define GR_EMISSIVE_ADD_NAME				"u_EmissiveAdd"
#define GR_AMBIENT_MOD_NAME					"u_AmbientMod"
#define GR_AMBIENT_ADD_NAME					"u_AmbientAdd"
#define GR_LIGHT_WIDTH_NAME					"u_LightWidth"
#define GR_LIGHT_HAZE_NAME					"u_LightHaze"
#define GR_FOG_START_NAME					"u_FogStart"
#define GR_FOG_RANGE_NAME					"u_FogRange"
#define GR_FOG_COLOR_NAME					"u_FogColor"

//==========================================================
// engine driven shader sampler names
//==========================================================
#define GR_TEX_CUBE_NORMALIZE_NAME			"s_NormalizeTexCube"
#define GR_TEX_SHADOW_VSM_CUBE_NAME			"s_LightShadowVSM"
#define GR_TEX_CUBE_FACE_X_NAME				"s_CubeFaceX"
#define GR_TEX_CUBE_FACE_Y_NAME				"s_CubeFaceY"
#define GR_TEX_JITTER_NAME					"s_NoiseTableTex3D"
#define GR_TEX_SHADOW_NAME					"s_LightShadowPage"
#define GR_TEX_PROJECTION_NAME				"s_LightProjTex"
#define GR_TEX_ATLAS_NAME					"s_AtlasTex"
#define GR_TEX_BUMP_NAME					"s_BumpTex"
#define GR_TEX_NORMAL_NAME					"s_NormalTex"
#define GR_TEX_DIFFUSE_NAME					"s_DiffuseTex"
#define GR_TEX_SPECULAR_NAME				"s_SpecularTex"

//#define GR_TEX_REFLECTION_NAME			"s_ReflectionTex"
//#define GR_TEX_ENVMAP_NAME				"s_EnvironmentTexCube"

#define GR_TEX_CUR_RENDER_NAME				"s_CurRenderTex"
#define GR_TEX_CUR_DEPTH_NAME				"s_CurDepthTex"

#define GR_TEX_STIPPLE_NAME					"s_StippleTex"
#define GR_TEX_EMISSIVE_NAME				"s_EmissiveTex"
#define GR_TEX_AMBIENT_NAME					"s_AmbientTex"
/*
#define GR_TEX_ENV_CUBE_NAME				"s_EnvTexCube"
#define GR_TEX_ENV_DEPTH_CUBE_NAME			"s_EnvDepthTexCube"
#define GR_TEX_REFLECTION_NAME				"s_ReflectionTex"

#define GR_TEX_USER0_NAME					"s_UserTex0"
#define GR_TEX_USER1_NAME					"s_UserTex1"
*/
//==========================================================
// engine driven samplers
//==========================================================
// ambience pass.
#define GR_TEX_STIPPLE						1
#define GR_TEX_AMBIENT						2
#define GR_TEX_EMISSIVE						3

// light pass.
#define GR_TEX_CUBE_NORMALIZE				0
#define GR_TEX_SHADOW_VSM_CUBE				1
#define GR_TEX_CUBE_FACE_X					2
#define GR_TEX_CUBE_FACE_Y					3
#define GR_TEX_JITTER						4
#define GR_TEX_SHADOW						5
#define GR_TEX_PROJECTION					6
#define GR_TEX_ATLAS						7
#define GR_TEX_BUMP							8
#define GR_TEX_NORMAL						9
#define GR_TEX_DIFFUSE						10
#define GR_TEX_SPECULAR						11

// misc textures.
//#define GR_TEX_REFLECTION					11
//#define GR_TEX_ENVMAP						12

// post render pass.
#define GR_TEX_CUR_RENDER					1
#define GR_TEX_CUR_DEPTH					2

/*
//==========================================================
// standard attribute names
//==========================================================
#define GR_ATTR_COLORS						"a_Color"
#define GR_ATTR_UVS							"a_TexCoord"
#define GR_ATTR_TANGENTS					"a_Tangent"
#define GR_ATTR_BINORMALS					"a_Binormal"
#define GR_ATTR_NORMALS						"a_Normal"
#define GR_ATTR_BONE_INDICES				"a_BoneIndices"
#define GR_ATTR_WEIGHTS						"a_Weights"
*/
/*
//==========================================================
// standard effect parameters
//==========================================================
#define GR_EP_VSUPAXIS						"vsUpAxis"
#define GR_EP_VSPOS							"vsPos"
#define GR_EP_VSDIR							"vsDir"
#define GR_EP_VSMAT							"vsMat"
#define GR_EP_ATTEN							"atten"
#define GR_EP_UPCOLOR						"upColor"
#define GR_EP_DOWNCOLOR						"downColor"
#define GR_EP_COLOR							"color"
#define GR_EP_WIDTH							"width"
#define GR_EP_PROJTEXTURE					"projTexture"
#define GR_EP_PROJTEXTURECUBE				"projTextureCube"
#define GR_EP_SPOTEXP						"spotExp"
#define GR_EP_CONEANGLECOS					"coneAngleCos"
#define GR_EP_MAXCOLOR						"maxColor"
#define GR_EP_SHADOWMATRIX					"shadowMatrix"
#define GR_EP_SHADOWCUBESAMPLER				"shadowTextureCube"
#define GR_EP_SHADOWSAMPLER					"shadowTexture"

#define MAKE_EFFECT_UNIFORM( name )			"effect."name

//==========================================================
// standard instance parameters
//==========================================================
#define GR_IP_MODELMATRIX					"u_ModelMatrix"
#define GR_IP_MODELVIEWMATRIX				"u_ModelViewMatrix"
#define GR_IP_INVVIEWMATRIX					"u_InvViewMatrix"
#define GR_IP_MODELVIEWPROJMATRIX			"u_ModelViewProjMatrix"
#define GR_IP_NORMALMATRIX					"u_NormalMatrix"
#define GR_IP_PROJMATRIX					"u_ProjMatrix"
*/
//==========================================================
// System texture names.
//==========================================================
#define GR_SYSTEX_BLACK						"black"
#define GR_SYSTEX_WHITE						"white"
#define GR_SYSTEX_GRAY						"gray"
#define GR_SYSTEX_BLACKWHITE				"blackwhite"
#define GR_SYSTEX_CUBE_X					"cubex"
#define GR_SYSTEX_CUBE_Y					"cubey"
#define GR_SYSTEX_NORMAL					"normal"
#define GR_SYSTEX_ERROR						"error"
#define GR_SYSTEX_DEPTH						"depth"
#define GR_SYSTEX_LINEAR					"linear"
#define GR_SYSTEX_QUADRATIC					"quadratic"
#define GR_SYSTEX_ATTEN						"atten"
#define GR_SYSTEX_ZLOOKUP					"zlookup"
#define GR_SYSTEX_SPOT_CONSTANT				"spotConstant"
#define GR_SYSTEX_SPOT_LINEAR				"spotLinear"
#define GR_SYSTEX_SPOT_QUADRATIC			"spotQuadratic"
#define GR_SYSTEX_STIPPLE0					"stipple0"
#define GR_SYSTEX_STIPPLE1					"stipple1"
#define GR_SYSTEX_STIPPLE2					"stipple2"
#define GR_SYSTEX_STIPPLE3					"stipple3"

//==========================================================
// System texture 3D names.
//==========================================================
#define GR_SYSTEX3D_JITTER					"jitter"

//==========================================================
// System shaders.
//==========================================================
#define GR_SHADER_DEFAULT_LIGHT				"deflight"
#define GR_SHADER_DEFAULT_AMBIENT			"defambient"

//==========================================================
// System texture cube names.
//==========================================================
#define GR_SYSTEXCUBE_BLACK					"blackcube"
#define GR_SYSTEXCUBE_WHITE					"whitecube"
#define GR_SYSTEXCUBE_NORMALIZE				"normalizecube"
#define GR_SYSTEXCUBE_ERROR					"errorcube"
