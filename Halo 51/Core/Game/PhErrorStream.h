//----------------------------------------------------------
// File:		PhErrorStream.h
// Author:		Shawn Presser
// Created:		12-15-08
//
// Purpose:		To report PhysX errors.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#ifdef WIN32
#include <windows.h>
#endif

class PhErrorStream : public NxUserOutputStream
{
public:
	void reportError( NxErrorCode e, const char* message, const char* file, int line )
	{
		PrintF( "%s (%d) :", file, line );
		switch ( e )
		{
		case NXE_INVALID_PARAMETER:
			PrintF( "invalid parameter" );
			break;
		case NXE_INVALID_OPERATION:
			PrintF( "invalid operation" );
			break;
		case NXE_OUT_OF_MEMORY:
			PrintF( "out of memory" );
			break;
		case NXE_DB_INFO:
			PrintF( "info" );
			break;
		case NXE_DB_WARNING:
			PrintF( "warning" );
			break;
		default:
			PrintF( "unknown error" );
		}

		PrintF( " : %s\n", message );
	}

	NxAssertResponse reportAssertViolation( const char* message, const char* file, int line )
	{
		PrintF( "assert violation : %s (%s line %d)\n", message, file, line );
#ifdef WIN32
		switch ( MessageBoxA( 0, message, "AssertViolation, see console for details.", MB_ABORTRETRYIGNORE ) )
		{
		case IDRETRY:
			return NX_AR_CONTINUE;
		case IDIGNORE:
			return NX_AR_IGNORE;
		case IDABORT:
		default:
			return NX_AR_BREAKPOINT;
		}
#elif LINUX
		assert(0);
#elif _XBOX
		return NX_AR_BREAKPOINT;
#elif __CELLOS_LV2__
		return NX_AR_BREAKPOINT;
#endif
	}

	void print( const char* message )
	{
		PrintF( message );
	}
};
