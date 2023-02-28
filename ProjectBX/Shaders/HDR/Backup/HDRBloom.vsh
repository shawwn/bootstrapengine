//----------------------------------------------------------
// File:		HDRBloom.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex outputs
varying vec2 v_BloomTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_TexCoordScale;
uniform vec4 u_TexCoordOffset;

//----------------------------------------------------------
// main
void main()
{
	gl_Position = ftransform();
	v_BloomTexCoord.xy = u_TexCoordScale.xy * gl_MultiTexCoord0.xy + u_TexCoordOffset.xy;
}
