//----------------------------------------------------------
// File:        MTransform.h
// Author:      Kevin Bray
// Created:     01-12-07
//
// Purpose:     To manage a transform.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "MVec3.h"

//**********************************************************
// class MTransform
//**********************************************************
class MTransform
{
public:
    MTransform();
    ~MTransform();

    // pivot points.
    void            SetRotatePivot( const MVec3& pivot );
    void            SetScalePivot( const MVec3& pivot );

    // sets the rotation, scale, or translation.  Note that the pivot points are
    // included.
    void            SetRotate( const MMat3x3& rotate );
    void            SetScale( float scale );
    void            SetTranslate( const MVec3& translate );

    // accessors.
    const MMat3x3&  GetRotate() const           {   return _rotate;     }
    float           GetScale() const            {   return _scale;      }
    const MVec3&    GetTranslate() const        {   return _translate;  }

    // returns the transformation matrix.
    const MMat4x4&  GetMatrix() const;

private:
    void            Update() const;

    // transformation matrix.
    mutable MMat4x4 _transform;

    // transform components.
    MMat3x3         _rotate;
    MVec3           _translate;
    float           _scale;

    // pivot points.
    MVec3           _rotatePivot;
    MVec3           _scalePivot;

    // dirty flag.
    mutable bool    _dirty;
};
