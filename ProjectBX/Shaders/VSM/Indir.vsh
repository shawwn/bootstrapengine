//----------------------------------------------------------
// File:        depth.vsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex attributes.
attribute vec3 a_Position;

//----------------------------------------------------------
// main
void main()
{
    gl_Position = vec4( a_Position, 1.0 );
}
