//----------------------------------------------------------
// File:		EdImportOBJ.cpp
// Author:		Kevin Bray
// Created:		08-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdImportOBJ.h"

// engine includes.
#include "EdSimplePolygonGroup.h"
#include "EdSimplePolygon.h"
#include "UFastArray.h"

// std c++ includes.
#include <vector>

#define MAX_OBJ_LINE_LENGTH				2048
#define MAX_POLYGON_VERTEX_COUNT		EdSimplePolygon::E_MAX_VERTEX_COUNT

template < typename _T > bool GetVectorElem( _T& elem, const std::vector< _T >& vec, int index )
{
	// check to see how we should address the vector passed in.
	if ( index >= 0 )
	{
		// ensure that the index passed in is valid.
		if ( ( unsigned int )index >= vec.size() )
			return false;
		elem = vec[ index ];
	}
	else
	{
		// ensure that the index passed in is valid.
		if ( ( unsigned int )( -index ) > vec.size() )
			return false;
		elem = vec[ vec.size() - index ];
	}

	// return true to indicate success.
	return true;
}

//**********************************************************
// class EdImportOBJ
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportOBJ::EdImportOBJ()
{

}

//----------------------------------------------------------
EdImportOBJ::~EdImportOBJ()
{

}


//==========================================================
// public
//==========================================================

//----------------------------------------------------------
void
EdImportOBJ::LoadOBJ( EdSimplePolygonGroup& group, const char* fileName )
{
	// buffer for file text.
	char buffer[ MAX_OBJ_LINE_LENGTH + 1 ];

	// position array.
//	std::vector< SVec2 > texcoords;
//	std::vector< SVec3 > normals;
	std::vector< int > pIndices;
//	std::vector< int > tIndices;
//	std::vector< int > nIndices;

	// reserve some memory to avoid lots of memory-allocation
	// overhead.
	pIndices.reserve( MAX_POLYGON_VERTEX_COUNT );
//	tIndices.reserve( MAX_POLYGON_VERTEX_COUNT );
//	nIndices.reserve( MAX_POLYGON_VERTEX_COUNT );

	// allocate some space for our vertices.
	SVec3 vecTemp;
	SVec3 vertexTemp[ MAX_POLYGON_VERTEX_COUNT ];
	SVec3 autoScale( 1.0f, 1.0f, 1.0f );
	SVec3 autoOffset( 0.0f, 0.0f, 0.0f );

	// open the file.
	FILE* fp = fopen( fileName, "rb" );
	if ( !fp )
		return;

	// iterate through everything in the file.
	char* context;
	while ( !feof( fp ) )
	{
		// read a line.
		unsigned int i = 0;
		char cur = fgetc( fp );
		while ( cur != '\n' && cur != EOF && i < MAX_OBJ_LINE_LENGTH )
		{
			buffer[ i++ ] = cur;
			cur = fgetc( fp );
		}

		// end the line with the null terminator.
		buffer[ i ] = '\0';

		// check to see if we have a supported hint.
		if ( _strnicmp( buffer, "#auto scale", 11 ) == 0 )
		{
			ReadHint( autoScale, buffer + 12 );
		}
		else if ( _strnicmp( buffer, "#auto offset", 12 ) == 0 )
		{
			ReadHint( autoOffset, buffer + 13 );
		}
		else if ( _strnicmp( buffer, "#face count", 11 ) == 0 )
		{
			group.ReservePolygons( atoi( buffer + 12 ) );
		}
		else if ( _strnicmp( buffer, "#vertex count", 13 ) == 0 )
		{
			unsigned int vertexCount = atoi( buffer + 14 );
			group.ReserveVertices( vertexCount );
		}

		// if this is a comment, simply skip it.
		if ( buffer[ 0 ] == '#' )
			continue;

		// parse the current line.
		context = 0;
		char* cmd = strtok_s( buffer, " \t\r", &context );
		if ( !cmd )
			continue;

		// determine what to do based on the current state.
		if ( _stricmp( cmd, "v" ) == 0 )
		{
			// parse the position and add it to our table of positions.
			ReadVec3( vecTemp, cmd, &context );

			// scale then offset.
//			vecTemp *= autoScale;
//			vecTemp += autoOffset;
			vecTemp /= 100.0f;
			group.AddVertex( vecTemp );
		}
		else if ( _stricmp( cmd, "t" ) == 0 )
		{
			// parse the position and add it to our table of positions.
//			ReadVec3( vecTemp, cmd, &context );
//			texcoords.push_back( SVec2( vecTemp.x, vecTemp.y ) );

		}
		else if ( _stricmp( cmd, "n" ) == 0 )
		{
			// parse the position and add it to our table of positions.
//			ReadVec3( vecTemp, cmd, &context );
//			normals.push_back( vecTemp );
		}
		else if ( _stricmp( cmd, "f" ) == 0 )
		{
			// parse a polygonal face.
			char* f = strtok_s( 0, " \t\r", &context );
			while ( f != 0 )
			{
				// parse a vertex specifier.
				int index = atoi( f );
				pIndices.push_back( index );

				// check for a texture specifier.
				char* loc = strchr( f, '/' );
				if ( loc )
				{
					// advance past the '/'.
					++loc;
					if ( *loc != '/' )
					{
//						tIndices.push_back( atoi( loc ) );
						loc = strchr( loc, '/' );
					}
				}

				// if loc has a value here, then we have a normal
				// specifier.
//				if ( loc )
//					nIndices.push_back( atoi( loc ) );

				// get the next token.
				f = strtok_s( 0, " \t\r", &context );
			}

			// create adjusted (non-negative) indices.
			unsigned int indices[ 100 ];
			unsigned int indexCount = ( unsigned int )pIndices.size();
			for ( unsigned int i = 0; i < indexCount; ++i )
			{
				if ( pIndices[ i ] >= 0 )
					indices[ i ] = pIndices[ i ] - 1;
				else
					indices[ i ] = ( unsigned int )( ( int )pIndices.size() + pIndices[ i ] );
			}

			// now add the triangles to our group.
			unsigned int triIndices[ 3 ];
			unsigned int triCount = indexCount - 2;
			for ( unsigned int i = 0; i < triCount; ++i )
			{
				// get the indices for the current triangle.
				triIndices[ 0 ] = indices[ 0 ];
				triIndices[ 1 ] = indices[ i + 1 ];
				triIndices[ 2 ] = indices[ i + 2 ];
				
				// add the triangle to our group.
				group.AddPolygon( triIndices, 3 );
			}

			// clear our indices.
			pIndices.resize( 0 );
//			tIndices.resize( 0 );
//			nIndices.resize( 0 );
		}
	}

	// close the file.
	fclose( fp );
}


//==========================================================
// private
//==========================================================

//----------------------------------------------------------
void
EdImportOBJ::ReadVec3( SVec3& result, char* start, char** strTokContext )
{
	for ( unsigned int i = 0; i < 3; ++i )
	{
		// initialize the current value to 0.
		result[ i ] = 0.0f;

		// read in the value specified by the file.
		char* p = strtok_s( 0, " \t\r", strTokContext );
		if ( p )
		{
			double value = atof( p );
			result[ i ] = ( float )value;
		}
	}
}

//----------------------------------------------------------
void
EdImportOBJ::ReadHint( SVec3& hint, char* buffer )
{
	// grab all of the components.
	unsigned int tokenCount = 0;
	char* tokens[ 6 ];
	char* context = 0;
	char* curTok = strtok_s( buffer, " \t=", &context );
	while ( curTok != 0 && tokenCount < 6 )
	{
		tokens[ tokenCount++ ] = curTok;
		curTok = strtok_s( 0, " \t=", &context );
	}

	// if there aren't enough tokens, then simply return.
	if ( tokenCount < 6 )
		return;

	// figure out what to do with the components.
	for ( unsigned int i = 0; i < 3; ++i )
	{
		// get the current axis.
		unsigned int axis = ( tolower( *tokens[ 2 * i + 0 ] ) - 'x' );
		if ( axis < 3 )
			hint[ axis ] = ( float )atof( tokens[ 2 * i + 1 ] );
	}
}
