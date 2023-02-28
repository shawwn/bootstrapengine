//----------------------------------------------------------
// File:        GrSceneRender.h
// Author:      Kevin Bray
// Created:     11-27-05 ( happy bday to me! )
//
// Purpose:     To pass scene information to the renderer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrRenderList.h"

class GrModel;
class GrBSPTree;
class GrLight;
class GrCamera;
class GrFrustum;
class GrColor;
class MVec3;
class MMat4x4;
class MMat3x3;
class GrShadowBuffer;
class MPlane;
struct SVec3;
class GrTerrain;


//**********************************************************
// class GrSceneRender
//**********************************************************
class GrSceneRender
{
public:
    static const unsigned int kMaxLOD = 5U;

    GrSceneRender();
    ~GrSceneRender();

    void                RenderAmbience( const GrCamera* camera, const MVec3& pvsEyePos, const GrBSPTree* bspRoot, const GrModel* modelRoot,
                                        const GrModel* skyboxRoot, const GrColor& ambColor, unsigned int frameId );
    void                RenderLight( const GrCamera* camera, const GrLight* light, const GrBSPTree* bspRoot, const GrModel* modelRoot,
                                     unsigned int frameId );

private:
    GrShadowBuffer*     _shadowBuffers[ kMaxLOD ];  // shadow buffers at different LODs.
    GrRenderList        _lightReceivers;
    GrRenderList        _shadowCasters;

    bool                _shadowEnable;
};
extern GrSceneRender* gGrSceneRender;
