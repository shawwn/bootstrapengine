#include "BApp.h"

// project includes.
#include <windows.h>

// engine includes.  
#include "common_afx.h"
#include "GLSubsys.h" 
#include "UExprMgr.h"
#include "RFileMgr.h"
#include "RRsrcMgr.h"
#include "GrSubsys.h"

//-----------------------
// file global variables.
HDC _BDC;
HGLRC _BRC;

//-----------------------
// file global functions.
static bool BInternalSetup( HWND windowHandle )
{
    // setup the pixel format.
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    
        1,                      /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,       /* support double-buffering */
        PFD_TYPE_RGBA,          /* framebuffer color type */
        32,                     /* preferred color depth */
        8, 0, 8, 0, 8, 0,       /* color bits (ignored) */
        8,                      /* no alpha buffer */
        0,                      /* alpha bits (ignored) */
        0,                      /* no accumulation buffer */
        0, 0, 0, 0,             /* accum bits (ignored) */
        24,                     /* depth buffer */
        0,                      /* no stencil buffer */
        0,                      /* no auxiliary buffers */
        PFD_MAIN_PLANE,         /* main layer */
        0,                      /* reserved */
        0, 0, 0,                /* no layer, visible, damage masks */
    };

    // TODO: update the pixel format based on attributes.
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags |= PFD_DOUBLEBUFFER;
    pfd.cColorBits = 24;    // 8 bits per color channel.
    pfd.cAlphaBits = 8;     // 8-bit alpha channel.
    pfd.cDepthBits = 16;    // 16-bit depth buffer.
    pfd.cStencilBits = 8;   // 8-bit stencil buffer.

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

    // initialize OpenGL.
    _BRC = wglCreateContext( _BDC );
    if( !_BRC )
        return false; // TODO: resource leaks?

    if( !wglMakeCurrent( _BDC, _BRC ) )
        return false; // TODO: resource leaks?

    StartGL( _BDC );
    return true;
}

//-----------------------
static void BInternalTeardown( HWND windowHandle )
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

//-----------------------
bool BAppSetup( WNDHANDLE windowHandle, const char* workingFolder )
{
    char appPath[ MAX_PATH ];
    ZeroMemory( appPath, MAX_PATH );

    if( workingFolder )
    {
        // copy the working folder.
        strcpy( appPath, workingFolder );
    }
    else
    {
        // if no working folder was specified, set it to the application exe's directory.
        GetModuleFileNameA( 0, appPath, MAX_PATH-1 );

        // grab a pointer to one past the last occurrence of '\'.
        char* slash = strrchr( appPath, '\\' )+1;

        // null terminate.
        *slash = 0;

        // set the directory.
        //SetCurrentDirectoryA( appPath );
    }

    // grab the window rect.
    RECT rect;
    GetWindowRect( windowHandle, &rect );

    // initialize OpenGL.
    if( !BInternalSetup( windowHandle ) )
        return false;

    // initialize the basics.
    new UExprMgr;
    new RFileMgr( appPath ); 
    new RRsrcMgr;
    new GrSubsys( rect.right - rect.left, rect.bottom - rect.top, true, true );

    return true;
}

//-----------------------
void BAppTeardown( WNDHANDLE windowHandle )
{
    delete gGrSubsys;
    delete gRRsrcMgr;
    delete gRFileMgr;
    delete gUExprMgr;

    return BInternalTeardown( windowHandle );
}

//-----------------------
void BAppSwapBuffers()
{
    SwapBuffers( _BDC );
}