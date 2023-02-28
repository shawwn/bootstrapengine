//----------------------------------------------------------
// File:        MRay.h
// Author:      Kevin Bray
// Created:     07-17-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MRay.h"

// project includes.
#include "MAABox.h"
#include "MOBox.h"
#include "MMat3x3.h"
#include "MSphere.h"
#include "MPlane.h"
#include "MLineSegment.h"

//**********************************************************
// class MRay
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MRay::MRay()
: _start()
, _dir( 0.0f, 0.0f, -1.0f )
{

}

//----------------------------------------------------------
MRay::MRay( const MVec3& start, const MVec3& dir )
: _start( start )
, _dir( dir.Normalized() )
{

}

//----------------------------------------------------------
MRay::~MRay()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MRay::SetDir( const MVec3& dir )
{
    _dir = dir;
    _dir.Normalize();
}

//----------------------------------------------------------
void
MRay::SetDirFast( const MVec3& dir )
{
    // make sure the incoming vector is mostly normalized.
    B_ASSERT( Abs( dir.MagSqr() - 1.0f ) < 0.0001f );
    _dir = dir;
}

// shamelessly plucked from Graphics Gems.
#define NUMDIM  3
#define RIGHT   0
#define LEFT    1
#define MIDDLE  2

//----------------------------------------------------------
bool
MRay::Intersect( const MAABox& box, float maxDist ) const
{
    MVec3 hitLoc;
    return Intersect( hitLoc, box, maxDist );
}

//----------------------------------------------------------
bool
MRay::Intersect( MVec3& hitLoc, const MAABox& box, float maxDist ) const
{
    MVec3 minB = box.GetCenter() - box.GetHalfExts();
    MVec3 maxB = box.GetCenter() + box.GetHalfExts();

    bool inside = true;
    char quadrant[ NUMDIM ] = { 0, 0, 0 };
    register int i = 0;
    int whichPlane = 0;
    MVec3 maxT;
    MVec3 candidatePlane;

    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perspective view)
    for ( i = 0; i < NUMDIM; ++i )
    {
        if( _start[i] < minB[i] )
        {
            quadrant[i] = LEFT;
            candidatePlane[i] = minB[i];
            inside = false;
        }
        else if ( _start[i] > maxB[i] )
        {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxB[i];
            inside = false;
        }
        else
        {
            quadrant[i] = MIDDLE;
        }
    }

    // Ray start inside bounding box
    if ( inside )
        return true;

    // Calculate T distances to candidate planes
    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( quadrant[i] != MIDDLE && _dir[i] !=0.0f )
            maxT[i] = ( candidatePlane[i] - _start[i] ) / _dir[i];
        else
            maxT[i] = -1.0f;
    }

    // Get largest of the maxT's for final choice of intersection
    whichPlane = 0;
    for ( i = 1; i < NUMDIM; ++i )
    {
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;
    }

    // Check final candidate actually inside box
    if ( maxT[whichPlane] < 0.0f || maxT[whichPlane] > maxDist )
        return false;

    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( whichPlane != i )
        {
            hitLoc[i] = _start[i] + maxT[whichPlane] * _dir[i];
            if (hitLoc[i] < minB[i] || hitLoc[i] > maxB[i])
                return false;
        }
        else
        {
            hitLoc[i] = candidatePlane[i];
        }
    }

    // ray hits box
    return true;
}

//----------------------------------------------------------
bool
MRay::Intersect( const MOBox& box ) const
{
    // calculate the matrices needed to transform the ray into the box's space.
    MMat4x4 xform;
    box.GetTransform().Inverse( xform );
    MMat3x3 normalRot = box.GetTransform().GetRotate();
    normalRot.Transpose();

    // transform the ray into the box's space.
    MVec3 start = _start;
    xform.TransformCoordNoPerspFast( start );
    MVec3 dir = _dir;
    normalRot.RotatePointFast( dir );

    // perform an AABox intersection with the new ray params.
    MVec3 maxB = box.GetHalfExts();
    MVec3 minB = -box.GetHalfExts();
    MVec3 coord;

    bool inside = true;
    char quadrant[ NUMDIM] = { 0, 0, 0 };
    register int i = 0;
    int whichPlane = 0;
    MVec3 maxT;
    MVec3 candidatePlane;

    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perpsective view)
    for ( i = 0; i < NUMDIM; ++i )
    {
        if( start[i] < minB[i] )
        {
            quadrant[i] = LEFT;
            candidatePlane[i] = minB[i];
            inside = false;
        }
        else if ( start[i] > maxB[i] )
        {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxB[i];
            inside = false;
        }
        else
        {
            quadrant[i] = MIDDLE;
        }
    }

    // Ray start inside bounding box
    if ( inside )
        return true;

    // Calculate T distances to candidate planes
    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( quadrant[i] != MIDDLE && dir[i] !=0.0f )
            maxT[i] = ( candidatePlane[i] - start[i] ) / dir[i];
        else
            maxT[i] = -1.0f;
    }

    // Get largest of the maxT's for final choice of intersection
    whichPlane = 0;
    for ( i = 1; i < NUMDIM; ++i )
    {
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;
    }

    // Check final candidate actually inside box
    if ( maxT[whichPlane] < 0.0f )
        return false;

    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( whichPlane != i )
        {
            coord[i] = start[i] + maxT[whichPlane] * dir[i];
            if (coord[i] < minB[i] || coord[i] > maxB[i])
                return false;
        }
        else
        {
            coord[i] = candidatePlane[i];
        }
    }

    // ray hits box
    return true;
}

//----------------------------------------------------------
bool
MRay::Intersect( MVec3& coord, const MOBox& box ) const
{
    // calculate the matrices needed to transform the ray into the box's space.
    MMat4x4 xform;
    box.GetTransform().Inverse( xform );
    MMat3x3 normalRot = box.GetTransform().GetRotate();
    normalRot.Transpose();

    // transform the ray into the box's space.
    MVec3 start = _start;
    xform.TransformCoordNoPerspFast( start );
    MVec3 dir = _dir;
    normalRot.RotatePointFast( dir );

    // perform an AABox intersection with the new ray params.
    MVec3 maxB = box.GetHalfExts();
    MVec3 minB = -box.GetHalfExts();

    bool inside = true;
    char quadrant[ NUMDIM] = { 0, 0, 0 };
    register int i = 0;
    int whichPlane = 0;
    MVec3 maxT;
    MVec3 candidatePlane;

    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perpsective view)
    for ( i = 0; i < NUMDIM; ++i )
    {
        if( start[i] < minB[i] )
        {
            quadrant[i] = LEFT;
            candidatePlane[i] = minB[i];
            inside = false;
        }
        else if ( start[i] > maxB[i] )
        {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxB[i];
            inside = false;
        }
        else
        {
            quadrant[i] = MIDDLE;
        }
    }

    // Ray start inside bounding box
    if ( inside )
        return true;

    // Calculate T distances to candidate planes
    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( quadrant[i] != MIDDLE && dir[i] !=0.0f )
            maxT[i] = ( candidatePlane[i] - start[i] ) / dir[i];
        else
            maxT[i] = -1.0f;
    }

    // Get largest of the maxT's for final choice of intersection
    whichPlane = 0;
    for ( i = 1; i < NUMDIM; ++i )
    {
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;
    }

    // Check final candidate actually inside box
    if ( maxT[whichPlane] < 0.0f )
        return false;

    for ( i = 0; i < NUMDIM; ++i )
    {
        if ( whichPlane != i )
        {
            coord[i] = start[i] + maxT[whichPlane] * dir[i];
            if (coord[i] < minB[i] || coord[i] > maxB[i])
                return false;
        }
        else
        {
            coord[i] = candidatePlane[i];
        }
    }

    // transform the coordinate into world space.
    box.GetTransform().TransformCoordNoPerspFast( coord );

    // ray hits box
    return true;
}

//----------------------------------------------------------
bool
MRay::Intersect( const MSphere& sphere ) const
{
    // return the shortest distance to the line from the point.
    MVec3 hyp = ( sphere.GetCenter() - _start );

    // calculate where the line comes closest to point 'pt'.
    float t = _dir.Dot( hyp );

    // if we don't come closest to the point 'pt' somewhere on
    // the ray, then take the distance to the start point and
    // return true if the radius of the sphere is greater than
    // or equal to that distance.
    if ( t <= 0.0f )
        return ( _start.Dist( sphere.GetCenter() ) <= sphere.GetRadius() );

    // if the radius of the sphere is greater than or equal to
    // the distance to the line, then return true.
    return ( hyp.Dist( t * _dir ) <= sphere.GetRadius() );
}

//----------------------------------------------------------
bool
MRay::Intersect( MVec3& hitPos, const MPlane& plane, float maxDist ) const
{
    // get the distance from the ray's start to the plane along
    // the ray's normal.
    float dist = plane.Dist( _start );
    float absDist = Abs( dist );
    if ( absDist > maxDist )
        return false;

    // get the cosecant between the ray's direction and the
    // plane's normal.  This gives us a ratio of how much
    // longer the ray is until the intersection of the plane.
    float t = plane.GetNormal().Dot( _dir );
    if ( dist < 0.0f && t <= 0.0f )
        return false;
    else if ( dist > 0.0f && t >= 0.0f )
        return false;
    else if ( Abs( t ) < 0.001f )
        return false;

    float cscTheta = 1.0f / -t;

    // calculate the final position and return true to indicate
    // intersection.
    hitPos = _start + cscTheta * dist * _dir;
    return true;
}

//----------------------------------------------------------
MRay
MRay::Transform( const MMat4x4& mat, const MMat3x3& normalRot ) const
{
    return MRay( mat.TransformCoord( _start ), normalRot.RotatePoint( _dir ) );
}

//----------------------------------------------------------
bool
MRay::Intersect( MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2, bool twoSided ) const
{
    // calculate some useful vars.
    MVec3 b = pt1 - pt0;
    MVec3 c = pt2 - pt0;
    MVec3 vec = _start - pt0;

    // perform our test.
    // calculate the face normal.
    MVec3 faceNormal = b.Cross( c );
    if ( faceNormal.MagSqr() < MVec3::kNormalizeEpsilon )
        return false;

    // normalize the face normal.
    faceNormal.Normalize();

    // calculate the point on the triangle's plane along the ray.
    float d = 0.0f;
    if ( twoSided )
    {
        d = Abs( _dir.Dot( faceNormal ) );
    }
    else
    {
        // if the ray is shooting away from the plane, then simply return false.
        d = _dir.Dot( faceNormal );
        if ( d >= 0.0001f )
            return false;
    }

    // calculate the distance along the ray to the plane.
    float distance = -vec.Dot( faceNormal ) / d;

    // calculate where we intersect the plane the triangle lies on.
    point = _start + distance * _dir;
    MVec3 p = point - pt0;

    float maxAxis = faceNormal.AbsMax();
    int domAxis = 0;
    for ( ; domAxis < 3; ++domAxis )
    {
        if ( Abs( maxAxis - Abs( faceNormal[ domAxis ] ) ) < 0.0001f )
            break;
    }

    // choose an axis to throw away and solve in 2D.
    int a0 = 0;
    int a1 = 1;
    if ( domAxis == 0 )
    {
        a0 = 1;
        a1 = 2;
    }
    else if ( domAxis == 1 )
    {
        a0 = 0;
        a1 = 2;
    }
    else
    {
        a0 = 0;
        a1 = 1;
    }

    // calculate the barycentric coordinates.
//  u = ( p.GetY()*c.GetX()-p.GetX()*c.GetY() ) / ( b.GetY()*c.GetX() - b.GetX()*c.GetY() );
//  v = ( p.GetY()*b.GetX()-p.GetX()*b.GetY() ) / ( c.GetY()*b.GetX() - c.GetX()*b.GetY() );
    u = ( p[a1]*c[a0] - p[a0]*c[a1] ) / ( b[a1]*c[a0] - b[a0]*c[a1] );
    v = ( p[a1]*b[a0] - p[a0]*b[a1] ) / ( c[a1]*b[a0] - c[a0]*b[a1] );

    // return true if we're inside the triangle.
    return ( u >= 0.0f && v >= 0.0f && ( u + v ) <= 1.0f );
}

//----------------------------------------------------------
bool
MRay::Intersect( float& dist, MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2, bool twoSided , float maxDist ) const
{
    // calculate some useful vars.
    MVec3 b = pt1 - pt0;
    MVec3 c = pt2 - pt0;
    MVec3 vec = _start - pt0;

    // perform our test.
    // calculate the face normal.
    MVec3 faceNormal = b.Cross( c );
    if ( faceNormal.MagSqr() < 0.001f )
        return false;

    // normalize the face normal.
    faceNormal.NormalizeFast();

    // calculate the point on the triangle's plane along the ray.
    float d = 0.0f;
    if ( twoSided )
    {
        d = Abs( _dir.Dot( faceNormal ) );
    }
    else
    {
        // if the ray is shooting away from the plane, then simply return false.
        d = _dir.Dot( faceNormal );
        if ( d >= 0.0001f )
            return false;
    }

    // calculate the distance along the ray to the plane.
    float distance = -vec.Dot( faceNormal ) / d;
    if ( distance >= maxDist )
        return false;

    // calculate where we intersect the plane the triangle lies on.
    point = _start + distance * _dir;
    MVec3 p = point - pt0;

    float maxAxis = faceNormal.AbsMax();
    int domAxis = 0;
    for ( ; domAxis < 3; ++domAxis )
    {
        if ( Abs( maxAxis - Abs( faceNormal[ domAxis ] ) ) < 0.0001f )
            break;
    }

    // choose an axis to throw away and solve in 2D.
    int a0 = 0;
    int a1 = 1;
    if ( domAxis == 0 )
    {
        a0 = 1;
        a1 = 2;
    }
    else if ( domAxis == 1 )
    {
        a0 = 0;
        a1 = 2;
    }
    else
    {
        a0 = 0;
        a1 = 1;
    }

    // calculate the barycentric coordinates.
//  u = ( p.GetY()*c.GetX()-p.GetX()*c.GetY() ) / ( b.GetY()*c.GetX() - b.GetX()*c.GetY() );
//  v = ( p.GetY()*b.GetX()-p.GetX()*b.GetY() ) / ( c.GetY()*b.GetX() - c.GetX()*b.GetY() );
    u = ( p[a1]*c[a0] - p[a0]*c[a1] ) / ( b[a1]*c[a0] - b[a0]*c[a1] );
    v = ( p[a1]*b[a0] - p[a0]*b[a1] ) / ( c[a1]*b[a0] - c[a0]*b[a1] );

    // return true if we're inside the triangle.
    if ( u >= 0.0f && v >= 0.0f && ( u + v ) <= 1.0f )
    {
        dist =  distance;
        return true;
    }
    else
        return false;
}

//----------------------------------------------------------
bool
MRay::Intersect( MVec3& point, float& u, float& v, const MVec3& pt0, const MVec3& pt1, const MVec3& pt2,
                 const MVec3& faceNormal, bool twoSided ) const
{
    // calculate some useful vars.
    MVec3 b = pt1 - pt0;
    MVec3 c = pt2 - pt0;
    MVec3 vec = _start - pt0;

    // calculate the point on the triangle's plane along the ray.
    float d = 0.0f;
    if ( twoSided )
    {
        d = Abs( _dir.Dot( faceNormal ) );
    }
    else
    {
        // if the ray is shooting away from the plane, then simply return false.
        d = _dir.Dot( faceNormal );
        if ( d >= 0.0001f )
            return false;
    }

    // calculate the distance along the ray to the plane.
    float distance = -vec.Dot( faceNormal ) / d;

    // calculate where we intersect the plane the triangle lies on.
    point = _start + distance * _dir;
    MVec3 p = point - pt0;

    float maxAxis = faceNormal.AbsMax();
    int domAxis = 0;
    for ( ; domAxis < 3; ++domAxis )
    {
        if ( Abs( maxAxis - Abs( faceNormal[ domAxis ] ) ) < 0.0001f )
            break;
    }

    // choose an axis to throw away and solve in 2D.
    int a0 = 0;
    int a1 = 1;
    if ( domAxis == 0 )
    {
        a0 = 1;
        a1 = 2;
    }
    else if ( domAxis == 1 )
    {
        a0 = 0;
        a1 = 2;
    }
    else
    {
        a0 = 0;
        a1 = 1;
    }

    // calculate the barycentric coordinates.
//  u = ( p.GetY()*c.GetX()-p.GetX()*c.GetY() ) / ( b.GetY()*c.GetX() - b.GetX()*c.GetY() );
//  v = ( p.GetY()*b.GetX()-p.GetX()*b.GetY() ) / ( c.GetY()*b.GetX() - c.GetX()*b.GetY() );
    u = ( p[a1]*c[a0] - p[a0]*c[a1] ) / ( b[a1]*c[a0] - b[a0]*c[a1] );
    v = ( p[a1]*b[a0] - p[a0]*b[a1] ) / ( c[a1]*b[a0] - c[a0]*b[a1] );

    // return true if we're inside the triangle.
    return ( u >= 0.0f && v >= 0.0f && ( u + v ) <= 1.0f );
}

//----------------------------------------------------------
float
MRay::Dist( const MVec3& point ) const
{
    // return the shortest distance to the line from the point.
    MVec3 hyp = ( point - _start );

    // calculate where the line comes closest to point 'pt'.
    float t = _dir.Dot( hyp );

    // if we don't come closest to the point 'pt' somewhere on
    // the segment, take the distance to the respective end
    // points.
    if ( t <= 0.0f )
        return _start.Dist( point );

    // return the distance to the line.
    return hyp.Dist( t * _dir );
}

//----------------------------------------------------------
//float
//MRay::Dist( const MRay& ray, float epsilon ) const
//{
//  B_ASSERT( false );  
//}

//----------------------------------------------------------
float
MRay::Dist( const MLineSegment& segment, float epsilon ) const
{
    float rayMu = 0.0f;
    float segMu = 0.0f;
    return Dist( segment, rayMu, segMu, epsilon );
}

//----------------------------------------------------------
float
MRay::Dist( const MLineSegment& segment, float& rayMu, float& segMu, float epsilon ) const
{
    // here, we're trying to solve a specialized line-line distance
    // equation.  We need to do the following:
    //
    //  - The line that connects the two closest points on either segment
    //    will be perpendicular... that is:  A dot C = 0 && B dot C = 0. 
    //  - The points must lie on the ray and the segment... otherwise the
    //    closest end point is used.
    //
    // A = s2 - s1
    // B = rdir
    //
    // A dot C = 0
    // B dot C = 0
    // C = ?
    //
    // Ax * Cx + Ay * Cy + Az * Cz = 0
    // Bx * Cx + By * Cy + Bz * Cz = 0
    //
    const MVec3& pt0 = segment.GetPt0();
    const MVec3& pt1 = segment.GetPt1();

    // p13 = vector from starting point to starting point.
    MVec3 p13( pt0 - _start );

    // p43 = _dir.
    // p21 = line segment dir.
    MVec3 p21( pt1 - pt0 );

    // is the segment just a point?
    if ( Abs( p21.GetX() ) < epsilon && Abs( p21.GetY() ) < epsilon && Abs( p21.GetZ() ) < epsilon )
        return Dist( pt1 );

    float d1343 = p13.Dot( _dir ); // p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    float d4321 = _dir.Dot( p21 ); // p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    float d1321 = p13.Dot( p21 );  // p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    float d2121 = p21.MagSqr();    // p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    float denom = d2121 - d4321 * d4321;
    if ( Abs( denom ) < epsilon )   // parallel.
        return Dist( pt1 );

    float numer = d1343 * d4321 - d1321;

    // calculate the positions on the ray and line segment.
    segMu = numer / denom;
    rayMu = (d1343 + d4321 * segMu);

    // now figure out the distances.
    if ( rayMu < 0.0f )
    {
        if ( segMu < 0.0f )
            return _start.Dist( pt0 );
        else if ( segMu > 1.0f )
            return _start.Dist( pt1 );
        else
            return _start.Dist( pt0 + segMu * p21 );
    }
    else if ( segMu < 0.0f )
    {
        // distance between the ray and the start of the line segment.
        return pt0.Dist( _start + rayMu * _dir );
    }
    else if ( segMu > 1.0f )
    {
        // distance between the ray and the end of the line segment.
        return pt1.Dist( _start + rayMu * _dir );
    }

    // distnace between the ray and the line segment.
    return ( pt0 + segMu * p21 ).Dist( _start + rayMu * _dir );
}
