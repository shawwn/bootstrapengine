//----------------------------------------------------------
// File:		reflect.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

invariant gl_Position;

//----------------------------------------------------------
// vertex attributes.
attribute vec3 a_Position;
attribute vec3 a_Tangent;
attribute vec3 a_Binormal;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

//==========================================================
// vertex outputs.
//==========================================================
varying vec3 v_LocalEyeVec;
varying vec4 v_ReflectTexCoord;
varying vec2 v_TexCoord;

//==========================================================
// uniforms.
//==========================================================
uniform mat4 u_ModelViewProjectionMatrix;
uniform vec4 u_ViewPos;				// X,Y,Z,undef
uniform mat4x2 u_TextureMatrix;

//==========================================================
// main.
//==========================================================
void main()
{
	gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );

	// calculate the eye vector.
	vec3 localEyeVec = u_ViewPos.xyz - a_Position.xyz;

	// transform the eye vector into tangent space.
	v_LocalEyeVec.x = dot( localEyeVec, a_Tangent.xyz );
	v_LocalEyeVec.y = dot( localEyeVec, a_Binormal.xyz );
	v_LocalEyeVec.z = dot( localEyeVec, a_Normal.xyz );

	// calculate the reflection texture coordinate.
	v_ReflectTexCoord = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );
	v_ReflectTexCoord.x = 0.5 * v_ReflectTexCoord.x + 0.5 * v_ReflectTexCoord.w;
	v_ReflectTexCoord.y = 0.5 * v_ReflectTexCoord.y + 0.5 * v_ReflectTexCoord.w;

	// calculate the bump map lookup.
	v_TexCoord = u_TextureMatrix * a_TexCoord;
}
