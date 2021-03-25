//----------------------------------------------------------
// File:		GrSoftwareRasterizer.cpp
// Author:		Kevin Bray
// Created:		02-25-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrSoftwareRasterizer.h"

// project inludes.
#include "GrSoftwareRTBase.h"

#define SCRATCH_BUFFER_SIZE		32767
#define RENDER_TILE_SIZE		8
#define RENDER_TILE_MASK		0x7
#define RENDER_TILE_INV_MASK	0xFFFFFFF8

/*
// performs a/b and returns the resulting floating point number.
__forceinline float FastDivide( int a, int b )
{
	__declspec( align( 8 ) ) int values[ 2 ] = { a, b };
	int* valuesPtr = values;
	float result;
	__asm
	{
		mov			esi, valuesPtr
		cvtpi2ps	xmm0, [esi]
		movaps		xmm1, xmm0
		shufps		xmm1, xmm1,	00010001b
		divss		xmm0, xmm1
		movss		result, xmm0
	};
	return result;
}

// subtracts src1 from src0 and places the result in dst.  This operates on 12 floats.
__forceinline void CalcTriangleVaryingDeltas( SVec4* dst, const SVec4* src )
{
	__asm
	{
		mov			esi, src
		mov			edi, dst

		movaps		xmm0, [esi+0  ]
		movaps		xmm1, [esi+16 ]
		movaps		xmm2, [esi+32 ]
		movaps		xmm3, [esi+48 ]
		subps		xmm0, [esi+64 ]
		subps		xmm1, [esi+80 ]
		subps		xmm2, [esi+96 ]
		subps		xmm3, [esi+112]
		movaps		[edi+0  ], xmm0
		movaps		[edi+16 ], xmm1
		movaps		[edi+32 ], xmm2
		movaps		[edi+48 ], xmm3

		movaps		xmm0, [esi+64 ]
		movaps		xmm1, [esi+80 ]
		movaps		xmm2, [esi+96 ]
		movaps		xmm3, [esi+112]
		subps		xmm0, [esi+128]
		subps		xmm1, [esi+144]
		subps		xmm2, [esi+160]
		subps		xmm3, [esi+176]
		movaps		[edi+64 ], xmm0
		movaps		[edi+80 ], xmm1
		movaps		[edi+96 ], xmm2
		movaps		[edi+112], xmm3

		movaps		xmm0, [esi+128]
		movaps		xmm1, [esi+144]
		movaps		xmm2, [esi+160]
		movaps		xmm3, [esi+176]
		subps		xmm0, [esi+0  ]
		subps		xmm1, [esi+16 ]
		subps		xmm2, [esi+32 ]
		subps		xmm3, [esi+48 ]
		movaps		[edi+128], xmm0
		movaps		[edi+144], xmm1
		movaps		[edi+160], xmm2
		movaps		[edi+176], xmm3
	};
}

//----------------------------------------------------------
__forceinline void CalcLineVaryingDeltas( SVec4* dst, const SVec4* src )
{
	__asm
	{
		mov			esi, src
		mov			edi, dst

		movaps		xmm0, [esi+0  ]
		movaps		xmm1, [esi+16 ]
		movaps		xmm2, [esi+32 ]
		movaps		xmm3, [esi+48 ]
		subps		xmm0, [esi+64 ]
		subps		xmm1, [esi+80 ]
		subps		xmm2, [esi+96 ]
		subps		xmm3, [esi+112]
		movaps		[edi+0  ], xmm0
		movaps		[edi+16 ], xmm1
		movaps		[edi+32 ], xmm2
		movaps		[edi+48 ], xmm3
	}
}

//----------------------------------------------------------
__forceinline void
LerpVaryingDeltas( SVec4* output, const SVec4* input0, const SVec4* input1, float t )
{
	__asm
	{
		; setup pointers.
		mov			eax, input0;
		mov			esi, input1;
		mov			edi, output;

		; load the interpolant.
		movss		xmm0, t
		shufps		xmm0, xmm0, 00000000b
		movaps		xmm1, xmm0;
		movaps		xmm2, xmm0;

		; perform a series of multiply-adds.
		mulps		xmm0, [esi+0]
		addps		xmm0, [eax+0]
		mulps		xmm1, [esi+16]
		addps		xmm1, [eax+16]
		mulps		xmm2, [esi+32]
		addps		xmm2, [eax+32]

		movaps		[edi+0], xmm0
		movaps		[edi+16], xmm1
		movaps		[edi+32], xmm2
	};
}
*/

class IXNullRT : public GrSoftwareRTBase
{
public:
	IXNullRT()
	{
		_buffer = ( unsigned char* )AlignedAlloc( 16, RENDER_TILE_SIZE );
	}

	~IXNullRT()
	{
		AlignedFree( _buffer );
	}

	unsigned int		GetWidth() const			{	return 32767;		}
	unsigned int		GetHeight() const			{	return 32767;		}
	unsigned int		GetBytesPerPel() const		{	return 0;			}
	unsigned int		GetStride() const			{	return GetBytesPerPel() * GetWidth();			}
	unsigned int		GetTile8x8Stride() const	{	return GetBytesPerPel() * RENDER_TILE_SIZE;		}
	unsigned char*		BeginTile8x8( unsigned int x, unsigned int y )				{	return _buffer;		}
	unsigned char*		BeginTile8x8WriteOnly( unsigned int x, unsigned int y )		{	return _buffer;		}
	void				EndTile8x8( unsigned int x, unsigned int y )				{ }

	unsigned char*		GetData() const				{	return 0;			}

	// fills the buffer with the pelClearData.  Note that pelClearData must be
	// as large as _bytesPerPel.
	void				Clear( void* pelClearData );

private:
	unsigned char*		_buffer;
};

//**********************************************************
// class GrSoftwareRasterizer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrSoftwareRasterizer::GrSoftwareRasterizer()
: _viewportScale( 1.0f, 1.0f )
, _viewportOffset( 0.0f, 0.0f )
, _vertexProg( 0 )
, _fragmentProg( 0 )
, _nullRT( new IXNullRT() )
, _targetWidth( 0 )
, _targetHeight( 0 )
, _floatTargetWidth( 0 )
, _floatTargetHeight( 0 )
, _scratchBuffer( new SPolygonVertex[ SCRATCH_BUFFER_SIZE ] )
{
	// initialize all render-target arrays to null.
	MemSet( _renderTarget, 0, sizeof( _renderTarget ) );
	MemSet( _targetBPP, 0, sizeof( _targetBPP ) );
	MemSet( _blockStride, 0, sizeof( _blockStride ) );

	// set all render targets to the null render target.
	for ( unsigned int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
		_renderTarget[ i ] = _nullRT;
}

//----------------------------------------------------------
GrSoftwareRasterizer::~GrSoftwareRasterizer()
{
	delete _nullRT;

	// free the scratch vertex buffer.
	delete[] _scratchBuffer;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetRenderTarget( unsigned int targetIdx, GrSoftwareRTBase* target )
{
	// set to _nullRT if no render target is bound.
	if ( target == 0 )
		target = _nullRT;

	// make sure the render target passed in is valid.
	B_ASSERT( targetIdx < MAX_RENDER_TARGET_COUNT );

	// store the current render target.
	_renderTarget[ targetIdx ] = target;

	// grab information about currently bound render-targets.
	_targetWidth = 0x7FFFFFFF;
	_targetHeight = 0x7FFFFFFF;
	for ( unsigned int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
	{
		// get the current render target.
		GrSoftwareRTBase* curRT = _renderTarget[ i ];

		// store the current bpp.
		_targetBPP[ i ] = curRT->GetBytesPerPel();
		_blockStride[ i ] = curRT->GetTile8x8Stride();

		// check to see if there is any target here.
		if ( _renderTarget[ i ] == _nullRT )
			continue;

		// get information about the current render-target.  Note that
		// we use the smallest render target to determine the size of
		// the rendered image.
		_targetWidth = Min( _targetWidth, ( int )curRT->GetWidth() );
		_targetHeight = Min( _targetHeight, ( int )curRT->GetHeight() );
	}
	if ( _targetWidth == ~0 )
		_targetWidth = 0;
	if ( _targetHeight == ~0 )
		_targetHeight = 0;
	
	_floatTargetWidth = ( float )_targetWidth;
	_floatTargetHeight = ( float )_targetHeight;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetVertexProgram( VertexProgram vertexProg )
{
	B_ASSERT( vertexProg != 0 );
	_vertexProg = vertexProg;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetFragmentProgram( FragmentProgram fragmentProg )
{
	B_ASSERT( fragmentProg != 0 );
	_fragmentProg = fragmentProg;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetVertexConstant( unsigned int idx, const SVec4& value )
{
	B_ASSERT( idx < 64 );
	_vertexConstants[ idx ] = value;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetVertexConstants( unsigned int idx, const MMat4x4& value )
{
	B_ASSERT( idx < (64-4) );
	for ( unsigned int i = 0; i < 4; ++i )
	{
		_vertexConstants[ idx + i ].x = value( i, 0 );
		_vertexConstants[ idx + i ].y = value( i, 1 );
		_vertexConstants[ idx + i ].z = value( i, 2 );
		_vertexConstants[ idx + i ].w = value( i, 3 );
	}
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetFragmentConstant( unsigned int idx, const SVec4& value )
{
	B_ASSERT( idx < 64 );
	_fragmentConstants[ idx ] = value;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetFragmentConstants( unsigned int idx, const MMat4x4& value )
{
	B_ASSERT( idx < (64-4) );
	for ( unsigned int i = 0; i < 4; ++i )
	{
		_fragmentConstants[ idx + i ].x = value( i, 0 );
		_fragmentConstants[ idx + i ].y = value( i, 1 );
		_fragmentConstants[ idx + i ].z = value( i, 2 );
		_fragmentConstants[ idx + i ].w = value( i, 3 );
	}
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetViewportTransform( const SVec2& scale, const SVec2& offset )
{
	_viewportScale = scale;
	_viewportOffset = offset;
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetSamplerTexture( unsigned int idx, GrImage* image )
{
	B_ASSERT( idx < 16 );
	_samplers[ idx ].SetTexture( image );
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::SetSamplerColor( unsigned int idx, unsigned color )
{
	B_ASSERT( idx < 16 );
	_samplers[ idx ].SetColor( color );
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::RenderPolygon( const GrPolygon& polygon )
{
	B_ASSERT( _renderTarget != 0 );
	B_ASSERT( _vertexProg != 0 && _fragmentProg != 0 );

	// triangulate and pass to RenderTriangleList.
	unsigned int triCount = ( polygon.GetVertexCount() - 2 );
	unsigned int vertCount = 3 * triCount;

	// allocate space for our vertices.
	SPolygonVertex* vertices = ( SPolygonVertex* )alloca( vertCount * sizeof( SPolygonVertex ) );

	// triangulate the current polygon.
	polygon.Triangulate( vertices );

	// render the triangle list.
	RenderTriangleList( vertices, triCount );
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::RenderPolygons( const std::vector< GrPolygon* >& polygons )
{
	B_ASSERT( _renderTarget != 0 );
	B_ASSERT( _vertexProg != 0 && _fragmentProg != 0 );

	// unpack vertices into the scratch buffer.
	SPolygonVertex* curVertexOffset = _scratchBuffer;
	unsigned int curVertexCount = 0;
	unsigned int curTriCount = 0;
	unsigned int polygonCount = ( unsigned int )polygons.size();
	unsigned int i = 0;
	while( polygonCount-- > 0 )
	{
		// if the buffer can't hold the polygon, then we need to flush.
		GrPolygon* curPoly = polygons[ i ];
		if ( ( curVertexCount + curPoly->GetVertexCount() ) > SCRATCH_BUFFER_SIZE )
		{
			// flush.
			RenderTriangleList( _scratchBuffer, curTriCount );
			curVertexOffset = _scratchBuffer;
			curTriCount = 0;
			curVertexCount = 0;
		}

		// triangulate the polygon into the scratch buffer.
		curPoly->Triangulate( curVertexOffset );
		curVertexOffset += curPoly->GetVertexCount();

		// next polygon.
		++i;
	}

	// flush remaining polygons.
	if ( curTriCount )
		RenderTriangleList( _scratchBuffer, curTriCount );
}

//----------------------------------------------------------
void
GrSoftwareRasterizer::RenderTriangleList( const SPolygonVertex* vertices, unsigned int triCount )
{
	B_ASSERT( _vertexProg != 0 && _fragmentProg != 0 );
	for ( unsigned int i = 0; i < triCount; ++i )
	{
		// render the current triangle.
		RenderTriangle( vertices );

		// next triangle.
		vertices += 3;
	}
}


//==========================================================
// private methods
//==========================================================

#define iround( x )			( int )( ( x ) + 0.5 )

#include "xmmintrin.h"

// varyings available to vertices.
#define VARYING_COUNT		4

//		int DUDX;
//		int DUDY;
//		int DVDX;
//		int DVDY;

		// Calculate the derivatives of the uvs across the face of the triangle.
		// note that this has to be calculated for all varyings eventually, but
		// for now, we'll do something simple.  This way, I can gain an
		// understanding of the math, and then it should be easy to extend for
		// everything.
/*
		x0 * DUDX + y0 * DUDY = U0
		x1 * DUDX + y1 * DUDY = U1
		--------------------------

		solve using the system of equations with matrices.

		| x0  y0 |   | U0 |
		| x1  y1 | = | U1 |

		det = 1 / (y1*x0 - y0*x1);

					| y1 -y0 |
		inv = det * |-x1  x0 |

			  | U0 |
		inv * | U1 |
	    


		x0 * DVDX + y0 * DVDY = V0
		x1 * DVDX + y1 * DVDY = V1
		--------------------------

		int U1 = iround( 65536.0f * varyings[0].x );
		int U2 = iround( 65536.0f * varyings[4].x );
		int U3 = iround( 65536.0f * varyings[8].x );
		int V1 = iround( 65536.0f * varyings[0].y );
		int V2 = iround( 65536.0f * varyings[4].y );
		int V3 = iround( 65536.0f * varyings[8].y );
*/
/*		else
		{
			curVarying[ v ].x = varyings[2*VARYING_COUNT+v].x - varyings[1*VARYING_COUNT+v].x;
			curVarying[ v ].y = varyings[2*VARYING_COUNT+v].y - varyings[1*VARYING_COUNT+v].y;
			curVarying[ v ].z = varyings[2*VARYING_COUNT+v].z - varyings[1*VARYING_COUNT+v].z;
			curVarying[ v ].w = varyings[2*VARYING_COUNT+v].w - varyings[1*VARYING_COUNT+v].w;

			varyingDX[ v ].x = ( Y3 * curVarying[ v ].x  + -Y2 * curVarying[ v ].x ) / invDet2;
			varyingDX[ v ].y = ( Y3 * curVarying[ v ].y  + -Y2 * curVarying[ v ].y ) / invDet2;
			varyingDX[ v ].z = ( Y3 * curVarying[ v ].z  + -Y2 * curVarying[ v ].z ) / invDet2;
			varyingDX[ v ].w = ( Y3 * curVarying[ v ].w  + -Y2 * curVarying[ v ].w ) / invDet2;

			varyingDY[ v ].x = (-X3 * curVarying[ v ].x  +  X2 * curVarying[ v ].x ) / invDet2;
			varyingDY[ v ].y = (-X3 * curVarying[ v ].y  +  X2 * curVarying[ v ].y ) / invDet2;
			varyingDY[ v ].z = (-X3 * curVarying[ v ].z  +  X2 * curVarying[ v ].z ) / invDet2;
			varyingDY[ v ].w = (-X3 * curVarying[ v ].w  +  X2 * curVarying[ v ].w ) / invDet2;
		}
*/

//----------------------------------------------------------
void
GrSoftwareRasterizer::RenderTriangle( const SPolygonVertex* vertices )
{
	// execute the vertex shader.
	SVec4 verts[ 3 ];
	//static __declspec( align( 16 ) ) SVec4 varyings[ 12 ];
	ALIGNED_VAR( 16, SVec4 varyings[ 3 * VARYING_COUNT ] );

	// make sure the render target is valid.
	if ( _targetWidth == 0 || _targetHeight == 0 )
		return;

	// execute the vertex program for the 3 vertices.
	_vertexProg( verts[ 0 ], varyings + 0 * VARYING_COUNT, _vertexConstants, vertices[ 0 ] );
	_vertexProg( verts[ 1 ], varyings + 1 * VARYING_COUNT, _vertexConstants, vertices[ 1 ] );
	_vertexProg( verts[ 2 ], varyings + 2 * VARYING_COUNT, _vertexConstants, vertices[ 2 ] );

	// apply the viewport transform.
	verts[ 0 ].x = _viewportScale.x * verts[ 0 ].x + _viewportOffset.x;
	verts[ 0 ].y = _viewportScale.y * verts[ 0 ].y + _viewportOffset.y;
	verts[ 1 ].x = _viewportScale.x * verts[ 1 ].x + _viewportOffset.x;
	verts[ 1 ].y = _viewportScale.y * verts[ 1 ].y + _viewportOffset.y;
	verts[ 2 ].x = _viewportScale.x * verts[ 2 ].x + _viewportOffset.x;
	verts[ 2 ].y = _viewportScale.y * verts[ 2 ].y + _viewportOffset.y;

    // 28.4 fixed-point coordinates
    const int Y1 = iround(16.0f * ( 0.5f * verts[ 0 ].y + 0.5f ) * _floatTargetHeight);
    const int Y2 = iround(16.0f * ( 0.5f * verts[ 1 ].y + 0.5f ) * _floatTargetHeight);
    const int Y3 = iround(16.0f * ( 0.5f * verts[ 2 ].y + 0.5f ) * _floatTargetHeight);
    const int X1 = iround(16.0f * ( 0.5f * verts[ 0 ].x + 0.5f ) * _floatTargetWidth);
    const int X2 = iround(16.0f * ( 0.5f * verts[ 1 ].x + 0.5f ) * _floatTargetWidth);
    const int X3 = iround(16.0f * ( 0.5f * verts[ 2 ].x + 0.5f ) * _floatTargetWidth);

    // Bounding rectangle
    int minx = (Min3(X1, X2, X3) + 0xF) >> 4;
    int maxx = (Max3(X1, X2, X3) + 0xF) >> 4;
    int miny = (Min3(Y1, Y2, Y3) + 0xF) >> 4;
    int maxy = (Max3(Y1, Y2, Y3) + 0xF) >> 4;

	// early out if the triangle is degenerate.
//	if ( minx == maxx || miny == maxy )
//		return;

	// calculate the height.
	int height = maxy - miny;

	// calculate edges.
	const float EX1 = float( X2 - X1 ) / 16.0f;
	const float EX2 = float( X3 - X1 ) / 16.0f;
	const float EY1 = float( Y2 - Y1 ) / 16.0f;
	const float EY2 = float( Y3 - Y1 ) / 16.0f;

	// matrix determinant for finding the inverse of the coefficient matrix.
	float invDet = 1.0f / ( float )( EY2*EX1 - EY1*EX2 );

	// the derivatives.
	ALIGNED_VAR( 16, SVec4 varyingDX[ VARYING_COUNT ] );	// DsDx, DtDx, DpDx, DqDx
	ALIGNED_VAR( 16, SVec4 varyingDY[ VARYING_COUNT ] );	// DsDy, DtDy, DpDy, DqDy
	ALIGNED_VAR( 16, SVec4 curVarying[ VARYING_COUNT ] );
	ALIGNED_VAR( 16, SVec4 curPelVarying[ VARYING_COUNT ] );
	for ( int v = 0; v < VARYING_COUNT; ++v )
	{
		float VDX1 = varyings[ 1 * VARYING_COUNT + v ].x - varyings[ v ].x;
		float VDX2 = varyings[ 2 * VARYING_COUNT + v ].x - varyings[ v ].x;
		float VDY1 = varyings[ 1 * VARYING_COUNT + v ].y - varyings[ v ].y;
		float VDY2 = varyings[ 2 * VARYING_COUNT + v ].y - varyings[ v ].y;
		float VDZ1 = varyings[ 1 * VARYING_COUNT + v ].z - varyings[ v ].z;
		float VDZ2 = varyings[ 2 * VARYING_COUNT + v ].z - varyings[ v ].z;
		float VDW1 = varyings[ 1 * VARYING_COUNT + v ].w - varyings[ v ].w;
		float VDW2 = varyings[ 2 * VARYING_COUNT + v ].w - varyings[ v ].w;

		varyingDX[ v ].x = invDet * ( EY2 * VDX1  + -EY1 * VDX2 );
		varyingDX[ v ].y = invDet * ( EY2 * VDY1  + -EY1 * VDY2 );
		varyingDX[ v ].z = invDet * ( EY2 * VDZ1  + -EY1 * VDZ2 );
		varyingDX[ v ].w = invDet * ( EY2 * VDW1  + -EY1 * VDW2 );

		varyingDY[ v ].x = invDet * (-EX2 * VDX1  +  EX1 * VDX2 );
		varyingDY[ v ].y = invDet * (-EX2 * VDY1  +  EX1 * VDY2 );
		varyingDY[ v ].z = invDet * (-EX2 * VDZ1  +  EX1 * VDZ2 );
		varyingDY[ v ].w = invDet * (-EX2 * VDW1  +  EX1 * VDW2 );

		// calculate the varying at the origin of the screen.
		curVarying[ v ] =  varyings[ v ];
		curVarying[ v ] += -float( X1 >> 4 ) * varyingDX[ v ];
		curVarying[ v ] += -float( Y1 >> 4 ) * varyingDY[ v ];
	}

	// Deltas
    int DX12 = X1 - X2;
    int DX23 = X2 - X3;
    int DX31 = X3 - X1;

    int DY12 = Y1 - Y2;
    int DY23 = Y2 - Y3;
    int DY31 = Y3 - Y1;

    // Half-edge constants
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY23 * X2 - DX23 * Y2;
    int C3 = DY31 * X3 - DX31 * Y3;

	// check the "outside" triangle point against the first edge.  If
	// the triangle is backfacing, this will reverse the face.
    if ( C1 + DX12 * Y3 - DY12 * X3 < 0 )
	{
		DX12 = -DX12;
		DX23 = -DX23;
		DX31 = -DX31;
		DY12 = -DY12;
		DY23 = -DY23;
		DY31 = -DY31;
		C1 = -C1;
		C2 = -C2;
		C3 = -C3;
	}

	if ( _lastPixel )
	{
		if ( DY12 < 0 || ( DY12 == 0 && DX12 > 0 ) )
			C1 += 2*65536;

		if ( DY23 < 0 || ( DY23 == 0 && DX23 > 0 ) )
			C2 += 2*65536;

		if ( DY31 < 0 || ( DY31 == 0 && DX31 > 0 ) )
			C3 += 2*65536;
	}

    // Correct for fill convention
	if ( DY12 < 0 || ( DY12 == 0 && DX12 > 0 ) )
		++C1;
	if ( DY23 < 0 || ( DY23 == 0 && DX23 > 0 ) )
		++C2;
	if ( DY31 < 0 || ( DY31 == 0 && DX31 > 0 ) )
		++C3;

    // Fixed-point deltas
    const int FDX12 = DX12 << 4;
    const int FDX23 = DX23 << 4;
    const int FDX31 = DX31 << 4;

    const int FDY12 = DY12 << 4;
    const int FDY23 = DY23 << 4;
    const int FDY31 = DY31 << 4;

    // Block size, standard 8x8 (must be power of two and greater than 4)
    const int q = RENDER_TILE_SIZE;

    // Start in corner of 8x8 block
    minx &= RENDER_TILE_INV_MASK;
    miny &= RENDER_TILE_INV_MASK;

	// clip.
	minx = Max( minx, 0 );
	miny = Max( miny, 0 );
	maxx = Min( maxx, _targetWidth );
	maxy = Min( maxy, _targetHeight );

	// destination render target pointers.
    unsigned char* dst[ MAX_RENDER_TARGET_COUNT ];

    // Loop through blocks
    for(int y = miny; y < maxy; y += RENDER_TILE_SIZE )
    {
        int y0 = y << 4;
        int y1 = (y + RENDER_TILE_MASK) << 4;
		int C1_DX12_y0 = C1 + DX12 * y0;
		int C1_DX12_y1 = C1 + DX12 * y1;
		int C2_DX23_y0 = C2 + DX23 * y0;
		int C2_DX23_y1 = C2 + DX23 * y1;
		int C3_DX31_y0 = C3 + DX31 * y0;
		int C3_DX31_y1 = C3 + DX31 * y1;

		for(int x = minx; x < maxx; x += RENDER_TILE_SIZE)
        {
            // Corners of block
            int x0 = x << 4;
            int x1 = (x + RENDER_TILE_MASK) << 4;

            // Evaluate half-space functions
            bool a00 = C1_DX12_y0 - DY12 * x0 > 0;
            bool a10 = C1_DX12_y0 - DY12 * x1 > 0;
            bool a01 = C1_DX12_y1 - DY12 * x0 > 0;
            bool a11 = C1_DX12_y1 - DY12 * x1 > 0;
            int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

            bool b00 = C2_DX23_y0 - DY23 * x0 > 0;
            bool b10 = C2_DX23_y0 - DY23 * x1 > 0;
            bool b01 = C2_DX23_y1 - DY23 * x0 > 0;
            bool b11 = C2_DX23_y1 - DY23 * x1 > 0;
            int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

            bool c00 = C3_DX31_y0 - DY31 * x0 > 0;
            bool c10 = C3_DX31_y0 - DY31 * x1 > 0;
            bool c01 = C3_DX31_y1 - DY31 * x0 > 0;
            bool c11 = C3_DX31_y1 - DY31 * x1 > 0;
            int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

            // Skip block when outside an edge.
            if ( a == 0x0 || b == 0x0 || c == 0x0 )
				continue;

			// get the current destination pixels.
			for ( int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
				dst[ i ] = _renderTarget[ i ]->BeginTile8x8( x, y );

            // Accept whole block when totally covered
            if ( a == 0xF && b == 0xF && c == 0xF )
            {
                for(int iy = y; iy < ( y + RENDER_TILE_SIZE ); ++iy)
                {
					// instead of using a for loop, use multiple threads to execute
					// this 8 times.  That way, we'll have 8 pixels in flight at a
					// time.  They'll have the same performance characteristics as
					// operating in lock-step, but that's fine.
					for(int ix = x; ix < x + RENDER_TILE_SIZE; ++ix /*, curU += DUDY, curV += DUDY*/)
                    {
						__asm
						{
							; setup our operation.
							mov			ecx, VARYING_COUNT
							lea			esi, curVarying
							lea			edx, varyingDX
							lea			eax, varyingDY
							lea			edi, curPelVarying

							; convert ix and iy to floating point.
							movd		xmm3, ix
							pshufd		xmm3, xmm3, 00000000b
							movd		xmm4, iy
							pshufd		xmm4, xmm4, 00000000b
							cvtdq2ps	xmm3, xmm3
							cvtdq2ps	xmm4, xmm4

						$loop:
							movaps		xmm0, [esi]
							movaps		xmm1, [edx]
							movaps		xmm2, [eax]

							mulps		xmm1, xmm3
							mulps		xmm2, xmm4
							addps		xmm0, xmm1
							addps		xmm0, xmm2

							movaps		[edi], xmm0

							add			esi, 16
							add			edx, 16
							add			eax, 16
							add			edi, 16
							dec			ecx
							jnz $loop
						}
//						for (int v = 0; v < VARYING_COUNT; ++v)
//						{
//							curPelVarying[ v ] =  curVarying[ v ];
//							curPelVarying[ v ] += float( ix ) * varyingDX[ v ];
//							curPelVarying[ v ] += float( iy ) * varyingDY[ v ];
//						}

						// run the fragment shader.
						_fragmentProg( dst, _targetBPP, _fragmentConstants, curPelVarying, varyingDX, varyingDY, _samplers );

						// advance the destination pointers.
						for ( unsigned int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
							dst[ i ] += _targetBPP[ i ];
                    }
                }
            }
            else	// Partially covered block
            {
                int CY1 = C1 + DX12 * y0 - DY12 * x0;
                int CY2 = C2 + DX23 * y0 - DY23 * x0;
                int CY3 = C3 + DX31 * y0 - DY31 * x0;

                for(int iy = y; iy < ( y + RENDER_TILE_SIZE ); ++iy)
                {
                    int CX1 = CY1;
                    int CX2 = CY2;
                    int CX3 = CY3;

                    for( int ix = x; ix < x + RENDER_TILE_SIZE; ++ix )
                    {
                        if ( CX1 > 0 && CX2 > 0 && CX3 > 0 )
                        {
							// calculate varyings the crappy, slow way.
							__asm
							{
								; setup our operation.
								mov			ecx, VARYING_COUNT
								lea			esi, curVarying
								lea			edx, varyingDX
								lea			eax, varyingDY
								lea			edi, curPelVarying

								; convert ix and iy to floating point.
								movd		xmm3, ix
								pshufd		xmm3, xmm3, 00000000b
								movd		xmm4, iy
								pshufd		xmm4, xmm4, 00000000b
								cvtdq2ps	xmm3, xmm3
								cvtdq2ps	xmm4, xmm4

							$loop2:
								movaps		xmm0, [esi]
								movaps		xmm1, [edx]
								movaps		xmm2, [eax]

								mulps		xmm1, xmm3
								mulps		xmm2, xmm4
								addps		xmm0, xmm1
								addps		xmm0, xmm2

								movaps		[edi], xmm0

								add			esi, 16
								add			edx, 16
								add			eax, 16
								add			edi, 16
								dec			ecx
								jnz $loop2
							}

							// run the fragment shader.
							_fragmentProg( dst, _targetBPP, _fragmentConstants, curPelVarying, varyingDX, varyingDY, _samplers );
                        }

						// advance the destination pointers.
						for ( unsigned int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
							dst[ i ] += _targetBPP[ i ];

						CX1 -= FDY12;
                        CX2 -= FDY23;
                        CX3 -= FDY31;
                    }

                    CY1 += FDX12;
                    CY2 += FDX23;
                    CY3 += FDX31;
                }
            }

			// flush the render target's contents.
			for ( int i = 0; i < MAX_RENDER_TARGET_COUNT; ++i )
				_renderTarget[ i ]->EndTile8x8( x, y );
        }
    }
}
