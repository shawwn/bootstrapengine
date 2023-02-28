//----------------------------------------------------------
// File:        depthmerge.vsh
// Author:      Kevin Bray
// Created:     02-07-06
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
    // calculate the output position.
    gl_Position = vec4( a_Position, 1.0 );

    // set the output texture coordinate.
    v_TexCoord = a_TexCoord.xy;
}
