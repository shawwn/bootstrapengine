//----------------------------------------------------------
// File:		GrMeshIB.h
// Author:		Kevin Bray
// Created:		12-10-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrMeshIB.h"

// project includes.
#include "GrUtil.h"
#include "GLSubsys.h"


//**********************************************************
// class GrMeshIB
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMeshIB::GrMeshIB( unsigned int indexCount, const unsigned short* indices, unsigned int flags,
				    unsigned int maxIndexCount )
: _indexBuffer( 0 )
, _indexCount( indexCount )
, _dirty( 1 )
, _flags( flags )
, _indices( new unsigned short[ indexCount ] )
, _maxIndexCount( Max( indexCount, maxIndexCount ) )
{
	// the kNoCopy flag cannot be specified with this constructor!
	B_ASSERT( ( flags & kNoCopy ) == 0 );

	MemCopy( _indices, indices, indexCount * sizeof( unsigned short ) );
}

//----------------------------------------------------------
GrMeshIB::GrMeshIB( unsigned int indexCount, unsigned int flags, unsigned int maxIndexCount )
: _indexBuffer( 0 )
, _indexCount( indexCount )
, _dirty( 1 )
, _flags( flags )
, _indices( ( flags & kNoCopy ) ? 0 : new unsigned short[ maxIndexCount ? Max( maxIndexCount, indexCount ) : indexCount ] )
, _maxIndexCount( Max( indexCount, maxIndexCount ) )
{

}

//----------------------------------------------------------
GrMeshIB::~GrMeshIB()
{
	delete[] _indices;
	if ( _indexBuffer )
		GrDeleteIB( _indexBuffer );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMeshIB::AdjustIndexCount( int growSize )
{
	int t = _indexCount + growSize;
	B_ASSERT( ( t > 0 ) || ( t < ( int )_maxIndexCount ) );
	_indexCount = t;
	MarkAsDirty();
}

//----------------------------------------------------------
unsigned short*
GrMeshIB::Map( bool writeOnly )
{
	// if we're a buffer with a local copy, then just return the local
	// copy.
	if ( _indices )
		return _indices;

	if ( !_indexBuffer )
		Cache();

	// if we're not a buffer with a local copy, then we need to map the
	// buffer and return the memory pointer.
	GrBindIB( _indexBuffer );
	void* ptr = bglMapBuffer( GL_ELEMENT_ARRAY_BUFFER, writeOnly ? GL_WRITE_ONLY : GL_READ_WRITE );

	// return the start of buffer memory.
	return ( unsigned short* )ptr;
}

//----------------------------------------------------------
void
GrMeshIB::Unmap()
{
	if ( _indices )
	{
		MarkAsDirty();
	}
	else
	{
		GrBindIB( _indexBuffer );
		bglUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	}
}

//----------------------------------------------------------
void
GrMeshIB::Update( unsigned int start, unsigned short* source, unsigned int count )
{
	// if we have a local copy, then simply update it.  Otherwise, update
	// the buffer directly.
	if ( _indices )
	{
		MemCopy( _indices + start, source, count * sizeof( unsigned short ) );
		MarkAsDirty();
	}
	else
	{
		// create the index buffer if it doesn't already exist.
		unsigned int prevIB = GrGetIB();

		if ( !_indexBuffer )
			Cache();

		GrBindIB( _indexBuffer );
		CHECK_GL();

		// update the data.
		bglBufferSubData( GL_ELEMENT_ARRAY_BUFFER, start * sizeof( unsigned short ),
			count * sizeof( unsigned short ), source );
		CHECK_GL();

		GrBindIB( prevIB );
		CHECK_GL();
	}
}

//----------------------------------------------------------
void
GrMeshIB::Cache()
{
	unsigned int prevIB = GrGetIB();
	if ( !_indexBuffer )
	{
		_dirty = 1;
		_indexBuffer = GrGenIB();
		GrBindIB( _indexBuffer );
		bglBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * _maxIndexCount, 0, ( ( _flags & kDynamic ) != 0 ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}
	GrBindIB( prevIB );
}

//----------------------------------------------------------
void
GrMeshIB::Evict()
{
	if ( _indexBuffer )
	{
		GrDeleteIB( _indexBuffer );
		_indexBuffer = 0;
	}
}

//----------------------------------------------------------
void
GrMeshIB::Bind()
{
	CHECK_GL();

	if ( !_indexBuffer )
		Cache();

	// bind the index buffer.
	GrBindIB( _indexBuffer );

	// update the contents if necessary.
	if ( _dirty != 0 )
	{
		if ( _indices )
			bglBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, 2*_indexCount, _indices );
		_dirty = 0;
	}
	CHECK_GL();
}

//----------------------------------------------------------
void
GrMeshIB::PreContextReset()
{
	// free our index buffer.
	if ( _indexBuffer )
		GrDeleteIB( _indexBuffer );
}

//----------------------------------------------------------
void
GrMeshIB::PostContextReset()
{
	// reallocate our index buffer.
	if ( _indexBuffer )
	{
		_indexBuffer = 0;
		Cache();
	}
}
