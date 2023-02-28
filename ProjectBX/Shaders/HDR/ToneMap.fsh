//----------------------------------------------------------
// File:        ToneMap.fsh
// Author:      Kevin Bray
// Created:     02-07-06
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : enable

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_CurrentRender;
uniform sampler2DRect s_CurrentLum;

//----------------------------------------------------------
// main
void main()
{
    // get the HDR color and simply pass it through.
    vec4 hdrColor = texture2DRect( s_CurrentRender, v_TexCoord.xy );

    // calculate the luminance level for the entire image.
    float lumAvg = texture2DRect( s_CurrentLum, vec2( 0.5, 0.5 ) ).r;
//  lumAvg = exp( lumAvg );
//  lumAvg = pow( 10, lumAvg );

    // use the luminance of the current pixel to "blue shift".
//  float localLum = dot( hdrColor.rgb, vec3( 0.2125, 0.7154, 0.0721 ) );   // lum
//  float blueShift = 0.4 * clamp( 0.8 - 8.2 * localLum, 0.0, 1.0 ); //clamp( 1.0 - ( lum + 1.5 ) / 4.1, 0.0, 1.0 );
//  vec3 rodColor = dot( hdrColor.rgb, vec3( 0.2125, 0.7154, 0.0721 ) ) * vec3( 1.05, 0.97, 1.27 );
//  hdrColor.rgb = mix( hdrColor.rgb, rodColor, blueShift );

    // this is the "middle grey" key value.  It is essentially an exposure
    // setting.
    float middleGrey = 1.0;

    // calculate the tone mapped color.
    hdrColor.rgb *= middleGrey / ( lumAvg + 1.0 );

    // write the final pixel color.
//  if ( gl_FragCoord.x > 512 && gl_FragCoord.y > 512 )
//      gl_FragColor = vec4( lumAvg, lumAvg, lumAvg, lumAvg );
//  else
    gl_FragColor = hdrColor;
}
