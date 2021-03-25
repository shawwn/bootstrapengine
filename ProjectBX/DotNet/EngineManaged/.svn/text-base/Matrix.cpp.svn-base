//----------------------------------------------------------
// File:		Matrix.cpp
// Author:		Shawn Presser
// Created:		09-22-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// project headers.
#include "Vector3.h"

// class header.
#include "Matrix.h"

// engine headers.
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "MQuat.h"

//**********************************************************
// value class Matrix
//**********************************************************

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( const MMat4x4& mat )
{
	const float* data = mat.GetDataStart();

	M11 = *data++; 
	M12 = *data++; 
	M13 = *data++; 
	M14 = *data++; 

	M21 = *data++; 
	M22 = *data++; 
	M23 = *data++; 
	M24 = *data++; 

	M31 = *data++; 
	M32 = *data++; 
	M33 = *data++; 
	M34 = *data++; 

	M41 = *data++; 
	M42 = *data++; 
	M43 = *data++; 
	M44 = *data++;
}

//----------------------------------------------------------
MMat4x4 
Bootstrap::Matrix::ToMMat4x4()
{
	return MMat4x4(
		M11, M12, M13, M14,
		M21, M22, M23, M24,
		M31, M32, M33, M34,
		M41, M42, M43, M44 );
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( float scale )
{
	M11 = scale;
	M12 = 0.0f;
	M13 = 0.0f;
	M14 = 0.0f;

	M21 = 0.0f;
	M22 = scale;
	M23 = 0.0f;
	M24 = 0.0f;

	M31 = 0.0f;
	M32 = 0.0f;
	M33 = scale;
	M34 = 0.0f;

	M41 = 0.0f;
	M42 = 0.0f;
	M43 = 0.0f;
	M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( Vector3 translation )
{
	M11 = 1.0f;
	M12 = 0.0f;
	M13 = 0.0f;
	M14 = translation.X;

	M21 = 0.0f;
	M22 = 1.0f;
	M23 = 0.0f;
	M24 = translation.Y;

	M31 = 0.0f;
	M32 = 0.0f;
	M33 = 1.0f;
	M34 = translation.Z;

	M41 = 0.0f;
	M42 = 0.0f;
	M43 = 0.0f;
	M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( Matrix3x3 rotation )
{
	M11 = rotation.M11;
	M12 = rotation.M12;
	M13 = rotation.M13;
	M14 = 0.0f;

	M21 = rotation.M21;
	M22 = rotation.M22;
	M23 = rotation.M23;
	M24 = 0.0f;

	M31 = rotation.M31;
	M32 = rotation.M32;
	M33 = rotation.M33;
	M34 = 0.0f;

	M41 = 0.0f;
	M42 = 0.0f;
	M43 = 0.0f;
	M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( Matrix3x3 rotation, Vector3 translation )
{
	M11 = rotation.M11;
	M12 = rotation.M12;
	M13 = rotation.M13;
	M14 = translation.X;

	M21 = rotation.M21;
	M22 = rotation.M22;
	M23 = rotation.M23;
	M24 = translation.Y;

	M31 = rotation.M31;
	M32 = rotation.M32;
	M33 = rotation.M33;
	M34 = translation.Z;

	M41 = 0.0f;
	M42 = 0.0f;
	M43 = 0.0f;
	M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( Matrix3x3 rotation, Vector3 translation, Vector3 scale )
{
	M11 = scale.X * rotation.M11;
	M12 = scale.X * rotation.M12;
	M13 = scale.X * rotation.M13;
	M14 = translation.X;

	M21 = scale.Y * rotation.M21;
	M22 = scale.Y * rotation.M22;
	M23 = scale.Y * rotation.M23;
	M24 = translation.Y;

	M31 = scale.Z * rotation.M31;
	M32 = scale.Z * rotation.M32;
	M33 = scale.Z * rotation.M33;
	M34 = translation.Z;

	M41 = 0.0f;
	M42 = 0.0f;
	M43 = 0.0f;
	M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix::Matrix( float M11, float M12, float M13, float M14,
				           float M21, float M22, float M23, float M24,
				           float M31, float M32, float M33, float M34,
				           float M41, float M42, float M43, float M44 )
{
	this->M11 = M11;
	this->M12 = M12;
	this->M13 = M13;
	this->M14 = M14;

	this->M21 = M21;
	this->M22 = M22;
	this->M23 = M23;
	this->M24 = M24;

	this->M31 = M31;
	this->M32 = M32;
	this->M33 = M33;
	this->M34 = M34;

	this->M41 = M41;
	this->M42 = M42;
	this->M43 = M43;
	this->M44 = M44;
}

//----------------------------------------------------------
// static constructor.
static 
Bootstrap::Matrix::Matrix()
{
    Identity = Matrix();
    Identity.M11 = 1.0f;
    Identity.M22 = 1.0f;
    Identity.M33 = 1.0f;
    Identity.M44 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Matrix::Transpose::get()
{
	return Matrix( M11, M21, M31, M41,
				   M12, M22, M32, M42,
				   M13, M23, M33, M43,
				   M14, M24, M34, M44 );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::XAxis::get()
{ 
    return Vector3( M11, M12, M13 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::XAxis::set( Vector3 v )
{ 
    M11 = v.X; 
    M12 = v.Y; 
    M13 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::YAxis::get()	
{ 
    return Vector3( M21, M22, M23 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::YAxis::set( Vector3 v )
{ 
    M21 = v.X; 
    M22 = v.Y; 
    M23 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::ZAxis::get()	
{ 
    return Vector3( M31, M32, M33 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::ZAxis::set( Vector3 v )
{ 
    M31 = v.X; 
    M32 = v.Y; 
    M33 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix::Rotate::get()				
{ 
	float xInvScale = 1.0f / XAxis.Mag;
	float yInvScale = 1.0f / YAxis.Mag;
	float zInvScale = 1.0f / ZAxis.Mag;
	return Matrix3x3( xInvScale*M11, xInvScale*M12, xInvScale*M13,
				      yInvScale*M21, yInvScale*M22, yInvScale*M23,
				      zInvScale*M31, zInvScale*M32, zInvScale*M33 );
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::Rotate::set( Matrix3x3 mat )
{
	Vector3 scale = this->Scale;
	M11 = scale.X * mat.M11;
	M12 = scale.X * mat.M12;
	M13 = scale.X * mat.M13;
	M21 = scale.Y * mat.M21;
	M22 = scale.Y * mat.M22;
	M23 = scale.Y * mat.M23;
	M31 = scale.Z * mat.M31;
	M32 = scale.Z * mat.M32;
	M33 = scale.Z * mat.M33;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::Translate::get()			
{ 
	return Vector3( M14, M24, M34 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::Translate::set( Vector3 v )	
{ 
	M14 = v.X, M24 = v.Y; M34 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::ScaleSqr::get()
{
	return Vector3( XAxis.MagSqr, YAxis.MagSqr, ZAxis.MagSqr );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::Scale::get()
{
	return Vector3( XAxis.Mag, YAxis.Mag, ZAxis.Mag );
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::Scale::set( Vector3 v )
{
	// if the scale goes below zero, then the axis information
	// will be lost!
	if ( v.X < 0.005 || v.Y < 0.005 || v.Z < 0.005 )
		return;

	XAxis = v.X * XAxis.Normalized;
	YAxis = v.Y * YAxis.Normalized;
	ZAxis = v.Z * ZAxis.Normalized;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix::TransformCoord( Vector3 coord )
{
	float x = coord.X*M11 + coord.Y*M12 + coord.Z*M13 + M14;
	float y = coord.X*M21 + coord.Y*M22 + coord.Z*M23 + M24;
	float z = coord.X*M31 + coord.Y*M32 + coord.Z*M33 + M34;
	float w = coord.X*M41 + coord.Y*M42 + coord.Z*M43 + M44;
	float invW = 1.0f / w;
	return Vector3( x * invW, y * invW, z * invW );
}

//----------------------------------------------------------
Bootstrap::Vector3
Bootstrap::Matrix::TransformCoordNoPersp( Vector3 coord )
{
	float x = coord.X*M11 + coord.Y*M12 + coord.Z*M13 + M14;
	float y = coord.X*M21 + coord.Y*M22 + coord.Z*M23 + M24;
	float z = coord.X*M31 + coord.Y*M32 + coord.Z*M33 + M34;
	return Vector3( x, y, z );
}

//----------------------------------------------------------
bool 
Bootstrap::Matrix::Inverse( [Out] Matrix% output )
{
	MMat4x4 inv;
	bool success = this->ToMMat4x4().Inverse( inv );
	output = Matrix( inv );
	return success;
}

//----------------------------------------------------------
void 
Bootstrap::Matrix::SetOrientation( Vector3 side, Vector3 forward )
{
	// compute the new matrix axes.
	Vector3 x( side.Normalized );
	Vector3 z( forward.Normalized );
	Vector3 y( z.Cross( x ).Normalized );
    x = y.Cross( z ).Normalized;

	// scale them.
	Vector3 scale = this->Scale;
	x *= scale.X;
	y *= scale.Y;
	z *= scale.Z;

	// set them.
	XAxis = x;
	YAxis = y;
	ZAxis = z;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Matrix::ToString() 
{
	return "\r\nX(" + M11 + ", " + M12 + ", " + M13 + ", " + M14 + ")" 
		   "\r\nY(" + M21 + ", " + M22 + ", " + M23 + ", " + M24 + ")" 
		   "\r\nZ(" + M31 + ", " + M32 + ", " + M33 + ", " + M34 + ")" 
		   "\r\nW(" + M41 + ", " + M42 + ", " + M43 + ", " + M44 + ")";
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::Matrix::operator*( Matrix left, Matrix right ) 
{ 
	Matrix result;

	result.M11 = (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31) + (left.M14 * right.M41);
	result.M12 = (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32) + (left.M14 * right.M42);
	result.M13 = (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33) + (left.M14 * right.M43);
	result.M14 = (left.M11 * right.M14) + (left.M12 * right.M24) + (left.M13 * right.M34) + (left.M14 * right.M44);

	result.M21 = (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31) + (left.M24 * right.M41);
	result.M22 = (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32) + (left.M24 * right.M42);
	result.M23 = (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33) + (left.M24 * right.M43);
	result.M24 = (left.M21 * right.M14) + (left.M22 * right.M24) + (left.M23 * right.M34) + (left.M24 * right.M44);

	result.M31 = (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31) + (left.M34 * right.M41);
	result.M32 = (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32) + (left.M34 * right.M42);
	result.M33 = (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33) + (left.M34 * right.M43);
	result.M34 = (left.M31 * right.M14) + (left.M32 * right.M24) + (left.M33 * right.M34) + (left.M34 * right.M44);

	result.M41 = (left.M41 * right.M11) + (left.M42 * right.M21) + (left.M43 * right.M31) + (left.M44 * right.M41);
	result.M42 = (left.M41 * right.M12) + (left.M42 * right.M22) + (left.M43 * right.M32) + (left.M44 * right.M42);
	result.M43 = (left.M41 * right.M13) + (left.M42 * right.M23) + (left.M43 * right.M33) + (left.M44 * right.M43);
	result.M44 = (left.M41 * right.M14) + (left.M42 * right.M24) + (left.M43 * right.M34) + (left.M44 * right.M44);

	return result;
}

//**********************************************************
// value class Matrix3x3
//**********************************************************

//----------------------------------------------------------
Bootstrap::Matrix3x3::Matrix3x3( const MMat3x3& mat )
{
	const float* data = mat.GetDataStart();

	M11 = *data++; 
	M12 = *data++; 
	M13 = *data++; 

	M21 = *data++; 
	M22 = *data++; 
	M23 = *data++;

	M31 = *data++; 
	M32 = *data++; 
	M33 = *data++;
}

//----------------------------------------------------------
MMat3x3 
Bootstrap::Matrix3x3::ToMMat3x3()
{
	return MMat3x3(
		M11, M12, M13,
		M21, M22, M23,
		M31, M32, M33 );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3::Matrix3x3( float y, float x, float z )
{
	EulerAngles = Vector3( x, y, z );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3::Matrix3x3( float x, float y, float z, float w )
{
    *this = Matrix3x3( MQuat( x, y, z, w ).MakeRot() );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3::Matrix3x3( float M11, float M12, float M13,
            					 float M21, float M22, float M23,
            					 float M31, float M32, float M33 )
{
	this->M11 = M11;
	this->M12 = M12;
	this->M13 = M13;

	this->M21 = M21;
	this->M22 = M22;
	this->M23 = M23;

	this->M31 = M31;
	this->M32 = M32;
	this->M33 = M33;
}

//----------------------------------------------------------
Bootstrap::Matrix3x3::Matrix3x3( Vector3 axis, float angle )
{
	*this = MakeRotation( axis, angle );
}

//----------------------------------------------------------
static Bootstrap::Matrix3x3::Matrix3x3()
{
    Identity = Matrix3x3();
    Identity.M11 = 1.0f;
    Identity.M22 = 1.0f;
    Identity.M33 = 1.0f;
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::FromEulerYXZ( float y, float x, float z )
{
	float ch = Cos( y );
	float sh = Sin( y );
	float cb = Cos( x );
	float sb = Sin( x );
	float ca = Cos( z );
	float sa = Sin( z );

	return Matrix3x3(
		ch*ca,		sh*sb - ch*sa*cb,		ch*sa*sb + sh*cb,
		sa,			ca*cb,					-ca*sb,
		-sh*ca,		sh*sa*cb + ch*sb,		-sh*sa*sb + ch*cb );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::MakeXRotation( float angleInRad )
{
	return Matrix3x3(
		1.0f,				0.0f,				 0.0f,
		0.0f,				Cos( angleInRad ),	-Sin( angleInRad ),
		0.0f,				Sin( angleInRad ),	 Cos( angleInRad ) );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::MakeYRotation( float angleInRad )
{
	return Matrix3x3(
		 Cos( angleInRad ),	0.0f,				Sin( angleInRad ),
		 0.0f,				1.0f,				0.0f,
		-Sin( angleInRad ),	0.0f,				Cos( angleInRad ) );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::MakeZRotation( float angleInRad )
{
	return Matrix3x3( 
		Cos( angleInRad ),	-Sin( angleInRad ),	0.0f,
		Sin( angleInRad ),	 Cos( angleInRad ), 0.0f,
		0.0f,				 0.0f,				1.0f );

}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::MakeRotation( Vector3 axis, float angle )
{
	return Matrix3x3(
	 1 + ( 1 - Cos( angle ) ) * ( axis.X * axis.X - 1.0f ),
	 -axis.Z * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.X * axis.Y,
	 axis.Y * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.X * axis.Z,

	 axis.Z * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.X * axis.Y,
	 1 + ( 1.0f - Cos( angle ) ) * ( axis.Y * axis.Y - 1.0f ),
	 -axis.X * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.Y * axis.Z,

	 -axis.Y * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.X * axis.Z,
	 axis.X * Sin( angle ) + ( 1.0f - Cos( angle ) ) * axis.Y * axis.Z,
	 1.0f + ( 1.0f - Cos( angle ) ) * ( axis.Z * axis.Z - 1.0f ) );
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::Transpose::get()
{
	return Matrix3x3( M11, M21, M31, 
				      M12, M22, M32, 
				      M13, M23, M33 );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix3x3::XAxis::get()
{ 
    return Vector3( M11, M12, M13 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix3x3::XAxis::set( Vector3 v )
{ 
    M11 = v.X; M12 = v.Y; M13 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix3x3::YAxis::get()
{ 
    return Vector3( M21, M22, M23 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix3x3::YAxis::set( Vector3 v )
{ 
    M21 = v.X; M22 = v.Y; M23 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix3x3::ZAxis::get()
{ 
    return Vector3( M31, M32, M33 ); 
}

//----------------------------------------------------------
void 
Bootstrap::Matrix3x3::ZAxis::set( Vector3 v )
{ 
    M31 = v.X; M32 = v.Y; M33 = v.Z; 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix3x3::EulerAngles::get()
{
	float x, y, z;
	ToEulerYXZ( y, x, z );
	return Vector3( x, y, z );
}

//----------------------------------------------------------
void 
Bootstrap::Matrix3x3::EulerAngles::set( Vector3 radians )
{
	*this = FromEulerYXZ( radians.Y, radians.X, radians.Z );
}

//----------------------------------------------------------
bool 
Bootstrap::Matrix3x3::Inverse( [Out] Matrix3x3% output )
{
	MMat3x3 inv( this->ToMMat3x3() );
	bool success = inv.Inverse();
	output = Matrix3x3( inv );
	return success;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Matrix3x3::RotatePoint( Vector3 pt )
{
return Vector3( M11 * pt.X + M12 * pt.Y + M13 * pt.Z,
			    M21 * pt.X + M22 * pt.Y + M23 * pt.Z,
			    M31 * pt.X + M32 * pt.Y + M33 * pt.Z );
}

//----------------------------------------------------------
void 
Bootstrap::Matrix3x3::ToEulerYXZ( [Out] float% y, [Out] float% x, [Out] float% z )
{
	// singularity at north pole
	if ( M21 > 0.998 ) {
		y = ATan2( M13, M33 );
		z = HALF_PI;
		x = 0.0f;
		return;
	}

	// singularity at south pole
	if ( M21 < -0.998 ) {
		y = ATan2( M13, M33 );
		z = -HALF_PI;
		x = 0.0f;
		return;
	}

	// matrix to euler, normal case.
	y = ATan2( -M31, M11 );
	x = ATan2( -M23, M22 );
	z = ASin( M21 );	
}

//----------------------------------------------------------
System::String^
Bootstrap::Matrix3x3::ToString() 
{
	return "\r\nX(" + M11 + ", " + M12 + ", " + M13 + ")"
		 + "\r\nY(" + M21 + ", " + M22 + ", " + M23 + ")"
		 + "\r\nZ(" + M31 + ", " + M32 + ", " + M33 + ")";
}

//----------------------------------------------------------
Bootstrap::Matrix3x3 
Bootstrap::Matrix3x3::operator*( Matrix3x3 left, Matrix3x3 right ) 
{ 
	Matrix3x3 result;
	
	result.M11 = (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31);
	result.M12 = (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32);
	result.M13 = (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33);

	result.M21 = (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31);
	result.M22 = (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32);
	result.M23 = (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33);

	result.M31 = (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31);
	result.M32 = (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32);
	result.M33 = (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33);

	return result;
}
