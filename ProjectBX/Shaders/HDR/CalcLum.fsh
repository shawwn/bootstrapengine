//----------------------------------------------------------
// File:		CalcLum.fsh
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
	// get the HDR color and simply pass it through.
	vec4 hdrColor = texture2DRect( s_CurrentRender, v_TexCoord.xy );
	vec3 kLum = vec3( 0.2125, 0.7154, 0.0721 );
	float lum = dot( kLum, hdrColor.rgb );

	// take the log of the lum so we can calculate the average
	// in log space.
//	lum = log( lum + 0.0001 );
//	lum = log( lum + 0.0001 ) / log( 10.0 );

	// scale the color by the average luminance.
	gl_FragColor = vec4( lum );
}
