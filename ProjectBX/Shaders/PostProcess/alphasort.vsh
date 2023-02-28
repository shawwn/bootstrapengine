//----------------------------------------------------------
// File:		postrender/test.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

invariant gl_Position;

//----------------------------------------------------------
// vertex attributes.
attribute vec3 a_Position;

//==========================================================
// vertex outputs.
//==========================================================
varying vec2 v_TexCoord;

//==========================================================
// uniforms.
//==========================================================
uniform mat4 u_ModelViewProjectionMatrix;

//==========================================================
// main.
//==========================================================
void main()
{
	gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );
}
