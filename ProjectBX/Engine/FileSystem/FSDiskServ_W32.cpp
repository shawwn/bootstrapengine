//----------------------------------------------------------
// File:        FSDiskServ.cpp
// Author:      Kevin Bray
// Created:     10-21-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "filesystem_afx.h"

// file header.
#include "FSDiskServ.h"

// standard C includes.
#include <process.h>

// Win32 includes.
#include <WinIoCtl.h>
#include <tchar.h>

// various constants.  These two numbers multiplied by eachother
// determine the maximum number of files that can be open
// simultaneously.
#define BLOCKS_PER_ALLOCATION       1024
#define MAX_BLOCK_COUNT             1024

// this determines the maximum number of simultaneous asynchronous
// read/write operations.
#define MAX_PENDING_IO              1024
#define MAX_DRIVE_COUNT             32

// drive types.
#define DTYPE_UNKNOWN               0
#define DTYPE_HDD                   1
#define DTYPE_CD                    2
#define DTYPE_DVD                   3
//#define DTYPE_HDDVD               4
//#define DTYPE_BLUERAY             5

// drive cache sizes - This indicates the size of the cache on drives
// of the specific type.  These numbers are set low to ensure the
// best possible performance on low end drives.
#define DISKCACHE_SIZE_HDD              262144
#define DISKCACHE_SIZE_CD               131072
#define DISKCACHE_SIZE_DVD              131072

#define LOOKAHEAD_SIZE_HDD              65536
#define LOOKAHEAD_SIZE_CD               65536
#define LOOKAHEAD_SIZE_DVD              65536

#define LOOKAHEAD_COUNT_HDD             128
#define LOOKAHEAD_COUNT_CD              512
#define LOOKAHEAD_COUNT_DVD             512

#define SEEKBACK_SIZE_HDD               8388608
#define SEEKBACK_SIZE_CD                2097152
#define SEEKBACK_SIZE_DVD               4194304

//==========================================================
// internal structures.
//==========================================================

typedef struct SFile_t
{
    volatile HANDLE handle;                     // Win32 file handle.
    unsigned short  flags;                      // creation flags.
    unsigned short  drive;                      // the drive on which the file is located.
    FSsize          offset;                     // absolute file offset.
    FSsize          physAddr;                   // start on the hard-drive.
    volatile struct SFile_t* nextFree;          // next free pointer.
} SFile;

typedef struct SFileOp_t
{
    OVERLAPPED              overlapped;
    SFSFileReadBuffer*      readBuffer;         // non-zero for a read event.
    SFSFileWriteBuffer*     writeBuffer;        // non-zero for a write event.
    SFile_t*                file;               // file pointer.
    unsigned int            size;               // amount of data to read/write.
    unsigned int            retired;            // prefetch retire flag.
    volatile int            pending;            // whether or not this operation has completed.
} SFileOp;

typedef struct SDrive_t
{
    HANDLE                  handle;             // handle to the drive.
    unsigned int            type;               // CD-ROM, DVD-ROM, HDD, SSD, unknown.
    unsigned int            bufferSize;         // size of the drive's cache in bytes.
    unsigned int            maxLASize;          // maximum size of a look-ahead read.
    unsigned int            maxLACount;         // maximum number of reads to look-ahead.
    unsigned int            maxSeekBack;        // maximum number of bytes to read-back.
    unsigned int            bytesPerSector;     // number of bytes per sector.
    unsigned int            sectorsPerCluster;  // number of sectors per cluster.
    unsigned int            bytesPerCluster;    // number of bytes per cluster.
} SDrive;

//==========================================================
// Memory management routines.
//==========================================================

void* ( *FSmalloc )( size_t size ) = malloc;
void ( *FSfree )( void* mem ) = free;
TCHAR* ConvertString( const FSchar* string );
void FreeString( TCHAR* string );

//==========================================================
// SFile free list variables.
//==========================================================

static volatile SFile* volatile _freeList = 0;
static volatile SFile* volatile _blocks[ MAX_BLOCK_COUNT ];
static volatile int _freeListGate = 0;
static volatile int _freeRegistered = 0;

//==========================================================
// SDrive management variables.
//==========================================================

static volatile SDrive* _drives[ MAX_DRIVE_COUNT ];
static volatile int _driveLock = 0;

//==========================================================
// Internal functions.
//==========================================================

// internal startup/shutdown.
static void Start();
static void Stop();
static void InitDrive( char letter );
static int GetDriveIndex( char letter );
static void FlushDriveQueue( int index );
static void ThreadMain( void* );
static void CALLBACK IOComplete( DWORD dwErrorCode, DWORD dwBytesTransfered, LPOVERLAPPED overlapped );
static FSsize CalcPhysAddr( SFileOp* op );

// worker thread handle.
static uintptr_t _workerThread = 0;

//==========================================================
// Thread communication variables.
//==========================================================

// worker thread communication.
static volatile int _threadExit = 0;
static SFileOp _ops[ MAX_DRIVE_COUNT ][ MAX_PENDING_IO ];
static volatile int _queuePushLock[ MAX_DRIVE_COUNT ];
static volatile int _queuePushIdx[ MAX_DRIVE_COUNT ];
static volatile int _queuePopIdx[ MAX_DRIVE_COUNT ];


//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
void FS_ScanDrives()
{
    // enumerate through logical drives.
    DWORD bitMask = 1;
    DWORD driveMask = GetLogicalDrives();
    for ( unsigned int i = 'A'; i < 'Z'; ++i, bitMask += bitMask )
    {
        if ( ( driveMask & bitMask ) != 0 )
            InitDrive( i );
    }
}

//----------------------------------------------------------
void* FS_OpenFile( const FSchar* name, unsigned int flags )
{
    // First, immediately open the file.
    // determine the Win32 access flags.
    unsigned int win32Flags = 0;
    if ( flags & FS_FILE_FLAG_READ )
        win32Flags |= GENERIC_READ;
    if ( flags & FS_FILE_FLAG_WRITE )
        win32Flags |= GENERIC_WRITE;

    // determine the Win32 creation flags.
    unsigned int win32CreationMode = OPEN_EXISTING;
    if ( flags & FS_FILE_FLAG_CREATE_ALWAYS )
        win32CreationMode = CREATE_ALWAYS;
    else if ( flags & FS_FILE_FLAG_OPEN_ALWAYS )
        win32CreationMode = OPEN_ALWAYS;

    // determine the Win32 attribute flags.
    unsigned int win32AttrFlags = FILE_FLAG_OVERLAPPED;
    if ( flags & FS_FILE_FLAG_USE_DMA )
        win32AttrFlags |= FILE_FLAG_NO_BUFFERING;

    // determine the Win32 file sharing flags.
    unsigned int win32ShareMode = FILE_SHARE_READ;
    if ( !( flags & FS_FILE_FLAG_WRITE ) )
        win32ShareMode |= FILE_SHARE_WRITE;

    // map the incoming string from UTF-8 to UCS-2.
    TCHAR* ucsName = ConvertString( name );

    // try open the file.  If the file cannot be opened, simply
    // return NULL.
    HANDLE handle = CreateFile( ucsName, win32Flags, win32ShareMode, NULL, win32CreationMode, win32AttrFlags, NULL );
    if ( handle == INVALID_HANDLE_VALUE )
    {
        FreeString( ucsName );
        return 0;
    }

    // determine the length of the full path name of the file.
    DWORD bufferLen = 1;
    TCHAR temp = 0;
    bufferLen = GetFullPathName( ucsName, bufferLen, &temp, NULL ) + 1;

    // get the file's full path name.
    TCHAR* fullPath = ( TCHAR* )FSmalloc( bufferLen * sizeof( TCHAR ) );
    DWORD result = GetFullPathName( ucsName, bufferLen, fullPath, NULL );

    // Now determine what drive the file lies on.
    unsigned short driveIndex = MAX_DRIVE_COUNT - 1;
    if ( result != 0 && result <= bufferLen )
    {
        // if we have a drive specifier, get the drive information.
        if ( fullPath[ 1 ] == _T( ':' ) )
        {
            unsigned short t0 = ( fullPath[ 0 ] - _T( 'A' ) );
            unsigned short t1 = ( fullPath[ 0 ] - _T( 'a' ) );
            driveIndex = min( t0, t1 );
        }
    }

    // free the full path.
    FSfree( fullPath );

    // free the UCS-2 string.
    FreeString( ucsName );

    // ensure that *nothing* about the file is cached in memory.
    if ( flags & FS_FILE_FLAG_USE_DMA )
        FlushFileBuffers( handle );

    // if we got here, then we need to grab a new file entry to
    // return to the user.  Spin until we gain control of the free
    // list.
    while ( _FS_Swap( &_freeListGate, 1 ) )
        _FS_Pause();

    // allocate an item from the free list.
    if ( !_freeList )
    {
        if ( !_freeRegistered )
            Start();

        // make sure there is room for a new block.
        if ( _freeRegistered == MAX_BLOCK_COUNT )
            return 0;

        // allocate file structures, aligned on 2K boundaries.  Note
        // that we simply use the alignment to determine where new
        // memory blocks begin to keep from having to track blocks.
        // This may be wasteful, depending on how the AlignedAlloc()
        // is implemented.
        SFile* memory = ( SFile* )_FS_AlignedAlloc( BLOCKS_PER_ALLOCATION * sizeof( SFile ), 32 );

        // link the file structures together.
        for ( unsigned int i = 1; i < BLOCKS_PER_ALLOCATION; ++i )
            memory[ i - 1 ].nextFree = memory + i;

        // terminate the last link with null.
        memory[ BLOCKS_PER_ALLOCATION - 1 ].nextFree = 0;
        _freeList = memory;

        // register the block and advance the '_freeRegistered'
        // count.
        _blocks[ _freeRegistered ] = _freeList;
        _FS_Increment( &_freeRegistered );
    }

    // grab a free file entry.
    volatile SFile* fileEntry = _freeList;
    _freeList = _freeList->nextFree;

    // release the free list gate.
    _FS_Swap( &_freeListGate, 0 );

    // fill out the file-entry's data.
    fileEntry->handle = handle;
    fileEntry->flags = flags;
    fileEntry->drive = driveIndex;
    fileEntry->offset = 0;
    fileEntry->physAddr = 0;
    fileEntry->nextFree = 0;

    // get the file's physical address.
    STARTING_VCN_INPUT_BUFFER inputVCN;
    inputVCN.StartingVcn.HighPart = 0;
    inputVCN.StartingVcn.LowPart = 0;
    RETRIEVAL_POINTERS_BUFFER fileAddr;
    DWORD bytesReturned = 0;
    OVERLAPPED overlapped;
    overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;
    BOOL ret = DeviceIoControl( handle, FSCTL_GET_RETRIEVAL_POINTERS, &inputVCN, sizeof( inputVCN ),
        &fileAddr, sizeof( fileAddr ), &bytesReturned, &overlapped );

    // if we successfully issued a command for the file's physical
    // address, set it to the file entry.  Otherwise, just assume
    // that the file is at location 0.  Note that on NTFS systems,
    // small files are stored in the MFT if possible.
    DWORD error = GetLastError();
    if ( ret || error == ERROR_MORE_DATA )
    {
        // wait for the DeviceIoControl() operation to complete.
        WaitForSingleObject( overlapped.hEvent, INFINITE );

        // store the file's location on the disk.
        fileEntry->physAddr = MAKE_FSIZE( fileAddr.Extents[ 0 ].Lcn.LowPart, fileAddr.Extents[ 0 ].Lcn.HighPart );
        fileEntry->physAddr *= _drives[ driveIndex ]->bytesPerCluster;
    }

    // free the event we created for the DeviceIoControl operation.
    CloseHandle( overlapped.hEvent );

    // return the file-entry pointer.
    return ( void* )fileEntry;
}

//----------------------------------------------------------
void FS_CloseFile( void* fileHandle )
{
    // get the file's entry.
    volatile SFile* fileEntry = ( volatile SFile* )fileHandle;

    // ensure that that handle is set to the invalid value.  Simply
    // return if the file has already been closed.
    HANDLE handle = ( HANDLE )_FS_INT_TO_PTR(
        _FS_Swap( ( volatile int* )( &fileEntry->handle ), _FS_PTR_TO_INT( INVALID_HANDLE_VALUE ) )
        );
    if ( handle == INVALID_HANDLE_VALUE )
        return;

    // flush the drive's queue to ensure that all read and write
    // operations are finished.
    FlushDriveQueue( fileEntry->drive );

    // close the Win32 file.
    CloseHandle( handle );

    // free the file-entry.
    while ( _FS_Swap( &_freeListGate, 1 ) )
        _FS_Pause();
    fileEntry->nextFree = _freeList;
    _freeList = fileEntry;
    _FS_Swap( &_freeListGate, 0 );
}

//----------------------------------------------------------
FSsize FS_GetFileSize( void* fileHandle )
{
    // get the file's entry.
    volatile SFile* fileEntry = ( volatile SFile* )fileHandle;

    // flush the drive's queue to ensure that all write operations
    // are finished.
    FlushDriveQueue( fileEntry->drive );

    // now get the file's size.
    LARGE_INTEGER size;
    GetFileSizeEx( fileEntry->handle, &size );

    // return the file size.
    return MAKE_FSIZE( size.LowPart, size.HighPart );
}

//----------------------------------------------------------
void FS_ReadFile( SFSFileReadBuffer* dst, void* file, FSsize offset, unsigned int size )
{
    // get the file handle.
    SFile* fileEntry = ( SFile* )file;
    unsigned short drive = fileEntry->drive;

    // acquire the push lock.
    while ( _FS_Swap( _queuePushLock + drive, 1 ) != 0 )
        _FS_Pause();

    // determine if there is room on the queue.  If not, we need
    // to wait until there is.
    while ( ( _queuePushIdx[ drive ] - _queuePopIdx[ drive ] ) == MAX_PENDING_IO )
        _FS_Pause();

    // issue the read on our worker thread.
    volatile SFileOp* curOp = _ops[ drive ] + _queuePushIdx[ drive ] % MAX_PENDING_IO;

    // fill out the current operation.
    while ( _FS_Swap( &curOp->pending, 1 ) != 0 )
        _FS_Pause();

    // if we got here, then the command can be safely issued.
    curOp->readBuffer = dst;
    curOp->writeBuffer = 0;
    curOp->size = size;
    curOp->file = fileEntry;
    curOp->retired = 0;
    curOp->overlapped.Internal = 0;
    curOp->overlapped.InternalHigh = 0;
    curOp->overlapped.Offset = ( DWORD )offset;
    curOp->overlapped.OffsetHigh = ( DWORD )( offset >> 32 );
    curOp->overlapped.hEvent = 0;

    // advance the push index.
    _FS_Increment( _queuePushIdx + drive );

    // release the push lock.
    _FS_Swap( _queuePushLock + drive, 0 );
}

//----------------------------------------------------------
void FS_WriteFile( void* file, FSsize offset, SFSFileWriteBuffer* src, unsigned int size )
{
    // get the file handle.
    SFile* fileEntry = ( SFile* )file;
    unsigned short drive = fileEntry->drive;

    // acquire the push lock.
    while ( _FS_Swap( _queuePushLock + drive, 1 ) != 0 )
        _FS_Pause();

    // determine if there is room on the queue.  If not, we need
    // to wait until there is.
    while ( ( _queuePushIdx[ drive ] - _queuePopIdx[ drive ] ) == MAX_PENDING_IO )
        _FS_Pause();

    // issue the write on our worker thread.
    volatile SFileOp* curOp = _ops[ drive ] + _queuePushIdx[ drive ] % MAX_PENDING_IO;

    // fill out the current operation.
    while ( _FS_Swap( &curOp->pending, 1 ) != 0 )
        _FS_Pause();

    // if we got here, then the command can be safely issued.
    curOp->readBuffer = 0;
    curOp->writeBuffer = src;
    curOp->size = size;
    curOp->file = fileEntry;
    curOp->retired = 0;
    curOp->overlapped.Internal = 0;
    curOp->overlapped.InternalHigh = 0;
    curOp->overlapped.Offset = ( DWORD )offset;
    curOp->overlapped.OffsetHigh = ( DWORD )( offset >> 32 );
    curOp->overlapped.hEvent = 0;

    // advance the push index.
    _FS_Increment( _queuePushIdx + drive );

    // release the push lock.
    _FS_Swap( _queuePushLock + drive, 0 );
}

//----------------------------------------------------------
void FS_Wait( volatile int* gate )
{
    // wait for a file operation to be completed.
    while ( _FS_Swap( gate, 0 ) == 0 )
        _FS_Pause();
}

//----------------------------------------------------------
bool FS_IsComplete( volatile int* gate )
{
    // Return true if we reset the gate from non-zero to 0.
    return ( _FS_Swap( gate, 0 ) != 0 );
}

//----------------------------------------------------------
bool FS_IsReadComplete( SFSFileReadBuffer* readBuffer )
{
    return ( FS_IsComplete( &readBuffer->complete ) );
}

//----------------------------------------------------------
bool FS_IsWriteComplete( SFSFileWriteBuffer* writeBuffer )
{
    return ( FS_IsComplete( &writeBuffer->complete ) );
}


//**********************************************************
// File local functions
//**********************************************************

//----------------------------------------------------------
TCHAR* ConvertString( const FSchar* string )
{
#if !defined FS_USE_WIDECHAR
    size_t len = strlen( string );
    TCHAR* tstr = ( TCHAR* )FSmalloc( ( len + 1 ) * sizeof( TCHAR ) );
    MultiByteToWideChar( CP_UTF8, 0, string, -1, tstr, ( int )len );
    tstr[ len ] = L'\0';
    return tstr;
#else
    return string;
#endif
}

//----------------------------------------------------------
void FreeString( TCHAR* string )
{
#if !defined FS_USE_WIDECHAR
    FSfree( string );
#endif
}

//----------------------------------------------------------
void Start()
{
    // register our cleanup function.
    atexit( Stop );

    // clear all drives.
    for ( unsigned int i = 0; i < MAX_DRIVE_COUNT; ++i )
        _drives[ i ] = 0;

    // scan for drives and get their information.
    FS_ScanDrives();

    // start the worker thread.
    _workerThread = _beginthread( ThreadMain, 1024, 0 );
}

//----------------------------------------------------------
void Stop()
{
    // kill the worker thread.
    _FS_Swap( &_threadExit, 1 );
    Sleep( 1 );

    // free all allocated blocks.
    for ( int i = 0; i < _freeRegistered; ++i )
        _FS_AlignedFree( ( void* )_blocks[ i ] );

    // free all drive references.
    for ( unsigned int i = 0; i < MAX_DRIVE_COUNT; ++i )
    {
        if ( _drives[ i ] )
        {
            // close the drive handle.
            if ( _drives[ i ]->handle != INVALID_HANDLE_VALUE )
                CloseHandle( _drives[ i ]->handle );

            // free the drive data.
            FSfree( ( void* )_drives[ i ] );
        }
    }
}

//----------------------------------------------------------
void InitDrive( char letter )
{
    // pack the letter into a UTF-8 string.
    char buffer[ 7 ] = "\\\\.\\_:";
    buffer[ 4 ] = ( char )toupper( letter );

    // convert the name to unicode.
    TCHAR driveName[ 7 ];
    int result = MultiByteToWideChar( CP_UTF8, 0, buffer, -1, driveName, 7 );
    if ( result == 0 )
    {
        // return if the drive letter specified isn't a valid unicode
        // character.
        DWORD error = GetLastError();
        return;
    }

    // open the volume.
    HANDLE volume = CreateFile( driveName, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING,
        NULL );
    if ( volume == INVALID_HANDLE_VALUE )
    {
        DWORD error = GetLastError();
        return;
    }

    // get the drive's index.
    int index = GetDriveIndex( letter );
    _drives[ index ] = ( SDrive* )FSmalloc( sizeof( SDrive_t ) );

    // fill out the drive structure.
    _drives[ index ]->handle = volume;
    _drives[ index ]->type = DTYPE_UNKNOWN;
    _drives[ index ]->bufferSize = 0;

    // get information about the volume's type.
    DWORD bytesReturned = 0;
    GET_MEDIA_TYPES* mediaTypes = ( GET_MEDIA_TYPES* )FSmalloc( 4096 );
    if ( DeviceIoControl( volume, IOCTL_STORAGE_GET_MEDIA_TYPES_EX, NULL, 0,
        mediaTypes, 4096, &bytesReturned, NULL ) )
    {
        // store the media type.
        DWORD deviceType = mediaTypes->DeviceType;
        DEVICE_MEDIA_INFO* mediaInfo = mediaTypes->MediaInfo;

        // figure out what kind of device we're working with.
        if ( deviceType == FILE_DEVICE_DVD )
            _drives[ index ]->type = DTYPE_DVD;
        else if ( deviceType == FILE_DEVICE_CD_ROM )
            _drives[ index ]->type = DTYPE_CD;
        else if ( deviceType == FILE_DEVICE_DISK )
            _drives[ index ]->type = DTYPE_HDD;
    }
    else
    {
        DWORD error = GetLastError();
        error = error;
    }
    FSfree( mediaTypes );

#if 0
    // get information about the volume's cache.
    DISK_CACHE_INFORMATION diskCacheInfo;
    if ( DeviceIoControl( volume, IOCTL_DISK_GET_CACHE_INFORMATION, NULL, 0,
        &diskCacheInfo, sizeof( diskCacheInfo ), &bytesReturned, NULL ) )
    {
        // store the cache information.

    }
    else
    {
        DWORD error = GetLastError();
        error = error;
    }
#else
    // make some assumptions about caches based on the type of the
    // drive.
    if ( _drives[ index ]->type == DTYPE_HDD )
    {
        _drives[ index ]->bufferSize = DISKCACHE_SIZE_HDD;
        _drives[ index ]->maxLASize = LOOKAHEAD_SIZE_HDD;
        _drives[ index ]->maxLACount = LOOKAHEAD_COUNT_HDD;
        _drives[ index ]->maxSeekBack = SEEKBACK_SIZE_HDD;
    }
    else if ( _drives[ index ]->type == DTYPE_DVD )
    {
        _drives[ index ]->bufferSize = DISKCACHE_SIZE_DVD;
        _drives[ index ]->maxLASize = LOOKAHEAD_SIZE_DVD;
        _drives[ index ]->maxLACount = LOOKAHEAD_COUNT_DVD;
        _drives[ index ]->maxSeekBack = SEEKBACK_SIZE_DVD;
    }
    else if ( _drives[ index ]->type == DTYPE_CD )
    {
        _drives[ index ]->bufferSize = DISKCACHE_SIZE_CD;
        _drives[ index ]->maxLASize = LOOKAHEAD_SIZE_CD;
        _drives[ index ]->maxLACount = LOOKAHEAD_COUNT_CD;
        _drives[ index ]->maxSeekBack = SEEKBACK_SIZE_CD;
    }
#endif

    // get information about the volume's layout.
    DWORD bytesPerSector = 0;
    DWORD sectorsPerCluster = 0;
    DWORD freeClusters = 0;
    DWORD totalClusters = 0;
    if ( GetDiskFreeSpace( driveName + 4, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters ) )
    {
        _drives[ index ]->bytesPerSector = bytesPerSector;
        _drives[ index ]->sectorsPerCluster = sectorsPerCluster;
        _drives[ index ]->bytesPerCluster = bytesPerSector * sectorsPerCluster;
    }
    else
    {
        DWORD error = GetLastError();
        error = error;
    }
}

//----------------------------------------------------------
int GetDriveIndex( char letter )
{
    // set the drive for all transactions that do not specify a
    // drive.
    unsigned char d0 = ( unsigned char )( letter - 'A' );
    unsigned char d1 = ( unsigned char )( letter - 'a' );
    return ( int )min( d0, d1 );
}

//----------------------------------------------------------
void FlushDriveQueue( int drive )
{
    // acquire the push lock.
    while ( _FS_Swap( _queuePushLock + drive, 1 ) != 0 )
        _FS_Pause();

    // wait for the queue to empty.  This ensures that all reads and
    // writes to the file finish as needed.
    while ( ( _queuePushIdx[ drive ] - _queuePopIdx[ drive ] ) > 0 )
        _FS_Pause();

    // release the push lock.
    _FS_Swap( _queuePushLock + drive, 0 );
}

//----------------------------------------------------------
void ThreadMain( void* )
{
    // keep a read/write statistics.
    unsigned int readCount = 0;
    unsigned int writeCount = 0;

    // allocate a table to keep track of drive heads.
    FSsize curDriveLoc[ MAX_DRIVE_COUNT ];
    memset( curDriveLoc, 0, sizeof( curDriveLoc ) );

    // create a bullshit event to make WaitForSingleObjectEx.
    HANDLE alwaysSet = CreateEvent( NULL, TRUE, FALSE, NULL );

    // run until we're told to exit.
    while ( _FS_Swap( &_threadExit, 0 ) == 0 )
    {
        // flag that can be used to determine if any file read/write
        // commands were issued.
        bool workDone = false;

        // iterate through the queues of each drive.
        for ( unsigned int i = 0; i < MAX_DRIVE_COUNT; ++i )
        {
            // get the current drive.
            volatile SDrive* drive = _drives[ i ];
            if ( !drive )
                continue;

            // determine if there is anything to do on the current
            // drive.
            if ( _queuePushIdx[ i ] == _queuePopIdx[ i ] )
                continue;

            // if we got here, then we have work to do.
            SFileOp* curOp = _ops[ i ] + _queuePopIdx[ i ] % MAX_PENDING_IO;

            // determine if the current operation has been retired.
            if ( curOp->retired )
            {
                // clear the retired flag and advance to the next
                // operation.
                curOp->retired = 0;
                _FS_Increment( _queuePopIdx + i );
                continue;
            }

            // look-ahead op flag used to specify whether we're
            // handling the original operation, or a look-ahead
            // operation.
            bool laOpFound = false;

            // scan ahead to determine if something else would be
            // better to read.
            FSsize targetAddr = CalcPhysAddr( curOp );
            unsigned int maxScan = ( unsigned int )min( ( int )drive->maxLACount, ( _queuePushIdx[ i ] - _queuePopIdx[ i ] ) );
            unsigned int scanIdx = _queuePopIdx[ i ] + 1;
            if ( targetAddr >= curDriveLoc[ i ] )
            {
                for ( unsigned int j = 1; j < maxScan; ++j, ++scanIdx )
                {
                    // get the operation at the current location.
                    SFileOp* lookAheadOp = _ops[ i ] + ( scanIdx % MAX_PENDING_IO );

                    // if we hit a write operation, stop scanning.
                    // We need to ensure that reads and writes
                    // happen in the same order that they're issued.
                    if ( lookAheadOp->writeBuffer != 0 && ( lookAheadOp->file->flags & FS_FILE_WRITE_REORDER ) == 0 )
                        break;

                    // if we've already handled the current operation,
                    // simply skip it.
                    if ( lookAheadOp->retired > 0 )
                        continue;

                    // if the operation is too big, then it's not an
                    // acceptable look-ahead operation.
                    if ( lookAheadOp->size > drive->maxLASize )
                        continue;

                    // check it's address.  Are we going to hit the
                    // look-ahead operation on our way to the desired
                    // location?  If so, make it the current operation.
                    FSsize curAddr = CalcPhysAddr( lookAheadOp );
                    if ( curAddr > curDriveLoc[ i ] && curAddr < targetAddr )
                    {
                        targetAddr = curAddr;
                        curOp = lookAheadOp;
                        laOpFound = true;
                    }
                }
            }
            else
            {
                // scan backwards for anything else that might be a
                // better read.
                FSsize baseAddr = targetAddr;
                for ( unsigned int j = 1; j < maxScan; ++j, ++scanIdx )
                {
                    // get the operation at the current location.
                    SFileOp* lookAheadOp = _ops[ i ] + ( scanIdx % MAX_PENDING_IO );
                    
                    // if we hit a write operation, stop scanning.
                    // We need to ensure that reads and writes
                    // happen in the same order that they're issued.
                    if ( lookAheadOp->writeBuffer != 0 && ( lookAheadOp->file->flags & FS_FILE_WRITE_REORDER ) == 0 )
                        break;

                    // if we've already handled the current operation,
                    // simply skip it.
                    if ( lookAheadOp->retired > 0 )
                        continue;

                    // if the operation is too big, then it's not an
                    // acceptable look-ahead operation.
                    if ( lookAheadOp->size > drive->maxLASize )
                        continue;

                    // check it's address.  Since we're seeking in
                    // reverse, find an address that's further back
                    // to maximize forward reading.
                    FSsize curAddr = CalcPhysAddr( lookAheadOp );
                    if ( curAddr < targetAddr )
                    {
                        if ( ( baseAddr - curAddr ) < drive->maxSeekBack )
                        {
                            targetAddr = curAddr;
                            curOp = lookAheadOp;
                            laOpFound = true;
                        }
                    }
                }
            }

            // get the current file we're going to read.
            SFile* file = curOp->file;

            // set the workDone to indicate that we've done
            // something.
            workDone = true;

            // determine whether or not we should read or write.
            if ( curOp->readBuffer )
            {
                assert( !curOp->writeBuffer );

                // issue a read.
                if ( !ReadFileEx( file->handle, ( void* )curOp->readBuffer->buffer, curOp->size,
                    ( LPOVERLAPPED )&curOp->overlapped, IOComplete ) )
                {
                    // if there was an error, mark the operation as
                    // complete to avoid FS_Wait() deadlocks.
                    _FS_Swap( &curOp->readBuffer->complete, 1 );
                }
                ++readCount;
            }
            else
            {
                // issue a write.
                if ( !WriteFileEx( file->handle, ( void* )curOp->writeBuffer->buffer, curOp->size,
                    ( LPOVERLAPPED )&curOp->overlapped, IOComplete ) )
                {
                    // if there was an error, mark the operation as
                    // complete to avoid FS_Wait() deadlocks.
                    _FS_Swap( &curOp->writeBuffer->complete, 1 );
                }
                ++writeCount;
            }

            // track the location of the disk's read/write heads.
            curDriveLoc[ i ] = targetAddr;

            // only advance the pop index if we handled the original
            // request (instead of a look-ahead request).
            if ( laOpFound )
                curOp->retired = 1;
            else
                _FS_Increment( _queuePopIdx + i );
        }

        // check the APC queue for file read/write callbacks.
        DWORD result = WaitForSingleObjectEx( alwaysSet, 0, TRUE );
        result = result;
    }
//  timeEndPeriod( 1 );
    CloseHandle( alwaysSet );

    readCount = readCount;
    writeCount = writeCount;
}

//----------------------------------------------------------
void CALLBACK IOComplete( DWORD dwErrorCode, DWORD dwBytesTransfered, LPOVERLAPPED overlapped )
{
    // get the file op by casting the overlapped structure pointer.
    // Note that this works because the overlapped member of the
    // SFileOp_t structure is the first member.
    volatile SFileOp* fileOp = ( volatile SFileOp* )overlapped;

    // mark the operation as completed.
    if ( fileOp->readBuffer )
        _FS_Swap( &fileOp->readBuffer->complete, 1 );
    else
        _FS_Swap( &fileOp->writeBuffer->complete, 1 );

    // clear the pending flag.
    _FS_Swap( &fileOp->pending, 0 );
}

//----------------------------------------------------------
FSsize CalcPhysAddr( SFileOp* op )
{
    // calculate the physical location of the read requested.
    FSsize targetAddr = op->file->physAddr;
    FSsize fileOffset = MAKE_FSIZE( op->overlapped.Offset, op->overlapped.OffsetHigh );

    // return the final address on disk.
    return targetAddr + fileOffset;
}
