//----------------------------------------------------------
// File:		Camera.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide a camera in a 3D scene.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrCamera.h"

// project includes.
#include "Vector3.h"
#include "Matrix.h"
#include "AABox.h"
#include "Frustum.h"

// forward declarations.
class GrCamera;

namespace Bootstrap 
{
    // .NET forward declarations.
	value class Vector3;
	value class Ray;
	value class Matrix3x3;
	value class Projection;
	value class Plane;
	ref class Frustum;

    //**********************************************************
    // ref class Camera
    //**********************************************************
	public ref class Camera
	{
	internal:
		Camera( const GrCamera& camera );

		GrCamera*		_camera;

	public:
		Camera();
		Camera( Vector3 pos, Matrix3x3 rot, float fov, float farClip, float aspectRatio );
		Camera( Vector3 pos, Matrix3x3 rot, float farCull, float fov, float farClip, float aspectRatio );
		Camera( Vector3 pos, Matrix3x3 rot, Bootstrap::Projection proj );

		~Camera();
		!Camera();

        // position, lookdir, projection.

		property Vector3		            Position
		{
			Vector3                             get()				    {   return _camera->GetPos();                               }
			void	                            set( Vector3 pos )	    {   _camera->SetPos( pos );                                 }
		}

		property Matrix3x3		            Rotation
		{
			Matrix3x3	                        get()					{   return Matrix3x3( _camera->GetRot() );                  }
			void		                        set( Matrix3x3 rot )	{   _camera->SetRot( rot.ToMMat3x3() );                     }
		}

		property Bootstrap::Projection		Projection
		{
			Bootstrap::Projection               get();
			void		                        set( Bootstrap::Projection proj );
		}

		// basis vector accessors.

		property Vector3		            Side		
        { 
            Vector3                             get()                   {   return Vector3(  _camera->GetRot().GetCol( 0 ) );       }
        }

		property Vector3		            Up			
        { 
            Vector3                             get()                   {   return Vector3(  _camera->GetRot().GetCol( 1 ) );       }       
        }

		property Vector3		            Forward		
        { 
            Vector3                             get()                   {   return Vector3( -_camera->GetRot().GetCol( 2 ) );       } 
        }

		// camera matrices.

		property Matrix			            ViewMatrix			
        { 
            Matrix                              get()                   {   return Matrix( _camera->GetViewMatrix() );              } 
        }

		property Matrix			            InvViewMatrix		
        { 
            Matrix                              get()                   {   return Matrix( _camera->GetInvViewMatrix() );           } 
        }

		property Matrix			            ProjMatrix			
        { 
            Matrix                              get()                   {   return Matrix( _camera->GetProjMatrix() );              } 
        }

		property Matrix			            ViewProjMatrix		
        { 
            Matrix                              get()                   {   return Matrix( _camera->GetViewProjMatrix() );          } 
        }

		property Matrix3x3		            NormalMatrix		
        { 
            Matrix3x3                           get()                   {   return Matrix3x3( _camera->GetNormalMatrix() );         } 
        }

		// camera frustum.
        property Bootstrap::Frustum^        Frustum		
        { 
            Bootstrap::Frustum^                 get()                   {   return gcnew Bootstrap::Frustum( _camera->GetFrustum() ); } 
        }

		// adds a frustum plane to the current frustum.  Note that user frustum
		// planes are cleared every time the camera is changed.
		void					            AddUserFrustumPlane( Plane plane );

        // camera manipulation methods.
		void					            Look( Vector3 direction )	{   _camera->Look( direction );                             }
		void					            LookAt( Vector3 target )	{   _camera->LookAt( Position, target );                    }
        void					            SetFarCull( float farCull ) {   _camera->SetFarCull( farCull );                         }
		void					            RotateRelative( float yawDegrees, float pitchDegrees );

		// build a ray in world space given a viewport coordinate.
		Ray						            BuildRay( float x, float y );

		// reflects the camera across the plane passed in.
		Camera^					            BuildReflectedCamera( Plane plane );

		// copies all members from the incoming camera into this camera.
		void					            MemberwiseCopy( Camera^ camera );

	private:
		float					            _yawInDegrees;
		float					            _pitchInDegrees;
	};
}