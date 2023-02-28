//----------------------------------------------------------
// File:        Camera.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Camera.h"

// project includes.
#include "Ray.h"
#include "Matrix.h"
#include "Projection.h"

// engine includes.
#include "GrScene.h"
#include "GrSubsys.h"
#include "MRay.h"

// .NET namespaces.
using namespace System::Windows::Forms;

//**********************************************************
// ref class Camera
//**********************************************************

//----------------------------------------------------------
Bootstrap::Camera::Camera( const GrCamera& camera )
{
    _camera = new GrCamera();
    *_camera = camera;

    // determine the yaw and pitch of the camera.
    float zRot = 0.0f;
    Rotation.ToEulerYXZ( _yawInDegrees, _pitchInDegrees, zRot );
}

//----------------------------------------------------------
Bootstrap::Camera::Camera()
{
    _camera = new GrCamera();
}

//----------------------------------------------------------
Bootstrap::Camera::Camera( Vector3 pos, Matrix3x3 rot, float fov, float farClip, float aspectRatio )
{
    _camera = new GrCamera( pos, rot.ToMMat3x3(), fov, farClip, aspectRatio );

    // determine the yaw and pitch of the camera.
    float zRot = 0.0f;
    Rotation.ToEulerYXZ( _yawInDegrees, _pitchInDegrees, zRot );
}

//----------------------------------------------------------
Bootstrap::Camera::Camera( Vector3 pos, Matrix3x3 rot, float farCull, float fov, float farClip, float aspectRatio )
{
    _camera = new GrCamera( pos, rot.ToMMat3x3(), farCull, fov, farClip, aspectRatio );

    // determine the yaw and pitch of the camera.
    float zRot = 0.0f;
    Rotation.ToEulerYXZ( _yawInDegrees, _pitchInDegrees, zRot );
}

//----------------------------------------------------------
Bootstrap::Camera::Camera( Vector3 pos, Matrix3x3 rot, Bootstrap::Projection proj )
{
    _camera = new GrCamera( pos, rot.ToMMat3x3(), proj.ToGrProjection() );

    // determine the yaw and pitch of the camera.
    float zRot = 0.0f;
    Rotation.ToEulerYXZ( _yawInDegrees, _pitchInDegrees, zRot );
}

//----------------------------------------------------------
Bootstrap::Camera::~Camera()
{
    this->!Camera();
}

//----------------------------------------------------------
Bootstrap::Camera::!Camera()
{
    delete _camera;
}

//----------------------------------------------------------
Bootstrap::Projection 
Bootstrap::Camera::Projection::get()
{
    return Bootstrap::Projection( _camera->GetProj() );
}

//----------------------------------------------------------
void 
Bootstrap::Camera::Projection::set( Bootstrap::Projection proj )
{
    _camera->SetProj( proj.ToGrProjection() );
}

//----------------------------------------------------------
void 
Bootstrap::Camera::AddUserFrustumPlane( Plane plane )
{
    _camera->AddUserFrustumPlane( plane.ToMPlane() );
}

//----------------------------------------------------------
void 
Bootstrap::Camera::RotateRelative( float yawDegrees, float pitchDegrees )
{
    _yawInDegrees += yawDegrees;
    _pitchInDegrees = Clamp( _pitchInDegrees + pitchDegrees, - 89.0f, 89.0f );

    // make our rotation.
    MMat3x3 yaw;
    MMat3x3 pitch;
    yaw.MakeYRotation( DegToRad( _yawInDegrees ) );
    pitch.MakeXRotation( DegToRad( _pitchInDegrees ) );

    // build the camera's full rotation by concatenating yaw and pitch.
    MMat3x3 camRot = yaw * pitch;

    // adjust for Z up if necessary.
    if ( gGrScene->GetZUp() )
    {
        // rotate the camera to look down positive Y.
        MMat3x3 adj( 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f,-1.0f,
            0.0f, 1.0f, 0.0f );
        camRot = adj * camRot;
    }

    // apply the rotation to the camera.
    _camera->SetRot( camRot );
}

//----------------------------------------------------------
Bootstrap::Ray 
Bootstrap::Camera::BuildRay( float x, float y )
{
    MRay ray;
    _camera->BuildRay( ray, x, y );
    return Ray( ray );
}

//----------------------------------------------------------
Bootstrap::Camera^ 
Bootstrap::Camera::BuildReflectedCamera( Plane plane )
{
    return gcnew Camera( _camera->BuildReflectedCamera( plane.ToMPlane() ) );
}

//----------------------------------------------------------
void 
Bootstrap::Camera::MemberwiseCopy( Camera^ camera )
{
    *_camera = *camera->_camera;
}
