//----------------------------------------------------------
// File:        GrRenderShadow.h
// Author:      Kevin Bray
// Created:     04-07-07
//
// Purpose:     To render a set of objects to a shadow.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrVirtualShadowPage.h"
#include "GrConfig.h"
#include "MMat4x4.h"

class GrStreamingIB;
class GrRenderList;
class GrTextureBase;
class GrRenderTarget;
class GrCamera;
class GrShader;
class GrMaterial;
class GrLight;
struct SVec2;

//**********************************************************
// class GrRenderShadow
//**********************************************************
class GrRenderShadow
{
public:
    GrRenderShadow( GrStreamingIB* streamingIB, unsigned int maxIndexBatchSize );
    ~GrRenderShadow();

    // this renders the shadow.
    URef< GrTextureBase >   RenderShadow( GrVirtualShadowPage::SRenderData& shadowData, const GrRenderList& shadowCasters,
                                          const GrCamera& shadowCamera, const GrLight& light, unsigned int time, EGRQUALITY quality );

    // returns the data needed for no shadow.
    const GrVirtualShadowPage::SRenderData&     GetNoShadowData() const         {   return _shadowPage.GetNoShadowData();   }

private:
    typedef std::vector< GrVirtualShadowPage::SShadowInfo > ShadowInfoVec;

    void                    RenderShadowCasters( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
                                                 unsigned int time, EGRQUALITY quality );
    void                    RenderShadowReceivers( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
                                                   unsigned int time, EGRQUALITY quality );


    void                    RenderShadowMap( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
                                             GrShader* depthShader, GrShader* alphaTestShader, unsigned int writeMask,
                                             unsigned int time, EGRQUALITY quality, bool honorCastShadow );
    void                    RenderShadowFaces( const GrRenderList& shadowCasters, const GrCamera& shadowCamera,
                                               GrShader* depthShader, GrShader* alphaTestShader, unsigned int writeMask,
                                               unsigned int time, EGRQUALITY quality, bool honorCastShadow );


    void                    RenderObjects( const GrRenderList& shadowCasters, unsigned int start, unsigned int end,
                                           const GrCamera& shadowCamera, GrShader* curShader, unsigned int time, bool alphaTest );
    void                    CalcShadowMidpoint( EGRQUALITY quality );

    void                    SetShadowPageViewport( unsigned int page );
    const GrMaterial*       GetShadowSpan( unsigned int& endObj, const GrRenderList& list, unsigned int startObj );
    bool                    IsCompatibleShadow( const GrMaterial* material1, const GrMaterial* material2 );
    unsigned int            BatchRender( const GrRenderList& renderableArray, unsigned int startIdx, unsigned int endIdx,
                                         unsigned int time, unsigned int vertexComponents );
    void                    SetActiveShadowMap( unsigned int shadowMapIdx );
    void                    CalcMapUVs( float x, float y, unsigned int idx );

    // the virtual shadow page used to render shadows.
    GrVirtualShadowPage     _shadowPage;

    // cache of shadow info from the last shadows generated.
    ShadowInfoVec           _virtualShadowCache;

    // streaming data.
    GrStreamingIB*          _streamingIB;
    unsigned int            _maxIndexBatchSize;

    // shadow map info.
    unsigned int            _shadowMapSize;
    unsigned int            _shadowPageSize;

    // render targets.
    URef< GrRenderTarget >  _shadowCasterPage;
    URef< GrRenderTarget >  _shadowReceiverPage;
    URef< GrRenderTarget >  _shadowMidpointPage;

    // textures.
    URef< GrTextureBase >   _defaultBump;

    // shaders.
    URef< GrShader >        _depth;
    URef< GrShader >        _depthPeel;
    URef< GrShader >        _depthPeelAlpha;
    URef< GrShader >        _mergeDepth;
};
