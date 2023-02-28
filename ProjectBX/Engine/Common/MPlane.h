//----------------------------------------------------------
// File:        MPlane.h
// Author:      Kevin Bray
// Created:     11-10-04
//
// Purpose:     To represent a plane in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"
#include "MMat4x4.h"

// std c++ includes.
#include <math.h>

class UReader;
class MLine;

//**********************************************************
// class MPlane
//**********************************************************
class MPlane
{
public:
    // normal to the plane, and any point the plane passes through.
    MPlane();
    MPlane( UReader& reader );
    MPlane( const MVec3& normal, const MVec3& point );
    MPlane( const MVec3& normal, float d );
    ~MPlane();

    // if the value is ( - ), then the point is behind the plane.  If ( + ), then on the plane.
    const MVec3&    GetNormal() const               {   return _normal;                 }
    float           GetD() const                    {   return _d;                      }
    void            SetD( float d )                 {   _d = d;                         }

    void            ReverseFacing()                 {   _normal = -_normal; _d = -_d;   }

    // adds a bias to the plane ( moves it along the normal by the amount specified ).
    void            Bias( float amount );

    // transforms the plane by a 4x4 matrix.
    MPlane          Transform( const MMat4x4& transform, const MMat3x3& normalRot );
    void            TransformFast( const MMat4x4& transform, const MMat3x3& normalRot );

    // translates the plane by a given amount.
    void            Translate( const MVec3& translate );

    // scales the plane's distance to the origin by the given amount.
    void            Scale( float scale );

    // builds a reflection matrix.
    MMat4x4         BuildReflectMatrix() const;

    // returns true if two planes are coplanar.
    bool            IsCoplanar( const MPlane& plane, float angleEpsilon, float dEpsilon ) const;

    // returns the side the point passed in is on.
    int             GetSide( const MVec3& pt, float epsilon ) const;

    // returns the line that is formed where two planes intersect.  If the line cannot
    // be determined, false is returned.  Otherwise, true is returned.
    bool            GetIntersect( MLine& line, const MPlane& plane ) const;

    // returns the point that is formed where three planes intersect.  If a point cannot
    // be determined, false is returned.  Otherwise, true is returned.
    bool            GetIntersect( MVec3& point, const MPlane& plane1, const MPlane& plane2 ) const;

    // signed distance to a point.
    template< typename _Vec >
    inline float    Dist( const _Vec& point ) const;

    template< typename _Vec >
    bool            GetSegmentIntersect( _Vec& pt, const _Vec& pt0, const _Vec& pt1 ) const;

    template< typename _Vec >
    bool            GetRayIntersect( float& rayMu, const _Vec& rayOrigin, const _Vec& rayDir ) const;

    // splits a triangle.  structure passed in must have vars pt0, pt1, pt2.
    template< typename _Triangle, typename _Vec >
    void            SplitTriangle( const _Triangle& tri, _Triangle *posTris, _Triangle *negTris, unsigned int& posCount,
                                   unsigned int& negCount, float epsilon ) const;

    // save/load.
    void            Save( UWriter& writer ) const;
    void            Load( UReader& reader );

private:
    MVec3           _normal;
    float           _d;
};


//**********************************************************
// class MPlane
//**********************************************************

//==========================================================
// public template methods.
//==========================================================

//----------------------------------------------------------
// _Vec must have a conversion to type MVec defined.
template< typename _Vec >
inline
float
MPlane::Dist( const _Vec& point ) const
{
    return( _normal.Dot( point ) + _d );
}

//----------------------------------------------------------
// _Vec must have a conversion to type MVec defined.
template< typename _Vec >
bool
MPlane::GetSegmentIntersect( _Vec& pt, const _Vec& pt0, const _Vec& pt1 ) const
{
    // essentially, we're finding how much of the line resides on one half
    // of the plane, and how much of the line resides on the other half.  I
    // then use that to find a ratio that I then use to interpolate between
    // the two points passed in.
    float d0 = Dist( pt0 );
    float d1 = Dist( pt1 );
    if ( ( d0 < 0.0f ) == ( d1 < 0.0f ) )
    {
        // both points are on the same side of the plane; does not intersect.
        return false;
    }

    // find the ratio of how much of the line resides on opposite sides of
    // the plane.
    d0 = Abs( d0 );
    d1 = Abs( d1 );
    float t = d0 / ( d0 + d1 );
    pt = Lerp( pt0, pt1, t );

    // indicate an intersection point was found.
    return true;
}

//----------------------------------------------------------
// _Vec must have a conversion to type MVec defined.
template< typename _Vec >
bool
MPlane::GetRayIntersect( float& rayMu, const _Vec& rayOrigin, const _Vec& rayDir ) const
{
    // find the distance between the ray's origin and the plane.
    float rayDist = Dist( rayOrigin );

    // if the distance is very close to zero, then the ray originates on the plane;
    // the origin is also the intersection.
    if ( ApproxEqual( rayDist, 0.0f, FLT_EPSILON ) )
    {
        rayMu = 0.0f;
        return true;
    }

    // find the angle between the ray direction and the plane normal.
    float cosTheta = -_normal.Dot( rayDir );

    // if the angle is very close to a right angle, then the ray is parallel 
    // to the plane; does not intersect.
    if ( ApproxEqual( cosTheta, 0.0f, FLT_EPSILON ) )
    {
        return false;
    }

    // solve for the length of the hypotenuse of the triangle formed by the ray's origin,
    // the ray's origin projected onto the plane, and the intersection point.
    rayMu = rayDist / cosTheta;

    // indicate an intersection point was found.
    return true;
}

//----------------------------------------------------------
template< typename _Triangle, typename _Vec >
void
MPlane::SplitTriangle( const _Triangle& tri, _Triangle *posTris, _Triangle *negTris, unsigned int& posCount,
                      unsigned int& negCount, float epsilon ) const
{
    posCount = 0;
    negCount = 0;

    // determine how many points are on which side of the plane, and how many triangles need to be created.
    float d0 = Dist( tri.pt0 );
    float d1 = Dist( tri.pt1 );
    float d2 = Dist( tri.pt2 );
    int vertPosCount = ( ( d0 >= epsilon ) + ( d1 >= epsilon ) + ( d2 >= epsilon ) );
    int vertNegCount = ( ( d0 <= -epsilon ) + ( d1 <= -epsilon ) + ( d2 <= -epsilon ) );

    // check to see if the triangle even collides with the plane.
    if ( vertPosCount == 3 || vertNegCount == 0 )
    {
        *posTris = tri;
        posCount = 1;
        return;
    }
    else if ( vertNegCount == 3 || vertPosCount == 0 )
    {
        *negTris = tri;
        negCount = 1;
        return;
    }

    // if we have two verts on one side, we'll have two tris on one side, one on the other.
    // note that the maximum number of triangles we can possibly get back is 3.
    posCount = vertPosCount;
    negCount = vertNegCount;

    // determine which segment is entirely on one side of the plane.
    if ( vertPosCount != vertNegCount )
    {
        if ( ( d0 >= 0.0f ) == ( d1 >= 0.0f ) )
        {
            _Vec intersect0;
            _Vec intersect1;
            GetSegmentIntersect( intersect0, tri.pt2, tri.pt0 );
            GetSegmentIntersect( intersect1, tri.pt2, tri.pt1 );

            // create the new triangles ( and preserve winding order ).
            if ( d2 >= 0.0f )
            {
                // single triangle on one side.
                posTris[ 0 ] = _Triangle( intersect0, intersect1, tri.pt2 );

                // two triangles on the other.
                negTris[ 0 ] = _Triangle( tri.pt0, tri.pt1, intersect0 );
                negTris[ 1 ] = _Triangle( intersect0, tri.pt1, intersect1 );
            }
            else
            {
                // single triangle on one side.
                negTris[ 0 ] = _Triangle( intersect0, intersect1, tri.pt2 );

                // two triangles on the other.
                posTris[ 0 ] = _Triangle( tri.pt0, tri.pt1, intersect0 );
                posTris[ 1 ] = _Triangle( intersect0, tri.pt1, intersect1 );
            }
        }
        else if ( ( d1 >= 0.0f ) == ( d2 >= 0.0f ) )
        {
            // split the segments that cross the plane.
            _Vec intersect1;
            _Vec intersect2;
            GetSegmentIntersect( intersect1, tri.pt0, tri.pt1 );
            GetSegmentIntersect( intersect2, tri.pt0, tri.pt2 );

            // create the new triangles ( and preserve winding order ).
            if ( d0 >= 0.0f )
            {
                // single triangle on one side.
                posTris[ 0 ] = _Triangle( tri.pt0, intersect1, intersect2 );

                // two triangles on the other.
                negTris[ 0 ] = _Triangle( intersect1, tri.pt1, tri.pt2 );
                negTris[ 1 ] = _Triangle( intersect1, tri.pt2, intersect2 );
            }
            else
            {
                // single triangle on one side.
                negTris[ 0 ] = _Triangle( tri.pt0, intersect1, intersect2 );

                // two triangles on the other.
                posTris[ 0 ] = _Triangle( intersect1, tri.pt1, tri.pt2 );
                posTris[ 1 ] = _Triangle( intersect1, tri.pt2, intersect2 );
            }
        }
        else
        {
            B_ASSERT( ( d2 >= 0.0f ) == ( d0 >= 0.0f ) );
            // split the segments that cross the plane.
            _Vec intersect0;
            _Vec intersect2;
            GetSegmentIntersect( intersect2, tri.pt1, tri.pt2 );
            GetSegmentIntersect( intersect0, tri.pt1, tri.pt0 );

            // create the new triangles ( and preserve winding order ).
            if ( d1 >= 0.0f )
            {
                // single triangle on one side.
                posTris[ 0 ] = _Triangle( intersect0, tri.pt1, intersect2 );

                // two triangles on the other.
                negTris[ 0 ] = _Triangle( intersect2, tri.pt2, tri.pt0 );
                negTris[ 1 ] = _Triangle( intersect2, tri.pt0, intersect0 );
            }
            else
            {
                // single triangle on one side.
                negTris[ 0 ] = _Triangle( intersect0, tri.pt1, intersect2 );

                // two triangles on the other.
                posTris[ 0 ] = _Triangle( intersect2, tri.pt2, tri.pt0 );
                posTris[ 1 ] = _Triangle( intersect2, tri.pt0, intersect0 );
            }
        }
    }
    else
    {
        // we only need to split a single edge.
        if ( d0 < epsilon && d0 > -epsilon )
        {
            // split edges 1 and 2.
            _Vec intersect;
            GetSegmentIntersect( intersect, tri.pt1, tri.pt2 );
            if ( d1 >= 0.0f )
            {
                posTris[ 0 ] = _Triangle( tri.pt0, tri.pt1, intersect );
                negTris[ 0 ] = _Triangle( tri.pt0, intersect, tri.pt2 );
            }
            else
            {
                negTris[ 0 ] = _Triangle( tri.pt0, tri.pt1, intersect );
                posTris[ 0 ] = _Triangle( tri.pt0, intersect, tri.pt2 );
            }
        }
        else if ( d1 < epsilon && d1 > -epsilon )
        {
            // split edges 2 and 0.
            _Vec intersect;
            GetSegmentIntersect( intersect, tri.pt2, tri.pt0 );
            if ( d2 >= 0.0f )
            {
                posTris[ 0 ] = _Triangle( tri.pt1, tri.pt2, intersect );
                negTris[ 0 ] = _Triangle( tri.pt1, intersect, tri.pt0 );
            }
            else
            {
                negTris[ 0 ] = _Triangle( tri.pt1, tri.pt2, intersect );
                posTris[ 0 ] = _Triangle( tri.pt1, intersect, tri.pt0 );
            }
        }
        else if ( d2 < epsilon && d2 > -epsilon )
        {
            // split edges 0 and 1.
            _Vec intersect;
            GetSegmentIntersect( intersect, tri.pt0, tri.pt1 );
            if ( d2 >= 0.0f )
            {
                posTris[ 0 ] = _Triangle( tri.pt2, tri.pt0, intersect );
                negTris[ 0 ] = _Triangle( tri.pt2, intersect, tri.pt1 );
            }
            else
            {
                negTris[ 0 ] = _Triangle( tri.pt2, tri.pt0, intersect );
                posTris[ 0 ] = _Triangle( tri.pt2, intersect, tri.pt1 );
            }
        }
    }
}
