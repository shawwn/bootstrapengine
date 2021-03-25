//----------------------------------------------------------
// File:		TMain.cpp
// Author:		Kevin Bray
// Created:		04-04-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "test_afx.h"

// windows includes.
#include <windows.h>
#include <mmsystem.h>

// project includes.
#include "TGeomCompress.h"

// engine includes.
#include "GrPolygonNode.h"
#include "UHuffman.h"
#include "URand.h"
#include "RFileMgr.h"
#include "UExprMgr.h"
#include "RRsrcMgr.h"
#include "GLSubsys.h"
#include "GrSubsys.h"
#include "GrCamera.h"
#include "GrScene.h"
#include "GrUTLoader.h"
#include "GrUTAtlas.h"

// EditLib includes.
#include "EdImportDAE.h"

#define WINDOW_CLASS_NAME		"TestApp_GLWindow"
#define WINDOW_TITLE			"TestApp v1.0"

#define KEY_MOVE_LEFT			0x01
#define KEY_MOVE_RIGHT			0x02
#define KEY_MOVE_FORWARD		0x04
#define KEY_MOVE_BACKWARD		0x08

static HWND			_window = 0;
static HDC			_windowDC = 0;
static HGLRC		_windowRC = 0;
static HINSTANCE	_instance = 0;

// miscellaneous state.
static float		_windowAspect = 0.0f;
static unsigned int	_keyDown = 0;	// combination of MOVE_KEY_xxx flags.

static void Startup( const char* rootPath, unsigned int width = 1024, unsigned int height = 768 );
static void Shutdown();
static unsigned int GetKeyFlag( unsigned int virtualKey );
static void TestHuffman();
static void TestDiskServ();

#include "MSVD.h"
#include "UFixed.h"

//**********************************************************
// Main function
//**********************************************************

//----------------------------------------------------------
int
__stdcall
WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow )
{
/*
	// test out fixed point math code.
	{
		fixed_t test1 = FloatToFixed( -10.355f );
		fixed_t test2 = FloatToFixed( 7.355f );
		fixed_t test4 = FloatToFixed( 2207.25f );
		fixed_t test5 = FixedDiv( test4, test3 );
		fixed_t test6 = FixedInv( FloatToFixed( 0.25f ) );
		float result = FixedToFloat( test6 );
		PrintF( "%f\n", result );
	}

	// test out SVD code.
	{
		// build some vertices.
		MVec3 vertices[ 4 ];
		vertices[ 0 ] = MVec3( 1.0f, 0.0f, 0.0f );
		vertices[ 1 ] = MVec3( 0.0f, 0.0f, 1.0f );
		vertices[ 2 ] = MVec3( 0.0f, 0.0f, 0.0f );
		vertices[ 3 ] = MVec3( 0.0f, 1.0f, 0.0f );

		MVec3 matrix[ 4 ];
		MVec3 centroid;
		for ( unsigned int i = 0; i < 4; ++i )
			centroid += vertices[ i ];
		centroid *= 0.25f;

		for ( unsigned int i = 0; i < 4; ++i )
			matrix[ i ] = vertices[ i ] - centroid;

		// perform the SVD.
		MSVD svd( matrix, 4 );

		// check the results.
		MVec3 s = svd.GetSingularValues().Abs();
		const MMat3x3& vMat = svd.GetV();

		unsigned int col = 2;
		if ( s[ 0 ] < s[ 1 ] )
		{
			if ( s[ 0 ] < s[ 2 ] )
				col = 0;
		}
		else
		{
			if ( s[ 1 ] < s[ 2 ] )
				col = 1;
		}

		MVec3 normal;
		vMat.GetCol( normal, col );
		
		bool breakpt = false;
		breakpt = breakpt;
	}
*/
	// start up the app.
	Startup( lpCmdLine );

	// load the test scene.
	tstring errors;
//	gGrScene->LoadScene( "data/ubertest", errors );
	gGrScene->LoadScene( "data/metalroom", errors );

	// TODO: compress and decompress geometry.
//	T_CompressGeometryTest(  );

	// create the camera.
	GrCamera camera( MVec3( 0.0f, 0.0f, 0.0f ), MMat3x3(), DegToRad( 70.0f ), 100.0f, 1.0f );

	// hide the cursor.
	ShowCursor( FALSE );

	// set the timer to 1ms precision.
	timeBeginPeriod( 1U );

	// useful variables.
	MMat3x3 rotX;
	MMat3x3 rotY;
	MMat3x3 cameraRot;
	MVec3 cameraPos;
	MSG msg;
	RECT rect;

	// mouse orientation.
	float mouseRotX = 0.0f;
	float mouseRotY = 0.0f;
	float aspect = 1.0f;

	// run.
	unsigned int time = timeGetTime();
	bool running = true;
	while ( running )
	{
		int mouseDeltaX = 0;
		int mouseDeltaY = 0;
		if ( GetFocus() == _window )
		{
			// calculate the center-screen coordinate for the mouse.
			GetWindowRect( _window, &rect );
			int mouseCenterX = ( rect.right - rect.left ) / 2;
			int mouseCenterY = ( rect.bottom - rect.top ) / 2;

			// get the mouse cursor's current position.
			POINT mousePos;
			GetCursorPos( &mousePos );

			// calculate the mouse deltas.
			mouseDeltaX = mousePos.x - mouseCenterX;
			mouseDeltaY = mousePos.y - mouseCenterY;

			// re-center the mouse cursor.
			SetCursorPos( mouseCenterX, mouseCenterY );

			// calculate the aspect ratio.
			GetClientRect( _window, &rect );
			aspect = ( float )rect.right / ( float )rect.bottom;
		}

		// check to see if there are any messages waiting.
		while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			// handle the current message.
			if ( GetMessage( &msg, NULL, 0, 0 ) != TRUE )
			{
				running = false;
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// calculate the time.
		unsigned int delta = time;
		time = timeGetTime();
		delta = time - delta;

		// build the camera's rotation.
		if ( mouseDeltaY != 0 )
			mouseRotX -= ( float )mouseDeltaY / 10.0f;
		if ( mouseDeltaX != 0 )
			mouseRotY -= ( float )mouseDeltaX / 10.0f;
		rotX.MakeXRotation( DegToRad( mouseRotX ) );
		rotY.MakeYRotation( DegToRad( mouseRotY ) );
		cameraRot = rotY * rotX;

		// determine the camera's position.
		float moveDist = 10.0f * delta / 1000.0f;		// 5 units per second.
		if ( _keyDown & KEY_MOVE_FORWARD )
			cameraPos -= moveDist * cameraRot.GetCol( 2 );
		if ( _keyDown & KEY_MOVE_BACKWARD )
			cameraPos += moveDist * cameraRot.GetCol( 2 );
		if ( _keyDown & KEY_MOVE_LEFT )
			cameraPos -= moveDist * cameraRot.GetCol( 0 );
		if ( _keyDown & KEY_MOVE_RIGHT )
			cameraPos += moveDist * cameraRot.GetCol( 0 );

		// apply the transform to the camera.
		camera.SetRot( cameraRot );
		camera.SetPos( cameraPos );
		camera.SetProjAspectRatio( aspect );

		// update the engine
		gGrSubsys->Update( delta, delta );
		gGrSubsys->Render( &camera );

		// present the contents of the backbuffer.
		SwapBuffers( _windowDC );
	}

	// end 1ms timer precision.
	timeEndPeriod( 1U );

	// test geometry compression.
//	TestGeometryCompression();

	// shut down.
	Shutdown();

	// return success.
	return 0;
}

//----------------------------------------------------------
LRESULT
CALLBACK
WindowProc( HWND window, UINT message, WPARAM wparam, LPARAM lparam )
{
	switch ( message )
	{
	case WM_KEYDOWN:
		if ( wparam == VK_ESCAPE )
			PostQuitMessage( 0 );
		else
			_keyDown |= GetKeyFlag( ( unsigned int )wparam );
		break;

	case WM_KEYUP:
		_keyDown &= ~GetKeyFlag( ( unsigned int )wparam );
		break;
	}
	return DefWindowProc( window, message, wparam, lparam );
}

//----------------------------------------------------------
void
Startup( const char* rootPath, unsigned int width, unsigned int height )
{
	// get the instance.
	_instance = GetModuleHandle( NULL );

	// fill out the window class.
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= _instance;
	wc.hIcon			= LoadIcon( NULL, IDI_WINLOGO );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= WINDOW_CLASS_NAME;
	RegisterClass( &wc );

	// create the window.
	RECT wndRect;
	wndRect.left = 0;
	wndRect.right = width;
	wndRect.top = 0;
	wndRect.bottom = height;
	AdjustWindowRect( &wndRect, WS_OVERLAPPEDWINDOW & ~WS_OVERLAPPED, FALSE );
	::_window = CreateWindow( WINDOW_CLASS_NAME, WINDOW_TITLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN |	WS_OVERLAPPEDWINDOW,
							  0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top,
							  NULL, NULL, _instance, NULL );

	// initialize OpenGL.
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	_windowDC = GetDC( _window );
	int pixelFormat = ChoosePixelFormat( _windowDC, &pfd );
	SetPixelFormat( _windowDC, pixelFormat, &pfd );
	
	_windowRC = wglCreateContext( _windowDC );
	wglMakeCurrent( _windowDC, _windowRC );

	ShowWindow( _window, SW_SHOW );

	// initialize the graphics engine.
	StartGL( _windowDC );
	new RFileMgr( rootPath );
	new UExprMgr;
	new RRsrcMgr;
	new GrSubsys( width, height, false, true );
}

//----------------------------------------------------------
void
Shutdown()
{
	delete gGrSubsys;
	delete gRRsrcMgr;
	delete gUExprMgr;
	delete gRFileMgr;
	StopGL();

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( _windowRC );
	ReleaseDC( _window, _windowDC );
	DestroyWindow( _window );
	UnregisterClass( WINDOW_CLASS_NAME, _instance );
}

//----------------------------------------------------------
unsigned int
GetKeyFlag( unsigned int virtualKey )
{
	// get the keycode from the virtual key passed in.
	unsigned int keyCode = 0;
	if ( virtualKey == 'W' )
		keyCode = KEY_MOVE_FORWARD;
	else if ( virtualKey == 'S' )
		keyCode = KEY_MOVE_BACKWARD;
	else if ( virtualKey == 'A' )
		keyCode = KEY_MOVE_LEFT;
	else if ( virtualKey == 'D' )
		keyCode = KEY_MOVE_RIGHT;

	// return the keycode.
	return keyCode;
}

#include "Compress.h"
//----------------------------------------------------------
void
TestHuffman()
{
	// do a simple test.
#if 1
	{
		UHuffman huffman;
		const char* test = "Testing, 123!  Please work!  Why are there errors being found?  This seems really strange to me.";
//		const char* test = "WTF..!";

		// compress data.
		unsigned char codeLen[ 256 ];
		unsigned char compressed[ 1024 ];
		unsigned int srcBitLen = huffman.Compress( compressed, codeLen, test, ( unsigned int )strlen( test ) + 1 );

		// decompress data.
		unsigned char buffer[ 1024 ];
		memset( buffer, 0x1F, 1024 );
		unsigned int testSize = huffman.Decompress( buffer, codeLen, compressed, srcBitLen );
		testSize = testSize;
	}
#else
	// object used for encoding data.
	UHuffman huffman;

	// total size of test data.
	unsigned int dataSize = 10 * 1024 * 1024; // 131072;
	unsigned int maxValue = 256;

	// allocate space for source and destination data.
	unsigned char* src = new unsigned char[ dataSize ];
	unsigned char* dst = new unsigned char[ dataSize + dataSize / 10 ];
	unsigned char* tmp = new unsigned char[ dataSize + dataSize / 10 ];

	// fill the source with some random data.
	PrintF( "Beginning Huffman speed test...\n" );
	PrintF( "Building test stream... " );
//	URand random( timeGetTime() );
	URand random( 123456789 );
	for ( unsigned int i = 0; i < dataSize; ++i )
	{
#if 1
		src[ i ] = ( unsigned char )( ( random.RandFloat() * random.RandFloat() ) * 255.0f );
#else
		src[ i ] = ( unsigned char )( rand() % 255 );
#endif
	}
	PrintF( "[done]\n\n" );

	// encode the data.
	PrintF( "Encoding data... " );
	unsigned int startTime = timeGetTime();

	// compress the data.
	unsigned char codeLengths[ 256 ];
	unsigned int numBits = huffman.Compress( tmp, codeLengths, src, dataSize );

	// calculate the total time and statistics.
	unsigned int endTime = timeGetTime();
	unsigned int totalTime = endTime - startTime;
	unsigned int compressedSize = ( numBits + 7 ) / 8;
	float compressedRatio = ( float )( compressedSize / ( double )dataSize ) * 100.0f;
	float encodeMBPerSec = ( float )( ( double )dataSize / ( totalTime / 1000.0 ) / 1048576.0 );
	PrintF( "[done]\n" );
	PrintF( "  Total time:			%d\n", totalTime );
	PrintF( "  Performance:			%.2fMB/sec\n", encodeMBPerSec );
	PrintF( "  Encoded Size:		%d\n", compressedSize );
	PrintF( "  Compression Ratio:   %.2f%%\n\n", compressedRatio );

	// now decode the data.
	PrintF( "Decoding data... " );
	startTime = timeGetTime();

	// decode the data.
	unsigned int repCount = 1;
//	while ( true )
	{
		for ( unsigned int i = 0; i < repCount; ++i )
		{
//			unsigned int dstLen = huffman.Decompress( dst, codeLengths, tmp, numBits );
			unsigned char* curDst = dst;
			huffman.SetDecompStream( codeLengths, tmp, numBits );
			for ( unsigned int i = 0; i < dataSize; ++i )
				*curDst++ = huffman.ReadValue();
		}
	}

	// calculate the total time and statistics.
	endTime = timeGetTime();
	totalTime = endTime - startTime;
	float decodeMBPerSec = ( float )( ( double )( dataSize * repCount ) / ( totalTime / 1000.0 ) / 1048576.0 );
	PrintF( "[done]\n" );
	PrintF( "  Total time:			%d\n", totalTime );
	PrintF( "  Performance:			%.2fMB/sec\n\n", decodeMBPerSec );

	// verify the input and the output match up.
	PrintF( "Checking data consistancy... " );
	bool errorsFound = false;
	for ( unsigned int i = 0; i < dataSize; ++i )
	{
		if ( src[ i ] != dst[ i ] )
		{
			errorsFound = true;
			break;
		}
	}

	PrintF( "[done]\n" );
	if ( errorsFound )
		PrintF( "  Errors found!\n\n" );
	else
		PrintF( "  No errors found.\n\n" );
	PrintF( "Compression test compete.\n" );
#endif
}

//----------------------------------------------------------
#include "FSDiskServ.h"
void
TestDiskServ()
{
	void* handle = FS_OpenFile( "fstest.txt", FS_FILE_FLAG_READ | FS_FILE_FLAG_USE_DMA );

	// useful constants.
	const unsigned int kDataSize = 64 * 1024 * 1024;
	const unsigned int kPageCount = 16;
	const unsigned int kPageSize = 65536;
	const unsigned int kAlignment = 2048;

	// write some file data.
	volatile int gate = 0;
//	SFSFileWriteBuffer buffer;
//	buffer.buffer = new unsigned char[ kDataSize ];
//	buffer.complete = 0;

	// write data to the file.
//	FS_WriteFile( handle, 0, &buffer, kDataSize );
//	FS_Wait( &buffer.complete );

	// allocate a bunch of read buffers.
	SFSFileReadBuffer readBuffer[ 2048 ];
	for ( unsigned int i = 0; i < 2048; ++i )
	{
		readBuffer[ i ].buffer = AlignedAlloc( kAlignment, kPageSize );
		readBuffer[ i ].complete = 0;
	}

	timeBeginPeriod( 1 );
	unsigned int start = timeGetTime();

	// now issue a bunch of read operations.
	for ( unsigned int i = 0; i < 2048; ++i )
	{
		unsigned int page = ( ( unsigned int )rand() % 1024 );
//		unsigned int page = ( i % 1024 );
		FS_ReadFile( readBuffer + i, handle, page * kPageSize, kPageSize );
	}

	// free the read buffers.
	for ( unsigned int i = 0; i < 2048; ++i )
		FS_Wait( &readBuffer[ i ].complete );

	unsigned int total = timeGetTime() - start;
	unsigned int totalRead = 65536 * 2048;
	float speedMBPerSec = ( float )( ( totalRead / ( total / 1000.0 ) ) / 1048576.0 );
	PrintF( "Read speed: %.2f MB/sec\n", speedMBPerSec );

	timeEndPeriod( 1 );

	for ( unsigned int i = 0; i < 2048; ++i )
		AlignedFree( ( void* )readBuffer[ i ].buffer );

	// close the file.
	FS_CloseFile( handle );
}

/*
	{
		ULRUList< int > lruList( 10 );
		for ( unsigned int i = 0; i < 100; ++i )
		{
			// add the current item to the list.
			lruList.Add( i );
			void* lru = lruList.GetLRUItem();
			PrintF( "Last item: %d, Cur item: %d\n", lruList.Get( lru ), i );
		}

		bool breakPt = false;
		breakPt = breakPt;
	}

	{
		ULRUHash< unsigned int, unsigned int > hash( 32 );

		// test out the hash table.
		for ( unsigned int i = 0; i < 64; ++i )
		{
			// add a new item to the hash table.
			unsigned int removed = ~0;
			hash.Insert( removed, i / 2, i );

			// post a message.
			if ( removed != ~0 )
				PrintF( "Item removed: %d\n", removed );
		}

		// caclulate the key for the value we want to remove (in this
		// case, 21).
		unsigned int key = 38 / 2;

		for ( void* item = hash.FindFirst( key ); item != 0; item = hash.FindNext( item, key ) )
		{
			if ( hash.Get( item ) == 38 )
			{
				hash.Remove( item );
				break;
			}	
		}

		// test that our item was actually removed.
		for ( void* item = hash.FindFirst( key ); item != 0; item = hash.FindNext( item, key ) )
			PrintF( "Item with key %d: %d\n", key, hash.Get( item ) );
	}
*/
/*
	// total tile count for a square texture of a given size.
	unsigned int tileCount = 0;
	unsigned int mipSize = 2048;
	while ( mipSize )
	{
		tileCount += mipSize * mipSize;
		mipSize >>= 1;
	}

	// build the morton table.
	unsigned int mortonTable[ 2048 ];
	for ( unsigned int i = 0; i < 2048; ++i )
	{
		mortonTable[ i ] = 0;
		unsigned int mask = 1;
		for ( unsigned int b = 0; b < 11; ++b, mask += mask )
			mortonTable[ i ] |= ( i & mask ) << b;
	}
*/

/*
	// test the loader.
	const char* images[] =
	{
		"lena.tga",
		"baboon.tga",
		"peppers.tga",
		"floorplate01_c.tga",
		"floorplate01_s.tga",
	};
	gGrUTLoader->Test( images, sizeof( images ) / sizeof( const char* ) );
*/
	// create a test ubertexture.
#if 0
		// build the morton table.
		unsigned int mortonTable[ 2048 ];
		for ( unsigned int i = 0; i < 2048; ++i )
		{
			mortonTable[ i ] = 0;
			unsigned int mask = 1;
			for ( unsigned int b = 0; b < 11; ++b, mask += mask )
				mortonTable[ i ] |= ( i & mask ) << b;
		}

		// calculate the tile use bitmap's size in DWORDs and
		// allocate the tile use bitmap.
		const unsigned int tileUseBitmapSize = ( GR_UBERTEX_MAX_SIZE_IN_TILES * GR_UBERTEX_MAX_SIZE_IN_TILES ) / 32;
		unsigned int* tileUseBitmap = new unsigned int[ tileUseBitmapSize ];

		// clear the tile-use bitmap.
		MemSet( tileUseBitmap, 0, 4 * tileUseBitmapSize );

		// set bits in the image.
		for ( unsigned int y = 0; y < 32; ++y )
		{
			// calculate row start/end.
			unsigned int start = ( unsigned int )Abs( 16 - ( int )y );
			unsigned int end = 32 - start; 
			for ( unsigned int x = start; x < end; ++x )
			{
				// calculate the bit-index of the current tile.
				unsigned int xBits = mortonTable[ x ];
				unsigned int yBits = mortonTable[ y ];
				unsigned int index = ( yBits + yBits ) | xBits;

				// now calculate which bit and DWORD we need to
				// modify.
				unsigned int pos = index >> 5;
				unsigned int bit = index & 31;

				// set the bit.
				tileUseBitmap[ pos ] |= 1 << bit;
			}
		}
/*
		// initialize the tile use bitmap.  Mark the first 256 tiles
		// as used (8 DWORDs) for a 1024 x 1024 usage mask.
		for ( unsigned int i = 0; i < 8; ++i )
			tileUseBitmap[ i ] = ~0;
		for ( unsigned int i = 8; i < tileUseBitmapSize; ++i )
			tileUseBitmap[ i ] = 0;
*/
		// open the file that will be our new ubertexture.
		unsigned int fileFlags = FS_FILE_FLAG_CREATE | FS_FILE_FLAG_READ | FS_FILE_FLAG_WRITE |
			FS_FILE_FLAG_USE_DMA;
		void* file = FS_OpenFile( "c:/bin/game/test.ubr", fileFlags );

		// create the ubertexure.
		GrUTAtlas* atlas = gGrUTLoader->CreateUbertex( file, tileUseBitmap, 0, 0, 0 );
		FS_CloseFile( file );
#endif
#if 0
		// open the ubertexture.
		unsigned int fileFlags = FS_FILE_FLAG_READ | FS_FILE_FLAG_WRITE | FS_FILE_FLAG_USE_DMA;
		void* file = FS_OpenFile( "c:/bin/game/test.ubr", fileFlags );
		GrUTAtlas* atlas = gGrUTLoader->LoadUbertex( file );
		FS_CloseFile( file );
#endif

#if 0
		// output information in the atlas to ensure that everything
		// is in the right order.
		unsigned int mipDim = 32;
		unsigned int mipLevel = 0;
		while ( mipLevel <= 11 )
		{
			for ( unsigned int y = 0; y < mipDim; ++y )
			{
				for ( unsigned int x = 0; x < mipDim; ++x )
					PrintF( "%3d, ", atlas->FindTileChunk( mipLevel, x, y ) );

				PrintF( "\n" );
			}
			mipDim = Max( mipDim >> 1, 1U );
			++mipLevel;
		}
		delete atlas;
#endif

#if 0
		// do atlas performance timing.
		unsigned int startTime = timeGetTime();
		const unsigned int lookupCount = 100000;
		for ( unsigned int i = 0; i < lookupCount; ++i )
		{
			atlas->FindTileChunk( 0, 14, 16 );
		}

		unsigned int endTime = timeGetTime();
		unsigned int totalTime = endTime - startTime;
		float lookupsSec = ( float )( ( lookupCount * 1000.0 ) / totalTime );
		PrintF( "Atlas lookups/sec: %f\n", lookupsSec );
		delete atlas;
#endif

