//----------------------------------------------------------
// File:        EdImportUtils.cpp
// Author:      Kevin Bray
// Created:     12-23-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdUtil.h"

// graphics headers.
#include "GrTypes.h"

//----------------------------------------------------------
UPath
EdFixupTexturePath( const UPath& outputPath, const UPath& fileName )
{
    // find the file name.
    size_t pos = fileName.GetPathString().find_last_of( '/' );
    if ( pos != tstring::npos )
        ++pos;
    else
        pos = 0;

    // make the relative path.
    return outputPath + UPath( "textures" ) + fileName.GetPathString().substr( pos );
}

//----------------------------------------------------------
bool
EdIsTriDegenerate( const SVec3& p0, const SVec3& p1, const SVec3& p2, float epsilon )
{
    // compute the triangle's normal.
    SVec3 normal( ( p0 - p1 ).Cross( p2 - p1 ) );

    // if any component of the normal isn't close to zero, then the triangle isn't degenerate.
    for ( int i = 0; i < 3; ++i )
    {
        if ( !ApproxEqual( normal[ i ], 0.0f, FLT_EPSILON ) )
            return false;
    }

    // all components are close to zero, so the triangle is degenerate.
    return true;
}
