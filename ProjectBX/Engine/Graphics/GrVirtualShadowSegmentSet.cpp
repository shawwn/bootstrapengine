//----------------------------------------------------------
// File:        GrVirtualShadowSegmentSet.cpp
// Author:      Kevin Bray
// Created:     05-05-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrVirtualShadowSegmentSet.h"

// project includes.
#include "GrCamera.h"
#include "GrConfig.h"

const float kMinAABoxDim = 0.0009765625f;

//**********************************************************
// class GrVirtualShadowSegment
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrVirtualShadowSegmentSet::GrVirtualShadowSegmentSet( const GrCamera& camera, unsigned int segmentCount, float firstSize )
: _segments( new SSegment[ segmentCount ] )
, _segmentCount( segmentCount )
, _frustum( camera.GetFrustum() )
{
    // get the frustum and the camera's look vector.
    const GrFrustum& frustum = camera.GetFrustum();
    const MVec3& cameraPos = camera.GetPos();
    MVec3 lookVec = camera.GetLookDir();

    // generate vectors that point along the edges of the frustum.
    MVec3 cornerVectors[ 4 ];

    // bottom left.
    cornerVectors[ 0 ] = frustum.GetFarBottomLeft() - frustum.GetNearBottomLeft();
    cornerVectors[ 0 ] *= 1.0f / cornerVectors[ 0 ].Dot( lookVec );

    // top left.
    cornerVectors[ 1 ] = frustum.GetFarTopLeft() - frustum.GetNearTopLeft();
    cornerVectors[ 1 ] *= 1.0f / cornerVectors[ 1 ].Dot( lookVec );

    // top right.
    cornerVectors[ 2 ] = frustum.GetFarTopRight() - frustum.GetNearTopRight();
    cornerVectors[ 2 ] *= 1.0f / cornerVectors[ 2 ].Dot( lookVec );

    // bottom right.
    cornerVectors[ 3 ] = frustum.GetFarBottomRight() - frustum.GetNearBottomRight();
    cornerVectors[ 3 ] *= 1.0f / cornerVectors[ 3 ].Dot( lookVec );

    // iterate through each segment and calculate the corner points.
    float length = firstSize;
    float start = 0.0f;
    float lengthMul = 1.0f / Tan( 0.5f * camera.GetProj().GetFOV() );
    unsigned int lastSegment = segmentCount - 1;
    for ( unsigned int i = 0; i < segmentCount; ++i )
    {
        // force the last segment to be very large.
        if ( i == lastSegment )
            length = 500.0f;

        // calculate the near and far corners.
        float end = start + length;
        unsigned int s = 0;
        for ( unsigned int j = 0; j < 4; ++j )
        {
            _segments[ i ].corners[ j+0 ] = cameraPos + start * cornerVectors[ j ];
            _segments[ i ].corners[ j+4 ] = cameraPos + end * cornerVectors[ j ];
        }

        // store the start and end distances.
        _segments[ i ].start = start;
        _segments[ i ].end = end;

        // increase the current segment's length.
        start += length;
        length *= lengthMul;
    }
}

//----------------------------------------------------------
GrVirtualShadowSegmentSet::~GrVirtualShadowSegmentSet()
{
    delete[] _segments;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrVirtualShadowSegmentSet::UpdateSegments( const GrCamera& faceCamera, float border, float distBias )
{
    for ( unsigned int i = 0; i < _segmentCount; ++i )
    {
        SSegment& curSegment = _segments[ i ];

        // check to see if the segment is inside of the shadow frustum.
        const GrFrustum& faceFrustum = faceCamera.GetFrustum();
        curSegment.visible = IsSegmentVisible( curSegment, faceFrustum );

        // if the segment isn't visible, we don't need to do any other processing.
        if ( !curSegment.visible )
            continue;

        // get the world to view matrix and the view to proj matrices.
        const MMat4x4& worldToViewMat = faceCamera.GetViewMatrix();
        const MMat4x4& viewToProjMat = faceCamera.GetProjMatrix();

        // now calculate the segment min/max.
        MVec3 curPos;
        float maxZ = -1.0f;
        MVec3 minExts( FLT_MAX, FLT_MAX, FLT_MAX );
        MVec3 maxExts( -FLT_MAX, -FLT_MAX, -FLT_MAX );
        for ( unsigned int j = 0; j < 8; ++j )
        {
            // transform each segment vertex into the shadow camera's post
            // projection space.  Note that we need to preserve W for
            // various special case situations.
            curPos = curSegment.corners[ j ];
            worldToViewMat.TransformCoordNoPerspFast( curPos );

            // take the min/max in post projection space.
            minExts.SetX( Min( minExts.GetX(), curPos.GetX() ) );
            minExts.SetY( Min( minExts.GetY(), curPos.GetY() ) );
            minExts.SetZ( Min( minExts.GetZ(), curPos.GetZ() ) );
            maxExts.SetX( Max( maxExts.GetX(), curPos.GetX() ) );
            maxExts.SetY( Max( maxExts.GetY(), curPos.GetY() ) );
            maxExts.SetZ( Max( maxExts.GetZ(), curPos.GetZ() ) );
        }

        // are we in front of the box entirely?  Should this even be possible?
        // if we're doing a segment-frustum check, then this shouldn't
        // happen.
        if ( minExts.GetZ() > 0.0f )
        {
            // if we're entirely in front of the box, simply mark the
            // current segment as not visible.
            curSegment.visible = false;
            continue;
        }

        // is this test even worth anything?  This seems like a bug.
#if 1
        if ( !faceCamera.GetProj().IsOrtho() && minExts.GetZ() < 0.0f && maxExts.GetZ() > faceCamera.GetProj().GetNearClip().GetD() )
        {
            // if we're inside the box, simply set the 2D bounds to -1..1.
            curSegment.aaBox.lowerLeft.x = -1.0f;
            curSegment.aaBox.lowerLeft.y = -1.0f;
            curSegment.aaBox.upperRight.x = 1.0f;
            curSegment.aaBox.upperRight.y = 1.0f;
            curSegment.aaBox.clipLogX = 0;
            curSegment.aaBox.clipLogY = 0;
            curSegment.aaBox.z = viewToProjMat.TransformCoord( minExts ).GetZ();    // do we even need z?
            return;
        }
#endif

        // is the light "inside" of this segment?  Note that this test is only
        // valid for non-orthogonal projections.  Otherwise, this test simply
        // wastes shadow resolution.
        if ( !faceCamera.GetProj().IsOrtho() && _frustum.IsPointInside( faceFrustum.GetPos() ) )
        {
            // we only care if the point is within the current segment.
            float dist = _frustum.GetCullPlane( GrFrustum::ECP_NEAR ).Dist( faceFrustum.GetPos() );
            if ( dist >= curSegment.start && dist <= curSegment.end )
            {
                // if we're inside the box, simply set the 2D bounds to -1..1.
                curSegment.aaBox.lowerLeft.x = -1.0f;
                curSegment.aaBox.lowerLeft.y = -1.0f;
                curSegment.aaBox.upperRight.x = 1.0f;
                curSegment.aaBox.upperRight.y = 1.0f;
                curSegment.aaBox.clipLogX = 0;
                curSegment.aaBox.clipLogY = 0;
                curSegment.aaBox.z = viewToProjMat.TransformCoord( minExts ).GetZ();    // do we even need z?
                return;
            }
        }

        // get the front and back face corners.
        MVec3 nearBottomLeft(   minExts.GetX(), minExts.GetY(), maxExts.GetZ()  );
        MVec3 nearTopRight(     maxExts.GetX(), maxExts.GetY(), maxExts.GetZ()  );
        MVec3 farBottomLeft(    minExts.GetX(), minExts.GetY(), minExts.GetZ()  );
        MVec3 farTopRight(      maxExts.GetX(), maxExts.GetY(), minExts.GetZ()  );

        // if we got here, then the box is in front of the camera.  Therefor,
        // we should transform it into screen space.
        viewToProjMat.TransformCoordFast( nearBottomLeft );
        viewToProjMat.TransformCoordFast( nearTopRight );
        viewToProjMat.TransformCoordFast( farBottomLeft );
        viewToProjMat.TransformCoordFast( farTopRight );

        // set the 2D box.
        curSegment.aaBox.lowerLeft.x = Min( nearBottomLeft.GetX(), farBottomLeft.GetX() );
        curSegment.aaBox.lowerLeft.y = Min( nearBottomLeft.GetY(), farBottomLeft.GetY() );
        curSegment.aaBox.upperRight.x = Max( nearTopRight.GetX(), farTopRight.GetX() );
        curSegment.aaBox.upperRight.y = Max( nearTopRight.GetY(), farTopRight.GetY() );
        curSegment.aaBox.z = nearBottomLeft.GetZ();

        // calculate the clip scale.
        SVec2 preClipLowerLeft = curSegment.aaBox.lowerLeft;
        SVec2 preClipUpperRight = curSegment.aaBox.upperRight;

        // clip to viewable area.
        curSegment.aaBox.lowerLeft.x = Max( curSegment.aaBox.lowerLeft.x, -1.0f );
        curSegment.aaBox.lowerLeft.y = Max( curSegment.aaBox.lowerLeft.y, -1.0f );
        curSegment.aaBox.upperRight.x = Min( curSegment.aaBox.upperRight.x, 1.0f );
        curSegment.aaBox.upperRight.y = Min( curSegment.aaBox.upperRight.y, 1.0f );

        // note:  this should just consider the user's planes coplanar to the user's
        // view plane!  Anything else is erroneous!
#if 0
        // determine how much we've clipped and calculate the log2 of the clipped
        // region.  This will let us determine how much resolution we need along X
        // and Y in the shadow page.

        // currently, we disable this feature.  This is because the viewer segment
        // has a depth that can be clipped... we don't care about the depth clipped.
        // here, we only want to know how much of the viewer plane is clipped!
        // Fix this, and enable this feature is it will save shadow generation time!

        float clipLeft = Max( curSegment.aaBox.lowerLeft.x - preClipLowerLeft.x, 0.0f );
        float clipRight = Max( preClipUpperRight.x - curSegment.aaBox.upperRight.x, 0.0f );
        float clipBottom = Max( curSegment.aaBox.lowerLeft.y - preClipLowerLeft.y, 0.0f );
        float clipTop = Max( preClipUpperRight.y - curSegment.aaBox.upperRight.y, 0.0f );

        curSegment.aaBox.clipLogX = ( int )( Log2( 0.5f * ( clipLeft + clipRight ) + 1.0f ) );
        curSegment.aaBox.clipLogY = ( int )( Log2( 0.5f * ( clipBottom + clipTop ) + 1.0f ) );
#else
        curSegment.aaBox.clipLogX = 0;
        curSegment.aaBox.clipLogY = 0;
#endif

        float curWidth = curSegment.aaBox.upperRight.x - curSegment.aaBox.lowerLeft.x;
        float curHeight = curSegment.aaBox.upperRight.y - curSegment.aaBox.lowerLeft.y;
        if ( curWidth < kMinAABoxDim )
        {
            float avg = 0.5f * ( curSegment.aaBox.upperRight.x + curSegment.aaBox.lowerLeft.x );
            curSegment.aaBox.upperRight.x = avg + kMinAABoxDim;
            curSegment.aaBox.lowerLeft.x = avg - kMinAABoxDim;
        }
        if ( curHeight < kMinAABoxDim )
        {
            float avg = 0.5f * ( curSegment.aaBox.upperRight.y + curSegment.aaBox.lowerLeft.y );
            curSegment.aaBox.upperRight.y = avg + kMinAABoxDim;
            curSegment.aaBox.lowerLeft.y = avg - kMinAABoxDim;
        }

        // calculate the scaled border amount.
        float borderX = ( curSegment.aaBox.upperRight.x - curSegment.aaBox.lowerLeft.x ) * border + distBias;
        float borderY = ( curSegment.aaBox.upperRight.y - curSegment.aaBox.lowerLeft.y ) * border + distBias;

        // add the border to our segment.
        if ( curSegment.aaBox.lowerLeft.x < 1.0f )
            curSegment.aaBox.lowerLeft.x -= borderX;
        if ( curSegment.aaBox.lowerLeft.y < 1.0f )
            curSegment.aaBox.lowerLeft.y -= borderY;
        if ( curSegment.aaBox.upperRight.x > -1.0f )
            curSegment.aaBox.upperRight.x += borderX;
        if ( curSegment.aaBox.upperRight.y > -1.0f )
            curSegment.aaBox.upperRight.y += borderY;
    }
}

//----------------------------------------------------------
const GrVirtualShadowSegmentSet::SAABox2D&
GrVirtualShadowSegmentSet::GetSegmentAABox( unsigned int segment ) const
{
    B_ASSERT( segment < _segmentCount );
    return _segments[ segment ].aaBox;
}

//----------------------------------------------------------
bool
GrVirtualShadowSegmentSet::IsSegmentVisible( unsigned int segment ) const
{
    B_ASSERT( segment < _segmentCount );
    return _segments[ segment ].visible;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
GrVirtualShadowSegmentSet::IsSegmentVisible( const SSegment& segment, const GrFrustum& frustum )
{
    // check to see if the segment is inside of the shadow frustum.
    for ( unsigned int i = 0; i < 6; ++i )
    {
        // get the current frustum cull plane.
        const MPlane& curPlane = frustum.GetCullPlane( ( GrFrustum::ECULLPLANE )i );

        // verify that any point is on the positive side of the current
        // plane.
        unsigned int j = 0;
        for ( ; j < 8; ++j )
        {
            // break if the current point is on the positive side of the plane or
            // on the plane itself.
            if ( curPlane.GetSide( segment.corners[ j ], GR_GENERIC_DISTANCE_EPSILON ) >= 0 )
                break;
        }

        // if all points are on the negative side of the current plane, set the AA
        // box to offscreen and return.
        if ( j == 8 )
            return false;
    }

    // return true to indicate that the segment is visible.
    return true;
}
