//----------------------------------------------------------
// File:        UBlockAllocator.h
// Author:      Kevin Bray
// Created:     08-20-08
//
// Purpose:     To allocate objects in chunks.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UBlockAllocator
//**********************************************************
class UBlockAllocator
{
public:
    UBlockAllocator( unsigned int itemSize, unsigned int itemsPerBlock );
    ~UBlockAllocator();

    // allocates an item.
    void*           AllocMem();

    // frees all allocated objects.
    void            FreeMem();

private:
    struct SBlock
    {
        unsigned char* objectMem;
        SBlock* next;
    };

    SBlock*         AllocBlock();

    SBlock*         _blocks;
    SBlock*         _curBlock;
    unsigned int    _curBlockItem;
    unsigned int    _itemSize;
    unsigned int    _itemsPerBlock;
};
