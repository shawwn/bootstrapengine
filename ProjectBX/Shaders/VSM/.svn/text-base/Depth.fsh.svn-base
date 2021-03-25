//----------------------------------------------------------
// File:		depth.fsh
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

//----------------------------------------------------------
// samplers
uniform samplerCube s_NormalizeTexCube;
uniform sampler2D s_AtlasTex;
uniform sampler2D s_BumpTex;
uniform sampler2D s_DiffuseTex;

//----------------------------------------------------------
// main
void main()
{
	//----------------------------------------------------
	// normalize incoming vectors.
	//----------------------------------------------------
	// normalize the eye vector using a texture lookup.
	half3 localEyeVec = 2.0 * textureCube( s_NormalizeTexCube, v_LocalEyeVec ).xyz - 1.0;

	//----------------------------------------------------
	// Calculate the ubertexture UVs.
	//----------------------------------------------------
	half4 atlas = texture2D( s_AtlasTex, v_TexCoord );
	half2 texCoord = atlas.xy * v_TexCoord.xy + atlas.zw;

	//----------------------------------------------------
	// lookup the bump and normal values.
	//----------------------------------------------------
	half bump = texture2D( s_BumpTex, texCoord ).a;
	bump = u_BumpMod.x * bump + u_BumpAdd.x;
	half2 uvOffset = bump * localEyeVec.xy;

	//----------------------------------------------------
	// Calculate the diffuse component.
	//----------------------------------------------------
	half alpha = texture2D( s_DiffuseTex, texCoord + uvOffset ).a;
	alpha = u_DiffuseMod.a * alpha + u_DiffuseAdd.a;

	// output the result.
	gl_FragColor = vec4( 0.0, 0.0, 0.0, alpha );
}
