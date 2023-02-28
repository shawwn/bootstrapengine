//----------------------------------------------------------
// File:        BscExpr.h
// Author:      Kevin Bray
// Created:     07-01-05
//
// Purpose:     A library for parsing a mathematical expression.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
struct SBscToken;
class BscArrayTable;
class BscVarTable;
class UExpr;
class UExprTerm;
class UExprNumber;
class UExprArrayRef;
class UExprCallback;
class UExprArray;

//**********************************************************
// function declarations
//**********************************************************
extern bool             BscIsExpr( const SBscToken* tokens, const BscArrayTable& arrayTable,
                                   const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int     BscParseExpr( URef< UExprTerm >* exprOut, const SBscToken* tokens,
                                      const BscArrayTable& arrayTable, const BscVarTable& varTable );

extern bool             BscIsNumber( const SBscToken* tokens, unsigned int* tokensRead = 0 );
extern unsigned int     BscParseNumber( URef< UExprNumber >* exprNum, const SBscToken* tokens );

extern bool             BscIsArrayRef( const SBscToken* tokens, const BscArrayTable& arrayTable,
                                       const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int     BscParseArrayRef( URef< UExprArrayRef >* arrayRef, const SBscToken* tokens,
                                          const BscArrayTable& arrayTable, const BscVarTable& varTable );

// callbacks must be smart pointers because they are registered with a table!
// The only other option is multiple 4 byte allocations all over the place.  ACK.
extern bool             BscIsCallback( const SBscToken* tokens, const BscVarTable& callbackTable,
                                       unsigned int* tokensRead = 0 );
extern unsigned int     BscParseCallback( URef< UExprCallback >* callback, const SBscToken* tokens,
                                          const BscVarTable& callbackTable );

extern bool             BscIsArray( const SBscToken* tokens, const BscArrayTable& arrayTable,
                                    const BscVarTable& varTable, unsigned int* tokensRead = 0 );
extern unsigned int     BscParseArray( UExprArray** exprArray, tstring& name, const SBscToken* tokens,
                                       const BscArrayTable& arrayTable, const BscVarTable& varTable );


// Internal use of the parsing lib.  Do not use.
class SafeOffset
{
public:
    SafeOffset( unsigned int* offset ) : _offset( offset ), _scratch( 0 ) { }

//  SafeOffset& operator ++ ( int )             { if ( _offset ) ++( *_offset );    else ++_scratch;    return *this;       }
    SafeOffset& operator ++ ()                  { if ( _offset ) ++( *_offset );    else ++_scratch;    return *this;       }
    SafeOffset& operator += ( unsigned int i )  { if ( _offset ) ( *_offset ) += i; else _scratch += i; return *this;       }
    SafeOffset& operator = ( unsigned int i )   { if ( _offset ) ( *_offset ) = i;  else _scratch = i;  return *this;       }

    operator unsigned int ()                    { if ( _offset ) return *_offset; else return _scratch; }

private:
    unsigned int*   _offset;
    unsigned int    _scratch;
};
