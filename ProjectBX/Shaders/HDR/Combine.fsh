//----------------------------------------------------------
// File:		PassThru.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : enable

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord0;
varying vec2 v_TexCoord1;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_CurrentRender;
uniform sampler2DRect s_CurrentBloom;

//----------------------------------------------------------
// main
void main()
{
	// simply return the texture lookup result.
	vec4 hdrColor = texture2DRect( s_CurrentRender, v_TexCoord0.xy );
	vec4 bloomColor = texture2DRect( s_CurrentBloom, v_TexCoord1.xy );

	// add the bloom.
	hdrColor.rgb += bloomColor.rgb;

	// return the final color.
	gl_FragColor = hdrColor;
}
