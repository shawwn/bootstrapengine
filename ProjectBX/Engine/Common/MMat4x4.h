//----------------------------------------------------------
// File:        MMat4x4.h
// Author:      Kevin Bray
// Created:     09-25-04
//
// Purpose:     performs matrix operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

class MVec3;
class MVec4;
class MMat3x3;
class UReader;
class UWriter;

//**********************************************************
// class MMat4x4
//**********************************************************
class MMat4x4
{
public:
    static const float kInverseDetEpsilon;

    MMat4x4();
    MMat4x4( float scale );
    MMat4x4( const MVec3& translation );
    MMat4x4( const MMat3x3& rot );
    MMat4x4( const MMat3x3& rot, const MVec3& translation );
    MMat4x4( const MMat3x3& rot, const MVec3& translation, const MVec3& scale );
    MMat4x4( float _11, float _12, float _13, float _14,
             float _21, float _22, float _23, float _24,
             float _31, float _32, float _33, float _34,
             float _41, float _42, float _43, float _44 );
    MMat4x4( const float* mat );
    MMat4x4( const MMat4x4& mat );
    ~MMat4x4();

    static const MMat4x4&   Identity();

    MMat4x4&        operator = ( const MMat4x4& mat );

    bool            Inverse( MMat4x4& output ) const;
    void            Transpose() const;
    MMat4x4         Transposed() const;

    void            TransformCoordFast( MVec3& coord ) const;
    MVec3           TransformCoord( const MVec3& coord ) const;
    void            TransformCoordPrePersp( MVec4& result, const MVec3& coord ) const;

    // transforms a coordinate but skips the perspective divide.
    MVec3           TransformCoordNoPersp( const MVec3& coord ) const;
    void            TransformCoordNoPerspFast( MVec3& coord ) const;

    void            SetRotate( const MMat3x3& mat );
    void            SetTranslate( const MVec3& pos );
    void            SetScale( const MVec3& scale );
    void            SetOrientation( const MVec3& side, const MVec3& forward );
    void            SetRow( unsigned int row, const MVec4& rowVec );
    void            SetCol( unsigned int col, const MVec4& rowVec );

    void            GetRotate( MMat3x3& mat ) const;
    MMat3x3         GetRotate() const;
    void            GetTranslate( MVec3& pos ) const;
    MVec3           GetTranslate() const;
    void            GetScaleSqr( MVec3& scale ) const;
    MVec3           GetScaleSqr() const;
    void            GetScale( MVec3& scale ) const;
    MVec3           GetScale() const;
    void            GetRow( MVec4& rowVec, unsigned int row ) const;
    MVec4           GetRow( unsigned int row ) const;
    void            GetCol( MVec4& rowVec, unsigned int col ) const;
    MVec4           GetCol( unsigned int col ) const;

    void            GetAxis( MVec3& axis, unsigned int col ) const;

    float&          operator ()( unsigned int row, unsigned int col );
    float           operator ()( unsigned int row, unsigned int col ) const;
    MMat4x4         operator * ( const MMat4x4& mat ) const;

    const float*    GetDataStart() const    {   return _data;   }
    operator const float* () const          {   return _data;   }

    bool            IsIdentity() const;

    // saving and loading.
    void            Save( UWriter& writer ) const;
    void            Load( UReader& reader );

private:
    void            SetAxes( const MVec3& x, const MVec3& y, const MVec3& z );

    void            GetAlignedPtr();
    void            Copy( const MMat4x4& mat );

    static MMat4x4  _identity;

    float           _array[ 19 ];       // unaligned data.
    float *         _data;          // 16-byte aligned data.
};
