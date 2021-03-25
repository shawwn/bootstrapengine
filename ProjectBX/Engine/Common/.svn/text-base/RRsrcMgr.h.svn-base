//----------------------------------------------------------
// File:		RRsrcMgr.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		To manage resources.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "RRsrc.h"
#include "UReader.h"
#include "UPath.h"
#include "BscVarTable.h"
#include "RConfig.h"

// STL
#include <map>
#include <vector>

// forward declarations.
class RFile;
class GrImage;
class GrMaterial;
class BscArrayTable;
class UExprArray;
struct SBscToken;

//**********************************************************
// class RRsrcMgr
//**********************************************************
class RRsrcMgr
{
public:
	RRsrcMgr();
	~RRsrcMgr();

	typedef bool ( *Parser )( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );

	// registers a script parser.  When an 'object' is encountered, it will be parsed with the appropriate
	// callback.
	void					RegisterScriptParser( const tstring& keyword, Parser parser );

	// loads a script file.  The default keyword is used if a keyword is not present.  Returns true
	// on success.  If noScriptName is false, then the name of the script file will be prepended to
	// the name of each resource created.  The script file name and the resource name will be separated
	// with a ':'.
	bool					LoadScript( const UPath& fileName, const tstring& defKeyword, tstring& errors );

	// returns the script currently being parsed (or the last script parsed).  This is available
	// mainly so that parsers know the name of the file they're parsing.
	const UPath&			GetRecentScriptName() const			{	return _recentScriptName;		}

	// Finds a resource that has already been loaded.
	template < typename _T >
	URef< _T >				Find( const UPath& fileName );

private:
	typedef std::map< UPath, URef< RRsrc > > RsrcMap;
	typedef std::map< tstring, Parser > ParserMap;

	bool					IsKeyword( const tstring& keyword ) const;
	bool					CreateRsrc( const tstring& keyword, const SBscToken*& stream, const BscArrayTable& arrayTable,
										const BscVarTable& varTable, tstring& errors );
	void					MakeParserName( tstring& parser, const tstring& keyword ) const;

	RsrcMap					_rsrcMap;
	ParserMap				_parserMap;
	UPath					_recentScriptName;
};

extern RRsrcMgr *gRRsrcMgr;


//==========================================================
// template functions
//==========================================================

//----------------------------------------------------------
template < typename _T >
URef< _T >
RRsrcMgr::Find( const UPath& name )
{
	RsrcMap::iterator iter = _rsrcMap.find( name );
	if ( iter != _rsrcMap.end() )
	{
		return ( _T * )( ( RRsrc * )iter->second );
	}
	return 0;
}
