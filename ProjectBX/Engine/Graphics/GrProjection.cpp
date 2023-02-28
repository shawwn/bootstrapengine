//----------------------------------------------------------
// File:        GrProjection.cpp
// Author:      Kevin Bray
// Created:     01-17-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrProjection.h"

// std c++ includes.
#include <math.h>

//**********************************************************
// class GrProjection
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrProjection::GrProjection( float fov, float farDist, float aspect, const MPlane& nearClipPlane )
: _fov( fov )
, _nearClip( nearClipPlane )
, _farDist( farDist )
, _aspect( aspect )
, _left( -1.0f )
, _right( 1.0f )
, _bottom( -1.0f )
, _top( 1.0f )
, _dirty( true )
{
    B_ASSERT( fov >= 0.0f );
}

//----------------------------------------------------------
GrProjection::GrProjection( float left, float right, float bottom, float top, float farDist )
: _fov( 0.0f )
, _farDist( farDist )
, _nearClip( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -1.0f ) )
, _aspect( 1.0f )
, _left( left )
, _right( right )
, _bottom( bottom )
, _top( top )
, _dirty( true )
{

}

//----------------------------------------------------------
GrProjection::~GrProjection()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrProjection::BuildRayOffsets( float& px, float& py, float& ax, float& ay, float x, float y )
{
    // either adjust the starting position or adjust the angles.
    if ( IsOrtho() )
    {
        // calculate interpolants.
        float tx = 0.5f * x + 0.5f;
        float ty = 0.5f * y + 0.5f;

        // calculate the starting position offset.
        px = Lerp( _left, _right, tx );
        py = Lerp( _bottom, _top, ty );
        ax = 0.0f;
        ay = 0.0f;
    }
    else
    {
        // calculate variables that reflect the current projection.
        float projNear = 1.0f / Tan( _fov * 0.5f );
        float xOffset = ( _right + _left ) / ( _right - _left );
        float yOffset = ( _top + _bottom ) / ( _top - _bottom );
        float xScale = 2.0f / ( _right - _left );
        float yScale = 2.0f / ( _top - _bottom );
        xScale /= _aspect;

        // calculate the new angle offsets.
        ax = ( x + xOffset ) / ( xScale * projNear );
        ay = ( y + yOffset ) / ( yScale * projNear );
        px = 0.0f;
        py = 0.0f;
    }
}

//----------------------------------------------------------
void
GrProjection::SetFOV( float fov )
{
    B_ASSERT( fov >= 0.0f );
    _dirty = true;
    _fov = fov;
}

//----------------------------------------------------------
const MMat4x4&
GrProjection::GetMatrix() const
{
    // update the cached projection matrix if necessary.
    if ( _dirty )
    {
        // make sure the width and height is valid.
        B_ASSERT( Abs( _right - _left ) > 0.0001f );
        B_ASSERT( Abs( _top - _bottom ) > 0.0001f );

        float xScale = 2.0f / ( _right - _left );
        float yScale = 2.0f / ( _top - _bottom );
        float xOffset = ( _right + _left ) / ( _right - _left );
        float yOffset = ( _top + _bottom ) / ( _top - _bottom );

        // calculate a projection matrix based on whether or not the
        // current configuration is orthographic or perspective.
        if ( !IsOrtho() )
        {
            // calculate matrix components.
            float projNear = 1.0f / Tan( _fov * 0.5f );

            // now build the matrix.
            _mat( 0, 0 ) = xScale * projNear / ( _aspect );
            _mat( 0, 1 ) = 0.0f;
            _mat( 0, 2 ) = xOffset;
            _mat( 0, 3 ) = 0.0f;

            _mat( 1, 0 ) = 0.0f;
            _mat( 1, 1 ) = yScale * projNear;
            _mat( 1, 2 ) = yOffset;
            _mat( 1, 3 ) = 0.0f;

            _mat( 2, 0 ) = 0.0f;
            _mat( 2, 1 ) = 0.0f;
            _mat( 2, 2 ) = -1.0f; //( projNear + _farDist ) / ( projNear - _farDist );
            _mat( 2, 3 ) = -1.0f; //_farDist * projNear / ( projNear - _farDist );

            _mat( 3, 0 ) = 0.0f;
            _mat( 3, 1 ) = 0.0f;
            _mat( 3, 2 ) = -1.0f;
            _mat( 3, 3 ) = 0.0f;

            // incorperate the near clip plane into the matrix by applying a shear
            // on the Z axis.
            AddNearClip();
        }
        else
        {
            // make sure a valid far distance was passed in.
            B_ASSERT( _farDist > 0.01f );

            // now build the matrix.
            _mat( 0, 0 ) = xScale;
            _mat( 0, 1 ) = 0.0f;
            _mat( 0, 2 ) = 0.0f;
            _mat( 0, 3 ) = -xOffset;

            _mat( 1, 0 ) = 0.0f;
            _mat( 1, 1 ) = yScale;
            _mat( 1, 2 ) = 0.0f;
            _mat( 1, 3 ) = -yOffset;

            _mat( 2, 0 ) = 0.0f;
            _mat( 2, 1 ) = 0.0f;
            _mat( 2, 2 ) = ( -1.0f / _farDist );
            _mat( 2, 3 ) = 0.0f;

            _mat( 3, 0 ) = 0.0f;
            _mat( 3, 1 ) = 0.0f;
            _mat( 3, 2 ) = 0.0f;
            _mat( 3, 3 ) = 1.0f;
        }

        // clear the dirty flag.
        _dirty = false;
    }

    // return the cached matrix.
    return _mat;
}

//----------------------------------------------------------
void
GrProjection::AddNearClip() const
{
/*
    // Calculate the clip-space corner point opposite the clipping plane
    // as ( sgn( clipPlane.x ), sgn( clipPlane.y ), 1, 1 ) and
    // transform it into camera space by multiplying it
    // by the inverse of the projection matrix
    float qx = ( Sign( _nearClip.GetNormal().GetX() ) + _mat( 0, 2 ) ) / _mat( 0, 0 );
    float qy = ( Sign( _nearClip.GetNormal().GetY() ) + _mat( 1, 2 ) ) / _mat( 1, 1 );
    float qz = -1.0f;
    float qw = ( 1.0f + _mat( 2, 2 ) ) / _mat( 2, 3 );

    float scale = 2.0f / ( 
        ( qx * _nearClip.GetNormal().GetX() ) +
        ( qy * _nearClip.GetNormal().GetY() ) +
        ( qz * _nearClip.GetNormal().GetZ() ) +
        ( qw * _nearClip.GetD() ) );

    MVec3 scaledNormal = scale * _nearClip.GetNormal();
    float scaledD = scale * _nearClip.GetD();

    // place the scaled plane into the third row of the projection matrix.
    _mat( 2, 0 ) = scaledNormal.GetX();
    _mat( 2, 1 ) = scaledNormal.GetY();
    _mat( 2, 2 ) = scaledNormal.GetZ() + 1.0f;
    _mat( 2, 3 ) = scaledD + 1.0f;
*/

    // since we're not using a far clip plane, we don't need to adjust for use
    // with the far clip plane.  That means we can just shove this plane into
    // the projection matrix.
    const MVec3& clipNormal = _nearClip.GetNormal();
    _mat( 2, 0 ) = clipNormal.GetX();
    _mat( 2, 1 ) = clipNormal.GetY();
    _mat( 2, 2 ) = clipNormal.GetZ();
    _mat( 2, 3 ) = _nearClip.GetD();
}

