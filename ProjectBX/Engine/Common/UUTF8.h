//----------------------------------------------------------
// File:        UUTF8.h
// Author:      Kevin Bray
// Created:     10-21-04
//
// Purpose:     A library for handling UTF8 characters.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// UCS-2 to UTF-8 functions...
extern tstring operator + ( const tstring& byteStr, wchar_t word );
extern tstring operator + ( const tstring& byteStr, wchar_t *wordStr );
extern tstring& operator += ( tstring& byteStr, wchar_t *wordStr );
extern wchar_t *MakeUnicode( const tstring& str );
extern tstring MakeUTF8( wchar_t *wordStr );
extern tstring UTF16toUTF8( UINT_PTR utf16 );
extern tstring UnicodeToUTF8( unsigned short uniChar );
extern tstring AsciiToUTF8( unsigned char ch );
extern tstring AsciiToUTF8( tstring str );
