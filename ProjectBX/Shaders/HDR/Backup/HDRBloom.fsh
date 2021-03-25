//----------------------------------------------------------
// File:		HDRBloom.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec2 v_BloomTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_BlurVec;

//----------------------------------------------------------
// samplers
uniform sampler2D s_BloomTex;

//----------------------------------------------------------
// main
void main()
{
	float weights[8] = {
		0.9394, 0.7788, 0.5697, 0.3678, 0.2096, 0.1053, 0.0467, 0.0183
	};

	// simply pass through for now.
	half4 color = texture2D( s_BloomTex, v_BloomTexCoord );
	float offset = 1.0;
	for ( int i = 0; i < 8; ++i )
	{
		color += weights[ i ] * texture2D( s_BloomTex, v_BloomTexCoord.xy + offset * u_BlurVec.xy );
		color += weights[ i ] * texture2D( s_BloomTex, v_BloomTexCoord.xy - offset * u_BlurVec.xy );
		offset += 1.0;
		color += weights[ i ] * texture2D( s_BloomTex, v_BloomTexCoord.xy + offset * u_BlurVec.xy );
		color += weights[ i ] * texture2D( s_BloomTex, v_BloomTexCoord.xy - offset * u_BlurVec.xy );
		offset += 1.0;
	}
	gl_FragColor = color * 0.141406;
}
