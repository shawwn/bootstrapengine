//----------------------------------------------------------
// File:		MOBox.cpp
// Author:		Kevin Bray
// Created:		03-04-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "MOBox.h"

// project includes.
#include "MVec3.h"

//**********************************************************
// class MOBox
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MOBox::MOBox()
{

}

//----------------------------------------------------------
MOBox::MOBox( const MAABox& aaBox )
: _halfExts( aaBox.GetHalfExts() )
{

}

//----------------------------------------------------------
MOBox::MOBox( const MMat4x4& transform, const MAABox& aaBox )
: _transform( transform * MMat4x4( aaBox.GetCenter() ) )
, _halfExts( aaBox.GetHalfExts() )
{

}

//----------------------------------------------------------
MOBox::MOBox( const MMat4x4& transform, const MVec3& halfExts )
: _transform( transform )
, _halfExts( halfExts )
{
	
}

//----------------------------------------------------------
MOBox::~MOBox()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MOBox::GetCorners( MVec3* corners ) const
{
	// this could probably be sped up.
	MVec3 center;
	MVec3 pxAxis;
	MVec3 pyAxis;
	MVec3 pzAxis;
	_transform.GetTranslate( center );
	_transform.GetAxis( pxAxis, 0 );
	_transform.GetAxis( pyAxis, 1 );
	_transform.GetAxis( pzAxis, 2 );

	pxAxis *= _halfExts.GetX();
	pyAxis *= _halfExts.GetY();
	pzAxis *= _halfExts.GetZ();
	MVec3 nxAxis = -pxAxis;
	MVec3 nyAxis = -pyAxis;
	MVec3 nzAxis = -pzAxis;

	corners[ 0 ] = center + nxAxis + nyAxis + nzAxis;
	corners[ 1 ] = center + nxAxis + nyAxis + pzAxis;
	corners[ 2 ] = center + nxAxis + pyAxis + nzAxis;
	corners[ 3 ] = center + nxAxis + pyAxis + pzAxis;
	corners[ 4 ] = center + pxAxis + nyAxis + nzAxis;
	corners[ 5 ] = center + pxAxis + nyAxis + pzAxis;
	corners[ 6 ] = center + pxAxis + pyAxis + nzAxis;
	corners[ 7 ] = center + pxAxis + pyAxis + pzAxis;
}
