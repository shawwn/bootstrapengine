//----------------------------------------------------------
// File:		PassThru.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : enable

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BlurVec;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_CurrentRender;

//----------------------------------------------------------
// main
void main()
{
	float weights[5];
	weights[0] = 70.0 / 256.0;
	weights[1] = 56.0 / 256.0;
	weights[2] = 28.0 / 256.0;
	weights[3] =  8.0 / 256.0;
	weights[4] =  1.0 / 256.0;

	// simply pass through for now.
	half4 color = weights[0] * texture2DRect( s_CurrentRender, v_TexCoord );
	float offset = 1.5;
	for ( int i = 1; i < 5; ++i )
	{
		color += 0.5 * weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy + offset * u_BlurVec.xy );
		color += 0.5 * weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy - offset * u_BlurVec.xy );
		offset += 2.0;
		color += 0.5 * weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy + offset * u_BlurVec.xy );
		color += 0.5 * weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy - offset * u_BlurVec.xy );
		offset += 2.0;

//		color += weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy + offset * u_BlurVec.xy );
//		color += weights[ i ] * texture2DRect( s_CurrentRender, v_TexCoord.xy - offset * u_BlurVec.xy );
//		offset += 1.0;
	}

	// simply return the texture lookup result.
	gl_FragColor = color;
}
