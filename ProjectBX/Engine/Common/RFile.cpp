//----------------------------------------------------------
// File:		RFile.h
// Author:		Kevin Bray
// Created:		11-12-05
//
// Purpose:		To manage a text file.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "RFile.h"

// project includes.
#include "RFileMgr.h"
#include "FSDiskServ.h"


//**********************************************************
// class RFile
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
RFile::RFile( const UPath& name, void* handle, unsigned int flags )
: RRsrc( name )
, _handle( handle )
, _data( 0 )
, _size( 0 )
, _flags( flags )
{
	// get the size of the file.
	_size = FS_GetFileSize( _handle );
}

//----------------------------------------------------------
RFile::~RFile()
{
	FS_CloseFile( _handle );
	Purge();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
RFile::Reset( void* handle, unsigned int flags )
{
	// reset the file's handle and creation flags.
	_handle = handle;
	_flags = flags;

	// free the data.
	Purge();

	// get the size of the file.
	_size = FS_GetFileSize( _handle );
}

//----------------------------------------------------------
bool
RFile::CanRead()
{
	return ( _flags & FS_FILE_FLAG_READ ) != 0;
}

//----------------------------------------------------------
bool
RFile::CanWrite()
{
	return ( _flags & FS_FILE_FLAG_WRITE ) != 0;
}

//----------------------------------------------------------
bool
RFile::Exists() const
{
	return ( _handle != 0 );
}

//----------------------------------------------------------
bool
RFile::Reload() const
{
	// simply return if the file doesn't exist.
	if ( !Exists() )
		return false;

	// free the previously loaded data.
	Purge();

	// allocate space for the file's data.
	_data = new char[ ( size_t )_size + 1 ];

	// fill out a read buffer.
	SFSFileReadBuffer_t readBuf;
	readBuf.buffer = ( volatile void* )_data;
	readBuf.complete = 0;

	// issue the read request.
	__int64 curSize = _size;
	__int64 offset = 0;
	while ( curSize > 0 )
	{
		// read in the current block.
		unsigned int blockSize = ( unsigned int )( ( curSize > 0x40000000 ) ? 0x40000000 : curSize );
		FS_ReadFile( &readBuf, _handle, offset, blockSize );

		// advance the current size and the offset.
		curSize -= blockSize;
		offset += blockSize;
	}

	// terminate the data with a null terminator.
	( ( unsigned char* )readBuf.buffer )[ _size ] = '\0';

	// wait until the read is complete.
	if ( _size )
		FS_Wait( &readBuf.complete );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void
RFile::Purge() const
{
	// free the data.
	delete[] _data;
	_data = 0;
}

//----------------------------------------------------------
const void*
RFile::GetData() const
{
	EnsureLoaded();
	return _data;
}

//----------------------------------------------------------
__int64
RFile::GetSize() const
{
	return _size;
}

//----------------------------------------------------------
void
RFile::WriteData( __int64 offset, const void* src, unsigned int size )
{
	// if we have data cached, write through it.
	if ( _data )
		MemCopy( ( char* )_data + ( unsigned int )offset, src, size );

	// issue a write request.
	SFSFileWriteBuffer writeBuffer;
	writeBuffer.buffer = src;
	writeBuffer.complete = 0;
	FS_WriteFile( _handle, offset, &writeBuffer, size );

	// wait for the operation to complete.
	FS_Wait( &writeBuffer.complete );

	// update the file's size if necessary.
	__int64 newSize = offset + size;
	if ( newSize > _size )
		_size = newSize;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
RFile::EnsureLoaded() const
{
	// if the data isn't loaded already, reload it.
	if ( !_data )
		Reload();
}
