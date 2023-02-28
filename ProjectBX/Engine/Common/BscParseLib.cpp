//----------------------------------------------------------
// File:        BscParseLib.cpp
// Author:      Kevin Bray
// Created:     07-07-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// lib header.
#include "BscParseLib.h"

// project includes.
#include "BscLexer.h"
#include "BscArrayTable.h"
#include "BscVarTable.h"
#include "BscExprLib.h"
#include "UExpr.h"
#include "UPath.h"


//**********************************************************
// SBscItemValue dtor.
//**********************************************************

//----------------------------------------------------------
SBscItemValue::SBscItemValue()
: type( EIT_COUNT )
, itemId( 0xFFFFFFFF )
, flagValues( 0 )
, exprs( 0 )
, exprCount( 0 )
, paths( 0 )
, pathCount( 0 )
, strings( 0 )
, stringCount( 0 )
, idents( 0 )
, identCount( 0 )
{

}

//----------------------------------------------------------
SBscItemValue::~SBscItemValue()
{
    delete[] idents;
    delete[] strings;
    delete[] paths;
    delete[] exprs;
}


//**********************************************************
// item parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsItem( SBscItemInfo* itemInfo, const SBscItem* items, unsigned int itemCount, const BscArrayTable* arrayTable,
           const BscVarTable* varTable, const SBscToken* tokens, unsigned int* tokensRead )
{
    // item info must not be null.
    B_ASSERT( itemInfo != 0 );

    // initialize to an invalid index.
    itemInfo->itemIdx = 0xFFFFFFFF;

    // go through the sorted items parse.
    for ( unsigned int i = 0; i < itemCount; ++i )
    {
        switch ( items[ i ].type )
        {
        case EIT_IDENT:
            if ( BscIsIdent( tokens, tokensRead ) )
            {
                itemInfo->type = EIT_IDENT;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_PATH:
            if ( BscIsPath( tokens, tokensRead ) )
            {
                itemInfo->type = EIT_PATH;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDEXPR:
            if ( BscIsNamedExpr( items[ i ].name, tokens, *arrayTable, *varTable, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDEXPR;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDEXPRLIST:
            {
                unsigned int exprCount = BscIsNamedExprList( items[ i ].name, tokens, *arrayTable, *varTable, tokensRead );
                if ( exprCount > 0 )
                {
                    itemInfo->type = EIT_NAMEDEXPRLIST;
                    itemInfo->itemId = items[ i ].itemId;
                    itemInfo->itemIdx = i;
                    itemInfo->valueCount = exprCount;
                    return true;
                }
            }
            break;

        case EIT_NAMEDFLAG:
            // must not be NULL if this is a named flag!
            B_ASSERT( items[ i ].flagValues != 0 );
            if ( BscIsNamedFlag( items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDFLAG;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDFLAGLIST:
            // must not be NULL if this is a flag list!
            B_ASSERT( items[ i ].flagValues != 0 );
            if ( BscIsNamedFlagList( items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDFLAGLIST;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDIDENTEXPR:
            if ( BscIsNamedIdentExpr( items[ i ].name, tokens, *arrayTable, *varTable, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDIDENTEXPR;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDIDENTEXPRLIST:
            {
                unsigned int exprCount = BscIsNamedIdentExprList( items[ i ].name, tokens, *arrayTable, *varTable, tokensRead );
                if ( exprCount > 0 )
                {
                    itemInfo->type = EIT_NAMEDIDENTEXPRLIST;
                    itemInfo->itemId = items[ i ].itemId;
                    itemInfo->itemIdx = i;
                    itemInfo->valueCount = exprCount;
                    return true;
                }
            }
            break;

        case EIT_NAMEDPATH:
            if ( BscIsNamedPath( items[ i ].name, tokens, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDPATH;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDPATHLIST:
            {
                unsigned int pathCount = BscIsNamedPathList( items[ i ].name, tokens, tokensRead );
                if ( pathCount > 0 )
                {
                    itemInfo->type = EIT_NAMEDPATHLIST;
                    itemInfo->itemId = items[ i ].itemId;
                    itemInfo->itemIdx = i;
                    itemInfo->valueCount = pathCount;
                    return true;
                }
            }
            break;

        case EIT_NAMEDIDENTPATH:
            if ( BscIsNamedIdentPath( items[ i ].name, tokens, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDIDENTPATH;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDIDENTPATHFLAGLIST:
            if ( BscIsNamedIdentPathFlagList( items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDIDENTPATHFLAGLIST;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        case EIT_NAMEDIDENTPATHLIST:
            {
                unsigned int pathCount = BscIsNamedIdentPathList( items[ i ].name, tokens, tokensRead );
                if ( pathCount )
                {
                    itemInfo->type = EIT_NAMEDIDENTPATHLIST;
                    itemInfo->itemId = items[ i ].itemId;
                    itemInfo->itemIdx = i;
                    itemInfo->valueCount = pathCount;
                    return true;
                }
            }
            break;

        case EIT_NAMEDIDENTPATHLISTFLAGLIST:
            {
                unsigned int pathCount = BscIsNamedIdentPathListFlagList( items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount, tokensRead );
                if ( pathCount )
                {
                    itemInfo->type = EIT_NAMEDIDENTPATHLISTFLAGLIST;
                    itemInfo->itemId = items[ i ].itemId;
                    itemInfo->itemIdx = i;
                    itemInfo->valueCount = pathCount;
                    return true;
                }
            }
            break;

        case EIT_NAMEDIDENTPAIR:
            if ( BscIsNamedIdentPair( items[ i ].name, tokens, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDIDENTPAIR;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 2;
                return true;
            }
            break;

        case EIT_NAMEDSTRING:
            if ( BscIsNamedString( items[ i ].name, tokens, tokensRead ) )
            {
                itemInfo->type = EIT_NAMEDSTRING;
                itemInfo->itemId = items[ i ].itemId;
                itemInfo->itemIdx = i;
                itemInfo->valueCount = 1;
                return true;
            }
            break;

        default:
            // unrecognized item type!
            B_ASSERT( false );
        }
    }

    return false;
}

//----------------------------------------------------------
unsigned int
BscParseItem( SBscItemValue* value, const SBscItemInfo* itemInfo, const SBscItem* items, unsigned int itemCount,
              const BscArrayTable* arrayTable, const BscVarTable* varTable, const SBscToken* tokens )
{
    B_ASSERT( value != 0 );
    B_ASSERT( itemInfo != 0 );
    B_ASSERT( itemInfo->itemIdx < itemCount );

    // make sure the item at the index specified and the item specified by itemInfo
    // match up!
    B_ASSERT( items[ itemInfo->itemIdx ].itemId == itemInfo->itemId );
    value->itemId = itemInfo->itemId;

    unsigned int i = itemInfo->itemIdx;
    switch ( items[ i ].type )
    {
    case EIT_IDENT:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->strings == 0 );

        value->idents = new tstring[ itemInfo->valueCount ];
        value->identCount = itemInfo->valueCount;

        return BscParseIdent( *value->strings, tokens );

    case EIT_PATH:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;

        return BscParsePath( *value->paths, tokens );

    case EIT_NAMEDEXPR:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->exprs == 0 );

        value->exprs = new URef< UExprTerm >[ itemInfo->valueCount ];
        value->exprCount = itemInfo->valueCount;

        return BscParseNamedExpr( value->exprs, items[ i ].name, tokens, *arrayTable, *varTable );

    case EIT_NAMEDEXPRLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->exprs == 0 );

        value->exprs = new URef< UExprTerm >[ itemInfo->valueCount ];
        value->exprCount = itemInfo->valueCount;

        return BscParseNamedExprList( value->exprs, itemInfo->valueCount, items[ i ].name, tokens, *arrayTable, *varTable );

    case EIT_NAMEDFLAG:
        // must not be NULL if this is a named flag!
        B_ASSERT( items[ i ].flagValues != 0 );

        return BscParseNamedFlag( value->flagValues, items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount );

    case EIT_NAMEDFLAGLIST:
        // must not be NULL if this is a flag list!
        B_ASSERT( items[ i ].flagValues != 0 );

        return BscParseNamedFlagList( value->flagValues, items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount );

    case EIT_NAMEDIDENTEXPR:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->exprs == 0 );
        B_ASSERT( value->idents == 0 );

        value->exprs = new URef< UExprTerm >[ itemInfo->valueCount ];
        value->exprCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentExpr( *value->idents, value->exprs, items[ i ].name, tokens, *arrayTable, *varTable );

    case EIT_NAMEDIDENTEXPRLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->exprs == 0 );
        B_ASSERT( value->idents == 0 );

        value->exprs = new URef< UExprTerm >[ itemInfo->valueCount ];
        value->exprCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentExprList( *value->idents, value->exprs, itemInfo->valueCount, items[ i ].name,
            tokens, *arrayTable, *varTable );

    case EIT_NAMEDPATH:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;

        return BscParseNamedPath( *value->paths, items[ i ].name, tokens );

    case EIT_NAMEDPATHLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;

        return BscParseNamedPathList( value->paths, itemInfo->valueCount, items[ i ].name, tokens );

    case EIT_NAMEDIDENTPATH:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );
        B_ASSERT( value->idents == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentPath( *value->idents, *value->paths, items[ i ].name, tokens );

    case EIT_NAMEDIDENTPATHFLAGLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );
        B_ASSERT( value->idents == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentPathFlagList( *value->idents, *value->paths, value->flagValues, items[ i ].name, tokens,
            items[ i ].flagValues, items[ i ].flagCount );

    case EIT_NAMEDIDENTPATHLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );
        B_ASSERT( value->idents == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentPathList( *value->idents, value->paths, itemInfo->valueCount,
            items[ i ].name, tokens );

    case EIT_NAMEDIDENTPATHLISTFLAGLIST:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );
        B_ASSERT( value->idents == 0 );

        value->paths = new UPath[ itemInfo->valueCount ];
        value->pathCount = itemInfo->valueCount;
        value->idents = new tstring[ 1 ];
        value->identCount = 1;

        return BscParseNamedIdentPathListFlagList( *value->idents, value->paths, itemInfo->valueCount,
            value->flagValues, items[ i ].name, tokens, items[ i ].flagValues, items[ i ].flagCount );

    case EIT_NAMEDIDENTPAIR:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->paths == 0 );

        value->idents = new tstring[ 2 ];
        value->identCount = 2;

        return BscParseNamedIdentPair( value->idents[ 0 ], value->idents[ 1 ], items[ i ].name, tokens );

    case EIT_NAMEDSTRING:
        // make sure it's NULL, or we're going to have a dangling pointer.
        B_ASSERT( value->strings == 0 );

        value->strings = new tstring[ itemInfo->valueCount ];
        value->stringCount = itemInfo->valueCount;

        return BscParseNamedString( *value->strings, items[ i ].name, tokens );

    default:
        // unrecognized type!
        B_ASSERT( false );
    }
    return 0;
}


//**********************************************************
// identifier parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsIdent( const SBscToken* tokens, unsigned int* tokensReadOut )
{
    SafeOffset tokensRead( tokensReadOut );

    if ( tokens->tokenId == BL_IDENTIFIER )
    {
        tokensRead = 1;
        return true;
    }
    tokensRead = 0;
    return false;
}

//----------------------------------------------------------
unsigned int
BscParseIdent( tstring& ident, const SBscToken* tokens )
{
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsIdent( tokens ) );

    // get the identifier.
    ident = tokens->text;

    // handled one token.
    return 1;
}


//**********************************************************
// path parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsPath( const SBscToken* tokens, unsigned int* tokensReadOut )
{
    SafeOffset tokensRead( tokensReadOut );

    if ( tokens->tokenId != BL_IDENTIFIER && tokens->tokenId != BL_STRING &&
        tokens->tokenId != BL_NUMBER )
    {
        tokensRead = 0;
        return false;
    }
    tokensRead = 1;
    return true;
}

//----------------------------------------------------------
unsigned int
BscParsePath( UPath& path, const SBscToken* tokens )
{
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsPath( tokens ) );

    // get the identifier.
    path = tokens->text;

    // handled one token.
    return 1;
}


//**********************************************************
// named expression parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedExpr( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable, 
                const BscVarTable& varTable, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );
    tokensRead = 0;
    unsigned int subTokensRead = 0;
    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

//  ++tokensRead;
//  if ( tokens[ 1 ].tokenId != BL_EQUALS )
//      return false;

    ++tokensRead;

    bool isExpr = BscIsExpr( tokens + 1, arrayTable, varTable, &subTokensRead );
    tokensRead += subTokensRead;

    return isExpr;
}

//----------------------------------------------------------
unsigned int
BscParseNamedExpr( URef< UExprTerm >* expr, const char* name, const SBscToken* tokens,
                   const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedExpr( name, tokens + tokensRead, arrayTable, varTable ) );

    // initialize to 2 to account for the name.
    tokensRead = 1;
    tokensRead += BscParseExpr( expr, tokens + tokensRead, arrayTable, varTable );

    return tokensRead;
}


//**********************************************************
// named expression list parser.
//**********************************************************

//----------------------------------------------------------
unsigned int
BscIsNamedExprList( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
                    const BscVarTable& varTable, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );
    unsigned int exprCount = 0;

    // check to see if we have an name, followed by an equal sign, followed
    // by expressions, separated by commas.
    tokensRead = 1;
    if ( _stricmp( tokens[ 0 ].text.c_str(), name ) != 0 )
        return 0;

//  tokensRead = 2;
//  if ( tokens[ 1 ].tokenId != BL_EQUALS )
//      return 0;

    // check all subsequent paths making sure they're paths.
    unsigned int subTokensRead = 0;
    do
    {
        // read and pass the identifier.
        if ( !BscIsExpr( tokens + tokensRead, arrayTable, varTable, &subTokensRead ) )
        {
            tokensRead += subTokensRead;
            return 0;
        }
        tokensRead += subTokensRead;

        // got another expression!
        ++exprCount;

        if ( tokens[ tokensRead ].tokenId == BL_COMMA )
        {
            // pass the comma.
            ++tokensRead;
        }
        else
        {
            // not a comma, so break.
            break;
        }
    } while ( true );

    // return the expression count.
    return exprCount;
}

//----------------------------------------------------------
unsigned int
BscParseNamedExprList( URef< UExprTerm >* exprs, unsigned int maxExprCount, const char* name, const SBscToken* tokens,
                       const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
    // make sure the output array is valid and large enough!
    B_ASSERT( exprs != 0 );
    B_ASSERT( maxExprCount == BscIsNamedExprList( name, tokens, arrayTable, varTable ) && maxExprCount > 0 );

    // initialize tokensRead to 1 to account for the name.
    unsigned int tokensRead = 1;
    unsigned int exprIdx = 0;

    // add the first expression to the array passed in.
    tokensRead += BscParseExpr( exprs + exprIdx, tokens + tokensRead, arrayTable, varTable );
    ++exprIdx;

    // go through subsequent tokens, checking for a comma, then a value.
    while ( tokens[ tokensRead ].tokenId == BL_COMMA )
    {
        // advance past the comma.
        ++tokensRead;

        // make sure we have enough room for this expression!
        B_ASSERT( exprIdx < maxExprCount );

        tokensRead += BscParseExpr( exprs + exprIdx, tokens + tokensRead, arrayTable, varTable );
        ++exprIdx;
    }

    return tokensRead;
}


//**********************************************************
// named flag parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedFlag( const char* name, const SBscToken* tokens, const SFlagValue* flagValues,
                unsigned int flagValueCount, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );
    tokensRead = 0;
    if ( _stricmp( tokens[ tokensRead ].text.c_str(), name ) != 0 )
        return false;
    ++tokensRead;

    unsigned int flag = flagValueCount;
    while ( flag-- > 0 )
    {
        B_ASSERT( flagValues[ flag ].str != 0 );
        if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
        {
            ++tokensRead;
            return true;
        }
    }

    // not a valid flag.
    return false;
}

//----------------------------------------------------------
unsigned int
BscParseNamedFlag( unsigned int& value, const char* name, const SBscToken* tokens,
                   const SFlagValue* flagValues, unsigned int flagValueCount )
{
    // make we have a valid named flag to parse!
    B_ASSERT( BscIsNamedFlag( name, tokens, flagValues, flagValueCount ) );

    // initialize to zero.
    unsigned int tokensRead = 0;

    // advance past the name.
    ++tokensRead;
    unsigned int flag = flagValueCount;
    while ( flag-- > 0 )
    {
        if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
        {
            value = flagValues[ flag ].value;
            return ++tokensRead;
        }
    }

    // how is this possible?
    B_ASSERT( false );
    return 0;
}


//**********************************************************
// named flag list parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues,
                     unsigned int flagValueCount, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    tokensRead = 0;
    if ( _stricmp( tokens[ tokensRead ].text.c_str(), name ) != 0 )
        return false;
    ++tokensRead;

    bool flagFound = false;
    unsigned int flag = 0;
    do
    {
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            B_ASSERT( flagValues[ flag ].str != 0 );
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                flagFound = true;
                ++tokensRead;
                break;
            }
        }

        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
        {
            break;
        }
        ++tokensRead;
    } while ( true );

    // We accept any as long as there is more than one flag present.
    return flagFound;
}

//----------------------------------------------------------
unsigned int
BscParseNamedFlagList( unsigned int& value, const char* name, const SBscToken* tokens,
                       const SFlagValue* flagValues, unsigned int flagValueCount )
{
    // make sure we have a valid named flag list to parse!
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedFlagList( name, tokens, flagValues, flagValueCount ) );

    tokensRead = 1;

    value = 0;
    unsigned int flag = 0;
    do
    {
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            // 'or' in the value for this flag.
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                value |= flagValues[ flag ].value;
                ++tokensRead;

                // next flag.
                break;
            }
        }

        // skip the comma.
        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
            break;

        ++tokensRead;
    } while ( true );

    // return the combined flag values.
    return tokensRead;
}


//**********************************************************
// named ident-expr pair parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedIdentExpr( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
                     const BscVarTable& varTable, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

    tokensRead = 1;
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    // make sure we're followed by a named expression list.
    unsigned int subTokensRead = 0;
    bool isIdentExprList = BscIsNamedExpr( tokens[ tokensRead ].text.c_str(), tokens + tokensRead, arrayTable, varTable, &subTokensRead );
    tokensRead += subTokensRead;

    return isIdentExprList;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentExpr( tstring& ident, URef< UExprTerm >* expr, const char* name, const SBscToken* tokens,
                        const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
    B_ASSERT( expr != 0 );

    // make sure we have a valid named ident-expr pair to parse!
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedIdentExpr( name, tokens, arrayTable, varTable ) );

    // initialize to 1 ( to include the name ).
    tokensRead = 1;

    // read
    ident = tokens[ 1 ].text;
    tokensRead += BscParseNamedExpr( expr, ident.c_str(), tokens + 1, arrayTable, varTable );

    return tokensRead;
}


//**********************************************************
// named ident-expr list parser.
//**********************************************************

//----------------------------------------------------------
unsigned int
BscIsNamedIdentExprList( const char* name, const SBscToken* tokens, const BscArrayTable& arrayTable,
                         const BscVarTable& varTable, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );
    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

    SafeOffset tokensRead( tokensReadOut );

    tokensRead = 1;

    // note that BscIsNamedExprList will include the following token.
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    // make sure we're followed by a named expression list.
    unsigned int subTokensRead = 0;
    unsigned int exprCount = BscIsNamedExprList( tokens[ tokensRead ].text.c_str(), tokens + tokensRead, arrayTable, varTable, &subTokensRead );
    tokensRead += subTokensRead;

    return exprCount;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentExprList( tstring& ident, URef< UExprTerm >* exprs, unsigned int maxExprCount, const char* name,
                            const SBscToken* tokens, const BscArrayTable& arrayTable, const BscVarTable& varTable )
{
    B_ASSERT( exprs != 0 );

    unsigned int tokensRead = 0;
    B_ASSERT( maxExprCount == BscIsNamedIdentExprList( name, tokens, arrayTable, varTable ) && maxExprCount > 0 );

    // initialize to 1 ( to include the name ).
    tokensRead = 1;
    
    // get the identifier and the expression list.
    ident = tokens[ 1 ].text;
    tokensRead += BscParseNamedExprList( exprs, maxExprCount, ident.c_str(), tokens + 1, arrayTable, varTable );

    return tokensRead;
}


//**********************************************************
// named path parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedPath( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    // check to see if we have an identifier, followed by an equal sign, which
    // is followed by another identifier, a number, or a string.
    tokensRead = 0;
    if ( _stricmp( name, tokens[ tokensRead ].text.c_str() ) != 0 )
        return false;
    ++tokensRead;

//  if ( tokens[ tokensRead ].tokenId != BL_EQUALS )
//      return false;
//  ++tokensRead;

    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER && tokens[ tokensRead ].tokenId != BL_STRING &&
        tokens[ tokensRead ].tokenId != BL_NUMBER )
        return false;
    ++tokensRead;

    return true;
}

//----------------------------------------------------------
unsigned int
BscParseNamedPath( UPath& path, const char* name, const SBscToken* tokens )
{
    // make sure we have a valid named path.
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedPath( name, tokens ) );

    path = tokens[ 1 ].text;

    // account for the name.
    tokensRead = 2;
    return tokensRead;
}


//**********************************************************
// named identifier-path parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedIdentPath( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

    tokensRead = 1;
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    // make sure we're followed by a named path.
    unsigned int subTokensRead = 0;
    bool isIdentPath = BscIsNamedPath( tokens[ tokensRead ].text.c_str(), tokens + tokensRead, &subTokensRead );
    tokensRead += subTokensRead;

    return isIdentPath;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentPath( tstring& ident, UPath& path, const char* name, const SBscToken* tokens )
{
    // make sure we have a valid named ident-expr pair to parse!
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedIdentPath( name, tokens ) );

    // initialize to 1 ( to include the name ).
    tokensRead = 1;

    // read
    ident = tokens[ 1 ].text;
    tokensRead += BscParseNamedPath( path, ident.c_str(), tokens + 1 );

    return tokensRead;
}


//**********************************************************
// named identifier-path parser.
//**********************************************************

//----------------------------------------------------------
unsigned int
BscIsNamedIdentPathList( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

    tokensRead = 1;
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    // make sure we're followed by a named path.
    unsigned int subTokensRead = 0;
    unsigned int pathCount = BscIsNamedPathList( tokens[ tokensRead ].text.c_str(), tokens + tokensRead, &subTokensRead );
    tokensRead += subTokensRead;

    return pathCount;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentPathList( tstring& ident, UPath* paths, unsigned int maxPathCount, const char* name, const SBscToken* tokens )
{
    // make sure we have a valid named ident-expr pair to parse!
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedIdentPath( name, tokens ) );

    // initialize to 1 ( to include the name ).
    tokensRead = 1;

    // read
    ident = tokens[ 1 ].text;
    tokensRead += BscParseNamedPathList( paths, maxPathCount, ident.c_str(), tokens + 1 );

    return tokensRead;
}


//**********************************************************
// named identifier-path, flag list parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedIdentPathFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues, 
                             unsigned int flagValueCount, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    unsigned int subTokensRead = 0;
    bool isIdentPath = BscIsNamedIdentPath( name, tokens, &subTokensRead );
    if ( !isIdentPath )
        return false;

    SafeOffset tokensRead( tokensReadOut );
    tokensRead += subTokensRead;

    // check for any flags that are separated by a comma.
    bool flagFound = false;
    unsigned int flag = 0;
    do
    {

        // go through each flag and check for a match with the current token.
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            B_ASSERT( flagValues[ flag ].str != 0 );
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                flagFound = true;
                ++tokensRead;
                break;
            }
        }

        // if value of flag wrapped around, and a flag was found previously,
        // then there is either an erroneous comma, or an invalid flag.
        if ( flag == ( unsigned int )-1 && flagFound )
        {
            return false;
        }

        // any flags specified?
        if ( !flagFound )
            break;

        // a flag was specified, but if it isn't followed by a comma, then there are
        // no more flags.
        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
            break;

        // advance past the comma.
        ++tokensRead;
    } while ( true );

    return isIdentPath;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentPathFlagList( tstring& ident, UPath& path, unsigned int& flagValue, const char* name, const SBscToken* tokens,
                                const SFlagValue* flagValues, unsigned int flagValueCount )
{
    B_ASSERT( BscIsNamedIdentPathFlagList( name, tokens, flagValues, flagValueCount ) );

    // parse the named-ident-path part.
    unsigned int tokensRead = BscParseNamedIdentPath( ident, path, name, tokens );

    // check for any comma separated flags following the named-ident-path.
    bool flagFound = false;
    flagValue = 0;
    unsigned int flag = 0;
    do
    {
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            // 'or' in the value for this flag.
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                flagFound = true;
                flagValue |= flagValues[ flag ].value;
                ++tokensRead;

                // next flag.
                break;
            }
        }

        // make sure we found the flag, or there are no flags present.
        B_ASSERT( flag != ( unsigned int )-1 || flagFound != true );

        // if no flags found, then just break.
        if ( !flagFound )
            break;

        // do we have another flag?
        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
            break;

        // advance past the comma.
        ++tokensRead;
    } while ( true );

    // return the combined flag values.
    return tokensRead;
}


//**********************************************************
// named identifier-path list, flag list parser.
//**********************************************************

//----------------------------------------------------------
unsigned int
BscIsNamedIdentPathListFlagList( const char* name, const SBscToken* tokens, const SFlagValue* flagValues, 
                                 unsigned int flagValueCount, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    unsigned int subTokensRead = 0;
    unsigned int pathCount = BscIsNamedIdentPathList( name, tokens, &subTokensRead );
    if ( !pathCount )
        return 0;

    SafeOffset tokensRead( tokensReadOut );
    tokensRead += subTokensRead;

    // check for any flags that are separated by a comma.
    bool flagFound = false;
    unsigned int flag = 0;
    do
    {

        // go through each flag and check for a match with the current token.
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            B_ASSERT( flagValues[ flag ].str != 0 );
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                flagFound = true;
                ++tokensRead;
                break;
            }
        }

        // if value of flag wrapped around, and a flag was found previously,
        // then there is either an erroneous comma, or an invalid flag.
        if ( flag == ( unsigned int )-1 && flagFound )
            return 0;

        // any flags specified?
        if ( !flagFound )
            break;

        // a flag was specified, but if it isn't followed by a comma, then there are
        // no more flags.
        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
            break;

        // advance past the comma.
        ++tokensRead;
    } while ( true );

    return pathCount;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentPathListFlagList( tstring& ident, UPath* paths, unsigned int maxPathCount, unsigned int& flagValue,
                                    const char* name, const SBscToken* tokens, const SFlagValue* flagValues,
                                    unsigned int flagValueCount )
{
    B_ASSERT( BscIsNamedIdentPathListFlagList( name, tokens, flagValues, flagValueCount ) );

    // parse the named-ident-path part.
    unsigned int tokensRead = BscParseNamedIdentPathList( ident, paths, maxPathCount, name, tokens );

    // check for any comma separated flags following the named-ident-path.
    bool flagFound = false;
    flagValue = 0;
    unsigned int flag = 0;
    do
    {
        flag = flagValueCount;
        while ( flag-- > 0 )
        {
            // 'or' in the value for this flag.
            if ( _stricmp( tokens[ tokensRead ].text.c_str(), flagValues[ flag ].str ) == 0 )
            {
                flagFound = true;
                flagValue |= flagValues[ flag ].value;
                ++tokensRead;

                // next flag.
                break;
            }
        }

        // make sure we found the flag, or there are no flags present.
        B_ASSERT( flag != ( unsigned int )-1 || flagFound != true );

        // if no flags found, then just break.
        if ( !flagFound )
            break;

        // do we have another flag?
        if ( tokens[ tokensRead ].tokenId != BL_COMMA )
            break;

        // advance past the comma.
        ++tokensRead;
    } while ( true );

    // return the combined flag values.
    return tokensRead;
}


//**********************************************************
// named identifier pair parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedIdentPair( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;

    tokensRead = 1;
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    tokensRead = 2;
    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER )
        return false;

    return true;
}

//----------------------------------------------------------
unsigned int
BscParseNamedIdentPair( tstring& ident1, tstring& ident2, const char* name, const SBscToken* tokens )
{
    // make sure we have a valid named ident-expr pair to parse!
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedIdentPair( name, tokens ) );

    // read data.
    ident1 = tokens[ 1 ].text;
    ident2 = tokens[ 2 ].text;

    // name ident ident.
    return 3;
}


//**********************************************************
// named path list parser.
//**********************************************************

//----------------------------------------------------------
unsigned int
BscIsNamedPathList( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    B_ASSERT( tokens != 0 );

    SafeOffset tokensRead( tokensReadOut );

    // check to see if we have an identifier, followed by an equal sign, which
    // is followed by another identifier, a number, or a string.
    tokensRead = 0;
    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return 0;
    ++tokensRead;

//  if ( tokens[ tokensRead ].tokenId != BL_EQUALS )
//      return 0;
//  ++tokensRead;

    if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER && tokens[ tokensRead ].tokenId != BL_STRING &&
        tokens[ tokensRead ].tokenId != BL_NUMBER )
        return 0;
    ++tokensRead;

    // initialized to 1 to account for the first path.
    unsigned int pathCount = 1;

    // check all subsequent paths making sure they're paths.
    while ( tokens[ tokensRead ].tokenId == BL_COMMA )
    {
        // skip the comma.
        ++tokensRead;
        if ( tokens[ tokensRead ].tokenId != BL_IDENTIFIER && tokens[ tokensRead ].tokenId != BL_STRING &&
            tokens[ tokensRead ].tokenId != BL_NUMBER )
        {
            return 0;
        }

        // pass the identifier we just read.
        ++tokensRead;
        ++pathCount;
    }

    return pathCount;
}

//----------------------------------------------------------
unsigned int
BscParseNamedPathList( UPath* path, unsigned int maxPathCount, const char* name, const SBscToken* tokens )
{
    B_ASSERT( path != 0 );

    // make sure we have a valid named path list to parse.
    unsigned int tokensRead = 0;
    B_ASSERT( maxPathCount == BscIsNamedPathList( name, tokens ) && maxPathCount > 0 );

    // go through subsequent tokens, checking for a comma, then a value.
    tokensRead = 2;
    unsigned int pathIndex = 0;
    path[ pathIndex++ ] = tokens[ 1 ].text;
    while ( tokens[ tokensRead ].tokenId == BL_COMMA )
    {
        ++tokensRead;
        path[ pathIndex++ ] = tokens[ tokensRead++ ].text;
    }

    return tokensRead;
}

//**********************************************************
// named string parser.
//**********************************************************

//----------------------------------------------------------
bool
BscIsNamedString( const char* name, const SBscToken* tokens, unsigned int* tokensReadOut )
{
    SafeOffset tokensRead( tokensReadOut );

    // check to see if we have an identifier, followed by an equal sign, which
    // is followed by another identifier, a number, or a string.
    tokensRead = 0;
    if ( _stricmp( name, tokens[ 0 ].text.c_str() ) != 0 )
        return false;
    ++tokensRead;

    if ( tokens[ 1 ].tokenId != BL_EQUALS )
        return false;
    ++tokensRead;

    if ( tokens[ 2 ].tokenId != BL_IDENTIFIER && tokens[ 2 ].tokenId != BL_STRING &&
        tokens[ 2 ].tokenId != BL_NUMBER )
        return false;
    ++tokensRead;

    return true;
}

//----------------------------------------------------------
unsigned int
BscParseNamedString( tstring& str, const char* name, const SBscToken* tokens )
{
    // make sure we have a valid string to parse.
    unsigned int tokensRead = 0;
    B_ASSERT( BscIsNamedString( name, tokens ) );

    // set str to the text.
    str = tokens[ 2 ].text;

    // account for the name and the assignment operator.
    return 3;
}
