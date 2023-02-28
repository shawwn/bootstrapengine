//----------------------------------------------------------
// File:        Module.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Module.h"

#pragma warning( disable : 4996 )
#define _CRT_VCCLRIT_NO_DEPRECATE
#pragma managed( push, off ) // required by C++/CLI due to the inline assembly.
#include <windows.h>
#include <_vcclrit.h>
#pragma managed( pop )
#pragma warning( default : 4996 )

//**********************************************************
// ref class Module
//**********************************************************

//----------------------------------------------------------
void 
Bootstrap::Module::Startup()
{
    // DLL initialization.
    __crt_dll_initialize();
    _EngineStarted = true;
}

//----------------------------------------------------------
void 
Bootstrap::Module::Shutdown()
{
    // DLL shutdown.
    __crt_dll_terminate();
    _EngineStarted = false;
}

//----------------------------------------------------------
bool 
Bootstrap::Module::IsActive()
{
    return _EngineStarted;
}
