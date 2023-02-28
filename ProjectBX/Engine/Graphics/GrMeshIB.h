//----------------------------------------------------------
// File:		GrMeshIB.h
// Author:		Kevin Bray
// Created:		12-10-06
//
// Purpose:		To manage an index buffer object.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"

//**********************************************************
// class GrMeshIB
//**********************************************************
class GrMeshIB : public URefCount
{
public:
	static const unsigned int kDynamic = 1;
	static const unsigned int kNoCopy = 2;

	GrMeshIB( unsigned int indexCount, const unsigned short* indices, unsigned int flags,
			  unsigned int maxIndexCount = 0 );
	GrMeshIB( unsigned int indexCount, unsigned int flags, unsigned int maxIndexCount = 0 );
	~GrMeshIB();

	// return the index count.
	unsigned int		GetIndexCount() const		{	return _indexCount;			}
	// return the index count.
	unsigned int		GetMaxIndexCount() const	{	return _maxIndexCount;			}

	// adjust the number of active indices.
	void				AdjustIndexCount( int growSize );

	// returns creation flags.
	unsigned int		GetFlags() const			{	return _flags;				}

	// get indices for read/write.  Call MarkAsDirty to force index data
	// to be resubmitted to the graphics hardware.
	unsigned short*		GetIndices() const			{	return _indices;			}
	void				MarkAsDirty()				{	_dirty = 1;					}

	// locks the index buffer.
	unsigned short*		Map( bool writeOnly );
	void				Unmap();

	// updates a portion of the index buffer.
	void				Update( unsigned int start, unsigned short* source, unsigned int count );

	// cache/evict mesh data.
	void				Cache();
	void				Evict();

	// bind the index buffer.
	void				Bind();

	// context reset.
	void				PreContextReset();
	void				PostContextReset();

private:
	unsigned int		_indexBuffer;
	unsigned int		_indexCount;
	unsigned int		_maxIndexCount;
	unsigned int		_dirty;
	unsigned int		_flags;
	unsigned short*		_indices;
};
