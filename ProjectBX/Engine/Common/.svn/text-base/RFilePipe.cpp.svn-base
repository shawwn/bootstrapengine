//----------------------------------------------------------
// File:		RFilePipe.cpp
// Author:		Kevin Bray
// Created:		10-26-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "RFilePipe.h"

// project includes.
#include "RFile.h"
#include "FSDiskServ.h"


//**********************************************************
// class RFilePipe
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
RFilePipe::RFilePipe( unsigned int pageSize, unsigned int pageCount, unsigned int alignment )
{

}

//----------------------------------------------------------
RFilePipe::~RFilePipe()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
const void*
RFilePipe::LockPage( unsigned int& pageIdx )
{
	// block until we can attain the read lock.
	while ( _FS_Swap( &_readLock, 1 ) == 1 )
		_FS_Pause();
	
	// verify that there is something to get.
	if ( _readPage == _writePage )
	{
		// release the read-lock and return null.
		_FS_Swap( &_readLock, 0 );
		return 0;
	}

	// get the read page and verify that it's ready.
	SFSFileReadBuffer* curPage = _pages + ( _readPage % _pageCount );
	if ( !FS_IsReadComplete( curPage ) )
	{
		// release the read-lock and return null.
		_FS_Swap( &_readLock, 0 );
		return 0;
	}

	// get the page index and the pointer to return.
	pageIdx = _readPage % _pageCount;
	const void* ptr = ( const void* )_pages->buffer;

	// set the page lock.
	int prev = _FS_Swap( _pageLocks + pageIdx, 1 );
	assert( prev == 0 );

	// increment the read page.
	_FS_Increment( ( int* )&_readPage );

	// release the read-lock.
	_FS_Swap( &_readLock, 0 );

	// return the data pointer.
	return ptr;
}

//----------------------------------------------------------
void
RFilePipe::UnlockPage( unsigned int pageIdx )
{
	// ensure that the page specified is unlocked.
	_FS_Swap( _pageLocks + pageIdx, 0 );
}

//----------------------------------------------------------
bool
RFilePipe::ReadFile( RFile* file, __int64 pageOffset, unsigned int pageCount )
{
	// block until we can attain the write lock.
	while ( _FS_Swap( &_writeLock, 1 ) == 1 )
		_FS_Pause();

	// check to see if we can advance the write pointer successfully.
	unsigned int readPage = _readPage;
	if ( ( _writePage - readPage ) == _pageCount )
	{
		// release the write-lock and return null.
		_FS_Swap( &_writeLock, 0 );
		return false;
	}

	// check to see if the current write page is currently locked.
	if ( _pageLocks[ _writePage ] != 0 )
	{
		// release the write-lock and return null.
		_FS_Swap( &_writeLock, 0 );
		return false;
	}

	// get the current read buffer page.
	unsigned int writePage = _writePage;

	// mark the transaction as incomplete.
	_pages[ writePage ].complete = 0;

	// issue the read request.
	FS_ReadFile( _pages + writePage, file->GetHandle(), pageOffset * _pageSize,
		pageCount * _pageSize );

	// increment the write page.
	_FS_Increment( ( volatile int* )&_writePage );

	// release the write-lock.
	_FS_Swap( &_writeLock, 0 );

	// return true to indicate success.
	return true;
}
