// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// forward declarations.
class Stream;

// compresses a sequence of 8-bit values using the lzw algorithm.
extern void LZWEncode8( Stream& output, const unsigned char* data, unsigned int count );
extern void LZWDecode8( Stream& decompressed, Stream& compressed );

// compresses a sequence of 32-bit values using the lzw algorithm.
extern void LZWEncode32( Stream& output, const unsigned int* data, unsigned int count );
extern void LZWDecode32( Stream& decompressed, Stream& compressed );
