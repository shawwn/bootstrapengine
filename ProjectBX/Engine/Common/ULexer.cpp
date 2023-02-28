//----------------------------------------------------------
// File:        ULexer.cpp
// Author:      Kevin Bray
// Created:     04-13-04
// Copyright © 2004 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "ULexer.h"

// project includes.


//**********************************************************
// class ULexer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ULexer::ULexer()
: _tokenStream( 0 )
{

}

//----------------------------------------------------------
ULexer::~ULexer()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
ULexer::AddKeyword( const tstring& pattern, int tokenId )
{
    // the token match ID must be greater than 0!
    B_ASSERT( tokenId >= 0 && tokenId < 0x70000000 );

    // add the pattern and it's match ID to the appropriate
    // vector.
    _keywordVec.push_back( pattern );
    _keywordTokenIdVec.push_back( tokenId );
}

//----------------------------------------------------------
bool
ULexer::Lex( const char *text, unsigned char eos )
{
    // reset lexer information.
    _tokenStream.clear();
    _errors.clear();

    unsigned int lineNum = 1;

    // get the current character.
    unsigned char ch = text[ 0 ];
    if ( ch == eos )
    {
        // nothing to lex, but not an error.
        _tokenStream.push_back( SToken( UL_EOS, "", 0 ) );
        return true;
    }

    // get the look-ahead character.
    unsigned char la = text[ 1 ];

    // for each character.
    unsigned long offset = 1;
    while ( ch != eos )
    {
        // check to see if this is a symbol.
        switch( ch )
        {
        // ignore whitespace.
        case '\n':
            ++lineNum;
        case ' ':
        case '\r':
        case '\t':
            break;

        // operators...
        case '=':
            // equality?
            if ( la == '=' )
            {   // ==
                la = text[ ++offset ];  // skip next character.
                _tokenStream.push_back( SToken( UL_ISEQUAL, "==", lineNum ) );
            }
            else // just an equal sign
            {   // =
                _tokenStream.push_back( SToken( UL_EQUAL, "=", lineNum ) );
            }
            break;

        case '+':
            switch( la )
            {
            case '+':
                {   // ++
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_PLUSPLUS, "++", lineNum ) );
                }
                break;

            case '=':
                {   // +=
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_PLUSEQUAL, "+=", lineNum ) );
                }
                break;

            default:
                {   // +
                    _tokenStream.push_back( SToken( UL_PLUS, "+", lineNum ) );
                }
                break;
            };
            break;

        case '-':
            switch( la )
            {
            case '-':
                {   // --
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_MINUSMINUS, "--", lineNum ) );
                }
                break;

            case '=':
                {   // -=
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_MINUSEQUAL, "-=", lineNum ) );
                }
                break;

            case '>':
                {   // ->
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_INDIRECTION, "->", lineNum ) );
                }
                break;

            default:
                {   // -
                    _tokenStream.push_back( SToken( UL_MINUS, "-", lineNum ) );
                }
                break;
            };
            break;

        case '*':
            if ( la == '=' )
            {   // *=
                la = text[ ++offset ];  // skip next character.
                _tokenStream.push_back( SToken( UL_MULTIPLYEQUAL, "*=", lineNum ) );
            }
            else
            {
                _tokenStream.push_back( SToken( UL_ASTERISK, "*", lineNum ) );
            }
            break;

        case '/':
            switch( la )
            {   
            case '=':
                {   // /=
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_DIVIDEEQUAL, "/=", lineNum ) );
                }
                break;

            case '/':
                {   // //
                    // scan until we hit a newline.
                    do
                    {
                        ch = la;

                        // if we hit an eos marker, we're done parsing.
                        if ( ch == eos )
                            return( _errors.size() == 0 );

                        la = text[ ++offset ];
                    }
                    while ( ch != '\n' );
                    ++lineNum;
                }
                break;

            case '*':
                {   // /*
                    // scan until we hit */
                    ch = la;
                    la = text[ ++offset ];
                    do
                    {
                        ch = la;
                        if ( ch == eos )
                        {
                            // finished parsing in the middle of a
                            // block comment!  Note an error.
                            _errors.push_back( "Lexer error: end of stream found before end of block comment!\n" );
                            return false;
                        }
                        if ( ch == '\n' )
                            ++lineNum;

                        la = text[ ++offset ];
                    }
                    while ( !( ch == '*' && la == '/' ) );

                    // skip next character.
                    la = text[ ++offset ];
                }
                break;

            default:
                {   // /
                    _tokenStream.push_back( SToken( UL_SLASH, "/", lineNum ) );
                }
                break;
            };
            break;

        case '%':
            if ( la == '=' )
            {   // %=
                la = text[ ++offset ];  // skip next character.
                _tokenStream.push_back( SToken( UL_PERCENTEQUAL, "%=", lineNum ) );
            }
            else
            {   // %
                _tokenStream.push_back( SToken( UL_PERCENT, "%", lineNum ) );
            }
            break;

        case '!':
            if ( la == '=' )
            {   // !=
                la = text[ ++offset ];  // skip next character.
                _tokenStream.push_back( SToken( UL_NOTEQUAL, "!=", lineNum ) );
            }
            else
            {   // !
                _tokenStream.push_back( SToken( UL_EXCLAMATION, "!", lineNum ) );
            }
            break;

        case '[':
            {
                _tokenStream.push_back( SToken( UL_LSQUAREBRACE, "[ ", lineNum ) );
            }
            break;

        case ']':
            {
                _tokenStream.push_back( SToken( UL_RSQUAREBRACE, " ]", lineNum ) );
            }
            break;

        case '(':
            {
                _tokenStream.push_back( SToken( UL_LPAREN, "(", lineNum ) );
            }
            break;

        case ')':
            {
                _tokenStream.push_back( SToken( UL_RPAREN, ")", lineNum ) );
            }
            break;

        case '{':
            {
                _tokenStream.push_back( SToken( UL_LBRACE, "{", lineNum ) );
            }
            break;

        case '}':
            {
                _tokenStream.push_back( SToken( UL_RBRACE, "}", lineNum ) );
            }
            break;

        case '&':
            {
                _tokenStream.push_back( SToken( UL_AND, "&", lineNum ) );
            }
            break;

        case '|':
            {
                _tokenStream.push_back( SToken( UL_OR, "|", lineNum ) );
            }
            break;

        case '<':
            switch( la )
            {
            case '=':
                {   // <=
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_LESSEQUAL, "<=", lineNum ) );
                }
                break;

            case '<':
                {   // <<
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_LSHIFT, "<<", lineNum ) );
                }
                break;

            default:
                {   // <
                    _tokenStream.push_back( SToken( UL_LESS, "<", lineNum ) );
                }
                break;
            };
            break;

        case '>':
            switch( la )
            {
            case '=':
                {   // >=
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_GREATEREQUAL, ">=", lineNum ) );
                }
                break;

            case '>':
                {   // >>
                    la = text[ ++offset ];  // skip next character.
                    _tokenStream.push_back( SToken( UL_RSHIFT, ">>", lineNum ) );
                }
                break;

            default:
                {   // >
                    _tokenStream.push_back( SToken( UL_GREATER, ">", lineNum ) );
                }
                break;
            };
            break;

        case ',':
            {
                _tokenStream.push_back( SToken( UL_COMMA, ",", lineNum ) );
            }
            break;

        case '.':
            {
                _tokenStream.push_back( SToken( UL_PERIOD, ".", lineNum ) );
            }
            break;

        case ';':
            {
                _tokenStream.push_back( SToken( UL_SEMICOLON, ";", lineNum ) );
            }
            break;

        case '\'':
        case '"':
            // handle a character sequence or a string ( they're almost
            // the same to the lexer ).
            {
                // scan until we hit '
                char endChar = ch;
                SToken newToken( ch == '\'' ? UL_CHAR : UL_STRING, "", lineNum );
                if ( la != endChar )
                {
                    do
                    {
                        // process the next character.
                        ch = la;
                        la = text[ ++offset ];

                        // check to see if the next two characters form
                        // an escape sequence.
                        if ( ch == '\\' )
                        {
                            // make sure we don't hit the end of the stream
                            // in the middle of an escape sequence.
                            if ( la == eos )
                            {
                                _errors.push_back( "Lexer error: end of stream encounted during escape sequence!\n" );
                                return false;
                            }

                            // handle the escape sequence ( \n \t \\ \' \" ).
                            switch( la )
                            {
                            case 'n':
                                newToken.text.append( 1, '\n' );
                                break;
                            case 't':
                                newToken.text.append( 1, '\t' );
                                break;
                            case '\\':
                                newToken.text.append( 1, '\\' );
                                break;
                            case '\'':
                                newToken.text.append( 1, '\'' );
                                break;
                            case '"':
                                newToken.text.append( 1, '\"' );
                                break;
                            default:
                                tstring error;
                                SPrintF( error, "Lexer error, line %d: unknown escape sequence encountered!\n", lineNum );
                                _errors.push_back( error );
                                break;
                            };

                            // advance past the escape sequence.
                            la = text[ ++offset ];
                        }
                        else
                        {
                            // if check to see if we've hit the end of the
                            // stream.
                            if ( ch == eos )
                            {
                                // finished parsing in the middle of a
                                // block comment!  Note an error.
                                _errors.push_back( "Lexer error: end of stream found before end of string or character sequence!\n" );
                                return false;
                            }

                            // check for a newline in the string or character
                            // sequence.  If we find one, post an error and
                            // simply ignore it.
                            if ( ch == '\n' || ch == '\r' )
                            {
                                // increment the current line number.
                                ++lineNum;
                                if ( la == '\r' )
                                {
                                    // skip.
                                    la = text[ ++offset ];
                                }

                                tstring error;
                                SPrintF( error, "Lexer error, line %d: newline found in string or character sequence!\n", lineNum );
                                _errors.push_back( error );
                                continue;
                            }

                            // add the current character to the sequence.
                            newToken.text.append( 1, ch );
                        }
                    }
                    while ( la != endChar );
                }

                // skip next character.
                la = text[ ++offset ];

                // add the new token to the stream.
                _tokenStream.push_back( newToken );
            }
            break;

        default:
            {
                // parse the following as a string.  Parse until we hit
                // a character that is not alpha-numeric or an underscore.
                if ( ch >= '0' && ch <= '9' )
                {
                    SToken newToken( UL_NUMBER, "", lineNum );
                    newToken.text.append( 1, ch );

                    // fetch chars as long as they are numeric.
                    while ( la >= '0' && la <= '9' )
                    {
                        // process the next character.
                        ch = la;

                        // add the current character to the sequence.
                        newToken.text.append( 1, ch );

                        // advance to the next character.
                        la = text[ ++offset ];
                    }

                    // add the new token to the stream.
                    _tokenStream.push_back( newToken );
                }
                else if ( isalpha( ch ) || ch == '_' )
                {
                    // check to see if this is a number or an identifier.
                    // To the lexer, a number is simply an identifier whose
                    // first character is a number.
                    SToken newToken( UL_IDENTIFIER, "", lineNum );
                    newToken.text.append( 1, ch );

                    // fetch chars as long as they are alpha-numeric or
                    // are an underscore.
                    while ( isalnum( la ) || la == '_' )
                    {
                        // process the next character.
                        ch = la;

                        // add the current character to the sequence.
                        newToken.text.append( 1, ch );

                        // advance to the next character.
                        la = text[ ++offset ];
                    }

                    // add the new token to the stream.
                    _tokenStream.push_back( newToken );
                }
                else
                {
                    // invalid character!  ignore.
                    tstring error;
                    SPrintF( error, "Lexer error, line %d: invalid character!\n", lineNum );
                    _errors.push_back( error );
                }

                // check to see if this is a keyword.

                // check to see if this is an identifier.
            }
        };

        // set ch to the next character.
        ch = la;

        // get the next character.
        if ( la != eos )
        {
            la = text[ ++offset ];
        }
    }

    // signal the end of the stream.
    _tokenStream.push_back( SToken( UL_EOS, "", lineNum ) );

    // return true if no errors.
    return( _errors.size() == 0 );
}
