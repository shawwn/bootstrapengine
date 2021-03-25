//----------------------------------------------------------
// File:		ClPlatform.h
// Author:		Shawn Presser
// Created:		12-15-08
//
// Purpose:		Cross-platform client code, such as creating a 
//				window, getting keyboard/mouse/gamepad input, etc.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#ifdef WIN32
// windows headers.
#include <windows.h>
#endif

// std c++ headers.
#include <list>
#include <string>

// forward declarations.
struct ClWindow_Internal;
class ClWindow;

// callback typedefs.
typedef void ( *CLRESIZEFUNC )( ClWindow* window, int width, int height );
typedef void ( *CLCLOSEFUNC )( ClWindow* window );
typedef void ( *CLKEYBOARDFUNC )( ClWindow* window, char key );
typedef void ( *CLMOUSEMOVEFUNC )( ClWindow* window, int x, int y );
typedef void ( *CLMOUSEDOWNFUNC )( ClWindow* window, int button );
typedef void ( *CLMOUSEUPFUNC )( ClWindow* window, int button );

//**********************************************************
// class ClNoncopyable
//**********************************************************
class ClNoncopyable
{
	ClNoncopyable( const ClNoncopyable& );
	ClNoncopyable& operator = ( const ClNoncopyable& );
public:
	ClNoncopyable() { }
};

//**********************************************************
// class ClWindow
//**********************************************************
class ClWindow : public ClNoncopyable
{
	friend struct ClWindow_Internal;
public:
	ClWindow( ClWindow_Internal* );
	~ClWindow();

	// called after the window resizes.
	void						SetResizeFunc( CLRESIZEFUNC func )			{	_resizeFunc = func;			}

	// called after the window closes.
	void						SetCloseFunc( CLCLOSEFUNC func )			{	_closeFunc = func;			}

	// called when the user presses a keyboard key.
	void						SetKeyDownFunc( CLKEYBOARDFUNC func )		{	_keyDownFunc = func;		}

	// called when the user releases a keyboard key.
	void						SetKeyUpFunc( CLKEYBOARDFUNC func )			{	_keyUpFunc = func;			}

	// called when the user moves the mouse.
	void						SetMouseMoveFunc( CLMOUSEMOVEFUNC func )	{	_mouseMove = func;			}

	// called when the user presses a mouse button.
	void						SetMouseDownFunc( CLMOUSEDOWNFUNC func )	{	_mouseDown = func;			}

	// called when the user releases a mouse button.
	void						SetMouseUpFunc( CLMOUSEUPFUNC func )		{	_mouseUp = func;			}

	// swaps the window's back and front buffers.
	void						SwapBuffers();

	// sets whether the cursor is hidden or visible.
	void						SetCursorVisible( bool isVisible );

	// moves the mouse to the specified location, relative to the window.
	void						WarpMousePointer( int x, int y );

	// processes window events.
	void						Update();

#ifdef WIN32
	// returns the window's win32 device context.
	HDC							GetDeviceContext() const;
#endif

private:
	ClWindow_Internal*			_impl;
	CLRESIZEFUNC				_resizeFunc;
	CLCLOSEFUNC					_closeFunc;
	CLKEYBOARDFUNC				_keyDownFunc;
	CLKEYBOARDFUNC				_keyUpFunc;
	CLMOUSEMOVEFUNC				_mouseMove;
	CLMOUSEDOWNFUNC				_mouseDown;
	CLMOUSEUPFUNC				_mouseUp;
};

//**********************************************************
// class ClWindowStyle
//**********************************************************
class ClWindowStyle : public ClNoncopyable
{
	friend class ClPlatform;
	ClWindowStyle();
public:
	~ClWindowStyle();

	// creates a window style.  Specify a unique name for the style.  Also specify settings
	// that will be applied to each window derived from this style, such as whether the windows 
	// will have a menu, whether the windows are fullscreen, which cursor the windows use, 
	// the window icon, etc.
	static ClWindowStyle*		MakeWindowStyle( const wchar_t* uniqueName, bool isFullScreen, bool isGLWindow );

	const wchar_t*				GetName() const								{	return _name.c_str();		}

private:
	std::wstring				_name;
	bool						_fullScreen;
	bool						_isGLWindow;
};

//**********************************************************
// class ClPlatform
//**********************************************************
class ClPlatform : public ClNoncopyable
{
	friend class ClWindow;
public:
	ClPlatform();
	~ClPlatform();

	// creates a window.  (Note: can't use the name "CreateWindow" because that conflicts
	// with a Windows API macro.)
	ClWindow*					MakeWindow( const ClWindowStyle* style, const wchar_t* title, int x, int y, int width, int height );

private:
	typedef std::list< ClWindow* >	WindowContainer;

	WindowContainer				_windows;
};
extern ClPlatform*				gClPlatform;
