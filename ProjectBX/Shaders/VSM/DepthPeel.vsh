//----------------------------------------------------------
// File:        depthpeel.vsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex attributes.
attribute vec3 a_Position;

//----------------------------------------------------------
// vertex outputs
varying vec4 v_TopLayerTexCoord;

//----------------------------------------------------------
// uniforms
uniform mat4 u_ModelViewProjectionMatrix;

//----------------------------------------------------------
// main
void main()
{
    gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position, 1.0 );

    // calculate the top layer lookup.
    v_TopLayerTexCoord.x = 0.5 * gl_Position.x + 0.5 * gl_Position.w;
    v_TopLayerTexCoord.y = 0.5 * gl_Position.y + 0.5 * gl_Position.w;
    v_TopLayerTexCoord.z = gl_Position.z;
    v_TopLayerTexCoord.w = gl_Position.w;
}
