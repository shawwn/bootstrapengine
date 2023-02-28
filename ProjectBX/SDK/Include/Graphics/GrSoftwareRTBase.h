//----------------------------------------------------------
// File:        GrSoftwareRTBase.h
// Author:      Kevin Bray
// Created:     08-14-08
//
// Purpose:     To provide an interface for a software rendering
//              target.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class GrSoftwareRTBase
//**********************************************************
class GrSoftwareRTBase
{
public:
    virtual ~GrSoftwareRTBase() = 0 { }

    // size information.
    virtual unsigned int    GetBytesPerPel() const = 0;
    virtual unsigned int    GetWidth() const = 0;
    virtual unsigned int    GetHeight() const = 0;
    virtual unsigned int    GetTile8x8Stride() const = 0;

    // tile begin and end.
    virtual unsigned char*  BeginTile8x8( unsigned int x, unsigned int y ) = 0;
    virtual unsigned char*  BeginTile8x8WriteOnly( unsigned int x, unsigned int y ) = 0;
    virtual void            EndTile8x8( unsigned int x, unsigned int y ) = 0;

private:
};
