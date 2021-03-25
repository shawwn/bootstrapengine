//----------------------------------------------------------
// File:		RotateGizmo.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To provide a UI to enable an artist to rotate a 
//              scene entity.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "Gizmo.h"

// project includes.
#include "Vector3.h"

// forward declarations.
class MVec3;
class MRay;
class GrColor;
class MLineSegment;

namespace Bootstrap
{
    // .NET forward declarations.
    value class Ray;

    //**********************************************************
    // ref class RotateGizmo
    //**********************************************************
	public ref class RotateGizmo : public IGizmo
	{
	public:
		RotateGizmo( float size );
		~RotateGizmo();
		!RotateGizmo();

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
        // enum class RotateAxis
        //**********************************************************
		enum class RotateAxis
		{
			// current movement axis.
			X = 0,
			Y = 1,
			Z = 2,
			None,
		};

		// makes a circle.
        void        		    			MakeCircle( MLineSegment* segments, const MVec3& translate, int c0,
                    									int c1, float scale );
		void			                    RenderCircle( MLineSegment* segments, const GrColor& color );
		float			                    GetDistToCircle( MLineSegment* segments, const MRay& ray );
		MLineSegment*	                    GetCircle( RotateAxis axis );

		// current entity.
		SceneEntity^	                    _target;

		// size of the gizmo.
		float			                    _size;
		array< bool >^	                    _highlightAxes;
		
		// the circle.  One per axis.
		MLineSegment*	                    _circles;
		
		// drag state.
		int				                    _startX;
		int				                    _startY;
		RotateAxis		                    _rotateAxis;
	};
}
