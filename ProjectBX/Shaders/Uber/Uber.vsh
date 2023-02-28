//----------------------------------------------------------
// File:        Uber.vsh
// Author:      Kevin Bray
// Created:     09-08-07
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//==========================================================
// vertex attributes.
//==========================================================
attribute vec3 a_Position;
attribute vec2 a_TexCoord;

//----------------------------------------------------------
// vertex outputs
varying vec2 v_UberTexCoord;

//----------------------------------------------------------
// uniforms
uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4x2 u_TextureMatrix;

//----------------------------------------------------------
// main
void main()
{
    // calculate the outgoing position.
    gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );

    // calculate the ubertexture lookup.
    v_UberTexCoord = u_TextureMatrix * vec4( a_TexCoord.xy, 0.0, 1.0 );
}
