//----------------------------------------------------------
// File:        Vector4.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To perform vector operations in 4D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec4;
struct SVec4;

namespace Bootstrap 
{
    //**********************************************************
    // value class Vector4
    //**********************************************************
    public value class Vector4
    {
    internal:
        Vector4( const MVec4& vec );
        Vector4( const SVec4& vec );

        MVec4                       ToMVec4();
        SVec4                       ToSVec4();

    public:
        Vector4( float x, float y, float z, float w );

        property float              X;
        property float              Y;
        property float              Z;
        property float              W;

        float                       Dot( Vector4 v );

        static Vector4              operator + ( Vector4 left, Vector4 right );
        static Vector4              operator - ( Vector4 left, Vector4 right );
        static Vector4              operator - ( Vector4 value );
        static Vector4              operator * ( Vector4 vector, float scale );
        static Vector4              operator * ( float scale, Vector4 vector );
        static Vector4              operator / ( Vector4 vector, float scale );

        // pretty-prints the vector.
        virtual System::String^     ToString() override;

        // equality.
        virtual bool                Equals( System::Object^ obj ) override;
        virtual int                 GetHashCode() override;

    internal:
        static                      operator MVec4( Vector4 v );
        static                      operator Vector4( const MVec4& v );
    };
}
