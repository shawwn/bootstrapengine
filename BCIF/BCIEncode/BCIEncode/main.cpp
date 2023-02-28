// main.cpp
// Copyright (C) 2007, Bootstrap Studios.
//
// For use, please see the accompanying readme.
//
#include "..\zlib\zlib.h"
#include "bci.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <algorithm>

static void ParseArgs( int argc, char** argv );
static void ShowHelp();

// global arguments.
static int quality = 100;
static char* srcFile = 0;
static bool normalMap = false;
static bool optimize = false;

int main( int argc, char** argv )
{
	if ( argc < 1 )
	{
		printf( "Invalid file specified!\n  Usage: bciencode filename\n" );
		return -1;
	}

	// parse arguments.
	ParseArgs( argc, argv );

	// copy the file name.
	size_t fileNameLen = strlen( srcFile ) + 5;
	char* fileName = new char[ fileNameLen ];
	strcpy_s( fileName, fileNameLen, srcFile );

	// open the source file.
	FILE* fp = 0;
	fopen_s( &fp, fileName, "rb" );
	if ( !fp )
	{
		printf( "File not found!\n" );
		return -1;
	}

	// get the file size.
	fseek( fp, 0, SEEK_END );
	unsigned long size = ftell( fp );
	rewind( fp );

	// determine what to do with the file.
	char* ext = strrchr( fileName, '.' );
	bool decode = ( _stricmp( ext, ".bci" ) == 0 );

	// are we decoding the file?
	if ( decode )
	{
		// clear the .bci extension.
		*ext = '\0';

		// read in the entire file.
		void* data = new unsigned char[ ( size_t )size ];
		fread( data, ( size_t )size, 1, fp );
		fclose( fp );

		// decode the file.
		unsigned long dstSize = *( unsigned int* )data;
		void* dstBuf = new unsigned char[ dstSize ];
		uncompress( ( unsigned char* )dstBuf, &dstSize, ( const unsigned char* )data + 4, ( unsigned long )size );
		delete[] data;

		// write out the file.
		fopen_s( &fp, fileName, "wb" );
		fwrite( dstBuf, dstSize, 1, fp );
		fclose( fp );
	}
	else if ( !optimize )
	{
		// open the input PNG file.
		PNG_src_stream_t png_stream;
		InitPNGStream( &png_stream, fp );

		// append the file extension and open the output file.
		strcat_s( fileName, fileNameLen, ".zip" );
		FILE* fpOut = 0;
		fopen_s( &fpOut, fileName, "w+b" );

		// write our BCI file.
		WriteBCIImage( fpOut, &png_stream, quality, normalMap );

		// close our files.
		fclose( fpOut );
		fclose( fp );
	}
	else
	{
		// optimize!
		RepackBCIImage( fp );
		fclose( fp );
	}

	// free the source data.
	delete[] fileName;

	return 0;
};

void ParseArgs( int argc, char** argv )
{
	bool error = false;
	int tok = 1;
	while ( tok < argc )
	{
		// get the current argument.
		char* curArg = argv[ tok ];
		if ( *curArg != '-' && *curArg != '/' )
		{
			// was the file already set?
			if ( srcFile != 0 )
			{
				printf( "Source file already supplied...\n" );
				error = true;
			}
			else
				srcFile = curArg;

			// next token.
			++tok;
			continue;
		}

		// advance past the flag character.
		++curArg;

		// check to see what kind of argument we're dealing with.
		if ( _stricmp( curArg, "quality" ) == 0 )
		{
			// get the next token.
			++tok;
			if ( tok >= argc || !isalnum( *argv[ tok ] ) )
			{
				printf( "No quality level specified!  Should be 0-100...\n" );
				error = true;
				continue;
			}

			// get the compression quality and advance to the next token.
			quality = atoi( argv[ tok++ ] );
			quality = std::min( quality, 100 );
			quality = std::max( quality, 30 );
			continue;
		}

		// check to see if the normal map flag was specified.
		if ( _stricmp( curArg, "normalmap" ) == 0 )
		{
			normalMap = true;
			++tok;
			if ( optimize )
			{
				printf( "Optimize is not compatible with any other flags!\n" );
				error = true;
			}
			continue;
		}

		// check to see if the optimize flag was specified.
		if ( _stricmp( curArg, "optimize" ) == 0 )
		{
			optimize = true;
			++tok;
			if ( normalMap )
			{
				printf( "Optimize is not compatible with any other flags!\n" );
				error = true;
			}
			continue;
		}

		// not valid?
		++tok;
		printf( "Invalid flag specified!\n" );
		error = true;
	}

	if ( error )
	{
		ShowHelp();
		exit( -1 );
	}
}

void ShowHelp()
{
	printf( "Implement help you slacker!\n" );
}
