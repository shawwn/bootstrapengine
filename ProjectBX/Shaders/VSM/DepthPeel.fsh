//----------------------------------------------------------
// File:        depthpeel.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------

//----------------------------------------------------------
// vertex inputs
varying vec4 v_TopLayerTexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2D s_TopLayer;

//----------------------------------------------------------
// main
void main()
{
    // lookup the top layer's depth value.
    half topLayerZ = texture2DProj( s_TopLayer, v_TopLayerTexCoord ).r;

    // if the current fragment is in front of the biased layer 0 value,
    // then the fragment must be on layer 0.
    if ( gl_FragCoord.z <= topLayerZ )
        discard;

    // output the result.
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
}
