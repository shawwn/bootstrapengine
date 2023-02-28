//----------------------------------------------------------
// File:        ThPrim.h
// Author:      Kevin Bray
// Created:     10-21-08
//
// Purpose:     To provide locked access to variables shared by threads.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// Global Functions
//**********************************************************

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic( _InterlockedCompareExchange, _InterlockedExchange)

#define ThSwap( prim, value )                       ( int )_InterlockedExchange( ( prim ), ( long )( value ) )
#define ThCompareSwap( prim, value, test )          ( int )_InterlockedCompareExchange( ( prim ), ( long )value, ( long )test )

#else

#error Implement me for the desired platform!

#endif
