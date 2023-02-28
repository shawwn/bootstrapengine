#pragma once

#include <stdio.h>

#define ZIP_METHOD_NONE			0
#define ZIP_METHOD_DEFLATE		1

// zip file info.
struct zip_t
{
	// data.
	size_t size;
	void* data;
};

struct zip_file_header_t;

// zip creation functions.
extern void Zip_Create( zip_t *zip );
extern void Zip_Load( zip_t *zip, FILE *fp );
extern void Zip_Free( zip_t *zip );

// zip file iteration functions.
extern size_t Zip_CalcFileCount( zip_t *zip );
extern zip_file_header_t *Zip_FindFile( zip_t *zip, const char* fileName );
extern zip_file_header_t *Zip_GetFirstFile( zip_t *zip );
extern zip_file_header_t *Zip_GetNextFile( zip_file_header_t *curFile );
extern size_t Zip_GetFileName( char* buf, size_t bufSize, zip_file_header_t *curFile );
extern size_t Zip_GetFileSize( zip_file_header_t *curFile );
extern size_t Zip_GetCompressedFileSize( zip_file_header_t *curFile );
extern size_t Zip_GetFileNames( char** fileNames, zip_t* zip );

// rebuilds the zip file so that items come in the order specified.
extern size_t Zip_Rebuild( zip_t *zip, const char *const *orderedNames, size_t nameCount );

// zip modification functions.
extern size_t Zip_ReadFile( void** dst, zip_file_header_t *file );
extern void Zip_RemoveFile( zip_t *zip, zip_file_header_t *file );
extern void Zip_AddFile( zip_t *zip, const char *name, const void *data, size_t size, unsigned int method );

// zip saving.
extern void Zip_Save( FILE *fp, zip_t *zip );
