//----------------------------------------------------------
// File:		ThPipe.h
// Author:		Kevin Bray
// Created:		09-02-08
//
// Purpose:		To provide an abstract interface for inter-thread,
//				inter-process, and network communication.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class ThSignal;

//**********************************************************
// class ThPipe
//**********************************************************
class ThPipe
{
public:
	virtual ~ThPipe() = 0;

	// read/write buffer size.
	virtual unsigned int	GetRegionSize() const = 0;

	// returns the current write region.
	virtual void*			GetWriteRegion( bool wait = true ) = 0;
	virtual const void*		GetReadRegion( bool wait = true ) = 0;

	// region access.
	virtual void*			NextWriteRegion( bool wait = true ) = 0;
	virtual const void*		NextReadRegion( bool wait = true ) = 0;

	// signal access.
	virtual const ThSignal&	GetReadSignal() const = 0;
	virtual const ThSignal& GetWriteSignal() const = 0;

private:
};
