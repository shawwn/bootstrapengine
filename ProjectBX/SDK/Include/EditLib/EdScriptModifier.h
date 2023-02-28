//----------------------------------------------------------
// File:        EdScriptModifier.h
// Author:      Kevin Bray
// Created:     12-03-06
//
// Purpose:     To ease editing script files.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>

//**********************************************************
// class EdScriptModifier
//**********************************************************
class EdScriptModifier
{
public:
    EdScriptModifier( const tstring& fileName );
    ~EdScriptModifier();

    // returns the file name.
    const tstring&  GetFileName() const     {   return _fileName;   }

    // return whether or not the script was loaded successfully.
    bool            IsValid() const         {   return _valid;      }

    // selects an object in the currently loaded script.
    bool            SelectObj( const tstring& name );

    // adds an object to the script and selects it for modification.
    void            AddObj( const tstring& name );

    // replaces the currently selected object.
    void            Replace( const tstring& data );

    // changes a command in the currently selected object.
    bool            ChangeCommand( const char* command, const tstring& newLine );

    // adds a flag to the script if it doesn't already exist.
    void            AddFlag( const tstring& flag );

    // saves the script.
    void            Save();

private:
    typedef std::vector< char* > StringVec;

    // case insensitive strstr
    void            ParseLines( StringVec& output, const char* text );
    bool            FindCommand( const char* text, const char* command );

    tstring         _fileName;
    StringVec       _lines;
    tstring         _objSelected;
    size_t          _startLine;
    size_t          _endLine;
    bool            _valid;
};
