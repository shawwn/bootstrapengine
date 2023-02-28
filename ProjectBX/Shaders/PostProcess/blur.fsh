//----------------------------------------------------------
// File:		postrender/test.fsh
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
// main
void main()
{
	vec4 color =	texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0, 0.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 0.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2(-1.0, 0.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0, 1.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 0.0,-1.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0, 1.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2(-1.0, 1.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2( 1.0,-1.0 ) );
	color +=		texture2DRect( s_CurRenderTex, gl_FragCoord.xy + vec2(-1.0,-1.0 ) );
	color /= 9.0;
	gl_FragColor = color;
}
