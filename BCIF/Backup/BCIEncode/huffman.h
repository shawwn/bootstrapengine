// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// forward declarations.
class Stream;

// compresses a sequence of 8-bit values using the huffman algorithm.
extern void HuffmanEncode8( Stream& output, const unsigned char* data, unsigned int count );
extern void HuffmanDecode8( Stream& decompressed, Stream& compressed );

// compresses a series of 16-bit values using the huffman algorithm.
extern void HuffmanEncode16( Stream& output, const unsigned short* data, unsigned int count );
extern void HuffmanDecode16( Stream& decompressed, Stream& compressed );

// compresses a series of 32-bit values using the huffman algorithm.
extern void HuffmanEncode32( Stream& output, const unsigned int* data, unsigned int count );
extern void HuffmanDecode32( Stream& decompressed, Stream& compressed );
