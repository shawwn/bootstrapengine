//----------------------------------------------------------
// File:        MMat3x3.h
// Author:      Kevin Bray
// Created:     09-25-04
//
// Purpose:     performs matrix operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

class MVec3;
class UReader;
class UWriter;

//**********************************************************
// class MMat3x3
//**********************************************************
class MMat3x3
{
public:
    MMat3x3();
    MMat3x3( float y, float x, float z );
    MMat3x3( float _11, float _12, float _13,
             float _21, float _22, float _23,
             float _31, float _32, float _33 );
    MMat3x3( const MVec3& axis, float angle );
    MMat3x3( const MVec3& row0, const MVec3& row1, const MVec3& row2 );
    MMat3x3( const MMat3x3& mat );
    ~MMat3x3();

    MMat3x3&        operator = ( const MMat3x3& mat );

    void            ToEulerYXZ( float& y, float& x, float& z );
    void            FromEulerYXZ( float y, float z, float x );

//  void            MakeRotation( float yaw, float pitch, float roll );
    void            MakeXRotation( float angleInRad );
    void            MakeYRotation( float angleInRad );
    void            MakeZRotation( float angleInRad );
    void            MakeRotation( const MVec3& axis, float angle );
    MVec3           RotatePoint( const MVec3& point ) const;
    void            RotatePointFast( MVec3& point ) const;

    void            SetRow( unsigned int i, const MVec3& row );
    void            GetRow( MVec3& rowOut, unsigned int i ) const;
    MVec3           GetRow( unsigned int i ) const;
    void            SetCol( unsigned int i, const MVec3& col );
    void            GetCol( MVec3& colOut, unsigned int i ) const;
    MVec3           GetCol( unsigned int i ) const;
    void            Transpose();
    MMat3x3         Transposed() const;
    bool            Inverse();
    bool            InverseTransposed( MMat3x3& output ) const;

    float&          operator ()( unsigned int i, unsigned int j );
    float           operator ()( unsigned int i, unsigned int j ) const;
    MMat3x3         operator * ( const MMat3x3& mat ) const;

    const float *   GetDataStart() const    {   return _data;   }
    operator const float* () const          {   return _data;   }

    // saving and loading.
    void            Save( UWriter& writer ) const;
    void            Load( UReader& reader );

private:
    void            Copy( const MMat3x3& mat );

    float           _data[ 9 ];
};
