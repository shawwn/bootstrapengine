//----------------------------------------------------------
// File:		FSDiskServ.h
// Author:		Kevin Bray
// Created:		10-21-08
//
// Purpose:		To provide a platform independent wrapper around a disk
//				and file operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//==========================================================
// File creation flags
//==========================================================

// The FS_FILE_FLAG_CREATE forces the file to be recreated when it's
// opened.  Note that this flag is not valid for read-only drives.
#define FS_FILE_FLAG_CREATE_ALWAYS	0x0001

// The FS_FILE_FLAG_OPEN_ALWAYS forces the file to be opened if it
// already exists, and created if it does not currently exist.
#define FS_FILE_FLAG_OPEN_ALWAYS	0x0002

// The FS_FILE_FLAG_READ flag indicates that the file will be
// readable.
#define FS_FILE_FLAG_READ			0x0004

// The FS_FILE_FLAG_WRITE flag indicates that the file will be
// writable.  Note that this flag is not valid for read-only drives.
#define FS_FILE_FLAG_WRITE			0x0008

// The FS_FILE_FLAG_CONTIGUOUS flag is only valid when combined with
// FS_FILE_FLAG_CREATE.  This flag tells the system to try and create
// the file on a contiguous section of the disk for optimal access
// performance.
#define FS_FILE_FLAG_CONTIGUOUS		0x0010

// The FS_FILE_FLAG_USE_DMA flag opens the file for DMA based access.
// With this method, the file must be read and written to in
// multiples of 2K.
#define FS_FILE_FLAG_USE_DMA		0x0020

// The FS_FILE_WRITE_REORDER flag tells the file system that writes
// can be re-ordered with respect to reads and writes.  Caution must
// be used with this flag as overlapping reads and writes may produce
// different results at different times.
#define FS_FILE_WRITE_REORDER		0x0040

//==========================================================
// Move method specifiers
//==========================================================

// The FS_FILE_BEGIN seeks from the beginning of the file.
#define FS_FILE_BEGIN				0

// The FS_FILE_CURRENT seeks from the current file pointer.
#define FS_FILE_CURRENT				1

// The FS_FILE_END seeks from the end of the file.
#define FS_FILE_END					2

//==========================================================
// File structures.
//==========================================================
typedef struct SFSFileReadBuffer_t
{
	volatile void* buffer;
	volatile int complete;
} SFSFileReadBuffer;

typedef struct SFSFileWriteBuffer_t
{
	volatile const void* buffer;
	volatile int complete;
} SFSFileWriteBuffer;

typedef struct SFSFileMap_t
{
	FSsize startSector;
	FSsize sectorCount;
	struct SFSFileMap_t* next;
	volatile int gate;
} SFSFileMap;

//==========================================================
// File management functions.
//==========================================================
extern void FS_ScanDrives();
extern void* FS_OpenFile( const FSchar* name, unsigned int flags );
extern void	FS_CloseFile( void* handle );
extern FSsize FS_GetFileSize( void* handle );
extern void FS_ReadFile( SFSFileReadBuffer* dst, void* file, FSsize offset, unsigned int size );
extern void	FS_WriteFile( void* file, FSsize offset, SFSFileWriteBuffer* src, unsigned int size );
extern void FS_Wait( volatile int* gate );			// waits for and resets the gate's value.
extern bool FS_IsComplete( volatile int* gate );	// checks and resets the gate's value.
extern bool FS_IsReadComplete( SFSFileReadBuffer* readBuffer );
extern bool FS_IsWriteComplete( SFSFileWriteBuffer* writeBuffer );
