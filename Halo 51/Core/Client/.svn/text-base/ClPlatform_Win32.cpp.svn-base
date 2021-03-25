#ifdef WIN32
//----------------------------------------------------------
// File:		ClPlatform_Win32.cpp
// Author:		Shawn Presser
// Created:		12-15-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "ClPlatform.h"

// Bootstrap Engine headers.
#include "Common/common_afx.h"

// windows headers.
#include <windows.h>

// std c++ headers.
#include <algorithm>

// global variables.
ClPlatform*					gClPlatform;

// forward declarations.
bool						InitializeGLWindow( HGLRC& result, HDC device );

//**********************************************************
// struct ClWindow_Internal
//**********************************************************
struct ClWindow_Internal
{
	// window message callback.
	static LRESULT CALLBACK		WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	HWND						Window;
	HDC							Device;
	HGLRC						GLContext;
};

//----------------------------------------------------------
LRESULT CALLBACK
ClWindow_Internal::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// get a pointer to the window interface object associated with that win32 window handle.
	ClWindow* window = (ClWindow*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	if ( window )
	{
		switch ( uMsg )
		{
		case WM_QUIT:
		case WM_CLOSE:
			{
				// if the window is closing, call the window's close callback.
				if ( window->_closeFunc )
				{
					window->_closeFunc( window );
					return 0;
				}
			}
			break;

		case WM_CHAR:
			{
				if ( window->_keyDownFunc )
				{
					// if the key is repeating, don't handle it.
					if ( HIWORD( lParam ) & KF_REPEAT )
						break;

					window->_keyDownFunc( window, (char)wParam );
					return 0;
				}
			}
			break;

		case WM_KEYUP:
			{
				if ( window->_keyUpFunc )
				{
					BYTE state[ 256 ];
					WORD code[ 2 ];

					GetKeyboardState( state );
					if ( ToAscii( wParam, 0, state, code, 0 ) == 1 )
					{
						window->_keyUpFunc( window, (char)code[ 0 ] );
						return 0;
					}
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				if ( window->_mouseMove )
				{
					// grab the mouse position.
					int mouseX = (int)(short)LOWORD( lParam );
					int mouseY = (int)(short)HIWORD( lParam );

					// restrict to [-32768, 32767] to match X11 behaviour.
					// see comment in "freeglut_developer" mailing list 10/4/04.
					if ( mouseX > 32767 ) mouseX -= 65536;
					if ( mouseY > 32767 ) mouseY -= 65536;

					// invoke the mouse move callback.
					window->_mouseMove( window, mouseX, mouseY );
					return 0;
				}
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
				// determine whether the mouse button is down.
				bool down = false;
				switch ( uMsg )
				{
				case WM_LBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_RBUTTONDOWN:
					down = true;
					break;
				}

				// determine which mouse button was pressed or released.
				int button = 0;
				switch ( uMsg )
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
					button = 0;
					break;

				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
					button = 1;
					break;

				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					button = 2;
					break;
				}

				// invoke the callback.
				if ( down && window->_mouseDown )
				{
					window->_mouseDown( window, button );
					return 0;
				}
				if ( !down && window->_mouseUp )
				{
					window->_mouseUp( window, button );
					return 0;
				}
			}
			break;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//**********************************************************
// struct ClWindow
//**********************************************************

//----------------------------------------------------------
ClWindow::ClWindow( ClWindow_Internal* impl )
: _impl( impl )
, _resizeFunc( 0 )
, _closeFunc( 0 )
, _keyDownFunc( 0 )
, _keyUpFunc( 0 )
, _mouseMove( 0 )
, _mouseDown( 0 )
, _mouseUp( 0 )
{
}

//----------------------------------------------------------
ClWindow::~ClWindow()
{
	B_ASSERT( gClPlatform );

	// shutdown the window's device context.
	if ( _impl->Device )
	{
		ReleaseDC( _impl->Window, _impl->Device );
	}

	// shutdown the window.
	if ( _impl->Window )
	{
		DestroyWindow( _impl->Window );
	}

	// free our internal state.
	delete _impl;

	// remove ourselves from the global list of windows.
	gClPlatform->_windows.remove( this );
}

//----------------------------------------------------------
void
ClWindow::SwapBuffers()
{
#ifdef WIN32
	if ( !::SwapBuffers( _impl->Device ) )
	{
		B_ASSERT( !"SwapBuffers() failed." );
	}
#else 
#error "TODO"
    //glXSwapBuffers( _impl->Display, _impl->Window );
#endif
}

//----------------------------------------------------------
void
ClWindow::SetCursorVisible( bool isVisible )
{
	ShowCursor( isVisible ? TRUE : FALSE );
}

//----------------------------------------------------------
void
ClWindow::WarpMousePointer( int x, int y )
{
	POINT coords;
	coords.x = x;
	coords.y = y;

	/* ClientToScreen() translates {coords} for us. */
	ClientToScreen( _impl->Window, &coords );
	SetCursorPos( coords.x, coords.y );
}

//----------------------------------------------------------
void
ClWindow::Update()
{
	for ( unsigned int i = 0; i < 4; ++i )
	{

		// process any pending window messages.
		MSG msg;
		if ( !PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			return;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

//----------------------------------------------------------
HDC
ClWindow::GetDeviceContext() const
{
	B_ASSERT( _impl->Device );
	return _impl->Device;
}

//**********************************************************
// class ClWindowStyle
//**********************************************************

//----------------------------------------------------------
ClWindowStyle::ClWindowStyle()
: _fullScreen( false )
{
}

//----------------------------------------------------------
ClWindowStyle::~ClWindowStyle()
{
	// get the current application instance.
	HINSTANCE appInstance = GetModuleHandle( 0 );

	// unregister the window style.
	if ( !UnregisterClass( _name.c_str(), appInstance ) )
	{
		B_ASSERT( !"Failed to unregister a window style." );
	}
}

//----------------------------------------------------------
ClWindowStyle*
ClWindowStyle::MakeWindowStyle( const wchar_t* uniqueName, bool isFullScreen, bool isGLWindow )
{
	// get the current application instance.
	HINSTANCE appInstance = GetModuleHandle( 0 );

	// translate the incoming window settings into equivalent Win32 API settings.
	WNDCLASS wc;
	{{
		wc.hInstance = appInstance;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0; // add space to store the ClWindow pointer.
		wc.hbrBackground = 0;

		// style name.
		wc.lpszClassName = uniqueName;

		// menu name.
		wc.lpszMenuName = 0;

		// redraw on move, and own a DC for the window.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		// window message callback.
		wc.lpfnWndProc = &ClWindow_Internal::WndProc;

		// use the default window icon for now.
		wc.hIcon = LoadIcon( 0, IDI_WINLOGO );

		// use the default cursor.
		wc.hCursor = LoadCursor( 0, IDC_ARROW );
	}}

	// register the window style.
	if ( !RegisterClass( &wc ) )
	{
		B_ASSERT( !"Failed to register a window style." );
		return 0;
	}

	// return a window style interface object.
	ClWindowStyle* result = new ClWindowStyle();
	result->_name = uniqueName;
	result->_fullScreen = isFullScreen;
	result->_isGLWindow = isGLWindow;
	return result;
}

//**********************************************************
// class ClPlatform
//**********************************************************

//----------------------------------------------------------
ClPlatform::ClPlatform()
{
	B_ASSERT( !gClPlatform );

	gClPlatform = this;
}

//----------------------------------------------------------
ClPlatform::~ClPlatform()
{
	B_ASSERT( gClPlatform );

	// close all windows.
	while ( !_windows.empty() )
	{
		ClWindow* window = *_windows.begin();
		delete window;
	}

	gClPlatform = 0;
}

//----------------------------------------------------------
ClWindow*
ClPlatform::MakeWindow( const ClWindowStyle* style, const wchar_t* title, int x, int y, int width, int height )
{
	// no menu, for now.
	HMENU menu = 0;

	// select the win32 window style flags.
	DWORD styleFlagsEx;
	DWORD styleFlags;
	if ( style->_fullScreen )
	{
		styleFlagsEx = WS_EX_APPWINDOW;
		styleFlags = WS_POPUP;
	}
	else
	{
		styleFlagsEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		styleFlags = WS_OVERLAPPEDWINDOW;
	}

	// adjust the windows width and height such that the area we draw on 
	// is the exact resolution passed in.
	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = width;
	windowRect.bottom = height;
	AdjustWindowRectEx( &windowRect, styleFlags, menu ? TRUE : FALSE, styleFlagsEx );

	// get the current application instance.
	HINSTANCE appInstance = GetModuleHandle( 0 );

	// try to create the window.
	HWND hWnd = CreateWindowEx(

		// window extended style settings.
		styleFlagsEx,

		// window class name.
		style->GetName(),

		// window title.
		title,

		// window style settings.
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | styleFlags,

		// window position.
		x, y,

		// adjusted window width / height.
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,

		// parent window.
		0,

		// menu.
		menu,

		// application instance.
		appInstance,

		// don't pass anything to WM_CREATE.
		0 );

	// if the window could not be created, abort.
	if ( !hWnd )
	{
		B_ASSERT( !"Failed to create a window." );
		return 0;
	}

	// create the window interface object.
	ClWindow_Internal* windowImpl = new ClWindow_Internal();
	windowImpl->Window = hWnd;
	windowImpl->Device = GetDC( hWnd );
	windowImpl->GLContext = 0;
	ClWindow* result = new ClWindow( windowImpl );

	// store it in the win32 window handle's userdata.
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)result );

	// if the window is a GL window, then prepare it for GL rendering.
	if ( style->_isGLWindow )
	{
		if ( !InitializeGLWindow( windowImpl->GLContext, windowImpl->Device ) )
		{
			B_ASSERT( !"Failed to initialize a window for GL rendering." );
			delete result;
			return 0;
		}
	}

	// show the window and pull it to the foreground.
	ShowWindow( hWnd, SW_SHOW );
	SetForegroundWindow( hWnd );
	SetFocus( hWnd );

	// add the window interface object to the global window list.
	_windows.push_back( result );

	// return the window interface object.
	return result;
}

//----------------------------------------------------------
static bool
InitializeGLWindow( HGLRC& result, HDC device )
{
	result = 0;

	// setup the pixel format.
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof( PIXELFORMATDESCRIPTOR ),    
		1,						// version
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,       // support double-buffering
		PFD_TYPE_RGBA,          // framebuffer color type
		24,						// preferred color depth
		8, 0, 8, 0, 8, 0,       // color bits (ignored)
		8,						// no alpha buffer
		0,						// alpha bits (ignored)
		0,						// no accumulation buffer
		0, 0, 0, 0,				// accum bits (ignored)
		24,						// depth buffer
		8,						// stencil buffer
		0,						// no auxiliary buffers
		PFD_MAIN_PLANE,         // main layer
		0,						// reserved
		0, 0, 0,				// no layer, visible, damage masks
	};

	// TODO: update the pixel format based on attributes.
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags |= PFD_DOUBLEBUFFER;

	// select the pixel format.
	int pixelFormat = ChoosePixelFormat( device, &pfd );
	if( pixelFormat == 0 )
		return false;

	if( !SetPixelFormat( device, pixelFormat, &pfd ) )
	{
		B_ASSERT( false );
		return false;
	}

	// grab the device.
	result = wglCreateContext( device );
	if( !result )
	{
		B_ASSERT( false );
		return false;
	}

	if( !wglMakeCurrent( device, result ) )
	{
		B_ASSERT( false );
		return false;
	}

	return true;
}

#endif
