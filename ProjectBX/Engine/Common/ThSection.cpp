//----------------------------------------------------------
// File:        ThSection.cpp
// Author:      Kevin Bray
// Created:     09-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// file header.
#include "ThSection.h"

// windows includes.
#include <windows.h>

//**********************************************************
// class ThSection
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
ThSection::ThSection()
{
    _section = new CRITICAL_SECTION;
    InitializeCriticalSection( ( CRITICAL_SECTION* )_section );
}

//----------------------------------------------------------
ThSection::~ThSection()
{
    DeleteCriticalSection( ( CRITICAL_SECTION* )_section );
    delete ( CRITICAL_SECTION* )_section;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
ThSection::Enter()
{
    EnterCriticalSection( ( CRITICAL_SECTION* )_section );
}

//----------------------------------------------------------
bool
ThSection::TryEnter()
{
    BOOL result = TryEnterCriticalSection( ( CRITICAL_SECTION* )_section );
    return ( result == TRUE );
}

//----------------------------------------------------------
void
ThSection::Leave()
{
    LeaveCriticalSection( ( CRITICAL_SECTION* )_section );
}
