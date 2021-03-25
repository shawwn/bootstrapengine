//----------------------------------------------------------
// File:		Uber.fsh
// Author:		Kevin Bray
// Created:		09-08-07
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : require

#define DOWNSAMPLE_SIZE		4

//----------------------------------------------------------
// vertex inputs
varying vec2 v_TexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_PrevLevel;

//----------------------------------------------------------
// main
void main()
{
	// temporary value for colors.
	vec4 temp;
	vec4 current;
	vec2 adj = vec2( 0.0, 0.0 );

	// iterate through pixel values and determine if all of them
	// are the same.
	current = texture2DRect( s_PrevLevel, v_TexCoord );

	// determine if all elements are the same.  If mask is set to 0.0
	// when these loops get done executing, then all pixel values are
	// equal.
	float anyDifferent = 0.0;
	for ( int y = 0; y < DOWNSAMPLE_SIZE; ++y )
	{
		for ( int x = 0; x < DOWNSAMPLE_SIZE; ++x )
		{
			temp = texture2DRect( s_PrevLevel, v_TexCoord + adj );
			if ( any( notEqual( current, temp ) ) )
				anyDifferent = 1.0;
			adj.x += 1.0;
		}
		adj.y += 1.0;
		adj.x = 0.0;
	}

	// note that we output 2.0234375, which is 2 + 12 / 2048.  We do this
	// as a conversion to get 12 in the engine, which is too large to be
	// a valid mip level (max is 11).  This is used to indicate the case
	// where a block of pixels have different values than the source.
	gl_FragColor = mix( current, vec4( -1.0, -1.0, 2.0234375, -1.0 ), anyDifferent );
}

/*
Note:
	0.05078125 is more than 12 in fixed point, and is impossible given
	uber-texture resolution limits.  We use this value to indicate that
	values in a block are not equal.
*/
