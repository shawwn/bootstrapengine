//----------------------------------------------------------
// File:		Module.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage module initialization and shutdown.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

namespace Bootstrap
{
    //**********************************************************
    // ref class Module
    //**********************************************************
	public ref class Module
	{
	public:
		static void                         Startup();
		static bool                         IsActive();

	internal:
		static void                         Shutdown();

	private:
		static bool                         _EngineStarted;
	};
}
