//----------------------------------------------------------
// File:        EMAfx.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To precompile headers for the managed layer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#pragma managed( push, off ) // required by C++/CLI due to the inline assembly.
#include "graphics_afx.h"
#pragma managed( pop ) // required by C++/CLI due to the inline assembly.

#include <cassert>

// managed layer macros.
#define E_ASSERT assert

// this is required because C++/CLI 'float' member variables 
// cannot be passed as a "const float&", so we cannot use 
// the engine's Lerp function.
inline float 
LerpF( float v0, float v1, float t )
{
    return ( 1.0f - t ) * v0 + t * v1;
}