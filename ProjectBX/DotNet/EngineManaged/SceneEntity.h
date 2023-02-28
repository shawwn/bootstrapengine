//----------------------------------------------------------
// File:		SceneEntity.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide a base class for entities within the scene.
//              Any class that inherits from SceneEntity can then
//              be manipulated with gizmos.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "Vector3.h"
#include "Matrix.h"

// forward declarations.
class MMat4x4;

namespace Bootstrap
{
    //**********************************************************
    // ref class SceneEntity
    //**********************************************************
	public ref class SceneEntity abstract
	{
	internal:
		SceneEntity( const MMat4x4& matrix, bool scalable );

	public:
		SceneEntity( Vector3 position, Vector3 rotation, Vector3 scale, bool scalable );

        // delegate declarations.
		delegate void TransformChangedHandler( SceneEntity^ entity );

        // events.
		event TransformChangedHandler^      TransformChanged;
		event TransformChangedHandler^      PositionChanged;
		event TransformChangedHandler^      RotationChanged;
		event TransformChangedHandler^      ScaleChanged;

		property Matrix                     Transform
		{
			Matrix                              get();
			void                                set( Matrix mat );
		}

		property Vector3                    Position
		{
			Vector3                             get();
			void                                set( Vector3 value );
		}

		property Vector3                    Rotation
		{
			Vector3                             get();
			void                                set( Vector3 value );
		}

		property Vector3                    Scale
		{
			Vector3                             get();
			void                                set( Vector3 value );
		}

		property bool                       Scalable
		{
			bool                                get()                   {   return _scalable;                   }
			void                                set( bool value )       {   _scalable = value;                  }
		}

		void                                SetTransform( Vector3 position, Vector3 rotation, Vector3 scale );

	protected:
        // called when a gizmo modifies the orientation of the entity.
		virtual void                        ApplyTransform( Matrix transform ) = 0;

        // called when a gizmo needs to know the orientation of the entity.
		virtual Matrix                      GetTransform() = 0;

    internal:

        // called when the user has stopped interacting with the active gizmo
        // (e.g. this is called on mouse up).
        virtual void                        CommitTransform() { }

    private:
		bool                                _scalable;
		Vector3                             _rotation;
	};
}
