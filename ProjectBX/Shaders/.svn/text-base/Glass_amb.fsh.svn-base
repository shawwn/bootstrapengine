//----------------------------------------------------------
// File:		Ambient.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec3 v_LocalEyeVec;
varying vec2 v_TexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BumpMod;
uniform vec4 u_BumpAdd;
uniform vec4 u_DiffuseMod;
uniform vec4 u_DiffuseAdd;
uniform vec4 u_AmbientMod;
uniform vec4 u_AmbientAdd;
uniform vec4 u_EmissiveMod;
uniform vec4 u_EmissiveAdd;
uniform vec4 u_AmbientColor;
uniform vec4 u_FogStart;
uniform vec4 u_FogRange;
uniform vec4 u_FogColor;

//----------------------------------------------------------
// samplers
uniform samplerCube s_NormalizeTexCube;
uniform sampler2D s_StippleTex;
uniform sampler2D s_AtlasTex;
uniform sampler2D s_BumpTex;
uniform sampler2D s_DiffuseTex;
uniform sampler2D s_AmbientTex;
uniform sampler2D s_EmissiveTex;

//----------------------------------------------------------
// main
void main()
{
	//----------------------------------------------------
	// normalize incoming vectors.
	//----------------------------------------------------
	// normalize the eye vector using a texture lookup.
	half3 localEyeVec = 2.0 * textureCube( s_NormalizeTexCube, v_LocalEyeVec ).xyz - 1.0;
	
	// calculate the fresnel effect (dot product with the localEyeVec and
	// the tangent space geometric normal vec3(0, 0, 1) ).
	half fresnel = clamp( localEyeVec.z, 0.0, 1.0 );
	fresnel = 1.0 - fresnel;

	//----------------------------------------------------
	// Calculate the ubertexture UVs.
	//----------------------------------------------------
	half4 atlas = texture2D( s_AtlasTex, v_TexCoord );
	half2 texCoord = atlas.xy * v_TexCoord.xy + atlas.zw;

	//----------------------------------------------------
	// lookup the bump and normal values.
	//----------------------------------------------------
	half bump = texture2D( s_BumpTex, texCoord ).a;
	bump = half( u_BumpMod.x ) * bump + half( u_BumpAdd.x );
	vec2 uvOffset = vec2( bump * localEyeVec.xy );

	//----------------------------------------------------
	// Calculate the diffuse component.
	//----------------------------------------------------
	half4 diffuse = texture2D( s_DiffuseTex, texCoord + uvOffset );
	diffuse = u_DiffuseMod * diffuse + u_DiffuseAdd;

	// calculate alpha to include the fresnel term and premultiply.
	diffuse.a += 0.7 * fresnel;
	diffuse.rgb *= diffuse.a;

	//----------------------------------------------------
	// Calculate the ambience.
	//----------------------------------------------------
	half3 ambient = texture2D( s_AmbientTex, texCoord + uvOffset ).rgb;
	ambient = u_AmbientMod.rgb * ambient + u_AmbientAdd.rgb;
	ambient *= u_AmbientColor.rgb;
	diffuse.rgb *= ambient;

	//----------------------------------------------------
	// Calculate the emissive component.
	//----------------------------------------------------
	half4 emissive = texture2D( s_EmissiveTex, texCoord + uvOffset );
	diffuse.rgb += u_EmissiveMod.rgb * emissive.rgb + u_EmissiveAdd.rgb;

	// lookup the stipple mask.
	diffuse.a *= texture2D( s_StippleTex, 0.5 * gl_FragCoord.xy ).a;

	// output the result.
	gl_FragColor = diffuse;
}
