//----------------------------------------------------------
// File:        GrPolygonBSPArea.h
// Author:      Kevin Bray
// Created:     09-12-06
//
// Purpose:     To manage BSP leaf data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrPolygonGroup.h"
#include "MAABox.h"
#include "MSphere.h"

// std c++ includes.
#include <vector>
#include <set>

// forward declarations.
class GrPolygonBSPPortalSet;
class GrPolygonUberPatchGroup;
class GrPortal;
class GrMeshInst;
class GrFrustum;
class UReader;
class UWriter;
class GrPolygonBSPSplit;
class MRay;
class MVec3;
class GrTextureBase;

//**********************************************************
// class GrPolygonBSPArea
//**********************************************************
class GrPolygonBSPArea
{
public:
    typedef std::vector< GrMeshInst* > MeshInstVector;
    typedef UFastArray< GrPolygonBSPPortalSet* > PortalSetArray;
    typedef std::vector< GrPolygonBSPArea* > AreaVector;

    GrPolygonBSPArea( const GrPolygonGroup& polygons );
    GrPolygonBSPArea( UReader& reader );
    ~GrPolygonBSPArea();

    // returns a pointer to the list of visibility info.
    void                        GetVisibilityData( const std::vector< unsigned int >** pvsIndices ) const;

    // mark the area as visible.
    const MAABox&               GetBoundingAABox() const;
    MSphere                     GetBoundingSphere() const;

    // local polygons.
    GrPolygonGroup&             GetPolygons()                           {   return _polygons;           }
    const GrPolygonGroup&       GetPolygons() const                     {   return _polygons;           }

    // generate ubertexture data.
    void                        GenUberTexData( float texelsPerMeter );

    // adds a portal.
    void                        AddPortal( GrPolygonBSPPortalSet* portal );

    // checks if the leaf passed in is a neighbor, and if so, adds it to it's
    // neighbor list.
    void                        FindNeighbors( GrPolygonBSPSplit* root );

    // checks neighbors for any tjunctions and fixes them.
    void                        FixTJuncs();

    // builds a list of visible leaves.
    void                        CalcPVS( const std::vector< GrPolygonBSPArea* >& leaves );

    // sets PVS to all leaves.
    void                        CalcPVSAllVisibleIndices( unsigned int leafCount );

    // compresses PVS data to a list of indices into the array passed in.
    void                        CompressPVSData( const std::vector< GrPolygonBSPArea* >& leaves );

    // checks for an intersection with the ray passed in.
    bool                        GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist );

    // returns the list of portal sets.  Note that each portal set contains a
    // pointer to the neighbor the portals in it connect to.
    const PortalSetArray&       GetPortalSets() const                   {   return _portals;            }

    // returns the ubertexture data for the current area.
    void                        GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const;
    GrPolygonUberPatchGroup*    GetUberTexPatchGroup() const            {   return _uberData;           }

    // leaf load/save.
    void                        Load( UReader& reader );
    void                        Save( UWriter& writer );

private:
    typedef std::vector< unsigned int > UIntVector;

    bool                        SubtractAreaFromPortal( GrPolygonBSPPortalSet* portal, GrPolygonBSPArea* area );
    void                        FindPortalSetNeighbors( std::vector< GrPolygonBSPPortalSet* >& neighborLinks, GrPolygonBSPSplit* root, GrPolygonBSPPortalSet* curPortalSet );
    void                        FloodPVS( std::set< GrPolygonBSPArea* >& visibleLeaves, std::vector< MPlane >& cullPlanes, const GrPortal& fromPortal,
                                          const GrPolygonBSPSplit* portalCreator );
    bool                        CheckVisibility( GrPolygonBSPArea* area );
    bool                        FloodToArea( std::vector< MPlane >& cullPlanes, const GrPortal& fromPortal, GrPolygonBSPArea* toArea );
    bool                        IsPortalVisible( const GrPortal& curPortal, const std::vector< MPlane >& cullPlanes );
    void                        Clean();

    // group of polygons and a temporary renderable mesh.
    GrPolygonGroup              _polygons;

    // ubertexture data for the BSP.
    GrPolygonUberPatchGroup*    _uberData;

    // this is needed for rendering with PVS data.
    UIntVector                  _pvsIndices;

    // data only needed for PVS generation.
    PortalSetArray              _portals;
    AreaVector                  _pvsVector;
    bool                        _inFlood;

    // last frame ID we were visible.
    unsigned int                _visibleFrameID;
};
