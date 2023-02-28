//----------------------------------------------------------
// File:        MCone.h
// Author:      Kevin Bray
// Created:     09-04-05
//
// Purpose:     To represent a cone in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"

// forward declarations.
class MPlane;

//**********************************************************
// class MCone
//**********************************************************
class MCone
{
public:
    MCone( MVec3 point, MVec3 base, float baseRadius );
    ~MCone();

    void            SetPoint( const MVec3& point )  {   _point = point;     }
    void            SetBase( const MVec3& base )    {   _base = base;       }
    void            SetBaseRadius( float radius );

    const MVec3&    GetPoint() const                {   return _point;      }
    const MVec3&    GetBase() const                 {   return _base;       }
    float           GetBaseRadius() const           {   return _baseRadius; }

    int             GetSide( const MPlane& plane ) const;

private:
    MVec3           _point;
    MVec3           _base;
    float           _baseRadius;
};
