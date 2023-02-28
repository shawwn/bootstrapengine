//----------------------------------------------------------
// File:        GrBSPTree.cpp
// Author:      Kevin Bray
// Created:     11-06-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrBSPTree.h"

// project includes.
#include "GrLightMgr.h"
#include "GrBSPSplit.h"
#include "GrBSPArea.h"
#include "GrMeshInst.h"
#include "GrPolygonBSPTree.h"
#include "GrMeshCombiner.h"
#include "GrFrustum.h"
#include "GrScene.h"
#include "GrLight.h"
#include "GrRenderList.h"
#include "MSphere.h"
#include "UReader.h"
#include "UWriter.h"
#include "GrCamera.h"

static bool IsEqual( const MVec3& left, const MVec3& right )
{
    return ( left.GetX() == right.GetX() && left.GetY() == right.GetY() && left.GetZ() == right.GetZ() );
}

//**********************************************************
// class GrBSPTree
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrBSPTree::GrBSPTree()
: _root( 0 )
, _inScene( false )
{

}

//----------------------------------------------------------
GrBSPTree::GrBSPTree( const UPath& fileName, UReader& reader )
: _fileName( fileName )
, _root( 0 )
, _inScene( false )
{
    Load( reader );
}

//----------------------------------------------------------
GrBSPTree::GrBSPTree( const GrPolygonBSPTree& polygonBSPTree, const GrPolygonUberUVMapper& mapper )
: _root( 0 )
, _inScene( false )
{
    // build the BSP tree from the polygon BSP tree.
    std::vector< GrMeshCombiner* > combinerVector;
    _root = new GrBSPSplit( combinerVector, *polygonBSPTree.GetRoot(), mapper );
    _root->GetAreas( _areas );

    // go through combined meshes and build renderable meshes.
    char buf[ 20 ];
    unsigned int i = 0;
    std::vector< GrMeshCombiner* >::iterator iter = combinerVector.begin();
    const std::vector< GrMeshCombiner* >::iterator end = combinerVector.end();
    for ( ; iter != end; ++iter, ++i )
    {
        // determine a good name for the current mesh.
        _itoa_s( i, buf, 20, 10 );

        // build a renderable mesh instance from the current combiner.
        GrMeshCombiner* curCombiner = ( *iter );
        GrMeshInst* meshInst = curCombiner->BuildFinalMesh( buf );
        _meshes.push_back( meshInst );

        // delete the current mesh combiner.
        delete ( *iter );
    }
}

//----------------------------------------------------------
GrBSPTree::~GrBSPTree()
{
    Clean();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrBSPTree::SetInScene( bool inScene )
{
    // if our status isn't changing, then simply return.
    if ( _inScene == inScene )
        return;

    void* test = malloc( 200 );
    tstring* str = new( test ) tstring;

    // set our _inScene flag.
    _inScene = inScene;

    // add or remove lights.
    if ( inScene )
        AddLightsToScene();
    else
        RemoveLightsFromScene();
}

//----------------------------------------------------------
void
GrBSPTree::AddLight( URef< GrLight > light )
{
    _lights.AddLight( light );
    if ( _inScene )
        gGrScene->AddLight( light );
}

//----------------------------------------------------------
URef< GrLight >
GrBSPTree::FindLight( const char* name, int cmpLen ) const
{
    return _lights.FindLight( name, cmpLen );
}

//----------------------------------------------------------
bool
GrBSPTree::RemoveLight( const char* name )
{
    bool found = _lights.RemoveLight( name );
    if ( found && _inScene )
        gGrScene->RemoveLight( name );
    return found;
}

//----------------------------------------------------------
bool
GrBSPTree::RemoveLight( GrLight* light )
{
    bool found = _lights.RemoveLight( light );
    if ( found && _inScene )
        gGrScene->RemoveLight( light );
    return found;
}

//----------------------------------------------------------
bool
GrBSPTree::Pick( GrMeshInst*& meshInst, GrMesh::STriCollision& hit, const MRay& ray )
{
    // is there anything to hit?
    if ( !_root )
        return false;

    // pick.
    return _root->Pick( meshInst, hit, _meshes, ray );
}

//----------------------------------------------------------
void
GrBSPTree::Traverse( GrSceneTraverser* traverser )
{
    // traverse the tree.
    if ( _root )
        _root->Traverse( traverser, _meshes );
}

//----------------------------------------------------------
void
GrBSPTree::Cache()
{
    unsigned int meshCount = ( unsigned int )_meshes.size();
    for ( unsigned int i = 0; i < meshCount; ++i )
        _meshes[ i ]->Cache();

}

//----------------------------------------------------------
void
GrBSPTree::Evict()
{
    unsigned int meshCount = ( unsigned int )_meshes.size();
    for ( unsigned int i = 0; i < meshCount; ++i )
        _meshes[ i ]->Evict();
}

//----------------------------------------------------------
void
GrBSPTree::GetLitObjectsAndMarkVisible( GrRenderList& lightReceivers, const GrFrustum& frustum, const MVec3& sceneEyePos,
                                        unsigned int frameId ) const
{
    // if there isn't anything in the scene, then there is nothing to draw!
    if ( _root == 0 )
        return;

    // get the area the position lies in.
    GrBSPArea *area = _root->GetArea( sceneEyePos );
    if ( !area )
        return;

    // get the list of visible areas from the BSP area we're in.
    const GrBSPArea::IndexArray& visibleAreas = area->GetVisibleAreas();

    // only add areas that are marked as visible.
    GrBSPArea::IndexArray::const_iterator iter = visibleAreas.begin();
    GrBSPArea::IndexArray::const_iterator end = visibleAreas.end();
    for ( ; iter != end; ++iter )
    {
        // get the current area.
        GrBSPArea* curArea = _areas[ *iter ];
        if ( frustum.IsAABoxInside( curArea->GetBoundingBox() ) )
        {
            // mark the current area as visible.
            curArea->SetLastVisibleFrameID( frameId );

            // add the ubertexture data to the render list.
            GrUberPatchSet* curAreaPatchSet = curArea->GetUberPatchSet();
            if ( curAreaPatchSet )
                lightReceivers.AddUberPatchSet( curAreaPatchSet );

            // check mesh instances for visibility.
            unsigned int meshInstCount = curArea->GetMeshInstCount();
            for ( unsigned int i = 0; i < meshInstCount; ++i )
            {
                // get the current mesh instance information.
                const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                // go through all of the ranges in the current area.
                unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                {
                    // check visibility.
                    GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];
                    const MAABox& curBox = curMeshInst->GetMesh()->GetRangeAABox( j );
                    if ( frustum.IsAABoxInside( curBox ) )
                    {
                        // mark as visible and send to the renderer.
                        curArea->SetMeshInstLastVisibleFrameID( i, frameId );
                        lightReceivers.AddMeshInstRange( curMeshInst, j, true );
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------
bool
GrBSPTree::GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const MSphere& region,
                          unsigned int frameId ) const
{
    // if there isn't anything in the scene, then there is nothing to draw!
    if ( _root == 0 )
        return false;

    // get the area the position lies in.
    GrBSPArea *area = _root->GetArea( region.GetCenter() );
    if ( !area )
        return false;

    // get the list of visible areas from the BSP area we're in.
    const GrBSPArea::IndexArray& visibleAreas = area->GetVisibleAreas();

    // only add areas that are marked as visible.
    GrBSPArea::IndexArray::const_iterator iter = visibleAreas.begin();
    GrBSPArea::IndexArray::const_iterator end = visibleAreas.end();

    // do a quick check for visibility.
    for ( ; iter != end; ++iter )
        if ( _areas[ *iter ]->GetLastVisibleFrameID() == frameId )
            break;

    // if nothing lit is visible, return false.
    if ( iter == end )
        return false;

    iter = visibleAreas.begin();
    if ( shadowCasters )
    {
        // go through all areas checking visibility with the user camera
        // and lights volume (for shadow cast purposes).
        for ( ; iter != end; ++iter )
        {
            // get the current area.
            GrBSPArea* curArea = _areas[ *iter ];

            // check to see if the area is touched by the light.
            if ( curArea->GetBoundingBox().Intersect( region ) )
            {
                // if the area is in the light's influence, we need to check
                // each mesh instance.
                unsigned int meshInstCount = curArea->GetMeshInstCount();
                for ( unsigned int i = 0; i < meshInstCount; ++i )
                {
                    // get the current mesh instance information.
                    const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                    // go through all of the ranges in the current area.
                    unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                    for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                    {
                        // get the mesh instance.
                        GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];

                        // check to see if we're within the light's radius.
                        const MAABox& curBounds = curMeshInst->GetMesh()->GetRangeAABox( j );
                        if ( curBounds.Intersect( region ) )
                        {
                            // mark as visible and send to the renderer.
                            if ( curMeshInstInfo.lastVisibleFrameId == frameId )
                                lightReceivers.AddMeshInstRange( curMeshInst, j, true );

                            // send the shadow caster to the renderer.
                            shadowCasters->AddMeshInstRange( curMeshInst, j, true );
                        }
                    }
                }
            }
        }
    }
    else
    {
        for ( ; iter != end; ++iter )
        {
            // get the current area.
            GrBSPArea* curArea = _areas[ *iter ];

            // check to see if the area is touched by the light.
            if ( curArea->GetLastVisibleFrameID() == frameId && curArea->GetBoundingBox().Intersect( region ) )
            {
                // if the area is in the light's influence, we need to check
                // each mesh instance.
                unsigned int meshInstCount = curArea->GetMeshInstCount();
                for ( unsigned int i = 0; i < meshInstCount; ++i )
                {
                    // get the current mesh instance information.
                    const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                    // go through all of the ranges in the current area.
                    unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                    for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                    {
                        // get the mesh instance.
                        GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];

                        // check to see if we're within the light's radius.
                        const MAABox& curBounds = curMeshInst->GetMesh()->GetRangeAABox( j );
                        if ( curMeshInstInfo.lastVisibleFrameId == frameId && curBounds.Intersect( region ) )
                            lightReceivers.AddMeshInstRange( curMeshInst, j, true );
                    }
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------
bool
GrBSPTree::GetLitObjects( GrRenderList& lightReceivers, GrRenderList* shadowCasters, const GrFrustum& region,
                          unsigned int frameId ) const
{
    // if there isn't anything in the scene, then there is nothing to draw!
    if ( _root == 0 )
        return false;

    // get the area the position lies in.
    GrBSPArea *area = _root->GetArea( region.GetPos() );
    if ( !area )
        return false;

    // get the list of visible areas from the BSP area we're in.
    const GrBSPArea::IndexArray& visibleAreas = area->GetVisibleAreas();

    // only add areas that are marked as visible.
    GrBSPArea::IndexArray::const_iterator iter = visibleAreas.begin();
    GrBSPArea::IndexArray::const_iterator end = visibleAreas.end();

    // do a quick check for visibility.
    for ( ; iter != end; ++iter )
        if ( _areas[ *iter ]->GetLastVisibleFrameID() == frameId )
            break;

    // if nothing lit is visible, return false.
    if ( iter == end )
        return false;

    iter = visibleAreas.begin();
    if ( shadowCasters )
    {
        // go through all areas checking visibility with the user camera
        // and lights volume (for shadow cast purposes).
        for ( ; iter != end; ++iter )
        {
            // get the current area.
            GrBSPArea* curArea = _areas[ *iter ];

            // check to see if the area is touched by the light.
            if ( region.IsAABoxInside( curArea->GetBoundingBox() ) )
            {
                // if the area is in the light's influence, we need to check
                // each mesh instance.
                unsigned int meshInstCount = curArea->GetMeshInstCount();
                for ( unsigned int i = 0; i < meshInstCount; ++i )
                {
                    // get the current mesh instance information.
                    const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                    // go through all of the ranges in the current area.
                    unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                    for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                    {
                        // get the mesh instance.
                        GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];

                        // check to see if we're within the light's radius.
                        const MAABox& curBounds = curMeshInst->GetMesh()->GetRangeAABox( j );
                        if ( region.IsAABoxInside( curBounds ) )
                        {
                            // mark as visible and send to the renderer.
                            if ( curMeshInstInfo.lastVisibleFrameId == frameId )
                                lightReceivers.AddMeshInstRange( curMeshInst, j, true );

                            // send the shadow caster to the renderer.
                            shadowCasters->AddMeshInstRange( curMeshInst, j, true );
                        }
                    }
                }
            }
        }
    }
    else
    {
        for ( ; iter != end; ++iter )
        {
            // get the current area.
            GrBSPArea* curArea = _areas[ *iter ];

            // check to see if the area is touched by the light.
            if ( curArea->GetLastVisibleFrameID() == frameId && region.IsAABoxInside( curArea->GetBoundingBox() ) )
            {
                // if the area is in the light's influence, we need to check
                // each mesh instance.
                unsigned int meshInstCount = curArea->GetMeshInstCount();
                for ( unsigned int i = 0; i < meshInstCount; ++i )
                {
                    // get the current mesh instance information.
                    const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                    // go through all of the ranges in the current area.
                    unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                    for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                    {
                        // get the mesh instance.
                        GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];

                        // check to see if we're within the light's radius.
                        const MAABox& curBounds = curMeshInst->GetMesh()->GetRangeAABox( j );
                        if ( curMeshInstInfo.lastVisibleFrameId == frameId && region.IsAABoxInside( curBounds ) )
                            lightReceivers.AddMeshInstRange( curMeshInst, j, true );
                    }
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------
bool                    
GrBSPTree::GetObjectsInFrustum( GrRenderList& objects, const GrFrustum& region ) const
{
    // if there isn't anything in the scene, then there is nothing to get!
    if ( _root == 0 )
        return false;

    // get the area the position lies in.
    GrBSPArea *area = _root->GetArea( region.GetPos() );
    if ( !area )
        return false;

    // get the list of visible areas from the BSP area we're in.
    const GrBSPArea::IndexArray& visibleAreas = area->GetVisibleAreas();

    // iterate over all areas.
    GrBSPArea::IndexArray::const_iterator iter = visibleAreas.begin();
    GrBSPArea::IndexArray::const_iterator end = visibleAreas.end();
    for ( ; iter != end; ++iter )
    {
        // get the current area.
        GrBSPArea* curArea = _areas[ *iter ];

        // check to see if the area is touched by the light.
        if ( region.IsAABoxInside( curArea->GetBoundingBox() ) )
        {
            // if the area is in the light's influence, we need to check
            // each mesh instance.
            unsigned int meshInstCount = curArea->GetMeshInstCount();
            for ( unsigned int i = 0; i < meshInstCount; ++i )
            {
                // get the current mesh instance information.
                const GrBSPArea::SMeshInstInfo& curMeshInstInfo = curArea->GetMeshInst( i );

                // go through all of the ranges in the current area.
                unsigned int rangeEnd = curMeshInstInfo.rangeStart + curMeshInstInfo.rangeCount;
                for ( unsigned int j = curMeshInstInfo.rangeStart; j < rangeEnd; ++j )
                {
                    // get the mesh instance.
                    GrMeshInst* curMeshInst = _meshes[ curMeshInstInfo.meshInstIdx ];

                    // check to see if we're within the light's radius.
                    const MAABox& curBounds = curMeshInst->GetMesh()->GetRangeAABox( j );
                    if ( region.IsAABoxInside( curBounds ) )
                        objects.AddMeshInstRange( curMeshInst, j, true );
                }
            }
        }
    }

    return true;
}


/*
//----------------------------------------------------------
void
GrBSPTree::AddToRendererFiltered( const MSphere& sphere ) const
{
    // get the area the position lies in.
    GrBSPArea *area = _root->GetArea( sphere.GetCenter() );
    if ( !area )
        return;

    // get the list of visible areas from the BSP area we're in.
    const GrBSPArea::IndexArray& visibleAreas = area->GetVisibleAreas();

    // only add areas that are marked as visible.
    GrBSPArea::IndexArray::const_iterator iter = visibleAreas.begin();
    GrBSPArea::IndexArray::const_iterator end = visibleAreas.end();
    for ( ; iter != end; ++iter )
    {
        // check for visibility as well as intersection with the
        // sphere passed in.
        GrBSPArea* curArea = _areas[ *iter ];
        if ( curArea->GetVisibleFrameID() == _curFrameId &&
             sphere.Intersect( curArea->GetBoundingSphere() ) )
        {
            // send mesh instances in the area to the renderer.
            curArea->AddToRenderer();
        }
    }
}
*/
//----------------------------------------------------------
void
GrBSPTree::Load( UReader& reader )
{
    Clean();

    // read in the meshes.
    unsigned int meshCount = reader.ReadUInt();
    _meshes.resize( meshCount );
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        // read in the mesh and the mesh instance.
        URef< GrMesh > mesh = new GrMesh( reader );
        _meshes[ i ] = new GrMeshInst( reader, mesh );
    }

    // read in the light chunk.
    unsigned int lightCount = reader.ReadUInt();
    tstring errors;
    for ( unsigned int i = 0; i < lightCount; ++i )
    {
        tstring lightName = reader.ReadString();
        GrLight* light = gGrLightMgr->GetLight( lightName, errors );
        _lights.AddLight( light );
    }

    // read the BSP tree and get all areas.
    _root = new GrBSPSplit( reader );
    _root->GetAreas( _areas );
}

//----------------------------------------------------------
void
GrBSPTree::Save( UWriter& writer )
{
    // save out the meshes.
    unsigned int meshCount = ( unsigned int )_meshes.size();
    writer.WriteUInt( meshCount );
    for ( unsigned int i = 0; i < meshCount; ++i )
    {
        // these instances don't share any meshes, so we can just save the
        // mesh, then the instance data.
        _meshes[ i ]->GetMesh()->Save( writer );
        _meshes[ i ]->Save( writer );
    }

    // save out the light chunk.
    unsigned int lightCount = _lights.GetLightCount();
    writer.WriteUInt( lightCount );
    for ( unsigned int i = 0; i < lightCount; ++i )
        writer.WriteString( _lights.GetLight( i )->GetId().GetPathString() );

    // save out the BSP tree.
    _root->Save( writer );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrBSPTree::AddLightsToScene()
{
    unsigned int lightCount = _lights.GetLightCount();
    for ( unsigned int i = 0; i < lightCount; ++i )
        gGrScene->AddLight( _lights.GetLight( i ) );
}

//----------------------------------------------------------
void
GrBSPTree::RemoveLightsFromScene()
{
    unsigned int lightCount = _lights.GetLightCount();
    for ( unsigned int i = 0; i < lightCount; ++i )
        gGrScene->RemoveLight( _lights.GetLight( i ) );
}

//----------------------------------------------------------
void
GrBSPTree::Clean()
{
    // free the node hierarchy.
    delete _root;
    _root = 0;

    // clear out all lights.
    _lights.Clear();

    // clear all areas.
    _areas.clear();

    // free mesh instances.
    MeshInstVec::iterator iter = _meshes.begin();
    const MeshInstVec::iterator end = _meshes.end();
    for ( ; iter != end; ++iter )
        delete ( *iter );

    // clear the mesh instance vector.
    _meshes.clear();
}
