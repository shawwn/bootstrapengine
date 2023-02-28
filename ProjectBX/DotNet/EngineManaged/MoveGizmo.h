//----------------------------------------------------------
// File:        MoveGizmo.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To provide a UI to enable an artist to move a 
//              scene entity.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "Gizmo.h"

// project headers.
#include "Vector3.h"

// forward declarations.
class MVec3;

namespace Bootstrap
{
    // .NET forward declarations.
    ref class SceneEntity;
    value class Ray;

    //**********************************************************
    // ref class MoveGizmo
    //**********************************************************
    public ref class MoveGizmo : public IGizmo
    {
    public:
        MoveGizmo( float size );
        ~MoveGizmo();
        !MoveGizmo();

        // entity management.
        virtual void                        SetEntity( SceneEntity^ target );

        // mouse events.
        virtual bool                        MouseDown( Ray ray, int mouseX, int mouseY );
        virtual void                        MouseMove( Ray ray, int mouseX, int mouseY, bool active );
        virtual void                        MouseUp( Ray ray, int mouseX, int mouseY );

        // rendering.
        virtual void                        Render();

    private:
        //**********************************************************
        // enum class MoveMode
        //**********************************************************
        enum class MoveMode
        {
            // current movement axis.
            X = 0,
            Y = 1,
            Z = 2,

            // currently not supported.
            XY = 3,
            XZ = 4,
            YZ = 5,

            None,
        };

        // current entity.
        SceneEntity^                        _target;
        array<Vector3>^                     _axes;

        // size of the gizmo.
        float                               _size;
        array< bool >^                      _highlightAxes;

        // drag state.
        Vector3                             _startPos;
        float                               _startSegMu;
        MoveMode                            _moveMode;
    };
}
