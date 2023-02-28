//----------------------------------------------------------
// File:        GrFrustum.cpp
// Author:      Kevin Bray
// Created:     11-10-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrFrustum.h"

// project includes.
#include "MVec3.h"
#include "GrTypes.h"
#include "MOBox.h"
#include "MAABox.h"
#include "MLine.h"
#include "MRay.h"
#include "GrPolygon.h"

// OpenGL includes.
#include "GLSubsys.h"

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrFrustum::GrFrustum( const MVec3& lookPoint, const MVec3& lookDir, const MVec3& nearBottomLeft, const MVec3& nearTopLeft,
    const MVec3& nearTopRight, const MVec3& nearBottomRight, const MVec3& farBottomLeft, const MVec3& farTopLeft,
    const MVec3& farTopRight, const MVec3& farBottomRight, float nearDist, float farDist )
: _pos( lookPoint )
, _nearBottomLeft( nearBottomLeft )
, _nearTopLeft( nearTopLeft )
, _nearTopRight( nearTopRight )
, _nearBottomRight( nearBottomRight )
, _farBottomLeft( farBottomLeft )
, _farTopLeft( farTopLeft )
, _farTopRight( farTopRight )
, _farBottomRight( farBottomRight )
, _isHull( true )
{
    MVec3 leftDir = ( farBottomLeft - nearBottomLeft ).Cross( nearTopLeft - nearBottomLeft );
    MVec3 rightDir = ( nearTopRight - nearBottomRight ).Cross( farTopRight - nearTopRight );
    MVec3 topDir = ( farTopLeft - nearTopLeft ).Cross( nearTopRight - nearTopLeft );
    MVec3 bottomDir = ( farBottomRight - nearBottomRight ).Cross( nearBottomLeft - nearBottomRight );

    AddCullPlane( MPlane( leftDir, nearTopLeft ) ); // left
    AddCullPlane( MPlane( rightDir, nearBottomRight ) );    // right
    AddCullPlane( MPlane( topDir, nearTopLeft ) );  // top
    AddCullPlane( MPlane( bottomDir, nearBottomRight ) );   // bottom
    AddCullPlane( MPlane( lookDir, _pos + ( nearDist + 0.00001f ) * lookDir ) );    // near
    AddCullPlane( MPlane( -lookDir, _pos + ( farDist + 0.00001f ) * lookDir ) );    // far
}

//----------------------------------------------------------
GrFrustum::GrFrustum( const MVec3& lookPoint, const MVec3& bottomLeft, const MVec3& topLeft, const MVec3& topRight,
                      const MVec3& bottomRight, const MVec3& lookDir, float nearDist, float farDist )
: _pos( lookPoint )
, _nearBottomLeft( bottomLeft )
, _nearTopLeft( topLeft )
, _nearTopRight( topRight )
, _nearBottomRight( bottomRight )
, _isHull( true )
{
    B_ASSERT( lookDir.MagSqr() > 0.99f && lookDir.MagSqr() < 1.01 );

    MVec3 leftDir = ( bottomLeft - lookPoint ).Cross( topLeft - lookPoint );
    MVec3 rightDir = ( topRight - lookPoint ).Cross( bottomRight - lookPoint );
    MVec3 topDir = ( topLeft - lookPoint ).Cross( topRight - lookPoint );
    MVec3 bottomDir = ( bottomRight - lookPoint ).Cross( bottomLeft - lookPoint );

    // build the planes.
    AddCullPlane( MPlane( leftDir, lookPoint ) );   // left
    AddCullPlane( MPlane( rightDir, lookPoint ) );  // right
    AddCullPlane( MPlane( topDir, lookPoint ) );    // top
    AddCullPlane( MPlane( bottomDir, lookPoint ) ); // bottom
    AddCullPlane( MPlane( lookDir, lookPoint + ( nearDist + 0.00001f ) * lookDir ) );   // near
    AddCullPlane( MPlane( -lookDir, lookPoint + ( farDist + 0.00001f ) * lookDir ) );   // far

    // determine the far points.
    _farBottomLeft = farDist * ( bottomLeft - lookPoint ) + lookPoint;
    _farTopLeft = farDist * ( topLeft - lookPoint ) + lookPoint;
    _farTopRight = farDist * ( topRight - lookPoint ) + lookPoint;
    _farBottomRight = farDist * ( bottomRight - lookPoint ) + lookPoint;
}

//----------------------------------------------------------
GrFrustum::GrFrustum( const MVec3& lookPoint, const SQuad& quad, const MVec3& lookDir, float nearDist, float farDist )
: _pos( lookPoint )
, _nearBottomLeft( quad.pos )
, _nearTopLeft( MVec3( quad.pos ) + MVec3( quad.up ) )
, _nearTopRight( MVec3( quad.pos ) + MVec3( quad.up ) + MVec3( quad.right ) )
, _nearBottomRight( MVec3( quad.pos ) + MVec3( quad.right ) )
, _isHull( true )
{
    B_ASSERT( false );  // step through this function!  Make sure everything is okay ( never been tested )!
    B_ASSERT( lookDir.MagSqr() > 0.99999f && lookDir.MagSqr() < 1.00001 );

    // create a frustum from a quad.
//  MVec3 bottomLeft = _nearBottomLeft; //MVec3( quad.pos );
//  MVec3 topLeft = _nearTopLeft; //MVec3( quad.pos ) + MVec3( quad.up );
//  MVec3 topRight = _nearTopRight; //MVec3( quad.pos ) + MVec3( quad.up ) + MVec3( quad.right );
//  MVec3 bottomRight = _nearBottomRight; //MVec3( quad.pos ) + MVec3( quad.right );
    AddCullPlane( MPlane( ( _nearBottomLeft - lookPoint ).Cross( _nearTopLeft - lookPoint ), lookPoint ) );     // left
    AddCullPlane( MPlane( ( _nearTopRight - lookPoint ).Cross( _nearBottomRight - lookPoint ), lookPoint ) );   // right
    AddCullPlane( MPlane( ( _nearTopLeft - lookPoint ).Cross( _nearTopRight - lookPoint ), lookPoint ) );       // top
    AddCullPlane( MPlane( ( _nearBottomRight - lookPoint ).Cross( _nearBottomLeft - lookPoint ), lookPoint ) ); // bottom
    AddCullPlane( MPlane( lookDir, _pos + ( nearDist + 0.00001f ) * lookDir ) );                // near
    AddCullPlane( MPlane( -1.0f * lookDir, lookPoint + ( farDist + 0.00001f ) * lookDir ) );    // far

    // determine the far points.
    _farBottomLeft = farDist * ( _nearBottomLeft - lookPoint ) + lookPoint;
    _farTopLeft = farDist * ( _nearTopLeft - lookPoint ) + lookPoint;
    _farTopRight = farDist * ( _nearTopRight - lookPoint ) + lookPoint;
    _farBottomRight = farDist * ( _nearBottomRight - lookPoint ) + lookPoint;
}

//----------------------------------------------------------
GrFrustum::GrFrustum( const GrFrustum& frustum, const SQuad& quad, const MVec3& lookDir, float nearDist, float farDist )
: _pos( frustum._pos )
, _planes( frustum._planes )
, _farBottomLeft( frustum._farBottomLeft )
, _farTopLeft( frustum._farTopLeft )
, _farTopRight( frustum._farTopRight )
, _farBottomRight( frustum._farBottomRight )
, _nearBottomLeft( quad.pos )
, _nearTopLeft( MVec3( quad.pos ) + MVec3( quad.up ) )
, _nearTopRight( MVec3( quad.pos ) + MVec3( quad.up ) + MVec3( quad.right ) )
, _nearBottomRight( MVec3( quad.pos ) + MVec3( quad.right ) )
, _isHull( frustum._isHull )
{
    B_ASSERT( false );  // step through this function!  Make sure everything is okay ( never been tested )!
    B_ASSERT( lookDir.MagSqr() > 0.99999f && lookDir.MagSqr() < 1.00001 );

    // just add more planes.  Add them first so its usually the smaller frustum first
    // ( most common case ).
//  MVec3 bottomLeft = MVec3( quad.pos );
//  MVec3 topLeft = MVec3( quad.pos ) + MVec3( quad.up );
//  MVec3 topRight = MVec3( quad.pos ) + MVec3( quad.up ) + MVec3( quad.right );
//  MVec3 bottomRight = MVec3( quad.pos ) + MVec3( quad.right );

    AddCullPlane( MPlane( ( _nearBottomLeft - _pos ).Cross( _nearTopLeft - _pos ), _pos ) );        // left
    AddCullPlane( MPlane( ( _nearTopRight - _pos ).Cross( _nearBottomRight - _pos ), _pos ) );      // right
    AddCullPlane( MPlane( ( _nearTopLeft - _pos ).Cross( _nearTopRight - _pos ), _pos ) );          // top
    AddCullPlane( MPlane( ( _nearBottomRight - _pos ).Cross( _nearBottomLeft - _pos ), _pos ) );    // bottom
    AddCullPlane( MPlane( lookDir, _pos + ( nearDist + 0.00001f ) * lookDir ) );                    // near
    AddCullPlane( MPlane( -1.0f * lookDir, _pos + ( farDist + 0.00001f ) * lookDir ) );             // far
}

//----------------------------------------------------------
GrFrustum::GrFrustum( const GrFrustum& frustum )
: _pos( frustum._pos )
, _planes( frustum._planes )
, _farBottomLeft( frustum._farBottomLeft )
, _farTopLeft( frustum._farTopLeft )
, _farTopRight( frustum._farTopRight )
, _farBottomRight( frustum._farBottomRight )
, _nearBottomLeft( frustum._nearBottomLeft )
, _nearTopLeft( frustum._nearTopLeft )
, _nearTopRight( frustum._nearTopRight )
, _nearBottomRight( frustum._nearBottomRight )
, _isHull( frustum._isHull )
{

}

//----------------------------------------------------------
GrFrustum::~GrFrustum()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrFrustum&
GrFrustum::operator = ( const GrFrustum& frustum )
{
    // assign data.
    _isHull = frustum._isHull;
    _pos = frustum._pos;
    _farBottomLeft = frustum._farBottomLeft;
    _farTopLeft = frustum._farTopLeft;
    _farTopRight = frustum._farTopRight;
    _farBottomRight = frustum._farBottomRight;
    _nearBottomLeft = frustum._nearBottomLeft;
    _nearTopLeft = frustum._nearTopLeft;
    _nearTopRight = frustum._nearTopRight;
    _nearBottomRight = frustum._nearBottomRight;
    _planes = frustum._planes;

    return *this;
}

//----------------------------------------------------------
void
GrFrustum::AddCullPlane( const MPlane& plane )
{
    _planes.Push( plane );
    float dist = plane.Dist( _pos );
//  if ( dist < -1.0001f )
//      _isHull = false;
}

//----------------------------------------------------------
const MPlane&
GrFrustum::GetCullPlane( ECULLPLANE plane ) const
{
    return _planes[ plane ];
}

//----------------------------------------------------------
void
GrFrustum::GetCrossSection( UFastArray< MVec3 >& hull, const MPlane& plane ) const
{
    // finish implementing this!!!
    B_ASSERT( false );

    // first, just do a 'dumb' approach by finding all of the points
    // where all of the original 6 planes intersect with eachother and
    // the plane passed in.

    // for each orignal plane.
    UFastArray< MVec3 > points;
    MVec3 curPoint;
    for ( unsigned int i = 0; i < 6; ++i )
    {
        // for each subsequent orignal plane.
        for ( unsigned int j = i+1; j < 6; ++j )
        {
            // if the current planes and the plane passed in intersect,
            // add the point where they intersect to the current point
            // cloud.
            if ( plane.GetIntersect( curPoint, _planes[ i ], _planes[ j ] ) )
            {
                points.Push( curPoint );
            }
        }
    }

    // perform a 2D qhull on the points to ensure that they're in the
    // proper order.
    

    // now, clip the current hull with any extra planes.
}

//----------------------------------------------------------
void
GrFrustum::GetPerpCrossSection( UFastArray< MVec3 >& hull, float dist ) const
{
    // TODO: Fix this up so it takes into account additional culling
    // planes.  This could lead to a tighter convex hull.
    const MVec3& lookDir = GetCullPlane( ECP_NEAR ).GetNormal();
    MPlane plane( lookDir, _pos + dist * lookDir );

    // bottom left.
    plane.GetIntersect( hull.Push(), GetCullPlane( ECP_BOTTOM ), GetCullPlane( ECP_LEFT ) );

    // top left.
    plane.GetIntersect( hull.Push(), GetCullPlane( ECP_TOP ), GetCullPlane( ECP_LEFT ) );

    // top right.
    plane.GetIntersect( hull.Push(), GetCullPlane( ECP_TOP ), GetCullPlane( ECP_RIGHT ) );

    // bottom right.
    plane.GetIntersect( hull.Push(), GetCullPlane( ECP_BOTTOM ), GetCullPlane( ECP_RIGHT ) );
}

//----------------------------------------------------------
void
GrFrustum::BuildHull( SVec3* verts ) const
{
    // this assumes non-orthograpic projection. >=|
    // right.
    verts[ 0  ] = SVec3( _nearBottomRight );
    verts[ 1  ] = SVec3( _farBottomRight );
    verts[ 2  ] = SVec3( _farTopRight );
    verts[ 3  ] = SVec3( _nearBottomRight );
    verts[ 4  ] = SVec3( _farTopRight );
    verts[ 5  ] = SVec3( _nearTopRight );

    // left.
    verts[ 6  ] = SVec3( _nearTopLeft );
    verts[ 7  ] = SVec3( _farTopLeft );
    verts[ 8  ] = SVec3( _farBottomLeft );
    verts[ 9  ] = SVec3( _nearTopLeft );
    verts[ 10 ] = SVec3( _farBottomLeft );
    verts[ 11 ] = SVec3( _nearBottomLeft );

    // top.
    verts[ 12 ] = SVec3( _nearTopRight );
    verts[ 13 ] = SVec3( _farTopRight );
    verts[ 14 ] = SVec3( _farTopLeft );
    verts[ 15 ] = SVec3( _nearTopRight );
    verts[ 16 ] = SVec3( _farTopLeft );
    verts[ 17 ] = SVec3( _nearTopLeft );

    // bottom.
    verts[ 18 ] = SVec3( _nearBottomLeft );
    verts[ 19 ] = SVec3( _farBottomLeft );
    verts[ 20 ] = SVec3( _farBottomRight );
    verts[ 21 ] = SVec3( _nearBottomLeft );
    verts[ 22 ] = SVec3( _farBottomRight );
    verts[ 23 ] = SVec3( _nearBottomRight );

    // near.
    verts[ 24 ] = SVec3( _nearTopRight );
    verts[ 25 ] = SVec3( _nearTopLeft );
    verts[ 26 ] = SVec3( _nearBottomLeft );
    verts[ 27 ] = SVec3( _nearTopRight );
    verts[ 28 ] = SVec3( _nearBottomLeft );
    verts[ 29 ] = SVec3( _nearBottomRight );

    // far.
    verts[ 30 ] = SVec3( _farTopRight );
    verts[ 31 ] = SVec3( _farBottomLeft );
    verts[ 32 ] = SVec3( _farTopLeft );
    verts[ 33 ] = SVec3( _farTopRight );
    verts[ 34 ] = SVec3( _farBottomRight );
    verts[ 35 ] = SVec3( _farBottomLeft );
}

//----------------------------------------------------------
bool
GrFrustum::IsSphereInside( const MVec3& pt, float radius ) const
{
    // too far away.
//  if ( _pos.DistSqr( pt ) > ( ( _farDist + radius ) * ( _farDist + radius ) ) )
//      return false;

    // for each frustum edge...
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        if ( _planes[ plane ].Dist( pt ) < -radius )
            return false;
    }
    return true;
}

//----------------------------------------------------------
bool
GrFrustum::IsPointInside( const MVec3& pt ) const
{
//  if ( _pos.DistSqr( pt ) > ( _farDistSqr ) )
//      return false;

    // for each frustum edge...
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        if ( _planes[ plane ].Dist( pt ) < 0.0f )
            return false;
    }
    return true;
}

//----------------------------------------------------------
bool
GrFrustum::IsAABoxInside( const MAABox& box ) const
{
    MVec3 pts[ 8 ];         // there are 8 points on a cube.
    box.GetCorners( pts );

    // for each plane...
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        // for each point...
        unsigned int pt = 0;
        for ( ; pt < 8; ++pt )
        {
            // we are checking to see if all points are on the negative side of a single plane.
            if ( _planes[ plane ].Dist( pts[ pt ] ) > 0.0f )
                break;
        }

        // check to see if we went through the entire loop without breaking... if so, then all
        // points are on the negative side of a single plane.  Thus, the test failed.
        if ( pt == 8 )
            return false;
    }

    // the box collides with the frustum.
    return true;
}

//----------------------------------------------------------
bool
GrFrustum::IsOBoxInside( const MOBox& box ) const
{
    MVec3 pts[ 8 ];         // there are 8 points on a cube.
    box.GetCorners( pts );

    // for each plane...
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        // for each point...
        unsigned int pt = 0;
        for ( ; pt < 8; ++pt )
        {
            // we are checking to see if all points are on the negative side of a single plane.
            if ( _planes[ plane ].Dist( pts[ pt ] ) > 0.0f )
                break;
        }

        // check to see if we went through the entire loop without breaking... if so, then all
        // points are on the negative side of a single plane.  Thus, the test failed.
        if ( pt == 8 )
            return false;
    }

    // the box collides with the frustum.
    return true;
}
/*
//----------------------------------------------------------
bool
GrFrustum::IsPortalInside( const GrPortal& portal ) const
{
    // test this!
    B_ASSERT( false );

    // If all points in a portal are behind any of the planes, then the polygon is
    // not inside the frustum.
    const unsigned int planeCount = _planes.GetElemCount();
    const unsigned int vertCount = portal.GetVertCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        // if all vertices are behind one of the planes, then the polygon is not in the
        // frustum.
        for ( unsigned int vert = 0; vert < vertCount; ++vert )
        {
            // if not behind a plane, then stop iterating and go to the next plane.
            if ( _planes[ plane ].GetSide( portal.GetVert( vert ), 0.0f ) > 0 )
                break;
        }

        // check to see if all vertices were behind one of the planes.
        if ( vert == vertCount )
            return false;
    }

    // The portal was not entirely behind any of the frustum planes.
    return true;
}
*/
//----------------------------------------------------------
int
GrFrustum::FindRelativeLoc( const MPlane& plane ) const
{
/*
    // check all points.
    int eyeSide = ( plane.Dist( _pos ) > 0.0f ) ? ( 1 ) : ( -1 );
    if ( eyeSide == -1 )
    {
        if ( plane.Dist( _farBottomLeft ) > 0 )
            return 0;
        if ( plane.Dist( _farTopLeft ) > 0 )
            return 0;
        if ( plane.Dist( _farTopRight ) > 0 )
            return 0;
        if ( plane.Dist( _farBottomRight ) > 0 )
            return 0;
    }
    else
    {
        if ( plane.Dist( _farBottomLeft ) <= 0 )
            return 0;
        if ( plane.Dist( _farTopLeft ) <= 0 )
            return 0;
        if ( plane.Dist( _farTopRight ) <= 0 )
            return 0;
        if ( plane.Dist( _farBottomRight ) <= 0 )
            return 0;
    }
*/
    static const float kEpsilon = 0.001f;

    // check all points against the plane passed in.
    int side = plane.GetSide( _nearBottomLeft, kEpsilon );
    if ( side != plane.GetSide( _nearTopLeft, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _nearTopRight, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _nearBottomRight, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _farBottomLeft, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _farTopLeft, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _farTopRight, kEpsilon ) )
        return 0;
    if ( side != plane.GetSide( _farBottomRight, kEpsilon ) )
        return 0;

    // all points are on the same side of the plane.
    return side;
}

//----------------------------------------------------------
// -1 = outside, 1 = inside, 0 = both.
int
GrFrustum::FindRelativeLoc( const MVec3& pt0, const MVec3& pt1 ) const
{
/*
    // if one or both points is inside the frustum, then we know the line
    // intersects with or contains the line.
    if ( IsPointInside( pt0 ) )
    {
        if ( IsPointInside( pt1 ) )
            return 1;
        else
            return -1;
    }
    if ( IsPointInside( pt1 ) )
    {
        if ( IsPointInside( pt0 ) )
            return 1;
        else
            return -1;
    }

    // if both points lie entirely on the negative side of any of the
    // edges, then we know the line segment is not in the frustum.
    for ( unsigned int i = 0; i < _planeCount; ++i )
    {
        if ( _planes[ i ].Dist( pt0 ) <= 0.0f && _planes[ i ].Dist( pt1 ) <= 0.0f )
            return -1;  // entirely outside.
    }
*/
    // check to see if either of the points is inside the frustum.
    int inside = ( IsPointInside( pt0 ) ? 1 : 0 ) + ( IsPointInside( pt1 ) ? 1 : 0 ) - 1;
    if ( inside >= 0 )
        return inside;

    // if neither point is inside the frustum, we need to check to see
    // if both points are outside of the frustum, but the line itself
    // intersects with the frustum.
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int i = 0; i < planeCount; ++i )
    {
        MVec3 intersectPoint;
        if ( _planes[ i ].GetSegmentIntersect( intersectPoint, pt0, pt1 ) )
        {
            // we found a point where the segment intersects with the
            // plane... check to see if the point is 'inside' all other
            // planes.
            unsigned int j = 0;
            for ( ; j < planeCount; ++j )
            {
                // skip if we're testing the same plane.
                if ( j == i )
                    continue;

                if ( _planes[ j ].Dist( intersectPoint ) < -0.001f )
                {
                    // point is not inside.  Check any other intersections.
                    break;
                }
            }

            // if the intersection point is inside the frustum, then
            // the segment intersects with the frustum.
            if ( j == planeCount )
                return 0;
        }
    }

    // not inside the frustum.
    return -1;
}

//----------------------------------------------------------
bool
GrFrustum::IsFrustumInside( const GrFrustum& frustum ) const
{
    // check for intersection.
    if ( frustum.FindRelativeLoc( _farBottomLeft, _pos ) >= 0 )
        return true;
    if ( frustum.FindRelativeLoc( _farTopLeft, _pos ) >= 0 )
        return true;
    if ( frustum.FindRelativeLoc( _farTopRight, _pos ) >= 0 )
        return true;
    if ( frustum.FindRelativeLoc( _farBottomRight, _pos ) >= 0 )
        return true;

    if ( FindRelativeLoc( frustum._farBottomLeft, frustum._pos ) >= 0 )
        return true;
    if ( FindRelativeLoc( frustum._farTopLeft, frustum._pos ) >= 0 )
        return true;
    if ( FindRelativeLoc( frustum._farTopRight, frustum._pos ) >= 0 )
        return true;
    if ( FindRelativeLoc( frustum._farBottomRight, frustum._pos ) >= 0 )
        return true;

    // is not partially or fully inside.
    return false;
}

//----------------------------------------------------------
bool
GrFrustum::IsPolygonInside( const GrPolygon& polygon ) const
{
    // go through all vertices in the polygon and see if they're all on
    // the negative side of one of the planes.
    // for each plane...
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        // for each vertex...
        unsigned int vertexCount = polygon.GetVertexCount();
        unsigned int vertex = 0;
        for ( ; vertex < vertexCount; ++vertex )
        {
            // we are checking to see if all points are on the negative side of a single plane.
            if ( _planes[ plane ].Dist( polygon.GetVertex( vertex ).position ) > 0.0f )
                break;
        }

        // check to see if we went through the entire loop without breaking... if so, then all
        // vertices are on the negative side of a single plane.  Thus, the test failed.
        if ( vertex == vertexCount )
            return false;
    }

    // the polygon is wholly or partially inside the frustum.
    return true;
}

//----------------------------------------------------------
bool
GrFrustum::IsTriangleInside( const SVec3& p0, const SVec3& p1, const SVec3& p2 ) const
{
    // go through all vertices in the triangle and see if they're all on
    // the negative side of one of the planes.
    // for each plane...
    int neg = 0;
    const unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int plane = 0; plane < planeCount; ++plane )
    {
        // for each vertex...
        if ( _planes[ plane ].Dist( p0 ) > 0.0f )
            continue;
        if ( _planes[ plane ].Dist( p1 ) > 0.0f )
            continue;
        if ( _planes[ plane ].Dist( p2 ) > 0.0f )
            continue;

        return false;
    }

    // the triangle is at least partially inside the frustum.
    return true;
}

//----------------------------------------------------------
bool
GrFrustum::IsRayInside( const MRay& ray, float epsilon ) const
{
    // return true if the ray is inside.
    if ( IsPointInside( ray.GetStart() ) )
        return true;

    // check the ray's intersection locations.
    MVec3 intersect;
    unsigned int planeCount = _planes.GetElemCount();
    for ( unsigned int i = 0; i < planeCount; ++i )
    {
        if ( ray.Intersect( intersect, _planes[ i ], 10000.0f ) )
        {
            if ( _planes[ i ].GetNormal().Dot( ray.GetDir() ) < 0.0f )
            {
                if ( IsPointInside( intersect - epsilon * ray.GetDir() ) )
                    return true;
            }
            else
            {
                if ( IsPointInside( intersect + epsilon * ray.GetDir() ) )
                    return true;
            }
        }
    }

    // return false to indicate no.
    return false;
}

//----------------------------------------------------------
void
GrFrustum::DebugDraw() const
{
/*
    bglUseProgramObjectARB( 0 );
    bglDisable( GL_BLEND );
    bglActiveTexture( 0 );
    bglDisable( GL_TEXTURE_2D );
    bglActiveTexture( 1 );
    bglDisable( GL_TEXTURE_2D );
    bglActiveTexture( 2 );
    bglDisable( GL_TEXTURE_2D );
    bglActiveTexture( 3 );
    bglDisable( GL_TEXTURE_2D );
    bglActiveTexture( 4 );
    bglDisable( GL_TEXTURE_CUBE_MAP );
    bglActiveTexture( 5 );
    bglDisable( GL_TEXTURE_CUBE_MAP );
    CHECK_GL();
*/
    bglBegin( GL_LINES );
    bglVertex3f( _pos.GetX(), _pos.GetY(), _pos.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _farBottomLeft.GetX(), _farBottomLeft.GetY(), _farBottomLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _pos.GetX(), _pos.GetY(), _pos.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _farTopLeft.GetX(), _farTopLeft.GetY(), _farTopLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _pos.GetX(), _pos.GetY(), _pos.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _farTopRight.GetX(), _farTopRight.GetY(), _farTopRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _pos.GetX(), _pos.GetY(), _pos.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( _farBottomRight.GetX(), _farBottomRight.GetY(), _farBottomRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );

    MVec3 midBottomLeft( 0.5f * ( _farBottomLeft - _pos ) + _pos );
    MVec3 midTopLeft( 0.5f * ( _farTopLeft - _pos ) + _pos );
    MVec3 midTopRight( 0.5f * ( _farTopRight - _pos ) + _pos );
    MVec3 midBottomRight( 0.5f * ( _farBottomRight - _pos ) + _pos );

    bglVertex3f( midBottomLeft.GetX(), midBottomLeft.GetY(), midBottomLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midTopLeft.GetX(), midTopLeft.GetY(), midTopLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midTopLeft.GetX(), midTopLeft.GetY(), midTopLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midTopRight.GetX(), midTopRight.GetY(), midTopRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midTopRight.GetX(), midTopRight.GetY(), midTopRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midBottomRight.GetX(), midBottomRight.GetY(), midBottomRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midBottomRight.GetX(), midBottomRight.GetY(), midBottomRight.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );
    bglVertex3f( midBottomLeft.GetX(), midBottomLeft.GetY(), midBottomLeft.GetZ() );
    bglColor4ub( 255, 255, 255, 255 );

    bglEnd();
    CHECK_GL();
}
