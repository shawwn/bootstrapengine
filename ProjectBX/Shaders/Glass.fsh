//----------------------------------------------------------
// File:        glass.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

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
const float kShadowInvSampleCount = 1.0 / float( kShadowSampleCount );
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
uniform sampler2D s_AtlasTex;
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

    // calculate the fresnel effect (dot product with the localEyeVec and
    // the tangent space geometric normal vec3(0, 0, 1) ).
    half fresnel = clamp( localEyeVec.z, 0.0, 1.0 );
    fresnel = 1.0 - fresnel;

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
    light *= texture2DProj( s_LightProjTex, v_ProjTexCoord ).rgb;
    light = v_ProjTexCoord.z <= 0.0 ? vec3( 0.0, 0.0, 0.0 ) : light;

    // calculate attenuation and scale the light color accordingly.
    half linAtten = clamp( -localLightDist * u_LightRange.w + 1.0, 0.0, 1.0 );  // 1-(d/r)
    light *= dot( vec4( linAtten * linAtten, linAtten, linAtten, 1.0 ), u_LightAtten );

    //----------------------------------------------------
    // Calculate the shadow texture coordinate.
    //----------------------------------------------------
    vec3 axisX = textureCube( s_CubeFaceX, v_ShadowTexCoord.xyz ).xyz;
    vec3 axisY = textureCube( s_CubeFaceY, v_ShadowTexCoord.xyz ).xyz;
    vec3 maxAxis = abs( v_ShadowTexCoord.xyz );
    float invMA = 1.0 / max( maxAxis.x, max( maxAxis.y, maxAxis.z ) );

    vec4 shadowTexCoord;
    shadowTexCoord.x = dot( axisX, v_ShadowTexCoord.xyz ) * invMA;
    shadowTexCoord.y = dot( axisY, v_ShadowTexCoord.xyz ) * invMA;
    shadowTexCoord.z = 1.0 + 0.5 * u_ShadowNear.z * invMA;  // ( z - 0.5 * n ) / z = ( z / z - 0.5 * n / z )
    shadowTexCoord.z *= 0.5 * v_ShadowTexCoord.w + 0.5;
    shadowTexCoord.w = 1.0;

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
    vec4 shadowCoord = vec4( 0.0, 0.0, shadowTexCoord.z, shadowTexCoord.w );
    for ( int i = 0; i < kShadowSampleCountOver2; ++i )
    {
        half4 jitter = shadowMulAdd.xyxy * invMA * ( 2.0 * texture3D( s_NoiseTableTex3D, jitterSTQ ) - 1.0 );
        jitterSTQ.z += kShadowInvSampleCount;
        shadowCoord.xy = u_LightWidth.xx * jitter.xy + shadowTexCoord.xy;
        center += shadow2DProj( s_LightShadowPage, shadowCoord ).x;
        shadowCoord.xy = u_LightWidth.xx * jitter.zw + shadowTexCoord.xy;
        center += shadow2DProj( s_LightShadowPage, shadowCoord ).x;
    }

    // apply the shadow mask.
    light *= center * kShadowInvSampleCount;
#else
    light *= shadow2DProj( s_LightShadowPage, shadowTexCoord ).x;
#endif

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

    half3 normal = 2.0 * texture2D( s_NormalTex, texCoord + uvOffset ).xwz - 1.0;
    normal = normalize( normal );

    //----------------------------------------------------
    // Calculate the diffuse component.
    //----------------------------------------------------
    half4 diffuse = texture2D( s_DiffuseTex, texCoord + uvOffset );
    diffuse = u_DiffuseMod * diffuse + u_DiffuseAdd;
    diffuse.rgb *= clamp( dot( localLightVec, normal ), 0.0, 1.0 );
    diffuse.rgb *= light;

    // calculate alpha to include the fresnel term and premultiply.
    diffuse.a += 0.7 * fresnel;
    diffuse.rgb *= diffuse.a;

    //----------------------------------------------------
    // Calculate the specular component.
    //----------------------------------------------------
    half specularDP = clamp( dot( localHalfVec, normal ), 0.0, 1.0 );
    specularDP = clamp( 4.0 * specularDP - 3.0, 0.0, 1.0 );
    specularDP = pow( specularDP, 20.0 );
    specularDP *= 2.0 * fresnel + 0.5;

    // incorperate the specular color.
    half3 specular = texture2D( s_SpecularTex, texCoord + uvOffset ).rgb;
    specular = u_SpecularMod.rgb * specular + u_SpecularAdd.rgb;
    specular *= specularDP;
    specular *= light;

    // add in specular
    diffuse.rgb += specular;

    // return the lit color.
    gl_FragColor = diffuse;
}
