//----------------------------------------------------------
// File:		GrStreamingIB.h
// Author:		Kevin Bray
// Created:		01-12-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrStreamingIB.h"

// project includes.
#include "GrMeshBufferMgr.h"
#include "GLSubsys.h"


//**********************************************************
// class GrStreamingIB
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrStreamingIB::GrStreamingIB( unsigned int initSize, unsigned int growExtra )
: _offset( 0 )
, _growExtra( growExtra )
{
	// create the index buffer to stream to.
	_indexBuffer = gGrMeshBufferMgr->CreateMeshIB( initSize/2, GrMeshIB::kDynamic | GrMeshIB::kNoCopy );
}

//----------------------------------------------------------
GrStreamingIB::~GrStreamingIB()
{
	_indexBuffer->Evict();
	_indexBuffer = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrStreamingIB::AddIndices( unsigned short* indices, unsigned int count )
{
	// can we fit?
	unsigned int sizeNeeded = _offset + count;
	if ( sizeNeeded > _indexBuffer->GetIndexCount() )
	{
		// allocate a new index buffer since we can't fit.
		URef< GrMeshIB > newIB = gGrMeshBufferMgr->CreateMeshIB( sizeNeeded + _growExtra, /*GrMeshIB::kDynamic | */GrMeshIB::kNoCopy );
		CHECK_GL();

		unsigned short* ptr = _indexBuffer->Map( false );
		CHECK_GL();

		newIB->Update( 0, ptr, _offset );
		CHECK_GL();

		_indexBuffer->Unmap();
		CHECK_GL();

		_indexBuffer->Evict();

		// set the new index buffer and then bind.
		_indexBuffer = newIB;
		_indexBuffer->Bind();
	}

	// stream the indices.
	_indexBuffer->Update( _offset, indices, count );
	_offset += count;
}

//----------------------------------------------------------
void
GrStreamingIB::Reset()
{
	_offset = 0;
}

//----------------------------------------------------------
void
GrStreamingIB::Bind()
{
	_indexBuffer->Bind();
}
