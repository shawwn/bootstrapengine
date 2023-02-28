//----------------------------------------------------------
// File:        GrSubsys.h
// Author:      Kevin Bray
// Created:     12-28-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSubsys.h"

// project includes.
#include "GLSubsys.h"
#include "GrSoftwareTexture.h"
#include "GrSoftwareTextureMgr.h"
#include "GrTextureMgr.h"
#include "GrMesh.h"
//#include "GrUberCache.h"
#include "GrMaterialMgr.h"
#include "GrFramebuffer.h"
#include "GrScene.h"
#include "GrTexGen.h"
#include "GrRenderTargetMgr.h"
#include "GrUtil.h"
#include "GrRenderTarget.h"
#include "GrShaderMgr.h"
#include "GrSceneRender.h"
//#include "UIFontMgr.h"
#include "GrLightMgr.h"
#include "GrKFAnimMgr.h"
#include "GrMeshBufferMgr.h"
#include "GrRenderer.h"
#include "GrRenderUtil.h"
#include "RFileMgr.h"
#include "RRsrcMgr.h"
#include "GrModelMgr.h"
#include "GrUTLoader.h"
#include "GrUberDetailCache.h"
#include "GrUberTextureMgr.h"
#include "GrPixelBufferMgr.h"
#include "GrDebug.h"

GrSubsys *gGrSubsys = 0;

// defined in GrDebugSwitches.cpp
extern void GrRegisterDebugSwitches();

//**********************************************************
// class GrSubsys
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSubsys::GrSubsys( unsigned int screenWidth, unsigned int screenHeight, bool uberDXTCompress, bool uberOverlaySupport )
: _screenWidth( screenWidth )
, _screenHeight( screenHeight )
, _gamma( 1.0f )
, _brightness( 1.0f )
, _textureQuality( EGQ_HIGH /*EGQ_ULTRA*/ )
, _shadingQuality( EGQ_HIGH /*EGQ_ULTRA*/ )
, _hdrQuality( EGQ_ULTRA )
, _parallaxEnable( true )
, _hdrEnable( true )
, _colorTableDirty( false )
, _uberDXTCompression( uberDXTCompress )
, _uberOverlaySupport( uberOverlaySupport )
{
    // the file and resource manager must be initialized before the
    // graphics subsystem is started!
    B_ASSERT( gRFileMgr != 0 );
    B_ASSERT( gRRsrcMgr != 0 );
    B_ASSERT( uberDXTCompress != uberOverlaySupport );

    // make sure we haven't already been created.
    B_ASSERT( gGrSubsys == 0 );
    gGrSubsys = this;

    // initialize our render state filtering library.
    GrStateStart( screenWidth, screenHeight );

    // create singletons.
    new GrRenderTargetMgr( screenWidth, screenHeight );
    new GrShaderMgr;
    new GrFramebuffer( screenWidth, screenHeight, _hdrQuality, _hdrEnable );
    new GrPixelBufferMgr;
    new GrMeshBufferMgr;
    new GrSoftwareTextureMgr;
    new GrTextureMgr( _textureQuality );
    new GrMaterialMgr;
    new GrUTLoader();
    new GrUberDetailCache();
    new GrUberTextureMgr();
    new GrRenderer( screenWidth, screenHeight, _shadingQuality );
    new GrRenderUtil;
    new GrTexGen( 0 );
    new GrModelMgr;
    new GrScene;
    new GrSceneRender;
//  new UIFontMgr;
    new GrLightMgr;
    new GrKFAnimMgr;

    CreateFrameBuffer();

    UpdateColorTable();

    // register all console commands.
    GrRegisterDebugSwitches();
}

//----------------------------------------------------------
GrSubsys::~GrSubsys()
{
    // clear the singleton pointer.
    gGrSubsys = 0;

    delete gGrKFAnimMgr;
//  delete gUIFontMgr;
    delete gGrSceneRender;
    delete gGrScene;
    delete gGrModelMgr;
    delete gGrTexGen;
    delete gGrRenderUtil;
    delete gGrRenderer;
    delete gGrMaterialMgr;
    delete gGrUberDetailCache;
    delete gGrUberTextureMgr;
    delete gGrUTLoader;
    delete gGrTextureMgr;
    delete gGrSoftwareTextureMgr;
    delete gGrFramebuffer;
    delete gGrShaderMgr;
    delete gGrLightMgr;
    delete gGrMeshBufferMgr;
    delete gGrPixelBufferMgr;
    delete gGrRenderTargetMgr;

    GrStateStop();
}


//==========================================================
// public methods
//==========================================================
/*
//----------------------------------------------------------
void
GrSubsys::ResolutionChanged( unsigned int width, unsigned int height )
{
    _screenWidth = width;
    _screenHeight = height;
    GrRenderTarget::ResizePrimaryRT( width, height );
    gGrFramebuffer->Resize( width, height );
}

//----------------------------------------------------------
void
GrSubsys::SetTexturingQuality( EGRQUALITY quality )
{
    
}

//----------------------------------------------------------
void
GrSubsys::SetShadingQuality( EGRQUALITY quality )
{

}

//----------------------------------------------------------
void
GrSubsys::SetShadowQuality( EGRQUALITY quality )
{
    B_ASSERT( false );
}

//----------------------------------------------------------
void
GrSubsys::SetHDREnable( bool enabled )
{
    // turn on/off HDR.
//  gGrFramebuffer->SetHDREnable( enabled );
}
*/

//----------------------------------------------------------
void
GrSubsys::SetGamma( float gamma )
{
    gamma = Clamp( gamma, 0.5f, 3.0f );
    if ( _gamma != gamma )
    {
        _gamma = gamma;
        _colorTableDirty = true;
    }
}

//----------------------------------------------------------
void
GrSubsys::SetBrightness( float brightness )
{
    brightness = Clamp( brightness, 0.75f, 1.5f );
    if ( _brightness != brightness )
    {
        _brightness = brightness;
        _colorTableDirty = true;
    }
}

//----------------------------------------------------------
void
GrSubsys::TakeScreenshot( const char* fileName )
{
    // get the viewport and read from the front buffer.
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    GrGetViewport( x, y, width, height );
    GrReadBuffer( GR_BACK );

    // read pixels from the front buffer.
    char* pixels = new char[ width * height * 4 ];
    bglFinish();
    bglReadPixels( x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels );

    // now gamma correct the pixels.
    char* cur = pixels;
    for ( int y = 0; y < height; ++y )
        for ( int x = 0; x < width; ++x )
            for ( int c = 0; c < 4; ++c, ++cur )
                *cur = _colorTable[ *cur & 0xFF ] & 0xFF;

    // now save us a targa.
    GrSaveTGA( fileName, pixels, width, height, 4 );

    // clean up.
    delete[] pixels;
}

//----------------------------------------------------------
void
GrSubsys::Update( unsigned int timeDelta, unsigned int sysTimeDelta )
{
    if ( _colorTableDirty )
        UpdateColorTable();

    // update materials.
    if ( timeDelta > 0 )
    {
        gGrModelMgr->Update( timeDelta );
        gGrMaterialMgr->Update();
    }
    gGrScene->Update( timeDelta, sysTimeDelta );
}

//----------------------------------------------------------
void
GrSubsys::Render( const GrCamera* camera )
{
    gGrScene->Render( camera );
}

//----------------------------------------------------------
void
GrSubsys::Resize( unsigned int width, unsigned int height )
{
    // inform the subsystems of a pending reset.
    gGrRenderer->PreContextReset();
    gGrMeshBufferMgr->PreContextReset();
    gGrMaterialMgr->PreContextReset();
    gGrFramebuffer->PreContextReset();      // must occur before the render target mgr reset.
    gGrRenderTargetMgr->PreContextReset();
    gGrPixelBufferMgr->PreContextReset();

    // shut down renderstate management.
    GrStateStop();

    // initialize our renderer.
    GrStateStart( _screenWidth, _screenHeight );

    // apply quality settings.
    gGrTextureMgr->SetTexturingQuality( _textureQuality );
    gGrMaterialMgr->SetShadingQuality( _shadingQuality );
    gGrFramebuffer->SetHDRQuality( _hdrQuality );
    gGrFramebuffer->SetHDREnable( _hdrEnable );
    gGrRenderer->SetQuality( _shadingQuality );

    _screenWidth = width;
    _screenHeight = height;
    GrRenderTarget::ResizePrimaryRT( width, height );

    // inform the subsystems that the reset has been completed.
    gGrPixelBufferMgr->PostContextReset();
    gGrRenderTargetMgr->PostContextReset();
    gGrFramebuffer->PostContextReset( width, height );
    gGrMaterialMgr->PostContextReset();
    gGrMeshBufferMgr->PostContextReset();
    gGrRenderer->PostContextReset( width, height );

    // make sure the correct type of framebuffer is created.
    CreateFrameBuffer();
}

//----------------------------------------------------------
void
GrSubsys::Reset( unsigned int width, unsigned int height )
{
    PreContextReset();
    PostContextReset( width, height );
}

//----------------------------------------------------------
void
GrSubsys::Reset()
{
    PreContextReset();
    PostContextReset( GetScreenWidth(), GetScreenHeight() );
}

//----------------------------------------------------------
void
GrSubsys::PreContextReset()
{
    // make sure a render target isn't bound!
    B_ASSERT( GrRenderTarget::GetCurrentRT() == gGrRenderTargetMgr->GetPrimaryRenderTarget() );

    // note that DXT compression and overlays are not supported
    // simultaneously!
    B_ASSERT( _uberDXTCompression != _uberOverlaySupport );

    // inform the subsystems of a pending reset.
    gGrRenderer->PreContextReset();
    gGrMeshBufferMgr->PreContextReset();
    gGrMaterialMgr->PreContextReset();
    gGrTextureMgr->PreContextReset();
    gGrFramebuffer->PreContextReset();      // must occur before the render target mgr reset.
    gGrRenderTargetMgr->PreContextReset();
    gGrShaderMgr->PreContextReset();
    gGrPixelBufferMgr->PreContextReset();

    // shut down renderstate management.
    GrStateStop();
}

//----------------------------------------------------------
void
GrSubsys::PostContextReset( unsigned int width, unsigned int height )
{
    // initialize our renderer.
    GrStateStart( _screenWidth, _screenHeight );

    // apply quality settings.
    gGrTextureMgr->SetTexturingQuality( _textureQuality );
    gGrMaterialMgr->SetShadingQuality( _shadingQuality );
    gGrFramebuffer->SetHDRQuality( _hdrQuality );
    gGrFramebuffer->SetHDREnable( _hdrEnable );
    gGrRenderer->SetQuality( _shadingQuality );

    _screenWidth = width;
    _screenHeight = height;
    GrRenderTarget::ResizePrimaryRT( width, height );

    // inform the subsystems that the reset has been completed.
    gGrPixelBufferMgr->PostContextReset();
    gGrRenderTargetMgr->PostContextReset();
    gGrFramebuffer->PostContextReset( width, height );
    gGrTextureMgr->PostContextReset();
    gGrShaderMgr->PostContextReset();
    gGrMaterialMgr->PostContextReset();
    gGrMeshBufferMgr->PostContextReset();
    gGrRenderer->PostContextReset( width, height );

    // make sure the correct type of framebuffer is created.
    CreateFrameBuffer();
}

//----------------------------------------------------------
bool
GrSubsys::IssueCommand( const tstring& command )
{
    // find the first white-space character, if any.
    tstring::size_type spacePos = command.find_first_of( ' ' );
    tstring::size_type tabPos = command.find_first_of( '\t' );
    unsigned int wsPos = Min( ( unsigned int )spacePos, ( unsigned int )tabPos );

    // get the name of the command that we need to execute.
    tstring commandName = command.substr( 0, wsPos );

    // try to find the command in our database.
    CommandMap::iterator iter = _commands.find( commandName );
    if ( iter == _commands.end() )
        return false;

    // invoke the handler.
    if ( !iter->second( command ) )
        PrintF( "Invalid command syntax.\n" );

    // search for a valid command.
    return true;
}

//----------------------------------------------------------
void
GrSubsys::AddCommandHandler( const char* command, bool ( *handler )( const tstring& ) )
{
    // add the command.
    _commands[ command ] = handler;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrSubsys::UpdateColorTable()
{
    // build a gamma table.
    unsigned short gammaTable[ 256 ];

    // build the gamma table for each color component.
    int curColor = 0;
    float invGamma = 1.0f / _gamma;
    for ( unsigned int j = 0; j < 256; ++j )
    {
        float color = j / 255.0f;

        // apply the brightness factor and calculate gamma.
        color = color * _brightness;
        color = Pow( color, invGamma );

        // get the current color.
        curColor = Clamp( ( int )( 255.0f * color ), 0, 255 );

        // clamp and store.
        gammaTable[ j ] = ( unsigned short )( curColor << 8 );
        _colorTable[ j ] = curColor & 0xFF;
    }
/*
    for ( unsigned int i = 1; i < 256; ++i )
    {
        B_ASSERT( gammaTable[ i ] >= gammaTable[ i-1 ] );
        if ( gammaTable[ i ] < gammaTable[ i-1 ] )
            gammaTable[ i ] = gammaTable[ i-1 ];
    }
*/
    // set the GL gamma table.
    SetGLGamma( gammaTable, gammaTable, gammaTable );

    // mark the color table as updated.
    _colorTableDirty = false;
}

//----------------------------------------------------------
void
GrSubsys::CreateFrameBuffer()
{
    // create the type of framebuffer needed.

}
