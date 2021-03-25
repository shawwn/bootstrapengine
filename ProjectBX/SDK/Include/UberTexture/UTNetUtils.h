//----------------------------------------------------------
// File:		UTNetUtils.cpp
// Author:		Kevin Bray
// Created:		11-24-07
//
// Purpose:		To provide a networking utility library.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

struct hostent;
struct sockaddr_in;

#ifndef _MSC_VER
#  define __w64
#endif

typedef __w64 unsigned int SOCKET;

enum SOCKETTYPE
{
	ST_STREAMING
};

//**********************************************************
// Global functions
//**********************************************************

// UTGetErrorString: returns the error string for the current error.
extern const char* UTGetErrorString();

// UTResolveAddress: takes in a string IP or named address and
// returns host information.
extern hostent* UTResolveHostAddress( const char* address );

// fill out a host address structure.
extern void UTMakeSockAddr( sockaddr_in* addr, unsigned long ip, unsigned short port );

// set a socket as asynchronous.
extern bool UTSetSocketAsync( SOCKET socket, bool async );

// enables or disables TCP buffering.
extern bool UTSetSocketBuffered( SOCKET socket, bool buffered );
