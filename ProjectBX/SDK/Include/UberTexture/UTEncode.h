//----------------------------------------------------------
// File:        UTEncode.h
// Author:      Kevin Bray
// Created:     09-05-08
//
// Purpose:     To provide a thread for encoding and decoding
//              uber-tiles.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//----------------------------------------------------------
// Encoding thread services.
//----------------------------------------------------------

// get memory to write to.
extern void* UTE_GetTileBuffer();
extern void UTE_PresentTileBuffer( void* messageHeader, int headerSize );