//----------------------------------------------------------
// File:		HDRCombine.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex outputs
varying vec2 v_CurRenderTexCoord;
varying vec2 v_BloomTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_TexCoordScale;
uniform vec4 u_CurRenderTexCoordOffset;
uniform vec4 u_BlurTexCoordOffset;
uniform vec4 u_BlurTexCoordScale;

//----------------------------------------------------------
// main
void main()
{
	gl_Position = ftransform();

	v_CurRenderTexCoord.xy = u_TexCoordScale.xy * gl_MultiTexCoord0.xy + u_CurRenderTexCoordOffset.xy;
	v_BloomTexCoord.xy = u_BlurTexCoordScale.xy * gl_MultiTexCoord0.xy + u_BlurTexCoordOffset.xy;
}
