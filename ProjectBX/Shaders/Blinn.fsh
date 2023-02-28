//----------------------------------------------------------
// File:        blinn.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_shader_texture_lod : enable

// constants.
#if B_QUALITY == 0
const int kShadowSampleCount = 32;
#elif B_QUALITY == 1
const int kShadowSampleCount = 16;
#elif B_QUALITY == 2
const int kShadowSampleCount = 8;
#endif

#if B_QUALITY < 3
const int kShadowSampleCountOver2 = kShadowSampleCount/2;
const half kShadowInvSampleCount = 1.0 / half( kShadowSampleCount );
const vec2 kShadowJitterTexScale = vec2( 0.03125, 0.03125 );            // 1 / 32
//const vec3 kShadowJitterTexScale = vec3( 0.0625, 0.0625, 0.0 );       // 1 / 16
//const vec3 kShadowJitterTexScale = vec3( 0.125, 0.125, 0.0 );         // 1 / 8
#endif

//----------------------------------------------------------
// vertex inputs
varying vec3 v_LocalEyeVec;
varying vec3 v_LocalLightVec;
varying vec4 v_ProjTexCoord;
varying vec4 v_ShadowTexCoord;
varying vec2 v_TexCoord;

//----------------------------------------------------------
// parameters
uniform vec4 u_LightColor;          // R,G,B,undef
uniform vec4 u_LightRange;          // range, range, range, 1/range
uniform vec4 u_LightAtten;          // squared atten, linear atten, 0, constant atten
uniform vec4 u_LightWidth;          // width, width, 0, 0
uniform vec4 u_LightHaze;           // haze, 1.0-haze, 0, 0
uniform vec4 u_UberTexCacheMiss;
uniform vec4 u_BumpMod;
uniform vec4 u_BumpAdd;
uniform vec4 u_DiffuseMod;
uniform vec4 u_DiffuseAdd;
uniform vec4 u_SpecularMod;
uniform vec4 u_SpecularAdd;
uniform vec4 u_ShadowNear;
uniform vec4 u_FogStart;
uniform vec4 u_FogRange;
uniform vec4 u_FogColor;

//----------------------------------------------------------
// samplers
uniform samplerCube s_NormalizeTexCube;
uniform samplerCube s_CubeFaceX;
uniform samplerCube s_CubeFaceY;
uniform sampler3D s_NoiseTableTex3D;
uniform sampler2D s_LightProjTex;
uniform sampler2DShadow s_LightShadowPage;
uniform samplerCube s_LightShadowVSM;
uniform sampler2D s_BumpTex;
uniform sampler2D s_NormalTex;
uniform sampler2D s_DiffuseTex;
uniform sampler2D s_SpecularTex;

//----------------------------------------------------------
// main
void main()
{
    //----------------------------------------------------
    // normalize incoming vectors.
    //----------------------------------------------------
    // normalize the eye vector using a texture lookup.
    half3 localEyeVec = 2.0 * textureCube( s_NormalizeTexCube, v_LocalEyeVec ).xyz - 1.0;

    // normalize the light vector with math and calculate both it's length, and it's
    // squared length.
    half localLightDistSqr = dot( v_LocalLightVec, v_LocalLightVec );
    half invLightDist = inversesqrt( localLightDistSqr );
    half localLightDist = invLightDist * localLightDistSqr;
    half3 localLightVec = invLightDist * v_LocalLightVec;

    // calculate the half angle and normalize with math.
    half3 localHalfVec = normalize( localLightVec + localEyeVec );

    //----------------------------------------------------
    // calculate attenuated light color as a combination of the light color and the
    // projected texture lookup.  Then scale by the attenuation terms.
    //----------------------------------------------------
    half3 light = u_LightColor.rgb;
    light *= half3( texture2DProj( s_LightProjTex, v_ProjTexCoord ).rgb );
    light = v_ProjTexCoord.z <= 0.0 ? half3( 0.0, 0.0, 0.0 ) : light;

    // calculate attenuation and scale the light color accordingly.
    half linAtten = clamp( -localLightDist * u_LightRange.w + 1.0, 0.0, 1.0 );  // 1-(d/r)
    light *= dot( vec4( linAtten * linAtten, linAtten, linAtten, 1.0 ), u_LightAtten );

    //----------------------------------------------------
    // Calculate the shadow texture coordinate.
    //----------------------------------------------------
    vec3 axisX = textureCube( s_CubeFaceX, v_ShadowTexCoord.xyz ).xyz;
    vec3 axisY = textureCube( s_CubeFaceY, v_ShadowTexCoord.xyz ).xyz;
    vec3 maxAxis = abs( v_ShadowTexCoord.xyz );
    half invMA = 1.0 / max( maxAxis.x, max( maxAxis.y, maxAxis.z ) );

    // calculate cube face X & Y.
    vec3 shadowTexCoord;
    shadowTexCoord.x = dot( axisX, v_ShadowTexCoord.xyz );
    shadowTexCoord.y = dot( axisY, v_ShadowTexCoord.xyz );
    shadowTexCoord.z = 0.5 * u_ShadowNear.z;    // ( z - 0.5 * n ) / z = ( z / z - 0.5 * n / z )
    shadowTexCoord *= invMA;

    // adjust Z to account for the near plane.
    shadowTexCoord.z += 1.0;
    shadowTexCoord.z *= 0.5 * v_ShadowTexCoord.w + 0.5;

    // dereference the proper region in the virtual shadow table.
    vec4 shadowMulAdd = textureCube( s_LightShadowVSM, v_ShadowTexCoord.xyz );
    shadowTexCoord.xy = shadowMulAdd.xy * shadowTexCoord.xy + shadowMulAdd.zw;

    //----------------------------------------------------
    // Calculate the shadow.
    //----------------------------------------------------
#if B_QUALITY < 3
    half3 jitterSTQ;
    jitterSTQ.xy = kShadowJitterTexScale.xy * gl_FragCoord.xy;
    jitterSTQ.z = 0.0;
    half center = 0.0;
    vec3 shadowCoord = vec3( 0.0, 0.0, shadowTexCoord.z );
    for ( int i = 0; i < kShadowSampleCountOver2; ++i )
    {
        half4 jitter = shadowMulAdd.xyxy * invMA * ( 2.0 * texture3D( s_NoiseTableTex3D, jitterSTQ ) - 1.0 );
        jitterSTQ.z += kShadowInvSampleCount;
        shadowCoord.xy = u_LightWidth.xx * jitter.xy + shadowTexCoord.xy;
        center += shadow2D( s_LightShadowPage, shadowCoord.xyz ).x;
        shadowCoord.xy = u_LightWidth.xx * jitter.zw + shadowTexCoord.xy;
        center += shadow2D( s_LightShadowPage, shadowCoord.xyz ).x;
    }

    // apply the shadow mask.
    light *= center * kShadowInvSampleCount;
#else
    light *= shadow2D( s_LightShadowPage, shadowTexCoord ).x;
#endif

    //----------------------------------------------------
    // Calculate the ubertexture UVs.
    //----------------------------------------------------
    half2 texCoord = v_TexCoord.xy;

    //----------------------------------------------------
    // lookup the bump and normal values.
    //----------------------------------------------------
    half bump = texture2D( s_BumpTex, texCoord ).r;
    bump = u_BumpMod.x * bump + u_BumpAdd.x;
    half2 uvOffset = bump * localEyeVec.xy;

    half3 normal = half3( 2.0 ) * texture2D( s_NormalTex, texCoord + uvOffset ).xwz - half3( 1.0 );
    normal = normalize( normal );

    //----------------------------------------------------
    // Calculate the diffuse component.
    //----------------------------------------------------
    half4 diffuse = texture2D( s_DiffuseTex, texCoord + uvOffset );
    diffuse = u_DiffuseMod * diffuse + u_DiffuseAdd;
    diffuse.rgb *= clamp( dot( localLightVec, normal ), 0.0, 1.0 );
    diffuse.rgb *= light;

    //----------------------------------------------------
    // Calculate the specular component.
    //----------------------------------------------------
    half specularDP = clamp( dot( localHalfVec, normal ), 0.0, 1.0 );
    specularDP = clamp( 4.0 * specularDP - 3.0, 0.0, 1.0 );
    specularDP = pow( specularDP, 20.0 );

    half3 specular = texture2D( s_SpecularTex, texCoord + uvOffset ).rgb;
    specular = u_SpecularMod.rgb * specular + u_SpecularAdd.rgb;
    specular *= specularDP;
    specular *= light;
    diffuse.rgb += specular;

    //----------------------------------------------------
    // Calculate light haze.
    //----------------------------------------------------
    diffuse.rgb = mix( diffuse.rgb, light.rgb, u_LightHaze.xxx );

    // output the result.
    gl_FragColor = diffuse;
}
