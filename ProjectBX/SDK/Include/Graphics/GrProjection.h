//----------------------------------------------------------
// File:        GrProjection.h
// Author:      Kevin Bray
// Created:     01-17-05
//
// Purpose:     To build and manage a projection matrix.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"
#include "MPlane.h"

//**********************************************************
// class GrProjection
//**********************************************************
class GrProjection
{
public:
    // perspective: note that the nearClip plane must be in view space.
    GrProjection( float fov, float farDist, float aspect, const MPlane& nearClip );

    // orthographic
    GrProjection( float left, float right, float bottom, float top, float far );

    // dtor
    ~GrProjection();

    bool            IsOrtho() const                         {   return _fov <= 0.01f;                       }

    // this calculates postion and angle offset values for a ray where z
    // is implied to be 1.
    void            BuildRayOffsets( float& px, float& py, 
                                     float& ax, float& ay,
                                     float x, float y );

    void            SetFOV( float fov );
    void            SetNearClip( const MPlane& nearClip )   {   _dirty = true;  _nearClip = nearClip;       }
    void            SetFarDist( float farDist )             {   _dirty = true;  _farDist = farDist;         }
    void            SetAspect( float aspect )               {   _dirty = true;  _aspect = aspect;           }
    void            SetLeft( float left )                   {   _dirty = true;  _left = left;               }
    void            SetRight( float right )                 {   _dirty = true;  _right = right;             }
    void            SetTop( float top )                     {   _dirty = true;  _top = top;                 }
    void            SetBottom( float bottom )               {   _dirty = true;  _bottom = bottom;           }

    float           GetFOV() const                          {   return _fov;                                }
    float           GetWidth() const                        {   return _right - _left;                      }
    float           GetHeight() const                       {   return _top - _bottom;                      }
    const MPlane&   GetNearClip() const                     {   return _nearClip;                           }
    float           GetFarDist() const                      {   return _farDist;                            }
    float           GetAspect() const                       {   return _aspect;                             }
    float           GetLeft() const                         {   return _left;                               }
    float           GetRight() const                        {   return _right;                              }
    float           GetTop() const                          {   return _top;                                }
    float           GetBottom() const                       {   return _bottom;                             }

    // returns a reference to the matrix.
    const MMat4x4&  GetMatrix() const;

private:
    void            AddNearClip() const;

    float           _fov;
    MPlane          _nearClip;
    float           _farDist;
    float           _aspect;    // width/height
    float           _left;      // for off center projections.
    float           _right;
    float           _top;
    float           _bottom;
    mutable MMat4x4 _mat;       // cached matrix.
    mutable bool    _dirty;
};
