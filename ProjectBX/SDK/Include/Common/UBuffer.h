//----------------------------------------------------------
// File:        UBuffer.h
// Author:      Kevin Bray
// Created:     10-09-06
//
// Purpose:     To manage a memory buffer.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UBuffer
//**********************************************************
class UBuffer
{
public:
    UBuffer( size_t initSize = 0 );
    ~UBuffer();

    // ensures there is enough room for the size specified.  This will
    // clear the contents of the buffer.
    void            ReAlloc( size_t size );

    // frees the memory.
    void            Free();

    // operators.
    char*           GetPtr()            {   return _ptr;        }
    size_t          GetSize()           {   return _size;       }

private:
    char*           _ptr;
    size_t          _size;
};
