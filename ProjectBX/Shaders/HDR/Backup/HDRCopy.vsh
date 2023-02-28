//----------------------------------------------------------
// File:		HDRCopy.vsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex outputs
varying vec2 v_HDRTexCoord;

//----------------------------------------------------------
// uniforms
uniform vec4 u_HDRTexGenS;
uniform vec4 u_HDRTexGenT;

//----------------------------------------------------------
// main
void main()
{
	gl_Position = ftransform();
	v_HDRTexCoord.x = 0.5 * gl_Vertex.x + 0.5; //dot( gl_Vertex.x, u_HDRTexGenS );
	v_HDRTexCoord.y = 0.5 * gl_Vertex.y + 0.5; //dot( gl_Vertex, u_HDRTexGenT );
}
