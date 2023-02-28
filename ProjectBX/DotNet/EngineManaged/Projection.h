//----------------------------------------------------------
// File:        Projection.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To build and manage a projection matrix.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Plane.h"
#include "Matrix.h"

// using namespaces.
using System::Runtime::InteropServices::OutAttribute;

// forward declarations.
class GrProjection;

namespace Bootstrap
{
    //**********************************************************
    // value class Projection
    //**********************************************************
    public value class Projection
    {
    internal:
        Projection( const GrProjection& projection );

        GrProjection                        ToGrProjection();

    public:
        // perspective: note that the nearClip plane must be in view space.
        Projection( float fov, float farDist, float aspect, Plane nearClip );

        // orthographic
        Projection( float left, float right, float bottom, float top, float far );

        property bool                       IsOrtho 
        { 
            bool                                get()                   {   return _fov <= 0.01f;                   } 
        }

        // this calculates postion and angle offset values for a ray where z
        // is implied to be 1.
        void                                BuildRayOffsets( [Out] float% px, [Out] float% py, 
                                                             [Out] float% ax, [Out] float% ay,
                                                             float x, float y );

        property float                      FOV
        {
            float                               get()                   {   return _fov;                            }
            void                                set( float fov );
        }

        property Plane                      NearClip
        {
            Plane                               get()                   {   return _nearClip;                       }
            void                                set( Plane nearClip )   {   _dirty = true; _nearClip = nearClip;    }
        }

        property float                      FarDistance
        {
            float                               get()                   {   return _farDist;                        }
            void                                set( float farDist )    {   _dirty = true; _farDist = farDist;      }
        }

        property float                      AspectRatio
        {
            float                               get()                   {   return _aspect;                         }
            void                                set( float aspect )     {   _dirty = true; _aspect = aspect;        }
        }

        property float                      Left
        {
            float                               get()                   {   return _left;                           }
            void                                set( float left )       {   _dirty = true; _left = left;            }
        }

        property float                      Right
        {
            float                               get()                   {   return _right;                          }
            void                                set( float right )      {   _dirty = true; _right = right;          }
        }

        property float                      Top
        {
            float                               get()                   {   return _top;                            }
            void                                set( float top )        {   _dirty = true; _top = top;              }
        }

        property float                      Bottom
        {
            float                               get()                   {   return _bottom;                         }
            void                                set( float bottom )     {   _dirty = true; _bottom = bottom;        }
        }

        property float                      Width
        {
            float                               get()                   {   return _right - _left;                  }
        }
        
        property float                      Height
        {
            float                               get()                   {   return _top - _bottom;                  }
        }

        property Matrix                     Transform
        {
            Matrix                              get();
        }

    private:
        void                                AddNearClip();

        float                               _fov;
        Plane                               _nearClip;
        float                               _farDist;
        float                               _aspect;    // width/height
        float                               _left;      // for off center projections.
        float                               _right;
        float                               _top;
        float                               _bottom;
        Matrix                              _mat;       // cached matrix.
        bool                                _dirty;
    };
}