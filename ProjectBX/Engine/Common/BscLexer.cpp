//----------------------------------------------------------
// File:        BscLexer.cpp
// Author:      Kevin Bray
// Created:     06-30-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "BscLexer.h"

// project includes.


//**********************************************************
// class BscLexer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
BscLexer::BscLexer()
{

}

//----------------------------------------------------------
BscLexer::~BscLexer()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
BscLexer::Lex( const char *text, unsigned char eos )
{
    // reset lexer information.
    _tokenStream.clear();
    _errors.clear();

    unsigned int lineNum = 1;

    // get the current character.
    unsigned int chIndex = 0;
    unsigned char ch = text[ chIndex ];
    if ( ch == eos )
    {
        // nothing to lex, but not an error.
        _tokenStream.push_back( SBscToken( BL_EOS, "", 0 ) );
        return true;
    }

    // get the look-ahead character.
    unsigned char la = text[ chIndex + 1 ];

    // for each character, lex!
    while ( ch != eos )
    {
        switch ( ch )
        {
        case '"':
            {
                ++chIndex;
                ch = la;
                la = text[ chIndex + 1 ];
                tstring str;
                while ( ch != '"' && ch != eos )
                {
                    str += ch;

                    ++chIndex;
                    ch = la;
                    la = text[ chIndex + 1 ];
                }
                _tokenStream.push_back( SBscToken( BL_STRING, str, lineNum ) );
            }
            break;

        case '\'':
            {
                ++chIndex;
                ch = la;
                la = text[ chIndex + 1 ];
                char buf[ 5 ] = { 0, 0, 0, 0, 0 };
                unsigned int i = 0;
                while ( ch != '\'' && i < 4 && ch != eos )
                {
                    buf[ i ] = ch;

                    ++chIndex;
                    ch = la;
                    la = text[ chIndex + 1 ];
            
                    ++i;
                }
                _tokenStream.push_back( SBscToken( BL_CHAR, buf, lineNum ) );
            }
            break;


        case '+':
            _tokenStream.push_back( SBscToken( BL_PLUS, "+", lineNum ) );
            break;

        case '-':
            _tokenStream.push_back( SBscToken( BL_MINUS, "-", lineNum ) );
            break;

        case '*':
            _tokenStream.push_back( SBscToken( BL_ASTERISK, "*", lineNum ) );
            break;

        case '/':
            if ( la == '/' )
            {
                // skip until we hit a newline.
                while ( ch != '\n' )
                {
                    ++chIndex;
                    ch = la;
                    la = text[ chIndex + 1 ];
                    if ( la == eos )
                        break;
                }

                // don't increment the character index.
                continue;
            }
            else if ( la == '*' )
            {
                // advance past the incoming /*
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];

                // since we're handling the look-ahead, skip it on the next iteration.
                while ( ( ch != '*' || la != '/' ) && ch != eos )
                {
                    if ( ch == '\n' )
                        ++lineNum;

                    ++chIndex;
                    ch = la;
                    if ( ch != eos )
                        la = text[ chIndex + 1 ];
                }

                // skip the trailing '*/' and continue.
                if ( ch != eos )
                {
                    chIndex += 2;
                    ch = text[ chIndex ];
                    if ( ch != eos )
                        la = text[ chIndex + 1 ];
                }

                // don't increment the character index.
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_SLASH, "/", lineNum ) );
            }
            break;

        case '%':
            if ( la == '%' )
            {
                // we have an 'anything goes except %% string'.
                // advance past the incoming %%
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];

                // since we're handling the look-ahead, skip it on the next iteration.
                tstring str;
                while ( ( ch != '%' || la != '%' ) && ch != eos )
                {
                    str += ch;
                    ++chIndex;
                    ch = la;
                    if ( ch != eos )
                        la = text[ chIndex + 1 ];
                }

                // skip the trailing '%%' and continue.
                if ( ch != eos )
                {
                    chIndex += 2;
                    ch = text[ chIndex ];
                    if ( ch != eos )
                        la = text[ chIndex + 1 ];
                }

                // add the new string.
                _tokenStream.push_back( SBscToken( BL_STRING, str, lineNum ) );

                // don't increment the character index.
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_PERCENT, "%", lineNum ) );
            }
            break;


        case '[':
            _tokenStream.push_back( SBscToken( BL_LSQUAREBRACE, "[ ", lineNum ) );
            break;

        case ']':
            _tokenStream.push_back( SBscToken( BL_RSQUAREBRACE, " ]", lineNum ) );
            break;

        case '(':
            _tokenStream.push_back( SBscToken( BL_LPAREN, "(", lineNum ) );
            break;

        case ')':
            _tokenStream.push_back( SBscToken( BL_RPAREN, ")", lineNum ) );
            break;

        case '{':
            _tokenStream.push_back( SBscToken( BL_LBRACE, "{", lineNum ) );
            break;

        case '}':
            _tokenStream.push_back( SBscToken( BL_RBRACE, "}", lineNum ) );
            break;


        case '<':
            if ( la == '=' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_LESSEQUAL, "<=", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_LESS, "<", lineNum ) );
            }
            break;

        case '>':
            if ( la == '=' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_GREATEREQUAL, ">=", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_GREATER, ">", lineNum ) );
            }
            break;

        case '!':
            if ( la == '=' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_NOTEQUAL, "!=", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_NOT, "!", lineNum ) );
            }
            break;

        case '=':
            if ( la == '=' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_ISEQUAL, "==", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_EQUALS, "=", lineNum ) );
            }
            break;

        case '&':
            if ( la == '&' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_LOGICALAND, "&&", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_AND, "&", lineNum ) );
            }
            break;

        case '|':
            if ( la == '|' )
            {
                // since we're handling the look-ahead, skip it on the next iteration.
                _tokenStream.push_back( SBscToken( BL_LOGICALOR, "||", lineNum ) );
                chIndex += 2;
                ch = text[ chIndex ];
                if ( ch != eos )
                    la = text[ chIndex + 1 ];
                continue;
            }
            else
            {
                _tokenStream.push_back( SBscToken( BL_OR, "|", lineNum ) );
            }
            break;

        case ',':
            _tokenStream.push_back( SBscToken( BL_COMMA, ",", lineNum ) );
            break;

        case ';':
            _tokenStream.push_back( SBscToken( BL_SEMICOLON, ";", lineNum ) );
            break;

        case ':':
            _tokenStream.push_back( SBscToken( BL_COLON, ":", lineNum ) );
            break;

        case '.':
            _tokenStream.push_back( SBscToken( BL_PERIOD, ".", lineNum ) );
            break;


        case '\n':
            // eat whitespace.
            ++lineNum;
        case ' ':
        case '\r':
        case '\t':
            // ignore.
            break;

        default:
            {
                tstring str;

                // we have an identifier.
                if ( isalpha( ch ) || ch == '_' || ch == '\\' )
                {
                    while ( ( isalnum( ch ) || ch == '_' || ch == '\\' || ch == '/' || ch == '.' || ch == ':' ) && ch != eos )
                    {
                        // note that we treat numerical operators as part of identifers.  This makes
                        // handling file names easier.  If this is undesired, it's up to the parser
                        // to error as needed.  That means that numerical operators must be split up
                        // as well.
                        str += ch;

                        // advance.
                        ++chIndex;
                        ch = la;
                        if ( ch != eos )
                            la = text[ chIndex + 1 ];
                    }
                    _tokenStream.push_back( SBscToken( BL_IDENTIFIER, str, lineNum ) );
                }
                else if ( isdigit( ch ) )
                {
                    while ( ( isalnum( ch ) || ch == '.' ) && ch != eos )
                    {
                        // note that we treat numerical operators as part of identifers.  This makes
                        // handling file names easier.  If this is undesired, it's up to the parser
                        // to error as needed.  That means that numerical operators must be split up
                        // as well.
                        str += ch;

                        // advance.
                        ++chIndex;
                        ch = la;
                        if ( ch != eos )
                            la = text[ chIndex + 1 ];
                    }
                    _tokenStream.push_back( SBscToken( BL_NUMBER, str, lineNum ) );
                }
                else
                {
//                  B_ASSERT( false );
                    ++chIndex;
                    ch = la;
                    if ( ch != eos )
                        la = text[ chIndex + 1 ];
                }
                continue;
            }
            break;

        };
        ++chIndex;
        ch = la;
        if ( ch != eos )
            la = text[ chIndex + 1 ];
    }

    // signal the end of the stream.
    _tokenStream.push_back( SBscToken( BL_EOS, "", lineNum ) );

    // return true if no errors.
    return( _errors.size() == 0 );
}
