//----------------------------------------------------------
// File:		GrMeshVB.h
// Author:		Kevin Bray
// Created:		12-10-05
//
// Purpose:		To manage a vertex buffer object.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrUtil.h"
#include "GrConfig.h"
#include "URef.h"
#include "UFastArray.h"

struct SVec3;
struct SVec2;
class MMat4x4;

//**********************************************************
// class GrMeshVB
//**********************************************************
class GrMeshVB : public URefCount
{
public:
	static const unsigned int kDynamic = 1;

	GrMeshVB( unsigned int vertexCount, const SVec3* positions, const SVec3* normals, const SVec3* biNormals,
			  const SVec3* tangents, const unsigned long* colors, const SVec2* texCoords, unsigned int flags,
			  unsigned int maxVertexCount = 0, bool cacheData = true );
	GrMeshVB( unsigned int vertexCount, unsigned int usedComponents, unsigned int flags, bool cacheData = true );
	~GrMeshVB();

	// return the vertex count.
	unsigned int		GetVertexCount() const		{	return _vertexCount;		}

	// return the max vertex count.
	unsigned int		GetMaxVertexCount() const	{	return _maxVertexCount;		}

	// adjust the number of active vertices
	void				AdjustVertexCount( int growSize );

	// returns used vertex components.
	unsigned int		GetUsedComponents() const	{	return _components;			}

	// returns creation flags.
	unsigned int		GetFlags() const			{	return _flags;				}

	// gets vertex data.  You must call MarkAsDirty() to cause data to be resent
	// to the graphics hardware.
	SVec3*				GetVertices() const			{	return _positions;			}
	SVec3*				GetNormals() const			{	return _normals;			}
	SVec3*				GetBiNormals() const		{	return _biNormals;			}
	SVec3*				GetTangents() const			{	return _tangents;			}
	unsigned long*		GetColors() const			{	return _colors;				}
	SVec2*				GetTexCoords() const		{	return _texCoords;			}

	// marks vertex data as dirty and causes the data to be sent to the graphics card.
	void				MarkAsDirty( unsigned int components );

	// writes data directly to the vertex buffer instead of writing to the cache, and
	// then uploading.  This can be used to achieve maximum performance.  However,
	// data must be ordered properly.  See vertex-flag bit positions for proper data
	// ordering.  Data types must match the types in the above Get* functions.
	void				UploadUncached( unsigned int writeOffset, const void* source, unsigned int sourceSize );

	// cache/evict mesh data.
	void				Cache();
	void				Evict();

	// bind the vertex buffer and it's components.
	void				Bind( unsigned int components );

	// context reset.
	void				PreContextReset();
	void				PostContextReset();

	// returns a list of vertex positions that have been transformed by the meshinst's transform.
	void				PretransformVertices( UFastArray< SVec3 >& results, const MMat4x4& transform );

private:
	void				PackData();

	unsigned int		_vertexBuffer;
	unsigned int		_vertexCount;
	// editor needs to adjust size of meshes, so we allow meshes with
	// extra space reserved
	unsigned int		_maxVertexCount;
	unsigned int		_components;
	unsigned int		_dirty;
	unsigned int		_flags;
	unsigned int		_stride;
	unsigned int		_size;

	// vertex data cache.
	SVec3*				_positions;
	SVec3*				_normals;
	SVec3*				_biNormals;
	SVec3*				_tangents;
	unsigned long*		_colors;
	SVec2*				_texCoords;

	// vertex data offsets.
	void*				_normalStart;
	void*				_biNormalStart;
	void*				_tangentStart;
	void*				_colorStart;
	void*				_texCoordStart;

	// true if data in the buffer is cached in system memory.
	bool				_cacheData;
};
