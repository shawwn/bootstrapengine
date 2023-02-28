//----------------------------------------------------------
// File:        RFile.h
// Author:      Kevin Bray
// Created:     11-12-05
//
// Purpose:     To manage a text file.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "RRsrc.h"

//**********************************************************
// class RFile
//**********************************************************
class RFile : public RRsrc
{
public:
    RFile( const UPath& name, void* handle, unsigned int flags );
    ~RFile();

    // resets the file's handle and flags.
    void                    Reset( void* handle, unsigned int flags );

    // returns information about the file's allowed usage.
    bool                    CanRead();
    bool                    CanWrite();

    // returns true if the file exists.
    bool                    Exists() const;

    // reloads the data in the file.
    bool                    Reload() const;

    // purges any loaded data.
    void                    Purge() const;

    // returns a pointer to the data.  Note that the data we contain is always
    // null terminated (this is done for us by the file manager).
    const void*             GetData() const;
    __int64                 GetSize() const;

    // write the data.
    void                    WriteData( __int64 offset, const void* src, unsigned int size );

    // returns the file handle for use with Bootstrap FS.
    void*                   GetHandle() const       {   return _handle;         }

    // returns the file's creation flags.
    unsigned int            GetFlags() const        {   return _flags;          }

private:
    // ensure the data is around.
    void                    EnsureLoaded() const;

    void*                   _handle;
    mutable const void*     _data;
    mutable __int64         _size;
    unsigned int            _flags;
};
