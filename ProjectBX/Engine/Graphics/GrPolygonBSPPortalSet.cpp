#include "graphics_afx.h"

// class header.
#include "GrPolygonBSPPortalSet.h"

// project includes.
#include "MPlane.h"
#include "GrPolygon.h"
#include "GrPolygonGroup.h"
#include "GrPolygonBSPSplit.h"
#include "GrPolygonBSPArea.h"
#include "GrMaterial.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrFrustum.h"
#include "MAABox.h"

//static const float kPortalEpsilon = 0.1f;

//**********************************************************
// class GrBSPPortal
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

GrPolygonBSPPortalSet::GrPolygonBSPPortalSet()
: _creator( 0 )
, _owner( 0 )
, _neighbor( 0 )
, _hullPortal( 0 )
, _hullDirty( true )
#ifdef _CACHE_CULL_PLANES
, _lastFromPortal( 0 )
#endif
{

}

//----------------------------------------------------------
GrPolygonBSPPortalSet::GrPolygonBSPPortalSet( GrPolygonBSPSplit* split, float scale )
: _creator( split )
, _plane( split->GetPlane() )
, _owner( 0 )
, _hullPortal( 0 )
, _hullDirty( true )
#ifdef _CACHE_CULL_PLANES
, _lastFromPortal( 0 )
#endif
{
    B_ASSERT( split != 0 );

    GrPortal::VertexArray polygon;
    CreatePlanePolygon( polygon, split->GetPlane(), scale );
    _portals.Push( GrPortal( split->GetPlane(), polygon ) );
}

//----------------------------------------------------------
GrPolygonBSPPortalSet::~GrPolygonBSPPortalSet()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
GrPolygonBSPPortalSet::Subtract( const GrPolygon& polygon, float epsilon )
{
    B_ASSERT( _creator != 0 );

    // mark the hull as dirty.
    _hullDirty = true;

    // TODO: Rewrite this for better performance.  If this is done on a
    // per polygon basis, then we could get much better performance.  This
    // would be because fewer polygons would be created.
    //
    // for each polygon:
    //  for each plane,
    //   split polygon.
    //   remove polygon from list.
    //   remove polygons on same side of all planes.
    //   add remaining polygons only if a polygon was removed, otherwise
    //    add the original polygon back to the list.

    // build a list of planes that face inward from the polygon passed in.
    int invert = 0;
    UFastArray< MPlane > planeArray;
    const unsigned int polygonVertexCount = polygon.GetVertexCount();
    for ( unsigned int i = 0; i < polygonVertexCount; ++i )
    {
        // determine the previous and current vertices.
        unsigned int curVert = i;
        unsigned int nextVert = ( i + 1 ) % polygonVertexCount;

        // calculate the current edge.
        MVec3 curPos = polygon.GetVertex( curVert ).position;
        MVec3 nextPos = polygon.GetVertex( nextVert ).position;
        MVec3 curEdge = nextPos - curPos;

        // calculate the new plane's normal.
        MVec3 planeNormal = polygon.GetNormal().Cross( curEdge );

        // ignore this edge if it's degenerate.
        if ( planeNormal.MagSqr() < 0.001f )
            continue;

        // ensure that the plane is facing inward.
        if ( invert == 0 )
        {
            invert = 1;
            MPlane testPlane( planeNormal, curPos );
            for ( unsigned int j = 0; j < polygonVertexCount; ++j )
            {
                if ( testPlane.GetSide( polygon.GetVertex( j ).position, epsilon ) < 0 )
                {
                    planeNormal = -planeNormal;
                    invert = -1;
                    break;
                }
            }
        }
        else if ( invert == -1 )
            planeNormal = -planeNormal;

        // normalize the plane's normal.
        planeNormal.Normalize();

        // build the new plane and add.
        planeArray.Push( MPlane( planeNormal, curPos ) );
    }

    // get the number of planes (should be one for every edge).
    const unsigned int curPlaneCount = planeArray.GetElemCount();
    if ( curPlaneCount < 3 )
        return ( _portals.GetElemCount() > 0 );

    GrPortal frontPortal;
    GrPortal backPortal;
    const unsigned int portalCount = _portals.GetElemCount();
    for ( int i = portalCount-1; i >= 0; --i )
    {
        GrPortal curPortal = _portals[ i ];
        _portals.Erase( i );

        // check to see if the current portal is entirely on the negative side of
        // any of the planes.
        bool skip = false;
        const unsigned int curPortalVertexCount = curPortal.GetVertexCount();
        for ( unsigned int j = 0; j < curPlaneCount; ++j )
        {
            // go through all the vertices and see if they're all on the negative
            // side of the current plane.
            unsigned int k = 0;
            for ( ; k < curPortalVertexCount; ++k )
            {
                if ( planeArray[ j ].GetSide( curPortal.GetVertex( k ), epsilon ) > 0 )
                    break;
            }
            if ( k == curPortalVertexCount )
            {
                skip = true;
                break;
            }
        }

        // do we need to early out?
        if ( skip )
        {
            _portals.Push( curPortal );
            continue;
        }

        // split the current portal by the planes.  Only keep whats on the
        // negative side.  Things on the positive side replace whats currently
        // in the portal's vertex list.
        for ( unsigned int j = 0; j < curPlaneCount; ++j )
        {
            // split.
            int ret = curPortal.Split( frontPortal, backPortal, planeArray[ j ], _plane, epsilon );
            if ( ret <= 0 && !backPortal.IsDegenerate() )
                _portals.Push( backPortal );    // add to our list of portals we should keep.
            if ( ret >= 0 && !frontPortal.IsDegenerate() )
                curPortal = frontPortal;        // replace current portal... will be split with next plane.

            // nothing left to split!
            if ( ret == -1 || frontPortal.IsDegenerate() )
                break;
        }
    }

    // return true if there are portals still left.
    return ( _portals.GetElemCount() > 0 );
}

//----------------------------------------------------------
int
GrPolygonBSPPortalSet::Split( GrPolygonBSPPortalSet& front, GrPolygonBSPPortalSet& back, const MPlane& plane, float epsilon ) const
{
    // clear any polygons.
    front._portals.Clear();
    front._creator = _creator;
    front._plane = _plane;
    back._portals.Clear();
    back._creator = _creator;
    back._plane = _plane;

    // get the portal count.
    const unsigned int polygonCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < polygonCount; ++i )
    {
        // get the current polygon.
        const GrPortal& curPortal = _portals[ i ];

        // split the polygon.
        GrPortal frontPortal;
        GrPortal backPortal;
        int ret = curPortal.Split( frontPortal, backPortal, plane, _plane, epsilon );

        // put the polygon(s) into the correct portal.
        if ( ret >= 0 && !frontPortal.IsDegenerate() )
        {
            B_ASSERT( frontPortal.GetVertexCount() > 0 );
            front._portals.Push( frontPortal );
        }
        if ( ret <= 0 && !backPortal.IsDegenerate() )
        {
            B_ASSERT( backPortal.GetVertexCount() > 0 );
            back._portals.Push( backPortal );
        }
    }

    // return -1 of only the back has polygons and return 1 if only
    // the front has polygons.
    int ret = 0;
    if ( front._portals.GetElemCount() != 0 )
        ret += 1;
    if ( back._portals.GetElemCount() != 0 )
        ret -= 1;

    // return 0 since both have polygons.
    return ret;
}

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::ReverseWinding()
{
    // get the portal count.
    const unsigned int portalCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalCount; ++i )
        _portals[ i ].ReverseWinding();

    // reverse the normal on the plane.
    _plane.ReverseFacing();

    if ( _hullPortal )
        _hullPortal->ReverseWinding();
}

//----------------------------------------------------------
unsigned int
GrPolygonBSPPortalSet::GetPortalCount() const
{
    return _portals.GetElemCount();
}

//----------------------------------------------------------
const GrPortal&
GrPolygonBSPPortalSet::GetPortal( unsigned int idx ) const
{
    B_ASSERT( idx < _portals.GetElemCount() );
    return _portals[ idx ];
}

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::GenCullPlaneSet( std::vector< MPlane >& cullPlanes, const GrPortal& fromPortal, unsigned int toPortalIdx, bool best, bool cache ) const
{
    // can we use the use cache?
#ifdef _CACHE_CULL_PLANES
    if ( cache )
    {
        if ( _lastFromPortal == &fromPortal )
        {
            PlaneMap::const_iterator iter = _cullPlaneCache.find( toPortalIdx );
            if ( iter != _cullPlaneCache.end() )
            {
                cullPlanes.insert( cullPlanes.end(), iter->second.begin(), iter->second.end() );
                return;
            }
        }
        else
        {
            _lastFromPortal = &fromPortal;
            _cullPlaneCache.clear();
        }
    }
#endif

    // check to see if we need to flip the winding of the portal we're looking
    // at.
    GrPortal toPortal = GetPortal( toPortalIdx );
    const MPlane& toPortalPlane = GetPlane();

#ifdef _CACHE_CULL_PLANES
    std::vector< MPlane >* planeCache;
    if ( cache )
        planeCache = &_cullPlaneCache[ toPortalIdx ];
#endif

    // if the portal we're looking at is facing away from the current portal,
    // then we need to reverse it's winding.
    unsigned int fromPortalVertexCount = fromPortal.GetVertexCount();
    for ( unsigned int i = 0; i < fromPortalVertexCount; ++i )
    {
        int side = toPortalPlane.GetSide( fromPortal.GetVertex( i ), 0.01f );
        if ( side < 0 )
        {
            toPortal.ReverseWinding();
            break;
        }
        else if ( side > 0 )
            break;
    }

    // now we can go through the edges of the plane we're looking at.
    unsigned int toPortalVertexCount = toPortal.GetVertexCount();
    for ( unsigned int i = 0; i < toPortalVertexCount; ++i )
    {
        // worst possible value (cosine between portal face normals).
        float bestDP = best ? -1000.0f : 1000.0f;
        MVec3 bestNormal;

        // get the current edge on the plane we're looking at.
        const MVec3& v0 = toPortal.GetVertex( i );
        const MVec3& v1 = toPortal.GetVertex( ( i + 1 ) % toPortalVertexCount );

        // go through all vertices in the plane we're looking from in
        // search of the best plane.
        for ( unsigned int j = 0; j < fromPortalVertexCount; ++j )
        {
            // calculate the normal of the current plane.
            MVec3 normal = ( fromPortal.GetVertex( j ) - v0 ).Cross( v1 - v0 );
            if ( normal.MagSqr() < 0.0001f )
                continue;

            // check to see if the current plane is better than the
            // previous plane.
            normal.Normalize();
            float curDP = normal.Dot( toPortalPlane.GetNormal() );
            if ( best )
            {
                if ( curDP > bestDP )
                {
                    bestDP = curDP;
                    bestNormal = normal;
                }
            }
            else
            {
                if ( curDP < bestDP )
                {
                    bestDP = curDP;
                    bestNormal = normal;
                }
            }
        }

        // make sure that the bestDP was actually set.  If not, then
        // we didn't find ANY good planes... just means we have shared
        // vertices.
        if ( best )
        {
            if ( bestDP > -1000.0f )
            {
                // create the best plane (plane that is most coplanar to the
                // portal we're looking at).
                MPlane bestPlane( bestNormal, v0 );
                cullPlanes.push_back( bestPlane );
#ifdef _CACHE_CULL_PLANES
                if ( cache )
                    planeCache->push_back( bestPlane );
#endif
            }
        }
        else
        {
            if ( bestDP < 1000.0f )
            {
                // create the best plane (plane that is most coplanar to the
                // portal we're looking at).
                MPlane bestPlane( bestNormal, v0 );
                cullPlanes.push_back( bestPlane );
#ifdef _CACHE_CULL_PLANES
                if ( cache )
                    planeCache->push_back( bestPlane );
#endif
            }
        }
    }
}

//----------------------------------------------------------
const GrPortal&
GrPolygonBSPPortalSet::GetHullPortal() const
{
    if ( !_hullDirty )
    {
        B_ASSERT( _hullPortal != 0 );
        return *_hullPortal;
    }

    // clear the dirty flag.
    _hullDirty = false;


    // return the new hull portal.
    return *_hullPortal;
}

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::SetOwner( GrPolygonBSPArea* owner )
{
    _owner = owner;
}

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::SetNeighbor( GrPolygonBSPArea* neighbor )
{
    _neighbor = neighbor;
}
/*
//----------------------------------------------------------
bool
GrPolygonBSPPortalSet::IsOverlapping( const GrPolygonBSPPortalSet& portalSet, float epsilon ) const
{
    // check our portals against their portals for any overlap.
    unsigned int portalCount = _portals.GetElemCount();
    for ( unsigned int i = 0; i < portalCount; ++i )
    {
        unsigned int otherCount = portalSet._portals.GetElemCount();
        for ( unsigned int j = 0; j < otherCount; ++j )
        {
            // check if the portals overlap.  If so, return true.
            if ( _portals[ i ].IsOverlapping( portalSet._portals[ j ], epsilon ) )
                return true;
        }
    }

    // no portals overlap, so return false.
    return false;
}*/

//----------------------------------------------------------
GrMeshInst*
GrPolygonBSPPortalSet::GenMesh( URef< GrMaterial > material )
{
    // return NULL if there are no polygons in the portal.
    if ( _portals.GetElemCount() == 0 )
        return 0;

    // create a new group and add our polygons to it.
    GrPolygonGroup group;
    unsigned int polygonCount = _portals.GetElemCount();
    unsigned int polyIdx = 0;
    for ( unsigned int i = 0; i < polygonCount; ++i )
    {
        GrPortal& curPortal = _portals[ i ];
        if ( !curPortal.IsDegenerate() )
        {
            // build the current polygon.
            GrPolygon polygon;
            const unsigned int polygonVertexCount = curPortal.GetVertexCount();
            SPolygonVertex* vertices = new SPolygonVertex[ polygonVertexCount ];
            for ( unsigned int j = 0; j < polygonVertexCount; ++j )
                vertices[ j ].position = curPortal.GetVertex( j );

            // set the polygon's vertices and add it to the group.
            polygon.SetVertices( 0, vertices, polygonVertexCount, material );
            group.AddPolygon( polygon );

            // delete the temporary array of vertices.
            delete vertices;

            // next polygon.
            ++polyIdx;
        }
    }

    // return null if there are no non-degenerate polygons.
    if ( group.GetPolygonCount() == 0 )
        return 0;

    // generate a tangent space basis so that any material can be applied to
    // it.
    group.GenPlanarUVMap( _plane );
    group.GenFaceNormals();
    group.GenTSBasis();

    // generate renderable meshes.
    std::vector< GrMeshInst* > meshes;
    group.GenMeshes( meshes, "Portal debug mesh", false, false );

    // there should only ever be one mesh in this case!
    B_ASSERT( meshes.size() == 1 );

    // return our mesh.
    return meshes[ 0 ];
}
/*
//----------------------------------------------------------
void
GrPolygonBSPPortalSet::Save( UWriter& writer )
{
    writer.WriteUInt( 'LTRP' );

    // save polygons.
    const unsigned int polyCount = _portals.GetElemCount();
    writer.WriteUInt( polyCount );
    for ( unsigned int i = 0; i < polyCount; ++i )
        _portals[ i ].Save( writer );
}

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::Load( UReader& reader )
{
    int tag = reader.ReadUInt();
    B_ASSERT( tag == 'LTRP' );

    // load all polygons.
    const unsigned int polyCount = reader.ReadUInt();
    for ( unsigned int i = 0; i < polyCount; ++i )
        _portals.Push( GrPortal( reader ) );
}
*/

//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonBSPPortalSet::CreatePlanePolygon( GrPortal::VertexArray& polygon, const MPlane& plane, float scale )
{
    const MVec3 kUp( 0.0f, 1.0f, 0.0f );
    const MVec3 kForward( 0.0f, 0.0f, 1.0f );

    // a point on our plane (happens to be the closest point to the origin).
    const MVec3& normal = plane.GetNormal();
    MVec3 point = -plane.GetD() * normal;

    // check to see if the plane's normal is facing up.
    bool facingUpOrDown = ( Abs( kUp.Dot( plane.GetNormal() ) ) > 0.9f );

    // calculate the vector to the plane's right.
    MVec3 right;
    if ( facingUpOrDown )
        right = kForward.Cross( plane.GetNormal() ).Normalized();
    else
        right = kUp.Cross( plane.GetNormal() ).Normalized();

    // calculate the vector to the plane's top.
    MVec3 up = right.Cross( plane.GetNormal() );

    // scale it up.
    right *= scale;
    up *= scale;

    // create the corners.
    MVec3 verts[ 4 ];
    polygon.Push( point + up - right );
    polygon.Push( point + up + right );
    polygon.Push( point - up + right );
    polygon.Push( point - up - right );
//  MVec3 topLeft = point + up - right;
//  MVec3 topRight = point + up + right;
//  MVec3 bottomRight = point - up + right;
//  MVec3 bottomLeft = point - up - right;

    // create a polygon from the corners.
//  polygon = GrPolygon( 0, verts, 4, 0 );

    // make sure our plane is correct!  If not, then we fucked up the math above.
    // Things to check would be the winding order and the cross products calculating
    // the corners.
#ifdef _DEBUG
    const unsigned int polygonVertexCount = polygon.GetElemCount();
    for ( unsigned int i = 0; i < polygonVertexCount; ++i )
    {
        float absDist = Abs( plane.Dist( polygon[ i ] ) );
//      B_ASSERT( plane.GetNormal().Dot( polygon.GetPlane().GetNormal() ) > 0.95f );
//      B_ASSERT( absDist < 1.0f );
    }
#endif
}
