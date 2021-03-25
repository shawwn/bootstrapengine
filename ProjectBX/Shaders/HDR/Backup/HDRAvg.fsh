//----------------------------------------------------------
// File:		HDRAvg.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// uniforms
uniform vec4 u_PelOffset;

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2D s_CurrentRender;

//----------------------------------------------------------
// main
void main()
{
	const vec2 offsets[4] =
	{
		vec2( -0.5, -0.5 ),
		vec2( -0.5,  0.5 ),
		vec2(  0.5, -0.5 ),
		vec2(  0.5,  0.5 ),
	};

	half logLumSum = 0.0;
	half color = 0.0;
	color += texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 0 ] * u_PelOffset.xy ).r;
	color += texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 1 ] * u_PelOffset.xy ).r;
	color += texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 2 ] * u_PelOffset.xy ).r;
	color += texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 3 ] * u_PelOffset.xy ).r;
	gl_FragColor = vec4( color.rrr * 0.25, 1.0 );
}
