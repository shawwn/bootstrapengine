//----------------------------------------------------------
// File:		UBlockAllocator.cpp
// Author:		Kevin Bray
// Created:		08-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UBlockAllocator.h"

//**********************************************************
// class UBlockAllocator
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UBlockAllocator::UBlockAllocator( unsigned int itemSize, unsigned int itemsPerBlock )
: _blocks( 0 )
, _curBlock( 0 )
, _curBlockItem( 0 )
, _itemSize( itemSize )
, _itemsPerBlock( itemsPerBlock )
{

}

//----------------------------------------------------------
UBlockAllocator::~UBlockAllocator()
{
	FreeMem();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void*
UBlockAllocator::AllocMem()
{
	if ( _curBlock == 0 )
	{
		B_ASSERT( _blocks == 0 );
		_blocks = AllocBlock();
		_curBlock = _blocks;
	}
	else if ( _curBlockItem == _itemsPerBlock )
	{
		_curBlockItem = 0;
		_curBlock->next = AllocBlock();
		_curBlock = _curBlock->next;
	}

	// return memory from the current block.
	unsigned char* mem = _curBlock->objectMem + ( _curBlockItem * _itemSize );
	++_curBlockItem;

	// return the allocated memory.
	return mem;
}

//----------------------------------------------------------
void
UBlockAllocator::FreeMem()
{
	// iterate through all blocks, destroying them.
	SBlock* curBlock = _blocks;
	while ( curBlock )
	{
		// get the block we're going to free and advance to the next
		// block.
		SBlock* deadBlock = curBlock;
		curBlock = curBlock->next;

		// free the current block.
		delete[] deadBlock->objectMem;
		delete deadBlock;
	}

	// set the current block item to 0.
	_curBlock = 0;
	_blocks = 0;
	_curBlockItem = 0;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
UBlockAllocator::SBlock*
UBlockAllocator::AllocBlock()
{
	// allocate a new block and return it.
	SBlock* newBlock = new SBlock;
	newBlock->objectMem = new unsigned char[ _itemsPerBlock * _itemSize ];
	newBlock->next = 0;
	return newBlock;
}
