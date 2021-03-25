// archive.h
// Copyright (C) 2007, Bootstrap Studios.
//
// For use, please see the accompanying readme.
//
#pragma once

#include <stdio.h>
#include <list>

#pragma pack ( push )
#pragma pack ( 1 )
struct SLocalFileHeader
{
	unsigned int sig;
	unsigned short versionNeeded;
	unsigned short flags;
	unsigned short method;
	unsigned short modFileTime;
	unsigned short modFileDate;
	unsigned int crc32;
	unsigned int compressedSize;
	unsigned int uncompressedSize;
	unsigned short fileNameLength;
	unsigned short extraFieldLength;
};

struct SCentralDirFileHeader
{
	unsigned int sig;
	unsigned short versionMadeBy;
	unsigned short versionNeeded;
	unsigned short flags;
	unsigned short method;
	unsigned short modFileTime;
	unsigned short modFileDate;
	unsigned int crc32;
	unsigned int compressedSize;
	unsigned int uncompressedSize;
	unsigned short fileNameLength;
	unsigned short extraFieldLength;
	unsigned short fileCommentLength;
	unsigned short diskNumberStart;
	unsigned short internalAttributes;
	unsigned int externalAttributes;
	unsigned int offsetOfLocalHeader;
};

struct SEndOfCentralDir
{
	unsigned int sig;
	unsigned short diskNumber;
	unsigned short centralDirStartDisk;
	unsigned short centralDirEntryCountOnDisk;
	unsigned short centralDirEntryCountTotal;
	unsigned int centralDirSize;
	unsigned int centralDirOffset;
	unsigned short centralDirCommentLength;
};
#pragma pack ( pop )

//====================================================================
// class Archive
class Archive
{
public:
	enum EMETHOD
	{
		EM_NONE,
		EM_DEFLATE,
		EM_HUFFMAN,
	};

	Archive( FILE* fp );
	~Archive();

	// adds a file to the archive.
	void			AddFile( const char* name, const void* data, unsigned int size, EMETHOD method );

private:
	typedef std::list< SCentralDirFileHeader > CentralDirList;
	typedef std::list< std::string > FileNameList;

	int				Deflate( void* dstData, unsigned int& dstSize, const void* data, unsigned int size );
	int				Huffman( void* dstData, unsigned int& dstSize, const void* data, unsigned int size );

	// file pointer.
	FILE*			_stream;
	CentralDirList	_centralDir;
	FileNameList	_fileNameList;
};