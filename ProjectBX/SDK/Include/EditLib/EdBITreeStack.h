//----------------------------------------------------------
// File:		EdBITreeStack.h
// Author:		Kevin Bray
// Created:		08-15-08
//
// Purpose:		To provide a stack for traversing a BITree.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// engine includes.
#include "GrTypes.h"

// forward declarations.
class EdBITreeInterval;

//**********************************************************
// class EdBITreeStack
//**********************************************************
class EdBITreeStack
{
public:
	EdBITreeStack( unsigned int treeDepth );
	~EdBITreeStack();
	
	struct SEntry
	{
		SVec3 start;
		EdBITreeInterval* interval;
		SVec3 end;
		unsigned int reserved;
	};

	// pushes the interval and the clipped ray on to the stack.
	SEntry&				Push();
	const SEntry&		Pop();
	const SEntry&		Top();

	unsigned int		GetItemCount()			{	return _top;		}

private:
	SEntry*				_stack;
	unsigned int		_depth;
	unsigned int		_top;
};
