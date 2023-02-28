//----------------------------------------------------------
// File:        Matrix.h
// Author:      Shawn Presser
// Created:     09-22-08
//
// Purpose:     To perform matrix operations.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

/*
matrices are row-major.

4x4 matrix:
    (0,0) (0,1) (0,2) (0,3)
    (1,0) (1,1) (1,2) (1,3)
    (2,0) (2,1) (2,2) (2,3)
    (3,0) (3,1) (3,2) (3,3)

matrix-to-array:
    array[ 0]=rot(0,0)  array[ 1]=rot(0,1)  array[ 2]=rot(0,2)  array[ 3]=trans.x
    array[ 4]=rot(1,0)  array[ 5]=rot(1,1)  array[ 6]=rot(1,2)  array[ 7]=trans.y
    array[ 8]=rot(2,0)  array[ 9]=rot(2,1)  array[10]=rot(2,2)  array[11]=trans.z
    array[12]=0.0       array[13]=0.0       array[14]=0.0       array[15]=1.0

matrix mul:
    D = zero
    for i 
        for j
            for k
                D[ i, j ] += A[ i, k ] * B[ k, j ]

*/

// forward declarations.
class MMat4x4;
class MMat3x3;

// using namespaces.
using System::Runtime::InteropServices::OutAttribute;

namespace Bootstrap
{
    // forward declarations.
    value class Vector3;
    value class Matrix3x3;

    //**********************************************************
    // value class Matrix
    //**********************************************************
    public value class Matrix
    {
    internal:
        // constructs a matrix from an MMat4x4.
        Matrix( const MMat4x4& mat );

        // converts the matrix to an MMat4x4.
        MMat4x4 ToMMat4x4();

    public:
        // constructs a matrix that represents a uniform scale factor.
        Matrix( float scale );

        // constructs a amtrix that represents a translation.
        Matrix( Vector3 translation );

        // constructs a matrix that represents a rotation.
        Matrix( Matrix3x3 rotation );

        // constructs a matrix that represents an orientation.
        Matrix( Matrix3x3 rotation, Vector3 translation );

        // constructs a matrix that represents an orientation and a scale factor.
        Matrix( Matrix3x3 rotation, Vector3 translation, Vector3 scale );
        Matrix( float M11, float M12, float M13, float M14,
                float M21, float M22, float M23, float M24,
                float M31, float M32, float M33, float M34,
                float M41, float M42, float M43, float M44 );

        // static constructor.
        static Matrix();

        // the components of the row-major 4x4 matrix.
        property float              M11; 
        property float              M12; 
        property float              M13; 
        property float              M14; 
        
        property float              M21; 
        property float              M22; 
        property float              M23; 
        property float              M24; 
        
        property float              M31; 
        property float              M32; 
        property float              M33; 
        property float              M34; 
        
        property float              M41; 
        property float              M42; 
        property float              M43; 
        property float              M44; 

        // a 4x4 identity matrix.
        static initonly Matrix      Identity;

        // computes the transpose of the matrix.
        property Matrix             Transpose 
        { 
            Matrix                      get(); 
        }

        // gets or sets the XAxis basis vector of the matrix. 
        // ( M11, M12, M13 )
        property Vector3            XAxis 
        { 
            Vector3                     get();  
            void                        set( Vector3 v ); 
        }

        // gets or sets the YAxis basis vector of the matrix. 
        // ( M21, M22, M23 )
        property Vector3            YAxis 
        { 
            Vector3                     get();  
            void                        set( Vector3 v ); 
        }

        // gets or sets the ZAxis basis vector of the matrix. 
        // ( M31, M32, M33 )
        property Vector3            ZAxis 
        { 
            Vector3                     get();  
            void                        set( Vector3 v ); 
        }

        // gets or sets the upper-left 3x3 component of the matrix.
        property Matrix3x3          Rotate 
        { 
            Matrix3x3                   get();  
            void                        set( Matrix3x3 mat ); 
        }

        // gets or sets the translation component of the matrix. 
        // ( M14, M24, M34 )
        property Vector3            Translate 
        { 
            Vector3                     get();  
            void                        set( Vector3 v ); 
        }

        // computes the squared magnitude of each axis.  ( XAxis.MagSqr, YAxis.MagSqr, ZAxis.MagSqr )
        property Vector3            ScaleSqr 
        { 
            Vector3                     get(); 
        }

        // computes or sets the magnitude of each axis.  
        // ( XAxis.Mag, YAxis.Mag, ZAxis.Mag)
        property Vector3            Scale 
        { 
            Vector3                     get();  
            void                        set( Vector3 v ); 
        }

        // transforms a vector by the matrix, then projects it back into 3D-space.
        Vector3                     TransformCoord( Vector3 coord );

        // transforms a vector by the matrix, ignoring the matrix's projection term.
        Vector3                     TransformCoordNoPersp( Vector3 coord );

        // computes the inverse of the matrix.
        bool                        Inverse( [Out] Matrix% output );

        // computes a new set of basis vectors based on the 'side' and 'forward' vectors.
        // (Note: the 'forward' direction is preserved.)
        void                        SetOrientation( Vector3 side, Vector3 forward );

        // pretty-prints the matrix.
        virtual System::String^     ToString() override;

        // multiplies two 4x4 matrices.
        static Matrix               operator* ( Matrix left, Matrix right );
    };

    //**********************************************************
    // value class Matrix3x3
    //**********************************************************
    public value class Matrix3x3
    {
    internal:
        // constructs a matrix from an MMat3x3.
        Matrix3x3( const MMat3x3& mat );

        // converts the matrix to an MMat3x3.
        MMat3x3                     ToMMat3x3();

    public:
        // constructs a 3x3 matrix from Euler rotation angles.
        Matrix3x3( float y, float x, float z );

        // constructs a 3x3 matrix from a quaternion rotation.
        Matrix3x3( float x, float y, float z, float w );

        // constructs a 3x3 matrix directly.
        Matrix3x3( float M11, float M12, float M13, 
                   float M21, float M22, float M23, 
                   float M31, float M32, float M33 );

        // constructs a 3x3 matrix that represents a rotation around an arbitrary axis.
        Matrix3x3( Vector3 axis, float angle );

        // the static constructor.
        static Matrix3x3();

        // constructs a matrix that represents a rotation defined by Euler rotation angles.
        static Matrix3x3            FromEulerYXZ( float y, float x, float z );

        // constructs a matrix that represents a rotation around the X-axis.
        static Matrix3x3            MakeXRotation( float angleInRad );

        // constructs a matrix that represents a rotation around the Y-axis.
        static Matrix3x3            MakeYRotation( float angleInRad );

        // constructs a matrix that represents a rotation around the Z-axis.
        static Matrix3x3            MakeZRotation( float angleInRad );

        // constructs a matrix that represents a rotation around an arbitrary axis.
        static Matrix3x3            MakeRotation( Vector3 axis, float angle );

        // the components of the row-major 3x3 matrix.
        property float M11; 
        property float M12; 
        property float M13; 

        property float M21; 
        property float M22; 
        property float M23; 

        property float M31;
        property float M32;
        property float M33;

        // a 3x3 identity matrix.
        static initonly Matrix3x3   Identity;

        // computes the transpose of the matrix.
        property Matrix3x3          Transpose 
        { 
            Matrix3x3                   get(); 
        }

        // gets or sets the XAxis basis vector of the matrix. 
        // ( M11, M12, M13 )
        property Vector3            XAxis 
        { 
            Vector3                     get();
            void                        set( Vector3 v );
        }

        // gets or sets the YAxis basis vector of the matrix. 
        // ( M21, M22, M23 )
        property Vector3            YAxis 
        { 
            Vector3                     get();
            void                        set( Vector3 v );
        }

        // gets or sets the ZAxis basis vector of the matrix. 
        // ( M31, M32, M33 )
        property Vector3            ZAxis 
        { 
            Vector3                     get();
            void                        set( Vector3 v );
        }

        // computes or sets the Euler rotation angles of the matrix.
        property Vector3            EulerAngles 
        { 
            Vector3                     get();  
            void                        set( Vector3 radians ); 
        }

        // computes the inverse of the matrix.
        bool                        Inverse( [Out] Matrix3x3% output );

        // transforms a point by the matrix.
        Vector3                     RotatePoint( Vector3 pt );

        // computes the Euler rotation angles of the matrix.
        void                        ToEulerYXZ( [Out] float% y, [Out] float% x, [Out] float% z );

        // pretty-prints the matrix.
        virtual System::String^     ToString() override;

        // multiplies two 3x3 matrices.
        static Matrix3x3            operator* ( Matrix3x3 left, Matrix3x3 right );
    };
}