//----------------------------------------------------------
// File:		RRsrcMgr.cpp
// Author:		Kevin Bray
// Created:		09-19-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "RRsrcMgr.h"

// project includes.
//#include "GrImage.h"
#include "RConfig.h"
//#include "ShApp.h"
//#include "GrParse.h"
#include "RFile.h"
#include "RFileMgr.h"
#include "UExprMgr.h"

#include "BscLexer.h"
#include "BscArrayTable.h"
#include "BscParseLib.h"
#include "BscExprLib.h"
#include "UExprCallback.h"
#include "UExprArray.h"

// std c++ includes.
#include <string.h>

//**********************************************************
// class RRsrcMgr
//**********************************************************

RRsrcMgr *gRRsrcMgr = 0;

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
RRsrcMgr::RRsrcMgr()
{
	// the file manager must be started before the resource manager!
	B_ASSERT( gUExprMgr != 0 );
	B_ASSERT( gRFileMgr != 0 );

	// make sure the resource manager hasn't already been started.
	B_ASSERT( gRRsrcMgr == 0 );
	gRRsrcMgr = this;
}

//----------------------------------------------------------
RRsrcMgr::~RRsrcMgr()
{
	gRRsrcMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
RRsrcMgr::RegisterScriptParser( const tstring& keyword, Parser parser )
{
	tstring parserName;
	MakeParserName( parserName, keyword );
	_parserMap[ parserName ] = parser;
}

//----------------------------------------------------------
bool
RRsrcMgr::LoadScript( const UPath& fileName, const tstring& defKeyword, tstring& errors )
{
	// create the array table.
	BscArrayTable arrayTable( gUExprMgr->GetGlobalArrayTable() );

	// note that we cannot create file local variables, so we can simply
	// grab a const reference to the global variable table.
	const BscVarTable& varTable = gUExprMgr->GetGlobalVarTable();

	// ensure that a default keyword was specified.
	B_ASSERT( IsKeyword( defKeyword ) );

	// read in the file data and pass it to the lexer.
	URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagRead );
	if ( !file )
	{
		// return an error if we could not load!
		errors = "Error loading script file '";
		errors += fileName.GetPathString() + "': File is damaged or missing!";
		return false;
	}

	// get the file data.
	const void* fileData = file->GetData();
	unsigned int size = ( unsigned int )file->GetSize();

	// create a lexer to build the token stream.
	BscLexer lexer;
	lexer.Lex( ( const char* )fileData );
	const SBscToken* curTok = lexer.GetTokenStream();

	// go through each token and determine what kind of resource needs to
	// be created.
	tstring keyword;
	unsigned int tokensRead = 0;
	while ( curTok->tokenId != BL_EOS )
	{
		if ( BscIsArray( curTok, arrayTable, varTable ) )
		{
			// we have an array.
			UExprArray* array = 0;
			tstring name;
			curTok += BscParseArray( &array, name, curTok, arrayTable, varTable );
			arrayTable.AddArray( name, array );

			// loop again.
			continue;
		}
		else if ( curTok->tokenId == BL_IDENTIFIER && IsKeyword( curTok->text ) )
		{
			// we have a keyword.
			keyword = curTok->text;
			++curTok;
		}
		else
		{
			// no keyword specified, so assume the default keyword.
			keyword = defKeyword;
		}

		// set the recent script name and create the resource.
		_recentScriptName = fileName;

		// Create the resource.
		if ( !CreateRsrc( keyword, curTok, arrayTable, varTable, errors ) )
		{
			// make sure we don't keep a copy of the file's data
			// around.
			file->Purge();

			// build the error message.
			std::ostringstream ss;
			ss << "Error creating resource specified in script!\n"
			   << "File: " << fileName.GetPathString() << "\n"
			   << "Line: " << curTok->lineNum << "\n"
			   << "Reason: " << errors;
			
			// set the output string.
			errors = ss.str();
			return false;
		}
	}

	// make sure we don't keep a copy of the file's data around.
	file->Purge();

	// success!
	return true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
RRsrcMgr::IsKeyword( const tstring& keyword ) const
{
	// check to see if we have a keyword.
	if ( _stricmp( keyword.c_str(), R_KW_LOADSCRIPT ) == 0 )
		return true;

	// see if a parser has been registered.
	tstring parser;
	MakeParserName( parser, keyword );

	// not a keyword.
	return _parserMap.find( parser ) != _parserMap.end();
}

//----------------------------------------------------------
bool
RRsrcMgr::CreateRsrc( const tstring& keyword, const SBscToken*& curTok, const BscArrayTable& arrayTable,
					  const BscVarTable& varTable, tstring& errors )
{
	// make sure that what is being passed in is a keyword.
	B_ASSERT( IsKeyword( keyword ) );

	unsigned int tokensRead = 0;

	if ( _stricmp( keyword.c_str(), R_KW_LOADSCRIPT ) == 0 )
	{
		// load script command is currently not supported.
		B_ASSERT( false );
		return false;
	}
	else
	{
		// determine a parser's name.
		tstring parser;
		MakeParserName( parser, keyword );

		// try to find the parser.
		ParserMap::iterator iter = _parserMap.find( parser );
		B_ASSERT( iter != _parserMap.end() );

		// call the parser.
		return iter->second( curTok, arrayTable, varTable, errors );
	}

	// indicate success.
	return true;
}

//----------------------------------------------------------
void
RRsrcMgr::MakeParserName( tstring& parser, const tstring& keyword ) const
{
	parser = keyword;
	for ( unsigned int i = 0; i < parser.length(); ++i )
		parser[ i ] = tolower( parser[ i ] );
}
