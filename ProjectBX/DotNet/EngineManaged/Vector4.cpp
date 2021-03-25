//----------------------------------------------------------
// File:		Vector4.h
// Author:		Shawn Presser
// Created:		09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Vector4.h"

// project includes.
#include "Utils.h"

// engine headers.
#include "MVec4.h"
#include "GrTypes.h"

//**********************************************************
// value class Vector4
//**********************************************************

//----------------------------------------------------------
Bootstrap::Vector4::Vector4( const MVec4& vec )
{
	X = vec.GetX();
	Y = vec.GetY();
	Z = vec.GetZ();
	W = vec.GetW();
}

//----------------------------------------------------------
Bootstrap::Vector4::Vector4( const SVec4& vec )
{
	X = vec.x;
	Y = vec.y;
	Z = vec.z;
	W = vec.w;
}

//----------------------------------------------------------
MVec4   
Bootstrap::Vector4::ToMVec4() 
{ 
    return MVec4( X, Y, Z, W ); 
}

//----------------------------------------------------------
SVec4   
Bootstrap::Vector4::ToSVec4() 
{ 
    return SVec4( X, Y, Z, W ); 
}

//----------------------------------------------------------
Bootstrap::Vector4::Vector4( float x, float y, float z, float w )
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

//----------------------------------------------------------
float 
Bootstrap::Vector4::Dot( Vector4 v )
{
	return X*v.X + Y*v.Y + Z*v.Z * W*v.W;
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator + ( Vector4 left, Vector4 right ) 
{ 
    return Vector4( 
        left.X + right.X, 
        left.Y + right.Y, 
        left.Z + right.Z, 
        left.W + right.W );
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator - ( Vector4 left, Vector4 right ) 
{ 
    return Vector4( 
        left.X - right.X, 
        left.Y - right.Y, 
        left.Z - right.Z, 
        left.W - right.W );
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator - ( Vector4 value ) 
{ 
    return Vector4( 
        -value.X, 
        -value.Y, 
        -value.Z, 
        -value.W ); 
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator * ( Vector4 vector, float scale ) 
{ 
    return Vector4( 
        scale * vector.X, 
        scale * vector.Y, 
        scale * vector.Z, 
        scale * vector.W ); 
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator * ( float scale, Vector4 vector ) 
{ 
    return Vector4( 
        scale * vector.X, 
        scale * vector.Y, 
        scale * vector.Z, 
        scale * vector.W ); 
}

//----------------------------------------------------------
Bootstrap::Vector4 
Bootstrap::Vector4::operator / ( Vector4 vector, float scale ) 
{ 
    float invScale = 1.0f / scale;
    return Vector4( 
        invScale * vector.X, 
        invScale * vector.Y, 
        invScale * vector.Z, 
        invScale * vector.W ); 
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Vector4::ToString() 
{
	return "( " + X + ", " + Y + ", " + Z + ", " + W + " )";
}

//----------------------------------------------------------
bool 
Bootstrap::Vector4::Equals( System::Object^ obj )
{
	if ( obj != nullptr && obj->GetType() == Vector4::GetType() )
	{
		Vector4 val = (Vector4)obj;
		return X == val.X && Y == val.Y && Z == val.Z && W == val.W;
	}
	return false;
}

//----------------------------------------------------------
int 
Bootstrap::Vector4::GetHashCode()
{
	return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
}

//----------------------------------------------------------
Bootstrap::Vector4::operator MVec4( Vector4 v ) 
{ 
    return v.ToMVec4(); 
}

//----------------------------------------------------------
Bootstrap::Vector4::operator Vector4( const MVec4& v ) 
{ 
    return Vector4( v ); 
}