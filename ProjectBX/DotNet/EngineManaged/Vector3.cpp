//----------------------------------------------------------
// File:		Vector3.cpp
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Vector3.h"

// project includes.
#include "Utils.h"

// engine headers.
#include "MVec3.h"
#include "GrTypes.h"

//**********************************************************
// value class Vector3
//**********************************************************

//----------------------------------------------------------
Bootstrap::Vector3::Vector3( const MVec3& vec )
{
	X = vec.GetX();
	Y = vec.GetY();
	Z = vec.GetZ();
}

//----------------------------------------------------------
Bootstrap::Vector3::Vector3( const SVec3& vec )
{
	X = vec.x;
	Y = vec.y;
	Z = vec.z;
}

//----------------------------------------------------------
MVec3   
Bootstrap::Vector3::ToMVec3() 
{ 
    return MVec3( X, Y, Z ); 
}

//----------------------------------------------------------
SVec3	
Bootstrap::Vector3::ToSVec3() 
{ 
    return SVec3( X, Y, Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3::Vector3( float x, float y, float z )
{
	X = x;
	Y = y;
	Z = z;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Abs::get()
{
    return Vector3( 
        ::Abs( X ),
        ::Abs( Y ),
        ::Abs( Z ) );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Normalized::get()
{
	float mag = Sqrt( X*X + Y*Y + Z*Z );
	if ( mag < 0.0001f )
		return Vector3( 0.0f, 1.0f, 0.0f );

    float invMag = 1.0f / mag;
	return Vector3( 
        invMag * X,
        invMag * Y,
        invMag * Z );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Max( Vector3 v )
{
    return Vector3( 
        ::Max( X, v.X ),
        ::Max( Y, v.Y ),
        ::Max( Z, v.Z ) );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Min( Vector3 v )
{
    return Vector3( 
        ::Min( X, v.X ),
        ::Min( Y, v.Y ),
        ::Min( Z, v.Z ) );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::Max()
{
    return ::Max3( X, Y, Z );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::Min()
{
    return ::Min3( X, Y, Z );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::AbsMax()
{
    return ::Max3( ::Abs( X ), ::Abs( Y ), ::Abs( Z ) );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::AbsMin()
{
    return ::Min3( ::Abs( X ), ::Abs( Y ), ::Abs( Z ) );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::Dist( Vector3 point ) 
{
    return ::Sqrt( DistSqr( point ) );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::DistSqr( Vector3 point ) 
{
    float x = X - point.X;
    float y = Y - point.Y;
    float z = Z - point.Z;
    return( x*x+y*y+z*z );
}

//----------------------------------------------------------
float 
Bootstrap::Vector3::Dot( Vector3 v )
{
	return X*v.X + Y*v.Y + Z*v.Z;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Cross( Vector3 v ) 
{
	return Vector3( 
        Y*v.Z - Z*v.Y, 
		Z*v.X - X*v.Z, 
		X*v.Y - Y*v.X );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::Lerp( Vector3 v, float t )
{
	return Vector3(
		::Lerp( X, v.X, t ),
		::Lerp( Y, v.Y, t ),
		::Lerp( Z, v.Z, t ) );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator + ( Vector3 left, Vector3 right ) 
{ 
    return Vector3( 
        left.X + right.X, 
        left.Y + right.Y, 
        left.Z + right.Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator - ( Vector3 left, Vector3 right ) 
{ 
    return Vector3( 
        left.X - right.X, 
        left.Y - right.Y, 
        left.Z - right.Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator - ( Vector3 value ) 
{ 
    return Vector3( 
        -value.X, 
        -value.Y, 
        -value.Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator * ( Vector3 vector, float scale ) 
{ 
    return Vector3( 
        scale * vector.X, 
        scale * vector.Y, 
        scale * vector.Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator * ( float scale, Vector3 vector ) 
{ 
    return Vector3( 
        scale * vector.X, 
        scale * vector.Y, 
        scale * vector.Z ); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Vector3::operator / ( Vector3 vector, float scale ) 
{ 
    float invScale = 1.0f / scale;
    return Vector3( 
        invScale * vector.X, 
        invScale * vector.Y, 
        invScale * vector.Z ); 
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Vector3::ToString() 
{
	return "( " + X + ", " + Y + ", " + Z + " )";
}

//----------------------------------------------------------
bool 
Bootstrap::Vector3::Equals( System::Object^ obj )
{
	if ( obj != nullptr && obj->GetType() == Vector3::GetType() )
	{
		Vector3 val = (Vector3)obj;
		return X == val.X && Y == val.Y && Z == val.Z;
	}
	return false;
}

//----------------------------------------------------------
int 
Bootstrap::Vector3::GetHashCode()
{
	return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
}

//----------------------------------------------------------
Bootstrap::Vector3::operator MVec3( Vector3 v ) 
{ 
    return v.ToMVec3(); 
}

//----------------------------------------------------------
Bootstrap::Vector3::operator Vector3( const MVec3& v ) 
{ 
    return Vector3( v ); 
}
