//----------------------------------------------------------
// File:		RRsrc.cpp
// Author:		Kevin Bray
// Created:		09-19-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"
#include "RRsrc.h"

//**********************************************************
// class RRsrc
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
RRsrc::RRsrc( const tstring& name )
: _rsrcName( name )
{

}

//----------------------------------------------------------
RRsrc::RRsrc( const UPath& name )
: _rsrcName( name )
{

}

//----------------------------------------------------------
RRsrc::~RRsrc()
{

}
