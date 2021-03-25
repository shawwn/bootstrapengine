//----------------------------------------------------------
// File:		HDRHiPass.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_HDRTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BloomStart;
uniform vec4 u_HalfPelOffset;

//----------------------------------------------------------
// samplers
uniform sampler2D s_HDRTex;

//----------------------------------------------------------
// main
void main()
{
	half4 color = half4( 0.0 );
	color += max( texture2D( s_HDRTex, v_HDRTexCoord + vec2( 1.0, 1.0 ) * u_HalfPelOffset.xy ) - u_BloomStart, 0.0 );
	color += max( texture2D( s_HDRTex, v_HDRTexCoord + vec2( 1.0,-1.0 ) * u_HalfPelOffset.xy ) - u_BloomStart, 0.0 );
	color += max( texture2D( s_HDRTex, v_HDRTexCoord + vec2(-1.0, 1.0 ) * u_HalfPelOffset.xy ) - u_BloomStart, 0.0 );
	color += max( texture2D( s_HDRTex, v_HDRTexCoord + vec2(-1.0,-1.0 ) * u_HalfPelOffset.xy ) - u_BloomStart, 0.0 );
	gl_FragColor = 0.25 * color;
}
