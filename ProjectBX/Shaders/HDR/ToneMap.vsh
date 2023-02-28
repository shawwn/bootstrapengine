//----------------------------------------------------------
// File:		ToneMap.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex attributes
attribute vec4 a_Position;
attribute vec2 a_TexCoord;

//----------------------------------------------------------
// vertex outputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_TexCoordScale;

//----------------------------------------------------------
// main
void main()
{
	gl_Position = vec4( a_Position.xyz, 1.0 );
	v_TexCoord.xy = u_TexCoordScale.xy * a_TexCoord.xy;
}
