//----------------------------------------------------------
// File:		BscParseLib.h
// Author:		Kevin Bray
// Created:		07-07-05
//
// Purpose:		A library for parsing BScript.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
struct SBscToken;
class BscArrayTable;
class BscVarTable;
class UPath;
class UExpr;
class UExprTerm;

struct SFlagValue
{
	const char* str;
	unsigned int value;
};

enum EBSCITEMTYPE
{
	EIT_IDENT,
	EIT_PATH,
	EIT_NAMEDEXPR,
	EIT_NAMEDEXPRLIST,
	EIT_NAMEDFLAG,
	EIT_NAMEDFLAGLIST,
	EIT_NAMEDIDENTEXPR,
	EIT_NAMEDIDENTEXPRLIST,
	EIT_NAMEDPATH,
	EIT_NAMEDPATHLIST,
	EIT_NAMEDIDENTPATH,
	EIT_NAMEDIDENTPATHFLAGLIST,
	EIT_NAMEDIDENTPATHLIST,
	EIT_NAMEDIDENTPATHLISTFLAGLIST,
	EIT_NAMEDIDENTPAIR,
	EIT_NAMEDSTRING,
	EIT_COUNT,
};

// item description that is passed in as an array.
struct SBscItem
{
	// type of this element.
	EBSCITEMTYPE		type;

	// if a name is needed.
	const char*			name;

	// id assigned to a value when an item of this type is found.
	unsigned int		itemId;

	// if flags are needed.
	const SFlagValue*	flagValues;
	unsigned int		flagCount;
};

// structure that gets filled out when an item is checked to see if it
// can be parsed.
struct SBscItemInfo
{
	// item type.
	EBSCITEMTYPE		type;

	// item ID... set when a value is parsed in.  This comes from the
	// ID contained in the SBscItem structure.
	unsigned int		itemId;

	// index of the item that was matched.
	unsigned int		itemIdx;

	// for lists of items, this is the number of items in a list.
	unsigned int		valueCount;
};

// structure that gets filled out when an item is parsed.
struct SBscItemValue
{
	SBscItemValue();
	~SBscItemValue();

	// used internally to know what data to delete.
	EBSCITEMTYPE		type;

	// item ID... set when a value is parsed in.
	unsigned int		itemId;

	// data that depends on what kind of parameter we're dealing with.
	unsigned int		flagValues;

	URef< UExprTerm >*	exprs;
	unsigned int		exprCount;

	UPath*				paths;
	unsigned int		pathCount;

	tstring*			strings;
	unsigned int		stringCount;

	tstring*			idents;
	unsigned int		identCount;
};

// NOTES:
// The return values for all of the BscParse methods is the number of tokens handled.

//**********************************************************
// function declarations
//**********************************************************

// High level parsing interface.  Note that items will be matched in the order they are
// present in the array.  This function will return on the first match found.
extern bool				BscIsItem( SBscItemInfo* itemInfo, const SBscItem* items, unsigned int itemCount,
								   const BscArrayTable* arrayTable, const BscVarTable* varTable, const SBscToken* tokens,
								   unsigned int* tokensRead = 0 );
extern unsigned int 	BscParseItem( SBscItemValue* value, const SBscItemInfo* itemInfo, const SBscItem* items,
									  unsigned int itemCount, const BscArrayTable* arrayTable, const BscVarTable* varTable,
									  const SBscToken* tokens );

// more specific parsing routines.
extern bool				BscIsIdent( const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseIdent( tstring& ident, const SBscToken* tokens );

extern bool				BscIsPath( const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParsePath( UPath& path, const SBscToken* tokens );

extern bool				BscIsNamedExpr( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
									    const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedExpr( URef< UExprTerm >* expr, const char* name, const SBscToken* tokens,
										   const BscArrayTable& arrayTable, const BscVarTable& varTable );

// returns the expression count.
extern unsigned int		BscIsNamedExprList( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable, 
										    const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedExprList( URef< UExprTerm >* exprs, unsigned int maxExprCount, const char* name, const SBscToken* tokens,
											   const BscArrayTable& arrayTable, const BscVarTable& varTable );

// pass in a list of acceptible flags.  BscParseNamedFlag will return the flag specified.
extern bool				BscIsNamedFlag( const char* name, const SBscToken* tokens, const SFlagValue* flagValues,
										unsigned int flagValueCount, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedFlag( unsigned int& value, const char* name, const SBscToken* tokens,
										   const SFlagValue* flagValues, unsigned int flagValueCount );

// pass in a list of acceptible flags. BscParseNamedFlagList will return the flags
// specified or'd together.
extern bool				BscIsNamedFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues, 
											unsigned int flagValueCount, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedFlagList( unsigned int& value, const char* name, const SBscToken* tokens,
											   const SFlagValue* flagValues, unsigned int flagValueCount );

// will return the identifier and the expression specified.
extern bool				BscIsNamedIdentExpr( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
											 const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentExpr( tstring& ident, URef< UExprTerm >* expr, const char* name, const SBscToken* tokens,
											    const BscArrayTable& arrayTable, const BscVarTable& varTable );

// returns the expression count.
extern unsigned int		BscIsNamedIdentExprList( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
												 const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentExprList( tstring& ident, URef< UExprTerm >* exprs, unsigned int maxExprCount, const char* name,
												    const SBscToken* tokens, const BscArrayTable& arrayTable,
													const BscVarTable& varTable );

// returns a path.
extern bool				BscIsNamedPath( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedPath( UPath& path, const char* name, const SBscToken* tokens );

// returns a path and an identifier.
extern bool				BscIsNamedIdentPath( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentPath( tstring& ident, UPath& path, const char* name, const SBscToken* tokens );

// returns paths and an identifier.
extern unsigned int		BscIsNamedIdentPathList( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentPathList( tstring& ident, UPath* paths, unsigned int maxPathCount, const char* name, const SBscToken* tokens );

// returns a path, an identifier, and flag values.
extern bool				BscIsNamedIdentPathFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues, 
													 unsigned int flagValueCount, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentPathFlagList( tstring& ident, UPath& path, unsigned int& flagValue, const char* name,
													    const SBscToken* tokens, const SFlagValue* flagValues,
														unsigned int flagValueCount );

// returns paths, an identifier, and flag values.
extern unsigned int		BscIsNamedIdentPathListFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues, 
														 unsigned int flagValueCount, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentPathListFlagList( tstring& ident, UPath* paths, unsigned int maxPathCount,
														    unsigned int& flagValue, const char* name, const SBscToken* tokens,
															const SFlagValue* flagValues, unsigned int flagValueCount );

// returns two identifiers.
extern bool				BscIsNamedIdentPair( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedIdentPair( tstring& ident1, tstring& ident2, const char* name, const SBscToken* tokens );

// returns a list of paths.
extern unsigned int		BscIsNamedPathList( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedPathList( UPath* paths, unsigned int maxPathCount, const char* name, const SBscToken* tokens );

// returns a named string.
extern bool				BscIsNamedString( const char* name, const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int		BscParseNamedString( tstring& str, const char* name, const SBscToken* tokens );
