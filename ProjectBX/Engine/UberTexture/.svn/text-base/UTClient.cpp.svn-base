//----------------------------------------------------------
// File:		UTClient.cpp
// Author:		Kevin Bray
// Created:		11-23-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "ubertexture_afx.h"

// file header.
#include "UTClient.h"

// project includes.
#include "UTNetUtils.h"
#include "UTConfig.h"
#include "UTMessage.h"
#include "common_afx.h"

// windows includes.
#include <windows.h>
#include <winsock.h>
#include <mmsystem.h>

#define SafeCloseHandle( x )	{ if ( x ) { CloseHandle( x ); x = 0; } }

#define QUEUE_SIZE			64
#define SEND_BUFFER_SIZE	1048576

// Winsock data
static WSADATA _wsaData;
static SOCKET _clientSocket = INVALID_SOCKET;
static UT_message_queue_t _inQueue;
static UT_message_queue_t _outQueue;
static void* _combineBuffer = 0;

// shared buffer for communicating with the server locally.
static void* _sharedBuffer = 0;
static void* _sendBuffer = 0;
static void* _recvBuffer = 0;
static HANDLE _sharedBufferFileMapping = INVALID_HANDLE_VALUE;
static HANDLE _sendMutex = 0;
static HANDLE _recvMutex = 0;

// send/recv procedures.
static SendProc _sendProc = 0;
static RecvProc _recvProc = 0;
static IoctlProc _ioctlProc = 0;

static const char _localhostIP[] = "127.0.0.1";

// local send/recv functions used to bypass winsock.
static int __stdcall SendLocal( SOCKET s, const char* buf, int len, int flags );
static int __stdcall RecvLocal( SOCKET s, char* buf, int len, int flags );
static int __stdcall IoctlLocal( SOCKET s, long cmd, unsigned long* argp );

//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
bool UTClientConnect( const char* address, unsigned short port )
{
	// make sure we haven't already been connected!
	if ( _clientSocket != INVALID_SOCKET )
		return false;

	// make sure winsock is running.
	WORD wsaVer = MAKEWORD( 1, 1 );
	if ( WSAStartup( wsaVer, &_wsaData ) != 0 )
		return false;

	// create the socket.
	_clientSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	// set to localhost if needed.
	if ( !address || strlen( address ) == 0 || _stricmp( address, "localhost" ) == 0 )
		address = _localhostIP;

	// make the socket address.
	sockaddr_in addr;
	if ( isalpha( *address ) )
	{
		// get the host from a name.
		hostent* host = gethostbyname( address );
		if ( !host )
			return false;

		// make the socket address.
		UTMakeSockAddr( &addr, *(unsigned long *)host->h_addr_list[0], port );
	}
	else
	{
		// create the socket from an IP address.
		UTMakeSockAddr( &addr, inet_addr( address ), port );
	}

	// try to connect to the address.
	if ( connect( _clientSocket, (struct sockaddr *)&addr, sizeof( addr ) ) == SOCKET_ERROR )
	{
		closesocket( _clientSocket );
		int error = WSAGetLastError();
		error = error;
		return false;
	}

	// figure out whether or not we're connecting to this machine.
#if UT_ALLOW_LOCAL
	bool local = ( strcmp( address, _localhostIP ) == 0 );
#else
	bool local = false;
#endif

	// allocate the handshake buffer.
	char buffer[ UT_HANDSHAKE_BUFFER_SIZE ];

	// select the proper handshake message.
#ifdef _MSC_VER
	if ( local )
	{
		strcpy_s( buffer, UT_HANDSHAKE_BUFFER_SIZE, UT_HANDSHAKE_CONNECT_LOCAL );

		// create the client mutex.
		_sendMutex = CreateMutex( NULL, FALSE, UT_CLIENT_MUTEX_NAME );
	}
	else
		strcpy_s( buffer, UT_HANDSHAKE_BUFFER_SIZE, UT_HANDSHAKE_CONNECT_REMOTE );
#else
	B_ASSERT( strlen( local ? UT_HANDSHAKE_CONNECT_LOCAL : UT_HANDSHAKE_CONNECT_REMOTE ) < UT_HANDSHAKE_BUFFER_SIZE - 1 );
	strncpy( buffer, local ? UT_HANDSHAKE_CONNECT_LOCAL : UT_HANDSHAKE_CONNECT_REMOTE, UT_HANDSHAKE_BUFFER_SIZE );
	buffer[ UT_HANDSHAKE_BUFFER_SIZE - 1 ] = 0;
#endif
	// send the handshake message.
	if ( send( _clientSocket, buffer, ( int )strlen( buffer ) + 1, 0 ) == SOCKET_ERROR )
	{
		SafeCloseHandle( _sendMutex );
		closesocket( _clientSocket );
		int error = WSAGetLastError();
		error = error;
		return false;
	}

	// is the connection local or not?
	if ( local )
	{
		// if we're local, receive 4 bytes.  That is the length of
		// the named buffer.
		int sharedBufNameLength = 0;
		int length = recv( _clientSocket, ( char* )&sharedBufNameLength, sizeof( sharedBufNameLength ), 0 );
		if ( length == SOCKET_ERROR )
		{
			SafeCloseHandle( _sendMutex );
			closesocket( _clientSocket );
			return false;
		}

		// now receive the name of the shared buffer.
		char sharedBufName[ UT_HANDSHAKE_BUFFER_SIZE ];
		length = recv( _clientSocket, sharedBufName, sharedBufNameLength, 0 );
		if ( length == SOCKET_ERROR )
		{
			SafeCloseHandle( _sendMutex );
			closesocket( _clientSocket );
			return false;
		}

		// try to open the shared memory.
		_sharedBufferFileMapping = OpenFileMapping( FILE_MAP_READ | FILE_MAP_WRITE, FALSE, sharedBufName );
		if ( _sharedBufferFileMapping == INVALID_HANDLE_VALUE )
		{
			SafeCloseHandle( _sendMutex );
			closesocket( _clientSocket );
			return false;
		}

		// try to map the memory.
		_sharedBuffer = MapViewOfFile( _sharedBufferFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0,
			2 * UT_LOCAL_MEMORY_SIZE );
		if ( !_sharedBuffer )
		{
			SafeCloseHandle( _sendMutex );
			closesocket( _clientSocket );
			return false;
		}

		// set the send and receive buffer pointers.
		_sendBuffer = ( char* )_sharedBuffer + UT_LOCAL_MEMORY_SIZE;
		_recvBuffer = _sharedBuffer;

		// get the server mutex.
		_recvMutex = OpenMutex( SYNCHRONIZE, FALSE, UT_SERVER_MUTEX_NAME );

		// set the send/receive procedures to use winsock.
		_sendProc = SendLocal;
		_recvProc = RecvLocal;
		_ioctlProc = IoctlLocal;
	}
	else
	{
		// get the accept message.
		memset( buffer, 0, UT_HANDSHAKE_ACCEPT_SIZE );
		int length = recv( _clientSocket, buffer, UT_HANDSHAKE_ACCEPT_SIZE, 0 );
		if ( length == SOCKET_ERROR )
		{
			closesocket( _clientSocket );
			return false;
		}

		// make sure the handshake was valid!
		if ( strcmp( buffer, UT_HANDSHAKE_ACCEPT ) != 0 )
		{
			closesocket( _clientSocket );
			return false;
		}

		// set the send/receive procedures to use winsock.
		_sendProc = send;
		_recvProc = recv;
		_ioctlProc = ioctlsocket;
	}

	// make the socket async.
	if ( !UTSetSocketAsync( _clientSocket, true ) )
	{
		if ( local )
		{
			SafeCloseHandle( _sendMutex );
			SafeCloseHandle( _recvMutex );
		}
		closesocket( _clientSocket );
		return false;
	}

	// make the socket non-buffered.
	if ( !UTSetSocketBuffered( _clientSocket, false ) )
	{
		if ( local )
		{
			SafeCloseHandle( _sendMutex );
			SafeCloseHandle( _recvMutex );
		}
		closesocket( _clientSocket );
		return false;
	}

	// initialize the FIFOs.
	UTMessageQueueCreate( &_inQueue, QUEUE_SIZE );
	UTMessageQueueCreate( &_outQueue, QUEUE_SIZE );

	// initialize the send buffer.
	_combineBuffer = malloc( SEND_BUFFER_SIZE );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTClientDisconnect()
{
	// free the send buffer.
	free( _combineBuffer );
	_combineBuffer = 0;

	// close mutex handles.
	if ( _sendMutex )
	{
		SafeCloseHandle( _sendMutex );
		SafeCloseHandle( _recvMutex );
		_sendMutex = 0;
		_recvMutex = 0;
	}

	// do we need to release our shared buffer?
	if ( _sharedBuffer )
	{
		// unmap the view of the file.
		UnmapViewOfFile( _sharedBuffer );
		SafeCloseHandle( _sharedBufferFileMapping );

		// clear our shared memory file mapping values.
		_sharedBufferFileMapping = INVALID_HANDLE_VALUE;
		_sharedBuffer = 0;
	}

	// if we're not connected, return failure.
	if ( _clientSocket == INVALID_SOCKET )
		return false;

	UTMessageQueueDestroy( &_inQueue );
	UTMessageQueueDestroy( &_outQueue );

	// close our socket.
	closesocket( _clientSocket );

	// set the socet to the invalid-socket value.
	_clientSocket = INVALID_SOCKET;

	// call cleanup to make sure that winsock is shutdown properly.
	WSACleanup();

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
UT_message_t* UTClientQueueOutgoing()
{
	// return the new message.
	return UTMessageQueueAdd( &_outQueue );
}

//----------------------------------------------------------
UT_message_t* UTClientRemoveIncoming()
{
	// get the message and verify that it's ready.
	UT_message_t* message = UTMessageQueuePeek( &_inQueue );
	if ( message && UTMessageReady( message ) )
	{
		// return the incoming message.
		return UTMessageQueueRemove( &_inQueue );
	}

	// no message is ready.
	return 0;
}

//----------------------------------------------------------
size_t UTClientOutgoingCount()
{
	return UTMessageQueueCount( &_outQueue );
}

//----------------------------------------------------------
size_t UTClientIncomingCount()
{
	return UTMessageQueueCount( &_inQueue );
}

//----------------------------------------------------------
bool UTSyncToResponse( unsigned int transactionId, unsigned int timeToWait )
{
	unsigned int startTime = timeGetTime();

	// spin-lock until we hit a response to the transaction ID passed in.
	UT_message_t* top = 0;
	while ( !top || top->transactionId < transactionId )
	{
		// check to see if the incoming message queue is full.  If so,
		// return false to indicate failure.
		if ( UTMessageQueueFull( &_inQueue ) )
			return false;

		// update the queues so we can process incoming messages.
		UTClientUpdateQueues();

		// get the top of the message queue.
		top = UTMessageQueueTop( &_inQueue );

		// check to see if we need to time-out.
		unsigned int timeRunning = timeGetTime() - startTime;
		if ( timeRunning >= timeToWait )
			return false;
	}

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void UTClientUpdateQueues()
{
	// send all queued messages.
	UTMessageQueueSend( _clientSocket, &_outQueue, ( char* )_combineBuffer,
		SEND_BUFFER_SIZE, 0, _sendProc );

	// recv incoming messages.
	UTMessageQueueRecv( &_inQueue, _clientSocket, 0, _recvProc, _ioctlProc );
}

//----------------------------------------------------------
#pragma warning( disable : 4100 )
int __stdcall
SendLocal( SOCKET s, const char* buf, int len, int flags )
{
	WaitForSingleObject( _sendMutex, INFINITE );

	// get the read/write addresses.
	unsigned int writeAddr = ( ( unsigned int* )_sendBuffer )[ 0 ];
	unsigned int readAddr = ( ( unsigned int* )_sendBuffer )[ 1 ];

	// reset the read/write addresses.
	if ( readAddr == writeAddr )
	{
		( ( unsigned int* )_sendBuffer )[ 0 ] = writeAddr = 0;
		( ( unsigned int* )_sendBuffer )[ 1 ] = readAddr = 0;
	}

	// how much room is available in the buffer?
	unsigned int spaceRemaining = UT_LOCAL_MEMORY_SIZE - writeAddr - 8;
	len = Min( ( int )spaceRemaining, len );

	// get the starting address and write the data.
	char* dst = ( char* )_sendBuffer + 8;
	MemCopy( dst + writeAddr, buf, len );

	// update the write address.
	( ( unsigned int* )_sendBuffer )[ 0 ] += len;

	// return the size of the data copied.
	ReleaseMutex( _sendMutex );
	return len;
}
#pragma warning( default : 4100 )

//----------------------------------------------------------
#pragma warning( disable : 4100 )
int __stdcall
RecvLocal( SOCKET s, char* buf, int len, int flags )
{
	WaitForSingleObject( _recvMutex, INFINITE );

	// get the read/write addresses.
	unsigned int writeAddr = ( ( unsigned int* )_recvBuffer )[ 0 ];
	unsigned int readAddr = ( ( unsigned int* )_recvBuffer )[ 1 ];

	// read up to the write address.
	unsigned int available = writeAddr - readAddr;
	len = Min( ( int )available, len );

	// get the starting address and read the data.
	const char* src = ( const char* )_recvBuffer + 8;
	MemCopy( buf, src + readAddr, len );

	// update the read address.
	( ( unsigned int* )_recvBuffer )[ 1 ] += len;

	// return the size of the data copied.
	ReleaseMutex( _recvMutex );
	return len;
}
#pragma warning( default : 4100 )

//----------------------------------------------------------
#pragma warning( disable : 4100 )
int __stdcall
IoctlLocal( SOCKET s, long cmd, unsigned long* argp )
{
	WaitForSingleObject( _recvMutex, INFINITE );

	// get the read/write addresses.
	unsigned int writeAddr = ( ( unsigned int* )_recvBuffer )[ 0 ];
	unsigned int readAddr = ( ( unsigned int* )_recvBuffer )[ 1 ];

	// return the amount of data available.
	*argp = writeAddr - readAddr;

	// return 0 to indicate success.
	ReleaseMutex( _recvMutex );
	return 0;
}
#pragma warning( default : 4100 )
