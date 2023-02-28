//----------------------------------------------------------
// File:        GrRenderer.h
// Author:      Kevin Bray
// Created:     01-04-04
//
// Purpose:     To render a set of objects.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>

// project includes.
#include "GrTypes.h"
#include "UFastArray.h"
#include "GrStreamingIB.h"
#include "GrRenderList.h"
#include "GrRenderAmbience.h"
#include "GrRenderLight.h"
//#include "GrRenderUberTexture.h"

class GrRendererImpl;
class GrMeshInst;
class GrCamera;
class GrLight;
class GrColor;
class GrRenderList;

//**********************************************************
// class GrRenderer
//**********************************************************
class GrRenderer
{
public:
    GrRenderer( unsigned int screenWidth, unsigned int screenHeight, EGRQUALITY initQuality );
    ~GrRenderer();

    // quality settings.
    void                    SetQuality( EGRQUALITY quality )        {   _quality = quality;             }
    EGRQUALITY              GetQuality() const                      {   return _quality;                }

    // debug functionality.
    void                    SetWireframeEnable( bool wireframe )    {   _wireframe = wireframe;         }
    bool                    GetWireframeEnable() const              {   return _wireframe;              }
    void                    SetColorTextureEnable( bool enable )    {   _colorTextureEnable = enable;   }
    bool                    GetColorTextureEnable() const           {   return _colorTextureEnable;     }

    // updates the renderer's time.
    void                    Update( unsigned int gameTimeDelta, unsigned int sysTimeDelta );

    // renders lighting data.
    void                    BeginRender();
    void                    RenderAmbience( const GrRenderList& objects, const GrCamera& camera, const GrColor& ambientColor );
    void                    RenderLight( const GrRenderList& lightReceivers, const GrRenderList& shadowCasters,
                                         const GrCamera& camera, const GrLight& light );
    void                    EndRender( const GrCamera& camera );

    void                    PreContextReset();
    void                    PostContextReset( unsigned int screenWidth, unsigned int screenHeight );

private:
    static const unsigned int kMaxShadowLOD = 4U;

    // private methods.
    void                    PreRender();
    void                    PostRender();

    const GrMaterial*       GetMaterialSpan( unsigned int& endObj, const GrRenderList& list, unsigned int startObj );

    GrRenderAmbience        _renderAmbience;
    GrRenderLight           _renderLight;

    // internal render lists.
    GrRenderList            _postRenderObjects;

    // index buffer used for batching.
    GrStreamingIB           _streamingIB;

    // system textures.
    URef< GrTextureCube >   _normalizeCube;

    // other data.
    unsigned int            _gameTime;
    unsigned int            _sysTime;

    // current rendering quality
    EGRQUALITY              _quality;

    // master enables.
    bool                    _wireframe;
    bool                    _colorTextureEnable;
};
extern GrRenderer* gGrRenderer;
