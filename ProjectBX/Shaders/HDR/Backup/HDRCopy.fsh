//----------------------------------------------------------
// File:        HDRCopy.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_HDRTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BloomStart;

//----------------------------------------------------------
// samplers
uniform sampler2D s_HDRTex;

//----------------------------------------------------------
// main
void main()
{
    gl_FragColor = texture2D( s_HDRTex, v_HDRTexCoord ) - u_BloomStart;
}