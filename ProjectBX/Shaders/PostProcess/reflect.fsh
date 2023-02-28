//----------------------------------------------------------
// File:        reflect.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec3 v_LocalEyeVec;
varying vec4 v_ReflectTexCoord;
varying vec2 v_TexCoord;

//----------------------------------------------------------
// parameters
uniform vec4 u_SpecularMod;
uniform vec4 u_SpecularAdd;
uniform vec4 u_BumpMod;
uniform vec4 u_BumpAdd;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_CurRenderTex;
uniform sampler2D s_DiffuseTex;     // really, this contains the reflection.
uniform sampler2D s_BumpTex;
uniform sampler2D s_NormalTex;
uniform sampler2D s_SpecularTex;

//----------------------------------------------------------
// main
void main()
{
    // calculate the bump offset.
    half3 localEyeVec = normalize( v_LocalEyeVec );

    //----------------------------------------------------
    // lookup the bump and normal values.
    //----------------------------------------------------
    half bump = texture2D( s_BumpTex, v_TexCoord ).r;

    bump = u_BumpMod.x * bump + u_BumpAdd.x;
    half2 uvOffset = bump * localEyeVec.xy;

    half3 normal = 2.0 * texture2D( s_NormalTex, v_TexCoord + uvOffset ).wyz - 1.0;
    normal = normalize( normal );

    //----------------------------------------------------
    // calculate the reflected color.
    //----------------------------------------------------
    vec4 reflectTexCoord = v_ReflectTexCoord;
    reflectTexCoord.xy += 10.0 * uvOffset;
    vec4 reflection = texture2DProj( s_DiffuseTex, reflectTexCoord );

    // lookup the specular color and modulate it against the refection.
    reflection *= u_SpecularMod * texture2D( s_SpecularTex, v_TexCoord + uvOffset ) + u_SpecularAdd;

    //----------------------------------------------------
    // blend with the destination color.
    //----------------------------------------------------
    vec4 dstColor = texture2DRect( s_CurRenderTex, gl_FragCoord.xy );

    // return the reflection.
    gl_FragColor = dstColor + reflection;
}
