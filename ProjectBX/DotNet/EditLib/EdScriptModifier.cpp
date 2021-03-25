//----------------------------------------------------------
// File:		EdScriptModifier.cpp
// Author:		Kevin Bray
// Created:		12-03-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdScriptModifier.h"

// project includes.
#include "RRsrcMgr.h"
#include "RFileMgr.h"
#include "RFile.h"


//**********************************************************
// class EdScriptModifier
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdScriptModifier::EdScriptModifier( const tstring& fileName )
: _fileName( fileName )
, _startLine( 0 )
, _endLine( 0 )
, _valid( false )
{
	// get the file data.
	URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagReadWrite );
	if ( file )
	{
		// reload the script to ensure that it's up to date with what's
		// on disk.
		file->Reload();

		// set the valid flag to true.
		_valid = true;

		// get the file text and calculate it's length.
		const char* text = ( const char* )file->GetData();
		ParseLines( _lines, text );

		// purge the file's data.
		file->Purge();
	}
}

//----------------------------------------------------------
EdScriptModifier::~EdScriptModifier()
{
	// free up allocate strings.
	StringVec::iterator iter = _lines.begin();
	const StringVec::iterator end = _lines.end();
	for ( ; iter != end; ++iter )
		delete ( *iter );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdScriptModifier::SelectObj( const tstring& name )
{
	// scan through each line and try to find the item we're looking for.
	StringVec::iterator iter = _lines.begin();
	const StringVec::iterator end = _lines.end();
	for ( ; iter != end; ++iter )
	{
		if ( FindCommand( *iter, name.c_str() ) )
			break;
	}

	// if we got through the whole list without breaking, then the object
	// wasn't found.
	if ( iter == end )
		return false;

	// find the first opening brace.
	for ( ; iter != end; ++iter )
		if ( strchr( *iter, '{' ) )
			break;

	// if we got through the whole list without breaking, then there was
	// no opening brace!
	if ( iter == end )
		return false;

	// advance past the open brace.
	++iter;
	if ( iter == end )
		return false;

	// store the first line in the object (excluding the opening brace).
	size_t startLine = ( size_t )( iter - _lines.begin() );

	// now we have a brace count of one.  Simply scan ahead until we hit
	// a brace count of 0.  Note that we add when we encounter an open
	// brace and subtract when we encounter a close brace.
	unsigned int braceCount = 1;
	for ( ; iter != end; ++iter )
	{
		if ( strchr( *iter, '{' ) )
			++braceCount;
		else if ( strchr( *iter, '}' ) )
			if ( --braceCount == 0 )
				break;
	}

	// if we couldn't find an end brace, then something is fucked up 
	// about the script.
	if ( iter == end )
		return false;

	// here we update the object's state.  This only happens if finding
	// the data was successful.
	// store the first line in the object.
	_startLine = startLine;

	// store the last line in the object (the closing brace).
	_endLine = iter - _lines.begin();

	// store the name of object we selected.
	_objSelected = name;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void
EdScriptModifier::AddObj( const tstring& name )
{
	// allocate space for the name and copy.
	size_t bufSize = name.length() + 1;
	char* nameLine = new char[ bufSize ];
	strcpy_s( nameLine, bufSize, name.c_str() );

	// allocate space for the opening brace.
	char* openBrace = new char[ 2 ];
	openBrace[0] = '{';
	openBrace[1] = '\0';

	char* closeBrace = new char[ 2 ];
	closeBrace[0] = '}';
	closeBrace[1] = '\0';

	// append the objects to the lines.
	_lines.push_back( nameLine );
	_lines.push_back( openBrace );
	_lines.push_back( closeBrace );

	// store selection info so that this is the currently
	// selected object.
	_startLine = _lines.size()-1;
	_endLine = _startLine;

	// store the name of object we selected.
	_objSelected = name;
}

//----------------------------------------------------------
void
EdScriptModifier::Replace( const tstring& data )
{
	// get new lines.
	StringVec newLines;
	ParseLines( newLines, data.c_str() );

	// determine what lines to remove.
	StringVec::iterator iter = _lines.begin() + _startLine;
	const StringVec::iterator end = _lines.begin() + _endLine;

	// free up the old lines.
	StringVec::iterator cur = iter;
	for ( ; cur != end; ++cur )
		delete[] *cur;

	// remove the old lines if necessary.
	if ( iter != end )
		iter = _lines.erase( iter, end );

	// insert new lines.
	_lines.insert( iter, newLines.begin(), newLines.end() );

	// calculate the new end line.
	_endLine = _startLine + newLines.size();
}

//----------------------------------------------------------
bool
EdScriptModifier::ChangeCommand( const char* command, const tstring& newLine )
{
	B_ASSERT( _valid );
	if ( !_valid )
		return false;

	// search lines in the object for the command we want to replace.
	for ( size_t i = _startLine; i < _endLine; ++i )
	{
		// try to find the command in the current line.
		if ( FindCommand( _lines[ i ], command ) )
		{
			// change the line and return true to indicate success.
			// delete the current line.
			delete _lines[ i ];

			// allocate space for the new line and copy.
			size_t bufSize = newLine.size() + 1;
			_lines[ i ] = new char[ bufSize ];
			strcpy_s( _lines[ i ], bufSize, newLine.c_str() );

			// return true to indicate success.
			return true;
		}
	}

	// currently, we only support changing parameters that are specified
	// in the script!

	// if we got here then we need to add a new line.  Simply add it
	// to the top.

	// return true to indicate success.
	return false;
}

//----------------------------------------------------------
void
EdScriptModifier::AddFlag( const tstring& flag )
{
	// search lines in the object for the flag we want to add.
	for ( size_t i = _startLine; i < _endLine; ++i )
	{
		// return true to indicate success.
		if ( FindCommand( _lines[ i ], flag.c_str() ) )
			return;
	}

	// if we got here then we couldn't find the flag.
	char* buf = new char[ flag.length() + 1 ];
	strcpy_s( buf, flag.length() + 1, flag.c_str() );
	_lines.insert( _lines.begin() + _startLine, buf );
}

//----------------------------------------------------------
void
EdScriptModifier::Save()
{
	// modify the text.
	tstring text;
	StringVec::iterator iter = _lines.begin();
	const StringVec::iterator end = _lines.end();
	int indent = 0;
	for ( ; iter != end; ++iter )
	{
		text += ( *iter );
		text += "\n";

		// check the indent
		if ( strstr( *iter, "{" ) )
			++indent;

		if ( strstr( *iter, "}" ) && indent > 0 )
		{
			// decrement the indent.
			--indent;

			// if we hit block zero, add another newline.
			if ( indent == 0 )
				text += "\n";
			// add another newline.
//			text += "\n";
		}
	}

	// save out the file.
	URef< RFile > file = gRFileMgr->GetFile( _fileName, RFileMgr::kFlagReadWrite );
	file->WriteData( 0, text.c_str(), ( unsigned int )text.length() );

	// TODO: FIX THIS!!  This is a hack around a bug where writing to a file doesn't
	// truncate it.  Temporarily fill the rest of the file with spaces.
	B_ASSERT( file->GetSize() >= text.length() );
	unsigned int extraCount = (unsigned int)( file->GetSize() - text.length() );
	if ( extraCount > 0 )
	{
		tstring padding;
		padding.append( extraCount, ' ' );
		file->WriteData( text.length(), padding.c_str(), extraCount );
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdScriptModifier::ParseLines( StringVec& output, const char* text )
{
	// make sure there isn't anything in the output vector.
	B_ASSERT( output.size() == 0 );

	// get the file text and calculate it's length.
	size_t memSize = strlen( text ) + 1;

	// create a copy of our string for our own purposes.
	char* localCopy = new char[ memSize ];
	strcpy_s( localCopy, memSize, text ); 

	// tokenize to get lines.  Note that this can be modified
	// to preserve zero length lines.
	char* context = 0;
	char* curLine = strtok_s( localCopy, "\n\r", &context );
	while ( curLine )
	{
		// copy the current token to a new chunk of memory.
		size_t len = strlen( curLine );
		char* str = new char[ len+1 ];
		strcpy_s( str, len+1, curLine );

		// add the line to our vector.
		output.push_back( str );

		// next line.
		curLine = strtok_s( 0, "\n\r", &context );
	}

	// free our local copy.
	delete localCopy;
}

//----------------------------------------------------------
bool
EdScriptModifier::FindCommand( const char* text, const char* command )
{
	// create a local copy of the text.
	size_t bufSize = strlen( text ) + 1;
	char* buf = new char[ bufSize ];
	strcpy_s( buf, bufSize, text );

	// if the command is the first word in the line, then we have a match.
	char* context = 0;
	char* curLineCommand = strtok_s( buf, " \t", &context );
	bool found = false;
	if ( curLineCommand )
		found = ( _stricmp( curLineCommand, command ) == 0 );

	// free our buffer.
	delete[] buf;

	// return whether or not we found the object.
	return found;
}
