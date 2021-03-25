// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.

// project includes.
#include "huffman.h"
#include "lzw.h"
#include "lz77.h"
#include "stream.h"
#include "tga.h"

// std c includes.
#include <stdio.h>
#include <string.h>

// main.
int main( int argc, char** argv )
{
#if 0
	const char* test = "This is my test string.  Hopefully it'll compress correctly. =)\n";
	unsigned int decompressedSize = ( unsigned int )strlen( test )+1;

	// LZ77 compression test.
	{
		Stream compressed;
		LZEncode8( compressed, ( unsigned char* )test, decompressedSize, 8192 );
		unsigned int compressedSize = compressed.GetOffset();

		Stream decompressed;
		compressed.SetOffset( 0 );
		LZDecode8( decompressed, compressed );
		char* decompressedResult = ( char* )decompressed.GetBufferPtr();
	}

	// LZW compression test.
	{
		Stream compressed;
		LZWEncode8( compressed, ( unsigned char* )test, decompressedSize );
		unsigned int compressedSize = compressed.GetOffset();

		Stream decompressed;
		compressed.SetOffset( 0 );
		LZWDecode8( decompressed, compressed );
		char* decompressedResult = ( char* )decompressed.GetBufferPtr();
	}

	// Huffman compression test.
	{
		Stream compressed;
		HuffmanEncode8( compressed, ( unsigned char* )test, decompressedSize );
		unsigned int compressedSize = compressed.GetOffset();

		Stream decompressed;
		compressed.SetOffset( 0 );
		HuffmanDecode8( decompressed, compressed );
		char* decompressedResult = ( char* )decompressed.GetBufferPtr();
	}
	return 0;
#else
	if ( argc < 1 )
	{
		printf( "Specify either a compressed or uncompressed file... (jackass)\n" );
		return -1;
	}

	// get the file.
	char* file = argv[ 1 ];

	// is this a known file type?
	char* ext = strrchr( file, '.' );
	if ( !ext )
	{
		printf( "Unrecognized file type.\n" );
		return -1;
	}

	// is the file compressed?
	bool deflate = false;
	if ( _stricmp( ext, ".bci" ) == 0 )
		deflate = true;

	// load the file.
	FILE* fp = 0;
	fopen_s( &fp, file, "rb" );
	if ( !fp )
	{
		printf( "Could not open file %s.\n", file );
		return -1;
	}

	// get the size of the file.
	fpos_t size = 0;
	fseek( fp, 0, SEEK_END );
	fgetpos( fp, &size );
	fseek( fp, 0, SEEK_SET );

	// read in the entire file.
	unsigned char* buffer = new unsigned char[ ( size_t )size ];
	fread( buffer, ( size_t )size, 1, fp );
	fclose( fp );

	// now either compress or decompress.
	if ( deflate )
	{
		// remove the .bci extension.
		*ext = 0;
		fp = 0;

		// now decompress using huffman encoding.
		Stream huffmanCompressed( buffer, ( unsigned int )size, true );
		Stream lzwCompressed;
		HuffmanDecode8( lzwCompressed, huffmanCompressed );

		// now decompress using LZW encoding.
		Stream decompressed;
		lzwCompressed.SetOffset( 0 );
		LZDecode8( decompressed, lzwCompressed );

		// write out the uncompressed file.
		fopen_s( &fp, file, "wb" );
		fwrite( decompressed.GetBufferPtr(), decompressed.GetOffset(), 1, fp );
		fclose( fp );
	}
	else
	{
		// parse the image.
//		TGAImage image( buffer );
//		image.Swizzle();

		// allocate the new file name.
		unsigned int newFileNameSize = ( unsigned int )strlen( file ) + 5;
		char* newFileName = new char[ newFileNameSize ];
		strcpy_s( newFileName, newFileNameSize, file );
		strcat_s( newFileName, newFileNameSize, ".bci" );

		// open the output file.
		fp = 0;

//		unsigned int width = image.GetWidth();
//		unsigned int height = image.GetHeight();
//		unsigned int bytesPerPel = image.GetBytesPerPel();
//		unsigned int size = width * height * bytesPerPel;

		// first compress using LZW encoding.
		Stream lzwCompressed;
		LZEncode8( lzwCompressed, ( unsigned char* )buffer, ( unsigned int )size, 32766 );

		// compress the contents and save.
		Stream huffmanCompressed;
		HuffmanEncode8( huffmanCompressed, ( unsigned char* )lzwCompressed.GetBufferPtr(), lzwCompressed.GetOffset() );

//		Stream huffmanCompressed;
//		HuffmanEncode8( huffmanCompressed, ( unsigned int* )image.GetBits(), size / 4 );

		// write out the compressed file.
		fopen_s( &fp, newFileName, "wb" );
		fwrite( huffmanCompressed.GetBufferPtr(), huffmanCompressed.GetOffset(), 1, fp );
		fclose( fp );
		delete[] newFileName;
	}

	// indicate success!
	return 0;
#endif
}
