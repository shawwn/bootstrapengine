//----------------------------------------------------------
// File:		vsm.fsh
// Author:		Kevin Bray
// Created:		02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// uniforms
uniform vec4 u_IndirModAdd;

//----------------------------------------------------------
// main
void main()
{
	// output the result.
	gl_FragColor = u_IndirModAdd;
}
