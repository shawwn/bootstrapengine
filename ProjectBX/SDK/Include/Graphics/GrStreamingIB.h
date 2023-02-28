//----------------------------------------------------------
// File:        GrStreamingIB.h
// Author:      Kevin Bray
// Created:     01-12-07
//
// Purpose:     To stream indices to the graphics card.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMeshIB;

//**********************************************************
// class GrStreamingIB
//**********************************************************
class GrStreamingIB
{
public:
    GrStreamingIB( unsigned int initSize = 1048576, unsigned int growExtra = 16384 );
    ~GrStreamingIB();

    // adds indices to the index buffer.
    void                AddIndices( unsigned short* indices, unsigned int count );

    unsigned int        GetOffset() const       {   return _offset;         }

    // clear indices and reset the current offset.
    void                Reset();

    // binds the buffer for use with rendering.
    void                Bind();

private:
    URef< GrMeshIB >    _indexBuffer;
    unsigned int        _offset;
    unsigned int        _growExtra;
};
