//----------------------------------------------------------
// File:        PassThru.fsh
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
uniform sampler2DRect s_CurrentRender;

//----------------------------------------------------------
// main
void main()
{
    // simply return the texture lookup result.
    gl_FragColor = texture2DRect( s_CurrentRender, v_TexCoord.xy );
}
