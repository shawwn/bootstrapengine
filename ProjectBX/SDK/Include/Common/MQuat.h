//----------------------------------------------------------
// File:		MQuat.h
// Author:		Kevin Bray
// Created:		04-08-06
//
// Purpose:		To contain animation data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec4.h"

// forward declarations.
class MVec3;
class MMat3x3;
class MMat4x4;

//**********************************************************
// class MQuat
//**********************************************************
class MQuat : public MVec4
{
public:
	MQuat();
	MQuat( float x, float y, float z, float w );
	MQuat( const MVec3& axis, float angle );
	MQuat( const MQuat& quat );
	explicit MQuat( const MMat3x3& rot );

	MQuat&			operator = ( const MQuat& quat );
	MQuat&			operator = ( const MMat3x3& rot );

	void			MakeRotFast( MMat3x3& rot ) const;
	MMat3x3			MakeRot() const;
	void			SetMatrixRot( MMat4x4& mat ) const;

	MQuat			operator * ( const MQuat& rhs ) const;
	MQuat&			operator *= ( const MQuat& rhs );

	// note that this assumes a quaternion is normalized.  It is really
	// calculating the conjugate.
	void			Invert();
	MQuat			Inverted() const;

	// ensure that the quaterion is normalized.
	void			Normalize();
	MQuat			Normalized() const;

	// slerps to the quat passed in.  1.0 = quat passed in, 0.0 = this quat.
	void			SlerpToFast( const MQuat& to, float t );
	MQuat			SlerpTo( const MQuat& to, float t ) const;

	// slerps from identity to this quaternion.  1.0 = this quaternion, 0.0 = identity.
	void			SlerpFromIdentFast( float t );
	MQuat			SlerpFromIdent( float t );

	// like a slerp, but doesn't take the shortest path... goes from identity to the
	// quaternion.
	void			BlendFromIdentFast( float t );

private:
	void			MakeFromMat3x3( const MMat3x3& rot );
};
