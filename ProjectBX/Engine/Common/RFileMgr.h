//----------------------------------------------------------
// File:		RFileMgr.h
// Author:		Kevin Bray
// Created:		12-17-05
//
// Purpose:		To wrap file IO operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"
#include "UPath.h"
#include "UReader.h"

// BootstrapFS file system includes.
#include "FSDiskServ.h"

// std c++ includes.
#include <map>
#include <vector>

class UWriter;
class RFile;

//**********************************************************
// class RFileMgr
//**********************************************************
class RFileMgr
{
public:
	typedef unsigned int Flags;
	static const unsigned int kFlagCreateAlways = FS_FILE_FLAG_CREATE_ALWAYS;
	static const unsigned int kFlagOpenAlways = FS_FILE_FLAG_OPEN_ALWAYS;
	static const unsigned int kFlagRead = FS_FILE_FLAG_READ;
	static const unsigned int kFlagWrite = FS_FILE_FLAG_WRITE;
	static const unsigned int kFlagContiguous = FS_FILE_FLAG_CONTIGUOUS;
	static const unsigned int kFlagDMA = FS_FILE_FLAG_USE_DMA;
	static const unsigned int kFlagReadWrite = kFlagRead | kFlagWrite;
	static const unsigned int kFlagWriteReorder = FS_FILE_WRITE_REORDER;

	RFileMgr( const UPath& rootPath );
	~RFileMgr();

	// get a file.
	URef< RFile >			GetFile( const UPath& path, Flags flags );

	// returns true if the file specified exists.
	bool					Exists( const UPath& relativeFilePath );

	// recursive directory creation.
	// possibly should allow specifying base path also when relative?
	void					CreateDir( const UPath& dirPath, bool relative = true );

	// directory management.
	void					GetDirListing( std::vector< UPath >* files, std::vector< UPath >* folders, const UPath& relativeDirPath );

	// file copy.
	bool					CopyFile( const UPath& dstPath, const UPath& srcPath );

	// get the workinf folder.
	const UPath&			GetRootPath() const				{	return _rootPath;		}

private:
	typedef std::map< UPath,URef< RFile > > FileMap;

	// performs very basic garbage collection.
	void					CollectGarbage();

	// creates a full path out of a relative path.  If a full path is already
	// passed in, then the path is unchanged.
	void					CreateFullPath( UPath& fullPath, const UPath& path );

	// currently open file streams.
	FileMap					_files;
	UPath					_rootPath;
	volatile int			_gate;
};
extern RFileMgr* gRFileMgr;
