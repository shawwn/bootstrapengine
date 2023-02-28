//----------------------------------------------------------
// File:		MSVD.cpp
// Author:		Kevin Bray
// Created:		01-12-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// Notes:
// Special thanks to the national institute of standards and
// technology for providing the initial implementation of the SVD
// operation.  Original source code came from the Template Numerical
// Toolkit and can be found at http://math.nist.gov/ .

// class header.
#include "MSVD.h"

// C++ includes.
#include <algorithm>

inline float hypot( float a, float b )
{
	// if a is 0.0, then simply return b.
	if ( a == 0.0f )
		return Abs( b );

	// return b since a has no length.
	double c = b / ( double )a;
	return ( float )( AbsD( a ) * SqrtD( 1.0 + c*c ) );
}

inline float AbsMax5( float a, float b, float c, float d, float e )
{
	return Max( Max( Max( Max( Abs( a ), Abs( b ) ), Abs( c ) ), Abs( d ) ), Abs( e ) );
}

//**********************************************************
// class MSVD
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MSVD::MSVD( const MVec3* matrixMx3, unsigned int rowCount )
: _U( 0 )
, _rowCount( rowCount )
{
	// we must have 3 or more elements to solve.
	B_ASSERT( rowCount >= 3 );

	int m = rowCount;
	int n = 3;
	int nu = 3;

	_U = new MVec3[ m ];
	float e[ 3 ];

	// allocate working memory.
	float* work = new float[ m ];
	MVec3* A = new MVec3[ rowCount ];
	MemSet( work, 0, m * sizeof( float ) );
	for ( unsigned int i = 0; i < rowCount; ++i )
		A[ i ] = matrixMx3[ i ];

	int wantu = 1;  					/* boolean */
	int wantv = 1;  					/* boolean */
	int i=0, j=0, k=0;

	// Reduce A to bidiagonal form, storing the diagonal elements
	// in _s and the super-diagonal elements in e.
	int nct = min( m-1, n );
	int nrt = max( 0, min( n-2, m ) );
	for ( k = 0; k < max( nct, nrt ); ++k )
	{
		if ( k < nct )
		{
			// Compute the transformation for the k-th column and
			// place the k-th diagonal in _s[k].
			// Compute 2-norm of k-th column without under/overflow.
			_s[ k ] = 0.0f;
			for (i = k; i < m; i++) {
				_s[ k ] = hypot( _s[ k ], A[ i ][ k ] );
			}
			if ( _s[ k ] != 0.0f )
			{
				if ( A[ k ][ k ] < 0.0f )
				{
					_s[ k ] = -_s[ k ];
				}
				for ( i = k; i < m; ++i )
				{
					A[ i ][ k ] /= _s[ k ];
				}
				A[ k ][ k ] += 1.0f;
			}
			_s[ k ] = -_s[ k ];
		}
		for ( j = k+1; j < n; ++j )
		{
			if ( ( k < nct ) && ( _s[k] != 0.0f ) )
			{
				// Apply the transformation.
				float t = 0.0f;
				for ( i = k; i < m; ++i )
					t += A[ i ][ k ] * A[ i ][ j ];
				t = -t / A[ k ][ k ];
				for ( i = k; i < m; ++i )
					A[ i ][ j ] += t * A[ i ][ k ];
            }

			// Place the k-th row of A into e for the
			// subsequent calculation of the row transformation.
			e[ j ] = A[ k ][ j ];
		}
		if ( wantu & ( k < nct ) )
		{
            // Place the transformation in _U for subsequent back
            // multiplication.
            for ( i = k; i < m; ++i )
               _U[ i ][ k ] = A[ i ][ k ];
		}
		if ( k < nrt )
		{
			// Compute the k-th row transformation and place the
			// k-th super-diagonal in e[k].
			// Compute 2-norm without under/overflow.
			e[ k ] = 0;
			for ( i = k+1; i < n; ++i )
			{
				e[ k ] = hypot( e[ k ], e[ i ] );
			}
            if ( e[ k ] != 0.0f )
			{
				if ( e[ k+1 ] < 0.0f )
				{
					e[ k ] = -e[ k ];
				}
				for ( i = k+1; i < n; ++i )
				{
					e[ i ] /= e[ k ];
				}
				e[ k+1 ] += 1.0;
            }
            e[ k ] = -e[ k ];
            if ( ( k+1 < m ) & ( e[ k ] != 0.0f) )
			{
				// Apply the transformation.
				for ( i = k+1; i < m; ++i )
					work[ i ] = 0.0f;

				for ( j = k+1; j < n; ++j )
				{
					for ( i = k+1; i < m; ++i )
						work[ i ] += e[ j ] * A[ i ][ j ];
				}
				for ( j = k+1; j < n; ++j )
				{
					float t = -e[ j ] / e[ k + 1 ];
					for ( i = k+1; i < m; ++i )
						A[ i ][ j ] += t * work[ i ];
				}
            }
            if ( wantv )
			{
				// Place the transformation in _V for subsequent
				// back multiplication.
				for ( i = k+1; i < n; ++i )
					_V( i, k ) = e[ i ];
            }
		}
	}

	// Set up the final bidiagonal matrix or order p.
	int p = min( n, m+1 );
	if ( nct < n )
		_s[ nct ] = A[ nct ][ nct ];
	if ( m < p )
		_s[ p-1 ] = 0.0f;
	if ( nrt+1 < p )
		e[ nrt ] = A[ nrt ][ p - 1 ];
	e[ p - 1 ] = 0.0f;

	// If required, generate _U.
	if ( wantu )
	{
		for ( j = nct; j < nu; ++j )
		{
			for ( i = 0; i < m; ++i )
				_U[ i ][ j ] = 0.0f;
			_U[ j ][ j ] = 1.0f;
		}
		for ( k = nct-1; k >= 0; --k )
		{
			if ( _s[ k ] != 0.0f )
			{
				for ( j = k+1; j < nu; ++j )
				{
					float t = 0.0f;
					for ( i = k; i < m; ++i )
						t += _U[ i ][ k ] * _U[ i ][ j ];
					t = -t / _U[ k ][ k ];
					for ( i = k; i < m; ++i )
						_U[ i ][ j ] += t * _U[ i ][ k ];
				}
				for (i = k; i < m; ++i )
					_U[ i ][ k ] = -_U[ i ][ k ];

				_U[ k ][ k ] = 1.0f + _U[ k ][ k ];
				for ( i = 0; i < k-1; ++i )
					_U[ i ][ k ] = 0.0f;
			}
			else
			{
				for ( i = 0; i < m; ++i )
					_U[ i ][ k ] = 0.0f;
				_U[ k ][ k ] = 1.0f;
			}
		}
	}

	// If required, generate _V.
	if ( wantv )
	{
		for ( k = n-1; k >= 0; --k )
		{
			if ( ( k < nrt ) & ( e[ k ] != 0.0f ) )
			{
				for ( j = k+1; j < nu; ++j )
				{
					float t = 0.0f;
					for ( i = k+1; i < n; ++i )
						t += _V( i, k ) * _V( i, j );
					t = -t / _V( k+1, k );
					for ( i = k+1; i < n; ++i )
						_V( i, j ) += t * _V( i, k );
				}
			}
			for ( i = 0; i < n; ++i )
				_V( i, k ) = 0.0f;
			_V( k, k ) = 1.0f;
		}
	}

	// Main iteration loop for the singular values.
	int pp = p - 1;
	int iter = 0;
	float eps = Pow( 2.0f, -52.0f );
	while ( p > 0 )
	{
		int k=0;
		int kase=0;

		// Here is where a test for too many iterations would go.

		// This section of the program inspects for
		// negligible elements in the _s and e arrays.  On
		// completion the variables kase and k are set as follows.

		// kase = 1     if _s(p) and e[k-1] are negligible and k<p
		// kase = 2     if _s(k) is negligible and k<p
		// kase = 3     if e[k-1] is negligible, k<p, and
		//              _s(k), ..., _s(p) are not negligible (qr step).
		// kase = 4     if e(p-1) is negligible (convergence).

		for ( k = p-2; k >= -1; --k )
		{
			if ( k == -1 )
				break;

			if ( Abs( e[ k ] ) <= eps * ( Abs( _s[ k ] ) + Abs( _s[ k + 1 ] ) ) )
			{
				e[ k ] = 0.0f;
				break;
			}
		}
		if ( k == ( p - 2 ) )
		{
			kase = 4;
		}
		else
		{
			int ks;
			for ( ks = p - 1; ks >= k; --ks )
			{
				if ( ks == k )
					break;

				float t = ( ( ( ks != p	   ) ? Abs( e[ ks - 0 ] ) : 0.0f ) + 
							( ( ks != k + 1 ) ? Abs( e[ ks - 1 ] ) : 0.0f ) );
				if ( Abs( _s[ ks ] ) <= eps * t )
				{
					_s[ ks ] = 0.0f;
					break;
				}
			}
			if ( ks == k )
			{
				kase = 3;
			}
			else if ( ks == p-1 )
			{
				kase = 1;
			}
			else
			{
				kase = 2;
				k = ks;
			}
		}
		k++;

		// Perform the task indicated by kase.
		switch ( kase )
		{
			case 1:		// Deflate negligible _s(p).
				{
					float f = ( e [ p-2 ] );
					e[ p-2 ] = 0.0;
					for ( j = p-2; j >= k; --j )
					{
						float t = hypot( _s[ j ], f );
						float cs = _s[ j ] / t;
						float sn = f / t;
						_s[ j ] = t;
						if ( j != k )
						{
							f = -sn * e[ j - 1 ];
							e[ j - 1 ] = cs * e[ j - 1 ];
						}
						if ( wantv )
						{
							for ( i = 0; i < n; ++i )
							{
								t = cs * _V( i, j ) + sn * _V( i, p - 1 );
								_V( i, p - 1 ) = -sn * _V( i, j ) + cs * _V( i, p - 1 );
								_V( i, j ) = t;
							}
						}
					}
				}
				break;

			case 2:		// Split at negligible _s(k).
				{
					float f = e[ k - 1 ];
					e[ k - 1 ] = 0.0f;
					for ( j = k; j < p; ++j )
					{
						float t = hypot( _s[ j ], f );
						float cs = _s[ j ] / t;
						float sn = f / t;
						_s[ j ] = t;
						f = -sn * e[ j ];
						e[ j ] = cs * e[ j ];
						if ( wantu )
						{
							for ( i = 0; i < m; ++i )
							{
								t = cs * _U[ i ][ j ] + sn * _U[ i ][ k - 1 ];
								_U[ i ][ k - 1 ] = -sn * _U[ i ][ j ] + cs * _U[ i ][ k - 1 ];
								_U[ i ][ j ] = t;
							}
						}
					}
				}
				break;

			case 3:		// Perform one qr step.
				{
					// Calculate the shift.
//					float scale = Max( Max( Max( Max(
//						   Abs( _s[ p - 1 ] ), Abs( _s[ p - 2 ] ) ), Abs( e[ p - 2 ] ) ), 
//						   Abs( _s[ k ] ) ), Abs( e[ k ] ) );
					float scale = AbsMax5( _s[ p - 1 ], _s[ p - 2 ], e[ p - 2 ], _s[ k ], e[ k ] );
					float sp = _s[ p - 1 ] / scale;
					float spm1 = _s[ p - 2] / scale;
					float epm1 = e[ p - 2 ] / scale;
					float sk = _s[ k ] / scale;
					float ek = e[ k ] / scale;
					float b = ( ( spm1 + sp ) * ( spm1 - sp ) + epm1 * epm1 ) / 2.0f;
					float c = ( sp * epm1 ) * ( sp * epm1 );
					float shift = 0.0f;
					if ( ( b != 0.0f ) || ( c != 0.0f ) )
					{
						shift = Sqrt( b*b + c );
						if ( b < 0.0 )
							shift = -shift;
						shift = c/(b + shift);
					}
					float f = ( sk + sp ) * ( sk - sp ) + shift;
					float g = sk * ek;

					// Chase zeros.
					for ( j = k; j < p-1; ++j )
					{
						float t = hypot( f, g );
						float cs = f / t;
						float sn = g / t;
						if ( j != k )
						{
							e[ j - 1 ] = t;
						}
						f = cs * _s[ j ] + sn * e[ j ];
						e[ j ] = cs * e[ j ] - sn * _s[ j ];
						g = sn * _s[ j + 1 ];
						_s[ j + 1 ] = cs * _s[ j + 1 ];
						if ( wantv )
						{
							for ( i = 0; i < n; ++i )
							{
								t = cs*_V( i, j ) + sn * _V( i, j + 1 );
								_V( i, j + 1 ) = -sn * _V( i, j ) + cs * _V( i, j + 1 );
								_V( i, j ) = t;
							}
						}
						t = hypot( f, g );
						cs = f / t;
						sn = g / t;
						_s[ j ] = t;
						f = cs * e[ j ] + sn * _s[ j + 1 ];
						_s[ j + 1 ] = -sn * e[ j ] + cs * _s[ j + 1 ];
						g = sn * e[ j + 1 ];
						e[ j + 1] = cs * e[ j + 1 ];
						if ( wantu && ( j < m - 1 ) )
						{
							for (i = 0; i < m; ++i )
							{
								t = cs * _U[ i ][ j ] + sn * _U[ i ][ j + 1 ];
								_U[ i ][ j + 1 ] = -sn * _U[ i ][ j ] + cs * _U[ i ][ j + 1 ];
								_U[ i ][ j ] = t;
							}
						}
					}
					e[p-2] = f;
					iter = iter + 1;
				}
				break;

			case 4:		// Convergence.
				{

					// Make the singular values positive.
					if ( _s[ k ] <= 0.0f )
					{
						// this check is to avoid -0.0f (because of the != 0.0f checks).
						_s[ k ] = ( _s[ k ] < 0.0f ? -_s[ k ] : 0.0f );
						if ( wantv )
						{
							for ( i = 0; i <= pp; ++i )
								_V( i, k ) = -_V( i, k );
						}
					}

					// Order the singular values.
					while ( k < pp )
					{
						if ( _s[ k ] >= _s[ k + 1 ] )
							break;
						float t = _s[ k ];
						_s[ k ] = _s[ k + 1 ];
						_s[ k + 1 ] = t;
						if ( wantv && ( k < n - 1 ) )
						{
							for ( i = 0; i < n; ++i )
							{
								t = _V( i, k+1 );
								_V( i, k+1 ) = _V( i, k );
								_V( i, k ) = t;
							}
						}
						if ( wantu && ( k < m - 1 ) )
						{
							for ( i = 0; i < m; ++i )
							{
								t = _U[ i ][ k + 1 ];
								_U[ i ][ k + 1 ] = _U[ i ][ k ];
								_U[ i ][ k ] = t;
							}
						}
						++k;
					}
					iter = 0;
					--p;
				}
				break;
		}
	}

	// free our scratch memory.
	delete[] A;
	delete[] work;
}

//----------------------------------------------------------
MSVD::~MSVD()
{
	delete[] _U;
}
