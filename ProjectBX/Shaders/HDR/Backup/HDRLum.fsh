//----------------------------------------------------------
// File:		HDRLum.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_PelOffset;

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
	half4 color = half4( 0.0, 0.0, 0.0, 0.0 );
	color = texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 0 ] * u_PelOffset.xy );
	logLumSum += log( dot( color.rgb, half3( 0.3, 0.59, 0.11 ) ) + 0.0001 );
	color = texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 1 ] * u_PelOffset.xy );
	logLumSum += log( dot( color.rgb, half3( 0.3, 0.59, 0.11 ) ) + 0.0001 );
	color = texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 2 ] * u_PelOffset.xy );
	logLumSum += log( dot( color.rgb, half3( 0.3, 0.59, 0.11 ) ) + 0.0001 );
	color = texture2D( s_CurrentRender, v_TexCoord.xy + offsets[ 3 ] * u_PelOffset.xy );
	logLumSum += log( dot( color.rgb, half3( 0.3, 0.59, 0.11 ) ) + 0.0001 );

	gl_FragColor = vec4( logLumSum.xxx * 0.25, 1.0 );
}
