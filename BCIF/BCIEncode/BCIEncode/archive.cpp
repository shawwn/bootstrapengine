// archive.cpp
// Copyright (C) 2007, Bootstrap Studios.
//
// For use, please see the accompanying readme.
//
#include "archive.h"
#include "zlib.h"
#include "zutil.h"

#include <string.h>
#include <windows.h>

//--------------------------------------------------------------------
Archive::Archive( FILE* fp )
: _stream( fp )
{

}

//--------------------------------------------------------------------
Archive::~Archive()
{
	// get where the central directory starts.
	unsigned long centralDirStart = ftell( _stream );

	// write out the central directory.
	FileNameList::iterator fileNameIter = _fileNameList.begin();
	for ( CentralDirList::iterator iter = _centralDir.begin(); iter != _centralDir.end(); ++iter )
	{
		fwrite( &( *iter ), sizeof( SCentralDirFileHeader ), 1, _stream );
		fwrite( fileNameIter->c_str(), iter->fileNameLength, 1, _stream );
		++fileNameIter;
	}

	unsigned long centralDirSize = ftell( _stream ) - centralDirStart;

	// write the end of central directory record.
	SEndOfCentralDir centralDirEnd;
	centralDirEnd.sig = 0x06054b50;
	centralDirEnd.diskNumber = 0;
	centralDirEnd.centralDirStartDisk = 0;
	centralDirEnd.centralDirEntryCountOnDisk = ( unsigned short )_centralDir.size();
	centralDirEnd.centralDirEntryCountTotal = ( unsigned short )_centralDir.size();
	centralDirEnd.centralDirSize = ( unsigned int )centralDirSize;
	centralDirEnd.centralDirOffset = ( unsigned int )centralDirStart;
	centralDirEnd.centralDirCommentLength = 0;
	fwrite( &centralDirEnd, sizeof( centralDirEnd ), 1, _stream );

	// close the stream.
	fclose( _stream );
}

//--------------------------------------------------------------------
void Archive::AddFile( const char* name, const void* data, unsigned int size, EMETHOD method )
{
	// deflate the incoming data.
#if 1
	unsigned int dstSize = 0;
	unsigned char* dstBuf = 0;
	if ( method != EM_NONE )
	{
		dstSize = compressBound( ( unsigned long )size );
		dstBuf = new unsigned char[ dstSize ];
		if ( method == EM_DEFLATE )
		{
			Deflate( dstBuf, dstSize, ( const unsigned char* )data, ( unsigned long )size );
		}
		else if ( method == EM_HUFFMAN )
		{
			Huffman( dstBuf, dstSize, ( const unsigned char* )data, ( unsigned long )size );
		}
	}
	else
		dstSize = size;
#else
	unsigned long dstSize = size;
	void* dstBuf = new unsigned char[ size ];
	memcpy( dstBuf, data, size );
#endif

	SYSTEMTIME sysTime;
	GetSystemTime( &sysTime );

	FILETIME fileTime;
	SystemTimeToFileTime( &sysTime, &fileTime );

	unsigned short fatDate = 0;
	unsigned short fatTime = 0;
	FileTimeToDosDateTime( &fileTime, &fatDate, &fatTime );

	// write out the local file header.
	unsigned long headerOffset = ftell( _stream );
	SLocalFileHeader header;
	header.sig = 0x04034b50;
	header.versionNeeded = 20;
	header.flags = ( ( method == EM_NONE ) ? 0 : 2 );	// max compression.
	header.method = ( ( method == EM_NONE ) ? 0 : 8 );	// deflate.
	header.modFileTime = fatTime;	// don't really care about this.
	header.modFileDate = fatDate;	// don't really care about this.
	header.crc32 = crc32( 0, ( Bytef* )data, size );
	header.compressedSize = dstSize;
	header.uncompressedSize = size;
	header.fileNameLength = ( unsigned short )strlen( name );
	header.extraFieldLength = 0;

	// write out the header and the file name.
	fwrite( &header, sizeof( header ), 1, _stream );
	fwrite( name, header.fileNameLength, 1, _stream );

	// write deflated data to the file.
	if ( method == EM_NONE )
		fwrite( data, size, 1, _stream );
	else
		fwrite( dstBuf+2, dstSize-4, 1, _stream );

	// fill out a central directory header.
	SCentralDirFileHeader centralDirHeader;
	centralDirHeader.sig = 0x02014b50;
	centralDirHeader.versionMadeBy = 0;
	centralDirHeader.versionNeeded = header.versionNeeded;
	centralDirHeader.flags = header.flags;
	centralDirHeader.method = header.method;
	centralDirHeader.modFileTime = header.modFileTime;
	centralDirHeader.modFileDate = header.modFileDate;
	centralDirHeader.crc32 = header.crc32;
	centralDirHeader.compressedSize = header.compressedSize;
	centralDirHeader.uncompressedSize = header.uncompressedSize;
	centralDirHeader.fileNameLength = header.fileNameLength;
	centralDirHeader.extraFieldLength = header.extraFieldLength;
	centralDirHeader.fileCommentLength = 0;
	centralDirHeader.diskNumberStart = 0;
	centralDirHeader.internalAttributes = 0;
	centralDirHeader.externalAttributes = 0;
	centralDirHeader.offsetOfLocalHeader = ( unsigned int )headerOffset;

	// add the current file to the central directory.
	_centralDir.push_back( centralDirHeader );
	_fileNameList.push_back( name );

	delete[] dstBuf;
}

//--------------------------------------------------------------------
int Archive::Deflate( void* dstData, unsigned int& dstSize, const void* data, unsigned int size )
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)data;
    stream.avail_in = (uInt)size;
    stream.next_out = (Bytef*)dstData;
    stream.avail_out = (uInt)dstSize;
    if ((uLong)stream.avail_out != dstSize) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit(&stream, 9);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    dstSize = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}

//--------------------------------------------------------------------
int Archive::Huffman( void* dstData, unsigned int& dstSize, const void* data, unsigned int size )
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)data;
    stream.avail_in = (uInt)size;
    stream.next_out = (Bytef*)dstData;
    stream.avail_out = (uInt)dstSize;
    if ((uLong)stream.avail_out != dstSize) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit2(&stream, 9, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
                       Z_HUFFMAN_ONLY);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    dstSize = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}
