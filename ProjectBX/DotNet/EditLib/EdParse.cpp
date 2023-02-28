//----------------------------------------------------------
// File:        EdParse.cpp
// Author:      Kevin Bray
// Created:     05-17-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// lib header.
#include "EdParse.h"

// project includes.
#include "EdBrushMgr.h"
#include "EdBrush.h"
#include "EdBrushImage.h"
#include "EdBrushPaletteMgr.h"
#include "EdBrushPalette.h"

// engine includes.
#include "BscLexer.h"
#include "BscExprLib.h"

// inline helper function.
static bool IsTok( const SBscToken* tok, const char* str )  {   return ( _stricmp( tok->text.c_str(), str ) == 0 ); }

static URef< EdBrushImage > ParseImageRef( const SBscToken*& curTok, tstring& errors );
static URef< EdBrush > ParseBrushRef( const SBscToken*& curTok, tstring& errors );

//**********************************************************
// GrParse implementations.
//**********************************************************

//==========================================================
// global functions.
//==========================================================

//----------------------------------------------------------
bool ParseBrush( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // stage lookup.
    static const char* stageLookup[] = {
        "bump",
        "normal",
        "diffuse",
        "specular",
        "emissive",
        "ambient",
        "user",
    };

    // parse the name.
    tstring name = curTok->text;
    ++curTok;

    // make sure the current token is the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    EdBrush::SInfo info;
    info.vertexProgram = 0;
    info.fragmentProgram = 0;

    // read until the end of the brush.
    while ( !IsTok( curTok, "}" ) )
    {
        // Do we have a mask texture?
        if ( IsTok( curTok, "mask" ) )
        {
            // advance past the current token.
            ++curTok;

            // get the image.
            info.mask = ParseImageRef( curTok, errors );
        }
        else
        {
            // operate on the current token.
            int i = 0;
            for ( ; i < 8; ++i )
                if ( IsTok( curTok, stageLookup[ i ] ) )
                    break;

            // make sure we have a stage texture.
            if ( i > ES_COUNT )
            {
                errors = "Unrecognized stage specifier \"";
                errors += curTok->text;
                errors += "\" found!";
                return false;
            }

            // get the stage and advance past the current token.
            ESTAGE stage = ( ESTAGE )i;
            ++curTok;

            // get the name of the image.
            info.images[ stage ] = ParseImageRef( curTok, errors );
        }
    }

    // advance past the closing brace.
    ++curTok;

    // build the brush.
    gEdBrushMgr->SetBrush( name, info );

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool ParseBrushPalette( const SBscToken*& curTok, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors )
{
    // parse the name.
    tstring name = curTok->text;
    ++curTok;

    // make sure the current token is the opening brace.
    if ( !IsTok( curTok, "{" ) )
    {
        errors = "Missing opening brace!";
        return false;
    }
    ++curTok;

    // brush vector to put all of the brushes in.
    EdBrushPaletteMgr::BrushVec brushes;

    // read until the end of the brush palette.
    while ( !IsTok( curTok, "}" ) )
    {
        // get the brush.  Note that we make this error tolerant.
        URef< EdBrush > brush = ParseBrushRef( curTok, errors );
        if ( brush != 0 )
            brushes.push_back( brush );
    }

    // advance past the closing brace.
    ++curTok;

    // set the contents of the brush.
    gEdBrushPaletteMgr->SetBrushPalette( name, brushes );

    // return true to indicate success.
    return true;
}


//==========================================================
// static functions.
//==========================================================

//----------------------------------------------------------
URef< EdBrushImage >
ParseImageRef( const SBscToken*& curTok, tstring& errors )
{
    // make sure the current token isn't a closing brace.
    if ( curTok->tokenId != BL_IDENTIFIER && curTok->tokenId != BL_STRING )
    {
        errors = "Expected file name but found something else!";
        return 0;
    }

    // get the name of the image.
    URef< EdBrushImage > image = gEdBrushMgr->GetBrushImage( curTok->text );
    ++curTok;

    // return the image.
    return image;
}

//----------------------------------------------------------
URef< EdBrush >
ParseBrushRef( const SBscToken*& curTok, tstring& errors )
{
    // make sure the current token isn't a closing brace.
    if ( curTok->tokenId != BL_IDENTIFIER && curTok->tokenId != BL_STRING )
    {
        errors = "Expected brush name but found something else!";
        ++curTok;
        return 0;
    }

    // get the name of the image.
    URef< EdBrush > brush = gEdBrushMgr->GetBrush( curTok->text, errors );
    ++curTok;

    // return the image.
    return brush;
}
