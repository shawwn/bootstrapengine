//----------------------------------------------------------
// File:        MoveGizmo.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "MoveGizmo.h"

// project headers.
#include "SceneEntity.h"
#include "Ray.h"

// engine headers.
#include "MRay.h"
#include "MLineSegment.h"
#include "GrColor.h"
#include "GrRenderUtil.h"

// .NET namespaces.
using namespace System::Runtime::InteropServices;

//**********************************************************
// ref class MoveGizmo
//**********************************************************

//----------------------------------------------------------
Bootstrap::MoveGizmo::MoveGizmo( float size )
    : _target( nullptr )
    , _size( size )
    , _startSegMu( 0.0f )
    , _moveMode( MoveMode::None )
{
    _axes = gcnew array< Vector3 >( 3 );
    _axes[ 0 ] = Vector3( _size, 0.0f, 0.0f );
    _axes[ 1 ] = Vector3( 0.0f, _size, 0.0f );
    _axes[ 2 ] = Vector3( 0.0f, 0.0f, _size );
    _highlightAxes = gcnew array< bool >( 3 );
}

//----------------------------------------------------------
Bootstrap::MoveGizmo::~MoveGizmo()
{
    this->!MoveGizmo();
}

//----------------------------------------------------------
Bootstrap::MoveGizmo::!MoveGizmo()
{
    _target = nullptr;
    _axes = nullptr;
}

//----------------------------------------------------------
void 
Bootstrap::MoveGizmo::SetEntity( SceneEntity^ target )
{
    _target = target;
}

//----------------------------------------------------------
bool 
Bootstrap::MoveGizmo::MouseDown( Ray ray, int mouseX, int mouseY )
{
    // simply return false if we don't have a target.
    if ( !_target )
        return false;

    // check to see if we're in range with the gizmo.
    if ( ray.Dist( _target->Position ) > _size )
        return false;

    // save the starting position of the drag operation.
    _startPos = _target->Position;

    // figure out which axis we're dragging along.
    float minDist = FLT_MAX;
    float rayMu = 0.0f;
    float segMu = 0.0f;
    MVec3 pos = _target->Position.ToMVec3();
    for ( int i = 0; i < 3; ++i )
    {
        float curDist = ray.Dist( MLineSegment( pos, pos + _axes[ i ].ToMVec3() ), rayMu, segMu );
        if ( curDist < minDist )
        {
            minDist = curDist;
            _moveMode = ( MoveMode )i;
            _startSegMu = segMu;
        }
    }

    // return true to indicate that we're now in a dragging state.
    return true;
}

//----------------------------------------------------------
void 
Bootstrap::MoveGizmo::MouseMove( Ray ray, int mouseX, int mouseY, bool active )
{
    if ( !_target )
        return;

    if( active )
    {
        // calculate a segment for the axis we're moving along.
        LineSegment segment( _startPos, _startPos + _axes[ ( int )_moveMode ] );

        // calculate the new position of the scene entity.
        float rayMu = 0.0f;
        float segMu = 0.0f;
        ray.Dist( segment, rayMu, segMu );

        // set the position of the entity we're moving.
        float segMuDelta = segMu - _startSegMu;
        _target->Position = _startPos + segMuDelta * _axes[ ( int )_moveMode ];
    }
    else
    {
        // clear the highlight flags.
        for( int i = 0; i < 3; ++i )
            _highlightAxes[ i ] = false;

        // check to see if we're in range with the gizmo.
        if ( ray.Dist( _target->Position ) > _size )
            return;

        // figure out which axis we would be dragging along.
        float minDist = FLT_MAX;
        float rayMu = 0.0f;
        float segMu = 0.0f;
        MVec3 pos = _target->Position.ToMVec3();
        int closestAxis = -1;
        for ( int i = 0; i < 3; ++i )
        {
            float curDist = ray.Dist( MLineSegment( pos, pos + _axes[ i ].ToMVec3() ), rayMu, segMu );
            if ( curDist < minDist )
            {
                minDist = curDist;
                closestAxis = i;
            }
        }

        // highlight the axis closest to the mouse.
        _highlightAxes[ closestAxis ] = true;
    }
}

//----------------------------------------------------------
void 
Bootstrap::MoveGizmo::MouseUp( Ray ray, int mouseX, int mouseY )
{
    if ( !_target )
        return;

    // all done.  
    _moveMode = MoveMode::None;

    // notify the target that we've finished moving it.
    _target->CommitTransform();
}

//----------------------------------------------------------
void 
Bootstrap::MoveGizmo::Render()
{
    if ( !_target )
        return;

    float h = _size;
    MVec3 pos = _target->Position.ToMVec3();
    MVec3 axes[ 3 ] = {
        MVec3( 1, 0, 0 ),   // X,
        MVec3( 0, 1, 0 ),   // Y,
        MVec3( 0, 0, 1 )    // Z,
    };

    GrColor xColor( 1.0f, 0.0f, 0.0f, 1.0f );
    GrColor yColor( 0.0f, 1.0f, 0.0f, 1.0f );
    GrColor zColor( 0.0f, 0.0f, 1.0f, 1.0f );
    GrColor selColor( 1.0f, 1.0f, 0.0f, 1.0f );

    gGrRenderUtil->DrawColoredLine( pos, pos + MVec3( _size, 0, 0 ), _highlightAxes[0] ? selColor : xColor );
    gGrRenderUtil->DrawColoredLine( pos, pos + MVec3( 0, _size, 0 ), _highlightAxes[1] ? selColor : yColor );
    gGrRenderUtil->DrawColoredLine( pos, pos + MVec3( 0, 0, _size ), _highlightAxes[2] ? selColor : zColor );

    // TODO: Add planar manipulators.
}
