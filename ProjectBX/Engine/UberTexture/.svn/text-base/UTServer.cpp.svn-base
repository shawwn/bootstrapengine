//----------------------------------------------------------
// File:		UTServe.cpp
// Author:		Kevin Bray
// Created:		08-26-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "ubertexture_afx.h"

// file header.
#include "UTServer.h"

// project includes.
#include "UTNetUtils.h"
#include "UTConfig.h"
#include "UTMessage.h"
#include "UTX.h"
#include "common_afx.h"

// windows includes.
#include <windows.h>
#include <winsock.h>
#include <process.h>

#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( default : 4201 )

// std c++ includes.
#include <vector>

// macro to safely close handles.
#define SafeCloseHandle( x )	{ if ( x ) { CloseHandle( x ); x = 0; } }

// debug settings.
#define PROFILE_UBERSERVER			0
#define PROFILE_MULTI_READ			0

// internal values.
#define STRING_BUFFER_LEN			32768
#define IN_QUEUE_SIZE				32
#define OUT_QUEUE_SIZE				64

// how often performance timings are updated, in milliseconds.
#define PERF_TIMING_UPDATE_SPEED	1000
#define PERF_TIMING_PRINT			0
#define SEND_BUFFER_SIZE			1048576

// connection descriptor.
struct SConnection
{
	SOCKET socket;
	unsigned int enabled;
	UT_message_queue_t inQueue;
	UT_message_queue_t outQueue;
	SendProc sendProc;
	RecvProc recvProc;
	IoctlProc ioctlProc;
	bool local;
};

// stream descriptor.
struct SStreamEntry
{
	unsigned int refCount;
	UTX_stream_t stream;
};

// true if the thread was created.
static void* _serveThread = 0;
static volatile bool _serverReady = false;
static volatile bool _run = false;
static bool _localOnly = false;

// primary socket.
SOCKET _primarySocket = INVALID_SOCKET;

static HANDLE _sendMutex = 0;
static HANDLE _recvMutex = 0;

// list of connections.
static SConnection _connections[ UT_CONNECTION_MAX ];

// uber-texture directory.
static SStreamEntry _streams[ UT_STREAMS_MAX ];

// working folder.
static int _workingFolderLen = 0;
static char* _workingFolder = 0;
static char* _strScratch = 0;
static char* _tempBuffer = 0;
static char* _combineBuffer = 0;

// local communication buffer.
static void* _localBuffer = 0;
static void* _sendBuffer = 0;
static void* _recvBuffer = 0;

// statistics, 22.10 format.
static unsigned int _bytesReadPerSec;
static unsigned int _bytesWrittenPerSec;
static unsigned int _totalBytesSentPerSec;
static unsigned int _totalBytesReceivedPerSec;
static unsigned int _readsPerSec;
static unsigned int _readsCancelledPerSec;

// statistics being accumulated.
static unsigned int _curBytesRead = 0;
static unsigned int _curBytesWritten = 0;
static unsigned int _curTotalBytesSent = 0;
static unsigned int _curTotalBytesReceived = 0;
static unsigned int _curReads = 0;
static unsigned int _curReadsCancelled = 0;
static unsigned int _lastPollTime;

// file local functions.
static int UTServe( unsigned short port );
static int UTDoServe( unsigned short port );
static bool CheckForNewConnections();
static bool UpdateConnections();
static void UpdateConnectionOutgoing( SConnection* connection );
static bool HandleMessage( SConnection* connection );
static bool HandleCreate( SConnection* connection, UT_message_t* message );
static bool HandleOpen( SConnection* connection, UT_message_t* message );
static bool HandleClose( SConnection* connection, UT_message_t* message );
static bool TryRead( SConnection* connection, UT_message_t* message );
static bool HandleRead( SConnection* connection, UT_message_t* message );
static bool HandleWrite( SConnection* connection, UT_message_t* message );
static bool TryMultiRead( SConnection* connection, UT_message_t* message );
static bool HandleMultiRead( SConnection* connection, UT_message_t* message );
static bool HandleDirListing( SConnection* connection, UT_message_t* message );
static bool HandlePing( SConnection* connection, UT_message_t* message );
static unsigned int AllocStream( char* fileName );
static bool CloseConnection( unsigned int idx );
static void CloseAllConnections();
static void Cleanup();
static unsigned int __stdcall UTServeThreadProc( void* param );

// local send/recv functions used to bypass winsock.
static int __stdcall SendLocal( SOCKET s, const char* buf, int len, int flags );
static int __stdcall RecvLocal( SOCKET s, char* buf, int len, int flags );
static int __stdcall IoctlLocal( SOCKET s, long cmd, unsigned long* argp );

//**********************************************************
// File global functions
//**********************************************************

//----------------------------------------------------------
void UTServeThreadCreate( const char* workingFolder, unsigned short port, bool localOnly )
{
	_localOnly = localOnly;

	// there can be only one server thread!
	if ( _serveThread != 0 )
		return;

	// allocate some space for the working folder.
	_workingFolderLen = ( int )strlen( workingFolder );
	_workingFolder = new char[ _workingFolderLen + 2 ];
#ifdef _MSC_VER
	strcpy_s( _workingFolder, _workingFolderLen + 2, workingFolder );
#else
	strcpy( _workingFolder, workingFolder );
#endif

	// make sure the working folder ends with an appropriate character.
	if ( _workingFolder[ _workingFolderLen-1 ] != '/' &&
		 _workingFolder[ _workingFolderLen-1 ] != '\\' )
	{
		_workingFolder[ _workingFolderLen ] = '/';
		_workingFolder[ ++_workingFolderLen ] = '\0';
	}

	// allocate space for the string composition buffer.
	_strScratch = new char[ STRING_BUFFER_LEN ];
#ifdef _MSC_VER
	strcpy_s( _strScratch, STRING_BUFFER_LEN, _workingFolder );
#else
	B_ASSERT( strlen( _workingFolder ) < STRING_BUFFER_LEN - 1 );
	strncpy( _strScratch, _workingFolder, STRING_BUFFER_LEN );
	_strScratch[ STRING_BUFFER_LEN - 1] = 0;
#endif

	// allocate the temporary buffer.
	_tempBuffer = new char[ UT_MAX_MESSAGE_SIZE ];

	// allocate the send buffer.
	_combineBuffer = new char[ SEND_BUFFER_SIZE ];

	// create the thread.
	_run = true;
	unsigned int threadId = 0;
	_serveThread = ( void* )_beginthreadex( NULL, 32768, UTServeThreadProc, ( void* )( size_t )port, 0, &threadId );

	// spin while the server starts up.
	while ( !_serverReady )
		Sleep( 0 );
}

//----------------------------------------------------------
void UTServeThreadClose()
{
	// set the running flag to false... this notifies the server to
	// terminate.
	_run = false;

	// close the thread.
	if ( _serveThread )
	{
		// wait for the thread to end and then close it's handle.
		WaitForSingleObject( _serveThread, INFINITE );
		SafeCloseHandle( ( HANDLE )_serveThread );
	}

	// free up resources.
	delete[] _combineBuffer;
	delete[] _workingFolder;
	delete[] _strScratch;
	delete[] _tempBuffer;

	// clear the server ready flag.
	_serveThread = 0;
	_serverReady = false;
}


//**********************************************************
// File local functions
//**********************************************************

//----------------------------------------------------------
int UTServe( unsigned short port )
{
	// start 1ms timing precision.
	timeBeginPeriod( 1 );

	// start up Winsock.
	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0 )
		return -1;

	// create a named memory share.
	HANDLE store = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT,
		0, 2 * UT_LOCAL_MEMORY_SIZE, UT_LOCAL_MEMORY_NAME );
	_localBuffer = MapViewOfFile( store, FILE_MAP_ALL_ACCESS, 0, 0, 2 * UT_LOCAL_MEMORY_SIZE );
	_sendBuffer = _localBuffer;
	_recvBuffer = ( char* )_localBuffer + UT_LOCAL_MEMORY_SIZE;

	// serve.
	int retVal = UTDoServe( port );

	// unmap the shared memory.
	UnmapViewOfFile( _localBuffer );
	_localBuffer = 0;

	// close our named memory share.
	SafeCloseHandle( store );

	// shutdown Winsock.
	WSACleanup();

	// end 1ms timing precision.
	timeEndPeriod( 1 );

	// return the value that UTDoServe returned.
	return retVal;
}

//----------------------------------------------------------
static int UTDoServe( unsigned short port )
{
	// clear statistics.
	_lastPollTime = timeGetTime();
	_curBytesRead = 0;
	_curBytesWritten = 0;
	_curTotalBytesSent = 0;
	_curTotalBytesReceived = 0;

	// timeout time value.
	timeval noTimeout;
	noTimeout.tv_sec = 0;
	noTimeout.tv_usec = 0;

	// create the socket.
	_primarySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	// prevent the intermittent  "Address already in use" error message.
	char yes = 1;
	if ( setsockopt( _primarySocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) 
	{
		PrintF( "Error setting a socket option! %s\n", UTGetErrorString() );
		closesocket( _primarySocket );
		return -1;
	} 

	// bind the socket to our local address.
	sockaddr_in localAddr;
	UTMakeSockAddr( &localAddr, INADDR_ANY, port );
	if ( bind( _primarySocket, ( sockaddr* )&localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		PrintF( "Error binding socket! %s\n", UTGetErrorString() );
		closesocket( _primarySocket );
		return -1;
	}

	// set the socket to non-blocking.
	if ( !UTSetSocketAsync( _primarySocket, true ) )
	{
		// did an error occur?
		closesocket( _primarySocket );
		return -1;
	}

	// set the socket to non-buffered.
	if ( !UTSetSocketBuffered( _primarySocket, false ) )
	{
		// did an error occur?
		closesocket( _primarySocket );
		return -1;
	}

	// put the primary socket into a listening state.
	if ( listen( _primarySocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		closesocket( _primarySocket );
		PrintF( "Unable to put the socket into a listening state! %s\n", UTGetErrorString() );
		return -1;
	}

	// set the server ready flag to true.
	_serverReady = true;

	// initialize the array of connections.
	for ( unsigned int i = 0; i < UT_CONNECTION_MAX; ++i )
	{
		_connections[ i ].socket = INVALID_SOCKET;
		_connections[ i ].enabled = 0;
	}

	// initialize the array of streams.
	for ( unsigned int i = 0; i < UT_STREAMS_MAX; ++i )
	{
		_streams[ i ].refCount = 0;
		UTX_InitStream( &_streams[ i ].stream );
	}

	// handle our update loop.
	unsigned int time = timeGetTime();
	unsigned int prevConnCheckTime = timeGetTime();
	while ( _run )
	{
		// reduce CPU consumption.
		Sleep(1);

		// check for new connections.
		if ( !_localOnly || _connections[ 0 ].enabled == 0 )
		{
			// todo: put in a windows message loop.
			unsigned int curTime = timeGetTime();
		
			// check for any incoming connections.
			if ( ( curTime - prevConnCheckTime ) > 1000 )
			{
				prevConnCheckTime = curTime;
				if ( !CheckForNewConnections() )
				{
					Cleanup();
					return -1;
				}
			}
		}

		// handle any incoming data.
		if ( !UpdateConnections() )
		{
			Cleanup();
			return -1;
		}

		// get the current time delta.
		unsigned int curTime = timeGetTime();

		// if we're running too fast, yield to other threads.
		unsigned int delta = curTime - time;
		if ( delta < UT_UPDATE_INTERVAL )
		{
			Sleep( UT_UPDATE_INTERVAL - delta );
			time = timeGetTime();
		}
		else
			time = curTime;
	}

	// clean up and return 0 to indicate success.
	Cleanup();
	return 0;
}

//----------------------------------------------------------
bool CheckForNewConnections()
{
	// simply check for and accept any incoming connections.
	sockaddr_in newConn;
	int newConnSize = sizeof( sockaddr_in );
	SOCKET newSock = accept( _primarySocket, ( sockaddr* )&newConn, &newConnSize );
	if ( newSock == SOCKET_ERROR )
	{
		int error = WSAGetLastError();
		if ( error != WSAEWOULDBLOCK )
		{
			PrintF( "Failed to accept a connection! %s\n", UTGetErrorString() );
			return false;
		}
	}
	else
	{
		// try to set the socket to non-buffered.
		UTSetSocketBuffered( newSock, false );

		// look for a free connection slot and add the socket.
		unsigned int i = 0;
		for ( ; i < UT_CONNECTION_MAX; ++i )
		{
			if ( _connections[ i ].socket == INVALID_SOCKET )
			{
				// create the new connection.
				_connections[ i ].socket = newSock;
				_connections[ i ].enabled = 0;
				_connections[ i ].sendProc = send;
				_connections[ i ].recvProc = recv;
				_connections[ i ].ioctlProc = ioctlsocket;
				UTMessageQueueCreate( &_connections[ i ].inQueue, IN_QUEUE_SIZE );
				UTMessageQueueCreate( &_connections[ i ].outQueue, OUT_QUEUE_SIZE );
				break;
			}
		}

		// if nothing is free, refuse the connection and close the socket.
		if ( i == UT_CONNECTION_MAX )
			closesocket( newSock );
	}

	// success!
	return true;
}

//----------------------------------------------------------
bool UpdateConnections()
{
	// update statistics.
	unsigned int curTime = timeGetTime();
	unsigned int statDelta = curTime - _lastPollTime;
	if ( statDelta > PERF_TIMING_UPDATE_SPEED )
	{
		// calculate the per-second statistics.
		_bytesReadPerSec = ( _curBytesRead * statDelta ) / 1000;
		_bytesWrittenPerSec = ( _curBytesWritten * statDelta ) / 1000;
		_totalBytesSentPerSec = ( _curTotalBytesSent * statDelta ) / 1000;
		_totalBytesReceivedPerSec = ( _curTotalBytesReceived * statDelta ) / 1000;
		_readsPerSec = ( _curReads * statDelta ) / 1000;
		_readsCancelledPerSec = ( _curReadsCancelled * statDelta ) / 1000;
		_lastPollTime = curTime;

#if PERF_TIMING_PRINT
		PrintF( "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n" );
		PrintF( "[UberServer] Total bytes read per second:      %d\n", _bytesReadPerSec );
		PrintF( "[UberServer] Total written read per second:    %d\n", _bytesWrittenPerSec );
		PrintF( "[UberServer] Total bytes sent per second:      %d\n", _totalBytesSentPerSec );
		PrintF( "[UberServer] Total bytes received per second:  %d\n", _totalBytesReceivedPerSec );
		PrintF( "[UberServer] Total reads per second:           %d\n", _readsPerSec );
		PrintF( "[UberServer] Total reads cancelled per second: %d\n", _readsCancelledPerSec );
#endif

		// clear the current results.
		_curBytesRead = 0;
		_curBytesWritten = 0;
		_curTotalBytesSent = 0;
		_curTotalBytesReceived = 0;
		_curReads = 0;
		_curReadsCancelled = 0;
	}

	// update all active connections.
	for ( unsigned int i = 0; i < UT_CONNECTION_MAX; ++i )
	{
		// get the current connection.
		SConnection& connection = _connections[ i ];

		// skip any uninitialized sockets.
		if ( connection.socket == INVALID_SOCKET )
			continue;

		// is data waiting?
		unsigned long dataWaiting = 0;
		if ( connection.ioctlProc( connection.socket, FIONREAD, &dataWaiting ) == SOCKET_ERROR )
		{
			PrintF( "Failed to poll socket for data! %s\n", UTGetErrorString() );
			return false;
		}

		// if incoming data is waiting, handle it.
		if ( dataWaiting > 0 )
		{
			// has the socket been enabled yet?
			if ( connection.enabled == 0 && dataWaiting >= UT_HANDSHAKE_CONNECT_SIZE )
			{
				// check for a hand shake.
				char buffer[ 32 ];
				recv( connection.socket, buffer, 8, 0 );

				// look for the handshake.
				if ( strcmp( buffer, UT_HANDSHAKE_CONNECT_REMOTE ) == 0 )
				{
					// send notice of the connection acceptance.
					if ( send( connection.socket, UT_HANDSHAKE_ACCEPT, UT_HANDSHAKE_ACCEPT_SIZE, 0 ) == SOCKET_ERROR )
					{
						// post a failure notice and close this connection.
						PrintF( "Failed to notify a client of connection acceptance! %s\n", UTGetErrorString() );
						CloseConnection( i );
						continue;
					}
					connection.enabled = 1;
					connection.local = false;

					// set the send/recv procs.
					connection.sendProc = send;
					connection.recvProc = recv;
					connection.ioctlProc = ioctlsocket;
				}
				else if ( strcmp( buffer, UT_HANDSHAKE_CONNECT_LOCAL ) == 0 )
				{
					// if we have a local connection, create the server mutex.
					_sendMutex = CreateMutex( NULL, FALSE, UT_SERVER_MUTEX_NAME );

					// send the length of the name of the memory share.
					int length = ( int )strlen( UT_LOCAL_MEMORY_NAME ) + 1;
					if ( send( connection.socket, ( const char* )&length, sizeof( length ), 0 ) == SOCKET_ERROR )
					{
						// post a failure notice and close this connection.
						PrintF( "Failed to notify a client of connection acceptance! %s\n", UTGetErrorString() );
						CloseConnection( i );
						continue;
					}

					// return the name of the memory share back to the client.
					if ( send( connection.socket, UT_LOCAL_MEMORY_NAME, UT_LOCAL_MEMORY_NAME_SIZE, 0 ) == SOCKET_ERROR )
					{
						// post a failure notice and close this connection.
						PrintF( "Failed to notify a client of connection acceptance! %s\n", UTGetErrorString() );
						CloseConnection( i );
						continue;
					}
					connection.enabled = 1;
					connection.local = true;

					// set the send/recv procs.
					connection.sendProc = SendLocal;
					connection.recvProc = RecvLocal;
					connection.ioctlProc = IoctlLocal;

					// get the client mutex.
					_recvMutex = OpenMutex( SYNCHRONIZE, FALSE, UT_CLIENT_MUTEX_NAME );

					// initialize the read/write offsets to 0.
					memset( _sendBuffer, 0, 8 );
					memset( _recvBuffer, 0, 8 );
				}
				else
				{
					// no clue who you are.
					CloseConnection( i );
				}
			}
			else if ( connection.enabled != 0 )
			{
				// process all waiting data.
				int bytesReceived = 0;
				UTMessageQueueRecv( &connection.inQueue, connection.socket, &bytesReceived,
					connection.recvProc, connection.ioctlProc );

				// if we received a message, increment the total number of bytes
				// received.
				_curTotalBytesReceived += bytesReceived;
			}
		}

		// handle all messages waiting in our incoming queue.
		size_t count = UTMessageQueueCount( &connection.inQueue );
		count = Min( count, (size_t)5 );
		while ( count-- > 0 )
		{
			if ( !HandleMessage( _connections + i ) )
				break;
		}

		// now send everything in the outgoing queue.
		UpdateConnectionOutgoing( &connection );
	}

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void UpdateConnectionOutgoing( SConnection* connection )
{
	// now send everything in the outgoing queue.
	int bytesSent = 0;
	UTMessageQueueSend( connection->socket, &connection->outQueue,
		_combineBuffer, SEND_BUFFER_SIZE, &bytesSent, connection->sendProc );
	_curTotalBytesSent += bytesSent;
}

//----------------------------------------------------------
bool HandleMessage( SConnection* connection )
{
	// get the oldest incoming message from the connection.
	UT_message_t* message = UTMessageQueuePeek( &connection->inQueue );
	if ( !message || !UTMessageReady( message ) )
		return false;

	// check to see if we can handle this message.  If the outgoing queue is
	// full, then we should simply ignore this command.
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// we know we can fulfill the request, so lets go ahead and do that.
	bool ret = true;
	switch ( message->code )
	{
	case UTMSG_CLIENT_CREATE:
		ret = HandleCreate( connection, message );
		break;

	case UTMSG_CLIENT_OPEN:
		ret = HandleOpen( connection, message );
		break;

	case UTMSG_CLIENT_FINALIZE:
		break;

	case UTMSG_CLIENT_CLOSE:
		ret = HandleClose( connection, message );
		break;

	case UTMSG_CLIENT_READ:
		ret = TryRead( connection, message );
		break;

	case UTMSG_CLIENT_READCANCEL:
		break;

	case UTMSG_CLIENT_MULTI_READ:
		ret = TryMultiRead( connection, message );
		break;

	case UTMSG_CLIENT_WRITE:
		ret = HandleWrite( connection, message );
		break;

	case UTMSG_CLIENT_DIR_LISTING:
		ret = HandleDirListing( connection, message );
		break;

	case UTMSG_CLIENT_PING:
		ret = HandlePing( connection, message );
	};

	// we've handled the incoming request, so remove the message.
	UTMessageQueueRemove( &connection->inQueue );

	// do we need to add a failure message?
	if ( !ret )
	{
		// add the message.
		UT_message_t* error = UTMessageQueueAdd( &connection->outQueue );
		if ( error )
		{
			UTMessageWriteBegin( error, UTMSG_ERROR, message->transactionId );
			UTMessageWriteEnd( error );
		}
	}

	// return true to indicate success.
	return ret;
}

//----------------------------------------------------------
bool HandleCreate( SConnection* connection, UT_message_t* message )
{
	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// read the width.
	int width = 0;
	if ( !UTMessageReadInt( &width, message ) )
		return false;

	// read the height.
	int height = 0;
	if ( !UTMessageReadInt( &height, message ) )
		return false;

	// read the stage count.
	unsigned int stageCount = 0;
	if ( !UTMessageReadUInt( &stageCount, message ) )
		return false;

	// read the flags.
	UTFlags* flags = ( UTFlags* )_tempBuffer;
	for ( unsigned int i = 0; i < stageCount; ++i )
	{
		if ( !UTMessageReadUInt( flags + i, message ) )
			return false;
	}

	// read the channel count.
	int* channelCounts = ( int* )( _tempBuffer + ( UT_MAX_MESSAGE_SIZE >> 1 ) );
	for ( unsigned int i = 0; i < stageCount; ++i )
	{
		if ( !UTMessageReadInt( channelCounts + i, message ) )
			return false;
	}

	// read the file name from the message.
	if ( !UTMessageReadStr( _strScratch + _workingFolderLen, STRING_BUFFER_LEN - _workingFolderLen, message ) )
		return false;

	// get the stream ID.
	unsigned int streamID = AllocStream( _strScratch );

	// get the stream and increment it's reference count.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	_streams[ streamID ].refCount += 1;

	// if a stream currently exists, close it and create a new file.
	if ( UTX_IsStream( stream ) )
		UTX_CloseStream( stream );

	// now try to create the stream.
	if ( !UTX_CreateStream( stream, _strScratch, width, height, stageCount, flags, channelCounts ) )
		return false;

	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	UTMessageWriteBegin( response, message->code, message->transactionId );
	UTMessageWriteUInt( response, streamID );
	UTMessageWriteEnd( response );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool HandleOpen( SConnection* connection, UT_message_t* message )
{
	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// read the file name from the message.
	if ( !UTMessageReadStr( _strScratch + _workingFolderLen, STRING_BUFFER_LEN - _workingFolderLen, message ) )
		return false;

	// get the stream ID.
	unsigned int streamID = AllocStream( _strScratch );

	// open the stream if it's not already open.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	if ( _streams[ streamID ].refCount == 0 )
	{
		if ( !UTX_OpenStream( stream, _strScratch ) )
			return false;
	}
	_streams[ streamID ].refCount += 1;

	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	UTMessageWriteBegin( response, message->code, message->transactionId );
	UTMessageWriteUInt( response, streamID );
	UTMessageWriteInt( response, UTX_GetWidth( stream ) );
	UTMessageWriteInt( response, UTX_GetHeight( stream ) );
	UTMessageWriteInt( response, UTX_NumStages( stream ) );
	UTMessageWriteEnd( response );

	// return true to indicate success.
	return true;
}

#pragma warning( disable : 4100 )
//----------------------------------------------------------
bool HandleClose( SConnection* connection, UT_message_t* message )
{
	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// get the stream ID.
	unsigned int streamID = 0;
	if ( !UTMessageReadInt( ( int* )&streamID, message ) )
		return false;
	if ( streamID >= UT_STREAMS_MAX )
		return false;

	// get the current stream and make sure it's valid.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	if ( _streams[ streamID ].refCount == 0 )
		return false;

	// decrement the reference count.
	_streams[ streamID ].refCount -= 1;

	// did the reference count hit zero?  If so, close the stream.
	if ( _streams[ streamID ].refCount == 0 )
		UTX_CloseStream( stream );

	// return true to indicate success.
	return true;
}
#pragma warning( default : 4100 )

//----------------------------------------------------------
bool TryRead( SConnection* connection, UT_message_t* message )
{
	// did the read go okay?
	bool didRead = HandleRead( connection, message );
	if( didRead )
		return true;

	// no, so send an error message.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if( !response ) 
	{
		// must indicate success during an error, since the tile data can't be echoed.
		return true; 
	}

	// streamID, x, y, mip
	int tileInfoSize = sizeof( int ) * 4;

	// echo the tile data back to the client.
	UTMessageReadBegin( message );
	UTMessageWriteBegin( response, UTMSG_ERROR, message->transactionId );
	UTMessageWriteBytes( response, UTMessageGetData( message, tileInfoSize ), tileInfoSize );
	UTMessageWriteEnd( response );
	UTMessageReadEnd( message );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool HandleRead( SConnection* connection, UT_message_t* message )
{
#if PROFILE_UBERSERVER
	unsigned int startTime = timeGetTime();
#endif

	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// look for any read cancel messages.
	size_t count = UTMessageQueueCount( &connection->inQueue );
	for ( size_t i = 0; i < count; ++i )
	{
		// get the current message.
		UT_message_t* cur = UTMessageQueuePeek( &connection->inQueue, i );

		// is the message a read cancel?
		if ( cur->code == UTMSG_CLIENT_READCANCEL )
		{
			// begin reading from the current message to get it's id.
			if ( !UTMessageReadBegin( cur ) )
				continue;

			// get the transaction id to cancel.
			unsigned int idToCancel = 0;
			UTMessageReadUInt( &idToCancel, cur );
			UTMessageReadEnd( cur );

			// if the read is supposed to cancel this transaction, then
			// simply return success and ignore this message.
			if ( idToCancel == message->transactionId )
			{
				++_curReadsCancelled;
				return true;
			}
		}
	}

	++_curReads;

	// get the stream ID.
	unsigned int streamID = 0;
	if ( !UTMessageReadInt( ( int* )&streamID, message ) )
		return false;
	if ( streamID >= UT_STREAMS_MAX )
		return false;

	// get the current stream and make sure it's valid.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	if ( !UTX_IsStream( stream ) )
		return false;

	// get the tile info from the message.
	int x = 0;
	if ( !UTMessageReadInt( &x, message ) )
		return false;

	int y = 0;
	if ( !UTMessageReadInt( &y, message ) )
		return false;

	int m = 0;
	if ( !UTMessageReadInt( &m, message ) )
		return false;

	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
		return false;

	// echo the tile data back to the client.
	if( !UTMessageWriteInt( response, streamID ) )
		return false;

	if( !UTMessageWriteInt( response, x ) )
		return false;

	if( !UTMessageWriteInt( response, y ) )
		return false;

	if( !UTMessageWriteInt( response, m ) )
		return false;

	// read the tile data into the message.
	int tileSize = 0;
	if ( !UTX_ReadTile( stream, _tempBuffer, x, y, m, &tileSize ) )
		return false;
	_curBytesRead += tileSize;

	// compress the tile data into the message.
	if ( !UTMessageCompressData( response, _tempBuffer, tileSize ) )
		return false;

	// we're done building the response.
	UTMessageWriteEnd( response );

#if PROFILE_UBERSERVER
	unsigned int endTime = timeGetTime();
	unsigned int totalTime = endTime - startTime;
	PrintF( "Uber-texture read occured, total time: %d\n", totalTime );
#endif

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool TryMultiRead( SConnection* connection, UT_message_t* message )
{
	// attempt to read multiple tiles at once.
	bool didRead = HandleMultiRead( connection, message );
	if( didRead )
		return true;

	// NOTES: Should we send an error message for every tile?

	// send an error message.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if( !response )
	{
		// must indicate success during an error, since the tile
		// data can't be echoed.
		return true;
	}

	// send a multiread error as a response.
	UTMessageWriteBegin( response, UTMSG_MULTI_READ_ERROR, message->transactionId );
	UTMessageWriteEnd( response );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool HandleMultiRead( SConnection* connection, UT_message_t* message )
{
#if PROFILE_UBERSERVER
	unsigned int startTime = timeGetTime();
#endif
	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// get the stream ID.
	unsigned int streamID = 0;
	if ( !UTMessageReadUInt( &streamID, message ) )
		return false;
	if ( streamID >= UT_STREAMS_MAX )
		return false;

	// get the current stream and make sure it's valid.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	if ( !UTX_IsStream( stream ) )
		return false;

	// get the number of tiles requested.
	int tileCount = 0;
	if ( !UTMessageReadInt( &tileCount, message ) )
		return false;

#if PROFILE_MULTI_READ
	unsigned int totalTime = 0;
	unsigned int startTime = timeGetTime();
	unsigned int initTileCount = tileCount;
	unsigned int startBytesRead = _curBytesRead;
	unsigned int startBytesSent = _curTotalBytesSent;
#endif

	// read in as many tiles as possible.
	unsigned int* tilesReq = ( unsigned int* )UTMessageGetData( message, tileCount * sizeof( unsigned int ) );
	unsigned int* curReq = tilesReq;
	while ( tileCount > 0 )
	{
		// determine how many tiles we can deal with in this
		// single read.
		unsigned int tilesToHandle = Min( tileCount, UT_MULTI_READ_MAX );

		// now sort the tiles for optimal order.
		UTX_OptimizeReadOrder( stream, curReq, tilesToHandle );

		// now issue lots of reads.
		for ( unsigned int i = 0; i < tilesToHandle; ++i )
		{
			// unpack the tile info.
			int m = ( curReq[ i ] & 0xFF );
			int x = ( curReq[ i ] >>  8 ) & 0xFFF;
			int y = ( curReq[ i ] >> 20 ) & 0xFFF;

			// allocate a message from the response queue.
			UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
			while( !response )
			{
				// give up our time slice.
				Sleep( 1 );

				// try to send data in the outgoing queue.
				UpdateConnectionOutgoing( connection );

				// look for another response.
				response = UTMessageQueueAdd( &connection->outQueue );
			}

			// write the response data.
			if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
				return false;

			// echo the tile data back to the client.
			if( !UTMessageWriteInt( response, streamID ) )
				return false;

			if( !UTMessageWriteInt( response, x ) )
				return false;

			if( !UTMessageWriteInt( response, y ) )
				return false;

			if( !UTMessageWriteInt( response, m ) )
				return false;

			// read the tile data into the message.
			int tileSize = 0;
			if ( !UTX_ReadTile( stream, _tempBuffer, x, y, m, &tileSize ) )
				return false;
			_curBytesRead += tileSize;

			// compress the tile data into the message.
			if ( !UTMessageCompressData( response, _tempBuffer, tileSize ) )
				return false;

			// we're done building the response.
			UTMessageWriteEnd( response );

			// now send everything in the outgoing queue.
			UpdateConnectionOutgoing( connection );
		}

		// advance past the tiles we just requested.
		curReq += tilesToHandle;

		// adjust the tile count.
		tileCount -= tilesToHandle;
	}

#if PROFILE_MULTI_READ
	totalTime = timeGetTime() - startTime;
	if ( totalTime == 0 )
		totalTime = 1;
	double timeInSec = ( totalTime / 1000.0f );
	float tilesPerSec = ( float )( initTileCount / timeInSec );
	float mbPerSec = ( float )( ( ( _curBytesRead - startBytesRead ) / timeInSec ) / 1048576.0 );
	float bytesSentPerSec = ( float )( ( ( _curTotalBytesSent - startBytesSent ) / timeInSec ) / 1024.0 );

	PrintF( "Uber-texture multi-read occured.\n" );
	PrintF( "  Total time:  %d\n", totalTime );
	PrintF( "  Tile count:  %d\n", initTileCount );
	PrintF( "  Tiles/sec:   %.2f\n", tilesPerSec );
	PrintF( "  MB read/sec: %.2f\n", mbPerSec );
	PrintF( "  KB sent/sec: %.2f\n", bytesSentPerSec );
#endif

#if PROFILE_UBERSERVER
	unsigned int endTime = timeGetTime();
	unsigned int totalTime = endTime - startTime;
	PrintF( "Uber-texture read occured, total time: %d\n", totalTime );
#endif

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool HandleWrite( SConnection* connection, UT_message_t* message )
{
	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	unsigned int streamID = 0;
	if ( !UTMessageReadInt( ( int* )&streamID, message ) )
		return false;
	if ( streamID >= UT_STREAMS_MAX )
		return false;

	// get the current stream and make sure it's valid.
	UTX_stream_t* stream = &_streams[ streamID ].stream;
	if ( !UTX_IsStream( stream ) )
		return false;

	// get the tile info from the message.
	int x = 0;
	if ( !UTMessageReadInt( &x, message ) )
		return false;

	int y = 0;
	if ( !UTMessageReadInt( &y, message ) )
		return false;

	int m = 0;
	if ( !UTMessageReadInt( &m, message ) )
		return false;

	// calculate the total size of the incoming data for the current
	// texture.
	int stageCount = UTX_NumStages( stream );
	int channelCount = 0;
	for ( int stage = 0; stage < stageCount; ++stage )
		channelCount += UTX_NumStageChannels( stream, stage );

	// decompress the tile data from the message.
	if ( !UTMessageDecompressData( _tempBuffer, channelCount * UTX_CHANNEL_SIZE_IN_BYTES, message ) )
		return false;

	// write the data to the ubertexture.
	int bytesWritten = 0;
	if ( !UTX_WriteTile( stream, x, y, m, _tempBuffer, &bytesWritten ) )
		return false;
	_curBytesWritten += bytesWritten;

	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
		return false;

	// we're done building the response.
	UTMessageWriteEnd( response );

	// finish reading from the message.
	UTMessageReadEnd( message );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool HandleDirListing( SConnection* connection, UT_message_t* message )
{
	// are we able to respond?
	if ( UTMessageQueueFull( &connection->outQueue ) )
		return false;

	// begin reading from the message.
	if ( !UTMessageReadBegin( message ) )
		return false;

	// read the scene name from the message.
	if ( !UTMessageReadStr( _strScratch + _workingFolderLen, STRING_BUFFER_LEN - _workingFolderLen, message ) )
		return false;

	// add the /*.* to the path specified for use with FindFirstFile.
	strcat_s( _strScratch, STRING_BUFFER_LEN, "/*.*" );

	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
		return false;

	// get the number of files under the folder requested.
	unsigned int fileCount = 0;
	WIN32_FIND_DATA findData;
	HANDLE findHandle = FindFirstFile( _strScratch, &findData );
	if ( findHandle != INVALID_HANDLE_VALUE )
	{
		// get the number of files in the current folder.
		do {
			if ( strcmp( findData.cFileName, ".." ) != 0 && strcmp( findData.cFileName, "." ) != 0 )
				++fileCount;		
		} while ( FindNextFile( findHandle, &findData ) );
	}

	// store the folder count in the first outgoing message.
	UTMessageWriteUInt( response, fileCount );

	// get a list of all directories under the folder requested.
	unsigned int sentFileCount = 0;
	findHandle = FindFirstFile( _strScratch, &findData );
	if ( findHandle != INVALID_HANDLE_VALUE )
	{
		do
		{
			// skip anything we shouldn't list.
			if ( strcmp( findData.cFileName, ".." ) == 0 || strcmp( findData.cFileName, "." ) == 0 )
				continue;

			// try to add the current string to the current message.
			if ( !UTMessageWriteStr( response, findData.cFileName ) )
			{
				// end the current message.
				UTMessageWriteEnd( response );

				// send the current message so that it's removed from the
				// outgoing queue.
				if ( UTMessageQueueFull( &connection->outQueue ) )
				{
					int bytesSent = 0;
					UTMessageQueueSend( connection->socket, &connection->outQueue,
						_combineBuffer, SEND_BUFFER_SIZE, &bytesSent, connection->sendProc );
					_curTotalBytesSent += bytesSent;
				}

				// error.
				if ( UTMessageQueueFull( &connection->outQueue ) )
					return false;

				// send another message.
				response = UTMessageQueueAdd( &connection->outQueue );
				if ( !response )
					return false;
				if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
					return false;
				if ( !UTMessageWriteStr( response, findData.cFileName ) )
					return false;
			}

			// add to the sent file count.
			++sentFileCount;

		} while ( FindNextFile( findHandle, &findData ) );
	}

	// close the file find.
	FindClose( findHandle );

	// we're done building the response.
	UTMessageWriteEnd( response );

	// finish reading from the message.
	UTMessageReadEnd( message );

	// return true to indicate success.
	return sentFileCount == fileCount;
}

//----------------------------------------------------------
bool HandlePing( SConnection* connection, UT_message_t* message )
{
	// allocate a message from the response queue.
	UT_message_t* response = UTMessageQueueAdd( &connection->outQueue );
	if ( !response )
		return false;

	// write the response data.
	if ( !UTMessageWriteBegin( response, message->code, message->transactionId ) )
		return false;

	// we're done building the response.
	UTMessageWriteEnd( response );

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
unsigned int AllocStream( char* fileName )
{
	// hash the name to find a stream.
	unsigned int len = ( unsigned int )strlen( fileName );

	// prepare the name.
	for ( int i = 0; i < ( int )len; ++i )
	{
		fileName[ i ] = ( char )tolower( fileName[ i ] );
		if ( fileName[ i ] == '\\' )
			fileName[ i ] = '/';
	}

	// hash the name.
	unsigned int hash = HashMem32( fileName, len );
	unsigned int idx = hash % UT_STREAMS_MAX;

	// check for collisions.
	int count = UT_STREAMS_MAX;
	while ( _streams[ idx ].refCount > 0 && count-- > 0 )
	{
		// TODO-KEVIN: Check this for correctness.. I added a "stream.fileName" null check.
		// refCount was > 0, but "stream.fileName" was null, so this was crashing.

		if( _streams[idx].stream.fileName != 0 )
		{
			// already open?
			if ( strcmp( fileName, _streams[ idx ].stream.fileName ) == 0 )
				return idx;
		}

		// advance the index.
		idx += 1;
		idx %= UT_STREAMS_MAX;
	}

	// check count... if it's negative, then the stream table is full.
	if ( count < 0 )
		return ~0U;

	// if we got here, then we found a valid stream slot.
	return idx;
}

//----------------------------------------------------------
bool CloseConnection( unsigned int idx )
{
	// invalid index specified!
	if ( idx >= UT_CONNECTION_MAX )
		return false;

	// close the socket if necessary.
	if ( _connections[ idx ].socket != INVALID_SOCKET )
	{
		closesocket( _connections[ idx ].socket );
		_connections[ idx ].socket = INVALID_SOCKET;
		_connections[ idx ].enabled = 0;
		if ( _connections[ idx ].local )
		{
			SafeCloseHandle( _sendMutex );
			SafeCloseHandle( _recvMutex );
		}
		UTMessageQueueDestroy( &_connections[ idx ].inQueue );
		UTMessageQueueDestroy( &_connections[ idx ].outQueue );
	}

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void CloseAllConnections()
{
	for ( unsigned int i = 0; i < UT_CONNECTION_MAX; ++i )
	{
		if ( _connections[ i ].socket != INVALID_SOCKET )
		{
			closesocket( _connections[ i ].socket );
			_connections[ i ].socket = INVALID_SOCKET;
			_connections[ i ].enabled = 0;
			UTMessageQueueDestroy( &_connections[ i ].inQueue );
			UTMessageQueueDestroy( &_connections[ i ].outQueue );
		}
	}
}

//----------------------------------------------------------
void Cleanup()
{
	// close all sockets.
	CloseAllConnections();
	closesocket( _primarySocket );

	// initialize the array of streams.
	for ( unsigned int i = 0; i < UT_STREAMS_MAX; ++i )
		UTX_CloseStream( &_streams[ i ].stream );
}

//----------------------------------------------------------
static unsigned int __stdcall UTServeThreadProc( void* param )
{
	return ( unsigned int )UTServe( ( unsigned short )( size_t )param );
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
