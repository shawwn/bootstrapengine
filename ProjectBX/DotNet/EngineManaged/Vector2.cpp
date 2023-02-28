//----------------------------------------------------------
// File:        Vector2.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Vector2.h"

// engine headers.
#include "GrTypes.h"
#include "MVec2.h"

//**********************************************************
// value class Vector2
//**********************************************************

//----------------------------------------------------------
Bootstrap::Vector2::Vector2( const MVec2& vec )
{
    X = vec.GetX();
    Y = vec.GetY();
}

//----------------------------------------------------------
Bootstrap::Vector2::Vector2( const SVec2& vec )
{
    X = vec.x;
    Y = vec.y;
}

//----------------------------------------------------------
MVec2   
Bootstrap::Vector2::ToMVec2() 
{ 
    return MVec2( X, Y ); 
}

//----------------------------------------------------------
SVec2 
Bootstrap::Vector2::ToSVec2()                                   
{ 
    return SVec2( X, Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2::Vector2( float x, float y )
{
    X = x;
    Y = y;
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::Normalized::get()
{
    float mag = Sqrt( X*X + Y*Y );
    if ( mag < 0.0001f )
        return Vector2( 0.0f, 1.0f );

    float invMag = 1.0f / mag;
    return Vector2( 
        invMag * X, 
        invMag * Y );
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator + ( Vector2 left, Vector2 right ) 
{ 
    return Vector2( 
        left.X + right.X, 
        left.Y + right.Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator - ( Vector2 left, Vector2 right ) 
{ 
    return Vector2( 
        left.X - right.X, 
        left.Y - right.Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator - ( Vector2 value ) 
{ 
    return Vector2( 
        -value.X, 
        -value.Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator * ( Vector2 vector, float scale ) 
{ 
    return Vector2( 
        scale * vector.X, 
        scale * vector.Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator * ( float scale, Vector2 vector ) 
{ 
    return Vector2( 
        scale * vector.X, 
        scale * vector.Y ); 
}

//----------------------------------------------------------
Bootstrap::Vector2 
Bootstrap::Vector2::operator / ( Vector2 vector, float scale ) 
{ 
    float invScale = 1.0f / scale;
    return Vector2( 
        invScale * vector.X, 
        invScale * vector.Y ); 
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Vector2::ToString() 
{
    return "( " + X + ", " + Y + " )";
}

//----------------------------------------------------------
bool 
Bootstrap::Vector2::Equals( System::Object^ obj )
{
    if ( obj != nullptr && obj->GetType() == Vector2::GetType() )
    {
        Vector2 val = (Vector2)obj;
        return X == val.X && Y == val.Y;
    }
    return false;
}

//----------------------------------------------------------
int 
Bootstrap::Vector2::GetHashCode()
{
    return X.GetHashCode() + Y.GetHashCode();
}

//----------------------------------------------------------
Bootstrap::Vector2::operator MVec2( Vector2 v ) 
{ 
    return v.ToMVec2(); 
}

//----------------------------------------------------------
Bootstrap::Vector2::operator Vector2( const MVec2& v ) 
{ 
    return Vector2( v ); 
}