//----------------------------------------------------------
// File:        MLine.h
// Author:      Kevin Bray
// Created:     09-05-05
//
// Purpose:     To represent a line in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

// forward declarations.
class UReader;
class UWriter;

//**********************************************************
// class MLine
//**********************************************************
class MLine
{
public:
    // dir is the direction vector, pt is any point on the line.
    MLine();
    MLine( const MVec3& dir, const MVec3& pt );
    ~MLine();

    const MVec3&    GetDir() const              {   return _dir;        }
    const MVec3&    GetPt() const               {   return _pt;         }

    void            SetDir( const MVec3& dir );
    void            SetPt( const MVec3& pt )    {   _pt = pt;           }

    // returns the shortest distance between the point and the line.
    float           Dist( const MVec3& pt );

    // returns true if the line is colinear.
    bool            IsCoLinear( const MLine& line, float tolerance ) const;

    void            Load( UReader& reader );
    void            Save( UWriter& writer ) const;

private:
    MVec3           _dir;
    MVec3           _pt;
};
