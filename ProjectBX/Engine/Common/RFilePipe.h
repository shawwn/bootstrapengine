//----------------------------------------------------------
// File:		RFilePipe.h
// Author:		Kevin Bray
// Created:		10-26-08
//
// Purpose:		To provide a pipe interface for reading files.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class RFile;
typedef struct SFSFileReadBuffer_t SFSFileReadBuffer;

//**********************************************************
// class RFilePipe
//**********************************************************
class RFilePipe
{
public:
	RFilePipe( unsigned int pageSize, unsigned int pageCount, unsigned int alignment = 1 );
	~RFilePipe();

	// accessors.
	unsigned int			GetBufferSize()	const		{	return _bufferSize;				}
	unsigned int			GetPageCount() const		{	return _pageCount;				}
	unsigned int			GetPageSize() const			{	return _pageSize;				}
	unsigned int			GetAlignment() const		{	return _alignment;				}

	// locks the current page and increments the current page index.
	// If no data is available, this function simply returns null.
	const void*				LockPage( unsigned int& pageIdx );
	void					UnlockPage( unsigned int pageIdx );

	// reads from a location within the file that is a multiple of
	// the page size.
	bool					ReadFile( RFile* file, __int64 pageOffset, unsigned int pageCount );

private:
	SFSFileReadBuffer*		_pages;
	void*					_buffer;
	volatile int*			_pageLocks;
	unsigned int			_pageCount;
	unsigned int			_pageSize;
	unsigned int			_bufferSize;
	unsigned int			_alignment;
	volatile unsigned int	_readPage;
	volatile unsigned int	_writePage;
	volatile int			_readLock;
	volatile int			_writeLock;
};
