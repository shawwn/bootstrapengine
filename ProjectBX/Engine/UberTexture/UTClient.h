//----------------------------------------------------------
// File:		UTClient.h
// Author:		Kevin Bray
// Created:		11-23-07
//
// Purpose:		To provide a common interface for communicating with
//				a UTServer.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes
#include "UTConfig.h"

// for size_t
#include <stdlib.h>

// forward declarations.
struct UT_message_t;

//**********************************************************
// Global functions
//**********************************************************

// UTConnect: connects to the UberTexture server at the ip-address specified.
// Returns true on success.
extern bool UTClientConnect( const char* host, unsigned short port );

// UTDisconnect: disconnects from the server.  Returns true on success.
extern bool UTClientDisconnect();

// UTClientQueueOutgoing: queue an outgoing message in the outgoing message
// queue.
extern UT_message_t* UTClientQueueOutgoing();

// UTClientRemoveIncoming: remove an incoming message in the incoming message
// queue.
extern UT_message_t* UTClientRemoveIncoming();

// UTClientOutgoingCount: returns the number of queued messages in the outgoing
// queue.
extern size_t UTClientOutgoingCount();

// UTClientIncomingCount: returns the number of queued messages in the incoming
// queue.
extern size_t UTClientIncomingCount();

// SyncToResponse: synchronizes to the response of the transaction ID passed
// in.  This function returns true on success, false on failure.  If this
// function returns true, then the reply to the message is waiting in the
// message queue.
extern bool UTSyncToResponse( unsigned int transactionId, unsigned int timeToWait = 1000 );

// UTClientUpdateQueues: updates the incoming and outgoing message queues.
extern void UTClientUpdateQueues();
