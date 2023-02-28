//----------------------------------------------------------
// File:        Engine.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Engine.h"

// module header.
#include "Module.h"

// standard headers.
#include <windows.h>
#include <mmsystem.h>
#include <gl/GL.h>
#include <vcclr.h>

// engine headers.  TODO: move some of these into a single file, such as "BApp.h"?
#pragma managed( push, off ) // required by C++/CLI due to inline assembly.
#include "common_afx.h"
#include "GrScene.h"
#pragma managed( pop )
#include "RFileMgr.h"
#include "RRsrcMgr.h"
#include "UExprMgr.h"
#include "UExprCallback.h"
#include "GLSubsys.h" 
#include "GrConfig.h"
#include "GrSubsys.h"
#include "GrUtil.h"
#include "GrRenderer.h"
#include "GrRenderUtil.h"
#include "GrTexGen.h"
#include "GrFramebuffer.h"

// editlib headers.
#include "editlib_afx.h"
#include "EdConfig.h"
#include "EdImportDAE.h"
#include "EdBrushMgr.h"
#include "EdBrushPaletteMgr.h"

// project headers.
#include "Scene.h"
#include "OBox.h"
#include "Settings.h"
#include "Color.h"
#include "TimeKeeper.h"
#include "Utils.h"
#include "Camera.h"

// .NET namespaces.
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System;

// engine libraries.
#ifdef NDEBUG

// release mode 
#pragma comment( lib, "Common.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "UberTexture.lib" )
#pragma comment( lib, "EditLib.lib" )

#else

// debug mode 
#pragma comment( lib, "CommonD.lib" )
#pragma comment( lib, "GraphicsD.lib" )
#pragma comment( lib, "UberTextureD.lib" )
#pragma comment( lib, "EditLibD.lib" )

#endif

//----------------------------------------------------------
// Output Window's PrintF.
#pragma managed( push, off ) 
tstring _EdPrintF_Text;
int ( *PreviousPrintF )( const char *txt, ... ) = 0;
static int OutputWindowPrintF( const char *fmt, ... )
{
    // allocate room for the new string on the stack.  
    va_list marker;
    va_start( marker, fmt );
    int bufSize = _vscprintf( fmt, marker );
    char *buf = ( char * )alloca( bufSize + 1 );
    vsprintf_s( buf, bufSize + 1, fmt, marker );
    va_end( marker );

    // first, output to the standard PrintF.
    PreviousPrintF( "%s", buf );

    // then output to the output window.
    _EdPrintF_Text.append( buf );

    return 0;
}
#pragma managed( pop ) 

// private static member variables.
static HDC          _BDC;
static HGLRC        _BRC;
static float        _scriptCallbackTime = 0.0f;

//**********************************************************
// utility functions.
// (Note: these will eventually be moved into the SDK itself.)
//**********************************************************

//----------------------------------------------------------
static bool 
BInternalSetup( HWND windowHandle )
{
    // setup the pixel format.
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    
        1,                      /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,       /* support double-buffering */
        PFD_TYPE_RGBA,          /* framebuffer color type */
        24,                     /* preferred color depth */
        8, 0, 8, 0, 8, 0,       /* color bits (ignored) */
        8,                      /* no alpha buffer */
        0,                      /* alpha bits (ignored) */
        0,                      /* no accumulation buffer */
        0, 0, 0, 0,             /* accum bits (ignored) */
        24,                     /* depth buffer */
        8,                      /* no stencil buffer */
        0,                      /* no auxiliary buffers */
        PFD_MAIN_PLANE,         /* main layer */
        0,                      /* reserved */
        0, 0, 0,                /* no layer, visible, damage masks */
    };

    // TODO: update the pixel format based on attributes.
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags |= PFD_DOUBLEBUFFER;

    // get the device context.
    _BDC = GetDC( windowHandle );
    if( !_BDC )
        return false;

    // select the pixel format.
    int pixelFormat = ChoosePixelFormat( _BDC, &pfd );
    if( pixelFormat == 0 )
        return false;

    if( !SetPixelFormat( _BDC, pixelFormat, &pfd ) )
        return false;

    // grab the device.
    _BRC = wglCreateContext( _BDC );
    if( !_BRC )
    {
        ReleaseDC( windowHandle, _BDC );
        _BDC = 0;
        return false;
    }

    if( !wglMakeCurrent( _BDC, _BRC ) )
    {
        ReleaseDC( windowHandle, _BDC );
        _BDC = 0;
        return false;
    }

    // initialize OpenGL.
    StartGL( _BDC );

    return true;
}

//----------------------------------------------------------
static void 
BInternalTeardown( HWND windowHandle )
{
    // free the rendering context.
    if( _BRC )
    {
        wglDeleteContext( _BRC ); 
        _BRC = 0;
    }

    // free the device context.
    if( _BDC )
    {
        ReleaseDC( windowHandle, _BDC );
        _BDC = 0;
    }
}

//----------------------------------------------------------
// de-initializes the app.
static void 
BAppTeardown( HWND windowHandle )
{
    delete gEdBrushPaletteMgr;
    delete gEdBrushMgr;
    delete gGrSubsys;
    delete gRRsrcMgr;
    delete gRFileMgr;
    delete gUExprMgr;

    return BInternalTeardown( windowHandle );
}

//----------------------------------------------------------
// basic application setup.  Most newcomers will simply call this method.
static bool 
BAppSetup( HWND windowHandle, tstring workingFolder )
{
    RECT rect;
    GetWindowRect( windowHandle, &rect );

    // initialize OpenGL.
    if( !BInternalSetup( windowHandle ) )
        return false;

    // initialize the basics.
    new UExprMgr;
    new RFileMgr( workingFolder );
    new RRsrcMgr;
    new GrSubsys( rect.right - rect.left, rect.bottom - rect.top, false, true );
    new EdBrushMgr( Bootstrap::StrToTStr( Bootstrap::Settings::Get()->GetBrushFolder() ) );
    new EdBrushPaletteMgr( Bootstrap::StrToTStr( Bootstrap::Settings::Get()->GetBrushFolder() ) );

    return true;
}

//----------------------------------------------------------
// swap the buffers.
static void 
BSwapBuffers()
{
    SwapBuffers( _BDC );
}

//----------------------------------------------------------
static float 
TimeCallback()
{
    return _scriptCallbackTime;
}

//----------------------------------------------------------
tstring 
LowercaseString( const tstring& str )
{
    tstring result;
    for( tstring::const_iterator it = str.begin(); it != str.end(); ++it )
        result.append( 1, tolower( *it ) );
    return result;

}

//**********************************************************
// ref class Engine
//**********************************************************

//----------------------------------------------------------
Bootstrap::Engine::Engine( Control^ renderOnto, System::String^ workingFolder )
: _disposed( false )
{
    // grab the working folder.  If 'workingFolder' is null, use 'Bootstrap.Settings.GetWorkingFolder()' instead.
    tstring useWorkingFolder;
    if ( workingFolder == nullptr )
        useWorkingFolder = StrToTStr( Bootstrap::Settings::Get()->GetWorkingFolder() );
    else
        useWorkingFolder = StrToTStr( workingFolder );


    // initialize the engine.
    if( !BAppSetup( ( HWND )renderOnto->Handle.ToPointer(), useWorkingFolder ) )
        return;

    gUExprMgr->CreateScriptCallback( "_time", TimeCallback );

    // finish initialization.
    _renderWindow = renderOnto;
    _scene = gcnew Bootstrap::Scene();
    _timeKeeper = gcnew TimeKeeper();
}

//----------------------------------------------------------
Bootstrap::Engine::~Engine()
{
    this->!Engine();
    _disposed = true;
}

//----------------------------------------------------------
Bootstrap::Engine::!Engine()
{
    // force garbage collection.
//      _drawUberTexture->~DrawUberTexture();
    _scene->~Scene();
    _singleton = nullptr;
    _disposed = true;
    if( Initialized )
        BAppTeardown( ( HWND )_renderWindow->Handle.ToPointer() );

    Module::Shutdown();
}

//----------------------------------------------------------
Bootstrap::Engine^ 
Bootstrap::Engine::Startup( Control^ renderOnto, OutputCallback^ outputCallback, System::String^ workingFolder )
{
    // store the output callback.
    _outputCallback = outputCallback;

    // assign the new PrintF function.
    // TODO: uncomment this for debug purposes.
    PreviousPrintF = PrintF;
    PrintF = OutputWindowPrintF;
    EdPrintF = OutputWindowPrintF;

    B_ASSERT( Module::IsActive() );

    // return the singleton if it's already been created.
    if ( _singleton != nullptr )
        return _singleton;

    // return the new engine.
    _singleton = gcnew Engine( renderOnto, workingFolder );
    return _singleton;
}

//----------------------------------------------------------
void 
Bootstrap::Engine::PumpOutputMessages()
{
    // output any EdPrintF statements to the output callback.
    if( _outputCallback != nullptr )
    {
        _outputCallback( TStrToStr( _EdPrintF_Text ) );
        _EdPrintF_Text.clear();
    }
}

//----------------------------------------------------------
Bootstrap::Engine^ 
Bootstrap::Engine::Get()
{
    return _singleton;
}

//----------------------------------------------------------
bool 
Bootstrap::Engine::RenderWireframe::get()
{
    if ( !gGrRenderer )
        return false;
    return gGrRenderer->GetWireframeEnable();
}

//----------------------------------------------------------
void 
Bootstrap::Engine::RenderWireframe::set( bool renderWireframe )
{
    if ( gGrRenderer )
        gGrRenderer->SetWireframeEnable( renderWireframe );
}

//----------------------------------------------------------
int
Bootstrap::Engine::SysTimeDelta::get()
{
    return (int)_timeKeeper->GetSysTimeDelta();
}

//----------------------------------------------------------
int
Bootstrap::Engine::GameTimeDelta::get()
{
    return (int)_timeKeeper->GetGameTimeDelta();
}

//----------------------------------------------------------
bool 
Bootstrap::Engine::IsReady()
{
    // if we're not initialized, fail.
    return !_disposed && Initialized;
}

//----------------------------------------------------------
bool 
Bootstrap::Engine::ImportArtScene( String^ targetName, String^ pathToArtFile, bool findNormalMaps,
                                   bool uberTexture, bool overwriteMaterials, bool overwriteLights )
{
    EdImportDAE importer;
    if( !importer.ImportScene( StrToTStr( targetName ).c_str(), StrToTStr( pathToArtFile ).c_str(), findNormalMaps,
        uberTexture, overwriteMaterials, overwriteLights ) )
    {
        // TODO: Improve this error message.
        EdPrintF( "Failed to load scene %s.", StrToTStr( pathToArtFile + "/" + targetName ).c_str() );
        return false;
    }
    return true;
}

//----------------------------------------------------------
void 
Bootstrap::Engine::LoadBrushScript( System::String^ filePath, System::String^ outErrors )
{
    tstring errors;
    gRRsrcMgr->LoadScript( UPath( StrToTStr( filePath ) ) + ED_BRUSH_FILENAME, ED_KW_BRUSH, errors );
    outErrors = TStrToStr( errors );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::LoadBrushPaletteScript( System::String^ filePath, System::String^ outErrors )
{
    tstring errors;
    gRRsrcMgr->LoadScript( UPath( StrToTStr( filePath ) ) + ED_BRUSH_PALETTE_FILENAME, ED_KW_BRUSHPALETTE, errors );
    outErrors = TStrToStr( errors );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::LoadMaterialScript( System::String^ filePath, System::String^ outErrors )
{
    tstring errors;
    gRRsrcMgr->LoadScript( UPath( StrToTStr( filePath ) ) + GR_MATERIAL_FILENAME, GR_KW_MATERIAL, errors );
    outErrors = TStrToStr( errors );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::TimePause()
{
    _timeKeeper->Pause();
}

//----------------------------------------------------------
void 
Bootstrap::Engine::TimeResume()
{
    _timeKeeper->Resume();
}

//----------------------------------------------------------
void 
Bootstrap::Engine::Update()
{
    PumpOutputMessages();

    // if we're not initialized, fail.
    if( !IsReady() )
        return;

    _timeKeeper->Update();
    unsigned int sysTimeDelta = _timeKeeper->GetSysTimeDelta();
    unsigned int gameTimeDelta = _timeKeeper->GetGameTimeDelta();

    // update the script callback time.
    static unsigned int intScriptCallbackTime = 0;
    intScriptCallbackTime += gameTimeDelta;
    if ( intScriptCallbackTime > 10000000 )
        intScriptCallbackTime -= 10000000;
    
    // convert from milliseconds to seconds.
    _scriptCallbackTime = ( float )( intScriptCallbackTime / 1000.0 );

    // update the engine.
    gGrSubsys->Update( gameTimeDelta, sysTimeDelta );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::BeginRender( Camera^ camera )
{
    // if we're not initialized, fail.
    if( !IsReady() )
        return;

    // set the viewport.
    GrViewport( 0, 0, _renderWindow->Width, _renderWindow->Height );

    // render the scene.
#if 0
    gGrScene->Render( camera->_camera );
#else
    // clear the previous frame's statistics.
    ClearGLStats();

    // generate any procedural textures.
    gGrTexGen->GenTextures( camera->_camera );

    // start rendering frame.
    gGrFramebuffer->BeginFrame();

    // render the scene to the texture ( the framebuffer is a texture ).
    gGrScene->RenderTexture( camera->_camera, camera->_camera->GetPos() );
#endif
}

//----------------------------------------------------------
void 
Bootstrap::Engine::EndRender()
{

    // end rendering frame.
    gGrFramebuffer->EndFrame( gGrScene->GetSceneTime() );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::Resize( unsigned int width, unsigned int height )
{
    gGrSubsys->Resize( width, height );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::ClearDepthBuffer()
{
    // clear the depth buffer.
    unsigned int mask = GrGetWriteEnable();
    GrSetWriteEnable( GR_DEPTH );
    GrClear();
    GrSetWriteEnable( mask );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::Present()
{
    // present the scene.
    BSwapBuffers();
}

//----------------------------------------------------------
void 
Bootstrap::Engine::SetupStates( Camera^ camera, bool wireframe, bool depthTest )
{
    gGrRenderUtil->SetupColoredRender( *camera->_camera, wireframe, false );

    if ( depthTest )
        GrEnable( GR_DEPTHTEST );
    else
        GrDisable( GR_DEPTHTEST );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawSphere( const Vector3& pos, float radius, Color color ) 
{ 
    DrawSphere( pos, radius, color, 16 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawSphere( Vector3 pos, float radius, Color color, int slices )
{
    gGrRenderUtil->DrawColoredSphere( pos.ToMVec3(), radius, color.ToGrColor(), slices );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawLine( Vector3 start, Vector3 end, Color color )
{
    gGrRenderUtil->DrawColoredLine( start.ToMVec3(), end.ToMVec3(), color.ToGrColor() );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawAABox( AABox box, Color color )
{
    gGrRenderUtil->DrawColoredAABox( box.Center + box.HalfExtents, box.Center - box.HalfExtents, color );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawOBox( OBox box, Color color )
{
    gGrRenderUtil->DrawColoredOBox( box.Transform.ToMMat4x4(), box.HalfExtents.ToMVec3(), color.ToGrColor() );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::DrawPoint( Camera^ camera, Vector3 pos, Color color, int sizeInPixels )
{
    gGrRenderUtil->DrawColoredPoint( *camera->_camera, pos.ToMVec3(), color.ToGrColor(), sizeInPixels );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::ProcessConsoleMessage( String^ message )
{
    tstring msg = StrToTStr( message );
    TokenContainer tokens;
    StrTokenize( tokens, msg.c_str() );
    HandleConsoleMessage( msg, tokens );
}

//----------------------------------------------------------
void 
Bootstrap::Engine::HandleConsoleMessage( const std::string& wholeMessage, const std::vector< tstring >& tokens )
{
    // if there are no tokens, disregard this.
    if( tokens.size() == 0 )
        return;

    // try to issue the command to the various systems.
    bool commandIssued = false;
    commandIssued = commandIssued || gGrSubsys->IssueCommand( wholeMessage );

    // was there an error?
    if ( !commandIssued )
        EdPrintF( "Unknown console command.", wholeMessage.c_str() );
}
