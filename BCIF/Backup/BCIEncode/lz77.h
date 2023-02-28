// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// forward declarations.
class Stream;

// compresses a sequence of 8-bit values using the lz77 algorithm.
extern void LZEncode8( Stream& output, const unsigned char* data, unsigned int count, unsigned int bufSize );
extern void LZDecode8( Stream& output, Stream& input );

extern void LZEncode32( Stream& output, const unsigned int* data, unsigned int count, unsigned int bufSize );
extern void LZDecode32( Stream& output, Stream& input );
