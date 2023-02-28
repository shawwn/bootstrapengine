//----------------------------------------------------------
// File:		UTClient.cpp
// Author:		Kevin Bray
// Created:		11-23-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "UTMessage.h"

// project includes.
#include "UTConfig.h"
#include "common_afx.h"

// zlib includes.
#include "zlib.h"

// windows includes.
#include <winsock.h>

#define ZLIB_COMPRESSION 0


//**********************************************************
// Global functions
//**********************************************************

//----------------------------------------------------------
int UTMessageSend( SOCKET socket, UT_message_t* message, SendProc sendProc )
{
	// make sure the message isn't too large.
	unsigned int totalSize = UTMessageSize( message );
	if ( totalSize > UT_MAX_MESSAGE_SIZE )
		return -1;

	B_ASSERT( totalSize >= 12 );
	B_ASSERT( message->code != UTMSG_INVALID );

	// everything was already sent, so just notify the caller of the
	// previous success.
	if ( message->offset >= totalSize )
		return 1;

	// try to send the message data.
	int dataSent = ( *sendProc )( socket, ( const char* )message + message->offset, totalSize - message->offset, 0 );
	if ( dataSent == SOCKET_ERROR )
	{
		// is the port ready?
		if ( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;
		return -1;
	}

	// advance the offset.
	message->offset += dataSent;

	// return 1 if the entire message was sent, 0 if not.
	return ( message->offset >= totalSize ) ? 1 : 0;
}

//----------------------------------------------------------
int UTMessageRecv( UT_message_t* message, SOCKET socket, RecvProc recvProc, IoctlProc ioctlProc )
{
	// if the message is already finished, simply return 1.
	if ( message->code != UTMSG_INVALID && UTMessageReady( message ) )
		return 1;

	// check to see how much data is waiting.
	unsigned long dataWaiting = 0;
	( *ioctlProc )( socket, FIONREAD, &dataWaiting );

	// simply return true if no data is waiting.
	if ( dataWaiting == 0 )
		return 0;

	// are we reading the header?
	if ( message->offset < 12 )
	{
		// simply return if there isn't enough data to read the header.
		int headerBytes = Min( ( int )dataWaiting, 12 - ( int )message->offset );

		// read the header.
		int bytesReceived = ( *recvProc )( socket, ( char* )message + message->offset, headerBytes, 0 );
		B_ASSERT( bytesReceived < 4 || message->code != 0 );

		// update the message offset.
		message->offset += bytesReceived;

		// if there isn't any more data waiting after reading the header,
		// simply return.
		dataWaiting -= bytesReceived;
		if ( dataWaiting == 0 )
		{
			// not finished reading.
			return 0;
		}
	}

	// determine now much data still needs to be read for the message.
	unsigned int totalSize = UTMessageSize( message );
	unsigned int leftToRead = totalSize - message->offset;

	// read as much data as possible.
	unsigned int readSize = Min( ( unsigned int )dataWaiting, leftToRead );

	// read as much message data as possible.
	int dataRead = ( *recvProc )( socket, ( char* )message + message->offset, readSize, 0 );
	if ( dataRead == SOCKET_ERROR )
	{
		// is data ready?
		if ( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;

		// return false to indicate failure.
		return -1;
	}

	// update the message buffer offset.
	message->offset += ( unsigned int )dataRead;

	// return true to indicate success.
	return ( message->offset == totalSize ) ? 1 : 0;
}

//----------------------------------------------------------
void UTMessageInit( UT_message_t* message )
{
	message->code = UTMSG_INVALID;
	message->dataSize = 0;
	message->offset = 0;
	message->transactionId = 0;
}

//----------------------------------------------------------
bool UTMessageReady( const UT_message_t* message )
{
	return ( ( message->code != UTMSG_INVALID ) && ( message->offset >= 12 ) &&
		( UTMessageSize( message ) == message->offset ) );
}

//----------------------------------------------------------
bool UTMessageEmpty( const UT_message_t* message )
{
	return ( message->code == UTMSG_INVALID );
}

//----------------------------------------------------------
unsigned int UTMessageSize( const UT_message_t* message )
{
	return message->dataSize + 12;
}

//----------------------------------------------------------
bool UTMessageReadBegin( UT_message_t* message )
{
	// make sure the message isn't empty.
	if ( UTMessageEmpty( message ) )
		return false;

	// set the offset to the beginning of data, minus the data size.
	message->offset = 0;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void UTMessageReadEnd( UT_message_t* message )
{
	// reset the offset to the "ready" value.
	message->offset = UTMessageSize( message );
}

//----------------------------------------------------------
bool UTMessageReadShort( short* value, UT_message_t* message )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->offset + 2 ) > message->dataSize )
		return false;

	// store the value.
	*value = *( short* )( message->buffer + message->offset );
	message->offset += 2;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageReadInt( int* value, UT_message_t* message )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->offset + 4 ) > message->dataSize )
		return false;

	// store the value.
	*value = *( int* )( message->buffer + message->offset );
	message->offset += 4;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageReadUInt( unsigned int* value, UT_message_t* message )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->offset + 4 ) > message->dataSize )
		return false;

	// store the value.
	*value = *( unsigned int* )( message->buffer + message->offset );
	message->offset += 4;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageReadStr( char* buffer, int bufLen, UT_message_t* message )
{
	// read the length from the message.
	short len = 0;
	if ( !UTMessageReadShort( &len, message ) )
		return false;

	// make sure the length isn't too long.
	if ( ( message->offset + len ) > message->dataSize )
		return false;

	// subtract one from the overall length of the buffer.
	bufLen -= 1;

	// calculate the copy size.
	if ( bufLen > len )
		bufLen = len;

	// copy the data and append the null terminator.
	memcpy( buffer, message->buffer + message->offset, bufLen );
	buffer[ bufLen ] = 0;

	// advance past the string.
	message->offset += len;

	// return true to indicate success.
	return true;
}

#pragma warning( disable : 4100 )
//----------------------------------------------------------
bool UTMessageDecompressData( void* buffer, int bufLen, UT_message_t* message )
{
	// get the source size.
	int srcLen = 0;
	if ( !UTMessageReadInt( &srcLen, message ) )
		return false;

	// decompress the data.
#if ZLIB_COMPRESSION
	unsigned long ulBufLen = bufLen;
	if ( uncompress( ( Bytef* )buffer, &ulBufLen, ( Bytef* )( message->buffer + message->offset ), srcLen ) != Z_OK )
		return false;
#else
	// copy raw data.
	MemCopy( buffer, message->buffer + message->offset, srcLen );
#endif

	// return true to indicate success.
	return true;
}
#pragma warning( default : 4100 )

//----------------------------------------------------------
char* UTMessageGetData( UT_message_t* message, int size )
{
	// make sure the length isn't too long.
	if ( ( message->offset + size ) > message->dataSize )
		return 0;

	// get the pointer to the data.
	char* ptr = message->buffer + message->offset;

	// advance the offset.
	message->offset += size;

	// return the pointer to the data.
	return ptr;
}

//----------------------------------------------------------
bool UTMessageWriteBegin( UT_message_t* message, int code, unsigned int transactionId )
{
	// store the message code.
	message->code = code;
	message->dataSize = 0;
	message->transactionId = transactionId;

	// reset message data.
	message->offset = 0;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void UTMessageWriteEnd( UT_message_t* message )
{
	message->offset = 0;
}

//----------------------------------------------------------
bool UTMessageWriteShort( UT_message_t* message, short value )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->dataSize + 2 ) > UT_MAX_MESSAGE_SIZE )
		return false;

	// store the value.
	*( short* )( message->buffer + message->dataSize ) = value;
	message->dataSize += 2;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageWriteInt( UT_message_t* message, int value )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->dataSize + 4 ) > UT_MAX_MESSAGE_SIZE )
		return false;

	// store the value.
	*( int* )( message->buffer + message->dataSize ) = value;
	message->dataSize += 4;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageWriteUInt( UT_message_t* message, unsigned int value )
{
	// make sure we have enough data to satisfy the request.
	if ( ( message->dataSize + 4 ) > UT_MAX_MESSAGE_SIZE )
		return false;

	// store the value.
	*( unsigned int* )( message->buffer + message->dataSize ) = value;
	message->dataSize += 4;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageWriteStr( UT_message_t* message, const char* str )
{
	// make sure the length isn't too long.
	short len = ( short )strlen( str );
	if ( ( message->dataSize + len + 2 ) > UT_MAX_MESSAGE_SIZE )
		return false;

	// write the length of the string.
	if ( !UTMessageWriteShort( message, len ) )
		return false;

	// copy the string data (excluding the null terminator).
	memcpy( message->buffer + message->dataSize, str, len );

	// update the offset.
	message->dataSize += len;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageWriteBytes( UT_message_t* message, const void* data, size_t len )
{
	// bad data?
	if( !data )
		return false;

	// make sure the length isn't too long.
	if ( ( message->dataSize + len ) > UT_MAX_MESSAGE_SIZE )
		return false;

	// copy the data.
	memcpy( message->buffer + message->dataSize, data, len );

	// update the offset.
	message->dataSize += ( unsigned int )len;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
bool UTMessageCompressData( UT_message_t* message, const void* src, int srcSize )
{
	// reserve room for the size.
	char* sizeLoc = UTMessageReserveData( message, 4 );
	if ( !sizeLoc )
		return false;

	// try to compress the data.
#if ZLIB_COMPRESSION
	unsigned long dstSize = UT_MAX_MESSAGE_SIZE - message->dataSize;
	if ( compress2( ( Bytef* )message->buffer + message->dataSize, &dstSize, ( Bytef* )src, srcSize, Z_BEST_SPEED ) )
	{
		// get rid of the 4 bytes reserved for the compressed size.
		message->dataSize -= 4;

		// return false to indicate falure.
		return false;
	}
#else
	// copy raw data.
	unsigned long dstSize = srcSize;
	MemCopy( message->buffer + message->dataSize, src, srcSize );
#endif
	// write the size.
	*( unsigned long* )sizeLoc = dstSize;

	// if we compressed the data successfully, adjust our size and return.
	message->dataSize += ( unsigned int )dstSize;

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
char* UTMessageReserveData( UT_message_t* message, int size )
{
	// make sure the length isn't too long.
	if ( ( message->dataSize + size ) > UT_MAX_MESSAGE_SIZE )
		return 0;

	// calculate the starting address and update the offset.
	char* start = message->buffer + message->dataSize;
	message->dataSize += size;

	// return the starting address.
	return start;
}

//----------------------------------------------------------
void UTMessageQueueCreate( UT_message_queue_t* queue, size_t size )
{
	// initialize queue data.
	queue->messages = ( UT_message_t* )malloc( size * sizeof( UT_message_t ) );
	queue->size = size;
	queue->head = 0;
	queue->tail = 0;
}

//----------------------------------------------------------
void UTMessageQueueExpand( UT_message_queue_t* queue, size_t size )
{
	// allocate new memory.
	size += queue->size;
	UT_message_t* mem = ( UT_message_t* )malloc( size * sizeof( UT_message_t ) );

	// copy the previous data.
	UT_message_t* cur = mem;
	for ( size_t i = queue->tail; i < queue->head; ++i, ++cur )
		*cur = queue->messages[ i % queue->size ];

	// free the old queue data.
	free( queue->messages );

	// adjust data in the queue.
	queue->messages = mem;
	queue->head -= queue->tail;
	queue->tail = 0;
	queue->size = size;
}

//----------------------------------------------------------
void UTMessageQueueDestroy( UT_message_queue_t* queue )
{
	// free memory allocated to the queue.
	free( queue->messages );
	queue->size = 0;
	queue->head = 0;
	queue->tail = 0;
}

//----------------------------------------------------------
UT_message_t* UTMessageQueueAdd( UT_message_queue_t* queue )
{
	// calculate how many items are in the current in the queue.
	size_t count = queue->head - queue->tail;

	// simply return null if the queue is full.
	if ( count >= queue->size )
		return 0;

	// calculate a pointer to the first free message and advance the head.
	UT_message_t* message = queue->messages + ( queue->head++ % queue->size );

	// initialize the message.
	UTMessageInit( message );

	// return the message.
	return message;
}

//----------------------------------------------------------
UT_message_t* UTMessageQueueRemove( UT_message_queue_t* queue )
{
	// remove the item from the bottom of the queue.
	if ( queue->tail < queue->head )
		return queue->messages + ( queue->tail++ % queue->size );

	// return null to indicate that there is nothing to remove.
	return 0;
}

//----------------------------------------------------------
UT_message_t* UTMessageQueueTop( UT_message_queue_t* queue )
{
	// return the first item in the queue if the queue isn't empty.
	if ( queue->tail < queue->head )
		return queue->messages + ( ( queue->head - 1 ) % queue->size );

	// return null to indicate that there is nothing in the queue.
	return 0;
}

//----------------------------------------------------------
UT_message_t* UTMessageQueuePeek( UT_message_queue_t* queue, size_t message )
{
	// return the last item in the queue if the queue isn't empty.
	if ( ( queue->tail + message ) < queue->head )
		return queue->messages + ( ( queue->tail + message ) % queue->size );

	// return null to indicate that there is nothing in the queue.
	return 0;
}

//----------------------------------------------------------
size_t UTMessageQueueCount( UT_message_queue_t* queue )
{
	// returns number of messages in the queue.
	return ( queue->head - queue->tail );
}

//----------------------------------------------------------
bool UTMessageQueueFull( UT_message_queue_t* queue )
{
	// returns true if the queue is full.
	return ( ( queue->head - queue->tail ) >= queue->size );
}

//----------------------------------------------------------
void UTMessageQueueClear( UT_message_queue_t* queue )
{
	// clear the head and the tail.
	queue->head = 0;
	queue->tail = 0;
}

//----------------------------------------------------------
int UTMessageQueueSend( SOCKET socket, UT_message_queue_t* queue, char* buf,
					    size_t bufSize,
						int* bytesSent, SendProc sendProc )
{
	// go through messages in the queue and combine them.
	size_t count = UTMessageQueueCount( queue );

	// simply return if there isn't anything to do!
	if ( count == 0 )
		return 1;

	size_t offset = 0;
	for ( size_t i = 0; i < count; ++i )
	{
		// get the current message to send.
		UT_message_t* toSend = UTMessageQueuePeek( queue, i );

		// get the size of the current message, minus the send offset.
		unsigned int curSize = UTMessageSize( toSend ) - toSend->offset;
		size_t newOffset = offset + curSize;

		// did we exhaust our buffer?  If so, break.
		if ( newOffset > bufSize )
			break;

		// add the message to the buffer.
		MemCopy( buf + offset, ( unsigned char* )toSend + toSend->offset, curSize );

		// update the buffer offset.
		offset = newOffset;
	}

	// try to send the message data.
	int dataSent = ( *sendProc )( socket, buf, ( int )offset, 0 );
	if ( dataSent == SOCKET_ERROR )
	{
		// is the port ready?
		if ( WSAGetLastError() == WSAEWOULDBLOCK )
			return 0;
		return -1;
	}

	// return the number of bytes sent if necessary.
	if ( bytesSent )
		*bytesSent = dataSent;

	// remove messages from the queue.
	while ( dataSent )
	{
		// get the current message.
		UT_message_t* cur = UTMessageQueuePeek( queue );

		// get the total size of the message data sent.
		int curSize = UTMessageSize( cur ) - cur->offset;
		if ( curSize <= dataSent )
		{
			// remove the message from the queue.
			UTMessageQueueRemove( queue );
			dataSent -= curSize;
		}
		else
		{
			// adjust the offset of the current message and clear
			// the data sent.
			cur->offset += dataSent;
			dataSent = 0;
		}
	}

	// return 1 if there are no more messages in the queue, or 0
	// if there are messages remaining.
	return ( UTMessageQueueCount( queue ) == 0 ) ? 1 : 0;
}

//----------------------------------------------------------
void UTMessageQueueRecv( UT_message_queue_t* queue, SOCKET socket, int* bytesReceived,
						 RecvProc recvProc, IoctlProc ioctlProc )
{
/*
	// check to see how much data is waiting.
	unsigned long dataWaiting = 0;
	( *ioctlProc )( socket, FIONREAD, &dataWaiting );

	// simply return if there isn't any data waiting.
	if ( dataWaiting == 0 )
		return;

	// now read the data.
	size_t sizeToRead = ( size_t )Min( dataWaiting, ( unsigned long )bufSize );
	int bytesRemaining = ( *recvProc )( socket, buf, ( int )sizeToRead, 0 );

	// check to see if any data was ready.
	if ( bytesRemaining == SOCKET_ERROR )
		return;

	// update the bytes recieved value if necessary.
	if ( bytesReceived )
		*bytesReceived = bytesRemaining;

	// now parse out the data and add the messages to the queue.
	UT_message_t* top = 0;
	while ( bytesRemaining )
	{
		// get the top message.
		top = UTMessageQueueTop( queue );

		// add a new message to the queue if necessary.
		if ( top == 0 || UTMessageReady( top ) )
		{
			top = UTMessageQueueAdd( queue );
			if ( !top )
			{
				UTMessageQueueExpand( queue, 10 );
				top = UTMessageQueueAdd( queue );
			}
		}

		// now read in the message data as needed.
		if ( top->offset < 4 )
		{
			// figure out how much of the header is remaining and copy
			// it to the message.
			int headerBytes = Min( 4 - ( int )top->offset, bytesRemaining );
			MemCopy( ( char* )top + top->offset, buf, headerBytes );

			// now add the number of bytes read to the offset.
			top->offset += headerBytes;

			// subtract from bytesRemaining and advance our buffer pointer.
			bytesRemaining -= headerBytes;
			buf += headerBytes;

			// is there any data left?
			if ( bytesRemaining == 0 )
				return;
		}

		// figure out how much data we need to read for the current message.
		unsigned int totalSize = UTMessageSize( top );
		unsigned int messageBytes = totalSize - top->offset;
		int readBytes = Min( ( int )messageBytes, bytesRemaining );

		// read the data for the message.
		MemCopy( ( char* )top + top->offset, buf, readBytes );
		top->offset += readBytes;

		// subtract from bytesRemaining and advance our buffer pointer.
		bytesRemaining -= readBytes;
		buf += readBytes;
	}
*/
	// recv incoming messages.
	UT_message_t* top = 0;
	do
	{
		if ( bytesReceived && top )
			*bytesReceived += UTMessageSize( top );

		// get the top message.
		top = UTMessageQueueTop( queue );

		// do we need to add a new message to the queue?
		if ( top )
		{
			if ( UTMessageReady( top ) )
			{
				top = UTMessageQueueAdd( queue );
			}
			else
			{
				bool breakpt = false;
				breakpt = breakpt;
			}
		}
		else
			top = UTMessageQueueAdd( queue );

	}
	while ( top && UTMessageRecv( top, socket, recvProc, ioctlProc ) == 1 );

	if ( bytesReceived && top && top->code != UTMSG_INVALID )
		*bytesReceived += top->offset;
}
