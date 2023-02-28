//----------------------------------------------------------
// File:        GrPixelBufferMgr.h
// Author:      Kevin Bray
// Created:     04-11-08
//
// Purpose:     To manage pixel buffers.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrPixelBuffer.h"
#include "URefArray.h"

//**********************************************************
// GrPixelBufferMgr
//**********************************************************
class GrPixelBufferMgr
{
public:
    GrPixelBufferMgr();
    ~GrPixelBufferMgr();

    // creates a pixel buffer of a given size.
    URef< GrPixelBuffer >   CreatePixelBuffer( unsigned int width, unsigned int height,
                                               GrPixelBuffer::EFORMAT format, GrPixelBuffer::EUSAGE usage );

    // forces garbage collection.
    void                    CollectGarbage();

    // context reset notification.
    void                    PreContextReset();
    void                    PostContextReset();

private:
    typedef URefArray< GrPixelBuffer > PixelBufferArray;

    // pixel buffers.
    PixelBufferArray        _pixelBuffers;
};
extern GrPixelBufferMgr* gGrPixelBufferMgr;