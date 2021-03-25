//----------------------------------------------------------
// File:		MMat4x4Array.cpp
// Author:		Kevin Bray
// Created:		01-27-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MMat4x4Array.h"

// project includes.
#include "UReader.h"
#include "UWriter.h"

// useful constants.
static const unsigned int kMatSize = 16 * sizeof( float );

//**********************************************************
// class MMat4x4Array
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MMat4x4Array::MMat4x4Array( unsigned int matCount )
: _matCount( matCount )
, _memory( 0 )
, _matrices( 0 )
{
	// allocate the memory we need to store the matrices.
	_memory = new char[ matCount * kMatSize + kMatSize * 16 - 1 ];

	// align to a 16 byte boundry by clipping off the low 4 bits.
	_matrices = ( float* )( ( ( ( size_t )_memory ) + 0xF ) & ~0xF );

	// initialize all matrices to identity.
	for ( unsigned int i = 0; i < matCount; ++i )
	{
		_matrices[ 0 ] = 1.0f;
		_matrices[ 1 ] = 0.0f;
		_matrices[ 2 ] = 0.0f;
		_matrices[ 3 ] = 0.0f;

		_matrices[ 4 ] = 0.0f;
		_matrices[ 5 ] = 1.0f;
		_matrices[ 6 ] = 0.0f;
		_matrices[ 7 ] = 0.0f;

		_matrices[ 8 ] = 0.0f;
		_matrices[ 9 ] = 0.0f;
		_matrices[ 10 ] = 1.0f;
		_matrices[ 11 ] = 0.0f;

		_matrices[ 12 ] = 0.0f;
		_matrices[ 13 ] = 0.0f;
		_matrices[ 14 ] = 0.0f;
		_matrices[ 15 ] = 1.0f;
	}
}

//----------------------------------------------------------
MMat4x4Array::~MMat4x4Array()
{
	delete[] (char*)_memory;
}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
MMat4x4Array::Resize( unsigned int matCount, bool preserve )
{
	// more than 1 billion?!  wtf?  smells fishy.
	B_ASSERT( matCount < 0x10000000 );

	unsigned int matMemSize = matCount * kMatSize;
	void* newMem = new char[ matMemSize + kMatSize - 1 ];

	// align to a 16 byte boundry by clipping off the low 4 bits.
	float* newMats = ( float* )Align16( newMem );

	if ( preserve )
		MemCopy( newMats, _matrices, _matCount * kMatSize );

	// free the old memory and reassign.
	delete[] (char*)_memory;
	_memory = newMem;
	_matrices = newMats;
	_matCount = matCount;
}

//----------------------------------------------------------
void
MMat4x4Array::SetMatrix( unsigned int idx, const MMat4x4& matrix )
{
	// ensure that the index is valid.
	B_ASSERT( idx < _matCount );

	// set the matrix.
	const float* srcData = matrix;
	for ( unsigned int i = 0; i < 16; ++i )
		_matrices[ idx * 16 + i ] = srcData[ i ];
}

//----------------------------------------------------------
MMat4x4
MMat4x4Array::GetMatrix( unsigned int idx ) const
{
	// ensure that the index is valid.
	B_ASSERT( idx < _matCount );

	// return the matrix.
	return MMat4x4(
		_matrices[ idx * 16 ],
		_matrices[ idx * 16 + 1 ],
		_matrices[ idx * 16 + 2 ],
		_matrices[ idx * 16 + 3 ],
		_matrices[ idx * 16 + 4 ],
		_matrices[ idx * 16 + 5 ],
		_matrices[ idx * 16 + 6 ],
		_matrices[ idx * 16 + 7 ],
		_matrices[ idx * 16 + 8 ],
		_matrices[ idx * 16 + 9 ],
		_matrices[ idx * 16 + 10 ],
		_matrices[ idx * 16 + 11 ],
		_matrices[ idx * 16 + 12 ],
		_matrices[ idx * 16 + 13 ],
		_matrices[ idx * 16 + 14 ],
		_matrices[ idx * 16 + 15 ]
	);
}

//----------------------------------------------------------
void
MMat4x4Array::SetMatrices( const MMat4x4Array& array )
{
	B_ASSERT( array._matCount >= _matCount );
	MemCopy( _matrices, array._matrices, _matCount * kMatSize );
}

//----------------------------------------------------------
void
MMat4x4Array::PostTransform( MMat4x4Array& target, unsigned int dstIdx, unsigned int srcIdx, unsigned int count,
							 const MMat4x4& matrix ) const
{
	// cannot pass in this pointer for a destination.
	B_ASSERT( &target != this );

	// make sure the matrix indices are valid!
	B_ASSERT( srcIdx < _matCount );
	B_ASSERT( dstIdx < target._matCount );

	// make sure we aren't stepping out of bounds!
	B_ASSERT( (srcIdx+count) <= _matCount );
	B_ASSERT( (dstIdx+count) <= target._matCount );

	// burn through the matrices and transform.
	float* dstPtr = target._matrices + dstIdx * 16;
	float* srcPtr = _matrices + srcIdx * 16;
	for ( unsigned int matIdx = 0; matIdx < count; ++matIdx )
	{
		// for each row.
		for ( unsigned int i = 0; i < 4; ++i, srcPtr += 4 )
		{
			// for each column.
			for ( unsigned int j = 0; j < 4; ++j, ++dstPtr )
			{
				*dstPtr = 0;
				for ( unsigned int k = 0; k < 4; ++k )
					*dstPtr += srcPtr[ k ] * matrix( k, j );
			}
		}
	}
}

//----------------------------------------------------------
void
MMat4x4Array::PreTransform( MMat4x4Array& target, unsigned int dstIdx, unsigned int srcIdx, unsigned int count,
							 const MMat4x4& matrix ) const
{
	// cannot pass in this pointer for a destination.
	B_ASSERT( &target != this );

	// make sure the matrix indices are valid!
	B_ASSERT( srcIdx < _matCount );
	B_ASSERT( dstIdx < target._matCount );

	// make sure we aren't stepping out of bounds!
	B_ASSERT( (srcIdx+count) <= _matCount );
	B_ASSERT( (dstIdx+count) <= target._matCount );

	// burn through the matrices and transform.
	float* dstPtr = target._matrices + dstIdx * 16;
	float* srcPtr = _matrices + srcIdx * 16;
	for ( unsigned int matIdx = 0; matIdx < count; ++matIdx )
	{
		// for each row.
		for ( unsigned int i = 0; i < 4; ++i )
		{
			// for each column.
			for ( unsigned int j = 0; j < 4; ++j, ++dstPtr )
			{
				*dstPtr = 0;
				for ( unsigned int k = 0; k < 4; ++k )
					*dstPtr += srcPtr[ 4*k+j ] * matrix( i, k );
			}
		}
	}
}

//----------------------------------------------------------
void
MMat4x4Array::Propagate( unsigned int* indices, const MMat4x4& root )
{
	// transform the first matrix in place.
	float temp[ 16 ];

	// for each row.
	for ( unsigned int i = 0; i < 4; ++i )
	{
		// for each column.
		for ( unsigned int j = 0; j < 4; ++j )
		{
			temp[ i*4+j ] = 0;
			for ( unsigned int k = 0; k < 4; ++k )
				temp[ i*4+j ] += _matrices[ 4*i+k ] * root( k, j );
		}
	}

	// copy back to the source.
	for ( unsigned int i = 0; i < 16; ++i )
		_matrices[ i ] = temp[ i ];

	// burn through the matrices and transform.
	for ( unsigned int matIdx = 1; matIdx < _matCount; ++matIdx )
	{
		// if no index, skip.
		if ( indices[ matIdx ] & 0x80000000 )
			continue;

		B_ASSERT( indices[ matIdx ] < matIdx );

		// for each row.
		float* lhsPtr = _matrices + matIdx * 16;
		float* rhsPtr = _matrices + indices[ matIdx ] * 16;
		for ( unsigned int i = 0; i < 4; ++i )
		{
			// for each column.
			for ( unsigned int j = 0; j < 4; ++j )
			{
				temp[ i*4+j ] = 0;
				for ( unsigned int k = 0; k < 4; ++k )
					temp[ i*4+j ] += lhsPtr[ 4*i+k ] * rhsPtr[ 4*k+j ];
			}
		}

		// copy back to the source.
		for ( unsigned int i = 0; i < 16; ++i )
			lhsPtr[ i ] = temp[ i ];
	}
}

//----------------------------------------------------------
void
MMat4x4Array::Save( UWriter& writer ) const
{
	writer.WriteInt( _matCount );
	writer.WriteBytes( _matrices, _matCount * kMatSize );
}

//----------------------------------------------------------
void
MMat4x4Array::Load( UReader& reader )
{
	// resize the matrix array.
	Resize( reader.ReadInt(), false );
	reader.ReadBytes( _matrices, _matCount * kMatSize );
}
