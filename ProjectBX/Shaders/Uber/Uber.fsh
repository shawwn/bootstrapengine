//----------------------------------------------------------
// File:        Uber.fsh
// Author:      Kevin Bray
// Created:     09-08-07
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_UberTexCoord;

//----------------------------------------------------------
// uniform inputs
uniform vec4 u_TextureSize;
uniform vec4 u_TextureID;
uniform vec4 u_MaxMipLevel;
uniform vec4 u_Scale;

//----------------------------------------------------------
// main
void main()
{
    // calculate the derivatives in texels.
    vec2 dudv;
    dudv = fwidth( v_UberTexCoord.xy );
    dudv *= u_TextureSize.xy;

    // take the maximum derivative in texels.
    float width = u_Scale.x * max( dudv.x, dudv.y );

    // calculate the mip level.  Note that we take the *worst* miplevel
    // to try and save on bandwidth.
    float mipLevel = floor( log2( width ) + 0.5 );
    mipLevel = clamp( mipLevel, 0.0, u_MaxMipLevel.x );

    // calculate the mip scale so that we can calculate the tiles needed.
    float mipScale = pow( 0.5, mipLevel );
    vec2 mipDimInTiles = max( mipScale * u_TextureSize.zw, vec2( 1.0, 1.0 ) );

    // calculate the tiles needed.
    vec2 tiles = floor( v_UberTexCoord * mipDimInTiles );
    
    // pack everything together.
    vec4 packedResult;
    packedResult.rg = tiles;
    packedResult.b = mipLevel;
    packedResult.a = u_TextureID.a;

    // encode the final value and store the result.
    vec4 i = floor( 0.0009765625 * packedResult );
    vec4 f = vec4( 0.5, 0.5, 0.5, 0.5 ) + 0.00048828125 * packedResult;
    gl_FragColor = i * f - i + f;
}
