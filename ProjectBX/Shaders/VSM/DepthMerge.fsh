//----------------------------------------------------------
// File:        depthmerge.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2D s_Layer0;
uniform sampler2D s_Layer1;

//----------------------------------------------------------
// main
void main()
{
    // lookup the top layer's depth value.
    half layer0 = texture2D( s_Layer0, v_TexCoord ).r;
    half layer1 = texture2D( s_Layer1, v_TexCoord ).r;

    // output the result.
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = 0.5 * layer0 + 0.5 * layer1;
}
