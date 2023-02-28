#include "common_afx.h"

// file header.
#include "RFileMgr.h"

// project includes.
#include "RFile.h"
#include "UWriter.h"

#define ARCHIVE_EXT         "zip"
#define FILE_FLAGS_MASK     ( FS_FILE_FLAG_READ | FS_FILE_FLAG_WRITE | FS_FILE_FLAG_USE_DMA ) 

RFileMgr* gRFileMgr = 0;

//**********************************************************
// class RFileMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
RFileMgr::RFileMgr( const UPath& rootPath )
: _rootPath( rootPath )
, _gate( LOCK_FREE )
{
    InitLock( &_gate );

    B_ASSERT( gRFileMgr == 0 );
    gRFileMgr = this;
}

//----------------------------------------------------------
RFileMgr::~RFileMgr()
{
    B_ASSERT( gRFileMgr != 0 );
    gRFileMgr = 0;
}


//==========================================================
// public methods
//==========================================================
/*
//----------------------------------------------------------
bool
RFileMgr::ReadFile( const UPath& path, const void** dataOut, unsigned long& size )
{
    B_ASSERT( dataOut != 0 );

    // determine the full path name.
    UPath name;

    // are we working with a full path or a relative path?
    CreateFullPath( name, path );

    // open the file using the flags specified.
    FILE* fp = 0;
    fopen_s( &fp, name.GetPathString().c_str(), "rb" );
    if ( fp != 0 )
    {
        fseek( fp, 0, SEEK_END );
        size = ( unsigned long )ftell( fp );
        void* data = ReadFileAlloc( size + 1 );
        fseek( fp, 0, SEEK_SET );
        fread( data, size, 1, fp );
        ( ( unsigned char* )data )[ size ] = 0;
        fclose( fp );
        *dataOut = data;
        return true;
    }

    *dataOut = 0;
    size = 0;
    return false;
}

//----------------------------------------------------------
void
RFileMgr::ReadFileMemFree( const void* ptr )
{
    delete[] ( char* )ptr;
}

//----------------------------------------------------------
void*
RFileMgr::ReadFileAlloc( size_t size )
{
    return new char[ size ];
}
*/
//----------------------------------------------------------
URef< RFile >
RFileMgr::GetFile( const UPath& path, Flags flags )
{
    // block until we can get at the file we need.
    while ( LockSwap( &_gate, LOCK_TAKEN ) == LOCK_TAKEN )
        LockPause();

    // determine the full path name.
    UPath fullName;
    CreateFullPath( fullName, path );

    // check to see if the file is already loaded.
    URef< RFile > file;
    FileMap::iterator iter = _files.find( fullName );
    if ( iter == _files.end() )
    {
        // Try to open the file.  If we can't, simply return null.
        void* fileHandle = FS_OpenFile( fullName.GetPathString().c_str(), ( unsigned int )flags );
        if ( !fileHandle )
        {
            // free the gate.
            LockSwap( &_gate, LOCK_FREE );

            // return null.
            return 0;
        }

        // create the file wrapper.
        file = new RFile( fullName, fileHandle, flags & FILE_FLAGS_MASK );

        // register the file with our internal database.
        _files[ fullName ] = file;
    }
    else
    {
        // get the file.
        file = iter->second;

        // check the file's creation flags.  Try to reopen the file
        // if needed.
        unsigned int curAccess = file->GetFlags();
        unsigned int newAccess = flags & ( FILE_FLAGS_MASK | FS_FILE_FLAG_CREATE_ALWAYS );
        if ( ( newAccess & curAccess ) != newAccess )
        {
            // adjust the flags to account for the additional access.
            unsigned int newFlags = ( curAccess | newAccess );

            // close the existing file and re-open it.
            FS_CloseFile( file->GetHandle() );
            void* fileHandle = FS_OpenFile( fullName.GetPathString().c_str(), newFlags );

            // reset the file with the new file information.
            file->Reset( fileHandle, newFlags );
        }
    }

    // free the gate.
    LockSwap( &_gate, LOCK_FREE );

    // return the file.
    return file;
}
/*
//----------------------------------------------------------
URef< RFileStream >
RFileMgr::GetFileStream( const UPath& path )
{
    // determine the full path name.
    UPath fileName;
    CreateFullPath( fileName, path );

    // check to see if the file stream already exists for the file specified.
    FileStreamMap::iterator iter = _fileStreams.find( fileName );
    if ( iter != _fileStreams.end() )
        return iter->second;

    // check to see if the file exists.  If not, simply return NULL.
    FILE* fp = 0;
    fopen_s( &fp, fileName.GetPathString().c_str(), "rb" );
    if ( fp == 0 )
        return 0;
    fclose( fp );

    // if we got here, then the file exists, so create the stream, add it to
    // our map, and return it.
    RFileStream* newStream = new RFileStream( fileName );
    _fileStreams[ fileName ] = newStream;
    return newStream;
}

//----------------------------------------------------------
bool
RFileMgr::WriteFile( const UPath& relativeFilePath, UWriter& writer )
{
    return WriteFile( relativeFilePath, writer.GetBufferPtr(), writer.GetBytesWritten() );
}

//----------------------------------------------------------
bool
RFileMgr::WriteFile( const UPath& relativeFilePath, const void* ptr, unsigned long size )
{
    // determine the full path name.
    UPath name;
    CreateFullPath( name, relativeFilePath );

    // write to the file on the disk.  We don't care if it came from an archive
    // originally.  This is because writing to an archive would be crazy slow.
    FILE* fp = 0;
    fopen_s( &fp, name.GetPathString().c_str(), "wb" );
    if ( fp == 0 )
        return false;
    fwrite( ptr, size, 1, fp );
    fclose( fp );
    return true;
}
*/
//----------------------------------------------------------
bool
RFileMgr::Exists( const UPath& relativeFilePath )
{
    // determine the full path name.
    UPath name;
    CreateFullPath( name, relativeFilePath );

    // check to see if the file exists.
    DWORD attributes = GetFileAttributes( name.GetPathString().c_str() );
    if ( attributes == INVALID_FILE_ATTRIBUTES )
    {
        GetLastError();
        return false;
    }

    // if we got here, then the file exists and therefor we should
    // return true.
    return true;
}

//----------------------------------------------------------
void
RFileMgr::CreateDir( const UPath& dirPath, bool relative )
{
    UPath curPath;
    if ( relative )
        curPath = _rootPath + dirPath;
    else
        curPath = dirPath;

    const tstring& dir = curPath.GetPathString();
    const char *separators = "/\\";

    tstring::size_type start = 0;
    tstring::size_type end = dir.find_first_of( separators, 0 );
    while ( end != tstring::npos )
    {
        end = dir.find_first_of( separators, ++end );
        ::CreateDirectory( curPath.GetPathString().substr( 0, end ).c_str(), 0 );
    }
}

//----------------------------------------------------------
void
RFileMgr::GetDirListing( std::vector< UPath >* files, std::vector< UPath >* folders, const UPath& relativeDirPath )
{
    // reserve some memory in our vector to keep from having to reallocate all
    // the time.
    if ( folders )
        folders->reserve( 256 );
    if ( files )
        files->reserve( 256 );

    // build a full path if a relative path was specified.
    UPath curPath;
    CreateFullPath( curPath, relativeDirPath );

    // get all child directories.
    WIN32_FIND_DATA findData;
    HANDLE findHandle = FindFirstFile( curPath.GetPathString().c_str(), &findData );
    if ( findHandle == INVALID_HANDLE_VALUE )
        return;

    do
    {
        // skip anything we shouldn't list.
        if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            if ( folders )
                if ( strcmp( findData.cFileName, ".." ) != 0 && strcmp( findData.cFileName, "." ) != 0 )
                    folders->push_back( findData.cFileName );
        }
        else 
        {
            if ( files )
                files->push_back( findData.cFileName );
        }

    } while ( FindNextFile( findHandle, &findData ) );

    // close the file find.
    FindClose( findHandle );
}

//----------------------------------------------------------
bool
RFileMgr::CopyFile( const UPath& dstPath, const UPath& srcPath )
{
    // calculate the paths.
    UPath localSrcPath;
    UPath localDstPath;
    CreateFullPath( localSrcPath, srcPath );
    CreateFullPath( localDstPath, dstPath );

    // open the source file.
    URef< RFile > srcFile = GetFile( localSrcPath, kFlagRead );
    if ( !srcFile )
        return false;

    // open the destination file.
    URef< RFile > dstFile = GetFile( localDstPath, kFlagCreateAlways | kFlagWrite );
    if ( !dstFile )
        return false;

    // make sure the source file isn't too big.
    if ( srcFile->GetSize() > ~0U )
        return false;

    // copy the data.
    dstFile->WriteData( 0, srcFile->GetData(), ( unsigned int )srcFile->GetSize() );

    // return true to indicate success.
    return true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
RFileMgr::CreateFullPath( UPath& fullPath, const UPath& path )
{
    // if there is a ':' in the file name, then a drive was specified!
    if ( path.GetPathString().find( ':' ) == tstring::npos )
        fullPath = _rootPath + path;
    else
        fullPath = path;
}
