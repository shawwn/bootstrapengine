// file header.
#include "zip.h"

// Win32 includes.
#include <windows.h>

// CRT includes.
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

// zlip includes.
#include "zlib.h"
#include "zutil.h"

// zip file header signatures.
#define ZIP_HEADER_SIG				0x04034b50
#define ZIP_CENTRAL_DIR_SIG			0x02014b50
#define ZIP_END_CENTRAL_DIR_SIG		0x06054b50

#pragma warning( disable : 4996 )

// zip file data structures.
#pragma pack ( push )
#pragma pack ( 1 )

// :ocal file header. There one for each file immediately
// preceding the file's data.
struct zip_file_header_t
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

// File header in the central directory.  There one for each
// File in the central directory.  The central directory resides
// after all compressed files.
struct zip_central_dir_t
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

// This is the end of the central directory.  It resides after
// the last central directory file header.
struct zip_end_central_dir_t
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

static zip_file_header_t *GetNextFile( zip_file_header_t *curFile );

//================================
// function:	Zip_Create
// purpose:		Creates a zip stream.
//================================
void Zip_Create( zip_t *zip )
{
	zip->size = 0;
	zip->data = malloc( 4 );
	*( (unsigned int *)zip->data ) = ZIP_CENTRAL_DIR_SIG;
}

//================================
// function:	Zip_Load
// purpose:		Loads a zip stream.
//================================
void Zip_Load( zip_t *zip, FILE *fp )
{
	// get the file size.
	fseek( fp, 0, SEEK_END );
	long size = ftell( fp );
	rewind( fp );

	// read in the stream.
	zip->size = size;
	zip->data = malloc( size + 4 );
	fread( zip->data, size, 1, fp );

	// append the last code.
	*(unsigned int *)( (char *)zip->data + size ) = ZIP_CENTRAL_DIR_SIG;
}

//================================
// function:	Zip_Load
// purpose:		To free allocated resources.
//================================
void Zip_Free( zip_t *zip )
{
	free( zip->data );
}

//================================
// function:	Zip_FindFile
// purpose:		Calculates the total file count in a zip file.
//================================
size_t Zip_CalcFileCount( zip_t *zip )
{
	// iterate through each file in the zip and increment our
	// counter.
	size_t count = 0;
	zip_file_header_t *curFile = Zip_GetFirstFile( zip );
	while ( curFile->sig == ZIP_HEADER_SIG )
	{
		++count;
		curFile = GetNextFile( curFile );
	}

	// return the count.
	return count;
}

//================================
// function:	Zip_FindFile
// purpose:		Finds a file of the specified name.
//================================
zip_file_header_t *Zip_FindFile( zip_t *zip, const char* fileName )
{	
	// get the first file.
	zip_file_header_t *cur = Zip_GetFirstFile( zip );
	if ( cur == 0 )
		return NULL;

	// get the length of the file name passed in.
	size_t nameLen = strlen( fileName );

	// now iterate through all files in the archive.
	while ( cur->sig == ZIP_HEADER_SIG )
	{
		// check to see if this is the right file.
		char *curName = (char *)( cur + 1 );
		if ( nameLen == cur->fileNameLength )
		{
			if ( strnicmp( curName, fileName, nameLen ) == 0 )
			{
				// return the header of the file.
				return cur;
			}
		}

		// next.
		cur = GetNextFile( cur );
	}

	// return NULL if the file wasn't found!
	return NULL;
}

//================================
// function:	Zip_GetFirstFile
// purpose:		Gets the file after the file passed in.
// remarks:		This function returns NULL if there are no files in
//				the archive.
//================================
zip_file_header_t *Zip_GetFirstFile( zip_t *zip )
{
	// return the first file.
	zip_file_header_t *first = (zip_file_header_t *)zip->data;
	if ( first->sig != ZIP_HEADER_SIG )
		return NULL;
	return first;
}

//================================
// function:	Zip_GetNextFile
// purpose:		Gets the file after the file passed in.
// remarks:		This function returns NULL if there are no more files.
//================================
zip_file_header_t *Zip_GetNextFile( zip_file_header_t *curFile )
{
	assert( curFile->sig == ZIP_HEADER_SIG );

	// get the next file.
	curFile = GetNextFile( curFile );
	if ( curFile->sig != ZIP_HEADER_SIG )
		return 0;
	return curFile;
}

//================================
// function:	Zip_GetFileName
// purpose:		Returns the name of the specified file.
//================================
size_t Zip_GetFileName( char* buf, size_t maxSize, zip_file_header_t *curFile )
{
	assert( curFile->sig == ZIP_HEADER_SIG );
	--maxSize;
	strncpy( buf, (const char *)( curFile + 1 ), maxSize );
	buf[ maxSize ] = '\0';
	return ( curFile->fileNameLength + 1 );
}

//================================
// function:	Zip_GetFileSize
// purpose:		Returns the size of the specified file.
//================================
size_t Zip_GetFileSize( zip_file_header_t *curFile )
{
	assert( curFile->sig == ZIP_HEADER_SIG );
	return curFile->uncompressedSize;
}

//================================
// function:	Zip_GetFileSize
// purpose:		Returns the compressed size of the specified file.
//================================
size_t Zip_GetCompressedFileSize( zip_file_header_t *curFile )
{
	assert( curFile->sig == ZIP_HEADER_SIG );
	return curFile->compressedSize;
}

//================================
// function:	Zip_GetFileNames
// purpose:		Returns all file names stored in the zip.
// remarks:		File name string memory is allocated by the function
//				and should be freed by the caller. 
//================================
size_t Zip_GetFileNames( char** fileNames, zip_t* zip )
{
	size_t count = 0;
	zip_file_header_t *curFile = Zip_GetFirstFile( zip );
	while ( curFile->sig == ZIP_HEADER_SIG )
	{
		// allocate memory for the string and copy the characters.
		fileNames[ count ] = (char *)malloc( curFile->fileNameLength+1 );
		memcpy( fileNames[ count ], ( curFile + 1 ), curFile->fileNameLength );
		fileNames[ count ][ curFile->fileNameLength ] = '\0';

		// increment our string count get the next file.
		curFile = GetNextFile( curFile );
		++count;
	}

	// return the number of files names.
	return count;
}

//================================
// function:	Zip_Rebuild
// purpose:		Rebuilds the archive with the files specified.
// remarks:		This function reorders files in the archive.  If one
//				of the names wasn't found, then the name is ignored.
//				This function returns the number of files in the new
//				zip.
//================================
size_t Zip_Rebuild( zip_t *zip, const char *const *orderedNames, size_t nameCount )
{
	// allocate memory for the zip.
	void *newMem = malloc( zip->size + 4 );

	// iterate through each name.
	size_t newCount = 0;
	size_t newSize = 0;
	for ( size_t i = 0; i < nameCount; ++i )
	{
		// get the current name.
		const char *curName = orderedNames[ i ];

		// try to find the file.
		zip_file_header_t *file = Zip_FindFile( zip, curName );
		if ( !file )
			continue;

		// get the end of the copy.
		zip_file_header_t *end = GetNextFile( file );
		size_t copySize = (char *)end - (char *)file;

		// copy the data (including the terminator) and update our info.
		memcpy( (char *)newMem + newSize, file, copySize );
		newSize += copySize;
		++newCount;
	}

	// assign the terminator.
	*(unsigned int *)( (char *)newMem + newSize ) = ZIP_CENTRAL_DIR_SIG;

	// free the old zip memory and such.
	free( zip->data );
	zip->data = newMem;
	zip->size = newSize;

	// return the new count.
	return newCount;
}

//================================
// function:	Zip_ReadFile
// purpose:		Reads a file from the archive.
// remarks:		This function returns 0 if the file wasn't found, or
//				the size of the file if it was found.  Memory is
//				allocated to hold the data and should be freed by the
//				calling application.
//================================
size_t Zip_ReadFile( void** dst, zip_file_header_t *file )
{
	assert( file->sig == ZIP_HEADER_SIG );

	// get the source.
	const char* src = (char *)( file + 1 ) + file->fileNameLength;

	// we found the file.
	*dst = malloc( file->uncompressedSize );
	unsigned long size = file->uncompressedSize;
	if ( file->method == 8 )
	{
		// read in the compressed data from the file.
		unsigned char *buf = (unsigned char *)malloc( file->compressedSize + 2 );
		memcpy( buf + 2, (Bytef *)src, file->compressedSize );

		// prepend the zlib header.
		buf[0] = 0x78;
		buf[1] = 0x9C;

		// uncompress.
		uncompress( (Bytef *)*dst, &size, (Bytef *)buf, file->compressedSize + 2 ); 
		assert( size == file->uncompressedSize );
	}
	else
	{
		memcpy( *dst, src, file->compressedSize );
		size = file->compressedSize;
	}

	// return the size of the uncompressed data to indicate success.
	return size;
}

//================================
// function:	Zip_RemoveFile
// purpose:		Removes a file from the zip stream.
// remarks:		Returns non-zero if successful, zero otherwise.
//================================
void Zip_RemoveFile( zip_t* zip, zip_file_header_t *file )
{
	// calculate the source and destination pointers.
	zip_file_header_t *next = GetNextFile( file );
	char *dst = (char *)file;
	char *src = (char *)next;

	// scan forward until we hit the end of the stream.
	while ( next->sig == ZIP_HEADER_SIG )
		next = GetNextFile( next );
	size_t remaining = (char *)next - src;

	// move memory, including the 4 byte terminator.
	memmove( dst, src, remaining + 4 );
	zip->size -= ( src - dst );
}

//================================
// function:	Zip_AddFile
// purpose:		Adds a file to the end of an existing zip stream.  The
//				data is compressed using the method specified.  See the
//				ZIP_METHOD_* flags.
//================================
void Zip_AddFile( zip_t *zip, const char *name, const void *data, size_t size, unsigned int method )
{
	// calculate various values.
	unsigned long nameLen = ( unsigned long )strlen( name );
	unsigned long uncompLen = ( unsigned long )size;
	unsigned long compLen = compressBound( ( unsigned long )size );

	// allocate a temporary buffer where we can pack the data.
	unsigned char *buf = (unsigned char *)malloc( compLen );
	if ( method == ZIP_METHOD_NONE )
	{
		memcpy( buf, data, size );
		compLen = uncompLen;
	}
	else
	{
		compress( buf, &compLen, ( const Bytef* )data, uncompLen );
		compLen -= 2;
	}

	// expand the data to allow for another file.
	size_t growSize = compLen + nameLen + sizeof( zip_file_header_t );
	zip->size += growSize;
	zip->data = realloc( zip->data, zip->size + 4 );

	// I'm not a big fan of this timestamp stuff, but I guess
	// it'll have to do?
	SYSTEMTIME sysTime;
	FILETIME fileTime;
	unsigned short fatDate = 0;
	unsigned short fatTime = 0;
	GetSystemTime( &sysTime );
	SystemTimeToFileTime( &sysTime, &fileTime );
	FileTimeToDosDateTime( &fileTime, &fatDate, &fatTime );

	// move to the end of the stream.
	zip_file_header_t *cur = (zip_file_header_t *)zip->data;
	while ( cur->sig == ZIP_HEADER_SIG )
		cur = GetNextFile( cur );

	// now add the compressed data.
	char *dst = (char *)cur;
	zip_file_header_t *header = (zip_file_header_t *)dst;

	// make sure the terminator was set correctly.
	assert( header->sig == ZIP_CENTRAL_DIR_SIG );

	// fill out the header.
	header->sig = ZIP_HEADER_SIG;
	header->versionNeeded = 20;
	header->flags = ( ( method == ZIP_METHOD_NONE ) ? 0 : 2 );	// max compression.
	header->method = ( ( method == ZIP_METHOD_NONE ) ? 0 : 8 );	// deflate.
	header->modFileTime = fatTime;
	header->modFileDate = fatDate;
	header->crc32 = crc32( 0, ( Bytef* )data, uncompLen );
	header->compressedSize = compLen;
	header->uncompressedSize = uncompLen;
	header->fileNameLength = ( unsigned short )nameLen;
	header->extraFieldLength = 0;

	// advance past the header and write the name.
	dst += sizeof( zip_file_header_t );
	memcpy( dst, name, nameLen );

	// advance past the name and write the compressed data.
	dst += nameLen;
	if ( method == ZIP_METHOD_NONE )
	{
		memcpy( dst, buf, compLen );
	}
	else
	{
		memcpy( dst, buf+2, compLen );
	}
	dst += compLen;

	// write the terminator.
	*( (unsigned int *)dst ) = ZIP_CENTRAL_DIR_SIG;

	// cleanup.
	free( buf );
}

//================================
// function:	Zip_Save
// purpose:		Saves the zip stream to a file.
//================================
void Zip_Save( FILE *fp, zip_t *zip )
{
	// get the address of the current terminator.
	unsigned int centralDirSize = sizeof( zip_end_central_dir_t );
	zip_file_header_t *curFile = (zip_file_header_t *)zip->data;
	while ( curFile->sig == ZIP_HEADER_SIG )
	{
		// record information about the current file.
		centralDirSize += sizeof( zip_central_dir_t );
		centralDirSize += curFile->fileNameLength;
		centralDirSize += curFile->extraFieldLength;

		// next.
		curFile = GetNextFile( curFile );
	}

	// make sure the terminator was set correctly.
	assert( curFile->sig == ZIP_CENTRAL_DIR_SIG );

	// resize the data for the central directory.
	zip->data = realloc( zip->data, zip->size + centralDirSize );

	// scan to the end of the file list.
	curFile = (zip_file_header_t *)zip->data;
	while ( curFile->sig == ZIP_HEADER_SIG )
		curFile = GetNextFile( curFile );
	char* dst = (char *)curFile;

	// create the central directory.
	unsigned int centralDirStart = ( unsigned int )( dst - (char *)zip->data );
	unsigned int count = 0;
	curFile = (zip_file_header_t *)zip->data;
	while ( curFile->sig == ZIP_HEADER_SIG )
	{
		// get the current entry.
		zip_central_dir_t *curEntry = (zip_central_dir_t *)dst;

		// fill out the current header.
		curEntry->sig = ZIP_CENTRAL_DIR_SIG;
		curEntry->versionMadeBy = 0;
		curEntry->versionNeeded = curFile->versionNeeded;
		curEntry->flags = curFile->flags;
		curEntry->method = curFile->method;
		curEntry->modFileTime = curFile->modFileTime;
		curEntry->modFileDate = curFile->modFileDate;
		curEntry->crc32 = curFile->crc32;
		curEntry->compressedSize = curFile->compressedSize;
		curEntry->uncompressedSize = curFile->uncompressedSize;
		curEntry->fileNameLength = curFile->fileNameLength;
		curEntry->extraFieldLength = curFile->extraFieldLength;
		curEntry->fileCommentLength = 0;
		curEntry->diskNumberStart = 0;
		curEntry->internalAttributes = 0;
		curEntry->externalAttributes = 0;
		curEntry->offsetOfLocalHeader = ( unsigned int )( (char *)curFile - (char *)zip->data );
		dst += sizeof( zip_central_dir_t );

		// write the file name.
		memcpy( dst, ( char* )( curFile + 1 ), curFile->fileNameLength );
		dst += curFile->fileNameLength;
		++count;

		// next.
		curFile = GetNextFile( curFile );
	}

	// fill out the end of the central directory record.
	zip_end_central_dir_t *endCentralDir = (zip_end_central_dir_t *)dst;
	endCentralDir->sig = ZIP_END_CENTRAL_DIR_SIG;
	endCentralDir->diskNumber = 0;
	endCentralDir->centralDirStartDisk = 0;
	endCentralDir->centralDirEntryCountOnDisk = count;
	endCentralDir->centralDirEntryCountTotal = count;
	endCentralDir->centralDirSize = ( unsigned int )centralDirSize - sizeof( zip_end_central_dir_t );
	endCentralDir->centralDirOffset = ( unsigned int )centralDirStart;
	endCentralDir->centralDirCommentLength = 0;
	dst += sizeof( zip_end_central_dir_t );

	// write out the data.
	fwrite( zip->data, dst - ( char* )zip->data, 1, fp );
}

//================================
// function:	GetNextFile
// purpose:		Gets the file after the file passed in.
//================================
zip_file_header_t *GetNextFile( zip_file_header_t *curFile )
{
	assert( curFile->sig == ZIP_HEADER_SIG );

	// calculate the source address.
	char *src = (char *)curFile;

	// calculate how far we need to step forward.
	size_t advance = sizeof( zip_file_header_t );
	advance += curFile->compressedSize;
	advance += curFile->fileNameLength;
	advance += curFile->extraFieldLength;

	// get the next header.
	zip_file_header_t *next = (zip_file_header_t *)(src + advance );

	// return the new zip file header.
	return next;
}
