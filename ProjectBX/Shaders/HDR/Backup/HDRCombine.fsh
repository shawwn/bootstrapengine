//----------------------------------------------------------
// File:        HDRCombine.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_CurRenderTexCoord;
varying vec2 v_BloomTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BloomStrength;

//----------------------------------------------------------
// samplers
uniform sampler2D s_CurrentRender;
uniform sampler2D s_BloomTex;

//----------------------------------------------------------
// main
void main()
{
    // combine the original image with the bloom.
    vec4 color = texture2D( s_CurrentRender, v_CurRenderTexCoord.xy );

    // tone map.
//  color.rgb /= ( color.rgb + 1.0 );

    // add in the bloom amount.
    color.rgb += u_BloomStrength.rgb * texture2D( s_BloomTex, v_BloomTexCoord.xy ).rgb;

    // write to the output register.
    gl_FragColor = color;
}
