//----------------------------------------------------------
// File:		ScaleGizmo.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide a UI to enable an artist to scale a 
//              scene entity.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "Gizmo.h"

// project includes.
#include "Ray.h"
#include "Vector3.h"

// forward declarations.
class MVec3;

namespace Bootstrap
{
    //**********************************************************
    // ref class ScaleGizmo
    //**********************************************************
	public ref class ScaleGizmo : public IGizmo
	{
	public:
		ScaleGizmo( float size );
		~ScaleGizmo();
		!ScaleGizmo();

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
        // enum class ScaleMode
        //**********************************************************
		enum class ScaleMode
		{
			// current movement axis.
			X = 0,
			Y = 1,
			Z = 2,
			XYZ = 3,
			None,
		};

		// current entity.
		SceneEntity^	                    _target;
		array<Vector3>^	                    _axes;

		// size of the gizmo.
		float			                    _size;
		
		// the circle.  One per axis.
		MLineSegment*	                    _circles;
		
		// drag state.
		int				                    _startX;
		int				                    _startY;
		ScaleMode		                    _scaleMode;

		// highlight flags.
		array< bool >^	                    _highlightAxes;
	};
}
