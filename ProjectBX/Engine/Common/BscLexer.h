//----------------------------------------------------------
// File:        BscLexer.h
// Author:      Kevin Bray
// Created:     06-30-05
//
// Purpose:     A lexer specifically for BscScript.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include <vector>

// note that we treat numerical operators as part of identifers.  This makes
// handling file names easier.  If this is undesired, it's up to the parser
// to error as needed.  That means that numerical operators must be split up
// as well.
// identifier
#define BL_IDENTIFIER       0x70000000

// literals
#define BL_NUMBER           0x70000001
#define BL_STRING           0x70000002  /* "" */
#define BL_CHAR             0x70000003  /* '' */

// mathematical operators
#define BL_PLUS             0x70000011  /* +  */
#define BL_MINUS            0x70000012  /* -  */
#define BL_ASTERISK         0x70000013  /* *  */
#define BL_SLASH            0x70000014  /* /  */
#define BL_PERCENT          0x70000015  /* %  */
#define BL_EQUALS           0x70000016  /* =  */
#define BL_AND              0x70000017  /* &  */
#define BL_OR               0x70000018  /* |  */

// grouping
#define BL_LSQUAREBRACE     0x70000030  /* [  */
#define BL_RSQUAREBRACE     0x70000031  /* ]  */
#define BL_LPAREN           0x70000032  /* ( */
#define BL_RPAREN           0x70000033  /* )  */
#define BL_LBRACE           0x70000034  /* {  */
#define BL_RBRACE           0x70000035  /* }  */

// equality
#define BL_LESS             0x70000040  /* <  */
#define BL_GREATER          0x70000041  /* >  */
#define BL_LESSEQUAL        0x70000042  /* <= */
#define BL_GREATEREQUAL     0x70000043  /* >= */
#define BL_NOT              0x70000045  /* !  */
#define BL_NOTEQUAL         0x70000044  /* != */
#define BL_ISEQUAL          0x70000046  /* == */
#define BL_LOGICALAND       0x70000047  /* && */
#define BL_LOGICALOR        0x70000048  /* || */

// punctuation
#define BL_COMMA            0x70000050  /* ,  */
#define BL_SEMICOLON        0x70000051  /* ;  */
#define BL_COLON            0x70000052  /* :  */
#define BL_PERIOD           0x70000053  /* .  */

// new line
//#define BL_NEWLINE            0x70000060

// text
#define BL_TEXT             0x70000070 

// end of a token stream.
#define BL_EOS              0x700000FF

struct SBscToken
{
    // constructors ( for convenience )
    SBscToken() : tokenId( -1 ) { }
    SBscToken( int id, const tstring& txt, unsigned int line )
        : tokenId( id ), text( txt ), lineNum( line ) { }

    // data members.
    int             tokenId;    // UL_nnn
    tstring         text;       // text that it was matched to.
    unsigned int    lineNum;    // line number.
};

//**********************************************************
// class BscLexer
//**********************************************************
class BscLexer
{
public:
    typedef std::vector< tstring > ErrorVector;

    BscLexer();
    ~BscLexer();

    // matches patterns and builds the token stream.  Note that the
    // token stream is invalidated each time Lex is called.
    bool                Lex( const char *text, unsigned char eos = '\0' );

    // the end of the token stream is marked by UL_EOS.
    const SBscToken*    GetTokenStream() const              {   return &_tokenStream.front();   }
    const ErrorVector&  GetErrorVec() const                 {   return _errors;                 }

private:
    typedef std::vector< tstring > KeywordVector;
    typedef std::vector< int > TokenIDVector;
    typedef std::vector< SBscToken > TokenVector;

    // the token stream.  This gets rebuilt every time Lex() is
    // called.
    KeywordVector   _keywordVec;
    TokenIDVector   _keywordTokenIdVec;
    TokenVector     _tokenStream;

    // note that any error information is cleared everytime Lex()
    // is called.
    ErrorVector     _errors;            // error message
};
