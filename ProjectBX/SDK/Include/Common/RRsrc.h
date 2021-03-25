//----------------------------------------------------------
// File:		RRsrc.h
// Author:		Kevin Bray
// Created:		09-19-04
//
// Purpose:		Provide an interface for resources.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "URefCountMT.h"
#include "UPath.h"

//**********************************************************
// class RRsrc
//**********************************************************
class RRsrc : public URefCountMT
{
public:
	RRsrc( const tstring& name );
	RRsrc( const UPath& name );
	virtual ~RRsrc();

	const tstring&	GetRsrcName() const		{ return _rsrcName.GetPathString(); }
	const UPath&	GetRsrcPath() const		{ return _rsrcName;					}

private:
	UPath			_rsrcName;
};
