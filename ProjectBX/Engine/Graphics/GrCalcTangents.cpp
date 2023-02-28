//----------------------------------------------------------
// File:		GrCalcTangents.cpp
// Author:		Kevin Bray
// Created:		12-01-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrCalcTangents.h"

// project includes.
#include "GrTypes.h"
#include "MMat4x4.h"
#include "MVec3.h"

//**********************************************************
// global functions
//**********************************************************

//----------------------------------------------------------
void GrCalcTangents( SVec3& tangent, SVec3& binormal, const SVec3& p0, const SVec3& p1,
					 const SVec3& p2, const SVec2& tc0, const SVec2& tc1, const SVec2& tc2,
					 bool normalize )
{
	SVec3 dp0 = p1 - p0;
	SVec3 dp1 = p2 - p0;
	SVec2 dtc0 = tc1 - tc0;
	SVec2 dtc1 = tc2 - tc0;

	B_ASSERT( !ApproxEqual( dtc0.Mag(), 0.0f, FLT_EPSILON ) );
	B_ASSERT( !ApproxEqual( dtc1.Mag(), 0.0f, FLT_EPSILON ) );

	// calculate the new tangent and binormal.
	float r = 1.0f / ( dtc0.x * dtc1.y - dtc1.x * dtc0.y );
	tangent = r * ( dtc1.y * dp0 - dtc0.y * dp1 );
	binormal = r * ( dtc0.x * dp1 - dtc1.x * dp0 );
	float tMag = tangent.Mag();
	float bMag = binormal.Mag();

	// normalize if necessary.
	if ( normalize )
	{
		tangent.Normalize();
		binormal.Normalize();
	}
}

//----------------------------------------------------------
void GrCalcLocalToTextureXForm( MMat4x4& matrix, const SVec3& tangent, const SVec3& binormal,
								const SVec3& normal, const SVec3& p0, const SVec2& tc0 )
{
	// determine the inverse scales for the tangent and binormal.
	float invTangentScale = 1.0f / tangent.Mag();
	float invBinormalScale = 1.0f / binormal.Mag();

	// create tangents that contain the inverse scales so that we can
	// go into texture space at the proper scale.
	SVec3 scaledTangent = invTangentScale * invTangentScale * tangent;
	SVec3 scaledBinormal = invBinormalScale * invBinormalScale * binormal;

	// create a rotation that maps us into texture space as X = U,
	// Y = V, and Z = W.
	matrix = MMat4x4();
	matrix.SetRow( 0, MVec4( scaledTangent, 0.0f ) );
	matrix.SetRow( 1, MVec4( scaledBinormal, 0.0f ) );
	matrix.SetRow( 2, MVec4( normal, 0.0f ) );

	// now determine the translation.
	MVec3 uvPos = matrix.TransformCoord( p0 );
	SVec2 uvRef( uvPos.GetX(), uvPos.GetY() );
	SVec2 uvTranslate = tc0 - uvRef;
	matrix( 0, 3 ) = uvTranslate.x;
	matrix( 1, 3 ) = uvTranslate.y;
}
