//----------------------------------------------------------
// File:        blinn.vsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

// ensure positional invariance between passes.
invariant gl_Position;

//==========================================================
// vertex attributes.
//==========================================================
attribute vec3 a_Position;
attribute vec3 a_Tangent;
attribute vec3 a_Binormal;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

//==========================================================
// vertex outputs.
//==========================================================
varying vec3 v_LocalEyeVec;
varying vec3 v_LocalLightVec;
varying vec4 v_ProjTexCoord;
varying vec4 v_ShadowTexCoord;
varying vec2 v_TexCoord;

//==========================================================
// uniforms.
//==========================================================
uniform mat4 u_ModelViewProjectionMatrix;
uniform vec4 u_ViewPos;             // X,Y,Z,undef
uniform vec4 u_LightPos;            // X,Y,Z,undef
uniform vec4 u_LightDirOverride;    // X,Y,Z,[0=override;1=no override]
uniform mat4 u_LightProjMatrix;
uniform mat4 u_ShadowMatrix;
uniform mat4x2 u_TextureMatrix;

//==========================================================
// main.
//==========================================================
void main()
{
    // calculate the outgoing position.
    gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );

    // calculate the eye vector.
    vec3 localEyeVec = u_ViewPos.xyz - a_Position.xyz;

    // calculate the light vector.
    vec3 localLightVec = u_LightPos.xyz - a_Position.xyz;
    localLightVec = u_LightDirOverride.w * localLightVec + u_LightDirOverride.xyz;

    // transform the eye vector into tangent space.
    v_LocalEyeVec.x = dot( localEyeVec, a_Tangent.xyz );
    v_LocalEyeVec.y = dot( localEyeVec, a_Binormal.xyz );
    v_LocalEyeVec.z = dot( localEyeVec, a_Normal.xyz );

    // transform the light vector into tangent space.
    v_LocalLightVec.x = dot( localLightVec, a_Tangent.xyz );
    v_LocalLightVec.y = dot( localLightVec, a_Binormal.xyz );
    v_LocalLightVec.z = dot( localLightVec, a_Normal.xyz );

    // evaluate the projected texture texgens.
    v_ProjTexCoord = u_LightProjMatrix * vec4( a_Position, 1.0 );

    // evaluate the shadow texture texgens.
    v_ShadowTexCoord = u_ShadowMatrix * vec4( a_Position, 1.0 );

    // calculate the bump map lookup.
    v_TexCoord = u_TextureMatrix * vec4( a_TexCoord.xy, 0.0, 1.0 );
}
