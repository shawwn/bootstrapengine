//----------------------------------------------------------
// File:		HDRTone.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2D s_CurrentRender;
uniform sampler2D s_LocalLum;

//----------------------------------------------------------
// main
void main()
{
	half4 hdrColor = texture2D( s_CurrentRender, v_TexCoord.xy );
	float avgLum = exp( texture2D( s_LocalLum, vec2( 0.0, 0.0 ) ).r );

	// perform the blue shift.
	half blueShiftT = clamp( 1.0 - ( avgLum + 1.5 ) / 4.1, 0.0, 1.0 );
	half3 blueShiftColor = dot( hdrColor.rgb, half3( 0.3, 0.59, 0.11 ) ) * half3( 1.05, 0.97, 1.27 );
    hdrColor.rgb = mix( hdrColor.rgb, blueShiftColor, blueShiftT );

	// calculate the tone mapped color.
	float scale = 1.0 / ( avgLum + 0.2 );
	hdrColor *= scale;
	hdrColor = hdrColor / ( hdrColor + 1.0 );

	// scale the color by the average luminance.
	gl_FragColor = hdrColor;
}
