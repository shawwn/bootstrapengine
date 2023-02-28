#include "graphics_afx.h"

// class header.
#include "GrMeshVB.h"

// project includes.
#include "GrTypes.h"
#include "GLSubsys.h"
#include "GrConfig.h"
#include "MMat4x4.h"

#define MAKE_ADDRESS( base, offset )		( GLintptr )( ( ( unsigned char* )(base) ) + (offset) )

// copies components in an array.
template < typename _T >
_T* CopyArray( const _T* src, unsigned int count, unsigned int maxCount )
{
	// return NULL if necessary.
	if ( !count || src == 0 )
		return 0;

	// allocate a new array and return.
	_T* a = new _T[ Max( count, maxCount ) ];
	MemCopy( a, src, count * sizeof( _T ) );
	return a;
}

//**********************************************************
// class GrVertexBuffer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrMeshVB::GrMeshVB( unsigned int vertexCount, const SVec3* positions, const SVec3* normals, const SVec3* biNormals,
				    const SVec3* tangents, const unsigned long* colors, const SVec2* texCoords, unsigned int flags,
					unsigned int maxVertexCount, bool cacheData )
: _vertexBuffer( 0 )
, _vertexCount( vertexCount )
, _components( 0 )
, _flags( flags )
, _dirty( _cacheData ? ~0 : 0 )
, _positions( cacheData ? CopyArray( positions, vertexCount, maxVertexCount ) : 0 )
, _normals( cacheData ? CopyArray( normals, vertexCount, maxVertexCount ) : 0 )
, _biNormals( cacheData ? CopyArray( biNormals, vertexCount, maxVertexCount ) : 0 )
, _tangents( cacheData ? CopyArray( tangents, vertexCount, maxVertexCount ) : 0 )
, _colors( cacheData ? CopyArray( colors, vertexCount, maxVertexCount ) : 0 )
, _texCoords( cacheData ? CopyArray( texCoords, vertexCount, maxVertexCount ) : 0 )
, _normalStart( 0 )
, _biNormalStart( 0 )
, _tangentStart( 0 )
, _colorStart( 0 )
, _texCoordStart( 0 )
, _stride( 0 )
, _size( 0 )
, _maxVertexCount( Max( maxVertexCount, vertexCount ) )
, _cacheData( cacheData )
{
	// Max vertex size: 12 + 12 + 12 + 12 + 4 + 8 = 68.
	if ( positions )
	{
		_components |= GR_ATTRIB_POSITION_MASK;
		_stride += 12;
	}

	if ( tangents )
	{
		_components |= GR_ATTRIB_TANGENT_MASK;
		_tangentStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( biNormals )
	{
		_components |= GR_ATTRIB_BINORMAL_MASK;
		_biNormalStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( normals )
	{
		_components |= GR_ATTRIB_NORMAL_MASK;
		_normalStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( texCoords )
	{
		_components |= GR_ATTRIB_TEXCOORD_MASK;
		_texCoordStart = ( void* )( size_t )_stride;
		_stride += 8;
	}

	if ( colors )
	{
		_components |= GR_ATTRIB_COLOR_MASK;
		_colorStart = ( void* )( size_t )_stride;
		_stride += 4;
	}

	// allocate the space required to fit all of the vertex data.
	_size = _stride * _maxVertexCount;
}

//----------------------------------------------------------
GrMeshVB::GrMeshVB( unsigned int vertexCount, unsigned int usedComponents, unsigned int flags, bool cacheData )
: _vertexBuffer( 0 )
, _vertexCount( vertexCount )
, _components( usedComponents )
, _flags( flags )
, _dirty( _cacheData ? ~0 : 0 )
, _positions( 0 )
, _normals( 0 )
, _biNormals( 0 )
, _tangents( 0 )
, _colors( 0 )
, _texCoords( 0 )
, _normalStart( 0 )
, _biNormalStart( 0 )
, _tangentStart( 0 )
, _colorStart( 0 )
, _texCoordStart( 0 )
, _stride( 0 )
, _size( 0 )
, _maxVertexCount( vertexCount )
, _cacheData( cacheData )
{
	// Max vertex size: 12 + 12 + 12 + 12 + 4 + 8 = 68.
	if ( _components & GR_ATTRIB_POSITION_MASK )
	{
		if ( cacheData )
			_positions = new SVec3[ vertexCount ];
		_stride += 12;
	}

	if ( _components & GR_ATTRIB_TANGENT_MASK )
	{
		if ( cacheData )
			_tangents = new SVec3[ vertexCount ];
		_tangentStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( _components & GR_ATTRIB_BINORMAL_MASK )
	{
		if ( cacheData )
			_biNormals = new SVec3[ vertexCount ];
		_biNormalStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( _components & GR_ATTRIB_NORMAL_MASK )
	{
		if ( cacheData )
			_normals = new SVec3[ vertexCount ];
		_normalStart = ( void* )( size_t )_stride;
		_stride += 12;
	}

	if ( _components & GR_ATTRIB_TEXCOORD_MASK )
	{
		if ( cacheData )
			_texCoords = new SVec2[ vertexCount ];
		_texCoordStart = ( void* )( size_t )_stride;
		_stride += 8;
	}

	if ( _components & GR_ATTRIB_COLOR_MASK )
	{
		if ( cacheData )
			_colors = new unsigned long[ vertexCount ];
		_colorStart = ( void* )( size_t )_stride;
		_stride += 4;
	}

	// allocate the space required to fit all of the vertex data.
	_size = _stride * _maxVertexCount;
}

//----------------------------------------------------------
GrMeshVB::~GrMeshVB()
{
	delete[] _positions;
	delete[] _normals;
	delete[] _biNormals;
	delete[] _tangents;
	delete[] _colors;
	delete[] _texCoords;

	if ( _vertexBuffer )
		GrDeleteVB( _vertexBuffer );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrMeshVB::AdjustVertexCount( int growSize )
{
	// calculate the new vertex count.
	int t = _vertexCount + growSize;
	B_ASSERT( ( ( t > 0 ) || ( t < ( int )_maxVertexCount ) ) );

	// adjust the total vertex count.
	_vertexCount = t;

	// force a rebuild the vertex buffer.
	Evict();
}

//----------------------------------------------------------
void
GrMeshVB::MarkAsDirty( unsigned int components )
{
	B_ASSERT( _cacheData );
	_dirty = components & _components;
}

//----------------------------------------------------------
void
GrMeshVB::UploadUncached( unsigned int writeOffset, const void* source, unsigned int sourceSize )
{
	Cache();
	GrBindVB( _vertexBuffer );
	bglBufferSubData( GL_ARRAY_BUFFER, writeOffset, sourceSize, source );
}

//----------------------------------------------------------
void
GrMeshVB::Cache()
{
	if ( !_vertexBuffer )
	{
		// if we're managing data, then set the dirty flags.
		if ( _cacheData )
			_dirty = ~0;

		// build the vertex buffer.
		_vertexBuffer = GrGenVB();
		GrBindVB( _vertexBuffer );
		bglBufferData( GL_ARRAY_BUFFER, _size, 0, ( ( _flags & kDynamic ) != 0 ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}
}

//----------------------------------------------------------
void
GrMeshVB::Evict()
{
	if ( _vertexBuffer )
	{
		GrDeleteVB( _vertexBuffer );
		_vertexBuffer = 0;
	}
}

//----------------------------------------------------------
void
GrMeshVB::Bind( unsigned int components )
{
	// if this hits, then the vertex buffer does not contain one or more
	// of the required components!
	B_ASSERT( ( components & _components ) == components );

	// cache the vertex buffer if necessary.
	if ( !_vertexBuffer )
		Cache();

	// bind our vertex buffer.
	GrBindVB( _vertexBuffer );

	// if we need to be updated, then do so now.
	if ( _dirty != 0 )
	{
		PackData();
		_dirty = 0;
	}

	// bind components.
	if ( components & GR_ATTRIB_POSITION_MASK )
		GrStreamArrayPointer( GR_ATTRIB_POSITION_INDEX, 3, ET_FLOAT, false, _stride, 0 );

	if ( components & GR_ATTRIB_TANGENT_MASK )
		GrStreamArrayPointer( GR_ATTRIB_TANGENT_INDEX, 3, ET_FLOAT, false, _stride, _tangentStart );

	if ( components & GR_ATTRIB_BINORMAL_MASK )
		GrStreamArrayPointer( GR_ATTRIB_BINORMAL_INDEX, 3, ET_FLOAT, false, _stride, _biNormalStart );

	if ( components & GR_ATTRIB_NORMAL_MASK )
		GrStreamArrayPointer( GR_ATTRIB_NORMAL_INDEX, 3, ET_FLOAT, false, _stride, _normalStart );

	if ( components & GR_ATTRIB_TEXCOORD_MASK )
		GrStreamArrayPointer( GR_ATTRIB_TEXCOORD_INDEX, 2, ET_FLOAT, false, _stride, _texCoordStart );

	if ( components & GR_ATTRIB_COLOR_MASK )
		GrStreamArrayPointer( GR_ATTRIB_COLOR_INDEX, 4, ET_UNSIGNED_BYTE, true, _stride, _colorStart );

	// enable arrays.
	GrStreamSetArrayState( components );
}

//----------------------------------------------------------
void
GrMeshVB::PreContextReset()
{
	// free our vertex buffer.
	if ( _vertexBuffer )
		GrDeleteVB( _vertexBuffer );
}

//----------------------------------------------------------
void
GrMeshVB::PostContextReset() 
{
	// reallocate our vertex buffer.
	if ( _vertexBuffer )
	{
		_vertexBuffer = 0;
		Cache();
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrMeshVB::PackData()
{
	// bind the vertex buffer.
	B_ASSERT( GrGetVB() == _vertexBuffer );

	// map the buffer to upload data.
	// TODO: Change this so that it can use GL_WRITE_ONLY_ARB.  We'll
	// need to use the movnta instruction to write that memory to
	// avoid reads into the cache.
	unsigned char* buffer = ( unsigned char* )bglMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB );
	unsigned char* mem = buffer;
	CHECK_GL();

	// upload data.
	for ( unsigned int vertex = 0; vertex < _vertexCount; ++vertex )
	{
		if ( _components & GR_ATTRIB_POSITION_MASK )
		{
			*( ( SVec3* )mem ) = _positions[ vertex ];
			mem += 12;
		}

		if ( _components & GR_ATTRIB_TANGENT_MASK )
		{
			*( ( SVec3* )mem ) = _tangents[ vertex ];
			mem += 12;
		}

		if ( _components & GR_ATTRIB_BINORMAL_MASK )
		{
			*( ( SVec3* )mem ) = _biNormals[ vertex ];
			mem += 12;
		}

		if ( _components & GR_ATTRIB_NORMAL_MASK )
		{
			*( ( SVec3* )mem ) = _normals[ vertex ];
			mem += 12;
		}

		if ( _components & GR_ATTRIB_TEXCOORD_MASK )
		{
			*( ( SVec2* )mem ) = _texCoords[ vertex ];
			mem += 8;
		}

		if ( _components & GR_ATTRIB_COLOR_MASK )
		{
			*( ( unsigned long* )mem ) = _colors[ vertex ];
			mem += 4;
		}
	}

	B_ASSERT( ( size_t )( mem - buffer ) <= ( size_t )_size );

	// unmap the buffer.
	GLboolean success = bglUnmapBuffer( GL_ARRAY_BUFFER );
	B_ASSERT( success );
}

//----------------------------------------------------------
void
GrMeshVB::PretransformVertices( UFastArray< SVec3 >& results, const MMat4x4& transform )
{
	// allocate enough space to contain the vertices.
	results.Reserve( results.GetReserved() + _vertexCount );

	// transform each vertex by the meshinst's transform.
	for ( unsigned int i = 0; i < _vertexCount; ++i )
	{
		SVec3 transformed( transform.TransformCoordNoPersp( _positions[ i ] ) );
		results.Push( transformed );
	}
}
