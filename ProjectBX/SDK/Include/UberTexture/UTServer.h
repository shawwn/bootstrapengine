//----------------------------------------------------------
// File:        UTServer.h
// Author:      Kevin Bray
// Created:     11-24-07
//
// Purpose:     To provide the entry point for the ubertexture server.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// UTServeThreadCreate: executes UTServe on it's own thread.  The return value
// is a handle that should be passed to calls regarding the server thread.
extern void UTServeThreadCreate( const char* workingFolder, unsigned short port, bool localOnly );

// UTServeThreadClose: terminates and closes the serve thread passed in.
extern void UTServeThreadClose();
