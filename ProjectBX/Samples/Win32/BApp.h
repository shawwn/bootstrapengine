#pragma once

#ifdef WIN32

// TODO: Move BApp.h and BApp.cpp outside of the Samples folder.

// windows.
#include <windows.h>
typedef HWND WNDHANDLE;

#else
#error "Platform not implemented."
#endif

//-----------------------
// basic application setup.  
// Specify 0 for workingFolder to default to working in the exe's directory.
bool BAppSetup( WNDHANDLE windowHandle, const char* workingFolder );

//-----------------------
// de-initialize the app.
void BAppTeardown( WNDHANDLE windowHandle );

//-----------------------
// swap OpenGL buffers.
void BAppSwapBuffers();