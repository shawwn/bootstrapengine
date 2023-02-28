//----------------------------------------------------------
// File:		BrightPass.fsh
// Author:		Kevin Bray
// Created:		02-07-06
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
	// get the HDR color.
	vec4 hdrColor;

	hdrColor =  texture2DRect( s_CurrentRender, v_TexCoord.xy );
	hdrColor += texture2DRect( s_CurrentRender, v_TexCoord.xy + vec2(-2.0, 0.0 ) );
	hdrColor += texture2DRect( s_CurrentRender, v_TexCoord.xy + vec2( 2.0, 0.0 ) );
	hdrColor += texture2DRect( s_CurrentRender, v_TexCoord.xy + vec2( 0.0,-2.0 ) );
	hdrColor += texture2DRect( s_CurrentRender, v_TexCoord.xy + vec2( 0.0, 2.0 ) );
	hdrColor *= 0.2;

	// calculate the luminance term that we'll use for bloom.
	vec3 kLum = vec3( 0.2125, 0.7154, 0.0721 );
	float localLum = dot( kLum, hdrColor.rgb );
	float adjustedLum = 2.0 * max( localLum - 0.6, 0.0 );

	// adjust the luminance of the filtered colors.
	hdrColor.rgb *= adjustedLum;

	// simply return the adjusted texture lookup.
	gl_FragColor = hdrColor;
}
