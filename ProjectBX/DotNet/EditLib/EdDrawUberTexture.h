//----------------------------------------------------------
// File:        EdDrawUberTexture.h
// Author:      Kevin Bray
// Created:     07-05-08
//
// Purpose:     To render a region of an ubertexture to the currently
//              bound render target.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrRenderList.h"
#include "GrRenderAmbience.h"
//#include "GrRenderUberTexture.h"
#include "GrStreamingIB.h"

// forward declarations.
class GrRenderTarget;
class GrUberTexture;
class GrMeshInst;
class GrMesh;
class GrMaterial;

#if 0
//**********************************************************
// class EdDrawUberTexture
//**********************************************************
class EdDrawUberTexture
{
public:
    EdDrawUberTexture();
    ~EdDrawUberTexture();

    // set the target window.
    bool                    SetTarget( HWND window );

    // renders an ubertexture from the coordinates specified.
    void                    Render( GrUberTexture* uberTexture, float startU, float startV, float endU, float endV );

private:
    void                    UpdateTargetSize();

    // rendering services.
    GrRenderAmbience        _renderAmbience;
//  GrRenderUberTexture     _renderUberTexture;
    GrStreamingIB           _streamingIB;

    // rendering resources.
    GrRenderList            _renderList;
    GrRenderList            _postRenderList;
    GrMeshInst*             _meshInst;
    URef< GrMaterial >      _uberMaterial;
    URef< GrMesh >          _mesh;

    // temporary buffer.
    URef< GrRenderTarget >  _tempTarget;

    // target window information.
    HWND                    _target;
    HDC                     _targetDC;
    HGLRC                   _targetRC;
    unsigned int            _targetWidth;
    unsigned int            _targetHeight;

    // primary window information.
    HDC                     _primaryDC;
    HGLRC                   _primaryRC;
};
extern EdDrawUberTexture* gEdDrawUberTexture;
#endif