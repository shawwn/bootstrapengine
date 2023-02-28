//----------------------------------------------------------
// File:		UTMessage.h
// Author:		Kevin Bray
// Created:		11-23-07
//
// Purpose:		To provide an interface for simple networking.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UTConfig.h"
#include "UTX.h"

// system includes.
#include <winsock.h>

typedef int ( __stdcall *SendProc )( SOCKET s, const char* buf, int len, int flags );
typedef int ( __stdcall *RecvProc )( SOCKET s, char* buf, int len, int flags );
typedef int ( __stdcall *IoctlProc )( SOCKET s, long cmd, unsigned long* argp );

// message structure.
struct UT_message_t
{
	unsigned int code;
	unsigned int dataSize;
	unsigned int transactionId;
	char buffer[ UT_MAX_MESSAGE_SIZE ];
	unsigned int offset;
};

// circular message queue structure.
struct UT_message_queue_t
{
	UT_message_t* messages;
	size_t head;
	size_t tail;
	size_t size;
};

// buffered message queue.
struct UT_buffered_message_queue_t
{
	UT_message_queue_t queue;
	void* buffer;
};

// this is an invalid message code and should never occur.
#define UTMSG_INVALID					0

//----------------------------------------------------------
// Client to server message codes.
//----------------------------------------------------------

// UTMSG_CLIENT_CREATE: creates an uber-texture.
//
// Parameters:
//		Width
//		Height
//		Stage count
//		Flags (1 per stage)
//		Channel counts (1 per stage)
//		File name string length
//		File name string
//
// Acknowledgement:
//		Uber texture ID
//
#define UTMSG_CLIENT_CREATE				1
/*
// UTMSG_CLIENT_IMPORT: imports an uber-texture.
//
// Parameters:
//		source TGA file name string length
//		source TGA file name string
//		destination file name string length
//		destination file name string
//		Channel count
//		Flags
//
// Acknowledgement:
//		Uber texture ID
//		Width
//		Height
//		Channel count
//		Flags
//
#define UTMSG_CLIENT_IMPORT				2
*/
// UTMSG_CLIENT_OPEN: opens an uber-texture.
//
// Parameters:
//		Transaction ID
//		File name string length
//		File name string
//
// Acknowledgement:
//		Uber texture ID
//		Width
//		Height
//		Flags
//		Stage count
//		Channel counts (1 per stage)
//
#define UTMSG_CLIENT_OPEN				3

// UTMSG_CLIENT_FINALIZE: finalizes an uber-texture.
//
// Parameters:
//		Uber texture ID
//		Compression Quality
//		Channel flags (one for each channel in each stage).
//
// Acknowledgement:
//		Transaction ID
//
#define UTMSG_CLIENT_FINALIZE			4

// UTMSG_CLIENT_CLOSE: closes an uber-texture.
//
// Parameters:
//		Uber texture ID
//
// Acknowledgement:
//		None
//		
#define UTMSG_CLIENT_CLOSE				5

// UTMSG_CLIENT_READ: reads a tile from an ubertexture.
//
// Parameters:
//		Uber texture ID
//		Tile X
//		Tile Y
//		Mip Level
//
// Acknowledgement:
//		Uber texture ID
//		Tile X
//		Tile Y
//		Mip Level
//		Compressed tileset data (LZ77 or S3TC)
//
#define UTMSG_CLIENT_READ				6

// UTMSG_CLIENT_READCANCEL: cancels a read from an ubertexture.
//
// Parameters:
//		Transaction ID of read to cancel.
//
// Acknowledgement:
//		None
//
#define UTMSG_CLIENT_READCANCEL			7

// UTMSG_CLIENT_MULTI_READ: reads multiple tiles from an ubertexture.
//
// Parameters:
//		Uber texture ID
//		Tile count
//		STRUCT
//			Mip level	:  8-bits
//			Tile X		: 12-bits
//			Tile Y		: 12-bits
//		END (1 per tile)
//
// Acknowledgement:
//		Same as UTMSG_CLIENT_READ.  However, one response is sent for
//		every tile requested.
//
#define UTMSG_CLIENT_MULTI_READ			8

// UTMSG_CLIENT_WRITE: reads a tile from an ubertexture.
//
// Parameters:
//		Uber texture ID
//		Tile X
//		Tile Y
//		Mip Level
//		Compressed tileset data
//
// Acknowledgement:
//		None
//
#define UTMSG_CLIENT_WRITE				9

// UTMSG_CLIENT_DIR_LISTING: returns a list of ubertextures in a given path.
//
// Parameters:
//		Relative path name.
//
// Acknowledgement:
//		Uber texture ID
//
#define UTMSG_CLIENT_DIR_LISTING		10

// UTMSG_ERROR: sent whenever a message request fails.
//
// Data:
//
#define UTMSG_ERROR						11

// UTMSG_MULTIREAD_ERROR: sent whenever a multi-read request fails.  Note
//
// Data:
//		This message does not echo back any data other than the original
//		read request.
//
#define UTMSG_MULTI_READ_ERROR			11

// UTMSG_CLIENT_PING: pings the ubertexture server.
//
// Parameters:
//
// Acknowledgement:
//		Transaction ID
//
#define UTMSG_CLIENT_PING				255

//----------------------------------------------------------
// Message handling.

// UTSendMessage: sends a message to the socket specified.
// Returns 1 on success, 0 if not complete, and -1 on error.
extern int UTMessageSend( SOCKET socket, UT_message_t* message, SendProc recvProc = send );

// UTGetMessage: gets a message from the socket specified.  Note that the
// pointer returned by this function cannot be used after any other calls to
// UTGetMessage() or UTSendMessage().
// Returns 1 on success, 0 if not complete, and -1 on error.
extern int UTMessageRecv( UT_message_t* message, SOCKET socket, RecvProc recvProc = recv,
						  IoctlProc ioctlProc = ioctlsocket );

// UTMessageInit: initalizes the message passed in.
extern void UTMessageInit( UT_message_t* message );

// UTMessageReady: returns true if the message is ready.
extern bool UTMessageReady( const UT_message_t* message );

// UTMessageEmpty: returns true if the message is currently in it's initial
// empty state.
extern bool UTMessageEmpty( const UT_message_t* message );

// UTMessageSize: returns the total size of the message in bytes.
extern unsigned int UTMessageSize( const UT_message_t* message );

// message reading utilities.
extern bool UTMessageReadBegin( UT_message_t* message );
extern void UTMessageReadEnd( UT_message_t* message );
extern bool UTMessageReadShort( short* value, UT_message_t* message );
extern bool UTMessageReadInt( int* value, UT_message_t* message );
extern bool UTMessageReadUInt( unsigned int* value, UT_message_t* message );
extern bool UTMessageReadStr( char* buffer, int bufLen, UT_message_t* message );
extern bool UTMessageReadBytes( char* buffer, int bufLen, UT_message_t* message );
extern bool UTMessageDecompressData( void* buffer, int bufLen, UT_message_t* message );
extern char* UTMessageGetData( UT_message_t* message, int size );

// message writing utilities.
extern bool UTMessageWriteBegin( UT_message_t* message, int code, unsigned int transactionId );
extern void UTMessageWriteEnd( UT_message_t* message );
extern bool UTMessageWriteShort( UT_message_t* message, short value );
extern bool UTMessageWriteInt( UT_message_t* message, int value );
extern bool UTMessageWriteUInt( UT_message_t* message, unsigned int value );
extern bool UTMessageWriteStr( UT_message_t* message, const char* str );
extern bool UTMessageWriteBytes( UT_message_t* message, const void* data, size_t len );
extern bool UTMessageCompressData( UT_message_t* message, const void* src, int srcSize );
extern char* UTMessageReserveData( UT_message_t* message, int size );

//----------------------------------------------------------
// Message queue functions.

// UTMessageQueueCreate: initializes a new message queue.
extern void UTMessageQueueCreate( UT_message_queue_t* queue, size_t size );

// UTMessageQueueResize: resizes memory allocated to the message queue.
extern void UTMessageQueueExpand( UT_message_queue_t* queue, size_t size );

// UTMessageQueueDestroy: frees memory allocated to the message queue.
extern void UTMessageQueueDestroy( UT_message_queue_t* queue );

// UTMessageQueueAdd: adds a new message to the queue and returns a pointer
// to that message.  If the queue is full, then this function returns NULL.
extern UT_message_t* UTMessageQueueAdd( UT_message_queue_t* queue );

// UTMessageQueueRemove: returns the last item in the queue and removes it.
extern UT_message_t* UTMessageQueueRemove( UT_message_queue_t* queue );

// UTMessageQueuePeek: returns the first item in the queue.
extern UT_message_t* UTMessageQueueTop( UT_message_queue_t* queue );

// UTMessageQueuePeek: returns the last item in the queue.
extern UT_message_t* UTMessageQueuePeek( UT_message_queue_t* queue, size_t message = 0 );

// UTMessageQueueRemove: removes the last item in the queue.
extern UT_message_t* UTMessageQueueRemove( UT_message_queue_t* queue );

// UTMessageQueueCount: returns the total number of messages in the queue.
extern size_t UTMessageQueueCount( UT_message_queue_t* queue );

// UTMessageQueueRemove: returns true if the queue is full.
extern bool UTMessageQueueFull( UT_message_queue_t* queue );

// UTMessageQueueRemove: removes all items from the queue.
extern void UTMessageQueueClear( UT_message_queue_t* queue );

// UTMessageQueueSend: sends as many messages in the queue as possible.
// Returns 1 on success, 0 if not complete, and -1 on error.
extern int UTMessageQueueSend( SOCKET socket, UT_message_queue_t* queue, char* buf,
							   size_t bufSize,
							   int* bytesSent = 0, SendProc sendProc = send );

// UTMessageQueueSend: sends as many messages in the queue as possible.
// Returns 1 on success, 0 if not complete, and -1 on error.
extern void UTMessageQueueRecv( UT_message_queue_t* queue, SOCKET socket, int* bytesReceived = 0,
							    RecvProc recvProc = recv, IoctlProc ioctlProc = ioctlsocket );
