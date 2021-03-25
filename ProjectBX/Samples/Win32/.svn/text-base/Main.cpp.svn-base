
// project includes.
#include <windows.h>

// engine includes.
#include "graphics_afx.h"
#include "GrMeshInst.h"
#include "BApp.h"

// forward declarations.
LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );

// member variables.
bool _done = false;

//-----------------------
// utility functions.
void DbgBreak()
{
	__asm
	{
		int 3;
	}
}

//-----------------------
// project functions.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd )
{
	// register a window class.
	WNDCLASS wc;
	ZeroMemory( &wc, sizeof( WNDCLASS ) );
	wc.hCursor = LoadCursor( instance, 0 );
	wc.hIcon = LoadIcon( instance, 0 );
	wc.hInstance = instance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"WinGraphicsSample";
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	if( !RegisterClass( &wc ) )
	{
		DbgBreak();
		return 0;
	}

	// create the window.
	HWND wnd = CreateWindowEx( 
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		L"WinGraphicsSample", L"Engine", 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, 
		100, 100, 
		1024, 768,
		0, 0, instance, 0 );
	if( !wnd )
	{
		DbgBreak();
		return 0;
	}

	// initialize Bootstrap Engine.
	BAppSetup( wnd, "C:/bin/game/" );

	// show the window.
	ShowWindow( wnd, SW_SHOW );

	while( !_done )
	{
		// process window messages.
		MSG msg;
		if( PeekMessage( &msg, wnd, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// render.

		// swap buffers.
		BAppSwapBuffers();

		// yield CPU time.
		Sleep( 1 );
	}

	// de-initialize Bootstrap Engine.
	BAppTeardown( wnd );

	return 0;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_CLOSE:
	case WM_QUIT:
		{
			// notify the engine that it needs to exit.
			_done = true;
		}
		break;
	}
	return DefWindowProc( wnd, msg, wParam, lParam );
}