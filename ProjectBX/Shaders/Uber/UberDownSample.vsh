//----------------------------------------------------------
// File:        UberDownSample.vsh
// Author:      Kevin Bray
// Created:     09-08-07
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex attributes.
attribute vec3 a_Position;
attribute vec2 a_TexCoord;

//----------------------------------------------------------
// vertex outputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// main
void main()
{
    // transform into world space.
    gl_Position = vec4( a_Position.xyz, 1.0 );

    // calculate the ubertexture lookup.
    v_TexCoord.xy = a_TexCoord.xy;
}
