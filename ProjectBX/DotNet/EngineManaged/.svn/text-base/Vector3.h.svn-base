//----------------------------------------------------------
// File:		Vector3.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To perform vector operations in 3D space.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec3;
struct SVec3;

namespace Bootstrap 
{
    //**********************************************************
    // value class Vector3
    //**********************************************************
	public value class Vector3
	{
	internal:
		Vector3( const MVec3& vec );
		Vector3( const SVec3& vec );

		MVec3                       ToMVec3();
		SVec3                       ToSVec3();

	public:
		Vector3( float x, float y, float z );

		property float              X;
		property float              Y;
		property float              Z;

        property Vector3            Abs
        {
            Vector3                     get();
        }

		property Vector3			Normalized
        {
            Vector3                     get();
        }

		property float	            MagSqr		
        { 
            float                       get()               {   return Dot( *this );    } 
        }

		property float	            Mag			
        { 
            float                       get()               {   return Sqrt( MagSqr );  }
        }

        Vector3                     Max( Vector3 v );
        Vector3                     Min( Vector3 v );

        float                       Max();
        float                       Min();
        float                       AbsMax();
        float                       AbsMin();

        float			            Dist( Vector3 point );
        float			            DistSqr( Vector3 point );
		float			            Dot( Vector3 v );
		Vector3			            Cross( Vector3 v );
		Vector3			            Lerp( Vector3 v, float t );

        // math operators.
		static Vector3              operator + ( Vector3 left, Vector3 right );
		static Vector3              operator - ( Vector3 left, Vector3 right );
		static Vector3              operator - ( Vector3 value );
		static Vector3              operator * ( Vector3 vector, float scale );
		static Vector3              operator * ( float scale, Vector3 vector );
		static Vector3              operator / ( Vector3 vector, float scale );

        // pretty-prints the vector.
		virtual System::String^     ToString() override;

		// equality.
		virtual bool				Equals( System::Object^ obj ) override;
		virtual int					GetHashCode() override;

	internal:
		static                      operator MVec3( Vector3 v );
		static                      operator Vector3( const MVec3& v );
	};
}
