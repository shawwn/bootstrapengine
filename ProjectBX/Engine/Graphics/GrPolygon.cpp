//----------------------------------------------------------
// File:        GrPolygon.cpp
// Author:      Kevin Bray
// Created:     07-31-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygon.h"

// project includes.
#include "MPlane.h"
#include "GrMaterial.h"     // using sptrs.
#include "GrMaterialMgr.h"  // load/save.
#include "GrUberTexture.h"
#include "MMat3x3.h"
#include "UWriter.h"
#include "UReader.h"

static const unsigned int kArrayGrowSize = 3;

float IsInLine( const SVec3& pt0, const SVec3& pt1, const SVec3& testPt, float epsilon );

//**********************************************************
// struct SPolygonVertex
//**********************************************************

//==========================================================
// operators.
//==========================================================

//----------------------------------------------------------
void
SPolygonVertex::NormalizeComponents( unsigned int componentFlags )
{
    if ( componentFlags & GrPolygon::kNormal )
        normal.NormalizeFast();

    if ( componentFlags & GrPolygon::kTangent )
        tangent.NormalizeFast();

    if ( componentFlags & GrPolygon::kBiNormal )
        binormal.NormalizeFast();
}

//----------------------------------------------------------
SPolygonVertex
SPolygonVertex::operator + ( const SPolygonVertex& vertex )
{
    SPolygonVertex newVertex;
    newVertex.position = position + vertex.position;
    newVertex.normal = normal + vertex.normal;
    newVertex.binormal = binormal + vertex.binormal;
    newVertex.tangent = tangent + vertex.tangent;
    newVertex.color[ 0 ] = color[ 0 ] + vertex.color[ 0 ];
    newVertex.color[ 1 ] = color[ 1 ] + vertex.color[ 1 ];
    newVertex.color[ 2 ] = color[ 2 ] + vertex.color[ 2 ];
    newVertex.color[ 3 ] = color[ 3 ] + vertex.color[ 3 ];
    newVertex.uv = uv + vertex.uv;
    return newVertex;
}

//----------------------------------------------------------
SPolygonVertex
operator * ( float scalar, const SPolygonVertex& vertex )
{
    SPolygonVertex newVertex;
    newVertex.position = scalar * vertex.position;
    newVertex.normal = scalar * vertex.normal;
    newVertex.binormal = scalar * vertex.binormal;
    newVertex.tangent = scalar * vertex.tangent;
    newVertex.color[ 0 ] = ( unsigned char )( scalar * vertex.color[ 0 ] );
    newVertex.color[ 1 ] = ( unsigned char )( scalar * vertex.color[ 1 ] );
    newVertex.color[ 2 ] = ( unsigned char )( scalar * vertex.color[ 2 ] );
    newVertex.color[ 3 ] = ( unsigned char )( scalar * vertex.color[ 3 ] );
    newVertex.uv = scalar * vertex.uv;
    return newVertex;
}

//**********************************************************
// class GrPolygon
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygon::GrPolygon()
: _material( 0 )
, _flags( 0 )
, _userFlags( 0 )
, _userData( 0 )
, _skinned( false )
{

}

//----------------------------------------------------------
GrPolygon::GrPolygon( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount, GrMaterial* material )
: _material( material )
, _flags( flags )
, _userFlags( 0 )
, _userData( 0 )
, _skinned( false )
{
    Init( verts, vertexCount );
    CalcPlane();
}

//----------------------------------------------------------
GrPolygon::GrPolygon( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount, GrMaterial* material,
                      const MPlane& plane )
: _material( material )
, _plane( plane )
, _flags( flags )
, _userFlags( 0 )
, _userData( 0 )
, _skinned( false )
{
    Init( verts, vertexCount );
}

//----------------------------------------------------------
GrPolygon::GrPolygon( const GrPolygon& polygon )
: _material( polygon._material )
, _plane( polygon._plane )
, _flags( polygon._flags )
, _vertices( polygon._vertices )
, _skinVertArray( polygon._skinVertArray )
, _userFlags( polygon._userFlags )
, _userData( polygon._userData )
, _skinned( polygon._skinned )
{

}

//----------------------------------------------------------
GrPolygon::GrPolygon( UReader& polygon )
: _material( 0 )
, _flags( 0 )
, _userFlags( 0 )
, _userData( 0 )
, _skinned( false )
{
    Load( polygon );
}

//----------------------------------------------------------
GrPolygon::GrPolygon( unsigned int flags, const SPolygonVertex& v0, const SPolygonVertex& v1, const SPolygonVertex& v2, GrMaterial* material )
: _material( material )
, _flags( flags )
, _userFlags( 0 )
, _userData( 0 )
, _skinned( false )
{
    AddVertex( v0 );
    AddVertex( v1 );
    AddVertex( v2 );

    SVec3 normal = ( v0.position - v1.position ).Cross( v2.position - v1.position );
    _plane = MPlane( normal, v0.position );
//  CalcBox();
}

//----------------------------------------------------------
GrPolygon::~GrPolygon()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygon::SetVertices( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount,
                        GrMaterial* material )
{
    _skinned = false;
    _flags = flags;
    _material = material;
    Init( verts, vertexCount );
    CalcPlane();
//  CalcBox();
}

//----------------------------------------------------------
void
GrPolygon::SetVertices( unsigned int flags, const SPolygonVertex* verts, unsigned int vertexCount,
                        GrMaterial* material, const MPlane& plane )
{
    _skinned = false;
    _flags = flags;
    _material = material;
    _plane = plane;
    Init( verts, vertexCount );
//  CalcBox();
}

//----------------------------------------------------------
void
GrPolygon::SetSkinningInfo( SPolygonSkinVertex* skinVertArray )
{
    _skinned = true;
    _skinVertArray.Clear();
    for ( unsigned int i = 0; i < _vertices.GetElemCount(); ++i )
        _skinVertArray.Push( skinVertArray[ i ] );
}

//----------------------------------------------------------
void
GrPolygon::RemapBoneIndices( unsigned int* indices )
{
    B_ASSERT( _skinned && indices != 0 );

    // remap bone indices using the table passed in.
    for ( unsigned int i = 0; i < _vertices.GetElemCount(); ++i )
    {
        unsigned int oldIndices = _skinVertArray[ i ].boneIds;
        unsigned int newIndices = 0;
        for ( unsigned int j = 0; j < 3; ++j )
        {
            unsigned int idx = oldIndices >> ( j*8 ) & 0xFF;
            newIndices |= indices[ idx ] << ( j*8 );
        }
        _skinVertArray[ i ].boneIds = newIndices;
    }
}

//----------------------------------------------------------
URef< GrMaterial >
GrPolygon::GetMaterial() const
{
    return _material;
}

//----------------------------------------------------------
void
GrPolygon::SetMaterial( URef< GrMaterial > material )
{
    _material = material;
}

//----------------------------------------------------------
URef< GrUberTexture >
GrPolygon::GetUberTexture() const
{
    return _uberTexture;
}

//----------------------------------------------------------
void
GrPolygon::SetUberTexture( URef< GrUberTexture > texture )
{
    _uberTexture = texture;
}

//----------------------------------------------------------
void
GrPolygon::Triangulate( SPolygonTriangle* triangles, SPolygonTriangleSkin* triangleSkinArray ) const
{
    // fan the polygon.
    unsigned int triangle = 0;
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 2; i < vertexCount; ++i, ++triangle )
    {
        triangles[ triangle ].v0 = _vertices[ 0 ];
        triangles[ triangle ].v1 = _vertices[ i - 1 ];
        triangles[ triangle ].v2 = _vertices[ i ];
    }

    if ( _skinned && triangleSkinArray )
    {
        // there should be one skin vertex for every vertex.
        B_ASSERT( _skinVertArray.GetElemCount() == _vertices.GetElemCount() );
        triangle = 0;
        for ( unsigned int i = 2; i < vertexCount; ++i, ++triangle )
        {
            triangleSkinArray[ triangle ].s0 = _skinVertArray[ 0 ];
            triangleSkinArray[ triangle ].s1 = _skinVertArray[ i - 1 ];
            triangleSkinArray[ triangle ].s2 = _skinVertArray[ i ];
        }
    }
}

//----------------------------------------------------------
void
GrPolygon::Triangulate( SPolygonVertex* triangleList ) const
{
    // fan the polygon.
    unsigned int vertex = 0;
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 2; i < vertexCount; ++i, vertex += 3 )
    {
        triangleList[ vertex+0 ] = _vertices[ 0 ];
        triangleList[ vertex+1 ] = _vertices[ i - 1 ];
        triangleList[ vertex+2 ] = _vertices[ i ];
    }
}

//----------------------------------------------------------
bool
GrPolygon::Intersect( SRayIntersectInfo& info, const SVec3& rayStart, const SVec3& rayDir, float maxDist ) const
{
    // calculate the point on the triangle's plane along the ray.
    SVec3 faceNormal( GetNormal() );
    float d = rayDir.Dot( faceNormal );

    // skip test if we're facing the direction of the ray (backfacing
    // to the ray).
    if ( d >= 0.0001f )
    {
//      d = -d;
//      faceNormal = -faceNormal;
        return false;
    }

    // get the first position in the polygon.
    const SVec3& pt0 = _vertices[ 0 ].position;

    // calculate the distance along the ray to the plane.
    SVec3 vec = rayStart - pt0;
    float distance = -vec.Dot( faceNormal ) / d;
//  if ( distance < 0.01f || distance > maxDist )
//      return false;
    if ( distance < 0.0f || distance > maxDist )
        return false;

    // calculate where we intersect the plane the triangle lies on.
    SVec3 point = rayStart + distance * rayDir;
    SVec3 p = point - pt0;

    // using the face normal, find the dominant axis so we can perform
    // our triangle-ray tests in 2D.
    float maxAxis = faceNormal.AbsMax();
    int domAxis = 0;

    // we only need to go through this loop twice... if we fail the
    // first two times, then we know the third axis is the dominant
    // one.
    for ( ; domAxis < 2; ++domAxis )
    {
        if ( Abs( maxAxis - Abs( faceNormal[ domAxis ] ) ) < 0.0001f )
            break;
    }

    // choose an axis to throw away and solve in 2D.
    unsigned int a0 = ( domAxis == 0 ) ? 1 : 0;
    unsigned int a1 = ( domAxis <= 1 ) ? 2 : 1;
//  a0 = domAxis == 0 ? 1 : 0
//  if ( domAxis == 0 )
//  {
//      a0 = 1;
//      a1 = 2;
//  }
//  else if ( domAxis == 1 )
//      a1 = 2;

    // now fan the polygon and perform ray tests with each triangle.
    float u = 0.0f;
    float v = 0.0f;
    unsigned int i = GetVertexCount();
    while ( i-- > 2 )
    {
        const SVec3& pt1 = _vertices[ i-1 ].position;
        const SVec3& pt2 = _vertices[ i ].position;

        // get the other points on the current triangle.
        SVec3 b = pt1 - pt0;
        SVec3 c = pt2 - pt0;

        // calculate the barycentric coordinates.
        u = ( p[a1]*c[a0] - p[a0]*c[a1] ) / ( b[a1]*c[a0] - b[a0]*c[a1] );
        v = ( p[a1]*b[a0] - p[a0]*b[a1] ) / ( c[a1]*b[a0] - c[a0]*b[a1] );

        // return true if we're inside the triangle.
        if ( u >= 0.0f && v >= 0.0f && ( u + v ) <= 1.0f )
        {
            info.hitDist = distance;
            info.hitLoc = point;
            info.u = u;
            info.v = v;
            info.triIdx = i - 2;
            return true;
        }
    }

    // if we got here, then we don't intersect with any triangles in
    // the polygon.
    return false;
}

//----------------------------------------------------------
float
GrPolygon::IntersectFast( const SVec3& rayStart, const SVec3& rayDir, float maxDist ) const
{
    // calculate the point on the triangle's plane along the ray.
    const MVec3& faceNormal = GetNormal();
    float d = -( rayDir.Dot( faceNormal ) );

    // skip test if we're facing the direction of the ray (backfacing
    // to the ray).
//  if ( d < -0.005f )
//      return -1.0f;

    // get the first position in the polygon.
    const SVec3& pt0 = _vertices[ 0 ].position;

    // calculate the distance along the ray to the plane.
    SVec3 vec = rayStart;
    vec -= pt0;
    float distance = vec.Dot( faceNormal );
    if ( d <= 0.0001f || distance < 0.0f || distance > d*maxDist )
        return -1.0f;

    // divide distance by cos theta.
    distance /= d;

    // calculate where we intersect the plane the triangle lies on.
    // note that this becomes rayStart + distance * rayDir - pt0, which
    // is the same as vec + distance * rayDir where vec = rayStart - pt0.
    vec.x += distance * rayDir.x;
    vec.y += distance * rayDir.y;
    vec.z += distance * rayDir.z;
//  vec += distance * rayDir;
//  SVec3 p = vec + distance * rayDir;

    // choose an axis to throw away and solve in 2D.
    float absX = Abs( faceNormal.GetX() );
    float absY = Abs( faceNormal.GetY() );
    float absZ = Abs( faceNormal.GetZ() );
    int a0 = ( absX >= absY ) ? 1 : 0;
    int a1 = ( absY >= absZ ) ? 2 : !a0;

    // now fan the polygon and perform ray tests with each triangle.
    float b[2];
    float c[2];
    for ( int i = GetVertexCount()-2; i > 0; --i )
    {
        const SVec3& pt1 = _vertices[ i+0 ].position;
        const SVec3& pt2 = _vertices[ i+1 ].position;

        // get the other points on the current triangle.
        b[0] = pt1[a0] - pt0[a0];
        b[1] = pt1[a1] - pt0[a1];
        c[0] = pt2[a0] - pt0[a0];
        c[1] = pt2[a1] - pt0[a1];

        // calculate the barycentric coordinates.
        float u = ( vec[a1]*c[0] - vec[a0]*c[1] ) / ( b[1]*c[0] - b[0]*c[1] );
        float v = ( vec[a1]*b[0] - vec[a0]*b[1] ) / ( c[1]*b[0] - c[0]*b[1] );

        // return true if we're inside the triangle.
        if ( u >= 0.0f && v >= 0.0f && ( u + v ) <= 1.0f )
            return distance;
    }
    

    // if we got here, then we don't intersect with any triangles in
    // the polygon.
    return -1.0f;
}

//----------------------------------------------------------
GrPolygon::EPLANERELATION
GrPolygon::CalcPlaneRelation( const MPlane& plane, float planeDistEpsilon ) const
{
    // go through all vertices and determine the side.
    EPLANERELATION relation = EPR_COPLANAR;
    const unsigned int elemCount = _vertices.GetElemCount();
    for ( unsigned int i = 0; i < elemCount; ++i )
    {
        int curSide = plane.GetSide( _vertices[ i ].position, planeDistEpsilon );
        if ( curSide != 0 )
        {
            EPLANERELATION curRelation = ( curSide == 1 ? EPR_FRONT : EPR_BACK );
            if ( relation == EPR_COPLANAR )
                relation = curRelation;     // initialize the relation.
            else if ( curRelation != relation )
                return EPR_SPLIT;           // both sides.
        }
    }

    // return the relation we've determined.
    return relation;
}

//----------------------------------------------------------
int
GrPolygon::Split( GrPolygon& front, GrPolygon& back, const MPlane& plane, float planeDistEpsilon ) const
{
    // get the number of vertices.
    unsigned int vertexCount = GetVertexCount();

    // place to store vertices in front and in back.
    UFastArray< SPolygonVertex > frontVerts;
    UFastArray< SPolygonVertex > backVerts;

    // get the starting vertex.
    const SPolygonVertex* vertexA = &GetVertex( vertexCount - 1 );

    // get the starting side.
    int sideA = 0;
    unsigned int vert = vertexCount;
    while ( sideA == 0 && vert-- > 0 )
        sideA = plane.GetSide( GetVertex( vert ).position, planeDistEpsilon );

    // are we entirely coplanar?
    if ( sideA == 0 )
    {
        // choose the side the polygon is facing.
        if ( GetNormal().Dot( plane.GetNormal() ) >= 0.0f )
        {
            front = *this;
            return 1;
        }
        else
        {
            back = *this;
            return -1;
        }
    }

    // simply go through the vertices and classify.
    bool prevOnPlane = ( vert != ( vertexCount-1 ) );
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        const SPolygonVertex* vertexB = &GetVertex( i );
        int sideB = plane.GetSide( vertexB->position, planeDistEpsilon );
        if ( sideB > 0 )
        {
            if ( sideA < 0 )
            {
                if ( !prevOnPlane )
                {
                    float distA = Abs( plane.Dist( vertexA->position ) );
                    float distB = Abs( plane.Dist( vertexB->position ) );
                    float t = distA / ( distA + distB );
                    frontVerts.Push() = backVerts.Push() = Lerp( *vertexA, *vertexB, t );
                }
                else
                    frontVerts.Push() = *vertexA;
            }
            frontVerts.Push( *vertexB );
            prevOnPlane = false;
        }
        else if ( sideB < 0 )
        {
            if ( sideA > 0 )
            {
                if ( !prevOnPlane )
                {
                    float distA = Abs( plane.Dist( vertexA->position ) );
                    float distB = Abs( plane.Dist( vertexB->position ) );
                    float t = distA / ( distA + distB );
                    frontVerts.Push() = backVerts.Push() = Lerp( *vertexA, *vertexB, t );
                }
                else
                    backVerts.Push() = *vertexA;
            }
            backVerts.Push( *vertexB );
            prevOnPlane = false;
        }
        else
        {
            prevOnPlane = true;
            if ( sideA == -1 )
                backVerts.Push( *vertexB );
            else
                frontVerts.Push( *vertexB );
        }

        // update previous vertex info.
        vertexA = vertexB;
        if ( sideB != 0 )
            sideA = sideB;
    }

    B_ASSERT( frontVerts.GetElemCount() == 0 || frontVerts.GetElemCount() >= 3 );
    B_ASSERT( backVerts.GetElemCount() == 0 || backVerts.GetElemCount() >= 3 );
    B_ASSERT( frontVerts.GetElemCount() > 0 || backVerts.GetElemCount() > 0 );

    // if we're not coplanar, we need to create the polygon.
    int ret = 0;
    if ( frontVerts.GetElemCount() >= 3 )
    {
        front.SetVertices( _flags, frontVerts.GetPtr(), frontVerts.GetElemCount(), _material, _plane );
        ret += 1;
    }
    if ( backVerts.GetElemCount() >= 3 )
    {
        back.SetVertices( _flags, backVerts.GetPtr(), backVerts.GetElemCount(), _material, _plane );
        ret -= 1;
    }
    return ret;
}

//----------------------------------------------------------
bool
GrPolygon::CheckTJunc( const GrPolygon& polygon, float epsilon )
{
    // we can't perform this on skinned vertices!  How would we
    // determine what bone(s) the any new verts are influenced by?
    B_ASSERT( !_skinned );

    // if both polygons aren't edge polygons, then there is no
    // possibility of a t-junction.
//  if ( ( polygon._userFlags & EFV_EDGEPOLY ) != 0 || ( _userFlags & EFV_EDGEPOLY ) != 0 )
//      return;

    // determine the squared epsilon.
    const float distSqrEpsilon = epsilon * epsilon;

    unsigned int vertexCount = GetVertexCount();
    unsigned int polyVertexCount = polygon.GetVertexCount();

    // check for a t-junction.
    UFastArray< std::map< float, SPolygonVertex > > vertsToAdd;
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        std::map< float, SPolygonVertex >& edgeVertsToAdd = vertsToAdd.Push();
        const SPolygonVertex& edgeV0 = _vertices[ i ];
        const SPolygonVertex& edgeV1 = _vertices[ ( i + 1 ) % vertexCount ];
        for ( unsigned int j = 0; j < polyVertexCount; ++j )
        {
            // get the current current vertex in the other polygon.
            const SVec3& pos = polygon.GetVertex( j ).position;

            // if the current point on the other polygon resides on one of our
            // vertices, ignore it.
            if ( ( pos.DistSqr( edgeV0.position ) < distSqrEpsilon ) || ( pos.DistSqr( edgeV1.position ) < distSqrEpsilon ) )
                continue;

            // check for a point residing on the current edge.
            float t = IsInLine( edgeV0.position, edgeV1.position, pos, epsilon );
            if ( t >= 0.0f )
            {
                // allocate and generate a new vertex.  Weld it's position to
                // the other polygon's position.
                SPolygonVertex& newVert = edgeVertsToAdd[ t ];
                newVert = Lerp( edgeV0, edgeV1, t );
                newVert.position = pos;
            }
        }
    }

    // add new vertices.  all done.
    unsigned int edgeCount = vertsToAdd.GetElemCount();
    unsigned int adj = 1;
    for ( unsigned int i = 0; i < edgeCount; ++i )
    {
        std::map< float, SPolygonVertex >& newEdgeVerts = vertsToAdd[ i ];
        std::map< float, SPolygonVertex >::iterator iter = newEdgeVerts.begin();
        std::map< float, SPolygonVertex >::iterator end = newEdgeVerts.end();
        for ( ; iter != end; ++iter, ++adj )
            InsertVertex( i + adj, iter->second );
    }

    // make sure it's clear on exit since this variable is static.
    vertsToAdd.Clear();

    // return true if we've added vertices to fix t-junctions.
    return adj > 0;
}

//----------------------------------------------------------
bool
GrPolygon::IsDegenerate( float epsilon ) const
{
    // less then 3 verts, then its definately degenerate.
    return ( GetArea() <= epsilon );
}

//----------------------------------------------------------
bool
GrPolygon::RemoveDuplicateVerts( float distEpsilonSqr )
{
    // remove any duplicate verts.
    int i = 0;
    while ( i < ( int )GetVertexCount() )
    {
        unsigned int prevVertIdx = ( i - 1 ) < 0 ? ( GetVertexCount() - 1 ) : ( i - 1 );
        SPolygonVertex& prevVert = _vertices[ prevVertIdx ];
        SPolygonVertex& curVert = _vertices[ i ];
        if ( curVert.position.DistSqr( prevVert.position ) < distEpsilonSqr )
        {
            // remove the current vertex and process the next one.
            RemoveVertex( i );
            continue;
        }
        ++i;
    }

    // return false if the polygon is now degenerate.
    return ( GetVertexCount() >= 3 );
}

//----------------------------------------------------------
void
GrPolygon::WeldVertices( const GrPolygon& polygon, float distEpsilonSqr )
{
    // check to see if any polygon verts need to be welded together.
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        unsigned int neighborVertexCount = polygon.GetVertexCount();
        for ( unsigned int j = 0; j < neighborVertexCount; ++j )
        {
            // lets weld.
            if ( _vertices[ i ].position.DistSqr( polygon.GetVertex( j ).position ) < distEpsilonSqr )
                _vertices[ i ].position = polygon.GetVertex( j ).position;
        }
    }
}

//----------------------------------------------------------
void
GrPolygon::ReverseWinding()
{
    // create an array with vertices reversed in order.
    unsigned int vertexCount = GetVertexCount();
    SPolygonVertex* newVerts = ( SPolygonVertex* )alloca( vertexCount * sizeof( SPolygonVertex ) );
    for ( unsigned int i = 0, r = vertexCount-1; i < vertexCount; ++i, --r )
        newVerts[ r ] = _vertices[ i ];

    // set the vertices.
    SetVertices( _flags, newVerts, vertexCount, _material );
}

//----------------------------------------------------------
bool
GrPolygon::IsEdgePolygon( const MPlane& plane, float planeDistEpsilon ) const
{
    // this function returns true if an edge lies on the plane passed in.
    // Since we know the polygon will be convex, if two or more vertices
    // lie on the plane in question, then we are an edge polygon.
    unsigned int vertexCount = GetVertexCount();
    unsigned int edgeVertexCount = 0;
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        // if we on the plane and there are two vertices on the plane so
        // far, then we know we have an edge polygon.
        if ( !plane.GetSide( _vertices[ i ].position, planeDistEpsilon ) )
            if ( ++edgeVertexCount == 2 )
                return true;
    }
    return false;
}

//----------------------------------------------------------
bool
GrPolygon::IsCoplanar( const MPlane& plane, float epsilon ) const
{
    // if we're facing wildly different directions, return false.
    if ( plane.GetNormal().Dot( _plane.GetNormal() ) < 0.7f )
        return false;

    // go through all vertices checking to see if we're coplanar.
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        if ( plane.GetSide( _vertices[ i ].position, epsilon ) != 0 )
            return false;
    }

    // if we made it through all vertices, then we're coplanar.
    return true;
}

//----------------------------------------------------------
bool
GrPolygon::IsNeighbor( const GrPolygon& polygon, float epsilon ) const
{
    // This function checks edges to see if they match up.  The edges
    // must be going opposite direction or else they are not considered
    // neighbors.

    // go through each edge in this polygon.
    unsigned int edgeCount = _vertices.GetElemCount();
    for ( unsigned int i = 0; i < edgeCount; ++i )
    {
        MVec3 vertA = _vertices[ i ].position;
        MVec3 vertB = _vertices[ (i+1) % edgeCount ].position;

        // go through each edge in the polygon passed in.
        unsigned int polyEdgeCount = polygon._vertices.GetElemCount();
        for ( unsigned int j = 0; j < polyEdgeCount; ++j )
        {
            // check to see if our vertices match up and the edge direction
            // is opposing.
            MVec3 polyVertA = polygon._vertices[ j ].position;
            MVec3 polyVertB = polygon._vertices[ (j+1) % polyEdgeCount ].position;
            if ( ( vertA - polyVertB ).AbsMax() < epsilon &&
                 ( vertB - polyVertA ).AbsMax() < epsilon )
                return true;
        }
    }

    // if we get here then the polygon is not a neighbor.
    return false;
}

//----------------------------------------------------------
GrPolygon
GrPolygon::Transform( const MMat4x4& transform, const MMat3x3& normalRot ) const
{
    GrPolygon newPoly = *this;
    newPoly.TransformFast( transform, normalRot );
    return newPoly;
}

//----------------------------------------------------------
void
GrPolygon::TransformFast( const MMat4x4& transform, const MMat3x3& normalRot )
{
    // transform all of the points by the matrix passed in.
    unsigned int vertexCount = _vertices.GetElemCount();
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        MVec3 pos = _vertices[ i ].position;
        MVec3 norm = _vertices[ i ].normal;
        MVec3 tan = _vertices[ i ].tangent;
        MVec3 binorm = _vertices[ i ].binormal;

        B_ASSERT( norm.MagSqr() > 0.0001f );
        B_ASSERT( tan.MagSqr() > 0.0001f );
        B_ASSERT( binorm.MagSqr() > 0.0001f );

        transform.TransformCoordFast( pos );
        normalRot.RotatePointFast( norm );
        normalRot.RotatePointFast( tan );
        normalRot.RotatePointFast( binorm );

        _vertices[ i ].position = pos;
        if ( norm.MagSqr() > 0.001f )
            _vertices[ i ].normal = norm.Normalized();
        else
            _vertices[ i ].normal = norm;
        _vertices[ i ].tangent = tan.Normalized();
        _vertices[ i ].binormal = binorm.Normalized();

        B_ASSERT( norm.MagSqr() > 0.0001f );
        B_ASSERT( tan.MagSqr() > 0.0001f );
        B_ASSERT( binorm.MagSqr() > 0.0001f );
    }

    // transform the plane the points lie in.
    _plane.TransformFast( transform, normalRot );
}

//----------------------------------------------------------
float
GrPolygon::GetArea() const
{
    // fan the polygon and calculate area for each triangle.
    float area = 0.0f;
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 2; i < vertexCount; ++i )
    {
        // get edge vectors of the current triangle.
        MVec3 edge0 = _vertices[ i - 1 ].position - _vertices[ i - 2 ].position;
        MVec3 edge1 = _vertices[ i - 0 ].position - _vertices[ i - 2 ].position;

        // skip this triangle if one of the edges is 0 length as it obviously has
        // no area.
        if ( edge0.MagSqr() <= 0.0001f || edge1.MagSqr() <= 0.0001f )
            continue;

        // calculate the area of the current triangle.
        float sinTheta = edge0.Normalized().Cross( edge1.Normalized() ).Mag();
        area += 0.5f * Abs( sinTheta ) * edge0.Mag() * edge1.Mag();
    }

    // return the total area.
    return area;
}

//----------------------------------------------------------
GrPolygon&
GrPolygon::operator = ( const GrPolygon& polygon )
{
    _vertices = polygon._vertices;
    _skinVertArray = polygon._skinVertArray;
    _material = polygon._material;
    _plane = polygon._plane;
    _flags = polygon._flags;
    _userFlags = polygon._userFlags;
    _userData = polygon._userData;
    _skinned = polygon._skinned;
    return *this;
}

//----------------------------------------------------------
void
GrPolygon::Save( UWriter& writer ) const
{
    // write the polygon flags.
    writer.WriteUInt( _flags );

    // write the polygon vertices.
    const unsigned int vertexCount = _vertices.GetElemCount();
    writer.WriteUInt( vertexCount );
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        writer.WriteBytes( &_vertices[ i ].position, sizeof( SVec3 ) );
        if ( _flags & kNormal )
            writer.WriteBytes( &_vertices[ i ].normal, sizeof( SVec3 ) );
        if ( _flags & kTangent )
            writer.WriteBytes( &_vertices[ i ].tangent, sizeof( SVec3 ) );
        if ( _flags & kBiNormal )
            writer.WriteBytes( &_vertices[ i ].binormal, sizeof( SVec3 ) );
        if ( _flags & kColor )
            writer.WriteBytes( &_vertices[ i ].color, sizeof( unsigned long ) );
        if ( _flags & kUV )
            writer.WriteBytes( &_vertices[ i ].uv, sizeof( SVec2 ) );
    }

    // save the skin vertices.
    const unsigned int skinVertexCount = _skinVertArray.GetElemCount();
    writer.WriteUInt( skinVertexCount );
    for ( unsigned int i = 0; i < skinVertexCount; ++i )
    {
        writer.WriteBytes( &_skinVertArray[ i ], sizeof( SPolygonSkinVertex ) );
    }

    // save the material name.
    if ( _material != 0 )
        writer.WriteString( _material->GetName() );
    else
        writer.WriteString( "" );

    // save the plane.
    _plane.Save( writer );

    // save user flags.
    writer.WriteUInt( _userFlags );

    // save the skinned flag.
    writer.WriteUInt( _skinned ? 1 : 0 );
}

//----------------------------------------------------------
void
GrPolygon::Load( UReader& reader )
{
    _vertices.Clear();
    _skinVertArray.Clear();

    // write the polygon flags.
    _flags = reader.ReadUInt();

    // write the polygon vertices.
    const unsigned int vertexCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        SPolygonVertex& vertex = _vertices.Push();
        reader.ReadBytes( &vertex.position, sizeof( SVec3 ) );
        if ( _flags & kNormal )
            reader.ReadBytes( &vertex.normal, sizeof( SVec3 ) );
        if ( _flags & kTangent )
            reader.ReadBytes( &vertex.tangent, sizeof( SVec3 ) );
        if ( _flags & kBiNormal )
            reader.ReadBytes( &vertex.binormal, sizeof( SVec3 ) );
        if ( _flags & kColor )
            reader.ReadBytes( &vertex.color, sizeof( unsigned long ) );
        if ( _flags & kUV )
            reader.ReadBytes( &vertex.uv, sizeof( SVec2 ) );
    }

    // save the skin vertices.
    const unsigned int skinVertexCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < skinVertexCount; ++i )
    {
        reader.ReadBytes( &_skinVertArray.Push(), sizeof( SPolygonSkinVertex ) );
    }

    // save the material name.
    tstring materialName = reader.ReadString();
    if ( materialName.length() > 0 )
    {
        tstring error;
        _material = gGrMaterialMgr->GetMaterial( materialName, error );
    }
    else
        _material = 0;

    // save the plane.
    _plane.Load( reader );

    // save user flags.
    _userFlags = reader.ReadUInt();

    // save the skinned flag.
    _skinned = ( reader.ReadUInt() == 1 );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygon::Init( const SPolygonVertex* verts, unsigned int vertexCount )
{
    // make sure we remove the previous verts.
    _vertices.Clear();

    // must specify vertices!
    B_ASSERT( verts != 0 );

    // must be at least a triangle.
    B_ASSERT( vertexCount >= 3 );

    for ( unsigned int i = 0; i < vertexCount; ++i )
        _vertices.Push( verts[ i ] );
}

//----------------------------------------------------------
void
GrPolygon::CalcPlane()
{
/*  -- This "virtual plane" crap is all but useless.
    // calculate the plane.
    MVec3 normal = SVec3( 0, 0, 0 );
    MVec3 position = SVec3( 0, 0, 0 );

    // take the average normal and average position.
    unsigned int vertexCount = GetVertexCount();
    float w = 1.0f / float( vertexCount );
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        normal += w * _vertices[ i ].normal;
        position += w * _vertices[ i ].position;
    }
*/
#if 0
    // determine what vertices to use.
    unsigned int vertexCount = GetVertexCount();

    unsigned int v0 = 0;
    unsigned int v1 = ( vertexCount * 2 / 3 ) - 1;
    unsigned int v2 = vertexCount - 1;

    MVec3 edge0 = _vertices[ v2 ].position - _vertices[ v1 ].position;
    MVec3 edge1 = _vertices[ v2 ].position - _vertices[ v0 ].position;

    MVec3 normal = edge1.Cross( edge0 );
    if ( normal.MagSqr() > 0.0001f )
    {
        _plane = MPlane( normal, _vertices[ v0 ].position );
        if ( IsCoplanar( _plane, 1.0f ) )
            return;
    }

    // simply create a dummy plane that passes through the current position.
    _plane = MPlane( MVec3( 0.0f, 1.0f, 0.0f ), _vertices[ 0 ].position );
#else
    // calculate the actual plane of the surface.
    unsigned int vertexCount = GetVertexCount();
    for ( unsigned int i = 2; i < vertexCount; ++i )
    {
        // get the edges.
        MVec3 edge0 = _vertices[ i ].position - _vertices[ i-1 ].position;
        MVec3 edge1 = _vertices[ i ].position - _vertices[ i-2 ].position;
        if ( edge0.MagSqr() <= 0.0001f || edge1.MagSqr() <= 0.0001f )
            continue;

        MVec3 normal = edge1.Normalized().Cross( edge0.Normalized() );
        if ( normal.MagSqr() > 0.0001f )
        {
            _plane = MPlane( normal, _vertices[ i ].position );
            return;
        }
    }

    // since we couldn't create a valid plane, create an approximate
    // one using the average of the vertex normals.
    B_ASSERT( _flags & kNormal );
    MVec3 normal( 0.0f, 0.0f, 0.0f );
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        normal += GetVertex( i ).normal;
    }

    // normalize the normal.
    if ( normal.MagSqr() < 0.00001f )
        normal.SetY( 1.0f );
    else
        normal.Normalize();

    // set the plane.
    _plane = MPlane( normal, GetVertex( 0 ).position );
#endif
}

//----------------------------------------------------------
void
GrPolygon::AddVertex( const SPolygonVertex& vertex )
{
    // adding a vertex when skinning info has already been set!
    // this could possibly mean there are more vertices than there
    // are skin vertices... there should be one skin-vertex per
    // vertex!
    B_ASSERT( !_skinned );

    // add the new vertex.
    _vertices.Push( vertex );
}

//----------------------------------------------------------
void
GrPolygon::InsertVertex( unsigned int pos, const SPolygonVertex& vertex )
{
    // inserting a vertex when skinning info has already been set!
    // this could possibly mean there are more vertices than there
    // are skin vertices... there should be one skin-vertex per
    // vertex!
    B_ASSERT( !_skinned );

    // insert the new vertex.
    _vertices.Insert( pos, vertex );
}

//----------------------------------------------------------
void
GrPolygon::RemoveVertex( unsigned int index )
{
    _vertices.Erase( index );
    if ( _skinned )
    {
        // there should be one skin vertex for every vertex if we're skinned.
        B_ASSERT( _skinVertArray.GetElemCount() == _vertices.GetElemCount() );
        _skinVertArray.Erase( index );
    }
}


//==========================================================
// global functions.
//==========================================================

//----------------------------------------------------------
float
IsInLine( const SVec3& pt0, const SVec3& pt1, const SVec3& testPt, float epsilon )
{
    // return the shortest distance to the line from the point.
    MVec3 hyp = testPt - pt0;
    MVec3 vec = pt1 - pt0;

    // calculate the direction vector's length and normalize it.
    float invLen = 1.0f / vec.Mag();
    MVec3 dir = invLen * vec;

    // calculate where the line comes closest to point 'pt'.
    float t = invLen * dir.Dot( hyp );

    // if we don't come closest to the point 'pt' somewhere on
    // the segment, take the distance to the respective end
    // points.
    if ( t < 0.0f || t > 1.0f )
        return -1.0f;

    // return the distance to the line.
    if ( hyp.DistSqr( t * vec ) < epsilon )
        return t;

    // not on the line.
    return -1.0f;
}
