//----------------------------------------------------------
// File:        GrFramebuffer.h
// Author:      Kevin Bray
// Created:     06-18-05
//
// Purpose:     To manage the primary render target.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrRenderTargetMgr.h"
#include "MVec3.h"
#include "GrConfig.h"
#include "enums.h"

// forward declarations.
class GrTextureBase;
class GrShader;
class GrRenderTarget;
//enum EGRQUALITY;

//**********************************************************
// class GrFramebuffer
//**********************************************************
class GrFramebuffer
{
public:
    GrFramebuffer( unsigned int width, unsigned int height, EGRQUALITY quality, bool hdrEnable );
    ~GrFramebuffer();

    // HDR settings.  Note that the context must be reset in order for these
    // settings to take full effect.
    void                    SetHDRQuality( EGRQUALITY quality ) {   _quality = quality;         }
    void                    SetHDREnable( bool enabled )        {   _hdrEnable = enabled;       }

    // accessor to current settings.
    EGRQUALITY              GetHDRQuality() const               {   return _quality;            }
    bool                    GetHDREnable() const                {   return _hdrEnable;          }

    // information for format compatibility with the currentRender functionality.
    GrRenderTargetMgr::EFORMAT      GetColorFormat() const;
    GrRenderTargetMgr::EFORMAT      GetDepthFormat() const;
    unsigned int            GetCurRenderWidth() const;
    unsigned int            GetCurRenderHeight() const;

    // BeginFrame() means that all rendering goes to the frame buffer being
    // managed by this object ( essentially what is on the screen ).
    void                    BeginFrame();
    void                    EndFrame( unsigned int curTime );

    // returns the contents of the current render target in a texture up to the
    // Note that you should not keep the texture handle around as the
    // contents of the current render texture are updated only when CopyCurrentRender
    // or CopyCurrentDepth() is called.  Note that this function does *not* preserve
    // texture bindings.  If either of these are called when a render target whose
    // format does not match the framebuffer's format, the performance may be
    // horribly slow due to software fallback.
    // Note that these are controlled by the GrFramebuffer class because it's size
    // and format depends on the primary framebuffer.
    URef< GrTextureBase >   CopyCurrentRender( int srcX, int srcY, int width, int height );
    URef< GrTextureBase >   CopyCurrentDepth( int srcX, int srcY, int width, int height );

    // TODO!  Fix render target change!
    void                    PreContextReset();
    void                    PostContextReset( unsigned int width, unsigned int height );

private:
    typedef GrRenderTargetMgr::EFORMAT ERTFORMAT;

    void                    ToneMap( unsigned int delta );
    void                    ProcessRT( GrRenderTarget* destRT, GrShader* shader, GrRenderTarget* sourceRT, GLenum filter,
                                       unsigned int vpSizeShift = 0, unsigned int sizeShift = 0 );
    void                    BindRTRectTex( GLenum texUnit, GLenum filter, URef< GrRenderTarget > renderTarget );
    void                    SetTexCoordScale( GrShader* shader, unsigned int paramHandle, GrRenderTarget* sourceRT,
                                              unsigned int sizeShift = 0 );
    void                    AllocResources( bool allocShaders = true );
    URef< GrRenderTarget >  CreateHDRRenderTarget( unsigned int width, unsigned int height, bool depthBuffer );

    // "current render" functionality
    URef< GrTextureBase >   _currentRender;
    URef< GrTextureBase >   _currentDepth;
    MVec3                   _postProcessScale;

    // render targets.
    URef< GrRenderTarget >  _mainRT;
    URef< GrRenderTarget >  _hdrRT;
    URef< GrRenderTarget >  _hdrScratch;
    URef< GrRenderTarget >  _downSampleRT;
    URef< GrRenderTarget >  _downSampleRT2;
    URef< GrRenderTarget >  _squareRT;
    URef< GrRenderTarget >  _halfSquareRT;

    // shaders.
    URef< GrShader >        _hdrPassThruShader;
    URef< GrShader >        _hdrLogAvgShader;
    URef< GrShader >        _hdrToneMapShader;
    URef< GrShader >        _hdrBrightPassShader;
    URef< GrShader >        _hdrBlurShader;
    URef< GrShader >        _hdrCombineShader;

    // width/height of the render targets.
    unsigned int            _width;
    unsigned int            _height;

    // time info.
    unsigned int            _lastTime;

    // miscellaneous settings.
    EGRQUALITY              _quality;
    ERTFORMAT               _hdrFormat;
    bool                    _hdrEnable;
};
extern GrFramebuffer* gGrFramebuffer;
