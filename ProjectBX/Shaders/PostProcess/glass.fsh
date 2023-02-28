//----------------------------------------------------------
// File:        postrender/test.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : enable

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_CurRenderTex;
uniform sampler2DRect s_CurDepthTex;

//----------------------------------------------------------
// main
void main()
{
    // calculate the lookup from the current render texture.
    vec2 screenUV;



    vec4 color = texture2DRect( s_CurRenderTex, gl_FragCoord.xy );
    color += texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 0.0 ) );
    color += texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0, 1.0 ) );
    color += texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 1.0 ) );
    color *= 0.25;
    gl_FragColor = color;
/*
    // now perform the lookup and tint it blue.
    vec4 color;
    float offset = 0.0;
    float scale = 1.0 / 16.0;
    for ( int i = 0; i < 4; ++i )
    {
        color += scale * texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0, offset ) );
        color += scale * texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, offset ) );
        color += scale * texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 2.0, offset ) );
        color += scale * texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 3.0, offset ) );
        offset += 1.0;
    }

    // now store the resulting color.
    gl_FragColor = color;
*/
}
