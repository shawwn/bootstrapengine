//----------------------------------------------------------
// File:		postrender/alphasort.fsh
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
uniform sampler2DRect s_CurRenderTex;
uniform sampler2DRect s_CurDepthTex;

//----------------------------------------------------------
// functions.

// swaps if depth1 is less then depth0.
void sortStep( inout vec4 color0, inout vec4 color1, inout float depth0, inout float depth1 )
{
	// swap if necessary.
	vec4 temp;
	if ( depth1 < depth0 )
	{
		temp = color0;
		color0 = color1;
		color1 = temp;

		temp.a = depth0;
		depth0 = depth1;
		depth1 = temp.a;
	}
}

//----------------------------------------------------------
// main
void main()
{
	// lookup four color values in a 2x2 grid pattern.
	vec4 color0 = texture2DRect( s_CurRenderTex, gl_FragCoord.xy );
	vec4 color1 = texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 0.0 ) );
	vec4 color2 = texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0, 1.0 ) );
	vec4 color3 = texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 1.0 ) );

	// lookup four depth values in a 2x2 grid pattern.
	float depth0 = texture2DRect( s_CurDepthTex, gl_FragCoord.xy ).r;
	float depth1 = texture2DRect( s_CurDepthTex, gl_FragCoord.xy + vec2( 1.0, 0.0 ) ).r;
	float depth2 = texture2DRect( s_CurDepthTex, gl_FragCoord.xy + vec2( 0.0, 1.0 ) ).r;
	float depth3 = texture2DRect( s_CurDepthTex, gl_FragCoord.xy + vec2( 1.0, 1.0 ) ).r;

	// sort pixels using a bubble sort.
	// first iteration.
	sortStep( color0, color1, depth0, depth1 );
	sortStep( color1, color2, depth1, depth2 );
	sortStep( color2, color3, depth2, depth3 );

	// second iteration.
	sortStep( color0, color1, depth0, depth1 );
	sortStep( color1, color2, depth1, depth2 );

	// third iteration.
	sortStep( color0, color1, depth0, depth1 );

	// perform the alpha blend.
	vec4 finalColor = color3;
	finalColor = ( 1.0 - color2.a ) * finalColor + color2;
	finalColor = ( 1.0 - color1.a ) * finalColor + color1;
	finalColor = ( 1.0 - color0.a ) * finalColor + color0;

	// store the result.
	gl_FragColor = finalColor;
}
