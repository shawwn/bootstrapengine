//----------------------------------------------------------
// File:        UTNetUtils.cpp
// Author:      Kevin Bray
// Created:     11-24-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "UTNetUtils.h"

// include common.
#include "common_afx.h"

// windows includes.
#include <winsock.h>

// std c++ includes.
#include <string.h>

//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
const char* UTGetErrorString()
{
    int errorCode = WSAGetLastError();
    switch( errorCode )
    {
    case WSAEINTR:          return "WSAEINTR";
    case WSAEBADF:          return "WSAEBADF";
    case WSAEACCES:         return "WSAEACCES";
    case WSAEDISCON:        return "WSAEDISCON";
    case WSAEFAULT:         return "WSAEFAULT";
    case WSAEINVAL:         return "WSAEINVAL";
    case WSAEMFILE:         return "WSAEMFILE";
    case WSAEWOULDBLOCK:    return "WSAEWOULDBLOCK";
    case WSAEINPROGRESS:    return "WSAEINPROGRESS";
    case WSAEALREADY:       return "WSAEALREADY";
    case WSAENOTSOCK:       return "WSAENOTSOCK";
    case WSAEDESTADDRREQ:   return "WSAEDESTADDRREQ";
    case WSAEMSGSIZE:       return "WSAEMSGSIZE";
    case WSAEPROTOTYPE:     return "WSAEPROTOTYPE";
    case WSAENOPROTOOPT:    return "WSAENOPROTOOPT";
    case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
    case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
    case WSAEOPNOTSUPP:     return "WSAEOPNOTSUPP";
    case WSAEPFNOSUPPORT:   return "WSAEPFNOSUPPORT";
    case WSAEAFNOSUPPORT:   return "WSAEAFNOSUPPORT";
    case WSAEADDRINUSE:     return "WSAEADDRINUSE";
    case WSAEADDRNOTAVAIL:  return "WSAEADDRNOTAVAIL";
    case WSAENETDOWN:       return "WSAENETDOWN";
    case WSAENETUNREACH:    return "WSAENETUNREACH";
    case WSAENETRESET:      return "WSAENETRESET";
    case WSAECONNABORTED:   return "WSWSAECONNABORTEDAEINTR";
    case WSAECONNRESET:     return "WSAECONNRESET";
    case WSAENOBUFS:        return "WSAENOBUFS";
    case WSAEISCONN:        return "WSAEISCONN";
    case WSAENOTCONN:       return "WSAENOTCONN";
    case WSAESHUTDOWN:      return "WSAESHUTDOWN";
    case WSAETOOMANYREFS:   return "WSAETOOMANYREFS";
    case WSAETIMEDOUT:      return "WSAETIMEDOUT";
    case WSAECONNREFUSED:   return "WSAECONNREFUSED";
    case WSAELOOP:          return "WSAELOOP";
    case WSAENAMETOOLONG:   return "WSAENAMETOOLONG";
    case WSAEHOSTDOWN:      return "WSAEHOSTDOWN";
    case WSASYSNOTREADY:    return "WSASYSNOTREADY";
    case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
    case WSANOTINITIALISED: return "WSANOTINITIALISED";
    case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
    case WSATRY_AGAIN:      return "WSATRY_AGAIN";
    case WSANO_RECOVERY:    return "WSANO_RECOVERY";
    case WSANO_DATA:        return "WSANO_DATA";
    default: return "NO ERROR";
    }
}

//----------------------------------------------------------
hostent* UTResolveHostAddress( const char* address )
{
    // if no address was passed in, get the local IP address.
    char hostname[ 256 ];
    if ( !address || strlen( address ) == 0 ||
         _stricmp( address, "localhost" ) == 0 || strcmp( address, "127.0.0.1" ) == 0 )
    {
        if ( gethostname( hostname, 256 ) == SOCKET_ERROR )
        {
            PrintF( "Error getting local host name: %s\n", UTGetErrorString() );
            return 0;
        }
        address = hostname;
    }

    // do we have an IP address?
    hostent* intHost = 0;
    if ( !isalpha( *address ) )
    {
        // is the address a good length?
        unsigned int addrLen = ( unsigned int )strlen( address );
        if ( addrLen < 16 )
            intHost = gethostbyaddr( address, addrLen, AF_INET );
        else
        {
            PrintF( "Invalid IP address specified!\n" );
            return 0;
        }
    }
    else
    {
        // get the host by it's name.
        intHost = gethostbyname( address );
    }

    // make sure a valid host was returned.
    if ( !intHost )
        PrintF( "Error resolving host name: %s\n", UTGetErrorString() );

    // return the host value.
    return intHost;
}

//----------------------------------------------------------
void UTMakeSockAddr( sockaddr_in* addr, unsigned long ip, unsigned short port )
{
    // fill out the structure.
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = ip;
    addr->sin_port = htons( port );
}

//----------------------------------------------------------
bool UTSetSocketAsync( SOCKET socket, bool async )
{
    unsigned long noBlock = async ? 1 : 0;
    if ( ioctlsocket( socket, FIONBIO, &noBlock ) == SOCKET_ERROR )
    {
        PrintF( "Unable to make socket nonblocking: %s\n", UTGetErrorString() );
        return false;
    }

    // return true to indicate success.
    return true;
}

//----------------------------------------------------------
bool UTSetSocketBuffered( SOCKET socket, bool buffered )
{
    // enable or disable buffering on the socket.
    BOOL bOptVal = buffered ? FALSE : TRUE;
    int retval = setsockopt( socket, IPPROTO_TCP, TCP_NODELAY,
        ( const char* )&bOptVal, sizeof( bOptVal ) );

    // check for an error.
    if ( retval == SOCKET_ERROR )
    {
        PrintF( "Unable to change socket buffering status: %s\n", UTGetErrorString() );
        return false;
    }

//  retval = setsockopt( socket, SOL_SOCKET, SO_DONTROUTE,
//      ( const char* )&bOptVal, sizeof( bOptVal ) );

    // buffering options.
//  unsigned int bufSize = 1024;
//  retval = setsockopt( socket, SOL_SOCKET, SO_KEEPALIVE,
//      ( const char* )&bOptVal, sizeof( bOptVal ) );

    // return true to indicate success.
    return true;
}
