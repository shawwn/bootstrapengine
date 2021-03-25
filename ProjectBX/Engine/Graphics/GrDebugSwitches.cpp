//----------------------------------------------------------
// File:		GrDebugSwitches.cpp
// Author:		Kevin Bray
// Created:		12-15-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrDebugSwitches.h"

// project includes.
#include "GrSubsys.h"

// disable compatibility warnings.
#pragma warning( disable : 4996 )

// boolean flag declaration macro.
#define DECLARE_BOOLEAN_SWITCH( name ) \
	bool name = false; \
	bool name##Proc( const tstring& command ) \
	{ \
		char buf[ 6 ]; \
		sscanf( command.c_str(), "%*s %5s", buf ); \
		if ( stricmp( buf, "true" ) == 0 || strcmp( buf, "1" ) == 0 ) \
			name = true; \
		else if ( stricmp( buf, "false" ) == 0 || strcmp( buf, "0" ) == 0 ) \
			name = false; \
		else \
			return false; \
		return true; \
	}

// boolean flag registration macro.
#define REGISTER_BOOLEAN_SWITCH( name )					gGrSubsys->AddCommandHandler( #name, name##Proc )

//**********************************************************
// Graphics engine debug switches
//**********************************************************
DECLARE_BOOLEAN_SWITCH( r_DrawUberPatchAABBs )

// this is called by GrSubsys on startup.
void GrRegisterDebugSwitches()
{
	REGISTER_BOOLEAN_SWITCH( r_DrawUberPatchAABBs );
}
