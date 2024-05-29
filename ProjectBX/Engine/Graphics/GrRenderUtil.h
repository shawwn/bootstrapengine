//----------------------------------------------------------
// File:        GrRenderUtil.h
// Author:      Kevin Bray
// Created:     10-20-08
//
// Purpose:     To provide rendering utilities.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMeshVB;
class GrCamera;
class MVec3;
class MMat4x4;
class MAABox;
class GrFrustum;
class GrColor;
class GrShader;

//**********************************************************
// class GrRenderUtil
//**********************************************************
class GrRenderUtil
{
public:
    GrRenderUtil();
    ~GrRenderUtil();

    // drawing functions that submit positions and UVs.
    void                DrawScreenQuad( float minS=0.0f, float minT=0.0f, float maxS=1.0f, float maxT=1.0f );
    void                DrawQuad( float sx, float sy, float w, float h, float minS=0.0f, float minT=0.0f, float maxS=1.0f, float maxT=1.0f );

    // binds the basic color shader to the device and sets common
    // render states.
    void                SetupColoredRender( const GrCamera& camera, bool wireframe, bool additive );

    // drawing functions that submit a point, line, or triangle for rendering.
    void                DrawColoredPoint( const GrCamera& camera, const MVec3& pos, const GrColor& color, unsigned int sizeInPixels = 1 );
    void                DrawColoredLine( const MVec3& pos1, const MVec3& pos2, const GrColor& color );
    void                DrawColoredLine( const MVec3& pos1, const MVec3& pos2, const GrColor& color1, const GrColor& color2 );
    void                DrawColoredTriangle( const MVec3& pos1, const MVec3& pos2, const MVec3& pos3, const GrColor& color );
    void                DrawColoredTriangle( const MVec3& pos1, const MVec3& pos2, const MVec3& pos3, const GrColor& color1, const GrColor& color2, const GrColor& color3 );

    // Drawing functions that submit a shape for rendering.  Note that
    // cones open towards -Z.
    void                DrawColoredSphere( const MVec3& pos, float radius, const GrColor& color, unsigned int slices = 32 );
    void                DrawColoredCone( const MVec3& pos, float theta, float length, const GrColor& color, unsigned int slices = 32 );
    void                DrawColoredCube( const MVec3& pos, float length, const GrColor& color );
    void                DrawColoredAABox( const MAABox& aaBox, const GrColor& color );
    void                DrawColoredAABox( const MVec3& minExts, const MVec3& maxExts, const GrColor& color );
    void                DrawColoredOBox( const MMat4x4& transform, const MVec3& halfExtents, const GrColor& color );

    // draws a frustum.
    void                DrawColoredFrustum( const GrFrustum& frustum, const GrColor& color );

private:
    void                CreateResources();

    URef< GrMeshVB >    _quadVB;
    URef< GrMeshVB >    _colorVB;
    URef< GrShader >    _colorShader;
};
extern GrRenderUtil* gGrRenderUtil;