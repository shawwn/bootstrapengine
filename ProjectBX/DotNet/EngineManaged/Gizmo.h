//----------------------------------------------------------
// File:        Gizmo.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To provide a standard gizmo interface.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

namespace Bootstrap
{
    value class Ray;
    ref class SceneEntity;

    public interface class IGizmo
    {
        virtual void SetEntity( SceneEntity^ target ) = 0;
        virtual bool MouseDown( Ray ray, int mouseX, int mouseY ) = 0;
        virtual void MouseMove( Ray ray, int mouseX, int mouseY, bool active ) = 0;
        virtual void MouseUp( Ray ray, int mouseX, int mouseY ) = 0;

        virtual void Render() = 0;
    };
}