//----------------------------------------------------------
// File:		MMat4x4Array.h
// Author:		Kevin Bray
// Created:		01-27-06
//
// Purpose:		performs matrix operations on large arrays of matrices.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat4x4.h"

//**********************************************************
// class MMat4x4Array
//**********************************************************
class MMat4x4Array
{
public:
	MMat4x4Array( unsigned int initMatCount );
	~MMat4x4Array();

	unsigned int	GetMatrixCount() const		{	return _matCount;		}
	void			Resize( unsigned int matCount, bool preserve = true );

	// sets a single matrix.
	void			SetMatrix( unsigned int idx, const MMat4x4& matrix );
	MMat4x4			GetMatrix( unsigned int idx ) const;

	// assigns all matrices using the array passed in.  The array passed in
	// must be the same size or larger than this array.
	void			SetMatrices( const MMat4x4Array& array );

	// transforms the matrices by the matrix passed in.  Specifically, performs
	// dest = source * matrix
	void			PostTransform( MMat4x4Array& target, unsigned int dstIdx, unsigned int srcIdx, unsigned int count,
								   const MMat4x4& matrix ) const;

	// dest = matrix * source
	void			PreTransform( MMat4x4Array& target, unsigned int dstIdx, unsigned int srcIdx, unsigned int count,
								  const MMat4x4& matrix ) const;

	// there should be one index for every matrix in the array except for the
	// first.  This function goes through all matrices and pre-multiplies the
	// current matrix in place by the matrix referenced by the coresponding
	// index.  The first matrix is transformed by root.  Note that this is
	// mainly useful for propagating transforms.
	void			Propagate( unsigned int* indices, const MMat4x4& root = MMat4x4() );

	void			Save( UWriter& writer ) const;
	void			Load( UReader& reader );

private:
	void*			_memory;
	float*			_matrices;
	unsigned int	_matCount;
};
