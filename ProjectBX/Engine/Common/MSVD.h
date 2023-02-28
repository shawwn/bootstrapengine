//----------------------------------------------------------
// File:        MSVD.h
// Author:      Kevin Bray
// Created:     01-12-07
//
// Purpose:     To perform singular value decomposition.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MMat3x3.h"
#include "MVec3.h"

//**********************************************************
// class MSVD
//**********************************************************
class MSVD 
{
public:
    MSVD( const MVec3* matrixMx3, unsigned int rowCount );
    ~MSVD();

    unsigned int    GetRowCount() const         {   return _rowCount;   }

    MVec3*          GetU() const                {   return _U;          }
    const MMat3x3&  GetV() const                {   return _V;          }

    // gets the singular values.
    MVec3           GetSingularValues() const   {   return _s;          }

private:
    MVec3*          _U;
    MMat3x3         _V;
    MVec3           _s;
    int             _rowCount;
};

#if 0

   /** Return the diagonal matrix of singular values
   @return     S
   */

   void getS (Array2D<Real> &A) {
      A = Array2D<Real>(n,n);
      for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
            A[i][j] = 0.0;
         }
         A[i][i] = s[i];
      }
   }

   /** Two norm  (max(S)) */

   Real norm2 () {
      return s[0];
   }

   /** Two norm of condition number (max(S)/min(S)) */

   Real cond () {
      return s[0]/s[min(m,n)-1];
   }

   /** Effective numerical matrix rank
   @return     Number of nonnegligible singular values.
   */

   int rank () 
   {
      Real eps = pow(2.0,-52.0);
      Real tol = max(m,n)*s[0]*eps;
      int r = 0;
      for (int i = 0; i < s.dim(); i++) {
         if (s[i] > tol) {
            r++;
         }
      }
      return r;
   }
};
#endif