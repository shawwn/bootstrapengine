//----------------------------------------------------------
// File:		ULexer.h
// Author:		Kevin Bray
// Created:		04-13-04
//
// Purpose:		A C-like lexer to tokenize text.
//
// Copyright © 2004 Bootstrap Studios
//----------------------------------------------------------
#pragma once

#include <vector>

// identifier
#define UL_IDENTIFIER		0x70000000

// literals
#define UL_NUMBER			0x70000001
#define UL_STRING			0x70000002	/* "" */
#define UL_CHAR				0x70000003	/* '' */

// mathematical operations
#define UL_EQUAL			0x70000010	/* =  */
#define UL_PLUS				0x70000011	/* +  */
#define UL_MINUS			0x70000012	/* -  */
#define UL_ASTERISK			0x70000013	/* *  */
#define UL_SLASH			0x70000014	/* /  */
#define UL_PERCENT			0x70000015	/* %  */
#define UL_EXCLAMATION		0x70000016	/* !  */
#define UL_AND				0x70000017	/* &  */
#define UL_OR				0x70000018	/* |  */
#define UL_LSHIFT			0x70000019	/* << */
#define UL_RSHIFT			0x7000001A	/* >> */
#define UL_PLUSPLUS			0x7000001B	/* ++ */
#define UL_MINUSMINUS		0x7000001C	/* -- */
#define UL_PLUSEQUAL		0x7000001D	/* += */
#define UL_MINUSEQUAL		0x7000001E	/* -= */
#define UL_MULTIPLYEQUAL	0x7000001F	/* *= */
#define UL_DIVIDEEQUAL		0x70000020	/* /= */
#define UL_PERCENTEQUAL		0x70000021	/* %= */

// grouping
#define UL_LSQUAREBRACE		0x70000030	/* [  */
#define UL_RSQUAREBRACE		0x70000031	/* ]  */
#define UL_LPAREN			0x70000032	/* ( */
#define UL_RPAREN			0x70000033	/* )  */
#define UL_LBRACE			0x70000034	/* {  */
#define UL_RBRACE			0x70000035	/* }  */

// equality
#define UL_LESS				0x70000040	/* <  */
#define UL_GREATER			0x70000041	/* >  */
#define UL_LESSEQUAL		0x70000042	/* <= */
#define UL_GREATEREQUAL		0x70000043	/* >= */
#define UL_NOTEQUAL			0x70000044	/* != */
#define UL_ISEQUAL			0x70000045	/* == */

// punctuation
#define UL_COMMA			0x70000050	/* ,  */
#define UL_PERIOD			0x70000051	/* .  */
#define UL_SEMICOLON		0x70000052	/* ;  */

// indirection operator
#define UL_INDIRECTION		0x70000060	/* -> */

// end of a token stream.
#define UL_EOS				0x700000FF

struct SToken
{
	// constructors ( for convenience )
	SToken() : tokenId( -1 ) { }
	SToken( int id, const tstring& txt, unsigned int line )
		: tokenId( id ), text( txt ), lineNum( line ) { }

	// data members.
	int				tokenId;	// UL_nnn
	tstring			text;		// text that it was matched to.
	unsigned int	lineNum;	// line number.
};

//**********************************************************
// class ULexer
//**********************************************************
class ULexer
{
public:
	ULexer();
	~ULexer();

	// adds a keyword, matchId can be 0 - 0x6FFFFFFF.
	void			AddKeyword( const tstring& pattern, int tokenId );

	// matches patterns and builds the token stream.  Note that the
	// token stream is invalidated each time Lex is called.
	bool			Lex( const char *text, unsigned char eos = '\0' );

	// the end of the token stream is marked by UL_EOS.
	const SToken*	GetTokenStream() const				{	return &_tokenStream.front();	}
	const std::vector< tstring >& GetErrorVec() const	{	return _errors;					}

private:
	typedef std::vector< tstring > KeywordVector;
	typedef std::vector< int > TokenIDVector;
	typedef std::vector< SToken > TokenVector;
	typedef std::vector< tstring > ErrorVector;

	// the token stream.  This gets rebuilt every time Lex() is
	// called.
	KeywordVector	_keywordVec;
	TokenIDVector	_keywordTokenIdVec;
	TokenVector		_tokenStream;

	// note that any error information is cleared everytime Lex()
	// is called.
	ErrorVector		_errors;			// error message
};
