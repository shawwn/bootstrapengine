//----------------------------------------------------------
// File:		UberHash.fsh
// Author:		Kevin Bray
// Created:		09-08-07
// Copyright © 2004 Bootstrap Games.  All rights reserved.
//----------------------------------------------------------
#extension GL_ARB_texture_rectangle : require

//----------------------------------------------------------
// vertex inputs
varying vec2 v_SourceTexCoord;

//----------------------------------------------------------
// samplers
uniform sampler2DRect s_TileSource;
uniform sampler2DRect s_HashTable;
uniform sampler2DRect s_HashMissTable;

//----------------------------------------------------------
// uniform inputs
uniform vec4 u_HashTableSize;
uniform vec4 u_HashKeyPrimes;

#define MAX_HASH_WALK_LEN			8
#define MAX_MIP_LEVEL				6

//----------------------------------------------------------
// main
void main()
{
	// lookup the current tile value.
	vec4 encodedTile = texture2DRect( s_TileSource, v_SourceTexCoord );

	// calculate the scale and bias.
	vec4 offset = 1024.0 * step( 1.0, encodedTile );
	vec4 base = 0.5 * step( 1.0, encodedTile ) + 0.5;
	vec4 scale = vec4( 2048.0 ) - offset;

	// decode the tile information.
	vec4 tile = scale * fract( encodedTile - base ) + offset;

	// initialize the result with the hash miss value.
	vec4 result = texture2DRect( s_HashMissTable, vec2( tile.w, 0.0 ) );

	// iterate through the hash table.
	bool miss = false;
	bool hit = false;
	bool end = false;
	for ( int fallback = 0; fallback < MAX_MIP_LEVEL; ++fallback )
	{
		if ( !hit )
		{
			// calculate the tile's hash key.
			float key = dot( tile, u_HashKeyPrimes );

			// build the hash lookup vector.
			vec2 hashIdx;
			hashIdx.x = 0.0;
			hashIdx.y = mod( key, u_HashTableSize.x );

			// iterate through the current hash slot, searching for the
			// current tile.
			for ( int i = 0; i < MAX_HASH_WALK_LEN; ++i )
			{
				// perform the hash table lookup.
				vec4 hashedTile = texture2DRect( s_HashTable, hashIdx );
				hashIdx.x += 1.0;
				vec4 hashedIndir = texture2DRect( s_HashTable, hashIdx );
				hashIdx.x += 1.0;

				// check to see if we've hit the end of the hash entries.
				if ( hashedIndir.x == 0.0 )
					miss = true;

				// compare the tile in the hash table to the current tile.
				if ( !miss && !hit && all( equal( tile, hashedTile ) ) )
				{
					result = hashedIndir;
					hit = true;
				}
			}

			// this changes the tile to a mip-level fallback.
			tile.xy = floor( 0.5 * tile.xy );
			tile.z += 1.0;
			miss = false;
		}
	}

	// output the result.
	gl_FragColor = result;
}

	//
	//	vec4 i = floor( 0.0009765625 * x );							// 0 if (x < 1024), 1 if (1024 <= x < 2048)
	//	vec4 f = vec4( 0.5, 0.5, 0.5, 0.5 ) + 0.00048828125 * x;	
	//	vec4 r = i * f - i + f;
	// 
	//	if x <  1024, then i = 0 and r = 0 * (0.5 + x / 2048 ) - 0 + (0.5 + x / 2048 )
	//	if x >= 1024, then i = 1 and r = 1 * (0.5 + x / 2048 ) - 1 + (0.5 + x / 2048 )
	// 
	//  x = 1023
	//  r = 0.5 + 1023 / 2048
	//
	//	if 0 <= x < 1024, then 0.5 <= r < 1.0
	//
	//
	//  x = 1024
	//  r = 0.5 + 1024 / 2048 - 1 + (0.5 + 1024 / 2048)
	//  r = 0.5 + 0.5 - 1 + 0.5 + 0.5 = 1
	//
	//	x = 2047
	//	r = 0.5 + 2047 / 2048 - 1 + (0.5 + 2047 / 2048)
	//	r = 0.5 + 0.999 - 1 + 0.5 + 0.999
	//	r = 1.499 - 1 + 1.499
	//  r = 0.499 + 1.499
	//
	//  if 1024 <= x < 2048, 1.0 <= r < 2.0

	// decode the current value.
	//	if r < 1.0
	//		0    + 2 * fract( r - 0.5 ) * 1024
	//	if r >= 1.0
	//		1024 + 1 * fract( r - 1.0 ) * 1024
	//
	//	if r < 1.0
	//		0    + 2048 * fract( r - 0.5 )
	//	if r >= 1.0
	//		1024 + 1024 * fract( r - 1.0 )
	//
	//	three conditionals...
	//		offset + scale * fract( r - base )
	//
	//	offset = 0 or 1024		= r < 1.0 ?	   0.0	: 1024.0
	//	scale = 2048 or 1024		= r < 1.0 ?	2048.0	: 1024.0
	//	base = 0.5 or 1.0		= r < 1.0 ?    0.5  : 1.0
	//
	//also:
	//	offset = 0 or 1024		= r < 1.0 ?	   0.0	: 1024.0
	//	scale = 2048 - offset
	//	base = 0.5 or 1.0		= r < 1.0 ?    0.5  : 1.0
	//
	//	down to two conditionals!  yay!
	//		offset + ( 2048 - offset ) * fract( r - base )
	//
