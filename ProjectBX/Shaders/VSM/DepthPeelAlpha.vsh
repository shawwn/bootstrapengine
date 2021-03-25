//----------------------------------------------------------
// File:		DepthPeelAlpha.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

invariant gl_Position;

//==========================================================
// vertex attributes.
//==========================================================
attribute vec3 a_Position;
attribute vec3 a_Tangent;
attribute vec3 a_Binormal;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

//----------------------------------------------------------
// vertex outputs
varying vec3 v_LocalEyeVec;
varying vec2 v_TexCoord;
varying vec4 v_TopLayerTexCoord;

//----------------------------------------------------------
// uniforms
uniform mat4 u_ModelViewProjectionMatrix;
uniform vec4 u_ViewPos;				// X,Y,Z,undef
uniform mat4x2 u_TextureMatrix;

//----------------------------------------------------------
// main
void main()
{
	// calculate the outgoing position.
	gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );

	// calculate the eye vector.
	vec3 localEyeVec = u_ViewPos.xyz - a_Position.xyz;

	// transform the eye vector into tangent space.
	v_LocalEyeVec.x = dot( localEyeVec, a_Tangent.xyz );
	v_LocalEyeVec.y = dot( localEyeVec, a_Binormal.xyz );
	v_LocalEyeVec.z = dot( localEyeVec, a_Normal.xyz );
	
	// calculate the bump map lookup.
	v_TexCoord = u_TextureMatrix * vec4( a_TexCoord.xy, 0.0, 1.0 );

	// calculate the top layer lookup.
	v_TopLayerTexCoord.x = 0.5 * gl_Position.x + 0.5 * gl_Position.w;
	v_TopLayerTexCoord.y = 0.5 * gl_Position.y + 0.5 * gl_Position.w;
	v_TopLayerTexCoord.z = gl_Position.z;
	v_TopLayerTexCoord.w = gl_Position.w;
}
